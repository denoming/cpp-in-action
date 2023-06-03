#include <gtest/gtest.h>

#include <list>
#include <future>
#include <thread>
#include <syncstream>
#include <iostream>

using namespace std::literals;

//--------------------------------------------------------------------------------------------------

static void
fn1()
{
    std::cout << "Thread 1" << std::endl;
    std::this_thread::sleep_for(1s);
}

class Functor {
public:
    void
    operator()() const
    {
        std::cout << "Thread 2" << std::endl;
        std::this_thread::sleep_for(1s);
    }
};

TEST(Thread, StartThread)
{
    // Using external function
    std::thread t1{&fn1};

    // Using callable functor
    std::thread t2{Functor{}};

    // Using lambda
    std::thread t3{[]() {
        std::cout << "Thread 3" << std::endl;
        std::this_thread::sleep_for(1s);
    }};

    t1.join();
    t2.join();
    t3.join();
}

//--------------------------------------------------------------------------------------------------

static void
fn2()
{
    std::cout << "(J)Thread 2" << std::endl;
    std::this_thread::sleep_for(1s);
}

static void
fn3(std::stop_token /*stoken*/)
{
    std::cout << "(J)Thread 3" << std::endl;
    std::this_thread::sleep_for(1s);
}

TEST(Thread, StartImprovedThread)
{
    std::jthread t2{&fn2};
    std::jthread t3{&fn3};
}

//--------------------------------------------------------------------------------------------------

TEST(Thread, InterruptionByToken)
{
    std::jthread t1{[]() {
        std::osyncstream scout{std::cout};
        int cnt{};
        while (cnt++ < 10) {
            std::this_thread::sleep_for(0.2s);
            scout << "t1: " << cnt << std::endl;
        }
    }};

    std::jthread t2{[](std::stop_token stoken) {
        std::osyncstream scout{std::cout};
        int cnt{};
        while (cnt++ < 10) {
            std::this_thread::sleep_for(0.2s);
            if (stoken.stop_requested()) {
                break;
            }
            scout << "t2: " << cnt << std::endl;
        }
    }};

    std::this_thread::sleep_for(1s);

    std::osyncstream scout{std::cout};
    scout << "Interrupt both threads" << std::endl;
    t1.request_stop();
    t2.request_stop();
}

//--------------------------------------------------------------------------------------------------

using Handler = std::function<void()>;
using Callback = std::stop_callback<Handler>;

static void
fn4(std::stop_token stoken, const std::string& name)
{
    std::list<Callback> callbacks;
    for (int i = 0; i < 5; ++i) {
        /* Registers callback (each of them will be invoked)*/
        callbacks.emplace_back(
            stoken, [=]() { std::cout << "Hi from " << name << " thread: (" << i << ")\n"; });
    }
    std::this_thread::sleep_for(0.1s);
    std::cout << '\n';
}

TEST(Thread, StopCallback)
{
    std::jthread t1{&fn4, "T1"};
    std::jthread t2{&fn4, "T2"};
    std::this_thread::sleep_for(0.2s);
    /* Requests to stop (each registered callback will be invoked) */
    t1.request_stop(), t2.request_stop();
}

//--------------------------------------------------------------------------------------------------

static void
fn5(std::stop_token stoken, const std::string& name)
{
    std::osyncstream scout{std::cout};
    while (true) {
        scout << "Tick: " << name << std::endl;
        if (stoken.stop_requested()) {
            scout << "Exit: " << name << std::endl;
            break;
        }
        std::this_thread::sleep_for(0.1s);
    }
}

TEST(Thread, CooperativeInteraption)
{
    std::stop_source ss;

    std::jthread t1{&fn5, ss.get_token() /* Provide stop token */, "T1"};
    std::jthread t2{&fn5, ss.get_token() /* Provide stop token */, "T2"};

    auto f1 = std::async(std::launch::async, [stoken = ss.get_token() /* Provide stop token */]() {
        std::osyncstream scout{std::cout};
        while (true) {
            scout << "Tick: (async)" << std::endl;
            if (stoken.stop_requested()) {
                scout << "Exit: (async) " << std::endl;
                break;
            }
            std::this_thread::sleep_for(0.1s);
        }
    });

    std::this_thread::sleep_for(1s);

    /* Request to stop T1, T2 and async call */
    ss.request_stop();
}

//--------------------------------------------------------------------------------------------------

static std::atomic<bool> dataReady{false};
static std::mutex guard;
static std::condition_variable_any cv;

static void
consumer(std::stop_token stoken)
{
    while (!stoken.stop_requested()) {
        /* Here we might be notified by stop token either */
        std::unique_lock lock{guard};
        bool rv = cv.wait(lock, stoken, [&]() { return dataReady.load(); });
        if (rv) {
            std::cout << "Receive data\n";
            dataReady.store(false);
        } else {
            std::cout << "Stop has been requested\n";
        }
        std::this_thread::sleep_for(0.2s);
    }
}

static void
producer(std::stop_token stoken)
{
    while (!stoken.stop_requested()) {
        {
            std::lock_guard lock{guard};
            std::cout << "Send data\n";
            dataReady.store(true);
        }
        cv.notify_one();
        std::this_thread::sleep_for(0.5s);
    }
}

TEST(Thread, InteraptionByCondition)
{
    std::stop_source ss;

    std::jthread t1{&consumer, ss.get_token()};
    std::jthread t2{&producer, ss.get_token()};

    /* Give some time to work */
    std::this_thread::sleep_for(3s);

    /* Stop both threads */
    ss.request_stop();
}
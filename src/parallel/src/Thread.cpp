#include <gtest/gtest.h>

#include <random>
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
            continue;
        }
        if (stoken.stop_requested()) {
            std::cout << "Stop has been requested\n";
            continue;
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
    std::this_thread::sleep_for(1s);

    /* Stop both threads */
    ss.request_stop();
}

//--------------------------------------------------------------------------------------------------

TEST(Thread, UsingAsync)
{
    namespace krn = std::chrono;

    auto begin = krn::system_clock::now();

    auto asyncLazy = std::async(std::launch::deferred, []() { return krn::system_clock::now(); });
    auto asyncEager = std::async(std::launch::async, []() { return krn::system_clock::now(); });

    std::this_thread::sleep_for(krn::seconds{1});

    auto lazyDur = krn::duration<double>(asyncLazy.get() - begin).count();
    auto eagerDur = krn::duration<double>(asyncEager.get() - begin).count();

    // clang-format off
    std::cout << std::fixed << std::setprecision(10)
              << "asyncLazy evaluated after: " << lazyDur << '\n'
              << "asyncEager evaluated after: " << eagerDur << '\n';
    // clang-format on
}

//--------------------------------------------------------------------------------------------------

TEST(Thread, UsingFireAndForgetAsync)
{
    std::async(std::launch::async, []() {
        std::this_thread::sleep_for(1s);
        std::cout << "(1) done\n";
    });

    std::async(std::launch::async, []() {
        std::this_thread::sleep_for(1s);
        std::cout << "(2) done\n";
    });

    std::cout << "We are here\n";
}

//--------------------------------------------------------------------------------------------------

static long long
getDotProduct(std::vector<int>& v, std::vector<int>& w)
{
    const auto size = v.size();
    auto f1 = std::async(std::launch::async,
                         [&]() { return std::inner_product(&v[0], &v[size / 4], &w[0], 0LL); });
    auto f2 = std::async(std::launch::async, [&]() {
        return std::inner_product(&v[size / 4], &v[size / 2], &w[size / 4], 0LL);
    });
    auto f3 = std::async(std::launch::async, [&]() {
        return std::inner_product(&v[size / 2], &v[size * 3 / 4], &w[size / 2], 0LL);
    });
    auto f4 = std::async(std::launch::async, [&]() {
        return std::inner_product(&v[size * 3 / 4], &v[size], &w[size * 3 / 4], 0LL);
    });
    return f1.get() + f2.get() + f3.get() + f4.get();
}

TEST(Thread, AsyncDistribution)
{
    std::random_device seed;
    std::mt19937 engine{seed()};
    std::uniform_int_distribution<int> provider{0, 1000};

    std::vector<int> v, w;
    v.reserve(1000), w.reserve(1000);
    for (int i = 0; i < 1000; ++i) {
        v.push_back(provider(engine));
        w.push_back(provider(engine));
    }

    std::cout << "getDotProduct(v, w) = " << getDotProduct(v, w) << "\n";
}
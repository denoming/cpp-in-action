#include <boost/asio.hpp>

#include <gtest/gtest.h>

#include <memory>
#include <iostream>
#include <thread>
#include <future>
#include <semaphore>
#include <syncstream>

namespace asio = boost::asio;

using namespace testing;

template<std::ptrdiff_t Capacity = std::numeric_limits<int>::max()>
class ThreadPool {
public:
    ThreadPool()
        : _threadsNum{std::thread::hardware_concurrency() * 2U}
        , _queue{Capacity}
    {
        reset();
    }

    explicit ThreadPool(std::size_t threadsNum)
        : _threadsNum{threadsNum}
        , _queue{Capacity}
    {
        reset();
    }

    [[nodiscard]] bool
    ready() const
    {
        return _ready.test();
    }

    template<typename Fn>
    void
    post(Fn&& fn)
    {
        waitForReady();

        _queue.acquire();
        asio::post([this, fn = std::forward<Fn>(fn)]() {
            fn();
            _queue.release();
        });
    }

    template<typename Fn>
    [[nodiscard]] auto
    submit(Fn&& fn)
    {
        waitForReady();

        std::promise<decltype(fn())> promise;
        auto future = promise.get_future();
        _queue.acquire();
        asio::post(*_workers,
                   [this, promise = std::move(promise), fn = std::forward<Fn>(fn)]() mutable {
                       promise.set_value(fn());
                       _queue.release();
                   });
        return future;
    }

    void
    wait()
    {
        _workers->wait();
    }

    void
    terminate()
    {
        _workers->stop();
        _workers->wait();
    }

    void
    reset(const bool finalize = true)
    {
        resetReady();

        if (_workers) {
            if (finalize) {
                wait();
            } else {
                terminate();
            }
            _workers.reset();
        }
        _workers = std::make_unique<asio::thread_pool>(_threadsNum);

        setReady();
    }

private:
    void
    waitForReady()
    {
        _ready.wait(false);
    }

    void
    setReady()
    {
        _ready.test_and_set();
        _ready.notify_all();
    }

    void
    resetReady()
    {
        _ready.clear();
    }

private:
    std::atomic_flag _ready;
    std::size_t _threadsNum;
    std::counting_semaphore<Capacity> _queue;
    std::unique_ptr<asio::thread_pool> _workers;
};

TEST(ThreadPool, SubmitTaskWithResult)
{
    using namespace std::literals;

    ThreadPool workers{8u};

    auto result = workers.submit([]() {
        std::size_t sum{2U}, count{3U};
        while (--count) {
            sum *= sum;
            std::this_thread::sleep_for(25ms);
        }
        return sum;
    });

    EXPECT_EQ(result.get(), 16);
}

TEST(ThreadPool, SubmitToFixedPool)
{
    using namespace std::literals;

    ThreadPool<3> workers{8u};
    for (std::size_t n{0u}; n < 16u; ++n) {
        workers.post([n = n + 1]() {
            std::osyncstream{std::cout} << "Hello from " << n << " thread\n";
            std::this_thread::sleep_for(n * 100ms);
        });
        std::osyncstream{std::cout} << ">>> Push Next\n";
    }
}

TEST(ThreadPool, Reset)
{
    using namespace std::literals;

    ThreadPool workers{8u};
    for (std::size_t n{0u}; n < 8u; ++n) {
        workers.post([n = n + 1]() {
            std::osyncstream{std::cout} << "Hello from " << n << " thread\n";
            std::this_thread::sleep_for(n * 100ms);
        });
    }

    std::osyncstream{std::cout} << "Start waiting\n";
    workers.reset();
    std::osyncstream{std::cout} << "Stop waiting\n";

    for (std::size_t n{8u}; n < 16u; ++n) {
        workers.post([n = n + 1]() {
            std::osyncstream{std::cout} << "Hello from " << n << " thread\n";
            std::this_thread::sleep_for(n * 100ms);
        });
    }
}
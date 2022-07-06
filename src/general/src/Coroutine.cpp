#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <coroutine>
#include <optional>
#include <iostream>

/**
 The Promise Workflow:
 {
    promise_type promise;
    co_await promise.initial_suspend();
    try {
        <function body having co_return, co_yield, or co_await>
    }
    catch (...) {
        promise.unhandled_exception();
    }
    FinalSuspend:
        co_await promise.final_suspend();
    }
 }
*/

/**
 The Awaiter Workflow (The 'co_await expr' call)
 {
    awaitable.await_ready() returns false:
     <suspend coroutine>
     awaitable.await_suspend(coroutineHandle) returns:
         void:
             awaitable.await_suspend(coroutineHandle);
             <coroutine keeps suspended>
             <return to caller>
         bool:
             bool result = awaitable.await_suspend(coroutineHandle);
             if result:
                 <coroutine keep suspended>
                 <return to caller>
             else:
                 <go to resumptionPoint>
         another coroutine handle:
             auto anotherCoroutineHandle = awaitable.await_suspend(coroutineHandle);
             anotherCoroutineHandle.resume();
             <return to caller>
    resumptionPoint:
    return awaitable.await_resume();
 }
*/

//--------------------------------------------------------------------------------------------------

template<typename T>
struct Generator {
    // Define promise type
    struct promise_type;
    using CoroutineHandle = std::coroutine_handle<promise_type>;

    // Default ctor must be defined
    Generator() = default;

    // Disallow to copy a return object
    Generator(const Generator&) = delete;
    Generator&
    operator=(const Generator&)
        = delete;

    Generator(Generator&& other) noexcept
        : _handle{other._handle}
    {
        other._handle = {};
    }

    Generator&
    operator=(Generator&& other) noexcept
    {
        if (this != &other) {
            if (_handle) {
                _handle.destroy();
            }
            _handle = other._handle;
            other._handle = {};
        }
        return *this;
    }

    // Creates return object from promise handle
    explicit Generator(CoroutineHandle handle)
        : _handle{handle}
    {
    }

    ~Generator()
    {
        if (_handle) {
            _handle.destroy();
        }
    }

    [[nodiscard]] T
    value() const noexcept
    {
        return _handle.promise().value();
    }

    bool
    next()
    {
        _handle.resume();
        return !_handle.done();
    }

    struct promise_type {
        promise_type() = default;
        ~promise_type() = default;

        auto
        initial_suspend() noexcept
        {
            return std::suspend_never{};
        };

        auto
        final_suspend() noexcept
        {
            return std::suspend_always{};
        }

        auto
        get_return_object()
        {
            return Generator{CoroutineHandle::from_promise(*this)};
        }

        auto
        yield_value(T value) noexcept
        {
            _value = std::move(value);
            return std::suspend_always{};
        }

        void
        unhandled_exception()
        {
            throw;
        }

        [[nodiscard]] const T&
        value() const
        {
            return *_value;
        }

    private:
        std::optional<T> _value;
    };

    class Iter {
    public:
        explicit Iter(CoroutineHandle handle)
            : _handle{handle}
        {
        }

        Iter&
        operator++()
        {
            if (!_handle.done()) {
                _handle.resume();
            }
            return *this;
        }

        bool
        operator==(std::default_sentinel_t) const
        {
            return !_handle || _handle.done();
        }

        const T&
        operator*() const
        {
            return _handle.promise().value();
        }

    private:
        CoroutineHandle _handle;
    };

    Iter
    begin()
    {
        return Iter{_handle};
    }

    std::default_sentinel_t
    end()
    {
        return {};
    }

private:
    CoroutineHandle _handle;
};

Generator<int>
range(int begin, int end, int step = 1)
{
    while (begin < end) {
        co_yield begin;
        begin += step;
    }
}

TEST(Coroutine, Generator)
{
    for (auto value : range(0, 10)) {
        std::cout << value << '\n';
    }
}

//--------------------------------------------------------------------------------------------------

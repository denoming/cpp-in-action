// Copyright 2025 Denys Asauliak
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.


#include <gtest/gtest.h>

#include <coroutine>
#include <optional>
#include <iostream>
#include <thread>
#include <syncstream>

/**
 Coroutine is associated with three parts:
  - promise object
  - coroutine handle
  - coroutine frame

 Promise oObject interface:
  - <default constructor>
  - initial_suspend() - determines if the coroutine suspends before it runs
  - final_suspend() - determines if the coroutine suspends before it ends
  - unhandled_exception() - called when an exception happens
  - get_return_object() - return the coroutine object
  - return_value() - is invoked by co_return val
  - return_void() - is invoked by co_return
  - yield_value(value) - is invoked by co_yield

 Awaitable object interface:
  - await_ready(): bool - indicates id the result is ready (if not, await_suspend is called)
  - await_suspend(coroutine_handle<>) - schedule the coroutine for resumption of destruction
  - await_resume() - provides the result for the "co_await exp" expression

 The Promise Workflow:
 (using co_yield, co_await or co_return in a function transforms the its body to following lines)
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

- coroutine begins execution
  * allocates the coroutine frame (if necessary)
  * copies all function parameters to the frame
  * creates the promise object
  * creates coroutine handle and keeps it in a local variable (return to the caller on first
suspend)
  * calls promise.initial_suspend() and co_await its result
  * run the body when co_await promise.initial_suspend() resumes
- coroutine reaches a suspension point
  * returns the return object (promise.get_return_object()) to the caller which resumed the
coroutine
- coroutine reaches co_return
  * calls promise.return_void() for co_return/co_return expr where expr has void type
  * calls promise.return_value(expr) for co_return expr, where expr has non-void type
  * destroys all stack-created variables
  * calls promise.final_suspend and co_await its result
- the coroutine is destroyed (via co_return or by uncaught exception or via coroutine handle)
  * calls the destruction of the promise object
  * calls the destructor of the function parameters
  * frees the memory used by the coroutine frame
  * transfers control back to the caller

 The Awaiter Workflow
 (the 'co_await expr' call causes the compiler to start inner awaitable workflow)
 {
     awaitable.await_ready() returns
        true:
        <coroutine is ready, returns with the result of the call to awaiter.await_resume()>
        false:
        <suspend coroutine and evaluate awaiter.await_suspend() return value>
     if awaitable.await_suspend(coroutineHandle) returns:
         void:
             awaitable.await_suspend(coroutineHandle);
             <coroutine keeps suspended>
             <return to caller>
         bool:
             bool result = awaitable.await_suspend(coroutineHandle);
             if result is true:
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

 On exception depend on where it is thrown:
    - in await_ready: the coroutine is not suspended, nor are the calls await_suspended or
await_resume evaluated
    - await_suspend: the exception is caught, the coroutine is resumed and the exception is rethrown
      and await_resume is not called
    - await_resume: await_ready and await_suspend are evaluated and all values are returned

 */

//--------------------------------------------------------------------------------------------------

/**
 * Define return object type
 */
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
        return not _handle.done();
    }

    struct promise_type {
        promise_type() = default;
        ~promise_type() = default;

        auto
        initial_suspend() noexcept
        {
            /* not lazy behaviour (call function body once since creation) */
            return std::suspend_never{};
        };

        auto
        final_suspend() noexcept
        {
            /* lazy behaviour */
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
finiteRange(int begin, int end, int step = 1)
{
    while (begin < end) {
        co_yield begin;
        begin += step;
    }
}

Generator<int>
infiniteRange(int begin, int step = 1)
{
    while (true) {
        co_yield begin;
        begin += step;
    }
}

TEST(Coroutine, Generator)
{
    std::cout << "Finite generator: ";
    for (auto value : finiteRange(0, 10)) {
        std::cout << value << ' ';
    }
    std::cout << '\n';

    std::cout << "Infinite generator: ";
    auto g = infiniteRange(0);
    for (int n = 0; n < 10; ++n) {
        g.next();
        std::cout << g.value() << ' ';
    }
    std::cout << '\n';
}

//--------------------------------------------------------------------------------------------------

struct Job {
    struct promise_type;
    using handle_t = std::coroutine_handle<promise_type>;

    explicit Job(handle_t handle)
        : _handle{handle}
    {
    }

    ~Job()
    {
        if (_handle) {
            _handle.destroy();
        }
    }

    void
    start()
    {
        _handle.resume();
    }

    struct promise_type {
        auto
        get_return_object()
        {
            return Job{handle_t::from_promise(*this)};
        }

        std::suspend_always
        initial_suspend()
        {
            std::cout << "    Preparing job" << '\n';
            return {};
        }

        std::suspend_always
        final_suspend() noexcept
        {
            std::cout << "    Performing job" << '\n';
            return {};
        }

        void
        return_void()
        {
        }

        void
        unhandled_exception()
        {
        }
    };

private:
    handle_t _handle;
};

static Job
prepareJob()
{
    /**
     * (1) After call initial_suspend() is called and coroutine is suspended, return object is given
     *     to caller.
     * (2) The start() is called, coroutine is resumed, due to body contains "never suspend" clause
     *     the coroutine is going to be finished
     * (3) The final_suspend() is called and the coroutine is suspended
     */
    co_await std::suspend_never{};
}

TEST(Coroutine, Job)
{
    std::cout << "Before job" << '\n';
    auto job = prepareJob();
    job.start();
    std::cout << "After job" << '\n';
}

//--------------------------------------------------------------------------------------------------

static void
log(const char* line)
{
    std::osyncstream{std::cout} << std::this_thread::get_id() << ": " << line;
}

class Event {
public:
    Event() = default;

    Event(const Event&) = delete;
    Event&
    operator=(const Event&)
        = delete;

    class Awaiter;
    Awaiter operator co_await() const noexcept;

    void
    notify() noexcept;

private:
    friend class Awaiter;
    mutable std::atomic<void*> _waiter{nullptr};
    mutable std::atomic<bool> _notified{false};
};

class Event::Awaiter {
public:
    explicit Awaiter(const Event& event)
        : _event{event}
    {
        log("Awaiter::Awaiter()\n");
    }

    [[nodiscard]] bool
    await_ready() const
    {
        log("Awaiter()::await_ready()\n");

        if (_event._waiter.load() != nullptr) {
            throw std::runtime_error{"More than one waiter is not valid"};
        }

        return _event._notified;
    }

    bool
    await_suspend(std::coroutine_handle<> handle) noexcept
    {
        log("Awaiter()::await_suspend(handle)\n");

        _handle = handle;

        _event._waiter.store(this);
        if (_event._notified) {
            void* self = this;
            if (_event._waiter.compare_exchange_strong(self, nullptr)) {
                /* return to resumption point (call await_resume() on awaiter object) */
                return false;
            }
        }

        /* coroutine is keep suspended, return to the caller */
        return true;
    }

    void
    await_resume() noexcept
    {
        log("Awaiter()::await_resume()\n");
    }

private:
    friend class Event;
    const Event& _event;
    std::coroutine_handle<> _handle;
};

void
Event::notify() noexcept
{
    log("Event()::notify()\n");

    _notified = true;

    void* waiter = _waiter.load();
    if (waiter != nullptr and _waiter.compare_exchange_strong(waiter, nullptr)) {
        static_cast<Awaiter*>(waiter)->_handle.resume();
    }
}

Event::Awaiter Event::operator co_await() const noexcept
{
    /* Creates awaiter object (Awaiter) from awaitable (Event) */
    return Awaiter(*this);
}

struct Task {
    struct promise_type {
        Task
        get_return_object()
        {
            log("promise_type()::get_return_object()\n");
            return {};
        }

        std::suspend_never
        initial_suspend()
        {
            log("promise_type()::initial_suspend()\n");
            return {};
        }

        std::suspend_never
        final_suspend() noexcept
        {
            log("promise_type()::final_suspend()\n");
            return {};
        }

        void
        return_void()
        {
        }

        void
        unhandled_exception()
        {
        }
    };
};

static Task
receiver(Event& event)
{
    log("receiver(): start\n");

    auto start = std::chrono::high_resolution_clock::now();
    co_await event;
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::string output{"receiver(): end"};
    output += " (";
    output += std::to_string(elapsed.count());
    output += " sec)\n";
    log(output.data());
}

static void
sender(Event& event)
{
    log("sender()\n");
    event.notify();
}

TEST(Coroutine, Event)
{
    using namespace std::literals;

    {
        Event event;
        std::jthread t1{&receiver, std::ref(event)};
        std::jthread t2{[&]() { event.notify(); }};
    }

    {
        Event event;
        std::jthread t1{&receiver, std::ref(event)};
        std::jthread t2{[&]() {
            std::this_thread::sleep_for(1s);
            event.notify();
        }};
    }
}

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
#include <gmock/gmock.h>

#include <memory>
#include <queue>
#include <thread>
#include <iostream>

using namespace testing;
using namespace std::literals;

/**
 * The activation list based on FIFO lock-free queue
 * @tparam T - the activation list items type
 */
template<typename T>
class ActivationList {
    template<typename U>
    using Aligned = typename std::aligned_storage<sizeof(U), std::alignment_of<U>::value>::type;

public:
    struct Linked {
        std::atomic<T*> next;
    };

    ActivationList()
        : _head{reinterpret_cast<T*>(new Aligned<T>)}
        , _tail{_head.load(std::memory_order_relaxed)}
        , _size{0}
        , _lastFront{nullptr}
        , _prevFront{_tail.load(std::memory_order_relaxed)}
    {
        _prevFront->next.store(nullptr, std::memory_order_relaxed);
    }

    ~ActivationList()
    {
        clear();
        ::operator delete(_head.load(std::memory_order_relaxed));
    }

    void
    clear()
    {
        while (front()) {
            pop_front();
        }
    }

    template<typename Linkable>
    std::size_t
    push_back(Linkable* e)
    {
        e->next.store(nullptr, std::memory_order_relaxed);
        T* back = _head.exchange(e, std::memory_order_acq_rel);
        back->next.store(e, std::memory_order_release);
        return _size.fetch_add(1, std::memory_order_release);
    }

    inline T*
    front()
    {
        return _lastFront = _prevFront->next.load(std::memory_order_acquire);
    }

    void
    pop_front()
    {
        _size.fetch_sub(1, std::memory_order_release);
        _tail.store(_lastFront, std::memory_order_release);
        _lastFront->~T();
        ::operator delete(_prevFront);
        _prevFront = _lastFront;
    }

    [[nodiscard]] inline std::size_t
    size() const
    {
        return _size.load(std::memory_order_acquire);
    }

    [[nodiscard]] inline bool
    empty() const
    {
        return !size();
    }

private:
    std::atomic<std::size_t> _size;
    std::atomic<T*> _head;
    std::atomic<T*> _tail;
    T* _lastFront;
    T* _prevFront;
};

/**
 * The Active Object (also known as Concurrent Object) design pattern decouples member function
 * execution from member function invocation for objects that each reside in their thread of control.
 * The goal is to introduce concurrency by using asynchronous member function invocation and
 * a scheduler for handling requests.
 *
 * Advantages:
 *  - Synchronization is only required on the Active Object’s thread but not on the client’s threads.
 *  - The synchronization challenges are on the implementer’s side.
 *  - Enhanced throughput of the system because of the asynchronous execution of the requests.
 *  - The scheduler can implement various strategies to execute the pending requests.
 *
 * Disadvantages:
 *  - If the requests are too fine-grained, the Active Object pattern’s performance overhead such
 *    as the proxy, the activation list, and the scheduler may be excessive.
 *  - Due to the scheduler’s scheduling strategy and the operating system’s scheduling, debugging
 *    the Active Object pattern is often quite tricky.
 *
 * Typical components (not applicable in this example):
 *  - Proxy - provides an interface for the accessible member functions on the Active Object.
 *    The proxy triggers the construction of a request which goes into the activation list.
 *    The proxy runs in the client thread.
 *  - MethodRequest - class defines the interface for the method executing on an Active Object.
 *    This interface also contains guard methods, indicating if the job is ready to run.
 *    The request includes all context information to be executed later.
 *  - ActivationList - maintains the pending requests. The activation list decouples the client’s
 *    thread from the Active Object thread. The proxy inserts the request object,
 *    and the scheduler removes them. Consequently, the access onto the activation list
 *    must be serialized.
 *  - Scheduler - runs in the thread of the Active Object and decides which request from
 *    the activation list is executed next. The scheduler evaluates the guards of the request
 *    to determine if the request can run.
 *  - Servant - implements the Active Object and runs in the active object’s thread.
 *    The servant implements the interface of the method request, and the scheduler invokes
 *    its member functions.
 *  - Future - is created by the proxy and is only necessary if the request returns a result.
 *    Therefore, the client receives the future and can obtain the result of the method invocation
 *    on the Active Object. The client can wait for the outcome or poll for it.
 *
 * Typical dynamic Behavior (not applicable in this example):
 * 1. Request construction and scheduling: The client invokes a method on the proxy. The proxy
 *    creates a request and passes it to the scheduler. The scheduler enqueues the request
 *    on the activation list. Additionally, the proxy returns a future to the client if the request
 *    returns a result.
 * 2. Member function execution: The scheduler determines which request becomes runnable
 *    by evaluating the guard method of the request. It removes the request from the activation list
 *    and dispatches it to the servant.
 * 3. Completion: If the request returns something, it will be stored in the future. The client can
 *    ask for the result. When the client has the result, the request and the future can be deleted.
 */
template<typename Runnable>
class ActiveObject {
public:
    using Ptr = std::shared_ptr<ActiveObject>;

    /**
     * Creates an active object and dispatch in dedicated thread
     * @tparam Args - the types of custom arguments
     * @param args - the custom arguments
     * @return the pointer to the
     */
    template<typename... Args>
    [[nodiscard]] static Ptr
    create(Args&&... args)
    {
        auto task = Ptr(new Runnable{std::forward<Args>(args)...}, [](Runnable* runnable) {
            if (runnable->stop(true)) {
                delete runnable;
            }
        });
        task->spawn();
        return task;
    }

    /**
     * Creates an active object and dispatch in current thread (can be invoked from main() function)
     * @tparam Args - the types of custom arguments
     * @param args - the custom arguments
     */
    template<typename... Args>
    static void
    run(Args&&... args)
    {
        struct RunTask : public Runnable {
            explicit RunTask(Args&&... args)
                : Runnable(std::forward<Args>(args)...)
            {
            }
        };
        auto task = std::make_shared<Runnable>(std::forward<Args>(args)...);
        task->dispatch();
    }

    [[nodiscard]] bool
    exiting() const
    {
        return !_dispatching;
    }

    /**
     * Stop active object
     */
    void
    stop()
    {
        stop(false);
    }

    /**
     * Invokes method with given data and schedule for executing in active object's thread.
     * @tparam Data - the custom method request data type
     * @param data - the custom method request data
     */
    template<typename Data>
    inline void
    invoke(Data data)
    {
        post<MethodRequest<Data>>(std::move(data));
    }

protected:
    struct IRequest : ActivationList<IRequest>::Linked {
        virtual ~IRequest() = default;

        virtual void
        deliverTo(Runnable* instance)
            = 0;
    };

    template<typename Data>
    struct MethodRequest : public IRequest {
        explicit MethodRequest(Data&& data)
            : data{std::move(data)}
        {
        }

        void
        deliverTo(Runnable* runnable) override
        {
            runnable->onMethod(data);
        }

        Data data;
    };

protected:
    ActiveObject()
        : _dispatching{true}
        , _detached{false}
    {
    }

    virtual ~ActiveObject() = default;

    /**
     * Invoked when started
     */
    virtual void
    onStart()
    {
    }

    /**
     * Invoked when stopped
     */
    virtual void
    onStop()
    {
    }

    /**
     * Invoked when dispatching is active
     */
    virtual void
    onDispatch()
    {
    }

private:
    [[nodiscard]] bool
    selfStop() const
    {
        return (_thread.get_id() == std::this_thread::get_id());
    }

    bool
    stop(bool force)
    {
        if (selfStop()) {
            if (!_dispatching) {
                /* Was already stopped and deleted (see custom deleter) */
                return false;
            }
            if (force) {
                if (_thread.joinable()) {
                    _thread.detach();
                    _detached = true;
                }
                _dispatching = false;
            }
            return false;
        } else {
            if (!_dispatching) {
                /* Was already stopped */
                return true;
            }
            _dispatching = false;
            if (_thread.joinable()) {
                _thread.join();
            }
            return true;
        }
    }

    /**
     * Dispatches active object request
     */
    void
    dispatch()
    {
        auto* self = static_cast<Runnable*>(this);
        self->onStart();
        while (_dispatching) {
            schedule();
            self->onDispatch();
        }
        self->onStop();
        if (_detached) {
            delete self;
        }
    }

    /**
     * Schedules executing in active object thread
     */
    void
    schedule()
    {
        auto* self = static_cast<Runnable*>(this);
        while (IRequest* request = _activationList.front()) {
            request->deliverTo(self);
            _activationList.pop_front();
        }
    }

    void
    spawn()
    {
        _thread = std::thread(&ActiveObject::dispatch, this);
    }

    template<typename RequestType, typename DataType>
    void
    post(DataType&& data)
    {
        _activationList.push_back(new RequestType(std::forward<DataType>(data)));
    }

private:
    std::atomic<bool> _dispatching;
    std::atomic<bool> _detached;
    std::thread _thread;
    ActivationList<IRequest> _activationList;
};

/* The GetPicture message */
struct GetPicture {
    int width{};
    int height{};
};

/* The GetMoney message */
struct GetMoney {
    double amount{};
};

class Consumer : public ActiveObject<Consumer> {
public:
    friend ActiveObject<Consumer>;

    void
    onStart() override
    {
        std::cout << "Consumer: onStart()" << '\n';
    }

    void
    onStop() override
    {
        std::cout << "Consumer: onStop()" << '\n';
    }

    void
    onMethod(GetPicture&)
    {
        std::cout << "Consumer: onMessage(GetPicture&)" << '\n';
    }

    void
    onMethod(GetMoney&)
    {
        std::cout << "Consumer: onMessage(GetMoney&)" << '\n';
    }
};

TEST(ActiveObject, Test)
{
    auto consumer = Consumer::create();
    EXPECT_FALSE(consumer->exiting());

    std::this_thread::sleep_for(100ms);

    consumer->invoke(GetPicture{.width = 0, .height = 0});
    consumer->invoke(GetMoney{.amount = 105.4});

    std::this_thread::sleep_for(100ms);

    consumer->stop();
    EXPECT_TRUE(consumer->exiting());
}
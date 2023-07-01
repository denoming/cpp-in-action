#include <gtest/gtest.h>

#include <mutex>
#include <shared_mutex>
#include <concepts>

using namespace testing;

template<typename T>
concept BasicLockable = requires(T lockable) {
    lockable.lock();
    lockable.unlock();
};

/* The interface of strategy */
class Lock {
public:
    virtual void
    lock() const
        = 0;

    virtual void
    unlock() const
        = 0;
};

class NullObjectMutex {
public:
    void
    lock()
    {
    }

    void
    unlock()
    {
    }
};

/* The run-time strategized locking */
class RunTimeStrategizedLocking {
public:
    explicit RunTimeStrategizedLocking(Lock& lock)
        : _lock{lock}
    {
        _lock.lock();
    }

    ~RunTimeStrategizedLocking()
    {
        _lock.unlock();
    }

private:
    Lock& _lock;
};

/* The compile-time strategized locking */
template<BasicLockable Lock>
class CompileTimeStrategizedLocking {
public:
    explicit CompileTimeStrategizedLocking(Lock& lock)
        : _lock{lock}
    {
        _lock.lock();
    }

    ~CompileTimeStrategizedLocking()
    {
        _lock.unlock();
    }

public:
    Lock& _lock;
};

/* NoLock strategy */
class NoLock : public Lock {
public:
    void
    lock() const override
    {
        _mutex.lock();
    }

    void
    unlock() const override
    {
        _mutex.unlock();
    }

private:
    mutable NullObjectMutex _mutex;
};

/* ExclusiveLock strategy */
class ExclusiveLock : public Lock {
public:
    void
    lock() const override
    {
        _mutex.lock();
    }

    void
    unlock() const override
    {
        _mutex.unlock();
    }

private:
    mutable std::mutex _mutex;
};

/* SharedLock strategy */
class SharedLock : public Lock {
public:
    void
    lock() const override
    {
        _mutex.lock_shared();
    }

    void
    unlock() const override
    {
        _mutex.unlock_shared();
    }

private:
    mutable std::shared_mutex _mutex;
};

TEST(StrategizedLocking, RunTime)
{
    {
        NoLock lock;
        RunTimeStrategizedLocking ctx{lock};
    }

    {
        ExclusiveLock lock;
        RunTimeStrategizedLocking ctx{lock};
    }

    {
        SharedLock lock;
        RunTimeStrategizedLocking ctx{lock};
    }
}

TEST(StrategizedLocking, CompileTime)
{
    {
        NoLock lock;
        CompileTimeStrategizedLocking<NoLock> ctx{lock};
    }

    {
        ExclusiveLock lock;
        CompileTimeStrategizedLocking<ExclusiveLock> ctx{lock};
    }

    {
        SharedLock lock;
        CompileTimeStrategizedLocking<SharedLock> ctx{lock};
    }
}
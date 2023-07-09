#pragma once

#include <memory>

#ifdef _WIN32
using Handle = intptr_t;
#else
using Handle = int;
#endif

/**
 * The event handler interface
 */
class EventHandler {
public:
    using Ptr = std::shared_ptr<EventHandler>;

    virtual void
    process(Handle handle, short what)
        = 0;
};

/**
 * The user event class.
 * (needed to manipulate user events: signal and remove handling)
 */
class UserEvent {
public:
    [[nodiscard]] bool
    valid() const
    {
        return not _handle.expired();
    }

private:
    friend class Reactor;
    explicit UserEvent(const std::shared_ptr<void>& handle)
        : _handle{handle}
    {
    }

    template<typename T>
    std::shared_ptr<T>
    lock()
    {
        return std::static_pointer_cast<T>(_handle.lock());
    }

private:
    std::weak_ptr<void> _handle;
};

/**
 * The reactor class.
 */
class Reactor {
public:
    Reactor();

    ~Reactor();

    [[nodiscard]] bool
    active() const;

    void
    signal(UserEvent event);

    [[nodiscard]] UserEvent
    registerHandler(std::shared_ptr<EventHandler> handler);

    void
    registerHandler(Handle handle, short what, std::shared_ptr<EventHandler> handler);

    void
    removeHandler(UserEvent event);

    void
    removeHandler(Handle handle);

    void
    run();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
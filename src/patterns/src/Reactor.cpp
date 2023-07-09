#include "Reactor.hpp"

#include <event2/event.h>
#include <event2/thread.h>

#include <boost/assert.hpp>

#include <thread>
#include <list>
#include <map>

using namespace std::literals;

/* Specific handle for user event */
static constexpr Handle UserEventHandle = -1;

//--------------------------------------------------------------------------------------------------

class Demuxer {
public:
    Demuxer()
    {
        if (evthread_use_pthreads() != 0) {
            throw std::runtime_error{"Unable to activate thread supporting"};
        }

        if (_base = event_base_new(); _base == nullptr) {
            throw std::runtime_error{"Unable to create event base"};
        }
    }

    ~Demuxer()
    {
        if (_base != nullptr) {
            event_base_free(_base);
        }
    }

    event_base*
    base()
    {
        return _base;
    }

    [[maybe_unused]] bool
    select()
    {
        BOOST_ASSERT(_base != nullptr);
        return (event_base_loop(_base, EVLOOP_NO_EXIT_ON_EMPTY) == 0);
    }

    void
    exit()
    {
        BOOST_ASSERT(_base != nullptr);
        event_base_loopbreak(_base);
    }

private:
    event_base* _base{};
};

//--------------------------------------------------------------------------------------------------

class DispatchTable {
public:
    struct Entry {
        using Ptr = std::shared_ptr<Entry>;
        event* evt{};
        std::shared_ptr<EventHandler> handler;
    };

    [[maybe_unused]] Entry::Ptr
    put(event* const event, std::shared_ptr<EventHandler> handler)
    {
        auto entry = std::make_shared<Entry>();
        entry->evt = event;
        entry->handler = std::move(handler);

        auto handleIt = _entries.insert(_entries.cend(), entry);
        _dispatchMap1.insert({event, handleIt});
        if (Handle handle = event_get_fd(event); handle != UserEventHandle) {
            _dispatchMap2.insert({handle, handleIt});
        }
        return entry;
    }

    [[nodiscard]] Entry::Ptr
    get(event* const event) const
    {
        if (auto it = _dispatchMap1.find(event); it != _dispatchMap1.end()) {
            auto [_, handleIt] = *it;
            return *handleIt;
        }
        return {};
    }

    [[nodiscard]] Entry::Ptr
    get(Handle handle) const
    {
        if (auto it = _dispatchMap2.find(handle); it != _dispatchMap2.end()) {
            auto [_, handleIt] = *it;
            return *handleIt;
        }
        return {};
    }

    void
    remove(event* const ev)
    {
        if (auto it = _dispatchMap1.find(ev); it != _dispatchMap1.end()) {
            auto [_, handleIt] = *it;
            _entries.erase(handleIt);
            _dispatchMap1.erase(it);
        }
    }

    void
    remove(Handle handle)
    {
        if (auto it = _dispatchMap2.find(handle); it != _dispatchMap2.end()) {
            auto [_, handleIt] = *it;
            _entries.erase(handleIt);
            _dispatchMap2.erase(it);
        }
    }

private:
    using Entries = std::list<Entry::Ptr>;
    using DispatchMap1 = std::map<event*, Entries::iterator>;
    using DispatchMap2 = std::map<Handle, Entries::iterator>;

    Entries _entries;
    DispatchMap1 _dispatchMap1;
    DispatchMap2 _dispatchMap2;
};

//--------------------------------------------------------------------------------------------------

class Reactor::Impl {
public:
    static thread_local Impl* self;

    ~Impl()
    {
        _demuxer.exit();
        if (_thread.joinable()) {
            _thread.join();
        }
    }

    [[nodiscard]] bool
    active() const
    {
        return _thread.joinable();
    }

    void
    signal(UserEvent event) const
    {
        if (active()) {
            if (auto entry = event.lock<DispatchTable::Entry>(); entry) {
                event_active(entry->evt, 0, 0);
            }
        }
    }

    [[nodiscard]] UserEvent
    registerHandler(std::shared_ptr<EventHandler> handler)
    {
        void* arg = event_self_cbarg();
        auto* event = event_new(_demuxer.base(), UserEventHandle, 0, &eventCallback, arg);
        BOOST_ASSERT(event != nullptr);
        auto entry = _table.put(event, std::move(handler));
        return UserEvent{entry};
    }

    void
    registerHandler(Handle handle, short what, std::shared_ptr<EventHandler> handler)
    {
        auto* ev = event_new(_demuxer.base(), handle, what, &eventCallback, event_self_cbarg());
        BOOST_ASSERT(ev != nullptr);
        _table.put(ev, std::move(handler));
    }

    void
    removeHandler(UserEvent event)
    {
        if (auto entry = event.lock<DispatchTable::Entry>(); entry) {
            _table.remove(entry->evt);
            event_del(entry->evt);
            event_free(entry->evt);
        }
    }

    void
    removeHandler(Handle handle)
    {
        auto entry = _table.get(handle);
        event_del(entry->evt);
        event_free(entry->evt);
        _table.remove(handle);
    }

    void
    run()
    {
        if (active()) {
            throw std::runtime_error{"Reactor is activated already"};
        }
        _thread = std::thread{&Impl::select, this};
    }

private:
    void
    select()
    {
        self = this;
        _demuxer.select();
        self = nullptr;
    }

    void
    dispatchEvent(Handle handle, short what, event* object)
    {
        if (auto entry = _table.get(object); handle) {
            auto handler = entry->handler;
            BOOST_ASSERT(handler);
            handler->process(handle, what);
        }
    }

    static void
    eventCallback(Handle handle, short what, void* arg)
    {
        BOOST_ASSERT(arg != nullptr);
        auto* evt = static_cast<event*>(arg);
        BOOST_ASSERT(self != nullptr);
        self->dispatchEvent(handle, what, evt);
    }

private:
    std::thread _thread;
    Demuxer _demuxer;
    DispatchTable _table;
};

thread_local Reactor::Impl* Reactor::Impl::self = nullptr;

Reactor::Reactor()
    : _impl{std::make_unique<Impl>()}
{
}

Reactor::~Reactor()
{
    BOOST_ASSERT(_impl);
    _impl.reset();
}

bool
Reactor::active() const
{
    BOOST_ASSERT(_impl);
    return _impl->active();
}

void
Reactor::signal(UserEvent event)
{
    BOOST_ASSERT(_impl);
    _impl->signal(std::move(event));
}

UserEvent
Reactor::registerHandler(std::shared_ptr<EventHandler> handler)
{
    BOOST_ASSERT(_impl);
    return _impl->registerHandler(std::move(handler));
}

void
Reactor::registerHandler(Handle handle, short what, std::shared_ptr<EventHandler> handler)
{
    BOOST_ASSERT(_impl);
    return _impl->registerHandler(handle, what, std::move(handler));
}

void
Reactor::removeHandler(UserEvent event)
{
    BOOST_ASSERT(_impl);
    _impl->removeHandler(std::move(event));
}

void
Reactor::removeHandler(Handle handle)
{
    BOOST_ASSERT(_impl);
    _impl->removeHandler(handle);
}

void
Reactor::run()
{
    BOOST_ASSERT(_impl);
    _impl->run();
}

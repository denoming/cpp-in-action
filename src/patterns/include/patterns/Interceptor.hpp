#pragma once

#include <string>

class Event {
public:
};

class Context {
public:
    /**
     * Updates framework state
     */
    void
    setValue(int)
    {
    }

    /**
     * Returns some framework state
     */
    int
    getValue()
    {
        return {};
    }
};

class Framework {
public:
};

class Interceptor {
public:
    virtual ~Interceptor() = default;

    virtual void
    onEvent1(Context& context)
        = 0;

    virtual void
    onEvent2(Context& context)
        = 0;
};

class Dispatcher {
public:
    /**
     * Attach interceptor
     */
    void
    attach(Interceptor* interceptor)
    {
    }

    /**
     * Detach interceptor
     */
    void
    detach(Interceptor* interceptor)
    {
    }

    /**
     * Dispatches incoming events and call particular interceptors
     */
    void
    dispatch(Event event)
    {
    }
};

class ConcreteInterceptor final : public Interceptor {
public:
    /**
     * Handles event1 and bring new behaviour to exists implementation
     */
    void
    onEvent1(Context& context) final
    {
    }

    /**
     * Handles event1 and bring new behaviour to exists implementation
     */
    void
    onEvent2(Context& context) final
    {
    }
};

class Application {
public:
    Application()
        : _dispatcher{new Dispatcher}
    {
    }

    [[noreturn]] void
    main()
    {
        ConcreteInterceptor* interceptor = new ConcreteInterceptor;
        _dispatcher->attach(interceptor);

        // ... set-up application ... //

        // Pass events to dispatcher(s)
        while (true) {
            _dispatcher->dispatch({});
        }

        // ... tear down application ... //
    }

private:
    Dispatcher* _dispatcher{};
};
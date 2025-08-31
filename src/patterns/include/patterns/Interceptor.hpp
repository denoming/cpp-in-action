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
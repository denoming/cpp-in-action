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

#include <stdexcept>
#include <string>

/**
 * Root interface (with core functionality)
 */
class IRoot {
public:
    /**
     * Returns extension interface or throw exception if it doesn't supported
     */
    IRoot&
    getExtension(unsigned id)
    {
        throw std::runtime_error{"Not supported"};
    }
};

class INotificationSink {
public:
};

class IManagerObject : public IRoot {
public:
    void
    setValue(int);

    int
    getValue();

    long
    addNotificationListener(INotificationSink sink);

    void
    removeNotificationListener(long handle);

    void
    setFilter(std::string expression);
};

/**
 * Extension interface for components to print debug information
 */
class IDump : public IRoot {
public:
    std::string
    dump();
};

/**
 * Extension interface for components to render
 */
class IRender : public IRoot {
public:
    void
    render();
};

/**
 * Component specific extension interface
 */
class IPort : public IRoot {
public:
    void
    setHost(std::string host);

    std::string
    getHost();

    void
    setPort(unsigned port);

    unsigned
    getPort();
};
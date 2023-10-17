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
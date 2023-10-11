#pragma once

#include <string>
#include <filesystem>

class Component {
public:
    virtual void
    init(int argc, char* argv[])
        = 0;

    virtual void
    fini()
        = 0;

    virtual void
    suspend()
        = 0;

    virtual void
    resume()
        = 0;

    virtual void
    info()
        = 0;
};

class ComponentRepository {
public:
    /**
     * Inserts newly configured component
     */
    void
    insert(std::string name, std::unique_ptr<Component> component)
    {
    }

    /**
     * Finds component by given name
     */
    Component*
    find(const std::string& name)
    {
    }

    /**
     * Removes terminated component
     */
    void
    remove(const std::string& name)
    {
    }

    /**
     * Suspends component temporary
     */
    void
    suspend(const std::string& name)
    {
    }

    /**
     * Resumes component
     */
    void
    resume(const std::string& name)
    {
    }
};

class ComponentConfigurator {
public:
    /**
     * Process configuration file
     */
    void
    process(const std::filesystem::path& configFile)
    {
    }

    /**
     * Process configuration directive (line by line)
     */
    void
    process(const std::string& directive)
    {
    }

    /**
     * Returns link to a component repository
     */
    ComponentRepository* repository()
    {
        return nullptr;
    }
};
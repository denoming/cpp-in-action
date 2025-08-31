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
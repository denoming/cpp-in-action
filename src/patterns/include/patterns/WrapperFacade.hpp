#pragma once

/**
 * The Wrapper Facade design pattern encapsulates the functions and data provided by existing
 * non-object-oriented APIs within more concise, robust, portable, maintainable, and cohesive
 * object-oriented class interfaces.
 *
 * Pros:
 *  - Concise, cohesive and robust higher-level object-oriented programming interfaces
 *  - Portability and maintainability
 *  - Modularity, re-usability and configurability
 *
 * Cons:
 *  - Loss of functionality (an 'escape-hatch' mechanism can be used)
 *  - Performance degradation (additional indirection)
 *  - Programming language and compiler limitations (define interfaces across different languages)
 *
 * Reference:
 *   "Pattern-Oriented Software Architecture, Volume 2, Chapter 2"
 *   By Douglas C. Schmidt, Michael Stal, Hans Rohnert, Frank Buschmann
 */
class WrapperFacade {
public:
    WrapperFacade()
    {
        // Construct RawHandle
        // (the RAII approach)
    }

    ~WrapperFacade()
    {
        // Destroy RawHandle
        // (the RAII approach)
    }

    void
    method1()
    {
        // functionA() # Call C API functionA
    }

    void
    method2()
    {
        // functionA() # Call C API functionA
        // functionB() # Call C API functionB
    }

private:
    // RawHandle handle;
};
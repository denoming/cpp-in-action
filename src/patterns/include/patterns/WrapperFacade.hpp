#pragma once

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
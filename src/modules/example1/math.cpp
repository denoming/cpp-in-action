module;

#include <cstddef>

export module math;

/* Export a concrete function */
export size_t
add(size_t arg1, size_t arg2)
{
    return (arg1 + arg2);
}

/* Export group of functions */
export
{
    size_t mul(size_t arg1, size_t arg2)
    {
        return (arg1 * arg2);
    }
}

/* Export particular namespace */
export namespace math {

double
div(double arg1, double arg2)
{
    return (arg1 / arg2);
}

} // namespace math
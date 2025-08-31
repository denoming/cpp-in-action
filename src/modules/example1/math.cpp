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
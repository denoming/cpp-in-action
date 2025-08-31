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

#include <chrono>

class Timer {
public:
    Timer()
        : _point{std::chrono::steady_clock::now()}
    {
    }

    uint64_t
    diff(bool isReset = false)
    {
        auto now{std::chrono::steady_clock::now()};
        std::chrono::duration<double, std::milli> diff{now - _point};
        if (isReset) {
            reset();
        }
        return diff.count();
    }

    void
    reset()
    {
        _point = std::chrono::steady_clock::now();
    }

private:
    std::chrono::steady_clock::time_point _point;
};

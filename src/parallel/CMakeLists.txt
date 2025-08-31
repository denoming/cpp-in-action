# Copyright 2025 Denys Asauliak
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set(TARGET "${PROJECT_NAME}-parallel")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    src/ParallelAlgorithms.cpp
    src/Atomic.cpp
    src/Fences.cpp
    src/Thread.cpp
    src/ThreadLocal.cpp
    src/Lock.cpp
    src/Initialization.cpp
    src/Conditional.cpp
    src/Semaphore.cpp
    src/Latch.cpp
    src/Barrier.cpp
    src/DiningPhilosophersProblem.cpp
    src/LockFreeStack.cpp
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
    Threads::Threads
    GTest::gtest_main
    GTest::gmock_main
    TBB::tbb
)

if (NOT CMAKE_CROSSCOMPILING)
    gtest_discover_tests(${TARGET})
endif()

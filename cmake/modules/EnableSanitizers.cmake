# Distributed under the MIT License. See accompanying file LICENSE.md

#[=======================================================================[.rst:
EnableSanitizers
-------

Enable address and thread sanitizers. First checking of compiler to support particular compiler and linker flags
is performed. If sanitizers are supported by compiler dedicated cmake target are created. Link with address of
thread sanitizer cmake target to enable sanitizing. Linking with both sanitizer target is not supported.

Custom Targets
^^^^^^^^^^^^^^^^

AddressSanitizer - dedicated cmake target to enable address sanitizing for target that is linked with.
ThreadSanitizer - dedicated cmake target to enable thread sanitizing for target that is linked with.

#]=======================================================================]

include(CheckCompilerFlag)
include(CMakePushCheckState)

option(ENABLE_ADDRESS_SANITIZER "Enable address sanitizer"
       OFF)
option(ENABLE_THREAD_SANITIZER "Enable thread sanitizer"
       OFF)

cmake_push_check_state(RESET)
set(CMAKE_REQUIRED_FLAGS "-O2")
set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
check_compiler_flag(CXX "-fsanitize=address" SUPPORT_SANITIZE_ADDRESS)
cmake_pop_check_state()

cmake_push_check_state(RESET)
set(CMAKE_REQUIRED_FLAGS "-O2")
set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=thread")
check_compiler_flag(CXX "-fsanitize=thread" SUPPORT_SANITIZE_THREAD)
cmake_pop_check_state()

if (ENABLE_ADDRESS_SANITIZER)
    if (SUPPORT_SANITIZE_ADDRESS AND NOT TARGET AddressSanitizer)
        set(_TARGET_NAME AddressSanitizer)
        add_library(${_TARGET_NAME} INTERFACE)
        target_compile_options(${_TARGET_NAME} INTERFACE "-fsanitize=address")
        target_link_options(${_TARGET_NAME} INTERFACE "-fsanitize=address")
        unset(_TARGET_NAME)
    endif ()
endif ()

if (ENABLE_THREAD_SANITIZER)
    if (SUPPORT_SANITIZE_THREAD AND NOT TARGET ThreadSanitizer)
        set(_TARGET_NAME ThreadSanitizer)
        add_library(${_TARGET_NAME} INTERFACE)
        target_compile_options(${_TARGET_NAME} INTERFACE "-fsanitize=thread")
        target_link_options(${_TARGET_NAME} INTERFACE "-fsanitize=thread")
        unset(_TARGET_NAME)
    endif ()
endif ()

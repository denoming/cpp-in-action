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

find_package(PkgConfig QUIET)
pkg_check_modules(PC_LIBEVENT QUIET libevent)

find_path(LIBEVENT_INCLUDE_DIR
    NAMES
        event2/event-config.h
    HINTS
        ${PC_LIBEVENT_INCLUDE_DIRS}
)

set(_LIBEVENT_REQUIRED_VARS)
foreach(COMPONENT ${LibEvent_FIND_COMPONENTS})
    set(_LIBEVENT_LIBNAME libevent)
    if(COMPONENT STREQUAL _LIBEVENT_LIBNAME)
        set(_LIBEVENT_LIBNAME event)
    else()
        set(_LIBEVENT_LIBNAME "event_${COMPONENT}")
    endif()
    string(TOUPPER "${COMPONENT}" COMPONENT_UPPER)
    find_library(LIBEVENT_${COMPONENT_UPPER}_LIBRARY
        NAMES ${_LIBEVENT_LIBNAME}
        HINTS ${PC_LIBEVENT_LIBRARY_DIRS}
    )
    if(LIBEVENT_${COMPONENT_UPPER}_LIBRARY)
        set(LibEvent_${COMPONENT}_FOUND 1)
    endif()
    list(APPEND _LIBEVENT_REQUIRED_VARS LIBEVENT_${COMPONENT_UPPER}_LIBRARY)
endforeach()
unset(_LIBEVENT_LIBNAME)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibEvent REQUIRED_VARS
    ${_LIBEVENT_REQUIRED_VARS}
    LIBEVENT_INCLUDE_DIR
    VERSION_VAR ${PC_LIBEVENT_VERSION}
    HANDLE_COMPONENTS
)

if(LIBEVENT_FOUND)
    set(LIBEVENT_INCLUDE_DIRS ${LIBEVENT_INCLUDE_DIR})
    set(LIBEVENT_LIBRARIES)
    foreach(COMPONENT ${LibEvent_FIND_COMPONENTS})
        string(TOUPPER "${COMPONENT}" COMPONENT_UPPER)
        list(APPEND LIBEVENT_LIBRARIES ${LIBEVENT_${COMPONENT_UPPER}_LIBRARY})
        set(LIBEVENT_${COMPONENT_UPPER}_FOUND ${LibEvent_${COMPONENT}_FOUND})
    endforeach()
endif()

mark_as_advanced(LIBEVENT_INCLUDE_DIR LIBEVENT_LIBRARIES ${_LIBEVENT_REQUIRED_VARS})
unset(_LIBEVENT_REQUIRED_VARS)
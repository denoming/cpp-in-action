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

find_package(LibEvent COMPONENTS core pthreads OPTIONAL_COMPONENTS extra)

if(LIBEVENT_CORE_FOUND)
    set(_LIBEVENT_TARGET_NAME LibEvent::Core)
    add_library(${_LIBEVENT_TARGET_NAME} SHARED IMPORTED GLOBAL)
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBEVENT_INCLUDE_DIR})
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES IMPORTED_LOCATION ${LIBEVENT_CORE_LIBRARY})
    unset(_LIBEVENT_TARGET_NAME)
endif()

if(${LIBEVENT_EXTRA_FOUND})
    set(_LIBEVENT_TARGET_NAME LibEvent::Extra)
    add_library(${_LIBEVENT_TARGET_NAME} SHARED IMPORTED GLOBAL)
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBEVENT_INCLUDE_DIR})
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES IMPORTED_LOCATION ${LIBEVENT_EXTRA_LIBRARY})
    unset(_LIBEVENT_TARGET_NAME)
endif()

if(${LIBEVENT_OPENSSL_FOUND})
    set(_LIBEVENT_TARGET_NAME LibEvent::OpenSSL)
    add_library(${_LIBEVENT_TARGET_NAME} SHARED IMPORTED GLOBAL)
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBEVENT_INCLUDE_DIR})
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES IMPORTED_LOCATION ${LIBEVENT_OPENSSL_LIBRARY})
    unset(_LIBEVENT_TARGET_NAME)
endif()

if(${LIBEVENT_PTHREADS_FOUND})
    set(_LIBEVENT_TARGET_NAME LibEvent::Threads)
    add_library(${_LIBEVENT_TARGET_NAME} SHARED IMPORTED GLOBAL)
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${LIBEVENT_INCLUDE_DIR})
    set_target_properties(${_LIBEVENT_TARGET_NAME} PROPERTIES IMPORTED_LOCATION ${LIBEVENT_PTHREADS_LIBRARY})
    unset(_LIBEVENT_TARGET_NAME)
endif()

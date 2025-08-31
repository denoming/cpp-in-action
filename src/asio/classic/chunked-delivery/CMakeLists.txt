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

set(TARGET "asio-chunked-delivery")

find_package(Threads REQUIRED)

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/TcpClient.cpp
        src/TcpServer.cpp
        src/Service.cpp
)

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PUBLIC Threads::Threads
    PRIVATE Boost::headers Boost::program_options
)

target_compile_definitions(${TARGET}
    PRIVATE -DBOOST_ASIO_ENABLE_HANDLER_TRACKING
            -DBOOST_ASIO_ENABLE_BUFFER_DEBUGGING
)
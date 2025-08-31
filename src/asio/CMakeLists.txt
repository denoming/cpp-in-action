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

add_subdirectory(basic)

add_subdirectory(classic/chat-async)
add_subdirectory(classic/chunked-delivery)
add_subdirectory(classic/daytime-async)
add_subdirectory(classic/p2p-sync)
add_subdirectory(classic/tcp-async)
add_subdirectory(classic/tcp-echo)
add_subdirectory(classic/tcp-sync)
add_subdirectory(classic/udp-sync)

add_subdirectory(coro/echo-service)
add_subdirectory(coro/chunked-delivery)
add_subdirectory(coro/primitives)

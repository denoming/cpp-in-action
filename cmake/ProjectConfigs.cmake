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

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules")

if (EXISTS $ENV{HOME}/.local)
    list(APPEND CMAKE_PREFIX_PATH $ENV{HOME}/.local)
endif()

include(BuildLocation)
include(BuildType)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(ENABLE_THREAD_SANITIZER)
    include(EnableSanitizers)
endif()
if(ENABLE_PARALLEL)
    include(AddTbb)
endif()
include(AddGoogleTest)
include(AddBoost)
include(AddFmt)
include(AddLibEvent)
include(AddSpdLog)

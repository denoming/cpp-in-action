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

include(FeatureSummary)

option(ENABLE_THREAD_SANITIZER "Enable parallel examples" OFF)
add_feature_info(
    ENABLE_THREAD_SANITIZER ENABLE_THREAD_SANITIZER "Build project with parallel examples"
)

##
# Enabling parallel part of project requires installing: Intel TBB
# (e.g. for Ubuntu: $ sudo apt install libtbb-dev)
##
option(ENABLE_PARALLEL "Enable parallel examples" OFF)
add_feature_info(
    ENABLE_PARALLEL ENABLE_PARALLEL "Build project with parallel examples"
)

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

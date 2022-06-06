list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules")

if (EXISTS $ENV{HOME}/.local)
    list(APPEND CMAKE_PREFIX_PATH $ENV{HOME}/.local)
endif()

include(BuildLocation)
include(BuildType)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MCPP_ENABLE_TESTS)
    include(AddGoogleTest)
endif()

if(MCPP_ENABLE_PARALLEL)
    find_package(Threads REQUIRED)
    find_package(TBB REQUIRED)
endif()

include(AddBoost)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/modules")

include(BuildLocation)
include(BuildType)

include(CxxTestCompiler)
checkForCxx20Compiler(CXX20_COMPILER)
if(CXX20_COMPILER)
    enableCxx20()
else()
    message(FATAL_ERROR "The C++20 standard not supported")
endif()

if(MCPP_ENABLE_TESTS)
    include(AddGoogleTest)
endif()

if(MCPP_ENABLE_PARALLEL)
    find_package(Threads REQUIRED)
    find_package(TBB REQUIRED)
endif()

include(AddBoost)

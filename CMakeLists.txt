cmake_minimum_required(VERSION 3.8)

#-------------------------------------------------------------------------------
# Project Declaration
#-------------------------------------------------------------------------------

project(mcpp)

#-------------------------------------------------------------------------------
# Project Configuration
#-------------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectConfiguration.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectOptions.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectResources.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ProjectDependencies.cmake)

#-------------------------------------------------------------------------------
# Structure Configuration
#-------------------------------------------------------------------------------

add_subdirectory(src)
if(PROJECT_ENABLE_TESTS)
    add_subdirectory(test)
endif()


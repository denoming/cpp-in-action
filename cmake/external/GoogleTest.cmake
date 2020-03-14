list(APPEND CMAKE_PREFIX_PATH "~/opt/googletest")
find_package(GTest REQUIRED)

if(${GTEST_FOUND})
    message(STATUS "Using GTest ${GTEST_LIBRARIES} library")
else()
    message(FATAL_ERROR "GTest library NOT found")
endif()

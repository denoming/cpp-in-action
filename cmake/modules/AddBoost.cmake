
if(DEFINED ENV{BOOST_ROOT})
    list(APPEND CMAKE_PREFIX_PATH $ENV{BOOST_ROOT})
endif()

find_package(Boost 1.74.0 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()
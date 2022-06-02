if(DEFINED ENV{BOOST_ROOT})
    list(APPEND CMAKE_PREFIX_PATH $ENV{BOOST_ROOT})
endif()

find_package(Boost 1.79.0 REQUIRED COMPONENTS program_options)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()
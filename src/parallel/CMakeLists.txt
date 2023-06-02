set(TARGET "${PROJECT_NAME}-parallel")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    src/ParallelAlgorithms.cpp
    src/Singleton.cpp
    src/Atomic.cpp
    src/Fences.cpp
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
    GTest::gtest_main GTest::gmock_main
)

if (NOT CMAKE_CROSSCOMPILING)
    gtest_discover_tests(${TARGET})
endif()

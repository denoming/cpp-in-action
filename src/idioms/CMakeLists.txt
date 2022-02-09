set(TARGET "${PROJECT_NAME}-idioms")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    src/CopyAndSwap.cpp
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
    GTest::gtest_main GTest::gmock_main
)

if (NOT CMAKE_CROSSCOMPILING)
    gtest_discover_tests(${TARGET})
endif()

set(TARGET "asio-coro-primitives")

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/AsyncEvent.cpp
)

target_include_directories(${TARGET}
        PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PRIVATE Boost::headers
            fmt::fmt
            GTest::gtest_main
            GTest::gmock_main
            ThreadSanitizer
)

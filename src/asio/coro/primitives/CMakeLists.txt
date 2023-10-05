set(TARGET "asio-coro-primitives")

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/EventTest.cpp
        src/SequenceTraitsTest.cpp
        src/SequenceBarrierTest.cpp
)

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

if (DEBUG)
    target_compile_definitions(${TARGET}
        PRIVATE -DDEBUG
    )
endif()

target_link_libraries(${TARGET}
    PRIVATE Boost::headers
            spdlog::spdlog
            GTest::gtest_main
            GTest::gmock_main
            ThreadSanitizer
)

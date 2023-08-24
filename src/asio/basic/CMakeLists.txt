set(TARGET "asio-basic")

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/Endpoint.cpp
        src/Socket.cpp
        src/Resolver.cpp
        src/ThreadPool.cpp
        src/Context.cpp
        src/Timers.cpp
        src/Client.cpp
        src/Buffer.cpp
)

target_link_libraries(${TARGET}
    PRIVATE GTest::gtest_main
            GTest::gmock_main
)

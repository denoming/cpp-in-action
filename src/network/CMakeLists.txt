set(TARGET "${PROJECT_NAME}-network")

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
    PRIVATE
        ${PROJECT_NAME}::common
        GTest::gtest_main GTest::gmock_main
)

add_subdirectory(misc/p2p-sync-service)
add_subdirectory(misc/daytime-async-service)
add_subdirectory(misc/file-exchange-service)
add_subdirectory(misc/tcp-async-service)
add_subdirectory(misc/tcp-sync-service)
add_subdirectory(misc/udp-sync-service)
add_subdirectory(misc/chat-async-service)
add_subdirectory(misc/tcp-echo-service)
add_subdirectory(coro/echo-service)
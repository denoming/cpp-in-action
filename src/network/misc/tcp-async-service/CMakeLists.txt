set(TARGET "network-tcp-async-service")

find_package(Threads REQUIRED)

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/TcpAsyncAcceptor.cpp
        src/TcpAsyncClient.cpp
        src/TcpAsyncServer.cpp
        src/TcpAsyncService.cpp
        src/main.cpp
)

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PUBLIC Threads::Threads
    PRIVATE Boost::headers Boost::program_options
)

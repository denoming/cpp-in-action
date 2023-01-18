set(TARGET "network-tcp-sync-service")

find_package(Threads REQUIRED)

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/TcpSyncClient.cpp
        src/TcpSyncAcceptor.cpp
        src/TcpSyncService.cpp
        src/TcpSyncServer.cpp
        src/Service.cpp
)

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PUBLIC Threads::Threads
    PRIVATE Boost::headers Boost::program_options
)

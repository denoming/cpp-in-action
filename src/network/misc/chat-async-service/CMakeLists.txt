set(TARGET "chat-async-service")

find_package(Threads REQUIRED)

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/Service.cpp
        src/ChatServer.cpp
        src/ChatSession.cpp
)

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PUBLIC Threads::Threads
    PRIVATE Boost::headers
)

target_compile_definitions(${TARGET}
    PRIVATE -DBOOST_ASIO_ENABLE_HANDLER_TRACKING
            -DBOOST_ASIO_ENABLE_BUFFER_DEBUGGING
)
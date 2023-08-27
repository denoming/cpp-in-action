set(TARGET "asio-coro-chunked-delivery")

add_executable(${TARGET} "")

target_sources(${TARGET}
    PRIVATE
        src/Service.cpp
        src/Server.cpp
        src/Client.cpp
)

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PRIVATE Boost::headers
            Boost::program_options
            fmt::fmt
)

target_compile_definitions(${TARGET}
    PRIVATE -DBOOST_ASIO_ENABLE_HANDLER_TRACKING
            -DBOOST_ASIO_ENABLE_BUFFER_DEBUGGING
)
set(TARGET "${PROJECT_NAME}-idioms")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CopyAndSwap.cpp"
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
)

target_add_tests(TARGET ${TARGET})

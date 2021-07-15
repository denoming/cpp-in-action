set(TARGET "${PROJECT_NAME}-parallel")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ParallelAlgorithms.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Singleton.cpp"
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
    Threads::Threads
    TBB::tbb
)

target_add_tests(TARGET ${TARGET})

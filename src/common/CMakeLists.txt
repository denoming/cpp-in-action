set(TARGET "${PROJECT_NAME}-common")

add_library(${TARGET})
add_library("${PROJECT_NAME}::common" ALIAS ${TARGET})

target_sources(${TARGET}
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/HeapMemoryTracker.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/Timer.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/MemoryResourcesTracker.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/MemoryDumper.hpp"
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/HeapMemoryTracker.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MemoryResourcesTracker.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MemoryDump.cpp"
)

target_include_directories(${TARGET}
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set(TARGET "${PROJECT_NAME}-common")

add_library(${TARGET})
add_library(${PROJECT_NAME}::common ALIAS ${TARGET})

target_sources(${TARGET}
PUBLIC
    include/common/HeapMemoryTracker.hpp
    include/common/Timer.h
    include/common/MemoryResourcesTracker.hpp
    include/common/MemoryDumper.hpp
PRIVATE
    src/HeapMemoryTracker.cpp
    src/MemoryResourcesTracker.cpp
    src/MemoryDump.cpp
)

target_include_directories(${TARGET}
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(${TARGET}
PUBLIC
    Threads::Threads
    TBB::tbb
)
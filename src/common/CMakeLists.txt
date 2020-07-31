set(TARGET "${PROJECT_NAME}-common")

add_library(${TARGET})
add_library("${PROJECT_NAME}::common" ALIAS ${TARGET})

target_sources(${TARGET}
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/common/Timer.h"
)

target_include_directories(${TARGET}
PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(${TARGET} PROPERTIES LINKER_LANGUAGE CXX)

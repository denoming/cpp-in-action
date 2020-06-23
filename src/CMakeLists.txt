#-------------------------------------------------------------------------------

set(TARGET_NAME "Tests")

file(GLOB TARGET_SOURCE ${CMAKE_CURRENT_LIST_DIR}/general/*.cpp
                        ${CMAKE_CURRENT_LIST_DIR}/parallel/*.cpp)

#-------------------------------------------------------------------------------
# Target Configuration
#-------------------------------------------------------------------------------

add_executable(${TARGET_NAME} "")

target_sources(${TARGET_NAME}
    PRIVATE
        ${TARGET_SOURCE}
    )

target_link_libraries(${TARGET_NAME}
    PRIVATE
        GTest::gtest
        GTest::gtest_main
        GTest::gmock
        GTest::gmock_main
    )

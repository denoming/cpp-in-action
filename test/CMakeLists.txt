#-------------------------------------------------------------------------------

set(TARGET_PATH ${PROJECT_SOURCE_DIR}/test)

set(TARGET_NAME "Tests")

set(TARGET_SOURCE
        ${TARGET_PATH}/src/StructuredBindingTest.cpp
        ${TARGET_PATH}/src/EmbeddedInitTest.cpp
        ${TARGET_PATH}/src/AggreagateExtensionTest.cpp
    )

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

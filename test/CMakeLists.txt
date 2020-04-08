#-------------------------------------------------------------------------------

set(TARGET_PATH ${PROJECT_SOURCE_DIR}/test)

set(TARGET_NAME "Tests")

set(TARGET_SOURCE
        ${TARGET_PATH}/src/general/StructuredBindingTest.cpp
        ${TARGET_PATH}/src/general/EmbeddedInitTest.cpp
        ${TARGET_PATH}/src/general/AggreagateExtensionTest.cpp
        ${TARGET_PATH}/src/general/CopyElisionTest.cpp
        ${TARGET_PATH}/src/general/LambdaExtensionTest.cpp
        ${TARGET_PATH}/src/general/MinorFeaturesTest.cpp
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

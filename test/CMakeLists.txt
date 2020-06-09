#-------------------------------------------------------------------------------

set(TARGET_PATH ${PROJECT_SOURCE_DIR}/test)

set(TARGET_NAME "Tests")

set(TARGET_SOURCE
        ${TARGET_PATH}/src/general/StructuredBindingTest.cpp
        ${TARGET_PATH}/src/general/IfAndSwitchWithInitTest.cpp
        ${TARGET_PATH}/src/general/AggreagateExtensionTest.cpp
        ${TARGET_PATH}/src/general/CopyElisionTest.cpp
        ${TARGET_PATH}/src/general/LambdaExtensionTest.cpp
        ${TARGET_PATH}/src/general/MinorFeaturesTest.cpp
        ${TARGET_PATH}/src/general/TemplateClassDeduction.cpp
        ${TARGET_PATH}/src/general/DeductionGuide.cpp
        ${TARGET_PATH}/src/general/CompileTimeIf.cpp
        ${TARGET_PATH}/src/general/FoldExpression.cpp
        ${TARGET_PATH}/src/general/ExtendedUsingDeclaration.cpp
        ${TARGET_PATH}/src/general/OptionalTest.cpp
        ${TARGET_PATH}/src/general/VariantTest.cpp
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

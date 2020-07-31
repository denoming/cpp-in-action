set(TARGET "${PROJECT_NAME}-general")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/AggreagateExtensionTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CompileTimeIfTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CopyElisionTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/DeductionGuideTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ExtendedUsingDeclarationTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileSystemTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FoldExpressionTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/IfAndSwitchWithInitTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LambdaExtensionTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MinorFeaturesTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/OptionalTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/StructuredBindingTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemplateClassDeductionTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/VariantTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/StringSearchTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/UtilityFunctionsTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ContainerImprovementsTest.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/UncaughtExceptionTest.cpp"
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
)

target_add_tests(TARGET ${TARGET})

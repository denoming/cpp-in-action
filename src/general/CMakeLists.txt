set(TARGET "${PROJECT_NAME}-general")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/AggregateExtension.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CompileTimeIf.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CopyElision.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/DeductionGuide.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ExtendedUsingDeclaration.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileSystem.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FoldExpression.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/IfAndSwitchWithInit.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/LambdaExtension.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MinorFeatures.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Optional.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/StructuredBinding.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/TemplateClassDeduction.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Variant.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/StringSearch.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/UtilityFunctions.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ContainerImprovements.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/UncaughtException.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/PolymorphicMemory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/CompareFloatNumbers.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/NewDeleteOperators.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/EnumIterator.cpp"
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
)

target_add_tests(TARGET ${TARGET})

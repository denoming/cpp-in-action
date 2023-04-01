set(TARGET "${PROJECT_NAME}-general")

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    src/AggregateExtension.cpp
    src/CompileTimeIf.cpp
    src/CopyElision.cpp
    src/DeductionGuide.cpp
    src/ExtendedUsingDeclaration.cpp
    src/FileSystem.cpp
    src/FoldExpression.cpp
    src/IfSwitchRangeForWithInit.cpp
    src/LambdaExtension.cpp
    src/MinorFeatures.cpp
    src/Optional.cpp
    src/StructuredBinding.cpp
    src/TemplateClassDeduction.cpp
    src/Variant.cpp
    src/StringSearch.cpp
    src/UtilityFunctions.cpp
    src/ContainerImprovements.cpp
    src/UncaughtException.cpp
    src/PolymorphicMemory.cpp
    src/CompareFloatNumbers.cpp
    src/NewDeleteOperators.cpp
    src/EnumIterator.cpp
    src/AnyType.cpp
    src/Template.cpp
    src/TemplateMeta.cpp
    src/TemplateVariadic.cpp
    src/Coroutine.cpp
    src/ThreeWayComparison.cpp
    src/Attributes.cpp
    src/Concepts.cpp
    src/Ranges.cpp
    src/Span.cpp
    src/Bits.cpp
    src/Chrono.cpp
)

target_link_libraries(${TARGET}
PRIVATE
    ${PROJECT_NAME}::common
    GTest::gtest_main
    GTest::gmock_main
    fmt::fmt
)

if (NOT CMAKE_CROSSCOMPILING)
    gtest_discover_tests(${TARGET})
endif()
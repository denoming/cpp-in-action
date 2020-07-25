#-------------------------------------------------------------------------------

set(TARGET "Tests")

#-------------------------------------------------------------------------------
# Target Configuration
#-------------------------------------------------------------------------------

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    "general/AggreagateExtensionTest.cpp"
    "general/CompileTimeIfTest.cpp"
    "general/CopyElisionTest.cpp"
    "general/DeductionGuideTest.cpp"
    "general/ExtendedUsingDeclarationTest.cpp"
    "general/FileSystemTest.cpp"
    "general/FoldExpressionTest.cpp"
    "general/IfAndSwitchWithInitTest.cpp"
    "general/LambdaExtensionTest.cpp"
    "general/MinorFeaturesTest.cpp"
    "general/OptionalTest.cpp"
    "general/StructuredBindingTest.cpp"
    "general/TemplateClassDeductionTest.cpp"
    "general/VariantTest.cpp"
    "general/StringSearchTest.cpp"
    "general/UtilityFunctionsTest.cpp"
    "general/ContainerImprovementsTest.cpp"
    "general/UncaughtExceptionTest.cpp"
)

target_include_directories(${TARGET}
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}"
)

if(MCPP_ENABLE_PARALLEL)
    find_package(Threads REQUIRED)
    find_package(TBB REQUIRED)

    target_link_libraries(${TARGET}
    PRIVATE
        Threads::Threads
        TBB::tbb
    )

    target_sources(${TARGET}
    PRIVATE
        "parallel/ParallelAlgorithmsTest.cpp"
    )
endif()

target_add_tests(TARGET ${TARGET})

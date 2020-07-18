#-------------------------------------------------------------------------------

set(TARGET "Tests")

#-------------------------------------------------------------------------------
# Target Configuration
#-------------------------------------------------------------------------------

add_executable(${TARGET} "")

target_sources(${TARGET}
PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/general/AggreagateExtensionTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/CompileTimeIfTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/CopyElisionTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/DeductionGuideTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/ExtendedUsingDeclarationTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/FileSystemTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/FoldExpressionTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/IfAndSwitchWithInitTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/LambdaExtensionTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/MinorFeaturesTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/OptionalTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/StructuredBindingTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/TemplateClassDeductionTest.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/general/VariantTest.cpp"
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
        "${CMAKE_CURRENT_LIST_DIR}/parallel/ParallelAlgorithmsTest.cpp"
    )
endif()

target_add_tests(TARGET ${TARGET})

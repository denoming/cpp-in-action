include(FeatureSummary)

option(MCPP_ENABLE_TESTS "Enable testing" ON)
add_feature_info(
    MCPP_ENABLE_TESTS MCPP_ENABLE_TESTS "Build project with tests"
)

#
# Required to install TBB
# (e.g. for Ubuntu: sudo apt install libtbb-dev)
#
option(MCPP_ENABLE_PARALLEL "Enable parallel examples" OFF)
add_feature_info(
    MCPP_ENABLE_PARALLEL MCPP_ENABLE_PARALLEL "Build project with parallel examples"
)

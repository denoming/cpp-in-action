include(FeatureSummary)

##
# Enabling parallel part of project requires installing: Intel TBB
# (e.g. for Ubuntu: $ sudo apt install libtbb-dev)
##
option(ENABLE_PARALLEL "Enable parallel examples" OFF)
add_feature_info(
    ENABLE_PARALLEL ENABLE_PARALLEL "Build project with parallel examples"
)

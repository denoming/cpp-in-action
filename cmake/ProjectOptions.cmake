include(FeatureSummary)

option(ENABLE_THREAD_SANITIZER "Enable parallel examples" OFF)
add_feature_info(
    ENABLE_THREAD_SANITIZER ENABLE_THREAD_SANITIZER "Build project with parallel examples"
)

##
# Enabling parallel part of project requires installing: Intel TBB
# (e.g. for Ubuntu: $ sudo apt install libtbb-dev)
##
option(ENABLE_PARALLEL "Enable parallel examples" OFF)
add_feature_info(
    ENABLE_PARALLEL ENABLE_PARALLEL "Build project with parallel examples"
)

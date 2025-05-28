cmake_minimum_required(VERSION 3.18)

project(figcone_tree VERSION 2.2.0 DESCRIPTION "C++17 interface library for creating configuration parsers compatible with figcone")
include(external/seal_lake)

SealLake_v040_Bundle(
        NAME figcone_tree_eel
        GIT_REPOSITORY https://github.com/kamchatka-volcano/eel.git
        GIT_TAG        v0.3.0
        DIRECTORIES
            include/eel
        DESTINATION
            include/figcone_tree/detail/external
        TEXT_REPLACEMENTS
            "namespace eel" "namespace figcone::tree::eel"
            "EEL_" "FIGCONE_TREE_EEL_"
)

SealLake_v040_HeaderOnlyLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
)

cmake_minimum_required(VERSION 3.18)

project(figcone_tree VERSION 1.0.0 DESCRIPTION "C++17 interface library for creating configuration parsers compatible with figcone")
include(GNUInstallDirs)
include(external/seal_lake)

SealLake_Bundle(
        NAME figcone_tree_sfun
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        v3.0.0
        DIRECTORIES
            include/sfun
        DESTINATION
            include/figcone_tree/detail/external
        TEXT_REPLACEMENTS
            "namespace sfun" "namespace figcone::tree::sfun"
            "SFUN_" "FIGCONE_TREE_SFUN_"
)

SealLake_HeaderOnlyLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
        INCLUDES
            figcone_tree/detail/external
)

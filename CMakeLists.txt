cmake_minimum_required(VERSION 3.18)

project(figcone_tree VERSION 0.11.0 DESCRIPTION "C++17 interface library for creating configuration parsers compatible with figcone")
include(GNUInstallDirs)
include(external/seal_lake)

SealLake_Download(
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun.git
        GIT_TAG        include_guards
        DIRECTORIES
            include/sfun
        DESTINATION include/figcone_tree/detail/external
)

SealLake_HeaderOnlyLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
        INCLUDES
            figcone_tree/detail/external
)

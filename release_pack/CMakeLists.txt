cmake_minimum_required(VERSION 3.18)

project(figcone_tree VERSION 2.2.0 DESCRIPTION "C++17 interface library for creating configuration parsers compatible with figcone")
include(seal_lake.cmake)

SealLake_v040_HeaderOnlyLibrary(
        NAMESPACE figcone
        COMPILE_FEATURES cxx_std_17
)

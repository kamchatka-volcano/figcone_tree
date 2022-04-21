cmake_minimum_required(VERSION 3.18)

project(figcone_tree VERSION 0.9.0 DESCRIPTION "figcone library interface")

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(FIGCONE_TREE_SUBPROJECT OFF)
else()
    set(FIGCONE_TREE_SUBPROJECT ON)
endif()

add_library(figcone_tree INTERFACE)
add_library(figcone::figcone_tree ALIAS figcone_tree)

target_compile_features(figcone_tree INTERFACE cxx_std_17)
target_include_directories(
        figcone_tree
        INTERFACE
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
set(PUBLIC_HEADERS
        "include/figcone_tree/errors.h"
        "include/figcone_tree/iparser.h"
        "include/figcone_tree/streamposition.h"
        "include/figcone_tree/tree.h")

set_target_properties(figcone_tree PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

option(INSTALL_FIGCONE_TREE "Install figcone_tree library unconditionally" OFF)
if (NOT FIGCONE_TREE_SUBPROJECT OR INSTALL_FIGCONE_TREE)
    include(GNUInstallDirs)

    set(FIGCONE_TREE_PACK_PATH ${CMAKE_INSTALL_LIBDIR}/cmake/figcone_tree)
    install(TARGETS figcone_tree
            EXPORT figcone_tree-targets
            PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/figcone_tree)
    install(EXPORT figcone_tree-targets
            FILE figcone_treeTargets.cmake
            NAMESPACE figcone::
            DESTINATION ${FIGCONE_TREE_PACK_PATH})

    include(CMakePackageConfigHelpers)
    write_basic_package_version_file(
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_treeConfigVersion.cmake
            COMPATIBILITY SameMajorVersion
            ARCH_INDEPENDENT
    )

    configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/figcone_treeConfig.cmake.in
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_treeConfig.cmake
            INSTALL_DESTINATION ${FIGCONE_TREE_PACK_PATH}
    )

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_treeConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/figcone_treeConfigVersion.cmake
            DESTINATION ${FIGCONE_TREE_PACK_PATH}
    )
endif()

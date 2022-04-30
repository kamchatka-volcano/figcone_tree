**figcone_tree** - is a C++17 header-only library providing an interface for creating configuration parsers compatible with [`figcone`](https://github.com/kamchatka-volcano/figcone) library. 


## Installation
Download and link the library from your project's CMakeLists.txt:
```
cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(figcone_tree
    GIT_REPOSITORY "https://github.com/kamchatka-volcano/figcone_tree.git"
    GIT_TAG "origin/master"
)
#uncomment if you need to install figcone_tree with your target
#set(INSTALL_FIGCONE_TREE ON)
FetchContent_MakeAvailable(figcone_tree)

add_executable(${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE figcone::figcone_tree)
```

For the system-wide installation use these commands:
```
git clone https://github.com/kamchatka-volcano/figcone_tree.git
cd figcone_tree
cmake -S . -B build
cmake --build build
cmake --install build
```

Afterwards, you can use find_package() command to make the installed library available inside your project:
```
find_package(figcone_tree 0.9.0 REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE figcone::figcone_tree)   
```

## License
**figcone_tree** is licensed under the [MS-PL license](/LICENSE.md)  

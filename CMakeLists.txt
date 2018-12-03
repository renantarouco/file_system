cmake_minimum_required(VERSION 3.12)
project(file_system VERSION 0.0.0 LANGUAGES CXX)
add_compile_options(-std=c++11)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(
    lib/nlohmann_json
)

include_directories(
    include
    lib/nlohmann_json/single_include
)

add_library(
    fs_models
    src/fs_models/fat_model.cpp
    src/fs_models/inode_model.cpp
)

add_library(
    fs_driver
    src/fs_driver/fs_driver.cpp
)

target_link_libraries(
    fs_driver
    fs_models
)

add_executable(
    file_system
    src/main.cpp
)

target_link_libraries(
    file_system
    fs_driver
    nlohmann_json::nlohmann_json
)
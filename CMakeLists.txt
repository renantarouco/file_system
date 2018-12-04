cmake_minimum_required(VERSION 3.12)
project(file_system VERSION 0.0.0 LANGUAGES CXX)
add_compile_options(-std=c++11)

include_directories(
    include
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
    terminal
    src/cli/terminal.cpp
)

add_executable(
    format_tool
    src/cli/format_tool.cpp
)
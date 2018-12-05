cmake_minimum_required(VERSION 3.10)
project(file_system)
add_compile_options(-std=c++11)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(lib/nlohmann_json)

include_directories(
    include
    lib/nlohmann_json/single_include
)

# LIBRARIES

add_library(
    sec_mem_driver
    src/sec_mem_driver/sec_mem_driver.cpp
)

add_library(
    block_manager
    src/block_manager/block_manager.cpp
)

add_library(
    file_systems
    src/file_systems/file_system.cpp
    src/file_systems/fat_system.cpp
    src/file_systems/inode_system.cpp
)

# LIBRARIES - LINKS

target_link_libraries(
    file_systems
    sec_mem_driver
    block_manager
    nlohmann_json::nlohmann_json
)

#EXECUTABLES

add_executable(
    terminal
    src/cli/terminal.cpp
)

add_executable(
    format_tool
    src/cli/format_tool.cpp
)

#EXECUTABLES - LINKS

target_link_libraries(
    terminal
    file_systems
)
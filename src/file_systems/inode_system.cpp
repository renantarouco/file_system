#include <file_systems/inode_system.h>

#include <iostream>

INodeSystem::INodeSystem() {}

bool INodeSystem::create_directory(std::string abs_path) {
    std::cout << "inode create directory" << std::endl;
    return false;
}
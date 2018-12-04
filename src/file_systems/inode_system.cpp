#include <file_systems/inode_system.h>

#include <iostream>

INodeSystem::INodeSystem() {}

void INodeSystem::create_directory(std::string abs_path) {
    std::cout << "inode create directory" << std::endl;
}
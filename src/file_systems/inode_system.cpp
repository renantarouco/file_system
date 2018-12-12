#include <file_systems/inode_system.h>

INodeSystem::INodeSystem() {}

INodeSystem::INodeSystem(int ps, int bs) : FileSystem(ps, bs) {}

std::vector<int> INodeSystem::_get_block_stream(int head) {
    
}

std::vector<int> INodeSystem::_get_directory_block_stream(std::vector<std::string> path) {
    
}

bool INodeSystem::mkdir(std::vector<std::string> path) {
    
}

bool INodeSystem::cd(std::vector<std::string> path) {
    
}

std::vector<FileDescriptor> INodeSystem::ls(std::vector<std::string> path) {
    
}

bool INodeSystem::touch(std::vector<std::string>, int, std::string) {
    
}

std::string INodeSystem::cat(std::vector<std::string> path) {
    
}
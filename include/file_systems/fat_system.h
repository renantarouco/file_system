#ifndef _FAT_SYS_H
#define _FAT_SYS_H

#include <file_systems/file_system.h>
#include <file_systems/file_descriptor.h>

#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

class FATSystem : public FileSystem {
private:
    std::vector<int> _fat;
    std::vector<int> _get_block_stream(int);
    std::vector<int> _get_directory_block_stream(std::vector<std::string>);
public:
    FATSystem();
    FATSystem(int, int);
    bool mkdir(std::vector<std::string>);
    bool cd(std::vector<std::string>);
    bool touch(std::vector<std::string>, int, std::string);
    std::vector<FileDescriptor> ls(std::vector<std::string>);
};


#endif
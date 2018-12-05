#ifndef _FAT_SYS_H
#define _FAT_SYS_H

#include <file_systems/file_system.h>

#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

class FATSystem : public FileSystem {
public:
    FATSystem();
    FATSystem(int, int);
    void init(int, int);
    bool create_directory(std::string);
};

#endif
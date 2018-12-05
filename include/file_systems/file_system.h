#ifndef _FS_MODEL_H
#define _FS_MODEL_H

#include <sec_mem_driver/sec_mem_driver.h>
#include <block_manager/block_manager.h>
#include <file_systems/file_descriptor.h>

#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

#define KB 1024
#define MB 1024 * KB
#define GB 1024 * MB

class FileSystem {
protected:
    int _partition_size;
    int _block_size;
    SecMemDriver _sec_mem_driver;
    BlockManager _block_manager;
    FileDescriptor _root_fd;
    virtual std::vector<int> _get_block_stream(int) = 0;
    virtual std::vector<int> _get_directory_block_stream(std::vector<std::string>) = 0;
public:
    FileSystem();
    FileSystem(int, int);
    virtual bool mkdir(std::vector<std::string>) = 0;
    virtual bool cd(std::vector<std::string>) = 0;
    virtual std::vector<FileDescriptor> ls(std::vector<std::string>) = 0;
    virtual std::string listmap();
};

#endif
#ifndef _FILE_DESC_H
#define _FILE_DESC_H

#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <ctime>
#include <iomanip>

struct FileDescriptor {
    std::string name;
    int pos;
    int size;
    char file_type;
    std::chrono::system_clock::time_point creation_time;
    FileDescriptor();
    FileDescriptor(std::string, int, int, char, std::chrono::system_clock::time_point);
    std::string to_str();
    static FileDescriptor from_inode_header(std::string);
    static FileDescriptor from_str(std::string);
    static std::vector<FileDescriptor> from_table_str(std::string);
};

#endif
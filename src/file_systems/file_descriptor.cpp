#include <file_systems/file_descriptor.h>
#include <iostream>
FileDescriptor::FileDescriptor() {}

FileDescriptor::FileDescriptor(
    std::string _name, int _pos, int _size, char _file_type, std::chrono::system_clock::time_point _creation_time
) :
    name(_name), pos(_pos), size(_size), file_type(_file_type), creation_time(_creation_time) {}

std::string FileDescriptor::to_str() {
    std::stringstream descss;
    descss << name << ' ';
    descss << std::to_string(pos) << ' ';
    descss << std::to_string(size) << ' ';
    descss << file_type << ' ';
    std::time_t ct_c = std::chrono::system_clock::to_time_t(creation_time);
    descss << std::put_time(std::localtime(&ct_c), "%Y-%m-%d %H:%M:%S") << std::endl;
    return descss.str();
}

FileDescriptor FileDescriptor::from_inode(std::string description, std::string name, int pos) {
    std::stringstream descss(description);
    FileDescriptor fd;
    fd.name = name;
    fd.pos = pos;
    std::tm c_time;
    std::cout<<descss.str()<<std::endl;
    descss >> fd.size >> fd.file_type >> std::get_time(&c_time, "%Y-%m-%d %H:%M:%S");
    fd.creation_time = std::chrono::system_clock::from_time_t(std::mktime(&c_time));
    return fd;
}

FileDescriptor FileDescriptor::from_fat(std::string description) {
    std::stringstream descss(description);
    FileDescriptor fd;
    std::tm c_time;
    descss >> fd.name >> fd.pos >> fd.size >> fd.file_type >> std::get_time(&c_time, "%Y-%m-%d %H:%M:%S");
    fd.creation_time = std::chrono::system_clock::from_time_t(std::mktime(&c_time));
    return fd;
}

std::vector<FileDescriptor> FileDescriptor::from_table_fat(std::string table_str) {
    std::vector<std::string> file_desc_strs;
    std::string fd_str = "";
    for (char c : table_str) {
        if (c != '\n') {
            fd_str.push_back(c);
        } else {
            file_desc_strs.push_back(fd_str);
            fd_str.clear();
        }
    }
    std::vector<FileDescriptor> fds;
    for (std::string fdstr : file_desc_strs) {
        fds.push_back(FileDescriptor::from_fat(fdstr));
    }
    return fds;
}
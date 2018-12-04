#include <file_systems/file_system.h>

#include <string>
#include <sstream>
#include <chrono>
#include <iomanip>

class FileDescriptor {
public:
    std::string name;
    int pos;
    int size;
    char file_type;
    std::chrono::system_clock::time_point creation_time;
    std::string to_str();
    static FileDescriptor from_str(std::string);
    static std::vector<FileDescriptor> from_table_str(std::string);
};

class FATSystem : public FileSystem {
private:
    FileDescriptor _root_fd;
public:
    FATSystem();
    void init(int, int);
    void create_directory(std::string);
};
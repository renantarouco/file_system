#include <file_systems/file_system.h>
#include <chrono>

class INodeSystem : public FileSystem {
private:
    std::vector<int> _get_block_stream(int);
    std::vector<int> _get_block_stream(std::vector<std::string> path, char file_type);
public:
    INodeSystem();
    INodeSystem(int, int);
    bool mkdir(std::vector<std::string>);
    bool cd(std::vector<std::string>);
    std::vector<FileDescriptor> ls(std::vector<std::string>);
    bool touch(std::vector<std::string>, int, std::string);
    std::string cat(std::vector<std::string> path);
    bool rm(std::vector<std::string> path);
};
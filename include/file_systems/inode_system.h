#include <file_systems/file_system.h>

class INodeSystem : public FileSystem {
public:
    INodeSystem();
    void create_directory(std::string);
};
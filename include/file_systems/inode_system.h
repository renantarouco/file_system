#include <file_systems/file_system.h>

class INodeSystem : public FileSystem {
public:
    INodeSystem();
    bool create_directory(std::string);
};
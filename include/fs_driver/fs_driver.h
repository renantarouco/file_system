#include <iostream>
#include <string>

#include <fs_models/fat_model.h>
#include <fs_models/inode_model.h>

#define MB 1024

class FSDriver {
private:
    FSModel *_file_system_model;
    uint _partition_size;
    uint _block_size;
public:
    FSDriver(std::string fs_model_name, uint partition_size, uint block_size);
};
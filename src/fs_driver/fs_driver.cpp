#include <fs_driver/fs_driver.h>

FSDriver::FSDriver(
    std::string fs_model_name, uint partition_size, uint block_size
) : _partition_size(partition_size), _block_size(block_size) {
    std::cout << "Starting file system driver..." << std::endl;
    std::cout << "MODEL: " << fs_model_name << std::endl;
    std::cout << "PARTTITION SIZE: " << _partition_size << std::endl;
    std::cout << "BLOCK SIZE: " << _block_size << std::endl;
    std::cout << "DONE!" << std::endl;
    if (fs_model_name == "fat") _file_system_model = new FATModel();
    else if (fs_model_name == "inode") _file_system_model = new INodeModel();
    else std::cout << "Invalid File System Model." << std::endl;
    _file_system_model->create_directory();
}
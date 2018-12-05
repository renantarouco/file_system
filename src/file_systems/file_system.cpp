#include <file_systems/file_system.h>

FileSystem::FileSystem() {}

FileSystem::FileSystem(int ps, int bs) :
 _partition_size(ps * MB), _block_size(ps * KB), _root_fd("", 0, 1, 'd', std::chrono::system_clock::now()){
     _sec_mem_driver.init(_partition_size, _block_size);
 }
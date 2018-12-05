#include <file_systems/file_system.h>

FileSystem::FileSystem() : _block_manager(0) {}

FileSystem::FileSystem(int ps, int bs) :
 _partition_size(ps * MB), _block_size(bs * KB),
 _root_fd("", 0, 1, 'd', std::chrono::system_clock::now()),
 _block_manager(_partition_size / _block_size) {
     _sec_mem_driver.init(_partition_size, _block_size);
 }

 std::string FileSystem::listmap() {
     return _block_manager.bitmap_str();
 }
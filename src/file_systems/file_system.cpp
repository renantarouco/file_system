#include <file_systems/file_system.h>

void FileSystem::init(int ps, int bs) {
    _partition_size = ps * MB;
    _block_size = bs * KB;
    _sec_mem_driver.init(_partition_size, _block_size);
}
#include <sec_mem_driver/sec_mem_driver.h>

SecMemDriver::SecMemDriver() {

}

void SecMemDriver::init(int ps, int bs) {
    _sec_mem_file_stream.open("sec_memory", std::ios::out);
    int count = ps / bs;
    for (int i = 0; i <= count; ++i) {
        std::string data(bs, '\0');
        _sec_mem_file_stream.write(data.c_str(), data.size());
        _sec_mem_file_stream.flush();
    }
    _sec_mem_file_stream.close();
    int bitmap_size = std::ceil(count / 8.0);
    _bitmap = std::vector<std::uint8_t>(bitmap_size, 0);
}

std::string SecMemDriver::read_block_data(int id, int bs) {
    _sec_mem_file_stream.open("sec_memory", std::ios::in);
    _sec_mem_file_stream.seekg(id * bs);
    char *data = new char[bs];
    _sec_mem_file_stream.read(data, bs);
    _sec_mem_file_stream.close();
    return std::string(data, bs);
}

int SecMemDriver::write_data(int id, int bs, int os, std::string data) {
    _sec_mem_file_stream.open("sec_memory", std::ios::out);
    _sec_mem_file_stream.seekg(id * bs + os);
    _sec_mem_file_stream.write(data.c_str(), data.size());
    _sec_mem_file_stream.close();
}
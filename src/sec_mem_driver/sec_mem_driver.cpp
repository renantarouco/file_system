#include <sec_mem_driver/sec_mem_driver.h>
SecMemDriver::SecMemDriver() {

}

void SecMemDriver::init(int ps, int bs) {
    _sec_mem_file_ofstream.open("sec_memory");
    int count = ps / bs;
    for (int i = 0; i <= count; ++i) {
        std::string data(bs, '\0');
        _sec_mem_file_ofstream.write(data.c_str(), data.size());
        _sec_mem_file_ofstream.flush();
    }
    _sec_mem_file_ofstream.flush();
    _sec_mem_file_ifstream.open("sec_memory");
    int bitmap_size = std::ceil(count / 8.0);
    _bitmap = std::vector<std::uint8_t>(bitmap_size, 0);
}

std::string SecMemDriver::read_block_data(int id, int bs) {
    _sec_mem_file_ifstream.seekg(id * bs);
    char *data = new char[bs];
    _sec_mem_file_ifstream.read(data, bs);
    return std::string(data, bs);
}

int SecMemDriver::write_data(int id, int bs, int os, std::string data) {
    _sec_mem_file_ofstream.seekp(id * bs + os);
    _sec_mem_file_ofstream.write(data.c_str(), data.size());
    _sec_mem_file_ofstream.flush();
}
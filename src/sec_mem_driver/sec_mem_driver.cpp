#include <sec_mem_driver/sec_mem_driver.h>
SecMemDriver::SecMemDriver() {

}

void SecMemDriver::init(int ps, int bs) {
    _fout.open("file", std::ios::out);
    int count = ps / bs;
    for (int i = 0; i <= count; ++i) {
        std::string data(bs, '\0');
        _fout.write(data.c_str(), data.size());
        _fout.flush();
    }
    _fin.open("file", std::ios::in);
}

std::string SecMemDriver::read_block_data(int id, int bs) {
    _fin.seekg(id * bs);
    char *data = new char[bs];
    _fin.read(data, bs);
    return std::string(data, bs);
}

int SecMemDriver::write_data(int id, int bs, int os, std::string data) {
    _fout.seekp(id * bs + os);
    _fout.write(data.c_str(), data.size());
    _fout.flush();
}
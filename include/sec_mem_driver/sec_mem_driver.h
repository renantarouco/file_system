#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>

class SecMemDriver {
private:
    std::fstream _sec_mem_file_stream;
    std::vector<std::uint8_t> _bitmap;
public:
    SecMemDriver();
    void init(int, int);
    std::string read_block_data(int, int);
    int write_data(int, int, int, std::string);
};
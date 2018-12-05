#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>

class SecMemDriver {
private:
    std::ofstream _fout;
    std::ifstream _fin;
public:
    SecMemDriver();
    void init(int, int);
    std::string read_block_data(int, int);
    int write_data(int, int, int, std::string);
};
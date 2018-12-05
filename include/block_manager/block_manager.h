#include <vector>
#include <string>

class BlockManager {
private:
    std::vector<bool> _bitmap;
public:
    BlockManager(int);
    std::vector<int> get_available_blocks(int);
    void occupy_block(int);
    std::string bitmap_str();
};
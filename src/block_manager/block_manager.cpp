#include <block_manager/block_manager.h>

BlockManager::BlockManager(int n) : _bitmap(n, false) {

}

std::vector<int> BlockManager::get_available_blocks(int n) {
    std::vector<int> available_ids;
    for (int i = 0; i < _bitmap.size() && n > 0; ++i) {
        if (!_bitmap[i]) {
            available_ids.push_back(i);
            n--;
        }
    }
    if (!n) return available_ids;
    return std::vector<int>();
}

void BlockManager::occupy_block(int id) {
    if(id >= 0 && id < _bitmap.size()) {
        _bitmap[id] = true;
    }
}
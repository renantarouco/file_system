#include <file_systems/file_systems.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main(int argc, char const *argv[])
{
    std::ifstream config_file("../config.json");
    json config_json;
    config_file >> config_json;
    std::string fs_model_str = config_json["model"].get<std::string>();
    FileSystem *fs;
    if (fs_model_str == "fat") fs = new FATSystem();
    else if (fs_model_str == "inode") fs = new INodeSystem();
    else {
        std::cerr << "unsupported file system model" << std::endl;
        return 1;
    }
    int ps = config_json["ps"].get<int>();
    int bs = config_json["bs"].get<int>();

    std::cout << "initializing file system..." << std::endl;
    std::cout << std::left << std::setw(8) << "model" << " ";
    std::cout << std::left << std::setw(8) << "ps" << " ";
    std::cout << std::left << std::setw(8) << "bs" << std::endl;
    std::cout << std::left << std::setw(8) << fs_model_str << " ";
    std::cout << std::left << std::setw(8) << ps << " ";
    std::cout << std::left << std::setw(8) << bs << std::endl;

    fs->init(ps, bs);

    std::cout << "... done." << std::endl;

    return 0;
}

#include <iostream>
#include <fstream>
#include <string>

#include <nlohmann/json.hpp>

#include <fs_driver/fs_driver.h>

using namespace std;
using json = nlohmann::json;

int main(int argc, char* argv[]) {
    cout << "hello, file_system" << endl;
    ifstream config_file("../config.json");
    json config_json;
    config_file >> config_json;
    FSDriver *fsd = new FSDriver(
        config_json["model"].get<string>(),
        config_json["partition"].get<int>(),
        config_json["block"].get<int>()
    );
    delete fsd;
    return 0;
}
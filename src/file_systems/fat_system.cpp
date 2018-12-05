#include <file_systems/fat_system.h>

FATSystem::FATSystem(int ps, int bs) : FATSystem(ps, bs) {
    create_directory("home");
}

bool FATSystem::create_directory(std::string abs_path) {
    std::vector<std::string> directories_names;
    std::string dir_name = "";
    for (char c : abs_path) {
        if (c != '/') dir_name.push_back(c);
        else {
            directories_names.push_back(dir_name);
            dir_name.clear();
        }
    }
    if (directories_names.empty()) {
        std::string block_data = _sec_mem_driver.read_block_data(0, _block_size);
        int table_end = block_data.find('\0');
        if (table_end == 0) {
            FileDescriptor dir;
            dir.name = dir_name;
            dir.pos = 1;
            dir.size = 1;
            dir.file_type = 'd';
            dir.creation_time = std::chrono::system_clock::now();
            _sec_mem_driver.write_data(0, _block_size, 0, dir.to_str());
        } else {
            std::string table_str(block_data.begin(), block_data.begin() + table_end);
            std::vector<FileDescriptor> directories = FileDescriptor::from_table_str(table_str);
            for(FileDescriptor file : directories) {
                if(file.name == dir_name) {
                    std::cout<<"name exists"<<std::endl;
                    return false;
                }
            }
            /*FileDescriptor dir;
            dir.name = dir_name;
            dir.pos = 2;
            dir.size = 1;
            dir.file_type = 'd';
            dir.creation_time = std::chrono::system_clock::now();
            _sec_mem_driver.write_data(0, _block_size, 0, dir.to_str());*/  
        }
    } else {

    }
}
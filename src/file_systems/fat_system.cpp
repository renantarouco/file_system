#include <file_systems/fat_system.h>

FATSystem::FATSystem() {}

void FATSystem::init(int ps, int bs) {
    FileSystem::init(ps, bs);
    _root_fd.name = "/";
    _root_fd.pos = 0;
    _root_fd.size = 1;
    _root_fd.file_type = 'd';
    _root_fd.creation_time = std::chrono::system_clock::now();
    fat = std::vector<int>(_partition_size/_block_size, -1);
    create_directory("home");
    create_directory("kkk");
    create_directory("home/oi");
    std::cout<<(directory_exists("kkk")?1:0)<<std::endl;
    std::vector<FileDescriptor> directories = get_files_descriptors("");
    for(int i = 0 ; i < directories.size() ; i++) {
        std::cout<<directories[i].name<<" ";
    }
    std::cout<<std::endl;
}

std::vector<int> FATSystem::get_all_blocks(int init) {
    std::vector<int> blocks;
    blocks.push_back(init);
    
    int i = init;
    while(fat[i] != -1) {
        i = fat[i];
        blocks.push_back(i);
    }

    return blocks;
}

std::vector<int> FATSystem::go_to_directory(std::string abs_path) {
    std::vector<std::string> directories_names;
    std::string dir_name = "";
    for (char c : abs_path) {
        if (c != '/') dir_name.push_back(c);
        else {
            directories_names.push_back(dir_name);
            dir_name.clear();
        }
    }
    if(dir_name.size() > 0) 
        directories_names.push_back(dir_name);

    if (directories_names.empty()) {
        return std::vector<int>{0};
    } else {
        int read_header = 0;
        std::vector<std::string>::iterator it;
        for(it = directories_names.begin() ; it != directories_names.end() ; it++) {
            std::string block_data = _sec_mem_driver.read_block_data(read_header, _block_size);
            std::size_t table_end = block_data.find('\0');
            if (table_end == block_data.npos) {
                if(it == (directories_names.end() - 1))
                    std::cout<<"the directory is empty"<<std::endl;
                else
                    std::cout<<"wrong path"<<std::endl;
                return std::vector<int>();
            } else {
                std::string table_str(block_data.begin(), block_data.begin() + table_end);
                std::vector<FileDescriptor> directories = FileDescriptor::from_table_str(table_str);
                for(FileDescriptor file : directories) {
                    if(file.name == *(it)) {
                        if(file.file_type == 'd' && it == (directories_names.end() - 1)) {
                            std::cout<<"success"<<std::endl;
                            return get_all_blocks(file.pos);
                        }   
                        else if(file.file_type == 'd')
                            read_header = file.pos;
                        else {
                            std::cout<<(*it)<<" is not a directory"<<std::endl;
                            return std::vector<int>();
                        }
                    }
                }
            }
        }
        std::cout<<"wrong path"<<std::endl;
        return std::vector<int>();
    }
}

bool FATSystem::directory_exists(std::string abs_path) {
    std::vector<int> ans;
    ans = go_to_directory(abs_path);

    return ans.size() == 0 ? false : true;
}

std::vector<FileDescriptor> FATSystem::get_files_descriptors(std::string abs_path) {
    std::vector<int> ans;
    ans = go_to_directory(abs_path);

    if(ans.size() == 0) return std::vector<FileDescriptor>();

    std::vector<FileDescriptor> directories;
    std::vector<int>::iterator it;
    for(it = ans.begin() ; it != ans.end() ; it++){
        std::string block_data = _sec_mem_driver.read_block_data(*it, _block_size);
        int table_end = block_data.find('\0');
        if(table_end != 0) {
            std::string table_str(block_data.begin(), block_data.begin() + table_end);
            std::vector<FileDescriptor> directories_l;
            directories_l = FileDescriptor::from_table_str(table_str);
            directories.insert(directories.end(), directories_l.begin(), directories_l.end());
        }
    }
    return directories;
}

bool FATSystem::create_directory(std::string abs_path) {
    std::size_t found = abs_path.find_last_of("/");
    std::string path, dir_name;
    if(found == abs_path.npos) {
        dir_name = abs_path.substr(0, found);
        path = "";
    } else {
        path = abs_path.substr(0, found);
        dir_name = abs_path.substr(found+1);
    }


    std::vector<int> ans;
    ans = go_to_directory(path);

    if(ans.size() == 0) {
        return false;
    }

    std::string block_data = _sec_mem_driver.read_block_data(*(ans.end() - 1), _block_size);
    int table_end = block_data.find('\0');
    if (table_end == 0) {
        FileDescriptor dir;
        dir.name = dir_name;
        dir.pos = 20; //@TODO: pede pro block
        dir.size = 1;
        dir.file_type = 'd';
        dir.creation_time = std::chrono::system_clock::now();
        //v1.0 - nao precisa verificar se vai caber o string
        _sec_mem_driver.write_data(*(ans.end() - 1), _block_size, 0, dir.to_str());
    } else {
        std::string table_str(block_data.begin(), block_data.begin() + table_end);
        std::vector<FileDescriptor> directories = FileDescriptor::from_table_str(table_str);
        int off_set = 0;
        for(FileDescriptor file : directories) {
            off_set+= file.to_str().size();
            if(file.name == dir_name) {
                std::cout<<"name exists"<<std::endl;
                return false;
            }
        }

        FileDescriptor dir;
        dir.name = dir_name;
        dir.pos = 2;//@TODO: pede pro block
        dir.size = 1;
        dir.file_type = 'd';
        dir.creation_time = std::chrono::system_clock::now();
        std::string w_str = dir.to_str();
        int new_block_size = off_set + w_str.size();
        if(new_block_size > _block_size) {
            int new_block = 55; //@TODO: pede novo bloco pro block_manager
            fat[*(ans.end() - 1)] = new_block;
            _sec_mem_driver.write_data(new_block, _block_size, 0, dir.to_str());
        }
        else
            _sec_mem_driver.write_data(*(ans.end() - 1), _block_size, off_set, dir.to_str());
    }
}

bool FATSystem::create_file(std::string abs_path, int size, std::string text) {
    std::vector<std::string> directories_names;
    std::string dir_name = "";
    for (char c : abs_path) {
        if (c != '/') dir_name.push_back(c);
        else {
            directories_names.push_back(dir_name);
            dir_name.clear();
        }
    }

    //@TODO: write in directory new file


    int blocks_number = std::ceil(size/_block_size);

    std::vector<int> blocks{10, 11}; //@TODO: pede pro block

    int i;
    std::string write_str;
    for(i = 0 ; i < blocks_number ; i++) {
        write_str = text.substr(_block_size*i, _block_size*(i + 1));
        if(write_str.size() > 0)
        {
            _sec_mem_driver.write_data(blocks[i], _block_size, 0, write_str);
        }

        if(i > 0) {
            fat[blocks[i - 1]] = blocks[i];
        }
    }
}

std::string FileDescriptor::to_str() {
    std::stringstream descss;
    descss << name << ' ';
    descss << std::to_string(pos) << ' ';
    descss << std::to_string(size) << ' ';
    descss << 'd' << ' ';
    std::time_t ct_c = std::chrono::system_clock::to_time_t(creation_time - std::chrono::hours(24));
    descss << std::put_time(std::localtime(&ct_c), "%F %T") << std::endl;
    return descss.str();
}

FileDescriptor FileDescriptor::from_str(std::string description) {
    std::stringstream descss(description);
    FileDescriptor fd;
    std::tm c_time;
    descss >> fd.name >> fd.pos >> fd.size >> fd.file_type >> std::get_time(&c_time, "%F %T");
    fd.creation_time = std::chrono::system_clock::from_time_t(std::mktime(&c_time));
    return fd;
}

std::vector<FileDescriptor> FileDescriptor::from_table_str(std::string table_description) {
    std::vector<std::string> file_desc_strs;
    std::string fd_str = "";
    for (char c : table_description) {
        if (c != '\n') {
            fd_str.push_back(c);
        } else {
            file_desc_strs.push_back(fd_str);
            fd_str.clear();
        }
    }
    std::vector<FileDescriptor> fds;
    for (std::string fdstr : file_desc_strs) {
        fds.push_back(FileDescriptor::from_str(fdstr));
    }
    return fds;
}


//possivelmente util
    /*std::vector<std::string> directories_names;
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
                    return;
                }
            }

            /*FileDescriptor dir;
            dir.name = dir_name;
            dir.pos = 2;
            dir.size = 1;
            dir.file_type = 'd';
            dir.creation_time = std::chrono::system_clock::now();
            _sec_mem_driver.write_data(0, _block_size, 0, dir.to_str());
        }
    } else {

    }*/
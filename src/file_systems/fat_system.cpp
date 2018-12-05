#include <file_systems/fat_system.h>

FATSystem::FATSystem() {}

FATSystem::FATSystem(int ps, int bs) : FileSystem(ps, bs), _fat(_partition_size / _block_size, -1) {
    _block_manager.occupy_block(0);
    std::vector<std::string> home_path;
    home_path.push_back("home");
    mkdir(home_path);
    //home_path.push_back("igor");
    //mkdir(home_path);
    home_path.push_back("dsd.txt");
    touch(home_path, 2048, "blablabla");
}

std::vector<int> FATSystem::_get_block_stream(int head) {
    std::vector<int> blocks{ head };
    int i = head;
    while(_fat[i] != -1) {
        i = _fat[i];
        blocks.push_back(i);
    }
    return blocks;
}

std::vector<int> FATSystem::_get_directory_block_stream(std::vector<std::string> path) {
    if (path.empty()) return std::vector<int>{0};
    else {
        int head = 0;
        std::vector<std::string>::iterator it;
        for(it = path.begin() ; it != path.end() ; it++) {
            std::string block_data = _sec_mem_driver.read_block_data(head, _block_size);
            std::size_t table_end = block_data.find('\0');
            if (table_end == block_data.npos) {
                if(it == (path.end() - 1))
                    std::cout<<"the directory is empty"<<std::endl;
                else
                    std::cout<<"wrong path"<<std::endl;
                return std::vector<int>();
            } else {
                std::string table_str(block_data.begin(), block_data.begin() + table_end);
                std::vector<FileDescriptor> directories = FileDescriptor::from_table_str(table_str);
                for(FileDescriptor file : directories) {
                    if(file.name == *(it)) {
                        if(file.file_type == 'd' && it == (path.end() - 1)) {
                            std::cout << "success" << std::endl;
                            return _get_block_stream(file.pos);
                        }   
                        else if(file.file_type == 'd')
                            head = file.pos;
                        else {
                            std::cout << (*it) << " is not a directory" << std::endl;
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

bool FATSystem::mkdir(std::vector<std::string> path) {
    std::string dir_name = path.back();
    path.pop_back();
    std::cout << path.size() << std::endl;
    std::vector<int> dir_block_stream;
    dir_block_stream = _get_directory_block_stream(path);
    if(dir_block_stream.size() == 0) return false;
    std::string block_data = _sec_mem_driver.read_block_data(dir_block_stream.back(), _block_size);
    int table_end = block_data.find('\0');
    if (table_end == 0) {
        // TODO: pedir pro bm
        FileDescriptor dir(
            dir_name, _block_manager.get_available_blocks(1)[0], 1, 'd', std::chrono::system_clock::now()
        );
        //v1.0 - nao precisa verificar se vai caber o string
        _sec_mem_driver.write_data(dir_block_stream.back(), _block_size, 0, dir.to_str());
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
        // TODO: pedir pro bm
        FileDescriptor dir(
            dir_name, _block_manager.get_available_blocks(1)[0], 1, 'd', std::chrono::system_clock::now()
        );
        std::string w_str = dir.to_str();
        int new_block_size = off_set + w_str.size();
        if(new_block_size > _block_size) {
            int new_block = _block_manager.get_available_blocks(1)[0]; //@TODO: pede novo bloco pro block_manager
            _fat[dir_block_stream.back()] = new_block;
            _sec_mem_driver.write_data(new_block, _block_size, 0, dir.to_str());
        }
        else _sec_mem_driver.write_data(dir_block_stream.back(), _block_size, off_set, dir.to_str());
    }
}

bool FATSystem::cd(std::vector<std::string> path) {
    std::vector<int> dir_block_stream;
    dir_block_stream = _get_directory_block_stream(path);
    return dir_block_stream.size() == 0 ? false : true;
}

bool FATSystem::touch(std::vector<std::string> path, int size, std::string text) {
    std::string file_name = path.back();
    path.pop_back();
    
    std::vector<int> dir_block_stream;
    dir_block_stream = _get_directory_block_stream(path);

    if(dir_block_stream.size() == 0) return false;

    int blocks_number = std::ceil((float)size/_block_size);

    std::cout<<_block_size<<std::endl;

    std::vector<int> blocks; 
    blocks = _block_manager.get_available_blocks(blocks_number);//@TODO: pede pro block

    std::cout<<blocks.size()<<std::endl;

    FileDescriptor file;
    std::string block_data = _sec_mem_driver.read_block_data(dir_block_stream.back(), _block_size);
    int table_end = block_data.find('\0');
    if (table_end == 0) {
        file = FileDescriptor(
            file_name, blocks[0], blocks_number, 'f', std::chrono::system_clock::now()
        );

        //v1.0 - nao precisa verificar se vai caber o string
        _sec_mem_driver.write_data(dir_block_stream.back(), _block_size, 0, file.to_str());

    } else {
        std::string table_str(block_data.begin(), block_data.begin() + table_end);
        std::vector<FileDescriptor> directories = FileDescriptor::from_table_str(table_str);
        int off_set = 0;
        for(FileDescriptor f : directories) {
            off_set+= f.to_str().size();
            if(f.name == file_name) {
                std::cout<<"name exists"<<std::endl;
                return false;
            }
        }
        file = FileDescriptor(
            file_name, blocks[0], blocks_number, 'f', std::chrono::system_clock::now()
        );
        std::string w_str = file.to_str();
        int new_block_size = off_set + w_str.size();
        if(new_block_size > _block_size) {
            //mudar size no diretorio anterior
            int new_block = 55; //@TODO: pede novo bloco pro block_manager
            _fat[dir_block_stream.back()] = new_block;
            _sec_mem_driver.write_data(new_block, _block_size, 0, file.to_str());
        }
        else _sec_mem_driver.write_data(dir_block_stream.back(), _block_size, off_set, file.to_str());
    }

    int i = 0;
    char *data = new char[_block_size];
    std::stringstream textss(text);
    while (!textss.eof()) {
        textss.read(data, _block_size);
        _sec_mem_driver.write_data(blocks[i], _block_size, 0, std::string(data));
        i++;
    }
    /*for(i = 0 ; i < blocks_number ; i++) {
        
        write_str = text.substr(_block_size*i, _block_size*(i + 1));
        if(write_str.size() > 0)
        {
            _sec_mem_driver.write_data(blocks[i], _block_size, 0, write_str);
        }

        if(i > 0) {
            _fat[blocks[i - 1]] = blocks[i];
        }
    }*/
}

std::vector<FileDescriptor> FATSystem::ls(std::vector<std::string> path) {
    std::vector<int> dir_block_stream = _get_directory_block_stream(path);
    if(dir_block_stream.size() == 0) return std::vector<FileDescriptor>();
    std::vector<FileDescriptor> directories;
    std::vector<int>::iterator it;
    for(it = dir_block_stream.begin() ; it != dir_block_stream.end() ; it++){
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
#include <file_systems/fat_system.h>

FATSystem::FATSystem() {}

FATSystem::FATSystem(int ps, int bs) : FileSystem(ps, bs), _fat(_partition_size / _block_size, -1) {
    _block_manager.occupy_block(0);
}

void FATSystem::_free_fat(std::vector<int> blocks) {
    for(int block: blocks) {
        _fat[block] = -1;
    }
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

std::vector<int> FATSystem::_get_block_stream(std::vector<std::string> path, char file_type) {
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
                std::vector<FileDescriptor> directories = FileDescriptor::from_table_fat(table_str);
                for(FileDescriptor file : directories) {
                    if(file.name == *(it)) {
                        if(it == (path.end() - 1)) {
                            if(file.file_type == file_type) {
                                return _get_block_stream(file.pos);
                            }
                            else {
                                std::cout<<file.name<<" is a "<<(file_type == 'f'?"directory":"file")<<std::endl;
                                return std::vector<int>();
                            }
                        }   
                        
                        if(file.file_type == 'd')
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
    std::vector<int> dir_block_stream;
    dir_block_stream = _get_block_stream(path, 'd');
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
        std::vector<FileDescriptor> directories = FileDescriptor::from_table_fat(table_str);
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
    dir_block_stream = _get_block_stream(path, 'd');
    return dir_block_stream.size() == 0 ? false : true;
}

bool FATSystem::touch(std::vector<std::string> path, int size, std::string text) {
    std::string file_name = path.back();
    path.pop_back();
    
    std::vector<int> dir_block_stream;
    dir_block_stream = _get_block_stream(path, 'd');

    if(dir_block_stream.size() == 0) return false;

    int blocks_number = std::ceil((float)size/_block_size);

    std::vector<int> blocks; 
    blocks = _block_manager.get_available_blocks(blocks_number);
    
    int i;
    for(i = 0 ; i < blocks.size() ; i++) {
        if(i < blocks.size() - 1) {
            _fat[blocks[i]] = blocks[i+1];
        }
    }

    _fat[blocks[i-1]] = -1;

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
        std::vector<FileDescriptor> directories = FileDescriptor::from_table_fat(table_str);
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
            int new_block = _block_manager.get_available_blocks(1)[0]; //@TODO: pede novo bloco pro block_manager
            _fat[dir_block_stream.back()] = new_block;
            _sec_mem_driver.write_data(new_block, _block_size, 0, file.to_str());
        }
        else _sec_mem_driver.write_data(dir_block_stream.back(), _block_size, off_set, file.to_str());
    }

    std::string write_str;
    for(i = 0 ; i < blocks_number ; i++) {
        int init = _block_size*i;
        if(init > text.size()) {
            break;
        }
        int end = _block_size*(i + 1);
        if(end > text.size()){
            end = text.size();
        }
        write_str = text.substr(init, end);
        if(write_str.size() > 0)
        {
            _sec_mem_driver.write_data(blocks[i], _block_size, 0, write_str);
        }
    }
    return true;
}

std::vector<FileDescriptor> FATSystem::ls(std::vector<std::string> path) {
    std::vector<int> dir_block_stream = _get_block_stream(path, 'd');
    if(dir_block_stream.size() == 0) return std::vector<FileDescriptor>();
    std::vector<FileDescriptor> directories;
    std::vector<int>::iterator it;
    for(it = dir_block_stream.begin() ; it != dir_block_stream.end() ; it++){
        std::string block_data = _sec_mem_driver.read_block_data(*it, _block_size);
        int table_end = block_data.find('\0');
        if(table_end != 0) {
            std::string table_str(block_data.begin(), block_data.begin() + table_end);
            std::vector<FileDescriptor> directories_l;
            directories_l = FileDescriptor::from_table_fat(table_str);
            directories.insert(directories.end(), directories_l.begin(), directories_l.end());
        }
    }
    return directories;
}

std::string FATSystem::cat(std::vector<std::string> path) {
    std::vector<int> file_block_stream = _get_block_stream(path, 'f');

    std::string text;
    std::vector<int>::iterator kt;
    for(kt = file_block_stream.begin() ; kt != file_block_stream.end() ; kt++) {
        std::string file_data = _sec_mem_driver.read_block_data(*kt, _block_size);
        text.append(file_data);
    }

    return text;
}

bool FATSystem::rm(std::vector<std::string> path) {
    std::string name = path.back();
    path.pop_back();
    
    std::vector<int> dir_blocks;
    dir_blocks = _get_block_stream(path, 'd');

    FileDescriptor file;
    bool found = false;
    int i, found_block;
    for(int block: dir_blocks) {
        std::string block_data = _sec_mem_driver.read_block_data(block, _block_size);
        size_t end = block_data.find('\0');

        std::string data;
        for(i = 0; i < end; i++) {
            if(block_data[i] != '\n') {
                data += block_data[i];
            }
            else {
                file = FileDescriptor::from_fat(data);
                if(file.name == name){
                    found = true;
                    found_block = block;
                    break;
                }
                data = "";
            }
        }
    }

    if(!found){
        std::cout<<"file does not exist."<<std::endl;
        return false;
    }

    std::vector<int> file_blocks = _get_block_stream(file.pos);

    if(file.file_type == 'd') {
        std::string block_data = _sec_mem_driver.read_block_data(file_blocks[0], _block_size);
        size_t end = block_data.find('\0');
        if(end != 0) {
            std::cout<<"directory is not empty."<<std::endl;
            return false;
        }
    }

    int dir_str_size = file.to_str().size() - 1;
    std::string void_line(dir_str_size, '\0');
    _sec_mem_driver.write_data(found_block, _block_size, (i - dir_str_size), void_line);

    _free_fat(file_blocks);
    _block_manager.free_block(file_blocks);
    
    std::string void_block(_block_size, '\0');
    for(int block: file_blocks) {
        _sec_mem_driver.write_data(block, _block_size, 0, void_block);
    }
    return true;
}
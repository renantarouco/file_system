#include <file_systems/inode_system.h>

INodeSystem::INodeSystem() {}

INodeSystem::INodeSystem(int ps, int bs) : FileSystem(ps, bs) {}

std::vector<int> INodeSystem::_get_block_stream(int head) {
    std::string inode_data = _sec_mem_driver.read_block_data(head, _block_size);
    std::size_t table_end = inode_data.find('\0');
    inode_data = inode_data.substr(0, table_end);
    std::size_t table_header = inode_data.find('\n');
    inode_data = inode_data.substr(table_header); 

    std::vector<int> blocks;
    std::string current_block = "";
    for(int i = 0 ; i < inode_data.size() ; i++) {
        if(inode_data[i] == ' ') {
            blocks.push_back(stoi(current_block));
            current_block = "";
        }
        else {
            current_block += inode_data[i];
        }
    }
    return blocks;
}

std::vector<int> INodeSystem::_get_directory_block_stream(std::vector<std::string> path) {
    if (path.empty()) {
        //busca no inode de posicao 0
    }
    else {
        int head = 0;
        std::vector<std::string>::iterator it;
        for(it = path.begin() ; it != path.end() ; it++) {
            std::string inode_data = _sec_mem_driver.read_block_data(head, _block_size);
            std::size_t table_end = inode_data.find('\0');
            inode_data = inode_data.substr(0, table_end);
            std::size_t table_header = inode_data.find('\n');
            std::string header = inode_data.substr(0, table_header);
            FileDescriptor file;
            file.pos = head;
            file.from_inode_header(header);
            inode_data = inode_data.substr(table_header); 

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

bool INodeSystem::mkdir(std::vector<std::string> path) {
    std::string dir_name = path.back();
    path.pop_back();
    std::vector<int> dir_block_stream;
}

bool INodeSystem::cd(std::vector<std::string> path) {
    
}

std::vector<FileDescriptor> INodeSystem::ls(std::vector<std::string> path) {
    
}

bool INodeSystem::touch(std::vector<std::string>, int, std::string) {
    
}

std::string INodeSystem::cat(std::vector<std::string> path) {
    
}
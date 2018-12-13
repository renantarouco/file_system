#include <file_systems/inode_system.h>

INodeSystem::INodeSystem() {}

INodeSystem::INodeSystem(int ps, int bs) : FileSystem(ps, bs) {
    _block_manager.occupy_block(0);
    std::stringstream descss;
    std::chrono::system_clock::time_point creation_time = std::chrono::system_clock::now();
    std::time_t ct_c = std::chrono::system_clock::to_time_t(creation_time);
    descss<<"1 d "<<std::put_time(std::localtime(&ct_c), "%Y-%m-%d %H:%M:%S")<<'\n'<<"1 ";
    _sec_mem_driver.write_data(0, _block_size, 0, descss.str());
    _block_manager.occupy_block(1);
}

std::vector<int> INodeSystem::_get_block_stream(int head) {
    std::string inode_data = _sec_mem_driver.read_block_data(head, _block_size);
    std::size_t table_end = inode_data.find('\0');
    inode_data = inode_data.substr(0, table_end);
    std::size_t table_header = inode_data.find('\n');
    inode_data = inode_data.substr(table_header + 1);     

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

std::vector<int> INodeSystem::_get_block_stream(std::vector<std::string> path, char file_type) {
    if (path.empty()) {
        return _get_block_stream(0);
    }
    
    int head = -1;
    std::vector<int> blocks = _get_block_stream(0);

    for(int block: blocks) {
        std::string block_data = _sec_mem_driver.read_block_data(block, _block_size);
        size_t end = block_data.find('\0');

        std::string data;
        for(int i = 0; i < end; i++) {
            if(block_data[i] != '\n') {
                data += block_data[i];
            }
            else {
                size_t space = data.find(' ');
                std::string name = data.substr(0, space);
                if(name == path[0]) {
                    head = stoi(data.substr(space + 1));
                    break;
                }
                data = "";
            }
        }
    }
    
    if(head == -1){
        std::cout<<"wrong path"<<std::endl;
        return std::vector<int>();
    }

    std::vector<std::string>::iterator it;
    for(it = path.begin() ; it != path.end() ; it++) {
        std::string inode_data = _sec_mem_driver.read_block_data(head, _block_size);
        std::size_t table_header = inode_data.find('\n');
        inode_data = inode_data.substr(0, table_header);

        std::vector<int> blocks = _get_block_stream(head);

        FileDescriptor file;
        file =  FileDescriptor::from_inode(inode_data, *it, head);

        if(file.name == *(path.end() - 1)) {
            if(file.file_type == file_type) {
                return blocks;
            }
            else {
                std::cout<<file.name<<" is a "<<(file_type == 'f'?"directory":"file")<<std::endl;
                return std::vector<int>();
            }
        }

        if(file.file_type != 'd') {
            std::cout << (*it) << " is not a directory" << std::endl;
            return std::vector<int>();
        }

        else {
            std::vector<std::pair<std::string, int> > inodes;
            std::string block_data;
            for(int b: blocks) {
                block_data = _sec_mem_driver.read_block_data(b, _block_size);
                size_t end = block_data.find('\0');
                std::string data;
                for(int i = 0; i < end; i++) {
                    if(block_data[i] != '\n') {
                        data += block_data[i];
                    }
                    else {
                        size_t space_pos = data.find(' ');
                        inodes.push_back(make_pair(data.substr(0, space_pos), stoi(data.substr(space_pos + 1))));
                        data = "";
                    }
                }
            }

            bool found = false;
            for(std::pair<std::string, int> inode: inodes) {
                if(inode.first == *(it + 1)) {
                    head = inode.second;
                    found = true;
                    break;
                }
            }

            if(!found) {
                std::cout<<"wrong_path"<<std::endl;
                return std::vector<int>();
            } 
        }
        
    }
}

bool INodeSystem::mkdir(std::vector<std::string> path) {
    std::string dir_name = path.back();
    path.pop_back();
    std::vector<int> dir_block_stream = _get_block_stream(path, 'd');
    if(dir_block_stream.empty()) {
        return false;
    }
    
    std::string block_data = _sec_mem_driver.read_block_data(dir_block_stream.back(), _block_size);
    size_t end = block_data.find('\0');

    std::string data;
    for(int i = 0; i < end; i++) {
        if(block_data[i] != '\n') {
            data += block_data[i];
        }
        else {
            size_t space = data.find(' ');
            std::string fname = data.substr(0, space);
            int fpos = stoi(data.substr(space + 1));
            
            if(fname == dir_name){
                std::cout<<"name exists."<<std::endl;
                return false;
            }
            data = "";
        }
    }

    std::vector<int> inode_pos = _block_manager.get_available_blocks(1);
    std::string inode_dir_data;
    inode_dir_data.append(dir_name);
    inode_dir_data += ' ';
    inode_dir_data.append(std::to_string(inode_pos[0]));
    inode_dir_data += '\n';
    _sec_mem_driver.write_data(dir_block_stream.back(), _block_size, end, inode_dir_data);

    std::vector<int> pos = _block_manager.get_available_blocks(1);

    std::stringstream descss;
    std::chrono::system_clock::time_point creation_time = std::chrono::system_clock::now();
    std::time_t ct_c = std::chrono::system_clock::to_time_t(creation_time);
    descss<<"1 d "<<std::put_time(std::localtime(&ct_c), "%Y-%m-%d %H:%M:%S")<<'\n'<<std::to_string(pos[0])<<" ";
    _sec_mem_driver.write_data(inode_pos[0], _block_size, 0, descss.str());
    return true;
}

bool INodeSystem::cd(std::vector<std::string> path) {
    std::vector<int> blocks = _get_block_stream(path, 'd');
    return !blocks.empty();
}

std::vector<FileDescriptor> INodeSystem::ls(std::vector<std::string> path) {
    std::vector<int> blocks = _get_block_stream(path, 'd');
    if(blocks.empty()) {
        return std::vector<FileDescriptor>();
    }
    std::vector<FileDescriptor> files;

    for(int block: blocks) {
        std::string block_data = _sec_mem_driver.read_block_data(block, _block_size);
        size_t end = block_data.find('\0');

        std::string data;
        for(int i = 0; i < end; i++) {
            if(block_data[i] != '\n') {
                data += block_data[i];
            }
            else {
                FileDescriptor file;
                size_t space = data.find(' ');
                std::string name = data.substr(0, space);
                int pos = stoi(data.substr(space + 1));
                std::string inode_data = _sec_mem_driver.read_block_data(pos, _block_size);
                size_t header_end = inode_data.find('\n');
                inode_data = inode_data.substr(0, header_end);

                file = FileDescriptor::from_inode(inode_data, name, pos);
                
                data = "";
                files.push_back(file);
            }
        }
    }
    return files;
}

bool INodeSystem::touch(std::vector<std::string> path, int size, std::string text) {
    std::string file_name = path.back();
    path.pop_back();
    std::vector<int> dir_block_stream = _get_block_stream(path, 'd');
    if(dir_block_stream.empty()) {
        return false;
    }

    std::string block_data = _sec_mem_driver.read_block_data(dir_block_stream.back(), _block_size);
    size_t end = block_data.find('\0');

    std::string data;
    for(int i = 0; i < end; i++) {
        if(block_data[i] != '\n') {
            data += block_data[i];
        }
        else {
            size_t space = data.find(' ');
            std::string fname = data.substr(0, space);
            int fpos = stoi(data.substr(space + 1));
            
            if(fname == file_name){
                std::cout<<"name exists."<<std::endl;
                return false;
            }
            data = "";
        }
    }

    std::vector<int> inode_pos = _block_manager.get_available_blocks(1);
    std::string inode_dir_data;
    inode_dir_data.append(file_name);
    inode_dir_data += ' ';
    inode_dir_data.append(std::to_string(inode_pos[0]));
    inode_dir_data += '\n';
    _sec_mem_driver.write_data(dir_block_stream.back(), _block_size, end, inode_dir_data);

    int blocks_number = std::ceil((float)size/_block_size);
    //TODO: verificiar se não passa do tamanho do inode
    std::vector<int> pos = _block_manager.get_available_blocks(blocks_number);
    
    std::stringstream descss;
    std::chrono::system_clock::time_point creation_time = std::chrono::system_clock::now();
    std::time_t ct_c = std::chrono::system_clock::to_time_t(creation_time);
    descss<<blocks_number<<" f "<<std::put_time(std::localtime(&ct_c), "%Y-%m-%d %H:%M:%S")<<'\n';

    for(int block: pos) {
        descss<<block<<" ";
    }

    _sec_mem_driver.write_data(inode_pos[0], _block_size, 0, descss.str());
    
    std::string write_str;
    for(int i = 0 ; i < blocks_number ; i++) {
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
            _sec_mem_driver.write_data(pos[i], _block_size, 0, write_str);
        }
    }
    
    return true;
}

std::string INodeSystem::cat(std::vector<std::string> path) {
    std::vector<int> file_block_stream = _get_block_stream(path, 'f');

    std::string text;
    std::vector<int>::iterator kt;
    for(kt = file_block_stream.begin() ; kt != file_block_stream.end() ; kt++) {
        std::string file_data = _sec_mem_driver.read_block_data(*kt, _block_size);
        text.append(file_data);
    }

    return text;
}

bool INodeSystem::rm(std::vector<std::string> path) {
    std::string name = path.back();
    path.pop_back();
    
    std::vector<int> dir_blocks;
    dir_blocks = _get_block_stream(path, 'd');

    FileDescriptor file;
    bool found = false;
    int i, found_block;
    std::string inode_dir_rep;
    for(int block: dir_blocks) {
        std::string block_data = _sec_mem_driver.read_block_data(block, _block_size);
        size_t end = block_data.find('\0');

        std::string data;
        for(i = 0; i < end; i++) {
            if(block_data[i] != '\n') {
                data += block_data[i];
            }
            else {
                size_t space = data.find(' ');
                std::string fname = data.substr(0, space);
                int fpos = stoi(data.substr(space + 1));
                std::string inode_data = _sec_mem_driver.read_block_data(fpos, _block_size);
                size_t header_end = inode_data.find('\n');
                inode_data = inode_data.substr(0, header_end);

                file = FileDescriptor::from_inode(inode_data, fname, fpos);
                
                if(file.name == name){
                    inode_dir_rep = data;
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

    int dir_str_size = inode_dir_rep.size() ;
    std::string void_line(dir_str_size, '\0');
    _sec_mem_driver.write_data(found_block, _block_size, (i - dir_str_size), void_line);

    _block_manager.free_block(file.pos);
    _block_manager.free_block(file_blocks);
    
    std::string void_block(_block_size, '\0');
    _sec_mem_driver.write_data(file.pos, _block_size, 0, void_block);
    for(int block: file_blocks) {
       _sec_mem_driver.write_data(block, _block_size, 0, void_block);
    }
    return true;
}
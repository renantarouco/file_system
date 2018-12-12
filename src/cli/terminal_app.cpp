#include <cli/terminal_app.h>

void TerminalApp::_trim_l(std::string &text) {
    int count = 0;
    for (std::string::iterator it = text.begin(); *it == ' '; ++it, ++count) {}
    text.erase(text.begin(), text.begin() + count);
}

void TerminalApp::_trim_r(std::string &text) {
    int count = text.size();
    for (std::string::reverse_iterator it = text.rbegin(); *it == ' '; ++it, --count) {}
    text.erase(text.begin() + count, text.end());
}

std::vector<std::string> TerminalApp::_tokenize_command(std::string command) {
    _trim_l(command);
    _trim_r(command);
    std::string token = "";
    std::vector<std::string> command_tkns;
    for (char c : command) {
        if (c == ' ' || c == '\t') {
            if (!token.empty()) {
                command_tkns.push_back(token);
                token.clear();
            }
        }
        else token.push_back(c);
    }
    command_tkns.push_back(token);
    return command_tkns;
}

std::vector<std::string> TerminalApp::_tokenize_path(std::string path_str) {
    std::string dir_name;
    std::vector<std::string> path;
    for (char c : path_str) {
        if (c != '/') dir_name.push_back(c);
        else {
            path.push_back(dir_name);
            dir_name.clear();
        }
    }
    path.push_back(dir_name);
    return path;
}

void TerminalApp::_print_terminal_command_line(bool status) {
    if (status) std::cout << "\033[1;32m";
    else std::cout << "\033[1;31m";
    std::cout << '#' << ' ';
    std::cout << "\033[1;34m" << '/';
    for (std::string dir_name : _working_path) {
        std::cout << dir_name;
        if (dir_name != _working_path.back()) std::cout << '/';
    }
    std::cout << ' ';
    std::cout << "\033[1;37m" << "$ " << "\033[0m";
}

void TerminalApp::init(std::string config_path) {
    std::ifstream config_file(config_path);
    json config_json;
    config_file >> config_json;
    std::string fs_model_str = config_json["model"].get<std::string>();
    int ps = config_json["ps"].get<int>();
    int bs = config_json["bs"].get<int>();
    std::cout << "initializing file system..." << std::endl << std::endl;
    if (fs_model_str == "fat") _fs = new FATSystem(ps, bs);
    else if (fs_model_str == "inode") _fs = new INodeSystem(ps, bs);
    else {
        std::cerr << "unsupported file system model" << std::endl;
        return;
    }
    std::cout << std::left << std::setw(8) << "model" << " ";
    std::cout << std::left << std::setw(8) << "ps" << " ";
    std::cout << std::left << std::setw(8) << "bs" << std::endl;
    std::cout << std::left << std::setw(8) << fs_model_str << " ";
    std::cout << std::left << std::setw(8) << ps << " ";
    std::cout << std::left << std::setw(8) << bs << std::endl << std::endl;
    std::cout << "... done." << std::endl << std::endl;
}

void TerminalApp::exec() {
    bool status = true;
    while(1) {
        _print_terminal_command_line(status);
        std::string command;
        std::getline(std::cin, command);
        std::vector<std::string> command_tkns = _tokenize_command(command);
        if (command_tkns[0] == "exit") return;
        else if (command_tkns[0] == "mkdir") {
            if (command_tkns.size() < 2) {
                std::cout << "usage: mkdir <DIR_NAME>" << std::endl;
                status = false;
            } else {
                std::vector<std::string> path;
                path.insert(path.end(), _working_path.begin(), _working_path.end());
                path.push_back(command_tkns[1]);
                status = _fs->mkdir(path);
            }
        }
        else if (command_tkns[0] == "cd") {
            if (command_tkns.size() < 2) {
                std::cout << "usage: cd <DIR_NAME>" << std::endl;
                status = false;
            } else {
                if (command_tkns[1] == "..") {
                    if (!_working_path.empty()) _working_path.pop_back();
                } else {
                    std::vector<std::string> path;
                    path.insert(path.end(), _working_path.begin(), _working_path.end());
                    std::vector<std::string> rel_path = _tokenize_path(command_tkns[1]);
                    path.insert(path.end(), rel_path.begin(), rel_path.end());
                    if (_fs->cd(path)) {
                        _working_path = path;
                        status = true;
                    } else {
                        status = false;
                    }
                }
            }
        }
        else if (command_tkns[0] == "ls") {
            std::vector<FileDescriptor> files_des = _fs->ls(_working_path);
            std::cout << files_des.size() << std::endl;
            for (FileDescriptor fd : files_des) {
                std::cout << fd.to_str();
            }
        }
        else if (command_tkns[0] == "touch") {
            if (command_tkns.size() < 4) {
                std::cout << "usage: touch <FILE_NAME> <FILE_SIZE> <DATA>" << std::endl;
                status = false;
            } else {
                std::vector<std::string> path;
                path.insert(path.end(), _working_path.begin(), _working_path.end());
                path.push_back(command_tkns[1]);
                status = _fs->touch(path, std::stoi(command_tkns[2]), command_tkns[3]);
            }
        }
        else if (command_tkns[0] == "rm") {}
        else if (command_tkns[0] == "cat") {
            if (command_tkns.size() < 2) {
                std::cout << "usage: cat <FILE_NAME>" << std::endl;
                status = false;
            } else{
                std::vector<std::string> path;
                path.insert(path.end(), _working_path.begin(), _working_path.end());
                path.push_back(command_tkns[1]);
                std::string data;
                data = _fs->cat(path);
                if(data == "") {
                    std::cout<<"File not found!"<<std::endl;
                } else {
                    std::cout<<data<<std::endl;
                }
            }
        }
        else if (command_tkns[0] == "listmap") {
            std::cout << _fs->listmap() << std::endl;
        }
    }
}
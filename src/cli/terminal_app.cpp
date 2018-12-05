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

void TerminalApp::init(std::string config_path) {
    std::ifstream config_file(config_path);
    json config_json;
    config_file >> config_json;
    std::string fs_model_str = config_json["model"].get<std::string>();
    int ps = config_json["ps"].get<int>();
    int bs = config_json["bs"].get<int>();
    if (fs_model_str == "fat") _fs = new FATSystem();
    else if (fs_model_str == "inode") _fs = new INodeSystem();
    else {
        std::cerr << "unsupported file system model" << std::endl;
        return;
    }
    std::cout << "initializing file system..." << std::endl << std::endl;
    std::cout << std::left << std::setw(8) << "model" << " ";
    std::cout << std::left << std::setw(8) << "ps" << " ";
    std::cout << std::left << std::setw(8) << "bs" << std::endl;
    std::cout << std::left << std::setw(8) << fs_model_str << " ";
    std::cout << std::left << std::setw(8) << ps << " ";
    std::cout << std::left << std::setw(8) << bs << std::endl << std::endl;
    _fs->init(ps, bs);
    std::cout << "... done." << std::endl << std::endl;
    _working_dir = "";
}

void TerminalApp::exec() {
    bool success = true;
    while(1) {
        if (success) std::cout << "\033[1;32m";
        else std::cout << "\033[1;31m";
        std::cout << '#' << ' ';
        std::cout << "\033[1;34m" << '/' << _working_dir << ' ';
        std::cout << "\033[1;37m" << "$ " << "\033[0m";
        std::string command;
        std::getline(std::cin, command);
        std::vector<std::string> command_tkns = _tokenize_command(command);
        if (command_tkns[0] == "exit") return;
        else if (command_tkns[0] == "mkdir") {
            std::cout << "FileSystem::create_directory" << std::endl;
            success = true;
        }
        else if (command_tkns[0] == "cd") {
            if (command_tkns.size() < 2) {
                std::cout << "usage: cd <DIR_NAME>" << std::endl;
                success = false;
            } else {
                std::cout << "FileSystem::directory_exists" << std::endl;
                _working_dir.push_back('/');
                _working_dir.append(command_tkns[1]);
                success = true;
            }
        }
        else if (command_tkns[0] == "ls") {
            std::cout << "FileSystem::directory_detail" << std::endl;
            success = false;
        }
        else if (command_tkns[0] == "touch") {}
        else if (command_tkns[0] == "rm") {}
        else if (command_tkns[0] == "cat") {}
        else if (command_tkns[0] == "listmap") {}
    }
}
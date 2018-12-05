#ifndef _TERM_APP_H
#define _TERM_APP_H

#include <file_systems/file_systems.h>

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class TerminalApp {
private:
    FileSystem *_fs;
    std::vector<std::string> _working_path;
    void _trim_l(std::string &);
    void _trim_r(std::string &);
    std::vector<std::string> _tokenize_command(std::string);
    std::vector<std::string> _tokenize_path(std::string);
public:
    void init(std::string);
    void exec();
};

#endif
#include <cli/terminal_app.h>

#include <string>

int main(int argc, char const *argv[])
{
    std::string config_path(argv[1]);
    TerminalApp tapp;
    tapp.init(config_path);
    tapp.exec();
    return 0;
}

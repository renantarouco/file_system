#include <iostream> 
#include <bits/stdc++.h> 
#include <sys/stat.h> 
#include <sys/types.h> 

using namespace std;

int main(int argc, char const *argv[])
{
    struct stat info;
    int statRC = stat("fs", &info);
    if (statRC != 0) {
        if (errno == ENOENT) {
            int mkdirRC = mkdir("fs", 0777);
            if (mkdirRC != 0) {
                cerr << "ERROR: " << strerror(errno) << endl;
            } else {
                cout << "partition directory created." << endl;
            }
        }
        else if (errno == ENOTDIR) cout << "path is not a directory." << endl;
    }
    return 0;
}

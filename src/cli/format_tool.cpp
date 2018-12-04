#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char const *argv[])
{
    fstream sec_mem_stream("sec_memory", ios::in | ios::out | ios::ate);
    cout << sec_mem_stream.tellg() << endl;
    sec_mem_stream.seekg(0);
    if (sec_mem_stream.peek() == 0) {
        cout << "empty disk" << endl;
        // TODO: Initialize disk.
    } else {
        cout << "looking for partition table" << endl;
        // TODO: Open partition table.
    }
    return 0;
}

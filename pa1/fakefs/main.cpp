#include <iostream>
#include <string>
#include <sstream>

#include "filesystem.hpp"

using namespace std;

void parse(const string &str);

static FileSystem fs;

int main(int argc, char** argv) {
    string line, fsname = (argc > 1) ? argv[1] : ".disk.ffs";
    
    fs.load(fsname);
    
    while (getline(cin, line)) {
        parse(line);
    }
}

void parse(const string &str) {
    int fd, val;
    string cmd, sarg1, sarg2;
    istringstream ss(str);
    
    // Ignore blank lines
    if (!(ss >> cmd)) 
        return;

    if (cmd == "mkfs") {
    } else if (cmd == "open") {
    } else if (cmd == "read") {
    } else if (cmd == "write") {
    } else if (cmd == "seek") {
    } else if (cmd == "close") {
    } else if (cmd == "mkdir") {
    } else if (cmd == "rmdir") {
    } else if (cmd == "cd") {
    } else if (cmd == "ls") {
    } else if (cmd == "cat") {
    } else if (cmd == "tree") {
    } else if (cmd == "import") {
    } else if (cmd == "export") {
    }
}

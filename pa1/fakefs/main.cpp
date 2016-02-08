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
        fs.format();
    } else if (cmd == "open") {
        if (ss >> sarg1 >> sarg2) {
            fs.open(sarg1, sarg2);
        } else {
            //error
        }
    } else if (cmd == "read") {
        if (ss >> fd >> val) {
            fs.read(fd, val);
        } else {
            //error
        }
    } else if (cmd == "write") {
        if (ss >> fd >> sarg1) {
            fs.write(fd, sarg1);
        } else {
            //error
        }
    } else if (cmd == "seek") {
        if (ss >> fd >> val) {
            fs.seek(fd, val);
        } else {
            //error
        }
    } else if (cmd == "close") {
        if (ss >> fd) {
            fs.close(fd);
        } else {
            //error
        }
    } else if (cmd == "mkdir") {
        if (ss >> sarg1) {
            fs.mkdir(sarg1);
        } else {
            //error
        }
    } else if (cmd == "rmdir") {
        if (ss >> sarg1) {
            fs.rmdir(sarg1);
        } else {
            //error
        }
    } else if (cmd == "cd") {
        if (ss >> sarg1) {
            fs.chdir(sarg1);
        } else {
            //error
        }
    } else if (cmd == "ls") {
        fs.dir_list();
    } else if (cmd == "cat") {
        if (ss >> sarg1) {
            fs.dump(sarg1);
        } else {
            //error
        }
    } else if (cmd == "tree") {
        fs.dir_tree();
    } else if (cmd == "import") {
        if (ss >> sarg1 >> sarg2) {
            fs.import(sarg1, sarg2);
        } else {
            //error
        }
    } else if (cmd == "export") {
        if (ss >> sarg1 >> sarg2) {
            fs.export(sarg1, sarg2);
        } else {
            //error
        }
    }
}

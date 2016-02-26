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
            cerr << "Bad filename or flag for open." << endl;
        }
    } else if (cmd == "read") {
        if (ss >> fd >> val) {
            fs.read(fd, val);
        } else {
            cerr << "Bad file descriptor for read." << endl;
        }
    } else if (cmd == "write") {
        if (ss >> fd) {
            // Read in the rest of the line to write to the file
            // Also ignore surrounding quotes and spaces
						getline(ss, sarg1);
						size_t strBegin = sarg1.find_first_not_of(" \"");
						size_t strEnd = sarg1.find_last_not_of(" \"");
						sarg1 = sarg1.substr(strBegin, strEnd - strBegin + 1);
						fs.write(fd, sarg1);
        } else {
            cerr << "Bad file descriptor for write." << endl;
        }
    } else if (cmd == "seek") {
        if (ss >> fd >> val) {
            fs.seek(fd, val);
        } else {
            cerr << "Bad arguments for seek." << endl;
        }
    } else if (cmd == "close") {
        if (ss >> fd) {
            fs.close(fd);
        } else {
            cerr << "Bad file descriptor for close." << endl;
        }
    } else if (cmd == "mkdir") {
        if (ss >> sarg1) {
            fs.mkdir(sarg1);
        } else {
            cerr << "Bad filename for mkdir." << endl;
        }
    } else if (cmd == "rmdir") {
        if (ss >> sarg1) {
            fs.rmdir(sarg1);
        } else {
            cerr << "Bad filename for rmdir." << endl;
        }
    } else if (cmd == "cd") {
        if (ss >> sarg1) {
            fs.chdir(sarg1);
        } else {
            cerr << "Bad filename for cd." << endl;
        }
    } else if (cmd == "ls") {
        fs.dir_list();
    } else if (cmd == "cat") {
        if (ss >> sarg1) {
            fs.dump(sarg1);
        } else {
            cerr << "Bad filename for cat." << endl;
        }
    } else if (cmd == "tree") {
        fs.dir_tree();
    } else if (cmd == "import") {
        if (ss >> sarg1 >> sarg2) {
            fs.import_file(sarg1, sarg2);
        } else {
            cerr << "Bad filename for import." << endl;
        }
    } else if (cmd == "export") {
        if (ss >> sarg1 >> sarg2) {
            fs.export_file(sarg1, sarg2);
        } else {
            cerr << "Bad filename for export." << endl;
        }
    }
}

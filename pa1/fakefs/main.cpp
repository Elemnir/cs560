#include <iostream>
#include <string>

#include "shell.hpp"
#include "filesystem.hpp"

int main(int argc, char** argv) {
    std::string line, fsname = (argc > 1) ? argv[1] : ".disk.ffs";
    FileSystem fs(fsname);
    Shell shell(fs);

    while (std::getline(std::cin, line)) {
        shell.parse(line);
    }
}

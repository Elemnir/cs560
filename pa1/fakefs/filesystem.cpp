#include "filesystem.hpp"

using namespace std;

void FileSystem::load(const string &fsname) {
    _fsfile.open(fsname);
    if (_fsfile.fail()) {
        
    }
}

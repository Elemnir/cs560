#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <fstream>
#include <string>

using namespace std;

class FileSystem {
  public:
    FileSystem() {};
    FileSystem(const string& fsname) { load(fsname); }

    void load(const string& fname);

  private:
    fstream _fsfile;
};

#endif // FILESYSTEM_HPP

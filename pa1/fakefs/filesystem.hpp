#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <fstream>
#include <string>

using namespace std;

class FileSystem {
  public:
    FileSystem() {};
    FileSystem(const string& fsname) { load(fsname); }

    void load(const string &fname);
    void format();
    void open(const string &fname);
    void read(int fd, int size);
    void write(int fd, const string &str);
    void seek(int fd, int offset);
    void close(int fd);
    void mkdir(const string &dir);
    void rmdir(const string &dir);
    void chdir(const string &dir);
    void dir_list();
    void dump(const string &fname);
    void dir_tree();
    void import(const string &src, const string &dest);
    void export(const string &dest, const string &src);

  private:
    fstream _fsfile;
};

#endif // FILESYSTEM_HPP

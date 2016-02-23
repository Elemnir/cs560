#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <cstdio>
#include <string>
#include <vector>

using namespace std;

class FileSystem {
  public:
    FileSystem() {};
    FileSystem(const string& fsname) { load(fsname); }

    void load(const string &fname);
    void format();
    void open(const string &fname, const string& mode);
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
    void import_file(const string &src, const string &dest);
    void export_file(const string &dest, const string &src);
  
  protected:
    struct FileHandle {
        FileHandle() : seek(0) {};
        size_t seek;
    };

  private:
    FILE *_fsfile;
    vector<FileHandle> _active;
};

#endif // FILESYSTEM_HPP

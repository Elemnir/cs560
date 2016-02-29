#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#define BLK_SIZE 4096
#define BLK_COUNT 25600
#define INODE_SLOTS 512

typedef unsigned int BLK_NO;
typedef unsigned int BLK_CT;
typedef unsigned int OFFSET;

using namespace std;

class FileSystem {
  public:
    FileSystem() {};
    FileSystem(const string& fsname);

    bool load(const string &fname, ostream &out = cout);
    void format();
    size_t open(const string &fname, const string& mode, ostream &out = cout);
    void read(unsigned fd, size_t size, ostream &out = cout);
    void write(unsigned fd, const string &str, ostream &out = cout);
    void seek(unsigned fd, size_t offset, ostream &out = cout);
    void close(unsigned fd, ostream &out = cout);
    void mkdir(const string &dir);
    void rmdir(const string &dir);
    void chdir(const string &dir);
    void dir_list();
    void dump(const string &fname);
    void dir_tree();
    void import_file(const string &src, const string &dest);
    void export_file(const string &dest, const string &src);
  
  protected:
    
    struct Header {
        Header() : root(0), flh(0), avail(0) {};
        BLK_NO root;
        BLK_NO flh;
        BLK_CT avail;
    };

    struct FreeListNode {
        FreeListNode() : next(0), size(0) {};
        BLK_NO next;
        BLK_CT size;
    };

    struct Inode {
        Inode() : first(0), next(0), blkCt(0), len(0), isDir(false) {};
        BLK_NO first, next, blkNo;
        BLK_CT blkCt;
        size_t len;
        bool isDir;
        BLK_NO block[INODE_SLOTS] = {0};
    };

    struct Block {
        char* operator&() { return text; }
        char& operator[](int i) { return text[i]; }
        char text[BLK_SIZE] = {0};
    };
    
    struct FileHandle {
        FileHandle() : seek(0), read(false), write(false), 
            valid(true) {};
        OFFSET seek;
        Inode inode;
        bool read, write, valid;
    };

  private:

    // Setters and Getters for the Header, Inodes, Blocks, and FreeListNodes
    void    get_header();
    void    set_header();

    template<class T>
    T get(const BLK_NO blk) {
        T rval;
        fsfile.seekg(BLK_SIZE * blk);
        fsfile.read((char*) &rval, sizeof(T));
        return rval;
    }

    template<class T>
    void set(const BLK_NO blk, const T& buf) {
        fsfile.seekp(BLK_SIZE * blk);
        fsfile.write((char*) &buf, sizeof(T));
    }
    
    // Use for selecting and returning blocks from the free set
    BLK_NO allocate_block();
    void   free_block(BLK_NO blk);

    // Directory and File tools
    void init_file(BLK_NO self, BLK_NO parent, bool isDir, const string &fname);
    void map_current_dir();

    // Handle to storage, the FS Header, and the Current Directory
    fstream fsfile;
    Header header;
    Inode cdi;

    // Files that are open, and the contents of the current directory
    map<string, BLK_NO> cdd;
    vector<FileHandle> opened;
};

#endif // FILESYSTEM_HPP

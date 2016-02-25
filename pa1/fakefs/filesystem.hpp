#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <fstream>
#include <map>
#include <string>
#include <vector>

#define BLK_SIZE 4096
#define BLK_COUNT 25600

typedef unsigned int BLK_NO;
typedef unsigned int BLK_CT;
typedef unsigned int OFFSET;

using namespace std;

class FileSystem {
  public:
    FileSystem() {};
    FileSystem(const string& fsname);

    bool load(const string &fname);
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
        Inode() : blkCount(0), len(0), isDir(false), isBig(false) {};
        BLK_CT blkCount;
        size_t len;
        bool isDir, isBig;
        BLK_NO block[512] = {0};
        BLK_NO blkno;
    };

    struct Block {
        char* operator&() { return text; }
        char& operator[](int i) { return text[i]; }
        char text[BLK_SIZE] = {0};
    };
    
    struct FileHandle {
        FileHandle() : seek(0), inode(0), read(false), write(false) {};
        OFFSET seek;
        BLK_NO inode;
        Block curr;
        bool read, write;
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

    // Directory tools
    void init_dir(BLK_NO self, BLK_NO parent);
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

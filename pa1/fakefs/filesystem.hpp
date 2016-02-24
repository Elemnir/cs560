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
        BLK_NO root;
        BLK_NO flh;
        BLK_CT avail;
    };

    struct FreeListNode {
        BLK_NO next;
        BLK_CT size;
    };

    struct Inode {
        BLK_CT blkCount;
        size_t len;
        bool isDir, isBig;
        BLK_NO block[512];
    };

    struct Block {
        char* operator&() { return text; }
        char& operator[](int i) { return text[i]; }
        char text[BLK_SIZE];
    };
    
    struct FileHandle {
        FileHandle() : seek(0) {};
        OFFSET seek;
        BLK_NO inode;
        Block curr;
        bool read, write;
    };

  private:

    // Setters and Getters for the Header, Inodes, Blocks, and FreeListNodes
    void    get_header();
    void    set_header();
    Inode   get_inode(const BLK_NO blk);
    void    set_inode(const BLK_NO blk, const Inode& buf);    
    Block   get_block(const BLK_NO blk);
    void    set_block(const BLK_NO blk, const Block& buf);
    FreeListNode get_freelistnode(const BLK_NO blk);
    void  set_freelistnode(const BLK_NO blk, const FreeListNode& buf);
    
    // Use for selecting and returning blocks from the free set
    BLK_NO allocate_block();
    void   free_block(BLK_NO blk);

    // Call whenever modifying the current directory
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

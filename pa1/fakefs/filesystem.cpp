#include "filesystem.hpp"

using namespace std;

FileSystem::FileSystem(const string &fsname) {
    if (load(fsname)) {
        get_header();
        cdi = get_inode(header.root);
        map_current_dir();
    }
}

bool FileSystem::load(const string &fsname) {
    fsfile.open(fsname, ios_base::in | ios_base::out | ios_base::binary);
    if (!fsfile.is_open()) {
        fsfile.clear();
        fsfile.open(fsname, ios_base::out);
        fsfile.close();
        fsfile.open(fsname, ios_base::in | ios_base::out | ios_base::binary);
        return false;
    }
    return true;
}

void FileSystem::format() {
    Inode root;
    FreeListNode flh;

    // Blank the filesystem
    fsfile.seekp(0);
    for (int i = 0; i < BLK_SIZE * BLK_COUNT; ++i) {
        fsfile.put('\0');
    }
    fsfile.seekp(0);

    // Initialize the header and freelist
    get_header();
    header.flh = sizeof(Header) / BLK_SIZE + 1;
    flh = get_freelistnode(header.flh);
    flh.size = header.avail = BLK_COUNT - header.flh;
    flh.next = 0;
    set_freelistnode(header.flh, flh);
    
    // Initialize the root directory Inode and first Block
    header.root = allocate_block();
    init_dir(header.root, header.root);
    set_header();
    cdi = get_inode(header.root);
    map_current_dir();
}

void FileSystem::open(const string &fname, const string& mode) {
}

void FileSystem::read(int fd, int size) {
}

void FileSystem::write(int fd, const string &str) {
}

void FileSystem::seek(int fd, int offset) {
}

void FileSystem::close(int fd) {
}

void FileSystem::mkdir(const string &dir) {
}

void FileSystem::rmdir(const string &dir) {
}

void FileSystem::chdir(const string &dir) {
}

void FileSystem::dir_list() {
}

void FileSystem::dump(const string &fname) {
}

void FileSystem::dir_tree() {
}

void FileSystem::import_file(const string &src, const string &dest) {
}

void FileSystem::export_file(const string &dest, const string &src) {
}

/* Internals */
void FileSystem::get_header() {
    fsfile.seekg(0);
    fsfile.read((char*) &header, sizeof(Header));
}

void FileSystem::set_header() {
    fsfile.seekp(0);
    fsfile.write((char*) &header, sizeof(Header));
}

FileSystem::Inode FileSystem::get_inode(const BLK_NO blk) {
    Inode rval;
    fsfile.seekg(BLK_SIZE * blk);
    fsfile.read((char*) &rval, sizeof(Inode));
    return rval;
}

void FileSystem::set_inode(const BLK_NO blk, const FileSystem::Inode& buf) {
    fsfile.seekp(BLK_SIZE * blk);
    fsfile.write((char*) &buf, sizeof(Inode));
}

FileSystem::Block FileSystem::get_block(const BLK_NO blk) {
    Block rval;
    fsfile.seekg(BLK_SIZE * blk);
    fsfile.read((char*) &rval, sizeof(Block));
    return rval;
}

void FileSystem::set_block(const BLK_NO blk, const FileSystem::Block& buf) {
    fsfile.seekp(BLK_SIZE * blk);
    fsfile.write((char*) &buf, sizeof(Block));
}

FileSystem::FreeListNode FileSystem::get_freelistnode(const BLK_NO blk) {
    FreeListNode rval;
    fsfile.seekg(BLK_SIZE * blk);
    fsfile.read((char*) &rval, sizeof(FreeListNode));
    return rval;
}

void FileSystem::set_freelistnode(const BLK_NO blk, 
        const FileSystem::FreeListNode &buf) {
    fsfile.seekp(BLK_SIZE * blk);
    fsfile.write((char*) &buf, sizeof(FreeListNode));

}

BLK_NO FileSystem::allocate_block() {
    BLK_NO rval = 0;
    if (header.flh == 0) return rval;

    FreeListNode node = get_freelistnode(header.flh);
    if (node.size == 1) {
        rval = header.flh;
        header.flh = node.next;
        set_header();
    } else { 
        node.size--;
        rval = header.flh + node.size;
        set_freelistnode(header.flh, node);
    }
    return rval;
}

void FileSystem::free_block(BLK_NO blk) {
    FreeListNode node;
    
    node.next = header.flh;
    node.size = 1;
    header.flh = blk;
    
    set_header();
    set_freelistnode(blk, node);
}

void FileSystem::init_dir(BLK_NO self, BLK_NO parent) {
    Inode node = get_inode(self);
    node.isDir = true;
    node.blkCount = 1;
    node.block[0] = allocate_block();

    Block blk = get_block(node.block[0]);
    node.len = sprintf(&blk, "%x .\n%x ..\n", self, parent);

    set_inode(self, node);
    set_block(node.block[0], blk);
}

void FileSystem::map_current_dir() {
}

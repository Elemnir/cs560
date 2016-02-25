#include "filesystem.hpp"

#include <iostream>

using namespace std;

FileSystem::FileSystem(const string &fsname) {
    if (load(fsname)) { 
        // If we had to create the file, all of this is undefined
        get_header();
        cdi = get<Inode>(header.root);
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
        cout << "Filesystem created, please run \"mkfs\"\n";
        return false;
    }
    cout << "Filesystem loaded\n";
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
    flh = get<FreeListNode>(header.flh);
    flh.size = header.avail = BLK_COUNT - header.flh;
    flh.next = 0;
    set<FreeListNode>(header.flh, flh);
    
    // Initialize the root directory Inode and first Block
    header.root = allocate_block();
    init_dir(header.root, header.root);
    set_header();
    cdi = get<Inode>(header.root);
    map_current_dir();
    cout << "Filesystem initialized\n";
}

void FileSystem::open(const string &fname, const string& mode) {
    if (cdd.find(fname) != cdd.end()) {
        // The file exists
    } else {
        // Gotta make it
    }
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
    Inode node;
    if (cdd.find(dir) != cdd.end()) {
        node = get<Inode>(cdd[dir]);
        if (node.isDir) {
            cdi = node;
            map_current_dir();
        } else {
            // Not a dir error
        }
    } else {
        // Directory doesn't exist
    }
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

BLK_NO FileSystem::allocate_block() {
    BLK_NO rval = 0;
    if (header.flh == 0) return rval;

    FreeListNode node = get<FreeListNode>(header.flh);
    if (node.size == 1) {
        rval = header.flh;
        header.flh = node.next;
    } else { 
        node.size--;
        rval = header.flh + node.size;
        set<FreeListNode>(header.flh, node);
    }
    header.avail--;
    set_header();

    return rval;
}

void FileSystem::free_block(BLK_NO blk) {
    FreeListNode node;
    
    node.next = header.flh;
    node.size = 1;
    header.flh = blk;
    header.avail++;
    
    set_header();
    set<FreeListNode>(blk, node);
}

void FileSystem::init_dir(BLK_NO self, BLK_NO parent) {
    Inode node = get<Inode>(self);
    node.isDir = true;
    node.blkCount = 1;
    node.block[0] = allocate_block();

    Block blk = get<Block>(node.block[0]);
    node.len = sprintf(&blk, "%x .\n%x ..\n", self, parent);

    set<Inode>(self, node);
    set<Block>(node.block[0], blk);
}

void FileSystem::map_current_dir() {
    
}

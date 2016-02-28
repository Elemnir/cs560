#include "filesystem.hpp"

#include <cstdio>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

FileSystem::FileSystem(const string &fsname) {
    load(fsname);
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
    get_header();
    cdi = get<Inode>(header.root);
    map_current_dir();
    
    /*
    printf("Header  (root: %d, flh: %d, avail: %d)\n", 
            header.root, header.flh, header.avail);
    printf("CDI     (blkno: %d, blks: %d, inodes: %d)\n", 
            cdi.blkNo, cdi.blkCount, cdi.inodeCount);
    auto t = get<Block>(cdi.block[0]);
    printf("Block (%s)\n", &(t.text));
    */
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
    init_file(header.root, header.root, true, "/");
    set_header();
    cdi = get<Inode>(header.root);
    map_current_dir();
    cout << "Filesystem initialized\n";
}

void FileSystem::open(const string &fname, const string& mode) {
    size_t i;
    FileHandle fh;
    if (mode != "w" && mode != "r") {
        cout << "Invalid mode: " << mode << endl;
        return;
    }
    fh.write = mode == "w";
    fh.read  = mode == "r";

    if (cdd.find(fname) != cdd.end()) {
        // Open the file if it exists
        fh.inode = get<Inode>(cdd[fname]);
        fh.currNo = fh.inode.block[0];
    } else if (mode == "w") {
        // Make the file if it doesn't exist
        BLK_NO blk = allocate_block();
        init_file(blk, cdi.blkNo, false, fname);
        map_current_dir();
        fh.inode = get<Inode>(blk);
        fh.currNo = fh.inode.block[0];
    } else {
        cout << "File not found.\n";
        return;
    }
    
    // Find a file descriptor and store the FileHandle in opened
    for (i = 0; i < opened.size(); i++) {
        if (!opened[i].valid) {
            opened[i] = fh;
            break;
        }
    }
    if (i == opened.size()) {
        opened.push_back(fh);
    }

    cout << "SUCCESS, fd=" << i << "\n";
}

//inod = iblk / INODE_SLOTS; // Calculate the Inode index in the chain
//index = iblk % INODE_SLOTS; // Calculate Block index within the Inode
//pos = offset % BLK_SIZE; // Calculate the Offset within the block
 
void FileSystem::read(unsigned fd, size_t size) {
  
}

void FileSystem::write(unsigned fd, const string &str) {
    size_t size = str.size();
    Inode fhead;
    Block curr;
    if (opened.size() <= fd || !opened[fd].valid) {
        cout << "Bad file descriptor " << fd << "\n";
    } else {
        fhead = get<Inode>(opened[fd].inode.first);
    }
}

void FileSystem::seek(unsigned fd, size_t offset) {
    size_t iblk, inod;
    Inode head, itmp;
    if (opened.size() <= fd || !opened[fd].valid) {
        cout << "Bad file descriptor " << fd << "\n";
    } else {
        head = get<Inode>(opened[fd].inode.first);
        if (head.len <= offset) {
            offset = head.len-1;
        }

        iblk = offset / BLK_SIZE; // Calculate the Global Block Index
        inod = iblk / INODE_SLOTS;
        itmp = head;
        while (inod-- > 0) {
            itmp = get<Inode>(itmp.next);
        }
        opened[fd].inode = itmp;
        opened[fd].currNo = iblk;
        opened[fd].seek = offset;
    }
}

void FileSystem::close(unsigned fd) {
    if (opened.size() <= fd || !opened[fd].valid) {
        cout << "Bad file descriptor " << fd << "\n";
    } else {
        opened[fd].valid = false;
    }
}

void FileSystem::mkdir(const string &dir) {
    init_file(allocate_block(), cdi.blkNo, true, dir);
    map_current_dir();
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
            cout << dir << " is not a directory\n";
        }
    } else {
        cout << "file not found\n";
    }
}

void FileSystem::dir_list() {
    for (const auto& i : cdd) {
        cout << i.first << "\n";
    }
}

void FileSystem::dump(const string &fname) {
    Inode head, node;
    Block blk;
    BLK_NO next;
    if (cdd.find(fname) == cdd.end()) {
        cout << "File not found: " << fname << "\n";
    } else if (get<Inode>(cdd[fname]).isDir) {
        cout << fname << " is a directory.\n";
    } else {
        node = get<Inode>(cdd[fname]);
        for (size_t rem = node.len, i = 0; rem > 0; rem -= BLK_SIZE) {
        }

    }
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

void FileSystem::init_file(BLK_NO self, BLK_NO parent, bool isDir, const string &name) {
    char buf[1024];
    Block blk;
    Inode node = get<Inode>(self);
    Inode par  = get<Inode>(parent);
    
    node.blkCt = 1;
    node.block[0] = allocate_block();
    node.blkNo = node.first = self;
    
    if (isDir) {
        node.isDir = true;
        blk = get<Block>(node.block[0]);
        node.len = sprintf(&blk, "%x .\n%x ..\n", self, parent);
        set<Block>(node.block[0], blk);
    }

    // Add the file to the current directory
    if (self != parent) {
        size_t len = snprintf(buf, 1024, "%x %s\n", self, name.c_str());
        BLK_CT i = (par.len + len) / BLK_SIZE;
        if (i != (par.len / BLK_SIZE)) {
            par.len += BLK_SIZE - (par.len % BLK_SIZE);
            par.block[i] = allocate_block();
            par.blkCt++;
        }
        blk = get<Block>(par.block[i]);
        par.len += sprintf(&blk + (par.len % BLK_SIZE), "%s", buf);
        set<Block>(par.block[i], blk);
        set<Inode>(parent, par);
    }
    set<Inode>(self, node);
}

void FileSystem::map_current_dir() {
    stringstream ss;
    BLK_NO ptr;
    string name;
    
    cdd.clear();
    for (BLK_CT i = 0; i < cdi.blkCt; i++) {
        ss.clear();
        ss.str(get<Block>(cdi.block[i]).text);
        while (ss >> hex >> ptr >> name) {
            cdd[name] = ptr;
        }
    }
}

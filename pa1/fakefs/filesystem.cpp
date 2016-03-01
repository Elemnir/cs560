#include "filesystem.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

FileSystem::FileSystem(const string &fsname) {
    load(fsname);
}

bool FileSystem::load(const string &fsname, ostream &out) {
    fsfile.open(fsname, ios_base::in | ios_base::out | ios_base::binary);
    if (!fsfile.is_open()) {
        fsfile.clear();
        fsfile.open(fsname, ios_base::out);
        fsfile.close();
        fsfile.open(fsname, ios_base::in | ios_base::out | ios_base::binary);
        out << "Filesystem created, please run \"mkfs\"\n";
        return false;
    }
    
    out << "Filesystem loaded\n";
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

size_t FileSystem::open(const string &fname, const string& mode, ostream &out) {
    size_t i;
    FileHandle fh;
    if (mode != "w" && mode != "r") {
        out << "Invalid mode: " << mode << endl;
        return 0;
    }
    fh.write = mode == "w";
    fh.read  = mode == "r";

    if (cdd.find(fname) != cdd.end()) {
        // Open the file if it exists
        fh.inode = get<Inode>(cdd[fname]);
    } else if (mode == "w") {
        // Make the file if it doesn't exist
        BLK_NO blk = allocate_block();
        init_file(blk, cdi.blkNo, false, fname);
        map_current_dir();
        fh.inode = get<Inode>(blk);
    } else {
        out << "File not found.\n";
        return 0;
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

    out << "SUCCESS, fd=" << i << "\n";
    return i;
}

//iblk = seek / BLK_SIZE;
//inod = iblk / INODE_SLOTS; // Calculate the Inode index in the chain
//index = iblk % INODE_SLOTS; // Calculate Block index within the Inode
//pos = seek % BLK_SIZE; // Calculate the Offset within the block
 
void FileSystem::read(unsigned fd, size_t size, ostream &out) {
    size_t len, iblk, pos, index, pre;
    if (opened.size() <= fd || !opened[fd].valid) {
        out << "Bad file descriptor " << fd << "\n";
    } else if (!opened[fd].read) {
        out << "File not open for reading.\n";
    } else {
        len = get<Inode>(opened[fd].inode.first).len;
        if (size > len) {
            size = len;
        }
        iblk = opened[fd].seek / BLK_SIZE;
        pos = opened[fd].seek % BLK_SIZE;
        index = iblk % INODE_SLOTS;
        pre = (BLK_SIZE - pos > size) ? BLK_SIZE - pos : size;
        
        out.write(&(get<Block>(opened[fd].inode.block[index]).text[pos]), pre);
        for (long rem = size - pre, i = index + 1; rem > 0; rem -= BLK_SIZE) {
            out.write(get<Block>(opened[fd].inode.block[i++]).text, 
                    (rem > BLK_SIZE) ? BLK_SIZE : rem);
            if (i == INODE_SLOTS) {
                opened[fd].inode = get<Inode>(opened[fd].inode.next);
                i = 0;
            }
        }
        opened[fd].seek += size;
    }
}

void FileSystem::write(unsigned fd, const string &str, ostream &out) {
    size_t len, iblk, pos, index, pre;
    Block blk;
    Inode head;
    BLK_NO num;
    if (opened.size() <= fd || !opened[fd].valid) {
        out << "Bad file descriptor " << fd << "\n";
    } else if (!opened[fd].write) {
        out << "File not open for writing.\n";
    } else {
        len = str.size();
        iblk = opened[fd].seek / BLK_SIZE;
        pos = opened[fd].seek % BLK_SIZE;
        index = iblk % INODE_SLOTS;
        pre = (BLK_SIZE - pos < len) ? BLK_SIZE - pos : len;
        
        head = get<Inode>(opened[fd].inode.first);
        if (len + opened[fd].seek > head.len) {
            head.len = len + opened[fd].seek;
            set<Inode>(head.blkNo, head);
        }
        
        if (index >= opened[fd].inode.blkCt) {
            opened[fd].inode = get<Inode>(opened[fd].inode.blkNo);
            opened[fd].inode.block[index] = allocate_block();
            opened[fd].inode.blkCt++;
            set<Inode>(opened[fd].inode.blkNo, opened[fd].inode);
        }

        blk = get<Block>(opened[fd].inode.block[index]);
        strncpy(&(blk.text[pos]), str.c_str(), pre);
        set<Block>(opened[fd].inode.block[index], blk);

        for (long rem = len - pre, i = index + 1; rem > 0; rem -= BLK_SIZE) {
            if (i < INODE_SLOTS && i >= opened[fd].inode.blkCt) {
                opened[fd].inode.block[i] = allocate_block();
                opened[fd].inode.blkCt++;
                set<Inode>(opened[fd].inode.blkNo, opened[fd].inode);
            }

            blk = get<Block>(opened[fd].inode.block[i]);
            strncpy(blk.text, str.c_str() + (len - rem), (rem > BLK_SIZE) ? BLK_SIZE : rem);
            set<Block>(opened[fd].inode.block[i++], blk);
            
            if (i == INODE_SLOTS) {
                if (opened[fd].inode.next == 0) {
                    BLK_NO first = opened[fd].inode.first;
                    opened[fd].inode.next = num = allocate_block();
                    set<Inode>(opened[fd].inode.blkNo, opened[fd].inode);
                    opened[fd].inode = get<Inode>(opened[fd].inode.next);
                    opened[fd].inode.blkNo = num;
                    opened[fd].inode.first = first;
                    set<Inode>(num, opened[fd].inode);
                } else {
                    opened[fd].inode = get<Inode>(opened[fd].inode.next);
                }
                i = 0;
            }
        }
        opened[fd].seek += len;
        out << "SUCCESS seek=" << opened[fd].seek << "\n";
    }
}

void FileSystem::seek(unsigned fd, size_t offset, ostream &out) {
    long iblk, inod;
    Inode head, itmp;
    if (opened.size() <= fd || !opened[fd].valid) {
        out << "Bad file descriptor " << fd << "\n";
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
        opened[fd].seek = offset;
        out << "SUCCESS seek=" << opened[fd].seek << "\n";
    }
}

void FileSystem::close(unsigned fd, ostream &out) {
    if (opened.size() <= fd || !opened[fd].valid) {
        out << "Bad file descriptor " << fd << "\n";
    } else {
        opened[fd].valid = false;
        out << fd << " closed\n";
    }
}

void FileSystem::mkdir(const string &dir) {
    init_file(allocate_block(), cdi.blkNo, true, dir);
    map_current_dir();
}

void FileSystem::rmdir(const string &dir) {
    Inode node;
    if (cdd.find(dir) == cdd.end()) {
        cout << "File not found\n";
    } else {
        node = get<Inode>(cdd[dir]); 
        if (!node.isDir) {
            cout << "Not a directory\n";
        } else if (node.len > 8) { // Length of a empty directory
            cout << "Directory not empty\n";
        } else {
            cout << "Removing " << dir << endl;
            free_block(node.block[0]);
            free_block(node.blkNo);
            cdd.erase(dir);
            store_current_dir();
            map_current_dir();
        }
    }
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
    Inode node;
    if (cdd.find(fname) == cdd.end()) {
        cout << "File not found: " << fname << "\n";
    } else if (get<Inode>(cdd[fname]).isDir) {
        cout << fname << " is a directory.\n";
    } else {
        node = get<Inode>(cdd[fname]);
        for (long rem = node.len, i = 0; rem > 0; rem -= BLK_SIZE) {
            cout.write(get<Block>(node.block[i++]).text, (rem > BLK_SIZE) ? BLK_SIZE : rem);
            if (i == INODE_SLOTS) {
                node = get<Inode>(node.next);
                i = 0;
            }
        }
    }
}

void FileSystem::dir_tree() {
    static int depth = 0;
    auto cdd_cur = cdd;
    for (const auto& i: cdd_cur) {
        if (i.first == "." || i.first == "..") {
            continue;
        }
        cout << string(depth * 4, ' ') << i.first << "\n";
        if (get<Inode>(i.second).isDir) {
            depth += 1;
            chdir(i.first);
            dir_tree();
            chdir("..");
            depth -= 1;
        }
    }
}

void FileSystem::import_file(const string &src, const string &dest) {
    ifstream ext(src, ifstream::binary);
    stringstream ss;
    size_t fd; char buf[BLK_SIZE + 1] = {0};
    if (ext.fail()) {
        cout << "Could not open " << src << " on external filesystem\n";
    } else {
        fd = open(dest, "w", ss);
        while (ext.read(buf, BLK_SIZE)) {
            ss.str(""); ss.clear();             
            write(fd, buf, ss);
            fill(buf, buf+BLK_SIZE, 0);
        }
        ss.str(""); ss.clear();             
        write(fd, buf, ss);
        close(fd, ss);
    }
}

void FileSystem::export_file(const string &src, const string &dest) {
    ofstream ext(dest);
    stringstream ss;
    size_t fd; long rem;
    if (cdd.find(src) == cdd.end()) {
        cout << "File " << src << " not found in local filesystem\n";
    } else if (ext.fail()) {
        cout << "Could not open " << dest << " on external filesystem\n";
    } else {
        rem = get<Inode>(cdd[src]).len;
        fd = open(src, "r", ss);
        while (rem > 0) {
            ss.str(""); ss.clear();
            read(fd, (rem > BLK_SIZE) ? BLK_SIZE : rem, ss);
            ext.write(ss.str().c_str(), ss.str().size());
            rem -= BLK_SIZE;
        }
        close(fd, ss);
    }
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
    if (header.flh == 0) {
        cout << "Out of nodes!" << endl;
        return rval;
    }

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
        node.len = sprintf(&blk, "%x ..\n", parent);
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

void FileSystem::store_current_dir() {
    stringstream ss;
    string block, line;
    Block blk; size_t index = 0;

    for (const auto &i : cdd) {
        ss.clear(); ss.str("");
        ss << hex << i.second << ' ' << i.first << '\n';
        if (line.size() + ss.str().size() > BLK_SIZE) {
            if (index > cdi.blkCt) {
                cdi.block[index] = allocate_block(); cdi.blkCt++;
                set<Inode>(cdi.blkNo, cdi);
            }
            blk = get<Block>(cdi.block[index]);
            strncpy(blk.text, line.c_str(), line.size());
            set<Block>(cdi.block[index++], blk);
            line = "";
        } 
        line += ss.str();
    }

    if (line.size() > 0) {    
        if (index > cdi.blkCt) {
            cdi.block[index] = allocate_block(); cdi.blkCt++;
            set<Inode>(cdi.blkNo, cdi);
        }
        blk = get<Block>(cdi.block[index]);
        /* Blank the block first */
        for(int i = 0; i < BLK_SIZE; i++) {
            blk[i] = 0;
        }
        strncpy(blk.text, line.c_str(), line.size());
        set<Block>(cdi.block[index], blk);
    }
}

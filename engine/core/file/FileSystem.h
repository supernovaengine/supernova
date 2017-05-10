
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>

class FileSystem {
public:
    virtual ~FileSystem();
    unsigned int read8();
    unsigned int read16();
    unsigned int read32();
    virtual int eof() = 0;
    virtual unsigned int read(unsigned char *aDst, unsigned int aBytes) = 0;
    virtual unsigned int length() = 0;
    virtual void seek(int aOffset) = 0;
    virtual unsigned int pos() = 0;
    virtual FILE * getFilePtr() { return 0; }
    virtual unsigned char * getMemPtr() { return 0; }
};


#endif


#ifndef FILE_H
#define FILE_H

#include <stdio.h>
#include <string>

class File {
public:
    virtual ~File();
    unsigned int read8();
    unsigned int read16();
    unsigned int read32();
    std::string readString();
    virtual int eof() = 0;
    virtual unsigned int read(unsigned char *aDst, unsigned int aBytes) = 0;
    virtual unsigned int length() = 0;
    virtual void seek(int aOffset) = 0;
    virtual unsigned int pos() = 0;
};


#endif

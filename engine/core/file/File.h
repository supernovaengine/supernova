
#ifndef FILE_H
#define FILE_H

#include "FileSystem.h"

class File: public FileSystem {
protected:
    FILE *fileHandle;

public:

    File();
    File(FILE *fp);
    virtual ~File();

    virtual int eof();
    virtual unsigned int read(unsigned char *aDst, unsigned int aBytes);
    virtual unsigned int length();
    virtual void seek(int aOffset);
    virtual unsigned int pos();
    unsigned int open(const char *aFilename);
    virtual FILE * getFilePtr();
};


#endif

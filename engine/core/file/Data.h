
#ifndef DATA_H
#define DATA_H

#include "FileSystem.h"
#include "File.h"

class Data: public FileSystem {

protected:
    unsigned char *dataPtr;
    unsigned int dataLength;
    unsigned int offset;
    bool dataOwned;

public:
    Data();
    virtual ~Data();

    virtual int eof();
    virtual unsigned int read(unsigned char *aDst, unsigned int aBytes);
    virtual unsigned int length();
    virtual void seek(int aOffset);
    virtual unsigned int pos();
    virtual unsigned char * getMemPtr();

    unsigned int open(unsigned char *aData, unsigned int aDataLength, bool aCopy=false, bool aTakeOwnership=true);
    unsigned int open(const char *aFilename);
    unsigned int open(File *aFile);
};


#endif

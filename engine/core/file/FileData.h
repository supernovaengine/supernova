
#ifndef FILEDATA_H
#define FILEDATA_H

#include "File.h"
#include "FileHandle.h"

namespace Supernova {

    class FileData: public File {

    protected:
        unsigned char *dataPtr;
        unsigned int dataLength;
        unsigned int offset;
        bool dataOwned;

    public:
        FileData();
        FileData(unsigned char *aData, unsigned int aDataLength, bool aCopy=false, bool aTakeOwnership=true);
        FileData(const char *aFilename);
        virtual ~FileData();

        virtual int eof();
        virtual unsigned int read(unsigned char *aDst, unsigned int aBytes);
        virtual unsigned int write(unsigned char *aSrc, unsigned int aBytes);
        virtual unsigned int length();
        virtual void seek(int aOffset);
        virtual unsigned int pos();
        virtual unsigned char * getMemPtr();

        unsigned int open(unsigned char *aData, unsigned int aDataLength, bool aCopy=false, bool aTakeOwnership=true);
        unsigned int open(const char *aFilename);
        unsigned int open(FileHandle *aFile);
    };
    
}


#endif

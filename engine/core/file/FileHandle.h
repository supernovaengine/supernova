
#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include "File.h"

namespace Supernova {

    class FileHandle: public File {
    protected:
        FILE *fileHandle;

    public:

        FileHandle();
        FileHandle(FILE *fp);
        FileHandle(const char *aFilename, bool write = false);
        virtual ~FileHandle();

        virtual int eof();
        virtual unsigned int read(unsigned char *aDst, unsigned int aBytes);
        virtual unsigned int write(unsigned char *aSrc, unsigned int aBytes);
        virtual unsigned int length();
        virtual void seek(int aOffset);
        virtual unsigned int pos();
        unsigned int open(const char *aFilename, bool write = false);
        virtual FILE * getFilePtr();
    };
    
}


#endif

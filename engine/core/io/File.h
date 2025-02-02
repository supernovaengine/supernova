//
// Inspired by work of Jari Komppa in SoLoud audio engine
// https://sol.gfxile.net/soloud/file.html
// Modified by Eduardo Doria.
//

//
// (c) 2024 Eduardo Doria.
//

#ifndef FILE_H
#define FILE_H

#include "FileData.h"

namespace Supernova {

    class SUPERNOVA_API File: public FileData {
    protected:
        FILE *fileHandle;

    public:

        File();
        File(FILE *fp);
        File(const char *aFilename, bool write = false);

        File(const File& f);
        File& operator = ( const File& f );

        virtual ~File();

        virtual int eof();
        virtual unsigned int read(unsigned char *aDst, unsigned int aBytes);
        virtual unsigned int write(unsigned char *aSrc, unsigned int aBytes);
        virtual unsigned int length();
        virtual void seek(int aOffset);
        virtual unsigned int pos();
        virtual FILE * getFilePtr();

        unsigned int open(const char *aFilename, bool write = false);
        void flush();
        void close();
    };
    
}


#endif


#ifndef FILEDATA_H
#define FILEDATA_H

#include <stdio.h>
#include <string>

namespace Supernova {

    class FileData {

    protected:
        static bool beginWith(std::string path, std::string prefix);
        static std::string simplifyPath(std::string path);

    public:
        virtual ~FileData();

        static FileData* newFile(bool useHandle = false);
        static FileData* newFile(const char *aFilename, bool useHandle = false);

        static char getDirSeparator();
        static std::string getBaseDir(std::string filepath);
        static std::string getFilePathExtension(const std::string &filepath);
        static std::string getSystemPath(std::string path);

        unsigned int read8();
        unsigned int read16();
        unsigned int read32();
        std::string readString(int aOffset = 0);
        unsigned int writeString(std::string s);

        virtual int eof() = 0;
        virtual unsigned int read(unsigned char *aDst, unsigned int aBytes) = 0;
        virtual unsigned int write(unsigned char *aSrc, unsigned int aBytes) = 0;
        virtual unsigned int length() = 0;
        virtual void seek(int aOffset) = 0;
        virtual unsigned int pos() = 0;
    };
    
}


#endif

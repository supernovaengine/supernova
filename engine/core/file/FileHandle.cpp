#include "FileHandle.h"

#include "system/System.h"

using namespace Supernova;

FileHandle::FileHandle(){
    fileHandle = NULL;
}

FileHandle::FileHandle(FILE *fp) {
    this->fileHandle = fp;
}

FileHandle::FileHandle(const char *aFilename){
    open(aFilename);
}

FileHandle::~FileHandle() {
    if (fileHandle)
        fclose(fileHandle);
}

unsigned int FileHandle::read(unsigned char *aDst, unsigned int aBytes) {
    return (unsigned int)fread(aDst, 1, aBytes, fileHandle);
}

unsigned int FileHandle::length() {
    int pos = ftell(fileHandle);
    fseek(fileHandle, 0, SEEK_END);
    int len = ftell(fileHandle);
    fseek(fileHandle, pos, SEEK_SET);
    return len;
}

void FileHandle::seek(int aOffset) {
    fseek(fileHandle, aOffset, SEEK_SET);
}

unsigned int FileHandle::pos() {
    return ftell(fileHandle);
}

FILE *FileHandle::getFilePtr() {
    return fileHandle;
}

unsigned int FileHandle::open(const char *aFilename){
    if (!aFilename)
        return 1;
    fileHandle = System::instance().platformFopen(aFilename, "rb");
    if (!fileHandle)
        return 2;
    return 0;
}

int FileHandle::eof() {
    return feof(fileHandle);
}

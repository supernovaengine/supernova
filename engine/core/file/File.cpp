#include "File.h"

#include "FileUtilsPlatform.h"


File::File(){
    fileHandle = NULL;
}

File::File(FILE *fp) {
    this->fileHandle = fp;
}

File::~File() {
    if (fileHandle)
        fclose(fileHandle);
}

unsigned int File::read(unsigned char *aDst, unsigned int aBytes) {
    return (unsigned int)fread(aDst, 1, aBytes, fileHandle);
}

unsigned int File::length() {
    int pos = ftell(fileHandle);
    fseek(fileHandle, 0, SEEK_END);
    int len = ftell(fileHandle);
    fseek(fileHandle, pos, SEEK_SET);
    return len;
}

void File::seek(int aOffset) {
    fseek(fileHandle, aOffset, SEEK_SET);
}

unsigned int File::pos() {
    return ftell(fileHandle);
}

FILE *File::getFilePtr() {
    return fileHandle;
}

unsigned int File::open(const char *aFilename){
    if (!aFilename)
        return 1;
    fileHandle = FileUtilsPlatform::instance().platformFopen(aFilename, "rb");
    if (!fileHandle)
        return 2;
    return 0;
}

int File::eof() {
    return feof(fileHandle);
}

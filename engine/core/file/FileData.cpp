#include "FileData.h"

#include <string.h>

FileData::FileData() {
    dataPtr = NULL;
    dataLength = 0;
    offset = 0;
    dataOwned = false;
}

FileData::FileData(unsigned char *aData, unsigned int aDataLength, bool aCopy, bool aTakeOwnership): FileData(){
    open(aData, aDataLength, aCopy, aTakeOwnership);
}

FileData::~FileData() {
    if (dataOwned)
        delete[] dataPtr;
}

unsigned int FileData::read(unsigned char *aDst, unsigned int aBytes) {
    if (offset + aBytes >= dataLength)
        aBytes = dataLength - offset;

    memcpy(aDst, dataPtr + offset, aBytes);
    offset += aBytes;

    return aBytes;
}

unsigned int FileData::length() {
    return dataLength;
}

void FileData::seek(int aOffset) {
    if (aOffset >= 0)
        offset = aOffset;
    else
        offset = dataLength + aOffset;
    if (offset > dataLength-1)
        offset = dataLength-1;
}

unsigned int FileData::pos() {
    return offset;
}

unsigned char * FileData::getMemPtr() {
    return dataPtr;
}

unsigned int FileData::open(unsigned char *aData, unsigned int aDataLength, bool aCopy, bool aTakeOwnership) {
    if (aData == NULL || aDataLength == 0)
        return 1;

    if (dataOwned)
        delete[] dataPtr;
    dataPtr = 0;
    offset = 0;

    dataLength = aDataLength;

    if (aCopy)
    {
        dataOwned = true;
        dataPtr = new unsigned char[aDataLength];
        if (dataPtr == NULL)
            return 3;
        memcpy(dataPtr, aData, aDataLength);
        return 0;
    }

    dataPtr = aData;
    dataOwned = aTakeOwnership;
    return 0;
}

unsigned int FileData::open(const char *aFile) {
    if (!aFile)
        return 1;
    if (dataOwned)
        delete[] dataPtr;
    dataPtr = 0;
    offset = 0;

    FileHandle df;
    int res = df.open(aFile);
    if (res != 0)
        return res;

    dataLength = df.length();
    dataPtr = new unsigned char[dataLength];
    if (dataPtr == NULL)
        return 3;
    df.read(dataPtr, dataLength);
    dataOwned = true;
    return 0;
}

unsigned int FileData::open(FileHandle *aFile) {
    if (dataOwned)
        delete[] dataPtr;
    dataPtr = 0;
    offset = 0;

    dataLength = aFile->length();
    dataPtr = new unsigned char[dataLength];
    if (dataPtr == NULL)
        return 3;
    aFile->read(dataPtr, dataLength);
    dataOwned = true;
    return 0;
}

int FileData::eof() {
    if (offset >= dataLength)
        return 1;
    return 0;
}
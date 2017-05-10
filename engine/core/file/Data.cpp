#include "Data.h"

#include <string.h>

Data::Data() {
    dataPtr = NULL;
    dataLength = NULL;
    offset = NULL;
    dataOwned = false;
}

Data::~Data() {
    if (dataOwned)
        delete[] dataPtr;
}

unsigned int Data::read(unsigned char *aDst, unsigned int aBytes) {
    if (offset + aBytes >= dataLength)
        aBytes = dataLength - offset;

    memcpy(aDst, dataPtr + offset, aBytes);
    offset += aBytes;

    return aBytes;
}

unsigned int Data::length() {
    return dataLength;
}

void Data::seek(int aOffset) {
    if (aOffset >= 0)
        offset = aOffset;
    else
        offset = dataLength + aOffset;
    if (offset > dataLength-1)
        offset = dataLength-1;
}

unsigned int Data::pos() {
    return offset;
}

unsigned char * Data::getMemPtr() {
    return dataPtr;
}

unsigned int Data::open(unsigned char *aData, unsigned int aDataLength, bool aCopy, bool aTakeOwnership) {
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

unsigned int Data::open(const char *aFile) {
    if (!aFile)
        return 1;
    if (dataOwned)
        delete[] dataPtr;
    dataPtr = 0;
    offset = 0;

    File df;
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

unsigned int Data::open(File *aFile) {
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

int Data::eof() {
    if (offset >= dataLength)
        return 1;
    return 0;
}
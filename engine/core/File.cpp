#include "File.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

File::File() {
    dataLength = NULL;
    data = NULL;
    fileHandle = NULL;
}

File::File(const char* path) {
    loadFile(path);
}

File::~File() {
    free((void*)data);
    free((void*)fileHandle);
}

void File::loadFile(const char* path) {

    assert(path != NULL);

    FILE* stream = fopen(path, "r");
    assert (stream != NULL);

    fseek(stream, 0, SEEK_END);
    long stream_size = ftell(stream);
    fseek(stream, 0, SEEK_SET);

    void* buffer = malloc(stream_size);
    fread(buffer, stream_size, 1, stream);

    assert(ferror(stream) == 0);
    fclose(stream);

    dataLength = stream_size;
    data = buffer;
    fileHandle = NULL;

}

long File::getDataLength(){
    return dataLength;
}

void* File::getData(){
    return data;
}

void* File::getFileHandle(){
    return fileHandle;
}

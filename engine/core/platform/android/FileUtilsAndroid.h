
#ifndef FILEUTILSANDROID_H
#define FILEUTILSANDROID_H

#include "FileUtilsPlatform.h"

class FileUtilsAndroid: public FileUtilsPlatform {

public:

    virtual FILE* platformFopen(const char* fname, const char* mode);
};


#endif

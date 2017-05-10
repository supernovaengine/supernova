
#ifndef FILEUTILSGENERIC_H
#define FILEUTILSGENERIC_H

#include "FileUtilsPlatform.h"

class FileUtilsGeneric: public FileUtilsPlatform {

    virtual FILE* platformFopen(const char* fname, const char* mode);
};

#endif //FILEUTILSGENERIC_H

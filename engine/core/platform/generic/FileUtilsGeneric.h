
#ifndef FILEUTILSGENERIC_H
#define FILEUTILSGENERIC_H

#include "platform/FileUtilsPlatform.h"

namespace Supernova {

    class FileUtilsGeneric: public FileUtilsPlatform {

    public:

        virtual FILE* platformFopen(const char* fname, const char* mode);
    };

}

#endif //FILEUTILSGENERIC_H

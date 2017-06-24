
#ifndef FILEUTILSPLATFORM_H
#define FILEUTILSPLATFORM_H

#include <stdio.h>

namespace Supernova {

    class FileUtilsPlatform {
    protected:

        FileUtilsPlatform() {}

    public:

        static FileUtilsPlatform& instance();

        virtual ~FileUtilsPlatform() {}

        virtual FILE* platformFopen(const char* fname, const char* mode) = 0;
    };

}

#endif //FILEUTILSPLATFORM_H

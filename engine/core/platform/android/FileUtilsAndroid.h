
#ifndef FILEUTILSANDROID_H
#define FILEUTILSANDROID_H

#include "platform/FileUtilsPlatform.h"

namespace Supernova {

    class FileUtilsAndroid : public FileUtilsPlatform {

    public:

        virtual FILE *platformFopen(const char *fname, const char *mode);
    };

}

#endif

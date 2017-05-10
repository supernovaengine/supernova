
#ifndef FILEUTILSPLATFORM_H
#define FILEUTILSPLATFORM_H

#include <stdio.h>

class FileUtilsPlatform {
protected:

    FileUtilsPlatform() {}

public:

    static FileUtilsPlatform& instance();

    virtual ~FileUtilsPlatform() {}

    virtual FILE* platformFopen(const char* fname, const char* mode);
};


#endif //FILEUTILSPLATFORM_H

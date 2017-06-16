#ifndef FileUtilsIOS_h
#define FileUtilsIOS_h

#include "FileUtilsPlatform.h"

namespace Supernova {

    class FileUtilsIOS: public FileUtilsPlatform {
        
    public:
        
        virtual FILE* platformFopen(const char* fname, const char* mode);
    };
    
}

#endif /* FileUtilsIOS_h */

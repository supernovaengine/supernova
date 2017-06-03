#ifdef SUPERNOVA_IOS

#include "FileUtilsIOS.h"
#include "SupernovaIOS.h"


FILE* FileUtilsIOS::platformFopen(const char* fname, const char* mode){
    
    
    return fopen(SupernovaIOS::getFullPath(fname), mode);
}

#endif
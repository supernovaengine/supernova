#ifdef SUPERNOVA_WEB

#include "FileUtilsGeneric.h"


FILE* FileUtilsGeneric::platformFopen(const char* fname, const char* mode){
    return fopen(fname, mode);
}

#endif
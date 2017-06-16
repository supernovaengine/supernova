#ifdef SUPERNOVA_ANDROID

#include "FileUtilsAndroid.h"
#include "SupernovaAndroid.h"

using namespace Supernova;

FILE* FileUtilsAndroid::platformFopen(const char* fname, const char* mode){
    return SupernovaAndroid::androidFopen(fname, mode);
}

#endif
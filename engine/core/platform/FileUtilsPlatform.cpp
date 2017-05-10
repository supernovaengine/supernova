
#include "FileUtilsPlatform.h"

#include "FileUtilsGeneric.h"

FileUtilsPlatform& FileUtilsPlatform::instance(){
#ifdef SUPERNOVA_ANDROID
    static FileUtilsPlatform *instance = new FileUtilsGeneric();
#elif SUPERNOVA_IOS
    static FileUtilsPlatform *instance = new FileUtilsGeneric();
#else
    static FileUtilsPlatform *instance = new FileUtilsGeneric();
#endif

    return *instance;
}

#include "FileUtilsPlatform.h"

#include "FileUtilsGeneric.h"

FileUtilsPlatform& FileUtilsPlatform::instance(){
#ifdef SUPERNOVA_ANDROID
    static FileUtilsPlatform *instance = new FileUtilsGeneric();
#endif
#ifdef  SUPERNOVA_IOS
    static FileUtilsPlatform *instance = new FileUtilsGeneric();
#endif
#ifdef  SUPERNOVA_WEB
    static FileUtilsPlatform *instance = new FileUtilsGeneric();
#endif

    return *instance;
}


#include "FileUtilsPlatform.h"

#include "FileUtilsGeneric.h"
#include "FileUtilsIOS.h"
#include "FileUtilsAndroid.h"

FileUtilsPlatform& FileUtilsPlatform::instance(){
#ifdef SUPERNOVA_ANDROID
    static FileUtilsPlatform *instance = new FileUtilsAndroid();
#endif
#ifdef  SUPERNOVA_IOS
    static FileUtilsPlatform *instance = new FileUtilsIOS();
#endif
#ifdef  SUPERNOVA_WEB
    static FileUtilsPlatform *instance = new FileUtilsGeneric();
#endif

    return *instance;
}

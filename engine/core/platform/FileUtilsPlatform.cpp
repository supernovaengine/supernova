
#include "FileUtilsPlatform.h"

using namespace Supernova;

#ifdef SUPERNOVA_ANDROID
#include "FileUtilsAndroid.h"
#endif
#ifdef  SUPERNOVA_IOS
#include "FileUtilsIOS.h"
#endif
#ifdef  SUPERNOVA_WEB
#include "FileUtilsGeneric.h"
#endif

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

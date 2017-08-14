
#include "FileUtilsPlatform.h"

using namespace Supernova;

#ifdef SUPERNOVA_ANDROID
#include "android/FileUtilsAndroid.h"
#endif
#ifdef  SUPERNOVA_IOS
#include "ios/FileUtilsIOS.h"
#endif
#ifdef  SUPERNOVA_WEB
#include "generic/FileUtilsGeneric.h"
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

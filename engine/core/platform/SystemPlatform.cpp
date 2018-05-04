#include "SystemPlatform.h"


using namespace Supernova;

#ifdef SUPERNOVA_ANDROID
#include "SupernovaAndroid.h"
#endif
#ifdef  SUPERNOVA_IOS
#include "SupernovaIOS.h"
#endif
#ifdef  SUPERNOVA_WEB
#include "SupernovaWeb.h"
#endif

SystemPlatform& SystemPlatform::instance(){
#ifdef SUPERNOVA_ANDROID
    static SystemPlatform *instance = new SupernovaAndroid();
#endif
#ifdef  SUPERNOVA_IOS
    static SystemPlatform *instance = new SupernovaIOS();
#endif
#ifdef  SUPERNOVA_WEB
    static SystemPlatform *instance = new SupernovaWeb();
#endif

    return *instance;
}

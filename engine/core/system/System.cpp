#include "System.h"


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

System& System::instance(){
#ifdef SUPERNOVA_ANDROID
    static System *instance = new SupernovaAndroid();
#endif
#ifdef  SUPERNOVA_IOS
    static SystemPlatform *instance = new SupernovaIOS();
#endif
#ifdef  SUPERNOVA_WEB
    static SystemPlatform *instance = new SupernovaWeb();
#endif

    return *instance;
}

void System::showVirtualKeyboard(){

}

void System::hideVirtualKeyboard(){

}

FILE* System::platformFopen(const char* fname, const char* mode){
    return fopen(fname, mode);
}

void System::platformLog(const int type, const char *fmt, va_list args){
    const char* priority;

    if (type == S_LOG_VERBOSE){
        priority = "VERBOSE";
    }else if (type == S_LOG_DEBUG){
        priority = "DEBUG";
    }else if (type == S_LOG_WARN){
        priority = "WARN";
    }else if (type == S_LOG_ERROR){
        priority = "ERROR";
    }

    if ((type == S_LOG_VERBOSE) || (type == S_LOG_DEBUG) || (type == S_LOG_WARN) || (type == S_LOG_ERROR))
        printf("( %s ): ", priority);

    vprintf(fmt, args);
    printf("\n");
}

//
// (c) 2020 Eduardo Doria.
//

#include "System.h"
#include "tinyxml2.h"
#include "util/XMLUtils.h"
#include <stdlib.h>

using namespace Supernova;

#define USERSETTINGS_ROOT "userSettings"
#define USERSETTINGS_XML_FILE "data://UserSettings.xml"

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
    static System *instance = new SupernovaIOS();
#endif
#ifdef  SUPERNOVA_WEB
    static System *instance = new SupernovaWeb();
#endif

    return *instance;
}

void System::showVirtualKeyboard(){

}

void System::hideVirtualKeyboard(){

}

bool System::isFullscreen(){
    return false;
}

void System::requestFullscreen(){

}

void System::exitFullscreen(){

}

char System::getDirSeparator(){
#if defined(_WIN32)
    return '\\';
#else
    return '/';
#endif
}

std::string System::getAssetPath(){
    return "";
}

std::string System::getUserDataPath(){
    return "";
}

FILE* System::platformFopen(const char* fname, const char* mode){
    return fopen(fname, mode);
}

bool System::syncFileSystem(){
    return true;
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

bool System::getBoolForKey(const char *key, bool defaultValue){
    const char* value = XMLUtils::getValueForKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key);

    if (!value)
        return defaultValue;

    return (! strcmp(value, "true"));
}

int System::getIntegerForKey(const char *key, int defaultValue){
    const char* value = XMLUtils::getValueForKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key);

    if (!value)
        return defaultValue;

    return atoi(value);
}

float System::getFloatForKey(const char *key, float defaultValue){
    return (float)getDoubleForKey(key, defaultValue);
}

double System::getDoubleForKey(const char *key, double defaultValue){
    const char* value = XMLUtils::getValueForKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key);

    if (!value)
        return defaultValue;

    return atof(value);
}

std::string System::getStringForKey(const char *key, std::string defaultValue){
    const char* value = XMLUtils::getValueForKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key);

    if (!value)
        return defaultValue;

    return std::string(value);
}

void System::setBoolForKey(const char *key, bool value){
    if (value) {
        setStringForKey(key, "true");
    } else {
        setStringForKey(key, "false");
    }
}

void System::setIntegerForKey(const char *key, int value){
    setStringForKey(key, std::to_string(value).c_str());
}

void System::setFloatForKey(const char *key, float value){
    setStringForKey(key, std::to_string(value).c_str());
}

void System::setDoubleForKey(const char *key, double value){
    setStringForKey(key, std::to_string(value).c_str());
}

void System::setStringForKey(const char* key, std::string value){
    XMLUtils::setValueForKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key, value.c_str());
}

void System::removeKey(const char *key){
    XMLUtils::removeKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key);
}

//
// (c) 2023 Eduardo Doria.
//

#include "System.h"
#include "util/XMLUtils.h"
#include "util/Base64.h"
#include "Log.h"
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
#ifdef  SUPERNOVA_SOKOL
#include "SupernovaSokol.h"
#endif
#ifdef  SUPERNOVA_GLFW
#include "SupernovaGLFW.h"
#endif
#ifdef  SUPERNOVA_APPLE
#include "SupernovaApple.h"
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
#ifdef  SUPERNOVA_SOKOL
    static System *instance = new SupernovaSokol();
#endif
#ifdef  SUPERNOVA_GLFW
    static System *instance = new SupernovaGLFW();
#endif
#ifdef  SUPERNOVA_APPLE
    static System *instance = new SupernovaApple();
#endif

    return *instance;
}

sg_context_desc System::getSokolContext(){
    return {};
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
    return getAssetPath();
}

std::string System::getLuaPath(){
    return getAssetPath();
}

std::string System::getShaderPath(){
    return getAssetPath() + "/" + "shaders";
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
    return getStringForKey(key, defaultValue ? "true" : "false") == "true";
}

int System::getIntegerForKey(const char *key, int defaultValue){
    return std::stoi(getStringForKey(key, std::to_string(defaultValue).c_str()));
}

long System::getLongForKey(const char *key, long defaultValue){
    return std::stol(getStringForKey(key, std::to_string(defaultValue).c_str()));
}

float System::getFloatForKey(const char *key, float defaultValue){
    return std::stof(getStringForKey(key, std::to_string(defaultValue).c_str()));
}

double System::getDoubleForKey(const char *key, double defaultValue){
    return std::stod(getStringForKey(key, std::to_string(defaultValue).c_str()));
}

Data System::getDataForKey(const char *key, const Data& defaultValue){
    std::string ret = System::instance().getStringForKey(key, "");

    if (ret.empty())
        return defaultValue;
    
    std::vector<unsigned char> decodedData = Base64::decode(ret);

    return Data(&decodedData[0], (unsigned int)decodedData.size(), true, true);
}

std::string System::getStringForKey(const char *key, std::string defaultValue){
    std::string value = XMLUtils::getValueForKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key);

    if (value.empty())
        return defaultValue;

    return value;
}

void System::setBoolForKey(const char *key, bool value){
    setStringForKey(key, value ? "true" : "false");
}

void System::setIntegerForKey(const char *key, int value){
    setStringForKey(key, std::to_string(value).c_str());
}

void System::setLongForKey(const char *key, long value){
    setStringForKey(key, std::to_string(value).c_str());
}

void System::setFloatForKey(const char *key, float value){
    setStringForKey(key, std::to_string(value).c_str());
}

void System::setDoubleForKey(const char *key, double value){
    setStringForKey(key, std::to_string(value).c_str());
}

void System::setDataForKey(const char *key, Data& value){
    if (value.getMemPtr()) {
        System::instance().setStringForKey(key, Base64::encode(value.getMemPtr(), value.length()));
    }else{
        Log::error("No data to add for key: %s", key);
    }
}

void System::setStringForKey(const char* key, std::string value){
    XMLUtils::setValueForKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key, value.c_str());
}

void System::removeKey(const char *key){
    XMLUtils::removeKey(USERSETTINGS_XML_FILE, USERSETTINGS_ROOT, key);
}

void System::initializeAdMob(){
    Log::error("Cannot initialize AdMob in this system");
}

void System::loadInterstitialAd(){
    Log::error("Cannot load InterstitialAd in this system");
}

bool System::isInterstitialAdLoaded(){
    return false;
}

void System::showInterstitialAd(){
    Log::error("Cannot show InterstitialAd in this system");
}
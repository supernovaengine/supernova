#include "SupernovaAndroid.h"

#include <errno.h>
#include <android/log.h>
#include <stdarg.h>
#include <android/asset_manager.h>
#include "AndroidJNI.h"

int android_read(void* cookie, char* buf, int size) {
    return AAsset_read((AAsset*)cookie, buf, size);
}

static int android_write(void* cookie, const char* buf, int size) {
    return EACCES; // can't provide write access to the apk
}

static fpos_t android_seek(void* cookie, fpos_t offset, int whence) {
    return AAsset_seek((AAsset*)cookie, offset, whence);
}

static int android_close(void* cookie) {
    AAsset_close((AAsset*)cookie);
    return 0;
}

SupernovaAndroid::SupernovaAndroid(){
    logtag = "Supernova";
}

int SupernovaAndroid::getScreenWidth(){
    return AndroidJNI::envRef->CallIntMethod(AndroidJNI::mainActivityObjRef, AndroidJNI::getScreenWidthRef);
}

int SupernovaAndroid::getScreenHeight(){
    return AndroidJNI::envRef->CallIntMethod(AndroidJNI::mainActivityObjRef, AndroidJNI::getScreenHeightRef);
}

void SupernovaAndroid::showVirtualKeyboard(){
    AndroidJNI::envRef->CallVoidMethod(AndroidJNI::mainActivityObjRef, AndroidJNI::showSoftKeyboardRef);
}

void SupernovaAndroid::hideVirtualKeyboard(){
    AndroidJNI::envRef->CallVoidMethod(AndroidJNI::mainActivityObjRef, AndroidJNI::hideSoftKeyboardRef);
}

std::string SupernovaAndroid::getUserDataPath() {
    jstring rv = (jstring)AndroidJNI::envRef->CallObjectMethod(AndroidJNI::mainActivityObjRef, AndroidJNI::getUserDataPathRef);
    return AndroidJNI::envRef->GetStringUTFChars(rv, 0);
}

FILE* SupernovaAndroid::platformFopen(const char* fname, const char* mode) {
    std::string path = fname;

    //Return regular fopen if writable path is in path
    if (path.find(getUserDataPath()) != std::string::npos) {
        return fopen(path.c_str(), mode);
    }

    if (path[0] == '/'){
        path = path.substr(1, path.length());
    }

    if(mode[0] == 'w') return NULL;
    AAsset* asset = AAssetManager_open(AndroidJNI::android_asset_manager, path.c_str(), 0);
    if(!asset) return NULL;
    return funopen(asset, android_read, android_write, android_seek, android_close);
}

void SupernovaAndroid::platformLog(const int type, const char *fmt, va_list args){
    int priority = ANDROID_LOG_VERBOSE;

    if (type == S_LOG_VERBOSE){
        priority = ANDROID_LOG_VERBOSE;
    }else if (type == S_LOG_DEBUG){
        priority = ANDROID_LOG_DEBUG;
    }else if (type == S_LOG_WARN){
        priority = ANDROID_LOG_WARN;
    }else if (type == S_LOG_ERROR){
        priority = ANDROID_LOG_ERROR;
    }

    __android_log_vprint(priority, logtag, fmt, args);
}

#include "SupernovaAndroid.h"

#include <errno.h>
#include <android/log.h>
#include <stdarg.h>

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

jclass SupernovaAndroid::mainActivityClsRef;
jmethodID SupernovaAndroid::showSoftKeyboardRef;
jmethodID SupernovaAndroid::hideSoftKeyboardRef;
jobject SupernovaAndroid::mainActivityObjRef;
JNIEnv * SupernovaAndroid::envRef;

AAssetManager* SupernovaAndroid::android_asset_manager;


SupernovaAndroid::SupernovaAndroid(){
    logtag = "Supernova";
}

void SupernovaAndroid::showVirtualKeyboard(){
    envRef->CallVoidMethod(mainActivityObjRef, showSoftKeyboardRef);
}

void SupernovaAndroid::hideVirtualKeyboard(){
    envRef->CallVoidMethod(mainActivityObjRef, hideSoftKeyboardRef);
}

FILE* SupernovaAndroid::platformFopen(const char* fname, const char* mode) {
    if(mode[0] == 'w') return NULL;

    AAsset* asset = AAssetManager_open(android_asset_manager, fname, 0);
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

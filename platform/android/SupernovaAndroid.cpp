#include "SupernovaAndroid.h"

#include <errno.h>
#include <android/log.h>
#include <stdarg.h>
#include <android/asset_manager.h>
#include "NativeEngine.h"

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
    return NativeEngine::getInstance()->getSurfWidth();
}

int SupernovaAndroid::getScreenHeight(){
    return NativeEngine::getInstance()->getSurfHeight();
}

void SupernovaAndroid::showVirtualKeyboard(){
    NativeEngine::getInstance()->showSoftInput();
}

void SupernovaAndroid::hideVirtualKeyboard(){
    NativeEngine::getInstance()->hideSoftInput();
}

std::string SupernovaAndroid::getUserDataPath() {
    return NativeEngine::getInstance()->getInternalDataPath();
}

FILE* SupernovaAndroid::platformFopen(const char* fname, const char* mode) {
    std::string path = fname;

    //Return regular fopen if writable path is in path
    if (!NativeEngine::getInstance() || path.find(getUserDataPath()) != std::string::npos) {
        return fopen(path.c_str(), mode);
    }

    if (path[0] == '/'){
        path = path.substr(1, path.length());
    }

    if(mode[0] == 'w') return NULL;
    AAsset* asset = AAssetManager_open(NativeEngine::getInstance()->getAssetManager(), path.c_str(), 0);
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

bool SupernovaAndroid::getBoolForKey(const char *key, bool defaultValue){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    bool value = env->CallBooleanMethod(jniData.userSettingsObjRef, jniData.getBoolForKeyRef, strKey, defaultValue);
    env->DeleteLocalRef(strKey);
    return value;
}

int SupernovaAndroid::getIntegerForKey(const char *key, int defaultValue){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    int value = env->CallIntMethod(jniData.userSettingsObjRef, jniData.getIntegerForKeyRef, strKey, defaultValue);
    env->DeleteLocalRef(strKey);
    return value;
}

long SupernovaAndroid::getLongForKey(const char *key, long defaultValue){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    long value = env->CallLongMethod(jniData.userSettingsObjRef, jniData.getLongForKeyRef, strKey, defaultValue);
    env->DeleteLocalRef(strKey);
    return value;
}

float SupernovaAndroid::getFloatForKey(const char *key, float defaultValue){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    float value = env->CallFloatMethod(jniData.userSettingsObjRef, jniData.getFloatForKeyRef, strKey, defaultValue);
    env->DeleteLocalRef(strKey);
    return value;
}

double SupernovaAndroid::getDoubleForKey(const char *key, double defaultValue){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    double value = env->CallDoubleMethod(jniData.userSettingsObjRef, jniData.getDoubleForKeyRef, strKey, defaultValue);
    env->DeleteLocalRef(strKey);
    return value;
}

std::string SupernovaAndroid::getStringForKey(const char *key, std::string defaultValue){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    jstring strDefaultValue = env->NewStringUTF(defaultValue.c_str());
    jstring rv = (jstring)env->CallObjectMethod(jniData.userSettingsObjRef, jniData.getStringForKeyRef, strKey, strDefaultValue);
    std::string value = env->GetStringUTFChars(rv, 0);
    env->DeleteLocalRef(strKey);
    env->DeleteLocalRef(strDefaultValue);
    env->DeleteLocalRef(rv);
    return value;
}

void SupernovaAndroid::setBoolForKey(const char *key, bool value){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    env->CallVoidMethod(jniData.userSettingsObjRef, jniData.setBoolForKeyRef, strKey, value);
    env->DeleteLocalRef(strKey);
}

void SupernovaAndroid::setIntegerForKey(const char *key, int value){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    env->CallVoidMethod(jniData.userSettingsObjRef, jniData.setIntegerForKeyRef, strKey, value);
    env->DeleteLocalRef(strKey);
}

void SupernovaAndroid::setLongForKey(const char *key, long value){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    env->CallVoidMethod(jniData.userSettingsObjRef, jniData.setLongForKeyRef, strKey, value);
    env->DeleteLocalRef(strKey);
}

void SupernovaAndroid::setFloatForKey(const char *key, float value){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    env->CallVoidMethod(jniData.userSettingsObjRef, jniData.setFloatForKeyRef, strKey, value);
    env->DeleteLocalRef(strKey);
}

void SupernovaAndroid::setDoubleForKey(const char *key, double value){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    env->CallVoidMethod(jniData.userSettingsObjRef, jniData.setDoubleForKeyRef, strKey, value);
    env->DeleteLocalRef(strKey);
}

void SupernovaAndroid::setStringForKey(const char* key, std::string value){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    jstring strValue = env->NewStringUTF(value.c_str());
    env->CallVoidMethod(jniData.userSettingsObjRef, jniData.setStringForKeyRef, strKey, strValue);
    env->DeleteLocalRef(strKey);
    env->DeleteLocalRef(strValue);
}

void SupernovaAndroid::removeKey(const char* key){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strKey = env->NewStringUTF(key);
    env->CallVoidMethod(jniData.userSettingsObjRef, jniData.removeKeyRef, strKey);
    env->DeleteLocalRef(strKey);
}

void SupernovaAndroid::initializeAdMob(){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    env->CallVoidMethod(jniData.adMobWrapperObjRef, jniData.initializeAdMob);
}

void SupernovaAndroid::tagForChildDirectedTreatmentAdMob(bool enable){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    env->CallVoidMethod(jniData.adMobWrapperObjRef, jniData.tagForChildDirectedTreatmentAdMob, enable);
}

void SupernovaAndroid::tagForUnderAgeOfConsentAdMob(bool enable){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    env->CallVoidMethod(jniData.adMobWrapperObjRef, jniData.tagForUnderAgeOfConsentAdMob, enable);
}

void SupernovaAndroid::loadInterstitialAd(std::string adUnitID){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    jstring strAdUnitID = env->NewStringUTF(adUnitID.c_str());
    env->CallVoidMethod(jniData.adMobWrapperObjRef, jniData.loadInterstitialAd, strAdUnitID);
    env->DeleteLocalRef(strAdUnitID);
}

bool SupernovaAndroid::isInterstitialAdLoaded(){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    bool value = env->CallBooleanMethod(jniData.adMobWrapperObjRef, jniData.isInterstitialAdLoaded);
    return value;
}

void SupernovaAndroid::showInterstitialAd(){
    JniData& jniData = NativeEngine::getInstance()->getJniData();
    JNIEnv* env = NativeEngine::getInstance()->getJniEnv();

    env->CallVoidMethod(jniData.adMobWrapperObjRef, jniData.showInterstitialAd);
}
#include "SupernovaAndroid.h"

#include <errno.h>

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
jmethodID SupernovaAndroid::showInputTextRef;
jobject SupernovaAndroid::mainActivityObjRef;
JNIEnv * SupernovaAndroid::envRef;

AAssetManager* SupernovaAndroid::android_asset_manager;


void SupernovaAndroid::showTextInput(const char* buffer){

	jstring jstr = envRef->NewStringUTF(buffer);
	envRef->CallVoidMethod(mainActivityObjRef, showInputTextRef, jstr);
}

FILE* SupernovaAndroid::androidFopen(const char* fname, const char* mode) {
    if(mode[0] == 'w') return NULL;

    AAsset* asset = AAssetManager_open(android_asset_manager, fname, 0);
    if(!asset) return NULL;

    return funopen(asset, android_read, android_write, android_seek, android_close);
}

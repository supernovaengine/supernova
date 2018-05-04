#ifndef SupernovaAndroid_H_
#define SupernovaAndroid_H_

#include <jni.h>
#include <android/asset_manager.h>
#include <stdio.h>
#include "platform/SystemPlatform.h"

class SupernovaAndroid: public Supernova::SystemPlatform {

public:

	static jclass mainActivityClsRef;
	static jmethodID showSoftKeyboardRef;
	static jmethodID hideSoftKeyboardRef;
	static jobject mainActivityObjRef;
	static JNIEnv * envRef;

	static AAssetManager* android_asset_manager;

	virtual void showVirtualKeyboard();
	virtual void hideVirtualKeyboard();
    virtual FILE* platformFopen(const char* fname, const char* mode);
};

#endif /* SupernovaAndroid_H_ */

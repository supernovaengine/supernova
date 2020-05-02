#ifndef SupernovaAndroid_H_
#define SupernovaAndroid_H_

#include <jni.h>
#include <android/asset_manager.h>
#include <stdio.h>
#include "system/System.h"

class SupernovaAndroid: public Supernova::System {

private:

    const char* logtag;

public:

	static jclass mainActivityClsRef;
	static jmethodID getScreenWidthRef;
	static jmethodID getScreenHeightRef;
	static jmethodID showSoftKeyboardRef;
	static jmethodID hideSoftKeyboardRef;
	static jobject mainActivityObjRef;
	static JNIEnv * envRef;

	static AAssetManager* android_asset_manager;

    SupernovaAndroid();

    virtual int getScreenWidth();
    virtual int getScreenHeight();

	virtual void showVirtualKeyboard();
	virtual void hideVirtualKeyboard();
    virtual FILE* platformFopen(const char* fname, const char* mode);
	virtual void platformLog(const int type, const char *fmt, va_list args);
};

#endif /* SupernovaAndroid_H_ */

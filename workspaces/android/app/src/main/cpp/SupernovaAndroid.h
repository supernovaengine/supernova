#ifndef SupernovaAndroid_H_
#define SupernovaAndroid_H_

#include <jni.h>
#include <android/asset_manager.h>
#include <stdio.h>

class SupernovaAndroid {

public:

	static jclass mainActivityClsRef;
	static jmethodID showInputTextRef;
	static jobject mainActivityObjRef;
	static JNIEnv * envRef;

	static AAssetManager* android_asset_manager;

	static void showTextInput(const char* buffer);
    static FILE* androidFopen(const char* fname, const char* mode);
};

#endif /* SupernovaAndroid_H_ */

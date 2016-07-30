/*
 * AndroidCallback.h
 *
 *  Created on: 5 de mai de 2016
 *      Author: eduardolima
 */

#ifndef JNI_ANDROIDCALLBACK_H_
#define JNI_ANDROIDCALLBACK_H_

#include <jni.h>

class AndroidCallback {

public:

	static jclass mainActivityClsRef;
	static jmethodID showInputTextRef;
	static jobject mainActivityObjRef;
	static JNIEnv * envRef;

	static void showTextInput(const char* buffer);
};

#endif /* JNI_ANDROIDCALLBACK_H_ */

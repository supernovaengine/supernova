/*
 * AndroidCallback.cpp
 *
 *  Created on: 5 de mai de 2016
 *      Author: eduardolima
 */

#include "androidcallback.h"

#include "supernova_android_jni.h"


jclass AndroidCallback::mainActivityClsRef;
jmethodID AndroidCallback::showInputTextRef;
jobject AndroidCallback::mainActivityObjRef;
JNIEnv * AndroidCallback::envRef;

void AndroidCallback::showTextInput(const char* buffer){

	jstring jstr = envRef->NewStringUTF(buffer);
	envRef->CallVoidMethod(mainActivityObjRef, showInputTextRef, jstr);
}

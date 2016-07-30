/*
 * supernova_android_jni.h
 *
 *  Created on: 1 de mai de 2016
 *      Author: eduardolima
 */

#ifndef JNI_SUPERNOVA_ANDROID_JNI_H_
#define JNI_SUPERNOVA_ANDROID_JNI_H_

#include <jni.h>


extern "C" {
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_init_1native(JNIEnv * env, jclass cls, jobject main_activity, jobject java_asset_manager);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1start(JNIEnv * env, jclass cls, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1surface_1created(JNIEnv * env, jclass cls);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1surface_1changed(JNIEnv * env, jclass cls, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1draw_1frame(JNIEnv * env, jclass cls);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1press(JNIEnv * env, jclass cls, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1up(JNIEnv * env, jclass cls, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1drag(JNIEnv * env, jclass cls, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1key_1press(JNIEnv * env, jclass cls, int key);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1key_1up(JNIEnv * env, jclass cls, int key);
	JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1text_1input(JNIEnv * env, jclass cls, jstring text);
}

//void showSoftKeyboard();


#endif /* JNI_SUPERNOVA_ANDROID_JNI_H_ */

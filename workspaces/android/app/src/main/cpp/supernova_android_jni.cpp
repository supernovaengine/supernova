#include "supernova_android_jni.h"

#include <android/asset_manager_jni.h>

#include "Engine.h"
#include "SupernovaAndroid.h"

#define UNUSED(x) (void)(x)

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_init_1native(JNIEnv * env, jclass cls, jobject main_activity, jobject java_asset_manager) {
    UNUSED(cls);

    SupernovaAndroid::envRef = env;
    SupernovaAndroid::mainActivityClsRef = env->FindClass("com/deslon/supernova/MainActivity");
    //SupernovaAndroid::showInputTextRef = env->GetMethodID(SupernovaAndroid::mainActivityClsRef, "showInputText", "(Ljava/lang/String;)V");
    SupernovaAndroid::showSoftKeyboardRef = env->GetMethodID(SupernovaAndroid::mainActivityClsRef, "showSoftKeyboard", "()V");
    SupernovaAndroid::hideSoftKeyboardRef = env->GetMethodID(SupernovaAndroid::mainActivityClsRef, "hideSoftKeyboard", "()V");
    SupernovaAndroid::mainActivityObjRef = env->NewGlobalRef(main_activity);

    SupernovaAndroid::android_asset_manager = AAssetManager_fromJava(env, java_asset_manager);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1start(JNIEnv * env, jclass cls, jint width, jint height) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemStart(width, height);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1surface_1created(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemSurfaceCreated();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1surface_1changed(JNIEnv * env, jclass cls, jint width, jint height) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemSurfaceChanged(width, height);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1draw(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemDraw();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1pause(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemPause();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1resume(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemResume();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1touch_1start(JNIEnv * env, jclass cls, jint pointer, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemTouchStart(pointer, x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1touch_1end(JNIEnv * env, jclass cls, jint pointer, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemTouchEnd(pointer, x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1touch_1drag(JNIEnv * env, jclass cls, jint pointer, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemTouchDrag(pointer, x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1key_1down(JNIEnv * env, jclass cls, jint key) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemKeyDown(key);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1key_1up(JNIEnv * env, jclass cls, jint key) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemKeyUp(key);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1text_1input(JNIEnv * env, jclass cls, jstring text) {
	UNUSED(cls);
	const char *nativeString = env->GetStringUTFChars(text, 0);
    Supernova::Engine::systemTextInput(nativeString);
}
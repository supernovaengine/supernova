#include "supernova_android_jni.h"

#include <android/log.h>
#include <android/asset_manager_jni.h>

#include "platform/Log.h"
#include "Engine.h"
#include "SupernovaAndroid.h"

#define UNUSED(x) (void)(x)

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1start(JNIEnv * env, jclass cls, jint width, jint height) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::onStart(width, height);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1surface_1created(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::onSurfaceCreated();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1surface_1changed(JNIEnv * env, jclass cls, jint width, jint height) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::onSurfaceChanged(width, height);

}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1draw(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::onDraw();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1pause(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::onPause();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1resume(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::onResume();
}

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

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1start(JNIEnv * env, jclass cls, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::onTouchStart(x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1end(JNIEnv * env, jclass cls, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::onTouchEnd(x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1drag(JNIEnv * env, jclass cls, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::onTouchDrag(x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1key_1down(JNIEnv * env, jclass cls, int key) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::onKeyDown(key);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1key_1up(JNIEnv * env, jclass cls, int key) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::onKeyUp(key);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1text_1input(JNIEnv * env, jclass cls, jstring text) {
	UNUSED(cls);
	const char *nativeString = env->GetStringUTFChars(text, 0);
    Supernova::Engine::onTextInput(nativeString);
}
#include "AndroidJNI.h"

#include <android/asset_manager_jni.h>

#include "Engine.h"

#define UNUSED(x) (void)(x)

jclass AndroidJNI::mainActivityClsRef;

jmethodID AndroidJNI::getScreenWidthRef;
jmethodID AndroidJNI::getScreenHeightRef;
jmethodID AndroidJNI::getUserDataPathRef;
jmethodID AndroidJNI::showSoftKeyboardRef;
jmethodID AndroidJNI::hideSoftKeyboardRef;

jmethodID AndroidJNI::getBoolForKeyRef;
jmethodID AndroidJNI::getIntegerForKeyRef;
jmethodID AndroidJNI::getFloatForKeyRef;
jmethodID AndroidJNI::getDoubleForKeyRef;
jmethodID AndroidJNI::getStringForKeyRef;

jmethodID AndroidJNI::setBoolForKeyRef;
jmethodID AndroidJNI::setIntegerForKeyRef;
jmethodID AndroidJNI::setFloatForKeyRef;
jmethodID AndroidJNI::setDoubleForKeyRef;
jmethodID AndroidJNI::setStringForKeyRef;

jmethodID AndroidJNI::removeKeyRef;

jobject AndroidJNI::mainActivityObjRef;
JNIEnv * AndroidJNI::envRef;

AAssetManager* AndroidJNI::android_asset_manager;


JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_init_1native(JNIEnv * env, jclass cls, jobject main_activity, jobject java_asset_manager) {
    UNUSED(cls);

    AndroidJNI::envRef = env;
    AndroidJNI::mainActivityClsRef = env->FindClass("com/deslon/supernova/MainActivity");

    AndroidJNI::getScreenWidthRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getScreenWidth", "()I");
    AndroidJNI::getScreenHeightRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getScreenHeight", "()I");
	AndroidJNI::getUserDataPathRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getUserDataPath", "()Ljava/lang/String;");
	AndroidJNI::showSoftKeyboardRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "showSoftKeyboard", "()V");
	AndroidJNI::hideSoftKeyboardRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "hideSoftKeyboard", "()V");

	AndroidJNI::getBoolForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getBoolForKey", "(Ljava/lang/String;Z)Z");
	AndroidJNI::getIntegerForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getIntegerForKey", "(Ljava/lang/String;I)I");
	AndroidJNI::getFloatForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getFloatForKey", "(Ljava/lang/String;F)F");
	AndroidJNI::getDoubleForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getDoubleForKey", "(Ljava/lang/String;D)D");
	AndroidJNI::getStringForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getStringForKey", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

	AndroidJNI::setBoolForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setBoolForKey", "(Ljava/lang/String;Z)V");
	AndroidJNI::setIntegerForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setIntegerForKey", "(Ljava/lang/String;I)V");
	AndroidJNI::setFloatForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setFloatForKey", "(Ljava/lang/String;F)V");
	AndroidJNI::setDoubleForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setDoubleForKey", "(Ljava/lang/String;D)V");
	AndroidJNI::setStringForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setStringForKey", "(Ljava/lang/String;Ljava/lang/String;)V");

	AndroidJNI::removeKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "removeKey", "(Ljava/lang/String;)V");

    AndroidJNI::mainActivityObjRef = env->NewGlobalRef(main_activity);

	AndroidJNI::android_asset_manager = AAssetManager_fromJava(env, java_asset_manager);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1start(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemStart();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1surface_1created(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemSurfaceCreated();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_system_1surface_1changed(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemSurfaceChanged();
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
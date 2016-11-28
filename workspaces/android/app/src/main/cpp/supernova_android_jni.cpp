#include "supernova_android_jni.h"

#include <android/log.h>
#include <android/asset_manager_jni.h>

#include "platform/Log.h"
#include "Engine.h"
//#include "config.h"
#include "android_fopen.h"
#include "androidcallback.h"



//static AAssetManager* asset_manager;

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1start(JNIEnv * env, jclass cls, jint width, jint height) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onStart(width, height);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1surface_1created(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onSurfaceCreated();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1surface_1changed(JNIEnv * env, jclass cls, jint width, jint height) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onSurfaceChanged(width, height);

}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1draw_1frame(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onDrawFrame();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1pause(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Engine::onPause();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1resume(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Engine::onResume();
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_init_1native(JNIEnv * env, jclass cls, jobject main_activity, jobject java_asset_manager) {
	UNUSED(cls);

	AndroidCallback::envRef = env;
	AndroidCallback::mainActivityClsRef = env->FindClass("com/deslon/supernova/MainActivity");
	AndroidCallback::showInputTextRef = env->GetMethodID(AndroidCallback::mainActivityClsRef, "showInputText", "(Ljava/lang/String;)V");
	AndroidCallback::mainActivityObjRef = env->NewGlobalRef(main_activity);

	//asset_manager = AAssetManager_fromJava(env, java_asset_manager);
	android_fopen_set_asset_manager(AAssetManager_fromJava(env, java_asset_manager));
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1press(JNIEnv * env, jclass cls, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onTouchPress(x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1up(JNIEnv * env, jclass cls, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onTouchUp(x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1touch_1drag(JNIEnv * env, jclass cls, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onTouchDrag(x, y);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1key_1press(JNIEnv * env, jclass cls, int key) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onKeyPress(key);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1key_1up(JNIEnv * env, jclass cls, int key) {
	UNUSED(env);
	UNUSED(cls);
	Engine::onKeyUp(key);
}

JNIEXPORT void JNICALL Java_com_deslon_supernova_JNIWrapper_on_1text_1input(JNIEnv * env, jclass cls, jstring text) {
	//UNUSED(env);
	UNUSED(cls);
	const char *nativeString = env->GetStringUTFChars(text, 0);
	Engine::onTextInput(nativeString);
}


//void showSoftKeyboard(){
//	envRef->CallVoidMethod(mainActivityObjRef, showInputTextRef);
//}

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
jmethodID AndroidJNI::getLongForKeyRef;
jmethodID AndroidJNI::getFloatForKeyRef;
jmethodID AndroidJNI::getDoubleForKeyRef;
jmethodID AndroidJNI::getStringForKeyRef;

jmethodID AndroidJNI::setBoolForKeyRef;
jmethodID AndroidJNI::setIntegerForKeyRef;
jmethodID AndroidJNI::setLongForKeyRef;
jmethodID AndroidJNI::setFloatForKeyRef;
jmethodID AndroidJNI::setDoubleForKeyRef;
jmethodID AndroidJNI::setStringForKeyRef;

jmethodID AndroidJNI::removeKeyRef;

JavaVM* AndroidJNI::jvm;
jobject AndroidJNI::mainActivityObjRef;

AAssetManager* AndroidJNI::android_asset_manager;


JNIEnv* AndroidJNI::getEnv() {
	JNIEnv* env;
	if (jvm->GetEnv((void**)&env, JNI_VERSION_1_4) == JNI_OK)
		return env;
	jvm->AttachCurrentThread(&env, NULL);
	pthread_key_t key;
	if (pthread_key_create(&key, detachCurrentThreadDtor) == 0) {
		pthread_setspecific(key, (void*)env);
	}
	return env;
}

void AndroidJNI::detachCurrentThreadDtor(void* p) {
	__android_log_print(ANDROID_LOG_INFO, "Supernova", "%s", "detached current thread");
	if (p != nullptr) {
		jvm->DetachCurrentThread();
	}
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_init_1native(JNIEnv * env, jclass cls, jobject main_activity, jobject java_asset_manager) {
    UNUSED(cls);

	env->GetJavaVM(&(AndroidJNI::jvm));
    AndroidJNI::mainActivityClsRef = env->FindClass("org/supernovaengine/supernova/MainActivity");

    AndroidJNI::getScreenWidthRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getScreenWidth", "()I");
    AndroidJNI::getScreenHeightRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getScreenHeight", "()I");
	AndroidJNI::getUserDataPathRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getUserDataPath", "()Ljava/lang/String;");
	AndroidJNI::showSoftKeyboardRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "showSoftKeyboard", "()V");
	AndroidJNI::hideSoftKeyboardRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "hideSoftKeyboard", "()V");

	AndroidJNI::getBoolForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getBoolForKey", "(Ljava/lang/String;Z)Z");
	AndroidJNI::getIntegerForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getIntegerForKey", "(Ljava/lang/String;I)I");
	AndroidJNI::getLongForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getLongForKey", "(Ljava/lang/String;J)J");
	AndroidJNI::getFloatForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getFloatForKey", "(Ljava/lang/String;F)F");
	AndroidJNI::getDoubleForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getDoubleForKey", "(Ljava/lang/String;D)D");
	AndroidJNI::getStringForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "getStringForKey", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");

	AndroidJNI::setBoolForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setBoolForKey", "(Ljava/lang/String;Z)V");
	AndroidJNI::setIntegerForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setIntegerForKey", "(Ljava/lang/String;I)V");
	AndroidJNI::setLongForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setLongForKey", "(Ljava/lang/String;J)V");
	AndroidJNI::setFloatForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setFloatForKey", "(Ljava/lang/String;F)V");
	AndroidJNI::setDoubleForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setDoubleForKey", "(Ljava/lang/String;D)V");
	AndroidJNI::setStringForKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "setStringForKey", "(Ljava/lang/String;Ljava/lang/String;)V");

	AndroidJNI::removeKeyRef = env->GetMethodID(AndroidJNI::mainActivityClsRef, "removeKey", "(Ljava/lang/String;)V");

    AndroidJNI::mainActivityObjRef = env->NewGlobalRef(main_activity);

	AndroidJNI::android_asset_manager = AAssetManager_fromJava(env, java_asset_manager);
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1start(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemInit(0, nullptr);
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1surface_1created(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemViewLoaded();
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1surface_1changed(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemViewChanged();
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1shutdown(JNIEnv * env, jclass cls) {
    UNUSED(env);
    UNUSED(cls);
    Supernova::Engine::systemShutdown();
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1draw(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemDraw();
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1pause(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemPause();
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1resume(JNIEnv * env, jclass cls){
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemResume();
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1touch_1start(JNIEnv * env, jclass cls, jint pointer, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemTouchStart(pointer, x, y);
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1touch_1end(JNIEnv * env, jclass cls, jint pointer, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemTouchEnd(pointer, x, y);
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1touch_1move(JNIEnv * env, jclass cls, jint pointer, jfloat x, jfloat y) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemTouchMove(pointer, x, y);
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1touch_1cancel(JNIEnv * env, jclass cls) {
	UNUSED(env);
	UNUSED(cls);
	Supernova::Engine::systemTouchCancel();
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1key_1down(JNIEnv * env, jclass cls, jint key, jboolean repeat, jint mods) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemKeyDown(key, repeat, mods);
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1key_1up(JNIEnv * env, jclass cls, jint key, jboolean repeat, jint mods) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemKeyUp(key, repeat, mods);
}

JNIEXPORT void JNICALL Java_org_supernovaengine_supernova_JNIWrapper_system_1char_1input(JNIEnv * env, jclass cls, jint codepoint) {
	UNUSED(env);
	UNUSED(cls);
    Supernova::Engine::systemCharInput(codepoint);
}
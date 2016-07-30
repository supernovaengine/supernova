LOCAL_PATH := $(call my-dir)

APP_ABI := armeabi
#  Enable C++11. However, pthread, rtti and exceptions aren’t enabled 
# APP_CPPFLAGS += -std=c++11
APP_CFLAGS := -include "$(LOCAL_PATH)/android_macros.h"
# Instruct to use the static GNU STL implementation
#APP_STL := gnustl_static
APP_STL := c++_static

APP_CPP_FEATURES := rtti exceptions
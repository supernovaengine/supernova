LOCAL_PATH := $(call my-dir)
 
include $(CLEAR_VARS)
 
LOCAL_ARM_MODE  := arm
LOCAL_CFLAGS    := -D"lua_getlocaledecpoint()='.'" -DLUA_ANSI -x c++
LOCAL_MODULE    := liblua

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c)
FILE_LIST := $(filter-out $(LOCAL_PATH)/lua.c, $(FILE_LIST))
FILE_LIST := $(filter-out $(LOCAL_PATH)/luac.c, $(FILE_LIST))

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)
	
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

include $(BUILD_STATIC_LIBRARY)
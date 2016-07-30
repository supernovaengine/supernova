LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libpng

FILE_LIST := $(wildcard $(LOCAL_PATH)/*.c)
FILE_LIST := $(filter-out $(LOCAL_PATH)/pngtest.c, $(FILE_LIST))
				  	
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)	
				  
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)
LOCAL_EXPORT_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)
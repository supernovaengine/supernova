LOCAL_PATH := $(call my-dir)
LIBS_PATH := $(LOCAL_PATH)/../../../engine/libs
CORE_PATH := $(LOCAL_PATH)/../../../engine/core
USERPROJECT_PATH := $(LOCAL_PATH)/../../../project

include $(CLEAR_VARS)

LOCAL_MODULE    := Supernova
LOCAL_CFLAGS    := -std=c++11 -Wall -Wextra -fexceptions -frtti
LOCAL_LDFLAGS 	:= -Wl,--gc-sections

		
FILE_LIST := $(wildcard $(LOCAL_PATH)/*.cpp)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c)
FILE_LIST += $(wildcard $(USERPROJECT_PATH)/*.cpp)
FILE_LIST += $(wildcard $(CORE_PATH)/*.cpp)
FILE_LIST += $(wildcard $(CORE_PATH)/*/*.cpp)
FILE_LIST += $(wildcard $(CORE_PATH)/*/*/*.cpp)

LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)		
				
				
LOCAL_C_INCLUDES := $(CORE_PATH)
LOCAL_C_INCLUDES += $(LIBS_PATH)/luaintf/
LOCAL_LDLIBS    := -llog -landroid -lGLESv2
LOCAL_STATIC_LIBRARIES := liblua
LOCAL_STATIC_LIBRARIES += libpng
LOCAL_STATIC_LIBRARIES += libtinyobjloader


include $(BUILD_SHARED_LIBRARY)


$(call import-add-path,$(LIBS_PATH))
$(call import-module,lua)
$(call import-module,libpng)
$(call import-module,tinyobjloader)
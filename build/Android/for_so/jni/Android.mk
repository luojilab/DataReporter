LOCAL_PATH := $(call my-dir)/../../../../src 
cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug -x $1
include $(CLEAR_VARS)
LOCAL_ARM_MODE := arm
LOCAL_MODULE := data-reporter
LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL \
                -fvisibility=hidden 

LOCAL_SRC_FILES := \
  $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/call_interface/android/*.cpp)) \
  $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/core/base/*.cpp)) \
  $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/core/crypto/*.c)) \
  $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/core/reporter/*.cpp)) \
  $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/core/thread/*.cpp)) \
  $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/platform/android/*.cpp)) \
  $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/platform/android/util/*.cpp)) 
   
LOCAL_C_INCLUDES := $(LOCAL_PATH)/call_interface/android 
LOCAL_C_INCLUDES := $(LOCAL_PATH)/core/base/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/core/crypto/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/core/reporter/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/core/thread/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/platform/android/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/platform/android/util/

LOCAL_LDLIBS := -latomic -llog -lz

#include $(BUILD_STATIC_LIBRARY)
include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += main.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../export/tee-client-android/include \
                    $(LOCAL_PATH)/../ta/include

LOCAL_LDFLAGS := $(LOCAL_PATH)/../../../export/tee-client-android/lib/libteec.so

LOCAL_MODULE := hello_world
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

include $(BUILD_EXECUTABLE)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES += main.c
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../export/tee-client/include \
                    $(LOCAL_PATH)/../ta/include

LOCAL_LDFLAGS := $(LOCAL_PATH)/../../../export/tee-client/lib/android/libteec.so

LOCAL_MODULE := secure_storage
LOCAL_MODULE_TAGS := optional
LOCAL_VENDOR_MODULE := true

include $(BUILD_EXECUTABLE)

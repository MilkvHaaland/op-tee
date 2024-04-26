LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(LOCAL_PATH)/acipher/host/Android.mk \
		$(LOCAL_PATH)/cipher/host/Android.mk \
		$(LOCAL_PATH)/curve25519/host/Android.mk \
		$(LOCAL_PATH)/dsa/host/Android.mk \
		$(LOCAL_PATH)/ecdh/host/Android.mk \
		$(LOCAL_PATH)/ecdsa/host/Android.mk \
		$(LOCAL_PATH)/efuse/host/Android.mk \
		$(LOCAL_PATH)/hash/host/Android.mk \
		$(LOCAL_PATH)/hello_world/host/Android.mk \
		$(LOCAL_PATH)/secure_storage/host/Android.mk \
		$(LOCAL_PATH)/trng/host/Android.mk \
		$(LOCAL_PATH)/float/host/Android.mk \
		$(LOCAL_PATH)/shared_memory/host/Android.mk \
		$(LOCAL_PATH)/gpio/host/Android.mk \
		$(LOCAL_PATH)/spi/host/Android.mk \
		$(LOCAL_PATH)/i2c/host/Android.mk \
		$(LOCAL_PATH)/se/host/Android.mk \

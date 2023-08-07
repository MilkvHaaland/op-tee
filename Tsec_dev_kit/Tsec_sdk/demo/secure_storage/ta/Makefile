CFG_TEE_TA_LOG_LEVEL ?= 4

# The UUID for the Trusted Application
-include ../../config.mk
TA_DEV_KIT_DIR=../../../export/ta-rv64
BINARY=9d662048-86d1-48e8-9436-5f9a9d7320d7

-include $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk

ifeq ($(wildcard $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk), )
clean:
	@echo 'Note: $$(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk not found, cannot clean TA'
	@echo 'Note: TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)'
endif

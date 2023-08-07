CFG_TEE_TA_LOG_LEVEL ?= 4

# The UUID for the Trusted Application
-include ../../config.mk
TA_DEV_KIT_DIR=../../../export/ta-rv64
BINARY=8165d0ff-2186-4105-a118-ba5328e63c12

-include $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk

ifeq ($(wildcard $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk), )
clean:
	@echo 'Note: $$(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk not found, cannot clean TA'
	@echo 'Note: TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)'
endif

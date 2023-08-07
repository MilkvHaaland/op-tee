-include ../../config.mk

CFG_TEE_TA_LOG_LEVEL ?= 4

TA_DEV_KIT_DIR=../../../export/ta-rv64
# The UUID for the Trusted Application
BINARY=de013140-d35d-4cbb-b9a8-95e5a2ba5f46


-include $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk

ifeq ($(wildcard $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk), )
clean:
	@echo 'Note: $$(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk not found, cannot clean TA'
	@echo 'Note: TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)'
endif

CFG_TEE_TA_LOG_LEVEL ?= 4

-include ../../config.mk
TA_DEV_KIT_DIR=../../../export/ta-rv64
# The UUID for the Trusted Application
BINARY=62180425-1809-aabc-abe2-0082abd5f43c

-include $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk

ifeq ($(wildcard $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk), )
clean:
	@echo 'Note: $$(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk not found, cannot clean TA'
	@echo 'Note: TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)'
endif

CFG_TEE_TA_LOG_LEVEL ?= 4

# The UUID for the Trusted Application
CROSS_COMPILE=/home/kjs/xuantie/Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4/bin/riscv64-unknown-linux-gnu-
TA_DEV_KIT_DIR=../../../export/ta-rv64
BINARY=1265d0ff-a186-4101-a118-ba3321e63c12
-include $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk

ifeq ($(wildcard $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk), )
clean:
	@echo 'Note: $$(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk not found, cannot clean TA'
	@echo 'Note: TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR)'
endif

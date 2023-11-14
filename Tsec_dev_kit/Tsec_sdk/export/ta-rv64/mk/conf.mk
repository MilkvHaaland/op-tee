sm := ta_riscv64
sm-ta_riscv64 := y
CFG_RISCV64_ta_riscv64 := y
ta_riscv64-platform-cppflags := -DRISCV64=1 -D__LP64__=1 -DMBEDTLS_SELF_TEST
ta_riscv64-platform-cflags := -mcmodel=medany -O0 -g3 -fpic -mstrict-align -march=rv64imafdcxtheadc -mabi=lp64d
ta_riscv64-platform-aflags := -pipe -g -mcmodel=medany
ta_riscv64-platform-cxxflags := -fpic -O0 -g3 -march=rv64imafdcxtheadc -mabi=lp64d
CFG_TA_FLOAT_SUPPORT := y
CFG_SECURE_DATA_PATH := n
CFG_TA_MBEDTLS_SELF_TEST := y
CFG_TA_MBEDTLS := y
CFG_TA_MBEDTLS_MPI := y
CFG_SYSTEM_PTA := y
CFG_FTRACE_SUPPORT := n
CFG_UNWIND := y
CFG_CORE_TPM_EVENT_LOG := n
CFG_TA_BGET_TEST := y
CFG_TA_MBEDTLS := y
CROSS_COMPILE64 ?= $(CROSS_COMPILE)
CROSS_COMPILE_ta_riscv64 ?= $(CROSS_COMPILE64)
COMPILER ?= gcc
COMPILER_ta_riscv64 ?= $(COMPILER)
PYTHON3 ?= python3
CFG_TEE_TA_LOG_LEVEL ?= 1


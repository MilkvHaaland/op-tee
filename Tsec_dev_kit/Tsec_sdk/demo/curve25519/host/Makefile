
####################################################################
## parameter define 
####################################################################

## Toolchain
CROSS_COMPILE=/home/kjs/xuantie/Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4/bin/riscv64-unknown-linux-gnu-

## TEE export path
TEEC_EXPORT ?= ../../../export/tee-client
# TEEC_EXPORT ?= ../../../export/ta-rv64

CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
AR      = $(CROSS_COMPILE)ar
NM      = $(CROSS_COMPILE)nm
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
READELF = $(CROSS_COMPILE)readelf

OBJS = main.o

CFLAGS += -Wall -I../ta/include -I$(TEEC_EXPORT)/include -I./include
#Add/link other required libraries here
LDADD += -lteec -L$(TEEC_EXPORT)/lib -lpthread

BINARY = optee_example_25519

.PHONY: all
all: $(BINARY)
	echo $(CROSS_COMPILE)

$(BINARY): $(OBJS)
	echo $(CROSS_COMPILE)
	$(CC) $(LDFLAGS) -o $@ main.o $(LDADD)

.PHONY: clean
clean:
	rm -f $(OBJS) $(BINARY)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

## Toolchain
CROSS_COMPILE := /home/kjs/xuantie/Xuantie-900-gcc-elf-newlib-x86_64-V2.2.4/bin/riscv64-unknown-linux-gnu-gcc

CC=$(CROSS_COMPILE)
CFLAGS=-DxDEBUG
LIBS=-L../../../export/tee-client/lib -lteec
INCLUDE=-I./inc -I../../../export/tee-client/include

BIN = optee_example_efuse
OUTDIR = ./
SRCS:=$(wildcard *.c)
COBJS:=$(SRCS:.c=.o)

all:$(OUTDIR)/$(BIN)

$(OUTDIR)/$(BIN):$(COBJS)
	mkdir -p $(OUTDIR)
	$(CC) -o $(OUTDIR)$(BIN) $(CFLAGS) $(COBJS) $(LIBS)

$(COBJS): %.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: clean

clean:
	rm -rf $(OUTDIR)$(BIN) $(COBJS)

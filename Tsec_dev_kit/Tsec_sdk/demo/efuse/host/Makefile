-include ../../config.mk

CC=$(CROSS_COMPILE)gcc
CFLAGS=-DxDEBUG
LIBS=-L../../../export/tee-client/lib/linux -lteec
INCLUDE=-I./inc -I../../../export/ta-rv64/host_include -I../../../export/tee-client/include

BIN = efuse
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

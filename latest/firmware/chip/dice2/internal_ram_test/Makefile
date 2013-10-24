# Mostly written by Jonathan Larmour, Red Hat, Inc.
# Reference to ecos.mak added by John Dallaway, eCosCentric Limited, 2003-01-20
# This file is in the public domain and may be used for any purpose

# Usage:   make INSTALL_DIR=/path/to/ecos/install

INSTALL_DIR=$$(INSTALL_DIR) # override on make command line

# **** use the following line for memory test with debug symbols
include $(INSTALL_DIR)/include/pkgconf/ecos.mak

# *** or use the following 3 lines to override debug symbol generation, which allows loading without the built sources
# ECOS_GLOBAL_CFLAGS = -mcpu=arm7tdmi -mlittle-endian -mno-short-load-words -Wall -Wpointer-arith -Wstrict-prototypes -Wno-inline -Wundef -Woverloaded-virtual -O2 -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fvtable-gc -finit-priority
# ECOS_GLOBAL_LDFLAGS = -mcpu=arm7tdmi -mlittle-endian -mno-short-load-words -Wl,--gc-sections -Wl,-static -nostdlib
# ECOS_COMMAND_PREFIX = arm-elf-

XCC           = $(ECOS_COMMAND_PREFIX)gcc
XCXX          = $(XCC)
XLD           = $(XCC)

CFLAGS        = -I$(INSTALL_DIR)/include
CXXFLAGS      = $(CFLAGS)
LDFLAGS       = -nostartfiles -L$(INSTALL_DIR)/lib -Ttarget.ld

# RULES

.PHONY: all clean

all: in_ram_test 

clean:
	-rm -f in_ram_test in_ram_test.o twothreads twothreads.o

%.o: %.c
	$(XCC) -c -o $*.o $(CFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cxx
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.C
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cc
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

in_ram_test: in_ram_test.o
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@ $@.o

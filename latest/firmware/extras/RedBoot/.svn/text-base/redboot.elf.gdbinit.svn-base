# use this file for loading redboot.elf during board bringup

set endian little

# for command-line gdb with OpenOCD probes, such as JTAGkey
# target remote 127.0.0.1:3333

define wd
p/x *((unsigned int*)0xbf000004) = 0x5678
end

define setup_int_mux
p/x *((unsigned int*)0xc7000024) = 0x0c83a443
p/x *((unsigned int*)0xc7000028) = 0x14488025
p/x *((unsigned int*)0xc700002c) = 0x20f6b98b
p/x *((unsigned int*)0xc7000030) = 0x12
p/x *((unsigned int*)0xc7000034) = 0x0
p/x *((unsigned int*)0xc7000038) = 0x0
end

#following is a define of 
#load, next step, break, backtrace with a watchdog kick

define myload
wd
load
wd
end

define mn
next
wd
end

define ms
step
wd
end

define mb
b $arg0
wd
end

define mbt
bt
wd
end

#dump uart registers.
define dumpuart
wd
echo UART0: 
x/8x 0xbe000000
echo UART1: 
x/8x 0xbd000000
wd
end



define startup
wd
setup_int_mux
#########################################################
# Kick watchdog
p/x *((unsigned long *) 0xbf000000) = 0x00001234
p/x *((unsigned long *) 0xbf000004) = 0x00005678

#########################################################
#set the "set0-set2" manager
#set0 - SRAM8
p/x *((unsigned long *) 0x81000094) = 0x10000041
#set1 - SRAM16
p/x *((unsigned long *) 0x81000098) = 0x10000041
#set2 - FLASH - 
#p/x *((unsigned long *) 0x8100009c) = 0x10011405 !using default now
#p/x *((unsigned long *) 0x8100009c) = 0x10744FFE 
p/x *((unsigned long *) 0x8100009c) = 0x10011405
#########################################################
#set the static memory ctrl register
#SMCTRLR (set0: 8bit, set1: 16bit, set2: 16bit)
p/x *((unsigned long *) 0x810000a4) = 0x0000020F

#########################################################
#Static Memory Mask Register

#SMSKR0	FLASH  -   2MB	- set2
p/x *((unsigned long *) 0x81000054) = 0x00000246
p/x *((unsigned long *) 0x81000054)
#SMSKR1 SDRAM  -   8MB  -
p/x *((unsigned long *) 0x81000058) = 0x00000008
#SMSKR2	SRAM8  - 512KB	- set1
p/x *((unsigned long *) 0x8100005c) = 0x00000024		
#SMSKR3	SRAM16 - 512KB	- set2
p/x *((unsigned long *) 0x81000060) = 0x00000124		
#SMSKR4 DSP    - Disable
p/x *((unsigned long *) 0x81000064) = 0x00000020		
#SMSKR5 FPGA   - Disable
p/x *((unsigned long *) 0x81000068) = 0x00000020		
#SMSKR6 FPGA   - Disable
p/x *((unsigned long *) 0x8100006c) = 0x00000020		
#SMSKR7 FPGA   - Disable
p/x *((unsigned long *) 0x81000070) = 0x00000020		

#########################################################
#SDRAM Configuration & Initialization
p/x *((unsigned long *) 0x81000000) = 0x00140f68
p/x *((unsigned long *) 0x81000004) = 0x00dcd449
p/x *((unsigned long *) 0x81000008) = 0x00012000
#50MHz clock
p/x *((unsigned long *) 0x81000010) = 0x0000030c
# 2MHz clock
#p/x *((unsigned long *) 0x81000010) = 0x0000001F
p/x *((unsigned long *) 0x81000058) = 0x00000008
p/x *((unsigned long *) 0x8100000c) = 0x00002071

#########################################################
#route CS in GPCSR Registers
#GPCSR_IO CS2,CS3,CS4,CS5,CS6,CS7
p/x *((unsigned long *) 0xc7000004) = 0x000502AA		
#GPCSR_IO CS2,CS3,CS4
p/x *((unsigned long *) 0xc7000004) = 0x00050280

#########################################################
#set all the base addresses, 
#FLASH
p/x *((unsigned long *) 0x81000014) = 0x04000000		
#SDRAM
p/x *((unsigned long *) 0x81000018) = 0x00000000		
#SRAM8
p/x *((unsigned long *) 0x8100001c) = 0x02080000		
#SRAM16
p/x *((unsigned long *) 0x81000020) = 0x02000000		
#DSP
p/x *((unsigned long *) 0x81000024) = 0x05000000		
#FPGA
p/x *((unsigned long *) 0x81000028) = 0x06000000		
#FPGA
p/x *((unsigned long *) 0x8100002c) = 0x07000000
#FPGA
#p/x *((unsigned long *) 0x81000030) = 0x08000000

#########################################################
#set the alias and remap registers to be the same as base
#CSALIAS0
p/x *((unsigned long *) 0x81000074) = 0x04000000
#CSALIAS1
p/x *((unsigned long *) 0x81000078) = 0x00000000		
#CSREMAP0
p/x *((unsigned long *) 0x81000084) = 0x04000000		
#CSREMAP1
p/x *((unsigned long *) 0x81000088) = 0x01000000		

#########################################################
# Remap for the internal ram
#p/x *((unsigned long *) 0xc0000008) = 0x00000001

#########################################################
# Kick watchdog
wd
end

#EVM board io setup. 
define evm_io_setup
p/x *((unsigned long *) 0xc7000004) = 0x28480
p/x *((unsigned long *) 0xc3000004) = 0xfdf
p/x *((unsigned long *) 0xc3000000) = 0xfc1
end

define reset
wd
startup
load
wd
end

define dumpirq
x/1x 0xc1000000
x/1x 0xc1000008
x/1x 0xc1000010
x/1x 0xc1000018
x/1x 0xc1000020
x/1x 0xc1000028
x/1x 0xc1000030
x/1x 0xc1000038
wd
end

define dumpgpio
x/2x 0xc3000000
wd
end

define dic
x/1x 0xc4000034
x/1x 0xc4000070
x/1x 0xc4000080
wd
end

define bquery
b flash_query
wd
end

#start up begins here. First to print out llc id
startup
evm_io_setup

# specify file for quicker load during DICEII bring-up
# load redboot.elf
# c


//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/* this is small test program that can run inside diceII internal sram 
 */
#include <stdio.h>
#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_arch.h>           // SAVE/RESTORE GP macros
#include <cyg/hal/hal_misc.h>           // Helper functions

#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_tables.h>
#include <cyg/infra/diag.h>

#if 1
#define LED_ON				targetBoardLedOn
#define LED_OFF				targetBoardLedOff
#define LED_TOGGLE			targetBoardLedToggle
#else
#define LED_ON
#define LED_OFF
#define LED_TOGGLE
#endif
typedef unsigned char	uint8;
typedef signed char		int8;
typedef unsigned short	uint16;
typedef signed short	int16;

typedef unsigned long	uint32;
typedef signed long		int32;
typedef unsigned long	HRESULT;

static uint8 LED_MAX = 7;

typedef struct
{
	volatile uint32*	addr;
	volatile uint32	mask;
} LED_CONFIG;

LED_CONFIG ledConfig[] =
{ 
	{(volatile uint32 *) GPIO_A_DR, 0x800}, 
	{(volatile uint32 *) GPIO_A_DR, 0x40},
	{(volatile uint32 *) GPIO_A_DR, 0x400},
	{(volatile uint32 *) GPIO_A_DR, 0x1},
	{(volatile uint32 *) GPIO_A_DR, 0x200},
	{(volatile uint32 *) GPIO_A_DR, 0x100},
	{(volatile uint32 *) GPIO_A_DR, 0x80},
};

void targetBoardLedOn(uint8 id)
{
	if(id == 0 || id > LED_MAX) return;
	id--;
	*(ledConfig[id].addr) |= ledConfig[id].mask;
}

void targetBoardLedOff(uint8 id)
{
	if(id == 0 || id > LED_MAX) return;
	id--;
	*(ledConfig[id].addr) &= ~ledConfig[id].mask;
}

void targetBoardLedToggle(uint8 id)
{
	if(id == 0 || id > LED_MAX) return;
	id--;
	if(*(ledConfig[id].addr) & ledConfig[id].mask)
	{
		*(ledConfig[id].addr) &= ~ledConfig[id].mask;
	}
	else
	{
		*(ledConfig[id].addr) |= ledConfig[id].mask;
	}
}

uint32 number_of_error  = 0;
uint32 *high_water_mark = 0;
uint32 *low_water_mark = (uint32 *)0xFFFFFFF0;
uint32 *last_error_addr = 0;

static	uint32	firstExcludeAddress; 
static	uint32	numExcludeBytes;

static int	addressExcluded(uint32 addr)		// statics above must have been set up properly
{
	return(	numExcludeBytes &&
			(addr >= firstExcludeAddress) &&
			(addr < (firstExcludeAddress+numExcludeBytes)));
}	

void kick_wd(void) {
	LED_TOGGLE(1);
	*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; // kick the wd
}

void halt_on_error(void) {
	LED_ON(7);
	for(;;)
		kick_wd();
}

uint32 _runs = 1;
#define HANDLE_ERROR(x) {sysDebugPrintf("error at %x\n", x);}
//halt_on_error()

void irt_serial_init(void);
void  irt_serial_putc(char ch);
uint32 irt_gets(char *buf, uint32 len);

#define sysDebugPrintf diag_printf

uint32 memTestDataBus8 (volatile uint8 *baseAddr)
{
	uint8	pattern;
	uint8	read;

	sysDebugPrintf("memTestDataBus8: base:0x%08x\n\r", baseAddr);
	sysDebugPrintf("memTestDataBus8: wr/rd single address with single bit set values\n\r");

	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			sysDebugPrintf("memTestDataBus8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", baseAddr, pattern, read);
			return ((uint32) baseAddr);
		}
	}

	sysDebugPrintf("memTestDataBus8: done\n\r");

	return 0;
}	
//*********************

uint32 memTestDataBus16 (volatile uint16 *baseAddr)
{
	uint16	pattern;
	uint16	read;

	sysDebugPrintf("memTestDataBus16: base:0x%08x\n\r", baseAddr);
	sysDebugPrintf("memTestDataBus16: wr/rd single address with single bit set values\n\r");

	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			sysDebugPrintf("memTestDataBus16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", baseAddr, pattern, read);
			return ((uint32) baseAddr);
		}
	}

	sysDebugPrintf("memTestDataBus16: done\n\r");

	return 0;
}	

uint32 memTestDataBus32 (volatile uint32 *baseAddr)
{
	uint32	pattern;
	uint32	read;


	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			HANDLE_ERROR(baseAddr);
			return ((uint32) baseAddr);
		}
	}
	return 0;
}

uint32	memTestAddrBus8  (
					uint32 baseAddr, 
					uint32 nBytes, 
					uint32 exludeStartAddress,
					uint32 excludeByteLength)
{
	uint8	pattern = 0xAA;
	uint8	antiPattern = 0x55;
	uint8	read;
	uint32	addressMask = (nBytes-1);		// assumes 'real' memory size
	uint32	offset;
	uint32	testOffset;
	uint32	targetAddr;
	uint32	testTargetAddr;

	// setup exclude region
	firstExcludeAddress = (uint32)exludeStartAddress;
	numExcludeBytes = excludeByteLength;

	sysDebugPrintf("memTestAddrBus8: base:0x%08x, addrMask:0x%08x\n\r", baseAddr, addressMask);
	sysDebugPrintf("memTestAddrBus8: write pattern:0x%02x to each of the power-of-two offsets\n\r", pattern);

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint8); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint8*)targetAddr) = pattern;
		}
	}

	sysDebugPrintf("memTestAddrBus8: check for address bits stuck high with antiPattern:0x%02x\n\r", antiPattern);

	// check for address bits stuck high
	*((uint8*)baseAddr) = antiPattern;
	for (offset = sizeof(uint8); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint8*)targetAddr)) != pattern)
			{
				sysDebugPrintf("memTestAddrBus8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
		}
	}

	*((uint8*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	sysDebugPrintf("memTestAddrBus8: check for address bits stuck low or shorted:\n\r");

	// check for address bits stuck low or shorted:
	// write antiPattern to each address, checking all the other addresses after each write
	for (testOffset = sizeof(uint8); (testOffset & addressMask) != 0; testOffset <<=1)
	{
		testTargetAddr = baseAddr + testOffset;
		if (!addressExcluded(testTargetAddr))
		{
			// write value to one address
			*((uint8*)testTargetAddr) = antiPattern;
			// check all the other addresses
			for (offset = sizeof(uint8); (offset & addressMask) != 0; offset <<=1)
			{
				targetAddr = baseAddr + offset;
				if (!addressExcluded(targetAddr))
				{
					if (offset != testOffset)
					{
						if ((read = *((uint8*)targetAddr)) != pattern)
						{
							sysDebugPrintf("memTestAddrBus8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, pattern, read);
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint8*)testTargetAddr) = pattern;
		}
	}

	sysDebugPrintf("memTestAddrBus8: done\n\r");

	return 0;
}	
//*********************

uint32 	memTestAddrBus16 (
					uint32 baseAddr, 
					uint32 nBytes,
					uint32 exludeStartAddress,
					uint32 excludeByteLength)
{
	uint16	pattern = 0xAAAA;
	uint16	antiPattern = 0x5555;
	uint16	read;
	uint32	addressMask = (nBytes-1);		// assumes 'real' memory size
	uint32	offset;
	uint32	testOffset;
	uint32	targetAddr;
	uint32	testTargetAddr;

	// setup exclude region
	firstExcludeAddress = (uint32)exludeStartAddress;
	numExcludeBytes = excludeByteLength;

	sysDebugPrintf("memTestAddrBus16: base:0x%08x, addrMask:0x%08x\n\r", baseAddr, addressMask);
	sysDebugPrintf("memTestAddrBus16: write pattern:0x%04x to each of the power-of-two offsets\n\r", pattern);

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint16); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint16*)targetAddr) = pattern;
		}
	}

	sysDebugPrintf("memTestAddrBus16: check for address bits stuck high with antiPattern:0x%04x\n\r", antiPattern);

	// check for address bits stuck high
	*((uint16*)baseAddr) = antiPattern;
	for (offset = sizeof(uint16); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint16*)targetAddr)) != pattern)
			{
				sysDebugPrintf("memTestAddrBus16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
		}
	}

	*((uint16*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	sysDebugPrintf("memTestAddrBus16: check for address bits stuck low or shorted:\n\r");

	// check for address bits stuck low or shorted:
	// write antiPattern to each address, checking all the other addresses after each write
	for (testOffset = sizeof(uint16); (testOffset & addressMask) != 0; testOffset <<=1)
	{
		testTargetAddr = baseAddr + testOffset;
		if (!addressExcluded(testTargetAddr))
		{
			// write value to one address
			*((uint16*)testTargetAddr) = antiPattern;
			// check all the other addresses
			for (offset = sizeof(uint16); (offset & addressMask) != 0; offset <<=1)
			{
				targetAddr = baseAddr + offset;
				if (!addressExcluded(targetAddr))
				{
					if (offset != testOffset)
					{
						if ((read = *((uint16*)targetAddr)) != pattern)
						{
							sysDebugPrintf("memTestAddrBus16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, pattern, read);
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint16*)testTargetAddr) = pattern;
		}
	}

	sysDebugPrintf("memTestAddrBus16: done\n\r");

	return 0;
}	

uint32 	memTestAddrBus32 (
					uint32 baseAddr, 
					uint32 nBytes,
					uint32 exludeStartAddress,
					uint32 excludeByteLength)
{
	uint32	pattern = 0xAAAAAAAA;
	uint32	antiPattern = 0x55555555;
	uint32	read;
	uint32	addressMask = (nBytes-1);		// assumes 'real' memory size
	uint32	offset;
	uint32	testOffset;
	uint32	targetAddr;
	uint32	testTargetAddr;

	// setup exclude region
	firstExcludeAddress = (uint32)exludeStartAddress;
	numExcludeBytes = excludeByteLength;

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint32); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr)) {
			*((uint32*)targetAddr) = pattern;
		}
	}

	// check for address bits stuck high
	*((uint32*)baseAddr) = antiPattern;
	for (offset = sizeof(uint32); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint32*)targetAddr)) != pattern)
			{
				HANDLE_ERROR(targetAddr);
				return targetAddr;
			}
		}
	}

	*((uint32*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	// check for address bits stuck low or shorted:
	// write antiPattern to each address, checking all the other addresses after each write
	for (testOffset = sizeof(uint32); (testOffset & addressMask) != 0; testOffset <<=1)
	{
		testTargetAddr = baseAddr + testOffset;
		if (!addressExcluded(testTargetAddr))
		{
			// write value to one address
			*((uint32*)testTargetAddr) = antiPattern;
			// check all the other addresses
			for (offset = sizeof(uint32); (offset & addressMask) != 0; offset <<=1)
			{
				targetAddr = baseAddr + offset;
				if (!addressExcluded(targetAddr))
				{
					if (offset != testOffset)
					{
						//diag_printf("  0x%x", targetAddr);
						if ((read = *((uint32*)targetAddr)) != pattern)
						{
							HANDLE_ERROR(testTargetAddr);
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint32*)testTargetAddr) = pattern;
		}
	}

	return 0;
}	


uint32	memTestRdWrMem8  (
					uint32 baseAddr, 
					uint32 nBytes, 
					uint32 exludeStartAddress,
					uint32 excludeByteLength,
					uint32 step)
{
	uint8	pattern;
	uint8	antiPattern;
	uint8	read;
	uint32	offset;
	uint32	targetAddr;
	uint32	i;

	// setup exclude region
	firstExcludeAddress = (uint32)exludeStartAddress;
	numExcludeBytes = excludeByteLength;

	sysDebugPrintf("memTestRdWrMem8: base:0x%08x, size:0x%08x, step:%s(0x%08x)\n\r", baseAddr, nBytes, (step==4) ? "all" : "jump", step);
	sysDebugPrintf("memTestRdWrMem8: testing addresses:\n\r");
	sysDebugPrintf("memTestRdWrMem8: ");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
		{
			targetAddr = baseAddr + offset;
			sysDebugPrintf("0x%08x,", targetAddr);
			offset++;
			if ((step !=4) && (offset%4==0)) offset+=step-4;
			if (i && (i % 4 == 0))
				{
					sysDebugPrintf("\n\r");
					sysDebugPrintf("memTestRdWrMem8: ");
				}
			kick_wd();
		}
	sysDebugPrintf("...\n\r");
	sysDebugPrintf("memTestRdWrMem8: fill memory with a pattern\n\r");

	// fill memory with a pattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint8*)targetAddr) = pattern;
		}
		offset++;
		if(offset % 0x10000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	sysDebugPrintf("memTestRdWrMem8: verify memory pattern and fill with antipattern\n\r");

	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint8*)targetAddr)) != pattern)
			{
				sysDebugPrintf("memTestRdWrMem8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
			antiPattern = ~pattern;
			*((uint8*)targetAddr) = antiPattern;
		}
		offset++;
		if(offset % 0x10000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	sysDebugPrintf("memTestRdWrMem8: verify memory antiPattern\n\r");

	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint8*)targetAddr)) != antiPattern)
			{
				sysDebugPrintf("memTestRdWrMem8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, antiPattern, read);
				return targetAddr;
			}
		}
		offset++;
		if(offset % 0x10000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	sysDebugPrintf("memTestRdWrMem8: done\n\r");

	return 0;
}	
//*********************

uint32 	memTestRdWrMem16 (
					uint32 baseAddr, 
					uint32 nBytes, 
					uint32 exludeStartAddress,
					uint32 excludeByteLength,
					uint32 step)
{
	uint16	pattern;
	uint16	antiPattern;
	uint16	read;
	uint32	offset;
	uint32	targetAddr;
	uint32	i;

	// setup exclude region
	firstExcludeAddress = (uint32)exludeStartAddress;
	numExcludeBytes = excludeByteLength;

	sysDebugPrintf("memTestRdWrMem16: base:0x%08x, size:0x%08x, step:%s(0x%08x)\n\r", baseAddr, nBytes, (step==4) ? "all" : "jump", step);
	sysDebugPrintf("memTestRdWrMem16: testing addresses:\n\r");
	sysDebugPrintf("memTestRdWrMem16: ");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
		{
			targetAddr = baseAddr + offset;
			sysDebugPrintf("0x%08x,", targetAddr);
			offset+=2;
			if ((step !=4) && (offset%4==0)) offset+=step-4;
			if (i && (i % 4 == 0))
				{
				sysDebugPrintf("\n\r");
				sysDebugPrintf("memTestRdWrMem16: ");
				}
		}
	sysDebugPrintf("...\n\r");
	sysDebugPrintf("memTestRdWrMem16: fill memory with a pattern\n\r");
		
	// fill memory with a pattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint16*)targetAddr) = pattern;
		}
		offset+=2;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	sysDebugPrintf("memTestRdWrMem16: verify memory pattern and fill with antipattern\n\r");

	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint16*)targetAddr)) != pattern)
			{
				sysDebugPrintf("memTestRdWrMem16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
			antiPattern = ~pattern;
			*((uint16*)targetAddr) = antiPattern;
		}
		offset+=2;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	sysDebugPrintf("memTestRdWrMem16: verify memory antiPattern\n\r");

	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint16*)targetAddr)) != antiPattern)
			{
				sysDebugPrintf("memTestRdWrMem16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, antiPattern, read);
				return targetAddr;
			}
		}
		offset+=2;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

		sysDebugPrintf("memTestRdWrMem16: done\n\r");

	return 0;
}	

uint32 	memTestRdWrMem32 (
					uint32 baseAddr, 
					uint32 nBytes, 
					uint32 exludeStartAddress,
					uint32 excludeByteLength,
					uint32 step)
{
	uint32	pattern;
	uint32	antiPattern;
	uint32	read;
	uint32	offset;
	uint32	targetAddr;
	uint32	i;

	// setup exclude region
	firstExcludeAddress = (uint32)exludeStartAddress;
	numExcludeBytes = excludeByteLength;
	
	sysDebugPrintf("memTestRdWrMem32: base:0x%08x, size:0x%08x, step:%s(0x%08x)\n\r", baseAddr, nBytes, (step==4) ? "all" : "jump", step);
	sysDebugPrintf("memTestRdWrMem32: testing addresses:\n\r");
	sysDebugPrintf("memTestRdWrMem32: ");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
	{
		targetAddr = baseAddr + offset;
		sysDebugPrintf("0x%08x,", targetAddr);
		offset+=4;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
		if (i && (i % 4 == 0))
		{
			sysDebugPrintf("\n\r");
			sysDebugPrintf("memTestRdWrMem32: ");
		}
	}
	sysDebugPrintf("...\n\r");
	sysDebugPrintf("memTestRdWrMem32: fill memory with a pattern\n\r");

	// fill memory with a pattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint32*)targetAddr) = pattern;
		}
		offset+=4;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	sysDebugPrintf("memTestRdWrMem32: verify memory pattern and fill with antipattern\n\r");
	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint32*)targetAddr)) != pattern)
			{
				HANDLE_ERROR(targetAddr);
				return targetAddr;
			}
			antiPattern = ~pattern;
			*((uint32*)targetAddr) = antiPattern;
		}
		offset+=4;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	sysDebugPrintf("memTestRdWrMem32: verify memory antiPattern\n\r");
	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint32*)targetAddr)) != antiPattern)
			{
				HANDLE_ERROR(targetAddr);
				return targetAddr;
			}
		}
		offset+=4;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
	sysDebugPrintf("memTestRdWrMem32: done\n\r");

	return 0;
}


static void
_mon_write_char(char c, void **param)
{
    if (c == '\n') {
        irt_serial_putc('\r');
    }
    irt_serial_putc(c);
}

#define MEM_START 0x1000000
#define MEM_LEN (0x800000)

uint32
mem_test(int counts)
{
	int _run;
	for(_run = 1; _run < counts + 1; _run++) {
		if(memTestDataBus8 ((volatile uint8 *)MEM_START)) 
			number_of_error++;
		if(memTestDataBus16 ((volatile uint16 *)MEM_START)) 
			number_of_error++;
		if(memTestDataBus32 ((volatile uint32 *)MEM_START)) 
			number_of_error++;
		diag_printf("\ndata bus test ... done. \n");
		LED_TOGGLE(3);
		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd
		
		if(memTestAddrBus8 (MEM_START, MEM_LEN, 0, 0x0)) 
			number_of_error++;
		if(memTestAddrBus16 (MEM_START, MEM_LEN, 0, 0x0)) 
			number_of_error++;
		if(memTestAddrBus32 (MEM_START, MEM_LEN, 0, 0x0)) 
			number_of_error++;
		diag_printf("\naddress bus test ... done. \n");
		LED_TOGGLE(4);
		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd
		diag_printf("************************************\n");
		diag_printf("This is %6dth run of the memTest.\n",_run);
		diag_printf("************************************\n");
		
		if(memTestRdWrMem8 (MEM_START, MEM_LEN, 0, 0x0, 4))
			number_of_error++;
		if(memTestRdWrMem16 (MEM_START, MEM_LEN, 0, 0x0, 4))
			number_of_error++;
		if(memTestRdWrMem32 (MEM_START, MEM_LEN, 0, 0x0, 4))
			number_of_error++;
		LED_TOGGLE(5);
		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd

	}

	if(number_of_error) {
		LED_ON(6);
	}
	diag_printf("Total number of errors is %d.\n", number_of_error);
	return number_of_error;
}

#if 1
int isxdigit(char c) {
	return ((c >= '0' && c <= '9') || 
			(c >= 'a' && c <= 'f') || 
			(c >= 'A' && c <= 'F')
			);
}

int isdigit(char c) {
	return (c >= '0' && c <= '9');
}
#endif

int
isspace(char c)
{
	return c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n';  
}

int
toint(char c)
{
	if(c >= '0' && c < '9') return c - '0';
	if(c >= 'A' && c <= 'F') return c - 'A' + 10;
	if(c >= 'a' && c <= 'f') return c - 'a' + 10;
	return 0;
}

char *
get_dec(char *buf, uint32 *value)
{
	*value = 0;
	//there should be no space.
	if(!isdigit(*buf)) return NULL;
	while(isdigit(*buf))
		*value = *value*10 + toint(*buf++);
	return buf;
}

static char *
get_hex(char *buf, uint32 *value)
{
	*value = 0;
	//there should be no space.
	if(!isxdigit(*buf)) return NULL;
	while(isxdigit(*buf))
		*value = (*value<<4) + toint(*buf++);
	return buf;
}

char *
get_number(char *buf, uint32 *value)
{
	char *ret;
	/* get rid of white spaces */
	while(isspace(*buf)) buf++;
	if(*buf == '0' && (*(buf+1) == 'x' || *(buf+1) == 'X'))
		ret = get_hex(buf+2, value);
	else
		ret = get_dec(buf, value);
	return ret;
}

void
do_help(void) 
{
	diag_printf("DICE II internal RAM tests:\n");
	diag_printf("   m count: Memory test\n");
	diag_printf("   x start_addr length_in_quadlet: display memory\n");
	diag_printf("   l/w/b address value: Set value\n");
	diag_printf("   ?/h: Help, this menu\n");
}

int 
process_cmd(char *cmd) {
	uint32 i = 0;
	uint32 addr, count, value;

	diag_printf("%s \n", cmd);
	switch (*cmd++) {
	case 'm':
		if(get_number(cmd, &count)) 
			mem_test(count);
		else
			diag_printf("Wrong parameters.\n");
		break;
	case 'g':
	case 'x':
		if((cmd = get_number(cmd, &addr))) {
			if(!(cmd = get_number(cmd, &count)))
				count = 1;
		} else {
			diag_printf("Wrong parameters.\n");
			break;
		}

		if(count < 1) count = 1;
		addr &= 0xFFFFFFFC;
		for(i = 0; i < count; i++, addr += 4) {
			if(addr % 16 == 0) {
				diag_printf("\n");
				diag_printf("0x%08x: ", addr);
			}
			diag_printf("0x%08x ", *((uint32 *)addr));
		}
		diag_printf("\n");
		break;
	case 'l': 
		if ((cmd = get_number(cmd, &addr)) && (get_number(cmd, &value))) {
			*((uint32 *)addr) = value;
			diag_printf("  *0x%08x = 0x%x\n", addr, *((uint32 *)addr));
		} else 
			diag_printf("Wrong parameters.\n");
		break;
	case 'b': 
		if ((cmd = get_number(cmd, &addr)) && (get_number(cmd, &value))) {
			*((uint8 *)addr) = (uint8) value;
			diag_printf("  *0x%08x = 0x%08x\n", addr, *((uint8 *)addr));
		} else 
			diag_printf("Wrong parameters.\n");
		break;
	case 'w': 
		if ((cmd = get_number(cmd, &addr)) && (get_number(cmd, &value))) {
			*((uint16 *)addr) = (uint16) value;
			diag_printf("  *0x%08x = 0x%08x\n", addr, *((uint16 *)addr));
		} else 
			diag_printf("Wrong parameters.\n");
		break;
	case '?':
	case 'h':
		do_help();
		break;
	default:
		diag_printf("unknown command.\n");
		break;
	}
	return 1;
}
#define LINE_SIZE 80
char prompt[] = "$";
static char cmd_line_buf[LINE_SIZE + 1];

void cyg_user_start(void)
{
	int i, j;

	for(i = 0; i < 7; i++) {
		for(j = 0; j < 10000; j++);
		LED_TOGGLE(i+1);
		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd
	}

	irt_serial_init();

#if 1
    diag_init_putc(_mon_write_char);
    hal_if_diag_init();

	diag_printf("\n\nTC Applied Technologies.\n");
#endif


	do_help(); 
	while(1) {
		diag_printf("%s ", prompt);
		irt_gets(cmd_line_buf, LINE_SIZE);
		process_cmd(cmd_line_buf);
	}
}

cyg_bool
irt_serial_getc_nonblock(char *ch) {
    cyg_uint8* base = (cyg_uint8*)(UART0BASE);
    cyg_uint32 stat, rbr;
        
    HAL_READ_UINT32(base+UART_LSR, stat);

    if ((stat & BIT0) == 0) {
        return false;
    }

    //HAL_READ_UINT8(base+UART_RBR, ch);
    HAL_READ_UINT32(base+UART_RBR, rbr);
    *ch = (cyg_uint8)(rbr & 0xff);
    return true;
}

cyg_uint8 
irt_serial_getc(void) {
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    while(!irt_serial_getc_nonblock(&ch)){
		*((unsigned long*)0xbf000004) = 0x5678; //hack, kick the wd
    };

    CYGARC_HAL_RESTORE_GP();
    return ch;
}

uint32
irt_gets(char *buf, uint32 len) {
	int count = 0;
	char ch;
	while(count < len - 1) {
		ch = irt_serial_getc(); 
		irt_serial_putc(ch); // echo
		if(ch == '\n' || ch == '\r'){
			irt_serial_putc('\n');
			irt_serial_putc('\r');
			break;
		} else if (ch == '\b') {
			count--;
			continue;
		}

		buf[count++] = ch;
	}
	buf[count] = '\0';
	return 1;
}

void 
irt_serial_putc(char ch) {
    cyg_uint8* base = (cyg_uint8*)(UART0BASE);
    cyg_uint32 status;
    
    CYGARC_HAL_SAVE_GP();

    do {
        HAL_READ_UINT32(base+UART_LSR, status);
    } while ((status & BIT5) == 0);

    HAL_WRITE_UINT8(base+UART_THR, ch);

    CYGARC_HAL_RESTORE_GP();
}

#if 0 //we don't need this if we have diag_printf
void
irt_serial_write(const cyg_uint8* __buf, 
                         cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP();

    while((__len-- > 0) &&  (*__buf != 0)) {
		if(*__buf == '\n') 
			irt_serial_putc('\r');
        irt_serial_putc(*__buf++);
	}

    CYGARC_HAL_RESTORE_GP();
}
#endif

void
irt_serial_init(void) {
    cyg_uint8* base = (cyg_uint8*)(UART0BASE);
    *((volatile int *)(base + UART_LCR)) |= 0x80;  
    *((volatile int *)(base + UART_DLL)) = 27; //divisor = 53, baudrate = 57962
    *((volatile int *)(base + UART_DLH)) = 0x00;
    *((volatile int *)(base + UART_LCR)) &= 0x7F;

    //*UART0_IER = 0x01;//0x0F, zluo, not sure should I enable it.

    //enable interrupt, it is up to ARM to mask off the interrupt or not.
    *((volatile int *)(base + UART_IER)) = 0x0;

    //Enable FIFO's and set RCVR Trigger = 1 char (FCR)
    *((volatile int *)(base + UART_FCR)) = 0x01;
  
    //Select 8bits/char, 1stop bit and enable NONE parity (LCR) 
    *((volatile int *)(base + UART_LCR)) = 0x3;
  
    //Clear "loop back mode" and set RTS and DTR (MCR)
    *((volatile int *)(base + UART_MCR)) = 0x03;
}

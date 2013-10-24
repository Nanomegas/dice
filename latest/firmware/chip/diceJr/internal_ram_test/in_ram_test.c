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
/* this is small test program that can run inside dice min or jr internal sram 
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

typedef unsigned char	uint8;
typedef signed char		int8;
typedef unsigned short	uint16;
typedef signed short	int16;

typedef unsigned long	uint32;
typedef signed long		int32;
typedef unsigned long	HRESULT;

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
	*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; // kick the wd
}

void halt_on_error(void) {
	for(;;)
		kick_wd();
}

uint32 _runs = 1;

void irt_serial_init(void);
void  irt_serial_putc(char ch);
uint32 irt_gets(char *buf, uint32 len);
void irt_serial_write(cyg_uint8* __buf);
void irt_serial_writeln(cyg_uint8* __buf);
char* itoa(int val, int base);
void handleError(uint32 error);
void irt_serial_write_hex(uint32 value);
void irt_serial_write_dec(uint32 value);

void handleError(uint32 error)
{
	irt_serial_write("error\0");
	irt_serial_write_dec(error);
	irt_serial_writeln("\0");	
}



uint32 memTestDataBus8 (volatile uint8 *baseAddr)
{
	uint8	pattern;
	uint8	read;

	irt_serial_write("\n\rmemTestDataBus8: base:\0");
	irt_serial_write_hex((uint32)baseAddr);
	irt_serial_writeln("\0");
	irt_serial_writeln("memTestDataBus8: wr/rd single address with single bit set values\0");

	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			irt_serial_write("memTestDataBus8: ERROR addr: \0");
			irt_serial_write_hex((uint32)baseAddr);
			irt_serial_write(", expected: \0");
			irt_serial_write_dec((uint32)pattern);
			irt_serial_write(", read:\0");
			irt_serial_write_dec((uint32)read);
			irt_serial_writeln("\0");
			return ((uint32) baseAddr);
		}
	}

	irt_serial_writeln("\n\rmemTestDataBus8: done\0");

	return 0;
}	
//*********************

uint32 memTestDataBus16 (volatile uint16 *baseAddr)
{
	uint16	pattern;
	uint16	read;

	irt_serial_write("memTestDataBus16: base:\0");
	irt_serial_write_hex((uint32)baseAddr);
	irt_serial_writeln("\0");
	irt_serial_writeln("memTestDataBus16: wr/rd single address with single bit set values\0");

	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			irt_serial_write("memTestDataBus16: ERROR addr: \0");
			irt_serial_write_hex((uint32)baseAddr);
			irt_serial_write(" expected: \0");
			irt_serial_write_hex((uint32)pattern);
			irt_serial_write(" read: \0");
			irt_serial_write_hex((uint32)read);
			irt_serial_writeln("\0");
			return ((uint32) baseAddr);
		}
	}

	irt_serial_writeln("\n\rmemTestDataBus16: done\0");

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
			handleError((uint32)baseAddr);
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

	irt_serial_write("memTestAddrBus8: base:\0");
	irt_serial_write_hex((uint32)baseAddr);
	irt_serial_write(", addrMask:\0");
	irt_serial_write_hex((uint32)addressMask);
	irt_serial_writeln("\0");

	irt_serial_write("memTestAddrBus8: write pattern:\0");
	irt_serial_write_hex((uint32)pattern);
	irt_serial_writeln(" to each of the power-of-two offsets\0");

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint8); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint8*)targetAddr) = pattern;
		}
	}

	irt_serial_write("memTestAddrBus8: check for address bits stuck high with antiPattern:\0");
	irt_serial_write_hex((uint32)antiPattern);
	irt_serial_writeln("\0");

	// check for address bits stuck high
	*((uint8*)baseAddr) = antiPattern;
	for (offset = sizeof(uint8); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint8*)targetAddr)) != pattern)
			{
				irt_serial_write("memTestAddrBus8: ERROR addr:\0");
				irt_serial_write_hex((uint32)targetAddr);
				irt_serial_write(" expected:\0");
				irt_serial_write_hex((uint32)pattern);
				irt_serial_write(" read:\0");
				irt_serial_write_hex((uint32)read);
				irt_serial_writeln("\0");
				return targetAddr;
			}
		}
	}

	*((uint8*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	irt_serial_write("memTestAddrBus8: check for address bits stuck low or shorted:\0");

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
							irt_serial_write("memTestAddrBus8: ERROR addr:\0");
							irt_serial_write_hex((uint32)targetAddr);
							irt_serial_write(" expected:\0");
							irt_serial_write_hex((uint32)pattern);
							irt_serial_write(" read:\0");
							irt_serial_write_hex((uint32)read);
							irt_serial_writeln("\0");
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint8*)testTargetAddr) = pattern;
		}
	}

	irt_serial_writeln("\n\rmemTestAddrBus8: done\0");

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

	irt_serial_write("memTestAddrBus16: base:\0");
	irt_serial_write_hex((uint32)baseAddr);
	irt_serial_write(", addrMask:\0");
	irt_serial_write_hex((uint32)addressMask);
	irt_serial_writeln("\0");

	irt_serial_write("memTestAddrBus16: write pattern:\0");
	irt_serial_write_hex((uint32)pattern);
	irt_serial_writeln(" to each of the power-of-two offsets\0");

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint16); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint16*)targetAddr) = pattern;
		}
	}

	irt_serial_write("memTestAddrBus16: check for address bits stuck high with antiPattern:\0");
	irt_serial_write_hex((uint32)antiPattern);
	irt_serial_writeln("\0");

	// check for address bits stuck high
	*((uint16*)baseAddr) = antiPattern;
	for (offset = sizeof(uint16); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint16*)targetAddr)) != pattern)
			{
				irt_serial_write("memTestAddrBus16: ERROR addr:\0");
				irt_serial_write_hex((uint32)targetAddr);
				irt_serial_write(" expected:\0");
				irt_serial_write_hex((uint32)pattern);
				irt_serial_write(" read:\0");
				irt_serial_write_hex((uint32)read);
				irt_serial_writeln("\0");
				return targetAddr;
			}
		}
	}

	*((uint16*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	irt_serial_writeln("memTestAddrBus16: check for address bits stuck low or shorted:\0");

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
							irt_serial_write("memTestAddrBus16: ERROR addr:\0");
							irt_serial_write_hex((uint32)targetAddr);
							irt_serial_write(" expected:\0");
							irt_serial_write_hex((uint32)pattern);
							irt_serial_write(" read:\0");
							irt_serial_write_hex((uint32)read);
							irt_serial_writeln("\0");
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint16*)testTargetAddr) = pattern;
		}
	}

	irt_serial_writeln("\n\rmemTestAddrBus16: done\0");

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
				handleError(targetAddr);
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
						//// bkbk diag_printf("  0x%x", targetAddr);
						if ((read = *((uint32*)targetAddr)) != pattern)
						{
							handleError(testTargetAddr);
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

	irt_serial_write("memTestRdWrMem8: base:\0");
	irt_serial_write_hex((uint32)baseAddr);
	irt_serial_write(", size:\0");
	irt_serial_write_hex((uint32)nBytes);
	if (4 == step)
	{
		irt_serial_write(", step:all(\0");
	}
	else
	{
		irt_serial_write(", size:jump(\0");
	}
	irt_serial_write_hex((uint32)step);
	irt_serial_writeln(")\0");
	
	irt_serial_writeln("memTestRdWrMem8: testing addresses:\0");
	irt_serial_write("memTestRdWrMem8: \0");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
		{
			targetAddr = baseAddr + offset;
			irt_serial_write_hex((uint32)targetAddr);
			irt_serial_write(",\0");

			offset++;
			if ((step !=4) && (offset%4==0)) offset+=step-4;
			if (i && (i % 4 == 0))
				{
					irt_serial_writeln("\0");
					irt_serial_write("memTestRdWrMem8: \0");
				}
			kick_wd();
		}
	irt_serial_writeln("...\0");
	irt_serial_writeln("memTestRdWrMem8: fill memory with a pattern\0");

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
		
	irt_serial_writeln("memTestRdWrMem8: verify memory pattern and fill with antipattern\0");

	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint8*)targetAddr)) != pattern)
			{
				irt_serial_write("memTestRdWrMem8: ERROR addr:\0");
				irt_serial_write_hex((uint32)targetAddr);
				irt_serial_write(" expected:\0");
				irt_serial_write_hex((uint32)pattern);
				irt_serial_write(" read:\0");
				irt_serial_write_hex((uint32)read);
				irt_serial_writeln("\0");
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

	irt_serial_writeln("memTestRdWrMem8: verify memory antipattern\0");

	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint8*)targetAddr)) != antiPattern)
			{
				irt_serial_write("memTestRdWrMem8: ERROR addr:\0");
				irt_serial_write_hex((uint32)targetAddr);
				irt_serial_write(" expected:\0");
				irt_serial_write_hex((uint32)antiPattern);
				irt_serial_write(" read:\0");
				irt_serial_write_hex((uint32)read);
				irt_serial_writeln("\0");
				return targetAddr;
			}
		}
		offset++;
		if(offset % 0x10000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	irt_serial_writeln("\n\rmemTestRdWrMem8: done\0");

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

	irt_serial_write("memTestRdWrMem16: base:\0");
	irt_serial_write_hex((uint32)baseAddr);
	irt_serial_write(", size:\0");
	irt_serial_write_hex((uint32)nBytes);
	if (4 == step)
	{
		irt_serial_write(", step:all(\0");
	}
	else
	{
		irt_serial_write(", step:jump(\0");
	}
	irt_serial_write_hex((uint32)step);
	irt_serial_writeln(")\0");

	irt_serial_writeln("memTestRdWrMem16: testing addresses:\0");
	irt_serial_write("memTestRdWrMem16: \0");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
		{
			targetAddr = baseAddr + offset;
			irt_serial_write_hex((uint32)targetAddr);
			irt_serial_write(",\0");
			offset+=2;
			if ((step !=4) && (offset%4==0)) offset+=step-4;
			if (i && (i % 4 == 0))
				{
					irt_serial_writeln("\0");
					irt_serial_write("memTestRdWrMem16: \0");
				}
		}
	irt_serial_writeln("...\0");
	irt_serial_writeln("memTestRdWrMem16: fill memory with a pattern\0");
		
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
		
	irt_serial_writeln("memTestRdWrMem16: verify memory pattern and fill with antipattern\0");

	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint16*)targetAddr)) != pattern)
			{
				irt_serial_write("memTestRdWrMem16: ERROR addr:\0");
				irt_serial_write_hex((uint32)targetAddr);
				irt_serial_write(" expected:\0");
				irt_serial_write_hex((uint32)pattern);
				irt_serial_write(" read:\0");
				irt_serial_write_hex((uint32)read);
				irt_serial_writeln("\0");
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
		
	irt_serial_writeln("memTestRdWrMem16: verify memory antiPattern\0");

	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint16*)targetAddr)) != antiPattern)
			{
				irt_serial_write("memTestRdWrMem16: ERROR addr:\0");
				irt_serial_write_hex((uint32)targetAddr);
				irt_serial_write(" expected:\0");
				irt_serial_write_hex((uint32)antiPattern);
				irt_serial_write(" read:\0");
				irt_serial_write_hex((uint32)read);
				irt_serial_writeln("\0");
				return targetAddr;
			}
		}
		offset+=2;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	irt_serial_writeln("\n\rmemTestRdWrMem16: done\0");

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
	
	irt_serial_write("memTestRdWrMem32: base:\0");
	irt_serial_write_hex((uint32)baseAddr);
	irt_serial_write(", size:\0");
	irt_serial_write_hex((uint32)nBytes);
	if (4 == step)
	{
		irt_serial_write(", step:all(\0");
	}
	else
	{
		irt_serial_write(", step:jump(\0");
	}
	irt_serial_write_hex((uint32)step);
	irt_serial_writeln(")\0");

	irt_serial_writeln("memTestRdWrMem32: testing addresses:\0");
	irt_serial_write("memTestRdWrMem32: \0");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
	{
		targetAddr = baseAddr + offset;
		irt_serial_write_hex((uint32)targetAddr);
		irt_serial_write(",\0");
		offset+=4;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
		if (i && (i % 4 == 0))
		{
			irt_serial_writeln("\0");
			irt_serial_write("memTestRdWrMem32: \0");
		}
	}
	irt_serial_writeln("...\0");
	irt_serial_writeln("memTestRdWrMem32: fill memory with a pattern\0");

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

	irt_serial_writeln("memTestRdWrMem32: verify memory pattern and fill with antipattern\0");
	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint32*)targetAddr)) != pattern)
			{
				handleError(targetAddr);
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
		
	irt_serial_writeln("memTestRdWrMem32: verify memory antiPattern\0");
	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint32*)targetAddr)) != antiPattern)
			{
				handleError(targetAddr);
				return targetAddr;
			}
		}
		offset+=4;
		if(offset % 0x100000 == 0) 
			kick_wd();
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
	irt_serial_writeln("\n\rmemTestRdWrMem32: done\0");

	return 0;
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
		irt_serial_writeln("\n\rdata bus test ... done.\0");

		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd
		
		if(memTestAddrBus8 (MEM_START, MEM_LEN, 0, 0x0)) 
			number_of_error++;
		if(memTestAddrBus16 (MEM_START, MEM_LEN, 0, 0x0)) 
			number_of_error++;
		if(memTestAddrBus32 (MEM_START, MEM_LEN, 0, 0x0)) 
			number_of_error++;
		irt_serial_writeln("\n\raddress bus test ... done.\0");

		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd
		irt_serial_writeln("************************************\0");
		irt_serial_write("This is \0");
		irt_serial_write_hex((uint32)_run);
		irt_serial_writeln(" run of the memTest.\0");
		irt_serial_writeln("************************************\0");
		
		if(memTestRdWrMem8 (MEM_START, MEM_LEN, 0, 0x0, 4))
			number_of_error++;
		if(memTestRdWrMem16 (MEM_START, MEM_LEN, 0, 0x0, 4))
			number_of_error++;
		if(memTestRdWrMem32 (MEM_START, MEM_LEN, 0, 0x0, 4))
			number_of_error++;

		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd

	}

	irt_serial_write("Total number of errors is: \0");
	irt_serial_write_dec((uint32)number_of_error);
	irt_serial_writeln("\0");
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

static char itoa_buf[32] = {0};
char* itoa(int val, int base){
		int i = 0;

		for (i=0;i<32;i++)
		{
			itoa_buf[i] = 0;
		}
		i = 30;
		for(; val && i ; --i, val /= base)
			itoa_buf[i] = "0123456789abcdef"[val % base];
		return &itoa_buf[i+1];
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
	irt_serial_writeln("\0");
	irt_serial_writeln("DICEII/Jr/Mini internal RAM tests:\0");
	irt_serial_writeln("   m count: Memory test\0");
	irt_serial_writeln("   x start_addr length_in_quadlet: display memory\0");
	irt_serial_writeln("   l/w/b address value: Set value\0");
	irt_serial_writeln("   ?/h: Help, this menu\0");
}

int 
process_cmd(char *cmd) {
	uint32 i = 0;
	uint32 addr, count, value;
	irt_serial_write(cmd);
	irt_serial_write("\0");

	switch (*cmd++) {
	case 'm':
		if(get_number(cmd, &count)) 
			mem_test(count);
		else
			irt_serial_writeln("Wrong parameters.\0");
		break;
	case 'g':
	case 'x':
		if((cmd = get_number(cmd, &addr))) {
			if(!(cmd = get_number(cmd, &count)))
				count = 1;
		} else {
			irt_serial_writeln("Wrong parameters.\0");
			break;
		}

		if(count < 1) count = 1;
		addr &= 0xFFFFFFFC;
		for(i = 0; i < count; i++, addr += 4) {
			if(addr % 16 == 0) {
				irt_serial_writeln("\0");
				irt_serial_write_hex(addr);
				irt_serial_write(": \0");
			}
			irt_serial_write(" \0");
			irt_serial_write_hex(*((uint32 *)addr));
		}
		irt_serial_writeln("\0");
		break;
	case 'l': 
		if ((cmd = get_number(cmd, &addr)) && (get_number(cmd, &value))) {
			*((uint32 *)addr) = value;
			irt_serial_write("  *\0");
			irt_serial_write_hex((uint32)addr);
			irt_serial_write(" = 0x\0");
			irt_serial_write_hex(*((uint32 *)addr));
			irt_serial_writeln("\0");
		} else 
			irt_serial_writeln("Wrong parameters.\0");
		break;
	case 'b': 
		if ((cmd = get_number(cmd, &addr)) && (get_number(cmd, &value))) {
			*((uint8 *)addr) = (uint8) value;

			irt_serial_write("  *\0");
			irt_serial_write_hex((uint32)addr);
			irt_serial_write(" = 0x\0");
			irt_serial_write_hex(*((uint8 *)addr));
			irt_serial_writeln("\0");
		} else 
			irt_serial_writeln("Wrong parameters.\0");
		break;
	case 'w': 
		if ((cmd = get_number(cmd, &addr)) && (get_number(cmd, &value))) {
			*((uint16 *)addr) = (uint16) value;

			// diag_printf("*0x%08x = %08x\n", addr, *((uint16 *)addr));
			irt_serial_write("  *\0");
			irt_serial_write_hex((uint32)addr);
			irt_serial_write(" = 0x\0");
			irt_serial_write_hex(*((uint16 *)addr));
			irt_serial_writeln("\0");
		} else 
			irt_serial_writeln("Wrong parameters.\0");
		break;
	case '?':
	case 'h':
		do_help();
		break;
	default:
		irt_serial_writeln("unknown command.\0");
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
		*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd
	}

	irt_serial_init();
	irt_serial_writeln("\n\rTC Applied Technologies\0");

	do_help(); 
	while(1) {
		irt_serial_write("$ \0");
		irt_gets(cmd_line_buf, LINE_SIZE);
		process_cmd(cmd_line_buf);
	}
}

cyg_bool irt_serial_getc_nonblock(char *ch) {
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

cyg_uint8 irt_serial_getc(void) {
    cyg_uint8 ch;
    CYGARC_HAL_SAVE_GP();

    while(!irt_serial_getc_nonblock(&ch)){
		*((unsigned long*)0xbf000004) = 0x5678; //hack, kick the wd
    };

    CYGARC_HAL_RESTORE_GP();
    return ch;
}

uint32 irt_gets(char *buf, uint32 len) {
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

void irt_serial_putc(char ch) {
    cyg_uint8* base = (cyg_uint8*)(UART0BASE);
    cyg_uint32 status;
    
    CYGARC_HAL_SAVE_GP();

    do {
        HAL_READ_UINT32(base+UART_LSR, status);
    } while ((status & BIT5) == 0);

    HAL_WRITE_UINT8(base+UART_THR, ch);

    CYGARC_HAL_RESTORE_GP();
}

void irt_serial_write(cyg_uint8* __buf)
{
	int len = 0;
    CYGARC_HAL_SAVE_GP();

	while((*__buf != 0) && (len++ < LINE_SIZE)) {
        irt_serial_putc(*__buf++);
	}

    CYGARC_HAL_RESTORE_GP();
}

void irt_serial_writeln(cyg_uint8* __buf)
{
	int len = 0;
	CYGARC_HAL_SAVE_GP();

    while((*__buf != 0) && (len++ < LINE_SIZE))
	{
        irt_serial_putc((char)*__buf++);
	}
	irt_serial_putc('\n');
	irt_serial_putc('\r');

    CYGARC_HAL_RESTORE_GP();
}

void irt_serial_write_hex(uint32 value)
{
	char *dst;
	char digits[32];
	char string[32];
	int i, n;

	dst = string;
	i = 0;
	do
	{
		n = value % 16;
		digits[i++] = (n < 10 ? (char)n+'0' : (char)n-10+'a');
		value /= 16;
	} while (value != 0);
	while (i > 0)
	{
		*dst++ = digits[--i];
	}
	*dst = 0;
	irt_serial_putc('0');
	irt_serial_putc('x');
	n = strlen(string);
	if (n < 8)
	{
		for (i=0;i<(8-n);i++)
				irt_serial_putc('0');
	}
	irt_serial_write(string);
}

void irt_serial_write_dec(uint32 value)
{
	char *buf = itoa(value, 10);
	if (0 == value)
	{
		irt_serial_write("0\0");
	}
	else
	{
		irt_serial_write(buf);
	}
}

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

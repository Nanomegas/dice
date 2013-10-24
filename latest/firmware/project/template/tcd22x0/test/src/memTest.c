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
/****************************************************************************
 * DESCRIPTION: 

	Memory Test Module for testing Memory Devices
		
	Revisions:
		created 07/08/2003 lm

*****************************************************************************/

#if defined(_CLI) && defined(_SYSDEBUG)
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "cli.h"
#include "memTest.h"
#include "sysDebug.h"

//static char 		moduleName[] = __FILE__;

#define MEM_TEST_ALL_BIT						0
#define MEM_TEST_8_BIT							8
#define MEM_TEST_16_BIT							16
#define MEM_TEST_32_BIT							32

HRESULT	memTestInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _CLI_TOOL_MEMTEST
	memTestCliInstallTools();
#endif //_CLI_TOOL_MEMTEST

	return hResult;
}

/*****************************************************************************

	memTestDataBus8	- Data bus test, byte access
	memTestDataBus16	- Data bus test, doublet access

		Walking ones test - write/read a single address with all the possible values
				that have only a single bit set (000001, 000010, 000100, etc)

	Returns:	pattern that failed
				0 if successful
				
*****************************************************************************/

uint32 memTestDataBus8 (volatile uint8 *baseAddr)
{
	uint8	pattern;
	uint8	read;

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus8: base:0x%08x\n\r", baseAddr);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus8: wr/rd single address with single bit set values\n\r");

	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", baseAddr, pattern, read);
			return ((uint32) baseAddr);
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus8: done\n\r");

	return 0;
}	
//*********************

uint32 memTestDataBus16 (volatile uint16 *baseAddr)
{
	uint16	pattern;
	uint16	read;

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus16: base:0x%08x\n\r", baseAddr);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus16: wr/rd single address with single bit set values\n\r");

	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", baseAddr, pattern, read);
			return ((uint32) baseAddr);
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus16: done\n\r");

	return 0;
}	
//*********************

uint32 memTestDataBus32 (volatile uint32 *baseAddr)
{
	uint32	pattern;
	uint32	read;

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus32: base:0x%08x\n\r", baseAddr);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus32: wr/rd single address with single bit set values\n\r");

	for (pattern = 1; pattern != 0; pattern <<= 1 )
	{
		*baseAddr = pattern;
		if ((read = *baseAddr) != pattern)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus32: ERROR addr:0x%08x expected:0x%08x read:0x%08x\n\r", baseAddr, pattern, read);
			return ((uint32) baseAddr);
		}
	}
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestDataBus32: done\n\r");

	return 0;
}	
/*****************************************************************************

	Address bus test

		Parameters for excluded region are for benefit of the Dhiva_X platform where
		msp registers overlay a portion of sdram.


	Returns address which failed, 0 if successful

*****************************************************************************/

static	uint32	firstExcludeAddress; 
static	uint32	numExcludeBytes;

static BOOL	addressExcluded(uint32 addr)		// statics above must have been set up properly
{
	return(	numExcludeBytes &&
			(addr >= firstExcludeAddress) &&
			(addr < (firstExcludeAddress+numExcludeBytes)));
}	
//*********************

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

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus8: base:0x%08x, addrMask:0x%08x\n\r", baseAddr, addressMask);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus8: write pattern:0x%02x to each of the power-of-two offsets\n\r", pattern);

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint8); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint8*)targetAddr) = pattern;
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus8: check for address bits stuck high with antiPattern:0x%02x\n\r", antiPattern);

	// check for address bits stuck high
	*((uint8*)baseAddr) = antiPattern;
	for (offset = sizeof(uint8); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint8*)targetAddr)) != pattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
		}
	}

	*((uint8*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus8: check for address bits stuck low or shorted:\n\r");

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
							SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, pattern, read);
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint8*)testTargetAddr) = pattern;
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus8: done\n\r");

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

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus16: base:0x%08x, addrMask:0x%08x\n\r", baseAddr, addressMask);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus16: write pattern:0x%04x to each of the power-of-two offsets\n\r", pattern);

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint16); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint16*)targetAddr) = pattern;
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus16: check for address bits stuck high with antiPattern:0x%04x\n\r", antiPattern);

	// check for address bits stuck high
	*((uint16*)baseAddr) = antiPattern;
	for (offset = sizeof(uint16); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint16*)targetAddr)) != pattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
		}
	}

	*((uint16*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus16: check for address bits stuck low or shorted:\n\r");

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
							SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, pattern, read);
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint16*)testTargetAddr) = pattern;
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus16: done\n\r");

	return 0;
}	
//*********************

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

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus32: base:0x%08x, addrMask:0x%08x\n\r", baseAddr, addressMask);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus32: write pattern:0x%08x to each of the power-of-two offsets\n\r", pattern);

	// write pattern to each of the power-of-two offsets
	for (offset = sizeof(uint32); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint32*)targetAddr) = pattern;
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus32: check for address bits stuck high with antiPattern:0x%08x\n\r", antiPattern);

	// check for address bits stuck high
	*((uint32*)baseAddr) = antiPattern;
	for (offset = sizeof(uint32); (offset & addressMask) != 0; offset <<=1)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint32*)targetAddr)) != pattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus32: ERROR addr:0x%08x expected:0x%08x read:0x%08x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
		}
	}

	*((uint32*)baseAddr) = pattern;		// all test addresses now contain 'pattern'

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus32: check for address bits stuck low or shorted:\n\r");

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
						if ((read = *((uint32*)targetAddr)) != pattern)
						{
							SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus32: ERROR addr:0x%08x expected:0x%08x read:0x%08x\n\r", targetAddr, pattern, read);
							return testTargetAddr;
						}
					}
				}
			}
			// restore original pattern
			*((uint32*)testTargetAddr) = pattern;
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestAddrBus32: done\n\r");

	return 0;
}	
/*****************************************************************************

	Device Test

		Fill memory with pattern then verify.
		This a 2-pass test, so every memory location gets tested with each bit as 0 and 1.

	Returns address which failed, 0 if successful

*****************************************************************************/

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

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: base:0x%08x, size:0x%08x, step:%s(0x%08x)\n\r", baseAddr, nBytes, (step==4) ? "all" : "jump", step);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: testing addresses:\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: ");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
	{
		targetAddr = baseAddr + offset;
		SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "0x%08x,", targetAddr);
		offset++;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
		if (i && (i % 4 == 0))
		{
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "\n\r");
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: ");
		}
	}
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "...\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: fill memory with a pattern\n\r");

	// fill memory with a pattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint8*)targetAddr) = pattern;
		}
		offset++;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: verify memory pattern and fill with antipattern\n\r");

	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint8*)targetAddr)) != pattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
			antiPattern = ~pattern;
			*((uint8*)targetAddr) = antiPattern;
		}
		offset++;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: verify memory antiPattern\n\r");

	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint8*)targetAddr)) != antiPattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: ERROR addr:0x%08x expected:0x%02x read:0x%02x\n\r", targetAddr, antiPattern, read);
				return targetAddr;
			}
		}
		offset++;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem8: done\n\r");

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

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: base:0x%08x, size:0x%08x, step:%s(0x%08x)\n\r", baseAddr, nBytes, (step==4) ? "all" : "jump", step);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: testing addresses:\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: ");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
	{
		targetAddr = baseAddr + offset;
		SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "0x%08x,", targetAddr);
		offset+=2;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
		if (i && (i % 4 == 0))
		{
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "\n\r");
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: ");
		}
	}
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "...\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: fill memory with a pattern\n\r");

	// fill memory with a pattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint16*)targetAddr) = pattern;
		}
		offset+=2;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: verify memory pattern and fill with antipattern\n\r");

	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint16*)targetAddr)) != pattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
			antiPattern = ~pattern;
			*((uint16*)targetAddr) = antiPattern;
		}
		offset+=2;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: verify memory antiPattern\n\r");

	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint16*)targetAddr)) != antiPattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: ERROR addr:0x%08x expected:0x%04x read:0x%04x\n\r", targetAddr, antiPattern, read);
				return targetAddr;
			}
		}
		offset+=2;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem16: done\n\r");

	return 0;
}	
//*********************

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

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: base:0x%08x, size:0x%08x, step:%s(0x%08x)\n\r", baseAddr, nBytes, (step==4) ? "all" : "jump", step);
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: testing addresses:\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: ");
	for (offset = 0, i = 0; offset < nBytes && i < 8; i++)
	{
		targetAddr = baseAddr + offset;
		SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "0x%08x,", targetAddr);
		offset+=4;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
		if (i && (i % 4 == 0))
		{
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "\n\r");
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: ");
		}
	}
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "...\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: fill memory with a pattern\n\r");

	// fill memory with a pattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			*((uint32*)targetAddr) = pattern;
		}
		offset+=4;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: verify memory pattern and fill with antipattern\n\r");

	// verify memory pattern and fill with antipattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			if ((read = *((uint32*)targetAddr)) != pattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: ERROR addr:0x%08x expected:0x%08x read:0x%08x\n\r", targetAddr, pattern, read);
				return targetAddr;
			}
			antiPattern = ~pattern;
			*((uint32*)targetAddr) = antiPattern;
		}
		offset+=4;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}
		
	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: verify memory antiPattern\n\r");

	// verify memory antiPattern
	for (pattern = 3, offset = 0; offset < nBytes; pattern++)
	{
		targetAddr = baseAddr + offset;
		if (!addressExcluded(targetAddr))
		{
			antiPattern = ~pattern;
			if ((read = *((uint32*)targetAddr)) != antiPattern)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: ERROR addr:0x%08x expected:0x%08x read:0x%08x\n\r", targetAddr, antiPattern, read);
				return targetAddr;
			}
		}
		offset+=4;
		if ((step !=4) && (offset%4==0)) offset+=step-4;
	}

	SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "memTestRdWrMem32: done\n\r");

	return 0;
}	

#define TEST_TYPE_DATA	0x01
#define TEST_TYPE_ADDR	0x02
#define TEST_TYPE_RDWR	0x04
#define TEST_TYPE_ALL	0x0F

uint32 memTestRun (uint32 type, uint32 baseAddr, uint32 rangeSize, uint32 bitWidth, uint32 step)
{
	uint32 baseTestAddress, baseExcludeAddress, excludeAddressSize;
	uint32 failedPattern;

	baseTestAddress = baseAddr;
	baseExcludeAddress = baseAddr;
	excludeAddressSize = 0;

	switch (bitWidth)
	{
		case MEM_TEST_8_BIT:
			if (type & TEST_TYPE_DATA)
			{
				failedPattern = memTestDataBus8((uint8 *)baseExcludeAddress + excludeAddressSize);
				if (failedPattern) return (uint32)failedPattern;
			}
			if (type & TEST_TYPE_ADDR)
			{
				failedPattern = memTestAddrBus8(baseTestAddress, \
													rangeSize, 
													baseExcludeAddress, 
													excludeAddressSize);
				if (failedPattern) return (uint32)failedPattern;
			}
			if (type & TEST_TYPE_RDWR)
			{
				failedPattern = memTestRdWrMem8(baseTestAddress, \
													rangeSize, 
													baseExcludeAddress, 
													excludeAddressSize,
													step);
				if (failedPattern) return (uint32)failedPattern;
			}
			break;
		case MEM_TEST_16_BIT:
			if (type & TEST_TYPE_DATA)
			{
				failedPattern = memTestDataBus16((uint16 *)baseExcludeAddress + excludeAddressSize);
				if (failedPattern) return (uint32)failedPattern;
			}
			if (type & TEST_TYPE_ADDR)
			{
				failedPattern = memTestAddrBus16(baseTestAddress, \
													rangeSize, 
													baseExcludeAddress, 
													excludeAddressSize);
				if (failedPattern) return (uint32)failedPattern;
			}
			if (type & TEST_TYPE_RDWR)
			{
				failedPattern = memTestRdWrMem16(baseTestAddress, \
													rangeSize, 
													baseExcludeAddress, 
													excludeAddressSize,
													step);
				if (failedPattern) return (uint32)failedPattern;
			}
			break;
		case MEM_TEST_32_BIT:
			if (type & TEST_TYPE_DATA)
			{
				failedPattern = memTestDataBus32((uint32 *)baseExcludeAddress + excludeAddressSize);
				if (failedPattern) return (uint32)failedPattern;
			}
			if (type & TEST_TYPE_ADDR)
			{
				failedPattern = memTestAddrBus32(baseTestAddress, \
													rangeSize, 
													baseExcludeAddress, 
													excludeAddressSize);
				if (failedPattern) return (uint32)failedPattern;
			}
			if (type & TEST_TYPE_RDWR)
			{
				failedPattern = memTestRdWrMem32(baseTestAddress, \
													rangeSize, 
													baseExcludeAddress, 
													excludeAddressSize,
													step);
				if (failedPattern) return (uint32)failedPattern;
			}
			break;
	}
	return 0;
}

HRESULT memTest(uint32 type, uint32 baseAddr, uint32 rangeSize, uint32 bitWidth, uint32 step, BOOL verbose)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		tests = 0;
	uint32		width = 0;
	uint32		error = 0;
	uint32		mask = 0;

	switch (bitWidth)
	{
		case MEM_TEST_8_BIT:
		case MEM_TEST_16_BIT:
		case MEM_TEST_32_BIT:
			width = bitWidth;
			tests = 1;
			break;
		case MEM_TEST_ALL_BIT:
			width = MEM_TEST_8_BIT;
			tests = 3;
			break;
		default: 
			return E_BAD_INPUT_PARAMETERS;
	}

	if (step == 0) step = 4;
	if (step % 4 != 0) return E_BAD_INPUT_PARAMETERS;

	if (type == 0) type = TEST_TYPE_ALL;

	if (verbose)
	{
		mask = sysDebugGetEnabled();
		sysDebugSetEnabled(SYSDEBUG_TRACE_TESTS);
	}

	for (i = 0; i < tests; i++)
	{
		error = memTestRun(type, baseAddr, rangeSize, width, step);
		if (error)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_TESTS, "error: 0x%08x\n\r", error);
		}

		width *= 2;
	}

	if (verbose)
	{
		sysDebugSetEnabled(mask);
	}

	if (error) hResult = E_FAIL;

	return hResult;
}

#ifdef _CLI_TOOL_MEMTEST

// MemTest
static  char	memTest_UsageString[]=\
				"memtest: \n\r"\
				"use: memtest <type> <baseaddr> <rangesize> <bitwidth> <step> <verbose>\n\r"\
				"<type>: data,addr,rdwr,all\n\r"\
				"<bitwidth>: 8b,16b,32b,all\n\r"\
				"<step>: all: default, testing all addresses (step=4)\n\r"\
				"       o.w.: value in steps of 4 (step=4,8,16,...)\n\r"\
				"<verbose>: on|off - output test messages\n\r"\
				"------------------------------------------------------------\n\r"\
				"test memory area using specific bit width access\n\r";

static	CLIDescriptor	memTest_Descriptor[]=
{
	{kCLI_Name,				(uint32)"memtest"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&memTest},
	{kCLI_UsageString, 		(uint32)memTest_UsageString},
	{kCLI_ConstantsString,	(uint32)"8b=8,16b=16,32b=32,all=0,data=1,addr=2,rdwr=4,off=0,on=1"},
	{kCLI_ArgumentPatternString, (uint32)"iiiiii"},
	{kCLI_ModuleName,		(uint32)"run memory test"},
	{kCLI_GroupName,		(uint32)"memtest"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void memTestCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(memTest_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing memTest cli tools");
	}
}

#endif //_CLI_TOOL_MEMTEST
#endif //_CLI && _SYSDEBUG
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

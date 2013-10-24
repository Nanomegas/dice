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

 /*********************************************************************
 * DESCRIPTION: 

	This module implements the 
		
	DSP Module for DSP HI08 interface
		
	Revisions:
		created 12/11/2004 lm

	Comment:
	This module is an example of host communication with a Motorola 56300 DSP.
	The code assumes that the DSP code implements host interrupts to read and
	write DSP memory locations.
	
	Hardware assumptions:
	- DSP HI08 port is connected to CS4
	- The Data Bus is connected to DICEII D8..D15
	- The DSP reset pin is connected to GPIO2 (Specific to DICE2_EVAL_BOARD)
	- The DSP mode pins are set to HDI08 ISA Mode (Mode C), 
		on the DICE2_EVAL_BOARD:
		R310, R311, R314 Mounted
		R309, R315, R316 Not Mounted
		GPIO4 Should be held low

***********************************************************************/

#include "TCTasking.h"
#include "diceDefs.h"
#include "coreDefs.h"
#include "dsp.h"

#ifdef _CLI
#include "cli.h"
#endif

static char 		moduleName[] = __FILE__;

// semaphore IDs for this module
uint32				dspSemID;										// mutual exclusion semaphore for access to dsp (mutex)
	
static BOOL	dspbHostOk = FALSE;

static void dspEnableHI08(void)
{
	uint32 iosel = 0;

	iosel = *((volatile uint32 *) GPCSR_IO_SEL);
	iosel &= 0xFFFFFE7F;
	iosel |= 0x080;
	*((volatile uint32 *) GPCSR_IO_SEL) = iosel;							// Enable ncs4 in GPCSR
	*((volatile uint32 *) (MEMCTLBASE + 0x24)) = DSP_BASE_ADDR;			 	// Map ncs4 to address
	*((volatile uint32 *) (MEMCTLBASE + 0x64)) = 0x00000121; 					// Setup ncs4 mask register SMSKR4 to SMTMGR_SET01, SRAM16 & 64KB
}

static HRESULT dspHostOk(void)
{
	HRESULT	hResult = NO_ERROR;

	if (dspbHostOk == FALSE)
	{
		hResult = E_DSP_HOST_OK;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

static HRESULT dspWaitTRDY(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32	startTime = TCTimeGet();

	DO_FOREVER
	{
		if (hi08Read(DSP_ISR_REG) & ISR_TRDY_BIT) break;
		if (TCTimeElapsed(startTime) > DSP_TIMEOUT_MSECS)
		{
			hResult = E_DSP_WAITTRDY;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
	}

	return hResult;
}

static HRESULT dspWaitNOTHC(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32	startTime = TCTimeGet();

	DO_FOREVER
	{
		if (!(hi08Read(DSP_CVR_REG) & 0x80)) break;
		if (TCTimeElapsed(startTime) > DSP_TIMEOUT_MSECS)
		{
			hResult = E_DSP_WAITNOTHC;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
	}

	return hResult;
}

static HRESULT dspWaitTXDE(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32	startTime = TCTimeGet();

	DO_FOREVER
	{
		if (hi08Read(DSP_ISR_REG) & ISR_TXDE_BIT) break;
//		if ((hi08Read(DSP_ISR_REG) & 0x06) == ISR_TXDE_BIT) break;
		if (TCTimeElapsed(startTime) > DSP_TIMEOUT_MSECS)
		{
			hResult = E_DSP_WAITTXDE;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
	}

	return hResult;
}

static HRESULT dspWaitRXDF(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32	startTime = TCTimeGet();

	DO_FOREVER
	{
		if (hi08Read(DSP_ISR_REG) & ISR_RXDF_BIT) break;
		if (TCTimeElapsed(startTime) > DSP_TIMEOUT_MSECS)
		{
			hResult = E_DSP_WAITTRDY;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
	}

	return hResult;
}

static HRESULT dspWaitHF3(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32	startTime = TCTimeGet();

	DO_FOREVER
	{
		if (hi08Read(DSP_ISR_REG) & ISR_HF3_BIT) break;
		if (TCTimeElapsed(startTime) > DSP_TIMEOUT_MSECS)
		{
			hResult = E_DSP_WAITTRDY;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
	}

	return hResult;
}

static HRESULT dspPutValue(uint32 value, BYTE cvr)
{
	HRESULT	hResult = NO_ERROR;

	hResult = dspWaitTRDY();
	if (hResult != NO_ERROR) return hResult;

	hResult = dspWaitNOTHC();
	if (hResult != NO_ERROR) return hResult;

	hi08Write(DSP_H_REG, value >> 16);
	hi08Write(DSP_M_REG, value >> 8);
	hi08Write(DSP_L_REG, value);

	hResult = dspWaitTXDE();
	if (hResult != NO_ERROR) return hResult;

	hi08Write(DSP_CVR_REG, cvr);
	hi08Write(DSP_CVR_REG, cvr | 0x80);

	return hResult;
}

static HRESULT dspGetValue(uint32 *value, BYTE cvr)
{
	HRESULT	hResult = NO_ERROR;

	hResult = dspWaitNOTHC();
	if (hResult != NO_ERROR) return hResult;

	hi08Write(DSP_CVR_REG, cvr);
	hi08Write(DSP_CVR_REG, cvr | 0x80);

	hResult = dspWaitRXDF();
	if (hResult != NO_ERROR) return hResult;

	*value = hi08Read(DSP_H_REG);
	*value <<= 8;
	*value |= hi08Read(DSP_M_REG);
	*value <<= 8;
	*value |= hi08Read(DSP_L_REG);

	return hResult;
}

static HRESULT dspPutCode(BYTE *code, uint32 size, uint32 start)
{
	HRESULT	hResult = NO_ERROR;
	BYTE	*p = code;
	uint32	i;

#ifdef _SYSDEBUG
	sysDebugPrintf("dspPutCode: put size\n\r");
#endif //_SYSDEBUG

	hi08Write(DSP_H_REG, size >> 16);
	hi08Write(DSP_M_REG, size >> 8);
	hi08Write(DSP_L_REG, size);

	hResult = dspWaitTXDE();
	if (hResult != NO_ERROR) return hResult;
#ifdef _SYSDEBUG
	sysDebugPrintf("dspPutCode: put start\n\r");
#endif //_SYSDEBUG
	hi08Write(DSP_H_REG, start >> 16);
	hi08Write(DSP_M_REG, start >> 8);
	hi08Write(DSP_L_REG, start);

	hResult = dspWaitTXDE();
	if (hResult != NO_ERROR) return hResult;
#ifdef _SYSDEBUG	
	sysDebugPrintf("dspPutCode: put code\n\r");
#endif //_SYSDEBUG
	for (i = 0; i < size; i++)
	{
		hi08Write(DSP_H_REG, *p++);
		hi08Write(DSP_M_REG, *p++);
		hi08Write(DSP_L_REG, *p++);

		hResult = dspWaitTXDE();
		if (hResult != NO_ERROR) return hResult;
	}
#ifdef _SYSDEBUG
	sysDebugPrintf("dspPutCode: wait hf3\n\r");
#endif //_SYSDEBUG
	hResult = dspWaitHF3();
	if (hResult != NO_ERROR) return hResult;
#ifdef _SYSDEBUG
	sysDebugPrintf("dspPutCode: finished\n\r");
#endif //_SYSDEBUG
	return hResult;
}

__inline HRESULT dspSetAddr(uint32 addr)
{
	return dspPutValue(addr, DSP_ADDR);
}

static HRESULT dspReadData(uint8 mem, uint32 *data)
{
	HRESULT	hResult = NO_ERROR;
	BYTE	cvr = 0;

	switch (mem)
	{
		case X_MEM: cvr = DSP_READX; break;
		case Y_MEM: cvr = DSP_READY; break;
		case P_MEM: cvr = DSP_READP; break;
		default: 
			hResult = E_BAD_INPUT_PARAMETERS;
	}

	hResult = dspGetValue(data, cvr);

	return hResult;
}

static HRESULT dspWriteData(uint8 mem, uint32 data)
{
	HRESULT	hResult = NO_ERROR;
	BYTE	cvr = 0;

	switch (mem)
	{
		case X_MEM: cvr = DSP_WRITEX; break;
		case Y_MEM: cvr = DSP_WRITEY; break;
		case P_MEM: cvr = DSP_WRITEP; break;
		default: 
			hResult = E_BAD_INPUT_PARAMETERS;
	}

	hResult = dspPutValue(data, cvr);

	return hResult;
}

BOOL dspIsPresent(void)
{
	return (hi08Read(DSP_ICR_REG) == 0x00 && 
			hi08Read(DSP_CVR_REG) == DSP_ADDR &&									// 0x32
			hi08Read(DSP_ISR_REG) == (ISR_TXDE_BIT | ISR_TRDY_BIT));				// 0x06
}

BOOL dspIsPresentAndRunning(void)
{
	// the dsp application loaded must set ISR_HF3_BIT to indicate code is running

	return (hi08Read(DSP_ICR_REG) == 0x00 && 
			hi08Read(DSP_CVR_REG) == DSP_ADDR &&									// 0x32
			hi08Read(DSP_ISR_REG) == (ISR_TXDE_BIT | ISR_TRDY_BIT | ISR_HF3_BIT));	// 0x16
}

HRESULT dspCheckPresentAndRunning(void)
{
	HRESULT	hResult = NO_ERROR;
	
	// assume dsp code loaded (either using dspLoadCode() or dsp debugger)
	dspbHostOk = dspIsPresentAndRunning();

	if (dspbHostOk == FALSE)
	{
		hResult = E_DSP_HOST_OK;
		sysLogError(hResult, __LINE__, moduleName);
#ifdef _SYSDEBUG
		sysDebugPrintf("dsp code not loaded correctly (either use dspLoadCode() or dsp debugger)\n\r");
#endif //_SYSDEBUG
		return hResult;
	}

	return hResult;
}

HRESULT dspReset(void)
{
	HRESULT	hResult = NO_ERROR;
	
	// RESET pin is available on the EVAL board
    *((volatile uint32 *) GPIO_A_DR)&= ~0x0000000a;  //Reset low, Mode low
    TCTaskWait(100);
    *((volatile uint32 *) GPIO_A_DR)|= 0x00000002;   //Reset high
    TCTaskWait(100);	

	dspbHostOk = FALSE;

	if (dspIsPresent() == FALSE)
	{
		hResult = E_DSP_ISDSP;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT dspReadMem(uint8 mem, uint32 addr, uint32 *data)
{
	HRESULT	hResult = NO_ERROR;

	hResult = dspHostOk();
	if (hResult != NO_ERROR) return hResult;

	// exclusive access for access to dsp (mutex)
	hResult = TCMutexLock(dspSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = dspSetAddr(addr);

	if (hResult == NO_ERROR)
	{
		hResult = dspReadData(mem, data);
	}

	// exclusive access for access to dsp (mutex)
	TCMutexUnlock(dspSemID);

	return hResult;
}

HRESULT dspWriteMem(uint8 mem, uint32 addr, uint32 data)
{
	HRESULT	hResult = NO_ERROR;

	hResult = dspHostOk();
	if (hResult != NO_ERROR) return hResult;

	// exclusive access for access to dsp (mutex)
	hResult = TCMutexLock(dspSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = dspSetAddr(addr);

	if (hResult == NO_ERROR)
	{
		hResult = dspWriteData(mem, data);
	}

	// exclusive access for access to dsp (mutex)
	TCMutexUnlock(dspSemID);

	return hResult;
}

HRESULT dspLoadCode(BYTE *code, uint32 size, uint32 start)
{
	HRESULT	hResult = NO_ERROR;

	hResult = dspReset();
	if (hResult != NO_ERROR) return hResult;

	// exclusive access for access to dsp (mutex)
	hResult = TCMutexLock(dspSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = dspPutCode(code, size, start);

	// exclusive access for access to dsp (mutex)
	TCMutexUnlock(dspSemID);

	return hResult;
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT dspCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = TCMutexOpen(&dspSemID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT dspInitialize(void)
{
	HRESULT			hResult = NO_ERROR;


	dspEnableHI08();

	hResult = dspCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

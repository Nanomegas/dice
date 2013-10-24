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

	AVS Test module recording AVS events
		
	Revisions:
		created 09/01/2005 ml

*****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "cli.h"
#include "avsTest.h"
#include "avs.h"
#include "coreDefs.h"
#include "llcDefs.h"
#include <cyg/kernel/kapi.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/plf_io.h>



HRESULT	avsTestInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	avsTestCliInstallTools();

	return hResult;
}


/*****************************************************************************

	Set-up timer to count at 49.152MHz clock rate.
	
	avsIntSuspendTasks (make sure no one touches the AVS)
	
	Install AVS int routines.
	
	The interrupt routines will gather events and time-stamp them with the timer counter
	
				
*****************************************************************************/

typedef struct
{
	uint32 time;
	uint32 avsInt0;
	uint32 avsInt1;
	uint32 avsInt2;
	uint32 phdr;
	uint32 cip0;
	uint32 cip1;
} AVS_LOG;

#define AVS_LOG_SIZE 8000

static AVS_LOG avsLog[AVS_LOG_SIZE+1];
static uint32 avsLogItems;

//generic, used for all interrupts
static cyg_uint32 avsGeneric_ISR(cyg_vector_t vector, cyg_addrword_t data)
{
	AVS_LOG * pLog = &avsLog[avsLogItems];
	avsEnvRegRead(APBA_INT0_STATUS, &pLog->avsInt0);
	avsEnvRegWrite(APBA_INT0_STATUS, pLog->avsInt0);
	avsEnvRegRead(APBA_INT1_STATUS, &pLog->avsInt1);
	avsEnvRegWrite(APBA_INT1_STATUS, pLog->avsInt1);
	avsEnvRegRead(APBA_INT2_STATUS, &pLog->avsInt2);
	avsEnvRegWrite(APBA_INT2_STATUS, pLog->avsInt2);
	
	avsEnvRegRead(APBA_ARXDO1_BASE + ARX_PHDR, &pLog->phdr);
	avsEnvRegRead(APBA_ARXDO1_BASE + ARX_CIP0, &pLog->cip0);
	avsEnvRegRead(APBA_ARXDO1_BASE + ARX_CIP1, &pLog->cip1);
	
	pLog->time =  *((volatile int *)(LLCBASE+CY_TMR_REG_DP));
	if (avsLogItems < AVS_LOG_SIZE)	avsLogItems++;
	
	return CYG_ISR_HANDLED;
}


static void avsGeneric_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {
    
}

cyg_interrupt			avs0_interrupt;
cyg_handle_t			avs0_interrupt_handle;

cyg_interrupt			avs1_interrupt;
cyg_handle_t			avs1_interrupt_handle;

cyg_interrupt			avs2_interrupt;
cyg_handle_t			avs2_interrupt_handle;


static int8 bInitialized;

static void InstallAvsHandler (void)
{

	if (bInitialized) return;
	
	cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_AVS0,
                             99,
                             (cyg_addrword_t)0,
                             avsGeneric_ISR,
                             avsGeneric_DSR,
                             &avs0_interrupt_handle,
                             &avs0_interrupt);
    cyg_drv_interrupt_attach(avs0_interrupt_handle);

	cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_AVS1,
                             99,
                             (cyg_addrword_t)0,
                             avsGeneric_ISR,
                             avsGeneric_DSR,
                             &avs1_interrupt_handle,
                             &avs1_interrupt);
    cyg_drv_interrupt_attach(avs1_interrupt_handle);
    
   	cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_AVS2,
                             99,
                             (cyg_addrword_t)0,
                             avsGeneric_ISR,
                             avsGeneric_DSR,
                             &avs2_interrupt_handle,
                             &avs2_interrupt);
    cyg_drv_interrupt_attach(avs2_interrupt_handle);
    
    bInitialized = true;
}    
    
    
   
HRESULT avsTestStartCapture (int time)
{
	avsLogItems = 0;
	uint32  timeEnd, timeCur, wasBigger, isBigger;
	
	//timerRegWrite(TIMER2_CTRL_REG, 4);
	//timerRegWrite(TIMER2_LOAD_CNT, 0xffffffff);
	//timerRegWrite(TIMER2_CTRL_REG, 5);
	InstallAvsHandler ();
	
	timeCur = *((volatile int *)(LLCBASE+CY_TMR_REG_DP));
	timeEnd = timeCur + time*0x02000000;
	wasBigger = (timeEnd > timeCur) ? 1 : 0;
	
	
	cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_AVS0);
	cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_AVS1);
	cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_AVS2);
	
	//avsRegWrite(APBA_PARTITION4_FLUSH,1);
	
    while (1)
    {
      if (avsLogItems >= AVS_LOG_SIZE) break;
      timeCur = *((volatile int *)(LLCBASE+CY_TMR_REG_DP));
      isBigger = (timeEnd > timeCur) ? 1 : 0;
      if (wasBigger != isBigger) break;    
      TCTaskYield();
    } 

	cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_AVS0);
	cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_AVS1);
	cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_AVS2);
    
    sysDebugPrintf("Captured %i items\n\r",avsLogItems);
    return NO_ERROR;
}

static void printTime (uint32 time)
{
	uint32 sec,cyc, ofs;
	
	sec = (time & 0xFE000000) >> 25;
	cyc = (time & 0x01FFF000) >> 12;
	ofs = (time & 0x00000FFF);
	cliPrintf ("%03i:%04i:%04i ",sec,cyc,ofs);
}





/*
		IRQ_ADO1_LOCKED,          LO
		IRQ_ADO1_STREAM_END,      SE
		IRQ_ADO1_STREAM_START,    SS

		IRQ_ARXDO1_SYT_OVERFLOW,  SO
		IRQ_ARXDO1_SYT_UNDERFLOW, SU
		IRQ_ADO1_REPEAT,          RP
		IRQ_ADO1_SLIP,            SL
		IRQ_ADO1_SYT_AGEOUT,      AG


		IRQ_ITP_EP_TOO_BIG,       IT
		IRQ_ARX1_CFG_FAIL,		  CG
		IRQ_ARX1_CIP_FAIL,        CI
		IRQ_ARX1_DBC_FAIL,        DB
		IRQ_ARX1_LONG_PKT,        LP
		IRQ_ARX1_PKT_ABORT,       PA
		IRQ_ARX1_STATUS_ERR,      SA
*/

HRESULT avsTestDumpLog (void)
{
	uint32 i;
	AVS_LOG * pLog;
	
	cliPrintf ("Time           LO SE SS SO SU RP SL AG IT CG CI DB LP PA SA PHDR     CIP0     CIP1\n\r");
	
	for (i = 0; i < avsLogItems; i++)
	{
		pLog = &avsLog[i];
		printTime(pLog->time);
		cliPrintf ((pLog->avsInt0 & IRQ_ADO1_LOCKED) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt0 & IRQ_ADO1_STREAM_END) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt0 & IRQ_ADO1_STREAM_START) ? "1  " : "   ");
		
		cliPrintf ((pLog->avsInt1 & IRQ_ARXDO1_SYT_OVERFLOW) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt1 & IRQ_ARXDO1_SYT_UNDERFLOW) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt1 & IRQ_ADO1_REPEAT) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt1 & IRQ_ADO1_SLIP) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt1 & IRQ_ADO1_SYT_AGEOUT) ? "1  " : "   ");
		
		cliPrintf ((pLog->avsInt2 & IRQ_ITP_EP_TOO_BIG) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt2 &	IRQ_ARX1_CFG_FAIL) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt2 &	IRQ_ARX1_CIP_FAIL) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt2 &	IRQ_ARX1_DBC_FAIL) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt2 & IRQ_ARX1_LONG_PKT) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt2 & IRQ_ARX1_PKT_ABORT) ? "1  " : "   ");
		cliPrintf ((pLog->avsInt2 & IRQ_ARX1_STATUS_ERR) ? "1  " : "   ");
		
		cliPrintf ("%08X ",pLog->phdr);
		cliPrintf ("%08X ",pLog->cip0);
		cliPrintf ("%08X ",pLog->cip1);
//		if (i)
//		{
//			printTime(pLog->time - avsLog[i-1].time);
//		}
		cliPrintf ("\n\r");
	}
	return NO_ERROR;
}	
		
static  char	avsCapture_UsageString[]=\
				"capture: \n\r"\
				"use: capture <seconds>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Capture <seconds> sec of avs events, max 8000\n\r";

static	CLIDescriptor	avsCapture_Descriptor[]=
{
	{kCLI_Name,				(uint32)"capture"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsTestStartCapture},
	{kCLI_UsageString, 		(uint32)avsCapture_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"Capture avs events"},
	{kCLI_GroupName,		(uint32)"avstst"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsDump_UsageString[]=\
				"dump: \n\r"\
				"use: dump\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dume captured avs events\n\r";

static	CLIDescriptor	avsDump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsTestDumpLog},
	{kCLI_UsageString, 		(uint32)avsDump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"Dump avs events"},
	{kCLI_GroupName,		(uint32)"avstst"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

HRESULT avsTestCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(avsCapture_Descriptor);
	hResult += cliInstallCLIDescriptor(avsDump_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avsTest cli tools");
	}
	return hResult;
}

#endif //_CLI

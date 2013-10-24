/*===================================================================*
  Copyright (c) 2002 TC Applied Technologies Ltd.
  -------------------------------------------------------------------
  $Header: /usr/local/cvsroot/firmware/os/ecos/TCKernel/src/TCKernelCli.c,v 1.2 2007/02/08 17:59:33 zluo Exp $
 *===================================================================*/ 

 /*********************************************************************
 * DESCRIPTION: 

	TCKernel Module CLI
		
	Revisions:
		created 12/11/2004 lm

***********************************************************************/


#ifdef _CLI
#include "TCTypes.h"
#include "TCTasking.h"
#include "TCKernelCli.h"
#include "cliToolDefs.h"
#include "cli.h"
#include <cyg/hal/plf_io.h>
#include <cyg/kernel/kapi.h>


//static char 		moduleName[] = __FILE__;

//this is for CPU load handling
#ifdef CYGPKG_CPULOAD_v2_0
#include <cyg/cpuload/cpuload.h>

static uint32 calCPU; //variable to hold CPU load calibration
static cyg_cpuload_t cpuLoadDescriptor;
static cyg_handle_t  cpuLoadHandle;


static void calibrateCPUload (void)
{
	cyg_cpuload_calibrate((cyg_uint32*)&calCPU);
}



static HRESULT cliCPU(uint32 action)
{
  cyg_uint32 avg1s, avg10s, avg;

	switch (action)
	{
		case 0: //open CPU load
			if (!cpuLoadHandle)
				cyg_cpuload_create(&cpuLoadDescriptor, calCPU, &cpuLoadHandle);
			sysDebugPrintf("CPU Load monitor is created\n");
			return NO_ERROR;

		case 1: //get CPU stats
			if (!cpuLoadHandle)
			{
				cliOutputLine("CPU load not opened!");
				return E_TASKING_CREATE_ERROR;
			}
			cyg_cpuload_get(cpuLoadHandle, &avg, &avg1s, &avg10s);
			sysDebugPrintf("CPU Load Statistics\r\n");
			sysDebugPrintf("Last 0.1s:   %3d%%\r\n", avg);
			sysDebugPrintf("1s average:  %3d%%\r\n", avg1s);
			sysDebugPrintf("10s average: %3d%%\r\n", avg10s);
			return NO_ERROR;

		case 2: //close CPU stats
			if (cpuLoadHandle)
				cyg_cpuload_delete(cpuLoadHandle);
			cpuLoadHandle = 0;
			sysDebugPrintf("CPU Load monitor is deleted\n");
			return NO_ERROR;
	}
	return NO_ERROR;
}

#endif //CYGPKG_CPULOAD_v2_0


HRESULT	TCKernelCliInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _CLI_TOOL_TCKERNEL
	TCKernelCliInstallTools();
#endif //_CLI_TOOL_TCKERNEL

	return hResult;
}

#define IRQ_ITEMS	19

const char TCKernelCliIntTexts[][16] = 
{
	"LLC        ",
	"LINKON     ",
	"I2C        ",
	"UART0      ",
	"UART1      ",
	"TIMER      ",
	"GPIO       ",
	"WATCHDOG   ",
	"ECPM       ",
	"PWRMGR     ",
	"GRAY       ",
	"AVS0       ",
	"AVS1       ",
	"AVS2       ",
	"APBAUDIOIRQ",
	"APBOVERFLOW",
	"HPLL0      ",
	"HPLL1      ",
	"MIDI       ",
};

HRESULT	TCKernelCliIRQ(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		enable = 0;
	uint32		i;
	uint32		bit;
	BOOL		bEnable = FALSE;

	cliPrintf("TCKernelCliIRQ:\n\r");

	cliPrintf("ICTL_ENABLE:    0x%08x\n\r", (*(unsigned long *)ICTL_ENABLE));
	cliPrintf("ICTL_MASK:      0x%08x\n\r", (*(unsigned long *)ICTL_MASK));
	cliPrintf("ICTL_RAWSTAT:   0x%08x\n\r", (*(unsigned long *)ICTL_RAWSTAT));
	cliPrintf("ICTL_STAT:      0x%08x\n\r", (*(unsigned long *)ICTL_STAT));
	cliPrintf("ICTL_FINALSTAT: 0x%08x\n\r", (*(unsigned long *)ICTL_FINALSTAT));

	enable = ~(*(unsigned long *)ICTL_MASK);

	cliPrintf("IRQ         [%2i]: Enabled\n\r", IRQ_ITEMS);

	for (i = 0; i < IRQ_ITEMS; i++)
	{
		bit = 1 << i;

		bEnable = (bit & enable) == bit;

		cliPrintf("%s [%2i]: %s\n\r", 
			TCKernelCliIntTexts[i],
			i, 
			(bEnable) ? "+" : " ");
	}

	return hResult;
}

HRESULT	TCKernelCliTask(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		maxLen = 24;
	int32		len = 0;
	int32		strLen = 0;
	uint32		maxNumTasks = 0;
	uint32		numTasks = 0;
	char*		taskName = 0;
	BOOL		bResumed = FALSE;
	char		str[32] = {0};
	maxNumTasks = TCTaskingGetMaxNumTasks();
	numTasks = TCTaskingGetNumTasks();

	cliPrintf("TCKernelCliTask:\n\r");

	cliPrintf("Tasks   max [%2i] active [%2i]: Resumed\n\r", maxNumTasks, numTasks);

	for (i = 0; i < numTasks; i++)
	{
		taskName = TCTaskingGetTaskName(i);
		bResumed = TCTaskingGetTaskResumeState(i);

		str[0] = 0;
		
		len = maxLen;
		if (taskName)
		{
			strLen = strlen(taskName);
			len -= strLen;
			if (len < 0) len = 0;
			if (strLen > maxLen) strLen = maxLen;
			strncat(str, taskName, strLen);
			str[strLen] = 0;
		}
		
		while (len--) strcat(str, " ");
		
		cliPrintf("%s[%2i]: %s\n\r",
			str,
			i, 
			(bResumed == TRUE) ? "+" : " ");
	}

	return hResult;
}

#ifdef _CLI_TOOL_TCKERNEL

#ifdef CYGPKG_CPULOAD_v2_0

static  char	TCKernelCliCPU_UsageString[]=\
				"kernel.cpu \n\r"\
				"use: cpu <action>\n\r"\
				"use: <action>: start, stop, stat\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump TCKernel cpu load measurements\n\r";

static	CLIDescriptor	TCKernelCliCPU_Descriptor[]=
{
	{kCLI_Name,				(uint32)"cpu"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliCPU},
	{kCLI_UsageString, 		(uint32)TCKernelCliCPU_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"start=0,stat=1,stop=2"},
	{kCLI_ModuleName,		(uint32)"cpu load"},
	{kCLI_GroupName,		(uint32)"kernel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

#endif  //CYGPKG_CPULOAD_v2_0


static  char	TCKernelCliIRQ_UsageString[]=\
				"kernel.irq \n\r"\
				"use: irq\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump TCKernel irq registers\n\r";

static	CLIDescriptor	TCKernelCliIRQ_Descriptor[]=
{
	{kCLI_Name,				(uint32)"irq"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&TCKernelCliIRQ},
	{kCLI_UsageString, 		(uint32)TCKernelCliIRQ_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"irq registers"},
	{kCLI_GroupName,		(uint32)"kernel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	TCKernelCliTask_UsageString[]=\
				"kernel.task \n\r"\
				"use: task\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump TCKernel task info\n\r";

static	CLIDescriptor	TCKernelCliTask_Descriptor[]=
{
	{kCLI_Name,				(uint32)"task"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&TCKernelCliTask},
	{kCLI_UsageString, 		(uint32)TCKernelCliTask_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"task info"},
	{kCLI_GroupName,		(uint32)"kernel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void TCKernelCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(TCKernelCliIRQ_Descriptor);
	hResult += cliInstallCLIDescriptor(TCKernelCliTask_Descriptor);
#ifdef CYGPKG_CPULOAD_v2_0
	hResult += cliInstallCLIDescriptor(TCKernelCliCPU_Descriptor);
	calibrateCPUload();
#endif //CYGPKG_CPULOAD_v2_0

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing TCKernel cli tools");
	}
	
}

#endif //_CLI_TOOL_TCKERNEL
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

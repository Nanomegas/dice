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

#include <cyg/kernel/kapi.h>
#include <cyg/io/devtab.h>
#include <stdio.h>
#include <stdlib.h>
#include <cyg/io/io.h>                  /* I/O functions */

#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "coreDefs.h"
#include "targetBoard.h"
#include "targetVendorDefs.h"
#include "version.h"
#include "dal.h"
#include "patch-level.h"

#ifdef _CLI
#include "cli.h"
#include "cliBuiltIn.h"
#include "cliCB.h"
#endif //_CLI
#include "llcInt.h"
#include "lal.h"
#include "dice.h"
#include "diceHPLL.h"

#ifdef _AVS
#include "avs.h"
#include "avsTest.h"
#endif //_AVS
#ifdef _FCP
#include "fcp.h"
#endif //_FCP
#ifdef _IRM
#include "irm.h"
#endif //_IRM
#ifdef _CMP
#include "cmp.h"
#endif //_CMP
#ifdef _AVC
#include "avc.h"
#include "avcpanel.h"
#endif //_AVC
#ifdef _EDS
#include "eds.h"
#endif //_EDS
#ifdef _DRD
#include "drd.h"
#endif //_DRD
#if defined (_AVC) && defined (_EDS) && defined (_DRD)
#include "avcAudio.h"
#include "avcMusic.h"
#include "avcDriver.h"
#endif //_AVC &&  _EDS && _DRD
#ifdef _DTCP
//#include "targetConfigDTCP.h"
#include "dtcp.h"
#endif //_DTCP
#ifdef _MLAN
#include "mLANroot.h"
#endif //_MLAN
#ifdef _PIONEERDVD
#include "pioneerDVDTest.h"
#endif //_PIONEERDVD
#ifdef _REMOTEDVD
#include "remoteDVDTest.h"
#endif //_REMOTEDVD
#ifdef _MEMTEST
#include "memTest.h"
#endif //_MEMTEST
#ifdef _DSP
#include "dsp.h"
#include "dspCli.h"
#endif //_DSP
#ifdef _GRAY
#include "gray.h"
#include "grayCli.h"
#endif //_GRAY
#include "TCKernelCli.h"
#ifdef _FIS
#include "fis.h"
#include "envCli.h"
#include "env.h"
#endif //_FIS

#ifdef _FRMWLOAD
#include "frmwLoad.h"
#endif //_FRMWLOAD

#ifdef _DALREMOTE
#include "dalRemote.h"
#include "dalRemoteInterface.h"
#endif //_DALREMOTE

#ifdef _DICE_DRIVER
#include "diceDriver.h"
#endif //DICE_DRIVER

#ifdef _I2C
#include "i2c.h"
#endif //I2C


#ifdef _DICE_TIMER2
#include "diceTimer2.h"
#endif //_DICE_TIMER2

#ifdef _AML
#include "aml.h"
#endif //_AML

#ifdef _SPI
#include "spi.h"
#endif

#ifdef _CRB
#include "crb.h"
#endif



#include "myApp.h"
#include "display.h"

/* The initTask, because this is purely ecos dependent, I don't call TCTask* to 
   define it. They should all be declared as static, but global for now.  */
cyg_thread init_thread;
char init_thread_stack[4096];
cyg_handle_t init_thread_handle;
cyg_thread_entry_t init_thread_main;

uint32 serialNo = 0xFFFFFFFF;

/** @page versionref DICE Firmware Source Code
	This documentation was compiled from document tags in the firmware source code. 
	The major and minor version numbers are defined in the <b> version .c </b> source file. 
	The version number is also shown in the CLI splash screen. <br> <br>

	If you make changes in your code that you wish to document, add the appropriate 
    <a href="http://www.doxygen.org/" title="doxygen home page" target="_top"><strong><font face="Arial, Helvetica, sans-serif">doxygen</font></strong></a>
	tags and recompile this documentation. <br> <br>

	Recompile using 
	@code /firmware/project/myProject/make/make doxygen @endcode <br>
	The resulting HTML documentation start page is 
	@code /firmware/project/myProject/doxygen/GeneratedDocumentation/html/index.html @endcode <br> <br>

	If you download and install the 
	<a href="http://go.microsoft.com/fwlink/?LinkId=14188" title="Download HTML Help Workshop" target="_top"><strong><font face="Arial, Helvetica, sans-serif">HTML Help Workshop</font></strong></a> 
	this HTML will automatically be compiled into a Windows Help (CHM) file. <br> <br> 
	The resulting file will be <br> 
	@code /firmware/project/myProject/doxygen/GeneratedDocumentation/help/SDKUserCompiledHelp.chm @endcode <br> <br> 

	If you install the HTML Help Workshop in a directory other than the default 
	@code C:\Program Files\HTML Help Workshop @endcode <br>
	then edit 
	@code /firmware/project/myProject/doxygen/doxyfile @endcode <br>
	and modify the <b> HHC_LOCATION </b> path to your installation accordingly.	<br> <br> 

*/

//check various defines for validity
#ifdef _MLAN

#ifdef _DICE_DRIVER
#error "DICE_DRIVER and MLAN can not be defined at the same time"
#endif//_DICE_DRIVER

#ifdef _AVC
#error "AVC and MLAN can not be defined at the same time"
#endif // _AVC

#elif defined _DICE_DRIVER

#ifdef _AVC
#error "AVC and DICE DRIVER can not be defined at the same time"
#endif // _AVC

#endif

#include "tcat_dice_version.h"


#ifdef _CLI
//	SplashCB
static HRESULT SplashCB(void)
{
	HRESULT	hResult = NO_ERROR;
	
	cliPrintf("* Running Dice JR/Mini 1394 Appl                         *\n");
	if (isChipDiceJR())
	{	
		cliPrintf("*    Chip Detected :  DICE JR (TCD2220)                  *\n");
	} 
	else
	{
		cliPrintf("*    Chip Detected :  DICE Mini (TCD2210)                *\n");
	}
	cliPrintf("* Board S/N: %08d                                    *\n", serialNo);
	cliPrintf("*  Built with SDK Version: %02d.%02d.%02d, build %04d          *\n", 
			  kTCAT_DICE_VERSION_MAJOR, 
			  kTCAT_DICE_VERSION_MINOR,  
			  kTCAT_DICE_VERSION_SUB,
			  kTCAT_DICE_VERSION_BUILD);

#ifdef _USE_OLD_VERSION_INFO
	cliPrintf("*  Firmware Application Version: %02d.%02d                     *\n", 
			  diceApp_vendor_major_rev(), 
			  diceApp_vendor_minor_rev());
#else
	cliPrintf("*  Firmware Application Version: %02d.%02d.%02d, build %04d    *\n", 
			  diceApp_vendor_major_rev(), 
			  diceApp_vendor_minor_rev(),  
			  diceApp_vendor_sub_rev(),
			  diceApp_vendor_build_rev());
#endif

	cliPrintf(diceApp_build_time());
	cliPrintf("*    MIDI is ");
#ifdef USE_UART1_FOR_MIDI
	{
		cliPrintf("enabled.");
	}
#else
	{
		cliPrintf("disabled.");
	}
#endif
	cliPrintf("                                    *\n");
	cliPrintf("**********************************************************\n");
	cliPrintf("* Target: EVM003                                         *\n");

#ifdef _MLAN
#ifdef _OPEN_MLAN 
	cliPrintf("* Driver: Generic Transporter mLAN                       *\n");
#else // _OPEN_MLAN 
	cliPrintf("* Driver: mLAN                                           *\n");
#endif
#elif defined _DICE_DRIVER
	cliPrintf("* Driver: DiceDriver                                     *\n");
#elif defined _AVC
	cliPrintf("* Driver: AV/C                                           *\n");
#else
    cliPrintf("* Driver: No Driver                                      *\n");
#endif
#ifdef _SPECIAL_PARTITION_MEMORY 
    cliPrintf("* AVS special memory partitions                          *\n");
#endif
#ifdef _EAP
	cliPrintf("* EAP is supported.                                      *\n");
#endif
	cliPrintf("*                                                        *\n");
	cliPrintf("**********************************************************\n");
	return hResult;
}

HRESULT StartBoot(void)
{
	// S/W Reset Power manager
	targetBoardPrepareReset();
	*((volatile uint32 *) PWRMGRBASE) = 0xF;
	
	TCTaskWait(500); // Reset 0.5 sec.

	return E_FAIL;
}
#endif //_CLI

// Initialisation of task
HRESULT mainInitializeTasks(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _CLI
	hResult = cliInitializeTasks();	
	if (hResult != NO_ERROR) return hResult;
#endif //_CLI

#ifdef _1394CORE
	hResult = lalInitialize1394Tasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_1394CORE

#ifdef _AVS
	hResult = avsInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_AVS

#ifdef _CMP
	hResult = cmpInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_CMP

#ifdef _AVC
	hResult = avcInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_AVC

#ifdef _EDS
	hResult = edsInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_EDS

#ifdef _DRD
	hResult = drdInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_DRD

#ifdef _DTCP
	hResult = dtcpInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_DTCP

#ifdef _DICEJR
	hResult = dalInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_DICEJR

#ifdef _REMOTEDVD
	hResult = remoteDVDInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_REMOTEDVD

#ifdef _DICE_DRIVER
	hResult = diceDriverInitializeTasks();
	if (hResult != NO_ERROR) return hResult;	
#endif //_DICE_DRIVER

#if defined (_AVC) && defined (_EDS) && defined (_DRD)
	hResult = avcDriverInitializeTasks();
	if (hResult != NO_ERROR) return hResult;	
#endif // (_AVC) && defined (_EDS) && defined (_DRD)

#ifdef _FRMWLOAD
	hResult = frmwLoadInitializeTasks();
	if (hResult != NO_ERROR) return hResult;	
#endif //_FRMWLOAD

#ifdef _DALREMOTE
	hResult = RDALInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_DALREMOTE

#ifdef _DICE_TIMER2
	hResult = diceTimer2InitializeTask();
	if (hResult != NO_ERROR) return hResult;
#endif //_DICE_TIMER2

	hResult = displayInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
	
	hResult = myAppInitializeTasks();
	if (hResult != NO_ERROR) return hResult;


	return hResult;
}

HRESULT mainInitialize(void)
{
	HRESULT		hResult = NO_ERROR;
    
#ifdef _CLI
	hResult = cliSIOInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_CLI

#if 1 //LM???
	//Setting WWUID for this device
	lalSetWWUID(DEFAULT_DEVICE_ID_HIGH, DEFAULT_DEVICE_ID_LOW);
#endif

#ifdef _SYSDEBUG
	//sysDebugSetEnabled(SYSDEBUG_TRACE_SPS | SYSDEBUG_TRACE_FIS);
#endif //_SYSDEBUG

	TCTaskingInitialize(); // Init kernel

	hResult = mainInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

#ifdef _FIS
	{
		fisInitialize();  //Initialize fis early to be able to get environment settings
		hResult = envInitialize("setup"); //Initialize environment, assume settings image named "setup"
		if (hResult != NO_ERROR)
		{
			sysDebugPrintf("Could not initialize environment - will use default values\n");      
		}
		envGetEenvironmentLongDefault	("SERIAL_NO", &serialNo, MAX_PRODUCT_SERIAL);
		lalSetWWUID (GET_WWUID_HIGH, GET_WWUID_LOW(serialNo));
		//ML:171106, per default JET uses PHY clock due to ADAT pulse width on DICE JR/Mini.
		//envGetEenvironmentLongDefault	("HPLL_CLK", &hpllClk, HPLL_CLK_HZ);
		//hpllSetClock(hpllClk);
	}
#endif //_FIS
	
#ifdef _CLI
	hResult = cliInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = cliBuiltInToolSetSplashCB(SplashCB);
	if (hResult != NO_ERROR) return hResult;

	hResult = cliBuiltInToolSetResetCB(StartBoot);
	if (hResult != NO_ERROR) return hResult;
	
#ifdef _FIS
	hResult = envCliInstallTools();
	if (hResult != NO_ERROR) return hResult;
#endif //_FIS
#endif //_CLI

#ifdef _1394CORE
	hResult = lalInitialize1394();			// create the basic 1394 protocol (lal layer and below)
	if (hResult != NO_ERROR) return hResult;
#endif //_1394CORE

#ifdef _AVS
	hResult = avsInitialize();
	if (hResult != NO_ERROR) return hResult;

//#ifdef _CLI
//	hResult = avsTestCliInstallTools();
//	if (hResult != NO_ERROR) return hResult;
//#endif // _CLI

#endif //_AVS

#ifdef _CRB
	crbInitialize();
#endif

#ifdef _IRM
	hResult = irmInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_IRM

#ifdef _CMP
	hResult = cmpInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_CMP

#ifdef _FCP
	hResult = fcpInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_FCP

#ifdef _AVC
	hResult = avcInitialize();
	if (hResult != NO_ERROR) return hResult;
#if 1 //LM???
	hResult = avcPanelInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif
#endif //_AVC

#ifdef _EDS
	hResult = edsInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_EDS

#ifdef _DRD
	hResult = drdInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_DRD

#if defined (_AVC) && defined (_EDS) && defined (_DRD)
	hResult = avcAudioInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = avcMusicInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_AVC &&  _EDS && _DRD

#ifdef _DTCP
	hResult = dtcpInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_DTCP

#ifdef _PIONEERDVD
	hResult = pioneerDVDInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_PIONEERDVD

#ifdef _REMOTEDVD
	hResult = remoteDVDInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_REMOTEDVD

#ifdef _CLI_CB
	// How do we get rid of this, ML????
	hResult = cliCBInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_CLI

#ifdef _MLAN
	hResult = mLANInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_MLAN

#ifdef _DICE_DRIVER
	hResult = diceDriverInitialize();
	if (hResult != NO_ERROR) return hResult;	
#endif //_DICE_DRIVER

#if defined (_AVC) && defined (_EDS) && defined (_DRD)
	hResult = avcDriverInitialize();
	if (hResult != NO_ERROR) return hResult;	
#endif // (_AVC) && defined (_EDS) && defined (_DRD)

#ifdef _FRMWLOAD
	hResult = frmwLoadInitialize();
	if (hResult != NO_ERROR) return hResult;	
	frmwLoadSetResetCB(StartBoot);
	if (hResult != NO_ERROR) return hResult;	
#endif //_FRMWLOAD

#ifdef _DALREMOTE
	hResult = RDALInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_DALREMOTE

#ifdef _DICEJR
	hResult = diceInitialize();
	if (hResult != NO_ERROR) return hResult;
	hResult = dalInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_DICE2

#ifdef _MEMTEST
	hResult = memTestInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_MEMTEST

#ifdef _DSP
#ifdef _CLI
	hResult = dspCliInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif // _CLI
#endif //_DSP

#ifdef _GRAY
//	hResult = grayInitialize();	//LM??? grayInitialize called from cli command
#ifdef _CLI
	hResult = grayCliInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif // _CLI
#endif //_GRAY

#ifdef _CLI
	hResult = TCKernelCliInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_CLI

#ifdef _I2C
	i2cInitialize();
#endif //I2C


#ifdef _DICE_TIMER2
	diceTimer2Initialize();
#endif //_DICE_TIMER2

#ifdef _AML
	amlInitialize();
#endif //_DICE_TIMER2

#ifdef _SPI
	spiInitialize();
#endif

	hResult = displayInitialize();
	if (hResult != NO_ERROR) return hResult;
	
	hResult = myAppInitialize();
	if (hResult != NO_ERROR) return hResult;

	TCTaskingStart();

	return hResult;
}

/* we create this thread to give a thread context to initializaiton to avoid 
   any scheduling realted issue. */
void init_thread_main(cyg_addrword_t data)
{
    HRESULT		hResult = NO_ERROR;
 
    hResult = mainInitialize();	
    
    if (hResult != NO_ERROR)
	{
		sysDebugPrintf("Could not initialize application\n");
	}
    
#ifdef _1394CORE
    lalInitialize1394Finish();
#endif //_1394CORE
}

void watchdogIsrHandler(void)
{
	*((volatile uint32 *) WATCHDOG_INT_CLR) = 0x5678; //hack, kick the wd
	//LED_TOGGLE(WD_LED_ID);
}

#define _PRMGR_INT
#undef _PRMGR_INT

#ifdef _PRMGR_INT
volatile uint32 pm_cntdown = 0;
void pmIsrHandler(void)
{
	//Clear interrupt
	*((volatile uint32 *) PWRMGR_INT) = BIT1;

	//Read power_manager power down pin value CSR address 0xC
	if (*((volatile uint32 *) PWRMGR_STAT) & BIT0) 
	//If power *on*
	{
		//Enable all DICE clocks
		*((volatile uint32 *) PWRMGR_MUX) = 0x00000001;
		LED_OFF(2);
	}
	else //If power down
	{
		//Prepare powerdown ie. SDRAM self refresh, LED status indicators, etc.
		//ML:140607, removed, was used for debugging
		//LED_ON(2);
		
		//Disable all clocks
		*((volatile uint32 *) PWRMGR_MUX) = 0x0000000F;
		
		//wait in this interupt routine while powered down 
		//since power manager takes some cycles to react
		//otherwise we experience crashes (can't be sure where we are)
		pm_cntdown = 10000;
		while (pm_cntdown--);
	}
}

#endif //_PRMGR_INT

/* we install our own startup routine which sets up threads */
void cyg_user_start(void)
{
    targetBoardInit();
    TCInterruptInstallHandler (CYGNUM_HAL_INTERRUPT_WATCHDOG, &watchdogIsrHandler, NULL);
#ifdef _PRMGR_INT
    TCInterruptInstallHandler (CYGNUM_HAL_INTERRUPT_PRMGR, &pmIsrHandler, NULL);
#endif //_PRGGR_INT

    cyg_thread_create(20, init_thread_main, (cyg_addrword_t)0,
		      "INIT Thread", (void *)init_thread_stack, 4096,
		      &init_thread_handle, &init_thread);
    cyg_thread_resume(init_thread_handle);
}


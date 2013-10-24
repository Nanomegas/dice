//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//===========================================================================
// $Id: myApp.c,v 1.10 2007/02/20 23:06:22 zluo Exp $
//===========================================================================
/****************************************************************************
	
	Purpose:	myApp.c - Example of a user application without firewire
							
	
	Revisions:
		created 25/02/2009 ml
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "dalRates.h"
#include "diceRouterReg.h"
#include "diceClock.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "targetBoard.h"
#include "myApp.h"
#include "sps.h"

#ifdef _CLI_TOOL_MYAPP
#include "myAppCli.h"
#endif //_CLI_TOOL_MYAPP

#ifdef _AML
#include "aml.h"
#include "avsAml.h"
#include "uartAml.h"
#endif //_AML

#ifdef _SPI
#include "spi.h"
#endif //_SPI

// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
// This file uses myModes to select between a number of configurations of the board
// The current mode is stored in the FLASH using SPS. When the board boots it will
// check the switch settings. If SW1 is on it will use the last mode stored in the
// FLASH SPS system otherwise it will use SW2-4 to select between mode0-3. 
//



uint32						myAppThreadID = 0;		
const char*					myAppThreadName = "myAppThread";

uint32						myMeterThreadID = 0;		
const char*					myMeterThreadName = "myMeterThread";





// This function is called when the system is initializing. At this point no threads are running and no audio is
// passed. After this function returns all threads are started. It is not legal to wait on semaphores etc at this
// point in the initialization sequence.


HRESULT	myAppInitialize(void)
{
	HRESULT hResult = NO_ERROR;	
	
	// Initialize the SPI devices (targetBoard.c)
	targetSpiInit();
	insSetClockPortI2S    (INS_ID0, INS_MCK_512BR, true);
	dalCreateInterface(eDAL_INTERFACE_1, eDAL_RATE_MODE_LOW,DEVICE_BIT(DEV_AES_RX),DEVICE_BIT(DEV_AES_TX));
	dalStartInterface(eDAL_INTERFACE_1);
	// After application initialization is done resume tasks to start them
	hResult = myAppResumeTasks();
	if (hResult != NO_ERROR) return hResult;
	// If the developer wishes to add Command Line Interface, define _CLI_TOOL_MYAPP in make.params
#ifdef _CLI
#ifdef _CLI_TOOL_MYAPP
	myAppCliInstallTools();
#endif //_CLI_TOOL_MYAPP
#endif // _CLI
	return hResult;
}


HRESULT myAppInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT myAppResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	
	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT myAppSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	
	return hResult;
}


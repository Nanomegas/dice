//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies  Ltd.(ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	myApp.h - Initialization of user application
	
	Revisions:
		created 02/02/2005 arg

****************************************************************************/
#ifndef _MYAPP_H_
#define	_MYAPP_H_

#ifdef _DICE_DRIVER
#include "dicedriver.h"
#endif // _DICE_DRIVER

// Includes
// ...
/** @addtogroup myApp
	The general steps for creating your Application are:

	1) Edit <b> targetbaord. c </b> and <b> targetboard. h </b> for your hardware configuration, OUI (see below), and device strings.

	2) Edit <b> myApp. c </b> to add your application init's, <br> 
	including port initializations using dice API calls, <br> 
	router initializations using DAL API calls, <br> 
	and driver callbacks to handle event notifications from the PC driver

	3) Edit <b> myApp. cli </b> to add any custom CLI commands you may find useful during development. <br>

	The fundamental routines to use in your Application are shown by example in <b> myApp. c </b> 

	@{
*/
/// Initialize
HRESULT myAppInitializeTasks(void);
/// Resume
HRESULT myAppResumeTasks(void);
/// Suspend
HRESULT myAppSuspendTasks(void);
/// Module initialize
HRESULT	myAppInitialize(void);
//! @}

HRESULT	myAppChangeConfig(uint32 mode);

HRESULT	myAppChangeAC3(uint32 isoc, uint32 flags);

HRESULT myModeBypassMidi (uint32 bOn);

HRESULT myAppSetLEDMode (bool mode);


// We use SPS to store settings in FLASH. In this example we
// only store the current mode. The structure below can be expanded
// in order to store other settings.
typedef struct
{
	uint32	mode;
	uint32	ac3Out; //ac3 bits lower 16 for first isoc, upper for second
	uint32  mode11Cfg;
	//bit 0..3:   Isoc0Tx audio Channels/2
	//bit 4..6:   Isoc0Tx midi Channels
	//bit 8..11:  Isoc1Tx audio Channels/2
	//bit 12..14: Isoc1Tx midi Channels
	//bit 16..19: Isoc0Rx audio Channels/2
	//bit 20..22: Isoc0Rx midi Channels
	//bit 24..27: Isoc1Rx audio Channels/2
	//bit 28..30: Isoc1Rx midi Channels
	uint32  reserved[7];
} EVM_CFG;

extern EVM_CFG evmCfg; //initialized to all zero

HRESULT	myAppChangeMode11rx    (uint32 avs1rxAudio,
								uint32 avs1rxMidi,
								uint32 avs2rxAudio,
								uint32 avs2rxMidi);

HRESULT	myAppChangeMode11tx    (uint32 avs1txAudio,
								uint32 avs1txMidi,
								uint32 avs2txAudio,
								uint32 avs2txMidi);


#endif//_MYAPP_H_

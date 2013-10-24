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

#endif//_MYAPP_H_

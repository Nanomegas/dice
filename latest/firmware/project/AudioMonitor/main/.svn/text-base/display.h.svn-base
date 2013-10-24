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
//===========================================================================
// $Id: display.h,v 1.4 2007/02/08 17:59:33 zluo Exp $
//===========================================================================
/****************************************************************************
	
	Purpose:	myApp.h - Initialization of user application
	
	Revisions:
		created 02/02/2005 arg

****************************************************************************/
#ifndef _DISPLAY_H_
#define	_DISPLAY_H_

typedef enum {
	AM_DISPLAY_VAL_0 = 0, 
	AM_DISPLAY_VAL_1, 
	AM_DISPLAY_VAL_2, 
	AM_DISPLAY_VAL_3, 
	AM_DISPLAY_VAL_4, 
	AM_DISPLAY_VAL_5, 
	AM_DISPLAY_VAL_6, 
	AM_DISPLAY_VAL_7, 
	AM_DISPLAY_VAL_8, 
	AM_DISPLAY_VAL_9, 
	AM_DISPLAY_VAL_A, 
	AM_DISPLAY_VAL_b, 
	AM_DISPLAY_VAL_C, 
	AM_DISPLAY_VAL_d, 
	AM_DISPLAY_VAL_E, 
	AM_DISPLAY_VAL_F, 
	AM_DISPLAY_VAL_SPC,
	AM_DISPLAY_VAL_P,
	AM_DISPLAY_VAL_DASH,
	AM_DISPLAY_VAL_MAX
} AM_DISPLAY_VAL_t;

#define AM_MAX_LED_INDEX 5

void amDisplaySetDigit0(AM_DISPLAY_VAL_t v);
void amDisplaySetDigit1(AM_DISPLAY_VAL_t v);
void amDisplaySetLed(uint8 index);
void amDisplayClearLed(uint8 index);
void amDisplayLed(uint8 index, uint8 on);

int  displayEncoderDelta (void);
bool displayClicked (void);

HRESULT displaySuspendTasks(void);
HRESULT displayResumeTasks(void);
HRESULT displayInitializeTasks(void);

HRESULT	displayInitialize(void);
#endif//_DISPALY_H_

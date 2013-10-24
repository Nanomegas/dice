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
 * DESCRIPTION: Target board specific configurations

	Configuration of target specific hardware setup
	The following should be added to this file:
	- Product specific setup of GPCSR registers
	- Target specific external hardware such as
	  a) SPI devices
	  b) Chip Selects and memory spaces

	Revisions:
		created 11/09/2004 zluo

*****************************************************************************/

#ifndef _TARGETBOARD_H
#define _TARGETBOARD_H

/**********************************LEDs*************************************/
// LED GPIO offsets
#ifdef _LEDS
// If you have LEDs on your board, set indexes into an array
// of GPIO data bits which are set up in targetBoard.c

// Led's used by the system for debugging
// WD_LED_ID			toggles whenever watchdog is kicked, set to 0 if not wanted, called from TCInterrupt.c
// LOCKUP_LED_ID		lit whenever domain 0 is locked, set to 0 if not wanted, called from myApp.c
// LLC_LED_ID			toggle whenever there is an LLC interrupt, set to 0 if not wanted, called from llcInt.c
// LLC_CYCLE_LED_ID		toggles whenever there is a cycle timer sec. interrupt, set to 0 if not wanted, called from lhlService.c
// AES_LOCK_LED_ID		lit whenever the AES receiver is locked, set to 0 if not wanted, called from diceAES.c		
// ADAT_LOCK_LED_ID		lit whenever the ADAT receiver is locked, set to 0 if not wanted, called from diceADAT.c		
// TDIF_LOCK_LED_ID		lit whenever the TDIF receiver is locked, set to 0 if not wanted, called from diceTDIF.c		
// DRIVER_EN_LED_ID		lit when the driver has enabled Isoc. streaming, set to 0 if not wanted, called from myApp.c
// DRIVER_ATCH_LED_ID	lit when the driver is attached, set to 0 if not wanted, called from myApp.c


#define WD_LED_ID			0
#define LLC_CYCLE_LED_ID	0
#define LLC_LED_ID			0
#define DRIVER_EN_LED_ID	0
#define AES_LOCK_LED_ID		2
#define DRIVER_ATCH_LED_ID	4
#define ADAT_LOCK_LED_ID	6
#define LOCKUP_LED_ID		7
#define TDIF_LOCK_LED_ID	0


#else
// dummy parameters for the dummy LED calls
#define WD_LED_ID			0
#define LOCKUP_LED_ID		0
#define LLC_LED_ID			0
#define LLC_CYCLE_LED_ID	0
#define AES_LOCK_LED_ID		0
#define ADAT_LOCK_LED_ID	0
#define TDIF_LOCK_LED_ID	0
#define DRIVER_EN_LED_ID	0
#define DRIVER_ATCH_LED_ID	0

#endif // _LEDS

#define LED_ON				targetBoardLedOn
#define LED_OFF				targetBoardLedOff
#define LED_TOGGLE			targetBoardLedToggle

extern void targetBoardLedOn(uint8 id);
extern void targetBoardLedOff(uint8 id);
extern void targetBoardLedToggle(uint8 id);
extern BOOL targetUseUart1ForMidi (void);
BOOL targetBoardIsMIDIEnabled(void);
BOOL IsSwitchOn(uint8 id);

/**********************************HPLL*************************************/

// HPLL frequency (required)
#define HPLL_CLK_HZ   (2*25000000) // HPLL clock 50.000 Mhz

/****************************************************************************
targetBoardPrepareReset, This function is called before a software reset is
executed. This function should shut down external devices which might prevent
the board from booting.
****************************************************************************/
extern void targetBoardPrepareReset(void);
/****************************************************************************
targetEnableAVSTrigger, This function is called by the FTM debug system if
_FTM is defined and _AVSTRIG is defined.
This function should (if implemented) select a GPIO for trigger output and
program the GPIO for output.
****************************************************************************/
extern void targetEnableAVSTrigger (void);

/****************************************************************************
targetSignalAVSTrigger, This function is called by the FTM debug system if
_FTM is defined and _AVSTRIG is defined.
It should toggle the GPIO selected for debug triggers.
****************************************************************************/
extern void targetSignalAVSTrigger (void);


extern void targetBoardInit(void);

#endif //_TARGETBOARD_H

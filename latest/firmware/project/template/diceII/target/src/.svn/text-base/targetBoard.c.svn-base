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
 * board specific configuration and routines

 this is where the OEM will put his board specific pin configurations.

****************************************************************************/

#include "TCTasking.h"
#include "TCTypes.h"
#include "coreDefs.h"
#include "targetBoard.h"
#include "targetConfigCSR.h"
#include "lhl.h"
#include "lhlbri.h"
#include "lhlIRM.h"

#ifdef _LINKON_TEST
#include "linkonTest.h"
#endif //_LINKON_TEST
#ifdef _TCSETTING
#include "TCFileMan.h"
#endif //_TCSETTING

typedef struct
{
	uint32*	addr;
	uint32	mask;
} LED_CONFIG;

/* Multifunction pin configuration
*	every boards needs a configuration for multifunction pins.
*   you *must* change these settings to reflect your hardware
*	
*/
void targetBoardInit(void)
{
	*((volatile uint32 *) GPCSR_IO_SEL) = 0x28480;
//	*((volatile uint32 *) GPCSR_VIDEO_SEL) = 0xe3e000;	// used with EV1AE1-01 version of the EVM Analog i/o expander board, not with AXM20
	*((volatile uint32 *) GPCSR_VIDEO_SEL) = 0x3e000;	// used with EVM and EVM+AXM20
//	*((volatile uint32 *) GPCSR_VIDEO_SEL) = 0xe000;	// used with F1418
	*((volatile uint32 *) GPIO_A_DDR) = 0xfcf;
	*((volatile uint32 *) GPIO_A_DR) = 0xf31; //turn on all LEDS
	*((volatile uint32 *) GPIO_A_DR) = 0x0;   //turn off all LEDS
//	*((volatile uint32 *) GPIO_A_DR) = 0x400;   //turn off all LEDS, leave CODEC on (F1418)

#ifdef _FIQ
	*((volatile uint32 *) GPCSR_IRQ_MUX_17_TO_12) = 0x20f6818b;	//turn off arm-audio irq
	*((volatile uint32 *) GPCSR_FIQ_MUX_5_TO_0) = 0xe;			//redirect arm-audio to fiq0
//	*((volatile uint32 *) ICTL_FIQ_ENABLE) = 0x1;				//enable fiq0 (moved to later state)
#endif //_FIQ
}

static BOOL targetAVSTriggerEnabled;
// For debugging, enable GPIO for AVS error trigger
void targetEnableAVSTrigger (void)
{
	//This board uses the WCLK out BNC for this, that is GPIO5
	*((volatile uint32 *) GPCSR_VIDEO_SEL) = (*((volatile uint32 *) GPCSR_VIDEO_SEL) & (~0x30000)); //GPIO15 functionality
	*((volatile uint32 *) GPIO_A_DDR) |= 1<<14;
	targetAVSTriggerEnabled = TRUE;
}

void targetSignalAVSTrigger (void)
{
	if (targetAVSTriggerEnabled)
	{
		*((volatile uint32 *) GPIO_A_DR) |= 1<<14; //Set GPIO15 (WCKO)
		*((volatile uint32 *) GPIO_A_DR) &= ~(1<<14); //Clear GPIO15 (WCKO)
	}
}


// the verification board does not have MIDI
BOOL targetUseUart1ForMidi (void)
{
	return FALSE;
}

HRESULT targetBoardShutdownAppl(void)
{
#ifdef _TCSETTING
	return TCFileManCheckUpdate();
#endif //_TCSETTING
	return E_FAIL;
}

HRESULT targetBoardShutdown(void)
{
	targetBoardShutdownAppl();

	return E_FAIL;
}

HRESULT targetBoardReset(void)
{
	// S/W Reset Power manager

	targetBoardShutdownAppl();

#ifdef _DICE2_ASIC
	*((volatile uint32 *) PWRMGRBASE) = 0xF;
#elif defined _DICE2_WIN
//nothing
#else
#error "missing case"
#endif
	
	TCTaskWait(500); // Reset 0.5 sec.

	return E_FAIL;
}

#ifdef _LEDS
/* LED configurations
*	if there are LEDs attached to GPIO pins, then 
*	make sure -D_LEDS exists in make.Params
*	and add a section below to set up the indexed offset to
*	the appropriate GPIO data bits
*/

LED_CONFIG ledConfig[] =
{ 
	{(uint32 *) GPIO_A_DR, 0x800}, 
	{(uint32 *) GPIO_A_DR, 0x40},
	{(uint32 *) GPIO_A_DR, 0x400},
	{(uint32 *) GPIO_A_DR, 0x1},
	{(uint32 *) GPIO_A_DR, 0x200},
	{(uint32 *) GPIO_A_DR, 0x100},
	{(uint32 *) GPIO_A_DR, 0x80},
	
};
static uint8 LED_MAX = 7;


// routines needed if there are LEDs
void targetBoardLedOn(uint8 id)
{
	if(id == 0 || id > LED_MAX) return;
	id--;
	*(ledConfig[id].addr) |= ledConfig[id].mask;
}

void targetBoardLedOff(uint8 id)
{
	if(id == 0 || id > LED_MAX) return;
	id--;
	*(ledConfig[id].addr) &= ~ledConfig[id].mask;
}

void targetBoardLedToggle(uint8 id)
{
	if(id == 0 || id > LED_MAX) return;
	id--;
	if(*(ledConfig[id].addr) & ledConfig[id].mask)
	{
		*(ledConfig[id].addr) &= ~ledConfig[id].mask;
	}
	else
	{
		*(ledConfig[id].addr) |= ledConfig[id].mask;
	}
}

#else //_LEDS
// dummy routines if there are no LEDs

void targetBoardLedOn(uint8 id)
{
}
void targetBoardLedOff(uint8 id)
{
}
void targetBoardLedToggle(uint8 id)
{
}
#endif //_LEDS

// Function to read the setting of the 2 DIP switches on the EVM board
BOOL IsSwitchOn(uint8 id)
{
	uint32 bits;
	bits = *((volatile uint32 *) GPIO_EXT_A);
//	if (id == 0) return (bits & 0x4000) ? FALSE : TRUE;			// F1418
	if (id == 0) return (bits & 0x04) ? TRUE : FALSE;			// EVM
	else if (id == 1) return (bits & 0x10) ? TRUE : FALSE;
	else if (id == 2) return (bits & 0x20) ? TRUE : FALSE;
	return 0;
}

BOOL targetBoardIsMIDIEnabled(void)
{
	//configure device for MIDI based on DIP switch
	return IsSwitchOn(1);
}

#define _CMC_CAPS		// only used in here
#define _ISC_CAPS		// only used in here

bool targetBoardIsIRMC(void)
{
	// is board isoch resource manager capable
#ifdef _IRMC_CAPS
	return TRUE;
#else //_IRMC_CAPS
	return FALSE;
#endif //_IRMC_CAPS
}

BOOL targetBoardIsISC(void)
{
	// is board isoch traffic capable
#ifdef _ISC_CAPS
	return TRUE;
#else //_ISC_CAPS
	return FALSE;
#endif //_ISC_CAPS
}

BOOL targetBoardIsBMC(void)
{
	// is board isoch bus manager capable
#ifdef _BMC_CAPS
	return TRUE;
#else //_BMC_CAPS
	return FALSE;
#endif //_BMC_CAPS
}

BOOL targetBoardIsPMC(void)
{
	// is board isoch power manager capable
#ifdef _PMC_CAPS
	return TRUE;
#else //_PMC_CAPS
	return FALSE;
#endif //_PMC_CAPS
}

// determine if using external power supply or powered from network
BOOL targetBoardIsSelfPowered(void)
{
	//gpio detection of external power supplied
#ifdef _LINKON_TEST
	BOOL	linkon = FALSE;

	linkonGetEnable(&linkon);
	if (linkon == FALSE) return TRUE; 
	return IsSwitchOn(2); //LM??? 
#else //_LINKON_TEST
	return TRUE;
#endif //_LINKON_TEST
}

bool targetBoardIsCMC(void)
{
	// is board cycle master capable
#ifdef _CMC_CAPS
	return targetBoardIsSelfPowered(); //LM???
#else //_CMC_CAPS
	return FALSE;
#endif //_CMC_CAPS
}

BOOL targetBoardIsRHB(void)
{
	// is board root hold off bit
	return targetBoardIsCMC(); //LM???
}

#if 0 // from llcPHY.h
											/* Define Power Class Codes */
#define PHY_PWR_NOSINK_NOREPEAT	0x00		/* does not need or repeat power		*/
#define PHY_PWR_SELF_SRC15W		0x01		/* self-powered, provides 15W min to bus*/
#define PHY_PWR_SELF_SRC30W		0x02		/* self-powered, provides 30W min to bus*/
#define PHY_PWR_SELF_SRC45W		0x03		/* self-powered, provides 45W min to bus*/
#define PHY_PWR_POSS_BUS_SINK3W	0x04		/* may be bus powered, using up to 3W	*/
#define PHY_PWR_BUS_SINK3W		0x05		/* is bus powered, using up to 3W, +0WL	*/
#define PHY_PWR_BUS_SINK6W		0x06		/* is bus powered, using up to 3W, +3WL	*/
#define PHY_PWR_BUS_SINK10W		0x07		/* is bus powered, using up to 3W, +7WL	*/
#endif

#define POWERCLASS_SELFPOWERED	PHY_PWR_POSS_BUS_SINK3W
#ifdef _LINKON_TEST
#define POWERCLASS_BUSPOWERED	PHY_PWR_BUS_SINK10W
#else //_LINKON_TEST
#define POWERCLASS_BUSPOWERED	PHY_PWR_POSS_BUS_SINK3W
#endif //_LINKON_TEST
/*
uint32 targetBoardGetPowerclass(void)
{
	if (targetBoardIsSelfPowered())
	{
		return POWERCLASS_SELFPOWERED;
	}
	else
	{
		return POWERCLASS_BUSPOWERED;
	}
}

*/

void targetBoardPrepareReset(void)
{
	//this function will assure that the board is ready for reset
	targetSetMinConfigROM();
	lhlSetIRMEnabled(FALSE);
  	lhlBriForceBusReset();

  	TCTaskWait(1200);  //enough for computer to determine the device has gone with new LLC fix.
}			

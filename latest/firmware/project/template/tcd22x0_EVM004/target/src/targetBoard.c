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
#include "lhl.h"
#include "lhlbri.h"
#include "spi.h"
#include "../../../chip/dicejr/spi/src/spiDefs.h"
#include "targetConfigCSR.h"


static uint32 audioPortDefault;

/* Multifunction pin configuration
*	every boards needs a configuration for multifunction pins.
*   you *must* change these settings to reflect your hardware
*
*	
*/


void targetBoardInit(void)
{
	uint32 chipID;
	//Example of target board init.


	
	chipID = *((volatile uint32 *) GPCSR_CHIP_ID);
	
	{  
		//---------------------------------------------------------
		//                   3           2            1           0  
		//           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//---------------------------------------------------------
		// GPCSR_AUDIO_SEL  0000 0000 0000 0000 0000 0000 0000 0000
		//             HEX     0    0    0    0    0    0    0    0
		//---------------------------------------------------------
		// AES0_RX          .... .... .... .... .... .... .... ..00  Off
		// AES1_RX          .... .... .... .... .... .... .... 00..  Off
		// AES2_RX          .... .... .... .... .... .... ..00 ....  Off
		// AES3_RX          .... .... .... .... .... .... 00.. ....  Off
		// AO0_0            .... .... .... .... .... ...0 .... ....  Port0 Line 0 is InS0 for Codec
		// AO0_1            .... .... .... .... .... ..0. .... ....  Port0 Line 0 is InS1 for Codec
		// AO0_2            .... .... .... .... .... 00.. .... ....  Port0 Line 0 is InS2 for Codec
		// AO0_3            .... .... .... .... ..00 .... .... ....  Port0 Line 0 is InS3 for Codec
		// AO1_0            .... .... .... .... .0.. .... .... ....  Port1 Line 0 is not on Mini
		// AO1_1            .... .... .... .... 0... .... .... ....  Port1 Line 1 is not on Mini
		// AO1_2            .... .... .... ...0 .... .... .... ....  Port1 Line 2 is not on Mini
		// AO1_3            .... .... .... ..0. .... .... .... ....  Port1 Line 3 is not on Mini
		audioPortDefault = 0x0000;
			
		
		//---------------------------------------------------------
		//                   3           2            1           0  
		//           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//---------------------------------------------------------
		// GPCSR_GPIO_SEL   0000 0000 0000 0000 0010 0000 0000 0010
		//             HEX     0    0    0    0    2    0    0    2
		//---------------------------------------------------------
		// GPIO0            .... .... .... .... .... .... .... ...0  GPIO0 not on Mini
		// SPI              .... .... .... .... .... .... .... .01.  SPI a selected
		// GPIO1            .... .... .... .... .... .... .... 0...  GPIO1 used for SPI SS
		// GPIO2            .... .... .... .... .... .... ...0 ....  GPIO2 used for SPI MISO
		// GPIO3            .... .... .... .... .... .... ..0. ....  GPIO3, To J2701
		// GPIO4            .... .... .... .... .... .... 00.. ....  GPIO4, To J2701
		// GPIO5            .... .... .... .... .... ..00 .... ....  GPIO5, LED 2700
		// GPIO6            .... .... .... .... .... .0.. .... ....  GPIO6, LED 2701
		// GPIO7            .... .... .... .... .... 0... .... ....  GPIO7, LED 2702
		// GPIO8            .... .... .... .... ...0 .... .... ....  GPIO8, LED 2703
		// ENC2             .... .... .... .... ..1. .... .... ....  No Encoder, use GPIO
		// GPIO12           .... .... .... .... .0.. .... .... ....  Not on Mini
		// GPIO13           .... .... .... .... 0... .... .... ....  Not on Mini
		// ENC1             .... .... .... ...1 .... .... .... ....  No Encoder, taken over by SPI
		*((volatile uint32 *) GPCSR_GPIO_SEL) = 0x2002;
	}
	
	*((volatile uint32 *) GPCSR_AUDIO_SEL) = audioPortDefault;
	
	//---------------------------------------------------------
	//                   3           2            1           0  
	//           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
	//---------------------------------------------------------
	//                  0000 0000 0000 0000 0000 0001 1111 1000
	// GPIO_A_DDR  HEX     0    0    0    0    0    1    f    8
	//---------------------------------------------------------
	// GPIO0            .... .... .... .... .... .... .... ...0  Input, not on Mini
	// GPIO1            .... .... .... .... .... .... .... ..0.  Input (pin used for other func)
	// GPIO2            .... .... .... .... .... .... .... .0..  Input (pin used for other func)
	// GPIO3            .... .... .... .... .... .... .... 1...  Output, 
	// GPIO4            .... .... .... .... .... .... ...1 ....  Output, 
	// GPIO5            .... .... .... .... .... .... ..1. ....  Output, LED
	// GPIO6            .... .... .... .... .... .... .1.. ....  Output, LED
	// GPIO7            .... .... .... .... .... .... 1... ....  Output, LED
	// GPIO8            .... .... .... .... .... ...1 .... ....  Output, LED
	// GPIO9            .... .... .... .... .... ..0. .... ....  Input, not on Mini
	// GPIO10           .... .... .... .... .... .0.. .... ....  Input, not on Mini
	// GPIO11           .... .... .... .... .... 0... .... ....  Input, not on Mini
	// GPIO12           .... .... .... .... ...0 .... .... ....  Input, not on Mini
	// GPIO13           .... .... .... .... ..0. .... .... ....  Input, not on Mini
	// GPIO14           .... .... .... .... .0.. .... .... ....  Input, not on Mini
	*((volatile uint32 *) GPIO_A_DDR) = 0x1f8;
	*((volatile uint32 *) GPIO_A_DR) = 1e0; //LED's off
	
}

void setTargetLed(uint8 led, uint8 bon)
{
  if (bon)
    *((volatile uint32 *) GPIO_A_DR) &= ~(((uint32)0x1)<<led);
  else
    *((volatile uint32 *) GPIO_A_DR) |= (((uint32)0x1)<<led);
}

static BOOL targetAVSTriggerEnabled;
// For debugging, enable GPIO for AVS error trigger
void targetEnableAVSTrigger (void)
{
	//This board uses the WCLK out BNC for this, that is GPIO5
	*((volatile uint32 *) GPCSR_GPIO_SEL) = (*((volatile uint32 *) GPCSR_GPIO_SEL) & (~0x300)); //GPIO5 functionality
	*((volatile uint32 *) GPIO_A_DDR) |= 1<<5;
	targetAVSTriggerEnabled = TRUE;
}

void targetSignalAVSTrigger (void)
{
	if (targetAVSTriggerEnabled)
	{
		*((volatile uint32 *) GPIO_A_DR) |= 1<<5; //Set GPIO5 (WCKO)
		*((volatile uint32 *) GPIO_A_DR) &= ~(1<<5); //Clear GPIO5 (WCKO)
	}
}




BOOL isChipDiceJR (void)
{
	uint32 chipID;
	
	chipID = *((volatile uint32 *) GPCSR_CHIP_ID);
	
	return (chipID== 0x0100000f);
}



// This function is called by the system when it needs to perform a
// soft reset. If you need to reset any specific hardware do it here
void targetBoardPrepareReset(void)
{
	//this function will assure that the board is ready for reset
	targetSetMinConfigROM();

  lhlBriForceBusReset();

  TCTaskWait(1200);  //enough for computer to determine the device has gone with new LLC fix.
}			


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

#define SEL_SPI0 0x00
#define SEL_SPI1 0x20
#define SEL_SPI2 0x10
#define SEL_SPI3 0x30
#define SEL_SPI4 0x08
#define SEL_SPI5 0x28
#define SEL_SPI6 0x18
#define SEL_SPI7 0x38

//Used in this application
#define SEL_CODEC SEL_SPI0
#define SEL_LED   SEL_SPI1
#define SEL_DISP  SEL_SPI2
#define SEL_DSP   SEL_SPI3



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
		// GPCSR_AUDIO_SEL  0000 0000 0000 0000 0010 0100 0001 0000
		//             HEX     0    0    0    0    2    4    1    0
		//---------------------------------------------------------
		// AES0_RX          .... .... .... .... .... .... .... ..00  Off
		// AES1_RX          .... .... .... .... .... .... .... 00..  Off
		// AES2_RX          .... .... .... .... .... .... ..01 ....  SPDIF Input from Port0 Line 2
		// AES3_RX          .... .... .... .... .... .... 00.. ....  Off
		// AO0_0            .... .... .... .... .... ...0 .... ....  Port0 Line 0 is InS0 for Codec
		// AO0_1            .... .... .... .... .... ..0. .... ....  Unused, progran as InS
		// AO0_2            .... .... .... .... .... 01.. .... ....  Port0 Line 2 is SPDIF
		// AO0_3            .... .... .... .... ..10 .... .... ....  Port0 Line 3 is ADAT
		// AO1_0            .... .... .... .... .0.. .... .... ....  Port1 Line 0 is not on Mini
		// AO1_1            .... .... .... .... 0... .... .... ....  Port1 Line 1 is not on Mini
		// AO1_2            .... .... .... ...0 .... .... .... ....  Port1 Line 2 is not on Mini
		// AO1_3            .... .... .... ..0. .... .... .... ....  Port1 Line 3 is not on Mini
		audioPortDefault = 0x2410;
			
		
		//---------------------------------------------------------
		//                   3           2            1           0  
		//           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//---------------------------------------------------------
		// GPCSR_GPIO_SEL   0000 0000 0000 0000 0010 0000 0000 0010
		//             HEX     0    0    0    0    2    0    0    2
		//---------------------------------------------------------
		// GPIO0            .... .... .... .... .... .... .... ...0  GPIO0 not on Mini
		// SPI              .... .... .... .... .... .... .... .01.  SPI a
		// GPIO1            .... .... .... .... .... .... .... 0...  GPIO1 used for SPI
		// GPIO2            .... .... .... .... .... .... ...0 ....  GPIO2 used for SPI
		// GPIO3            .... .... .... .... .... .... ..0. ....  GPIO3, SPI device select A2
		// GPIO4            .... .... .... .... .... .... 00.. ....  GPIO4, SPI device select A1
		// GPIO5            .... .... .... .... .... ..00 .... ....  GPIO5, SPI device select A0
		// GPIO6            .... .... .... .... .... .0.. .... ....  GPIO6, push switch
		// GPIO7            .... .... .... .... .... 0... .... ....  GPIO7, encoder input B 
		// GPIO8            .... .... .... .... ...0 .... .... ....  GPIO8, encoder input A
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
	//                  0000 0000 0000 0000 0000 0000 0011 1000
	// GPIO_A_DDR  HEX     0    0    0    0    0    0    3    8
	//---------------------------------------------------------
	// GPIO0            .... .... .... .... .... .... .... ...0  Input, not on Mini
	// GPIO1            .... .... .... .... .... .... .... ..0.  Input (pin used for other func)
	// GPIO2            .... .... .... .... .... .... .... .0..  Input (pin used for other func)
	// GPIO3            .... .... .... .... .... .... .... 1...  Output, SPI Select A2
	// GPIO4            .... .... .... .... .... .... ...1 ....  Output, SPI Select A1
	// GPIO5            .... .... .... .... .... .... ..1. ....  Output, SPI Select A0
	// GPIO6            .... .... .... .... .... .... .0.. ....  Input, Switch
	// GPIO7            .... .... .... .... .... .... 0... ....  Input, Encoder B
	// GPIO8            .... .... .... .... .... ...0 .... ....  Input, Encoder A
	// GPIO9            .... .... .... .... .... ..0. .... ....  Input, not on Mini
	// GPIO10           .... .... .... .... .... .0.. .... ....  Input, not on Mini
	// GPIO11           .... .... .... .... .... 0... .... ....  Input, not on Mini
	// GPIO12           .... .... .... .... ...0 .... .... ....  Input, not on Mini
	// GPIO13           .... .... .... .... ..0. .... .... ....  Input, not on Mini
	// GPIO14           .... .... .... .... .0.. .... .... ....  Input, not on Mini
	*((volatile uint32 *) GPIO_A_DDR) = 0x38;
	*((volatile uint32 *) GPIO_A_DR) = SEL_LED; //prepare for LED setting
	
	//We need to do a little hack here. We do not know if the PHY is in or out of reset so
	//we will send a command to initialize it here.
	MPTR(SPI_CTRL) = 0x75; //16 bit 
	MPTR(SPI_BAUD_RATE) = 10;
	MPTR(SPI_DATA) = LED_MASK(LED_PHY_NRST);
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


// SPI functionality

//Adding blocking functionality to the SPI


static uint32 codecSSId, ledSSId, dispSSId, dspSSId;
static uint32 dspSem;

static void spi_slave_select(uint32 ssId)
{
	if (ssId==codecSSId) 
	{
		*((volatile uint32 *) GPIO_A_DR) = SEL_CODEC; //enable codec
	}
	else if (ssId==ledSSId) 
	{
		*((volatile uint32 *) GPIO_A_DR) = SEL_LED; //enable LED
	}
	else if (ssId==dispSSId) 
	{
		*((volatile uint32 *) GPIO_A_DR) = SEL_DISP; //enable 7 segment display
	}
	else if (ssId==dspSSId) 
	{
		*((volatile uint32 *) GPIO_A_DR) = SEL_DSP; //enable DSP
	}
	else
	{
		*((volatile uint32 *) GPIO_A_DR) = SEL_SPI4; //enable something else
	}
}


//we might be reading from the DSP.
static void spi_slave_complete_sb(uint32 sid, uint32 * value)
{
	if (!value) return; //only read commands are blocking
	if (sid == dspSSId)
		TCSemaphoreSignal (dspSem);
}

// The write command is non blocking except if the buffer is full in which case the the function will retry

static void targetWriteGenericSPIWord (uint32 sid, uint32 w)
{
	HRESULT hResult;
	
	while (1)
	{
		hResult = spiOpNonBlock(sid, w, NULL);
		if (hResult != SPI_ERROR_BUF_FULL) break;
		TCTaskYield();
	}
}

// The read command is blocking, it will match up each read to the corresponding return.
// it can assume that each device will only have one outstanding read.

static uint32 targetReadGenericSPIWord (uint32 sid, uint32 w)
{
	HRESULT hResult;
	uint32 retval;
	
	while (1)
	{
		hResult = spiOpNonBlock(sid, w, &retval);
		if (hResult != SPI_ERROR_BUF_FULL) break;
		TCTaskYield();
	}
	//now wait for the matching reply
	if (sid == dspSSId)
		TCSemaphoreWait (dspSem);
	//else if (sid == cpldSSId)
	//	TCSemaphoreWait (cpldSem);
	return retval;
}


void targetWriteCodecSPIWord (uint16 w)
{
	targetWriteGenericSPIWord (codecSSId, w);
}

void targetWriteLEDSPIWord (uint16 w)
{
	targetWriteGenericSPIWord (ledSSId, w);
}

void targetWriteDisplaySPIWord (uint16 w)
{
	targetWriteGenericSPIWord (dispSSId, w);
}

void targetWriteDspSPIWord (uint32 w)
{
	targetWriteGenericSPIWord (dspSSId, w);
}

uint32 targetReadDspSPIWord (uint32 w)
{
	return targetReadGenericSPIWord (dspSSId, w);
}


#define SPI_SPEED 10 //baudrate = 49152000/(8*(SPI_SPEED+1))


void targetSpiInit(void) 
{
  //Create the SPI slaves
	spiGetSlaveDevice(&codecSSId);
	spiGetSlaveDevice(&ledSSId);
	spiGetSlaveDevice(&dispSSId);
	spiGetSlaveDevice(&dspSSId);

	//Create semaphore for receiving devices
	TCSemaphoreOpen (&dspSem,0);

	//this is common for all devices so we only need to set it for one
	spiConfigure(codecSSId, SPI_SET_SS_ROUTINE, spi_slave_select);

	//word size for the devices
	spiConfigure(codecSSId, SPI_WSIZE, (void *)SPI_WSIZE_16);
	spiConfigure(ledSSId,   SPI_WSIZE, (void *)SPI_WSIZE_16);
	spiConfigure(dispSSId,  SPI_WSIZE, (void *)SPI_WSIZE_16);
	spiConfigure(dspSSId,   SPI_WSIZE, (void *)SPI_WSIZE_24);

	//speed for the devices
	spiConfigure(codecSSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(ledSSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(dispSSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(dspSSId, SPI_RATE, (void *)SPI_SPEED);

  //Only the DSP device is read from so it needs the complete function
	spiConfigure(dspSSId, SPI_COMPLETE_CB, spi_slave_complete_sb);

	//Get the PHY powered up and turn off all LED's
	spiOpBlockNoTask(ledSSId, LED_MASK(LED_PHY_NRST) | LED_ALL_LED_MASK,NULL);
	//Turn off all 7 Seg display
	spiOpBlockNoTask(dispSSId, 0xffff,NULL);
	
	
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


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
#include "targetConfigCSR.h"


static uint32 audioPortDefault;

/* Multifunction pin configuration
*	every boards needs a configuration for multifunction pins.
*   you *must* change these settings to reflect your hardware
*	
*/
void targetBoardInit(void)
{
	uint32 chipID;
	//Example of target board init.


	
	chipID = *((volatile uint32 *) GPCSR_CHIP_ID);
	
	if ((chipID & 0xf)== 0xf) //DICE JR
	{  
		//This part is not used, Audio Monitor is DICE Mini only
		//setting up chip select NCS2 for DICE JR only
		*((volatile uint32 *) MEM_SCSLR2) = 0x02080000;
		*((volatile uint32 *) MEM_SMSKR2) = 0x00000024;//SRAM, 512kb, set 0
		*((volatile uint32 *) MEM_SMTMGR_SET0) = 0x10000841;
		*((volatile uint32 *) MEM_SMCTLR) = 0x20f; //set0=8bit all other sets 16 bits
		
		
		//------------------------------------------------------
		//                3           2            1           0  
		//        Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//------------------------------------------------------
		// GPCSR_AUDIO_SEL  0000 0000 0000 0011 1110 1000 1010 1010
		//             HEX     0    0    0    3    e    8    a    a
		//---------------------------------------------------------
		// AES0_RX          .... .... .... .... .... .... .... ..10  AES0 Rx Source is Port1
		// AES1_RX          .... .... .... .... .... .... .... 10..  AES1 Rx Source is Port1
		// AES2_RX          .... .... .... .... .... .... ..10 ....  AES2 Rx Source is Port1
		// AES3_RX          .... .... .... .... .... .... 10.. ....  AES3 Rx Source is Port1
		// AO0_0            .... .... .... .... .... ...0 .... ....  Port0 Line 0 is InS0_0
		// AO0_1            .... .... .... .... .... ..0. .... ....  Port0 Line 1 is InS0_1
		// AO0_2            .... .... .... .... .... 10.. .... ....  Port0 Line 2 is ADAT0
		// AO0_3            .... .... .... .... ..10 .... .... ....  Port0 Line 3 is ADAT1
		// AO1_0            .... .... .... .... .1.. .... .... ....  Port1 Line 0 is AES0
		// AO1_1            .... .... .... .... 1... .... .... ....  Port1 Line 1 is AES1
		// AO1_2            .... .... .... ...1 .... .... .... ....  Port1 Line 2 is AES2
		// AO1_3            .... .... .... ..1. .... .... .... ....  Port1 Line 3 is AES3
		audioPortDefault = 0x3e8aa;
		


		//DICE JR configuration, use SPI b for EVM
		//------------------------------------------------------
		//                3           2            1           0  
		//        Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//------------------------------------------------------
		// GPCSR_GPIO_SEL   0000 0000 0000 0001 1110 0011 0100 1100
		//             HEX     0    0    0    1    e    3    4    c
		//---------------------------------------------------------
		// GPIO0            .... .... .... .... .... .... .... ...0  GPIO0 not CLKE
		// SPI              .... .... .... .... .... .... .... .10.  SPI b
		// GPIO1            .... .... .... .... .... .... .... 1...  NCS2
		// GPIO2            .... .... .... .... .... .... ...0 ....  GPIO2
		// GPIO3            .... .... .... .... .... .... ..0. ....  GPIO3
		// GPIO4            .... .... .... .... .... .... 01.. ....  EXTFBR in/WCK
		// GPIO5            .... .... .... .... .... ..11 .... ....  WCK out
		// GPIO6            .... .... .... .... .... .0.. .... ....  GPIO6
		// GPIO7            .... .... .... .... .... 0... .... ....  GPIO7
		// GPIO8            .... .... .... .... ...0 .... .... ....  GPIO8
		// ENC2             .... .... .... .... ..1. .... .... ....  No Encoder, use GPIO
		// GPIO12           .... .... .... .... .1.. .... .... ....  WCKI
		// GPIO13           .... .... .... .... 1... .... .... ....  WCKO
		// ENC1             .... .... .... ...1 .... .... .... ....  No Encoder, use GPIO
		*((volatile uint32 *) GPCSR_GPIO_SEL) = 0x1e34c;
	}
	else //DICE Mini
	{  
		//------------------------------------------------------
		//                3           2            1           0  
		//        Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//------------------------------------------------------
		// GPCSR_AUDIO_SEL  0000 0000 0000 0011 1101 1000 0000 0001
		//             HEX     0    0    0    3    d    8    0    1
		//---------------------------------------------------------
		// AES0_RX          .... .... .... .... .... .... .... ..01  AES0 Rx Source is Port0
		// AES1_RX          .... .... .... .... .... .... .... 00..  AES1 Rx Source is off
		// AES2_RX          .... .... .... .... .... .... ..00 ....  AES2 Rx Source is off
		// AES3_RX          .... .... .... .... .... .... 00.. ....  AES3 Rx Source is off
		// AO0_0            .... .... .... .... .... ...0 .... ....  Port0 Line 0 is InS0_0
		// AO0_1            .... .... .... .... .... ..0. .... ....  Port0 Line 1 is InS0_1
		// AO0_2            .... .... .... .... .... 10.. .... ....  Port0 Line 2 is ADAT0
		// AO0_3            .... .... .... .... ..01 .... .... ....  Port0 Line 3 is AES3
		// AO1_0            .... .... .... .... .1.. .... .... ....  Port1 Line 0 is AES0
		// AO1_1            .... .... .... .... 1... .... .... ....  Port1 Line 1 is AES1
		// AO1_2            .... .... .... ...1 .... .... .... ....  Port1 Line 2 is AES2
		// AO1_3            .... .... .... ..1. .... .... .... ....  Port1 Line 3 is AES3
		audioPortDefault = 0x3d801;
			
		
		//------------------------------------------------------
		//                3           2            1           0  
		//        Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//------------------------------------------------------
		// GPCSR_GPIO_SEL   0000 0000 0000 0001 1110 0000 0000 0010
		//             HEX     0    0    0    1    e    0    0    2
		//---------------------------------------------------------
		// GPIO0            .... .... .... .... .... .... .... ...0  GPIO0 not CLKE
		// SPI              .... .... .... .... .... .... .... .01.  SPI b
		// GPIO1            .... .... .... .... .... .... .... 0...  GPIO1
		// GPIO2            .... .... .... .... .... .... ...0 ....  GPIO2
		// GPIO3            .... .... .... .... .... .... ..0. ....  GPIO3, DAC slave select
		// GPIO4            .... .... .... .... .... .... 00.. ....  GPIO4, DAC PDN signal
		// GPIO5            .... .... .... .... .... ..00 .... ....  GPIO5, display slave select
		// GPIO6            .... .... .... .... .... .0.. .... ....  GPIO6, push switch
		// GPIO7            .... .... .... .... .... 0... .... ....  GPIO7, encoder input B 
		// GPIO8            .... .... .... .... ...0 .... .... ....  GPIO8, encoder input A
		// ENC2             .... .... .... .... ..1. .... .... ....  No Encoder, use GPIO
		// GPIO12           .... .... .... .... .1.. .... .... ....  WCKI
		// GPIO13           .... .... .... .... 1... .... .... ....  WCKO
		// ENC1             .... .... .... ...1 .... .... .... ....  No Encoder, taken over by SPI
		*((volatile uint32 *) GPCSR_GPIO_SEL) = 0x1e002;
	}
	
	*((volatile uint32 *) GPCSR_AUDIO_SEL) = audioPortDefault;
	
	//---------------------------------------------------------
	//                   3           2            1           0  
	//           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
	//---------------------------------------------------------
	//                  0000 0000 0000 0000 0000 0000 0011 1000
	// GPIO_A_DDR  HEX     0    0    0    0    0    0    3    8
	//---------------------------------------------------------
	// GPIO0            .... .... .... .... .... .... .... ...0  Input
	// GPIO1            .... .... .... .... .... .... .... ..0.  Input (pin used for other func)
	// GPIO2            .... .... .... .... .... .... .... .0..  Input (pin used for other func)
	// GPIO3            .... .... .... .... .... .... .... 1...  Output
	// GPIO4            .... .... .... .... .... .... ...1 ....  Output
	// GPIO5            .... .... .... .... .... .... ..1. ....  Output
	// GPIO6            .... .... .... .... .... .... .0.. ....  Input
	// GPIO7            .... .... .... .... .... .... 0... ....  Input
	// GPIO8            .... .... .... .... .... ...0 .... ....  Input
	// GPIO9            .... .... .... .... .... ..0. .... ....  Input
	// GPIO10           .... .... .... .... .... .0.. .... ....  Input
	// GPIO11           .... .... .... .... .... 0... .... ....  Input
	// GPIO12           .... .... .... .... ...0 .... .... ....  Input (pin used for other func)
	// GPIO13           .... .... .... .... ..0. .... .... ....  Input (pin used for other func)
	// GPIO14           .... .... .... .... .0.. .... .... ....  Input (pin used for other func)
	*((volatile uint32 *) GPIO_A_DDR) = 0x38;
	*((volatile uint32 *) GPIO_A_DR) = 0x08;   //Enable display SPI
	
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

uint32 codecSSId, displaySSId;

static void spi_slave_select(uint32 ssId)
{
	if (ssId==displaySSId) 
	{
		*((volatile uint32 *) GPIO_A_DR) = 0x18; //enable display
	}
	else
	{
		*((volatile uint32 *) GPIO_A_DR) = 0x30; //enable codec
	}
}


//we are not reading from SPI so this is not needed.
static void spi_slave_complete_sb(uint32 sid, uint32 * value)
{
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


void targetWriteDisplaySPIWord (uint32 w)
{
	targetWriteGenericSPIWord (displaySSId, w);
}


void targetWriteCodecSPIWord (uint32 w)
{
	targetWriteGenericSPIWord (codecSSId, w);
}





#define SPI_SPEED 10 //baudrate = 49152000/(8*(SPI_SPEED+1))


void targetSpiInit(void) 
{

	spiGetSlaveDevice(&displaySSId);
	spiGetSlaveDevice(&codecSSId);
	
	spiConfigure(codecSSId, SPI_SET_SS_ROUTINE, spi_slave_select);

	spiConfigure(codecSSId, SPI_COMPLETE_CB, spi_slave_complete_sb);
	spiConfigure(codecSSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(codecSSId, SPI_WSIZE, (void *)SPI_WSIZE_16);
	
	spiConfigure(displaySSId, SPI_COMPLETE_CB, spi_slave_complete_sb);
	spiConfigure(displaySSId, SPI_PHASE, (void *)SPI_PHSASE_0);
	spiConfigure(displaySSId, SPI_POLARITY, (void *)SPI_POLARITY_0);
	spiConfigure(displaySSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(displaySSId, SPI_WSIZE, (void *)SPI_WSIZE_24);	
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


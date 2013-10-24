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
#include "lhlIRM.h"

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
		// GPCSR_AUDIO_SEL  0000 0000 0000 0011 1101 1000 0100 0000
		//             HEX     0    0    0    3    d    8    4    0
		//---------------------------------------------------------
		// AES0_RX          .... .... .... .... .... .... .... ..00  AES0 Rx Source is off
		// AES1_RX          .... .... .... .... .... .... .... 00..  AES1 Rx Source is off
		// AES2_RX          .... .... .... .... .... .... ..00 ....  AES2 Rx Source is off
		// AES3_RX          .... .... .... .... .... .... 01.. ....  AES3 Rx Source is Port0
		// AO0_0            .... .... .... .... .... ...0 .... ....  Port0 Line 0 is InS0_0
		// AO0_1            .... .... .... .... .... ..0. .... ....  Port0 Line 1 is InS0_1
		// AO0_2            .... .... .... .... .... 10.. .... ....  Port0 Line 2 is ADAT0
		// AO0_3            .... .... .... .... ..01 .... .... ....  Port0 Line 3 is AES3
		// AO1_0            .... .... .... .... .1.. .... .... ....  Port1 Line 0 is AES0
		// AO1_1            .... .... .... .... 1... .... .... ....  Port1 Line 1 is AES1
		// AO1_2            .... .... .... ...1 .... .... .... ....  Port1 Line 2 is AES2
		// AO1_3            .... .... .... ..1. .... .... .... ....  Port1 Line 3 is AES3
		audioPortDefault = 0x3d840;
			
		
		
		
		//DICE Mini configuration, use SPI a for EVM
		//------------------------------------------------------
		//                3           2            1           0  
		//        Bit No 1098 7654 3210 9876 5432 1098 7654 3210
		//------------------------------------------------------
		// GPCSR_GPIO_SEL   0000 0000 0000 0001 1110 0011 0100 0010
		//             HEX     0    0    0    1    e    3    4    2
		//---------------------------------------------------------
		// GPIO0            .... .... .... .... .... .... .... ...0  GPIO0 not CLKE, NOT ON MINI
		// SPI              .... .... .... .... .... .... .... .01.  SPI a
		// GPIO1            .... .... .... .... .... .... .... 0...  GPIO1 (don't care SPI)
		// GPIO2            .... .... .... .... .... .... ...0 ....  GPIO2 (don't care SPI)
		// GPIO3            .... .... .... .... .... .... ..0. ....  GPIO3
		// GPIO4            .... .... .... .... .... .... 01.. ....  EXTFBR in/WCK
		// GPIO5            .... .... .... .... .... ..11 .... ....  WCK out
		// GPIO6            .... .... .... .... .... .0.. .... ....  GPIO6
		// GPIO7            .... .... .... .... .... 0... .... ....  GPIO7
		// GPIO8            .... .... .... .... ...0 .... .... ....  GPIO8
		// ENC2             .... .... .... .... ..1. .... .... ....  No Encoder, use GPIO
		// GPIO12           .... .... .... .... .1.. .... .... ....  WCKI, NOT ON MINI
		// GPIO13           .... .... .... .... 1... .... .... ....  WCKO, NOT ON MINI
		// ENC1             .... .... .... ...1 .... .... .... ....  No Encoder, use GPIO, NOT ON MINI
		*((volatile uint32 *) GPCSR_GPIO_SEL) = 0x1e342;
	}
	
	*((volatile uint32 *) GPCSR_AUDIO_SEL) = audioPortDefault;
	
	//---------------------------------------------------------
    //                   3           2            1           0  
    //           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
    //---------------------------------------------------------
    //                  0000 0000 0000 0000 0000 0000 0000 1000
    // GPIO_A_DDR  HEX     0    0    0    0    0    0    0    8
    //---------------------------------------------------------
	// GPIO0            .... .... .... .... .... .... .... ...0  Input
	// GPIO1            .... .... .... .... .... .... .... ..0.  Input
	// GPIO2            .... .... .... .... .... .... .... .0..  Input (pin used for other func)
	// GPIO3            .... .... .... .... .... .... .... 1...  Output, controls SPI on Mini Microboard
	// GPIO4            .... .... .... .... .... .... ...0 ....  Input
	// GPIO5            .... .... .... .... .... .... ..0. ....  Input
	// GPIO6            .... .... .... .... .... .... .0.. ....  Input
	// GPIO7            .... .... .... .... .... .... 0... ....  Input
	// GPIO8            .... .... .... .... .... ...0 .... ....  Input
	// GPIO9            .... .... .... .... .... ..0. .... ....  Input
	// GPIO10           .... .... .... .... .... .0.. .... ....  Input
	// GPIO11           .... .... .... .... .... 0... .... ....  Input
	// GPIO12           .... .... .... .... ...0 .... .... ....  Input (pin used for other func)
	// GPIO13           .... .... .... .... ..0. .... .... ....  Input (pin used for other func)
	// GPIO14           .... .... .... .... .0.. .... .... ....  Input (pin used for other func)
	*((volatile uint32 *) GPIO_A_DDR) = 0x8;
	*((volatile uint32 *) GPIO_A_DR) = 0x0;   //turn off all GPIO outputs
	
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


// Some of the modes use the first optical as SPDIF instead of ADAT (Port0-2)

void targetChangeAudioPorts (uint8 mode)
{
	uint32 port;
	switch (mode)
	{
		default:
		case APM_NORMAL: port = audioPortDefault; break;
		case APM_TOS:	 port = (audioPortDefault & 0xfffff3cf) | 0x00000410; break;
		case APM_ALL_INS:port = 0; break;
	}
	*((volatile uint32 *) GPCSR_AUDIO_SEL) = port;
}


BOOL isChipDiceJR (void)
{
	uint32 chipID;
	
	chipID = *((volatile uint32 *) GPCSR_CHIP_ID);
	
	return (chipID== 0x0100000f);
}






// SPI functionality

//Adding blocking functionality to the SPI

static uint32 codecSem, codecSSId;
static uint32 cpldSem, cpldSSId;

static void spi_slave_select(uint32 ssId)
{
	//not used here as both devices share SS
}

static void spi_slave_complete_sb(uint32 sid, uint32 * value)
{
	if (!value) return; //only read commands are blocking
	if (sid == codecSSId)
		TCSemaphoreSignal (codecSem);
	else if (sid == cpldSSId)
		TCSemaphoreSignal (cpldSem);
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
	if (sid == codecSSId)
		TCSemaphoreWait (codecSem);
	else if (sid == cpldSSId)
		TCSemaphoreWait (cpldSem);
	return retval;
}



void targetWriteCPLDSPIWord (uint32 w)
{
	targetWriteGenericSPIWord (cpldSSId, w);
}

uint32 targetReadCPLDSPIWord (uint32 w)
{
	return targetReadGenericSPIWord (cpldSSId, w);
}

void targetWriteCodecSPIWord (uint32 w)
{
	targetWriteGenericSPIWord (codecSSId, w);
}




#define SPI_SPEED 10 //baudrate = 49152000/(8*(SPI_SPEED+1))

static uint32 initialSwitchSetting;

uint32 targetGetInitialSW (void)
{
	return initialSwitchSetting;
}

static uint32 cpldVer;
static bool cpldSupported;

void targetGetCPLDInfo (uint8 * ver, BOOL * bSupported)
{
	*ver = (uint8)cpldVer;
	*bSupported = cpldSupported;
}


void targetSpiInit(void) 
{

	spiGetSlaveDevice(&cpldSSId);
	spiGetSlaveDevice(&codecSSId);

	spiConfigure(codecSSId, SPI_SET_SS_ROUTINE, spi_slave_select);
	spiConfigure(codecSSId, SPI_COMPLETE_CB, spi_slave_complete_sb);
	spiConfigure(codecSSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(codecSSId, SPI_WSIZE, (void *)SPI_WSIZE_16);
	spiConfigure(cpldSSId, SPI_COMPLETE_CB, spi_slave_complete_sb);
	spiConfigure(cpldSSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(cpldSSId, SPI_WSIZE, (void *)SPI_WSIZE_16);	
	
	TCSemaphoreOpen (&codecSem,0);
	TCSemaphoreOpen (&cpldSem,0);
	//at init time, let's talk to CPLD and initialize
	//read the CPLD version
	spiOpBlockNoTask(cpldSSId, CPLD_SPI_RD_CMD(CPLD_VER_REG,0), &cpldVer);
	if (cpldVer>=255) cpldVer = 0;
	cpldSupported = ((cpldVer >= 12) && (cpldVer < 128));
	
	if (cpldSupported) 
	{
		if (isChipDiceJR())
			spiOpBlockNoTask(cpldSSId, CPLD_SPI_WR_CMD(CPLD_CTRL_REG,CPLD_CTRL_PAR_EN | 
																	CPLD_CTRL_CODEC_EN | 
																	CPLD_CTRL_SPI1_EN | 
																	CPLD_CTRL_SPI2_EN | 
																	CPLD_CTRL_USER_EN), NULL);
		else
			spiOpBlockNoTask(cpldSSId, CPLD_SPI_WR_CMD(CPLD_CTRL_REG,CPLD_CTRL_CODEC_EN | 
																	CPLD_CTRL_SPI1_EN | 
																	CPLD_CTRL_SPI2_EN | 
																	CPLD_CTRL_USER_EN), NULL);	
 		//read the switches
		spiOpBlockNoTask(cpldSSId, CPLD_SPI_RD_CMD(CPLD_SW_REG,0), &initialSwitchSetting);
	}
	else
		initialSwitchSetting = 0x0; //mode 0, not Meter mode
	
}	

//Virtual LED and SW functions
volatile uint8 * pTargetCpld = ((volatile uint8 *) 0x02080000);

static uint8 ledMsk;

static void updateLED(void)
{
	if (!cpldSupported) return;
	if (isChipDiceJR())
	{
		pTargetCpld[CPLD_LED_REG] = ledMsk;
	}
	else
	{
		targetWriteCPLDSPIWord (CPLD_SPI_WR_CMD(CPLD_LED_REG,ledMsk));
	}
}

void targetSetAllLED (uint8 msk)
{
	if (ledMsk != msk)
	{
		ledMsk = msk;
		updateLED();
	}
}

void targetSetLED (TGT_LED led, TGT_LED_STATE state)
{
	uint8 msk;
	
	msk = ledMsk;
	if (state==TGT_LED_OFF)
		msk &= ~(1<<led);
	else if (state==TGT_LED_ON)
		msk |= 1<<led;
	else if(state==TGT_LED_TGL)
		msk ^= 1<<led;
	targetSetAllLED (msk);
}

extern uint8 targetGetSw (void)
{
	if (!cpldSupported) return initialSwitchSetting;
	
	if (isChipDiceJR())
	{
		return pTargetCpld[CPLD_SW_REG] & 0x0f;
	}
	else
	{
		return (uint8)targetReadCPLDSPIWord (CPLD_SPI_RD_CMD(CPLD_SW_REG,0)) & 0x0f;
	}
}



// This function is called by the system when it needs to perform a
// soft reset. If you need to reset any specific hardware do it here
void targetBoardPrepareReset(void)
{
	//this function will assure that the board is ready for reset
  targetSetMinConfigROM();
  lhlSetIRMEnabled(FALSE);
  lhlBriForceBusReset();

  TCTaskWait(1200);  //enough for computer to determine the device has gone with new LLC fix.
}			


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
#include "spi.h"
#include "lhlIRM.h"


void targetBoardInit(void)
{
	
	//setting up chip select NCS6, used for parallel access to CPLD
	*((volatile uint32 *) MEM_SCSLR6) = 0x07000000;
	*((volatile uint32 *) MEM_SMSKR6) = 0x00000024;//SRAM, 512kb, set 0
	*((volatile uint32 *) MEM_SMTMGR_SET0) = 0x10000841;
	*((volatile uint32 *) MEM_SMCTLR) = 0x20f; //set0=8bit all other sets 16 bits
	
	
	//---------------------------------------------------------
	//                   3           2            1           0  
	//           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
	//---------------------------------------------------------
	// GPCSR_IO_SEL     0000 0000 0000 0010 1001 0101 1010 1000
	//             HEX     0    0    0    2    9    5    a    8
	//---------------------------------------------------------
	// CLKO             .... .... .... .... .... .... .... ...0  Always enable CLKO
	// A20              .... .... .... .... .... .... .... .00.  ENC 1a Input (JP1004 Pin 6)
	// A21              .... .... .... .... .... .... ...0 1...  CS6, used for CPLD CS
	// A22              .... .... .... .... .... .... .01. ....  CS5, CPLD Pin 70 (not used)
	// A23              .... .... .... .... .... ...1 1... ....  GPIO16 (JP1004 Pin 5)
	// CLKEN            .... .... .... .... .... ..0. .... ....  GPIO1, TP2213
	// BNK2             .... .... .... .... .... 01.. .... ....  GPIO2, TP1033
	// BNK3             .... .... .... .... ..01 .... .... ....  GPIO3, TP2221
	// SRAM_RDY         .... .... .... .... .0.. .... .... ....  GPIO4, TP2218
	// CS2              .... .... .... ...0 1... .... .... ....  GPIO5, TP1055
	// CS3              .... .... .... .01. .... .... .... ....  GPIO6, TP1057
	// OPTO_OUT         .... .... ..00 0... .... .... .... ....  ADAT out
	// OPTO_IN          .... ...0 00.. .... .... .... .... ....  ADAT in
	// GPIO7            .... ..0. .... .... .... .... .... ....  GPIO7, TP2217
	// GPIO8            .... .0.. .... .... .... .... .... ....  GPIO8, CPLD Soft SPI CK
	// GPIO9            .... 0... .... .... .... .... .... ....  GPIO9, TP2215
	*((volatile uint32 *) GPCSR_IO_SEL) = 0x295a8;



	
	
	//---------------------------------------------------------
	//                   3           2            1           0  
	//           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
	//---------------------------------------------------------
	// GPCSR_VIDEO_SEL  0000 0000 0000 0011 1100 0000 0000 0000
	//             HEX     0    0    0    3    c    0    0    0
	//---------------------------------------------------------
	// VIO              .... .... .... .... .... .... .... ...0  Don't care
	// VDATA            .... .... .... .... .... .... .... ..0.  Other function for video port
	// V_FSYN           .... .... .... .... .... .... .... 00..  Not used
	// V_RDY            .... .... .... .... .... .... ..00 ....  Not used
	// V_CLK            .... .... .... .... .... .... 00.. ....  Not used
	// V_ENDB           .... .... .... .... .... ..00 .... ....  Not used
	// V_VALID          .... .... .... .... .... 00.. .... ....  Not used
	// GPIO13           .... .... .... .... ..00 .... .... ....  GPIO13, TP2216
	// GPIO14           .... .... .... .... 11.. .... .... ....  WC_IN
	// GPIO15           .... .... .... ..11 .... .... .... ....  WC_OUT
	// BLKS             .... .... .... .0.. .... .... .... ....  BLK_IN/GPIO13, TP2216
	// V_EN             .... .... .... 0... .... .... .... ....  Video port disabled
	// CLK_MSTR         .... .... ...0 .... .... .... .... ....  Slave (not used)
	// GPIO10           .... .... ..0. .... .... .... .... ....  GPIO10, CPLD Soft SPI SS
	// GPIO11           .... .... .0.. .... .... .... .... ....  GPIO11, CPLD Soft SPI MOSI
	// GPIO12           .... .... 0... .... .... .... .... ....  GPIO12, CPLD Soft SPI MISO



	*((volatile uint32 *) GPCSR_VIDEO_SEL) = 0x3c000;	
	
	
	//---------------------------------------------------------
    //                   3           2            1           0  
    //           Bit No 1098 7654 3210 9876 5432 1098 7654 3210
    //---------------------------------------------------------
    //                  0000 0000 0000 0000 0000 0110 1000 0000
    // GPIO_A_DDR  HEX     0    0    0    0    0    6    8    0
    //---------------------------------------------------------
	// GPIO1            .... .... .... .... .... .... .... ...0  TP2213
	// GPIO2            .... .... .... .... .... .... .... ..0.  TP1033
	// GPIO3            .... .... .... .... .... .... .... .0..  TP2221
	// GPIO4            .... .... .... .... .... .... .... 0...  TP2218
	// GPIO5            .... .... .... .... .... .... ...0 ....  TP1055
	// GPIO6            .... .... .... .... .... .... ..0. ....  TP1057
	// GPIO7            .... .... .... .... .... .... .0.. ....  TP2217
	// GPIO8            .... .... .... .... .... .... 1... ....  CPLD Soft SPI CK
	// GPIO9            .... .... .... .... .... ...0 .... ....  TP2215
	// GPIO10           .... .... .... .... .... ..1. .... ....  CPLD Soft SPI SS
	// GPIO11           .... .... .... .... .... .1.. .... ....  CPLD Soft SPI MOSI
	// GPIO12           .... .... .... .... .... 0... .... ....  CPLD Soft SPI MISO
	// GPIO13           .... .... .... .... ...0 .... .... ....  TP2216
	// GPIO14           .... .... .... .... ..0. .... .... ....  WC_IN
	// GPIO15           .... .... .... .... .0.. .... .... ....  WC_OUT
	// GPIO16           .... .... .... .... .0.. .... .... ....  JP1004 Pin 5
	*((volatile uint32 *) GPIO_A_DDR) = 0x680;
	*((volatile uint32 *) GPIO_A_DR) = 0x200;   //turn off all GPIO except SS
	
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


// SPI functionality using the DICE II Software SPI
// 
// The EVM002 has two SPI devices:
// CPLD	To control LED's SW etc
// Codec to control the codecs
//
// The system uses only one Slave Select and by
// using the upper 4 bits of the 16 bit word as an
// address it can be assured that Codecs won't react
// to CPLD commands and so on.
//
// Two handles are created, one for each device.
// In this case both devices have the samme SPI settings
// and they use the same SS so just one handle could have
// been used.

static uint32 codecSSId;
static uint32 cpldSSId;


// This function drives the SS pin. In this case both devices
// share the SS pin but if multiple SS pins were used this would
// be the function setting the correct one.
// When ssId==0xffffffff the SS pin should be released.
//
static void spi_slave_select(uint32 ssId)
{
	if (ssId==0xffffffff)
		*((volatile uint32 *) GPIO_A_DR) |= 0x200;
	else
		*((volatile uint32 *) GPIO_A_DR) &= ~0x200;
}


// Generic function writing to a specified SPI device
static void targetWriteGenericSPIWord (uint32 sid, uint32 w)
{
	HRESULT hResult;
	
	hResult = spiOpBlock(sid, w, NULL);
}

// Generic function reading from a specified device
static uint32 targetReadGenericSPIWord (uint32 sid, uint32 w)
{
	HRESULT hResult;
	uint32 retval;
	
	hResult = spiOpBlock(sid, w, &retval);
	return retval;
}


// Specific exported functions for the SPI devices
//
//
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


// The DICE II SPI is based on a softwar eimplementation
// The SPI_SPEED define, sets the number of while loops
// used between each edge. A value of 0 will yield the
// fastest SPI which is ~100kHz (100kbps) assuming a
// CPU clock of ~50MHz.
// It is not recomended to run at a slower speed as the 
// timing is based on busy waiting and it will steal CPU
// from other threads.
//
#define SPI_SPEED 0 


// Holds the switch setting read as boot
static uint32 initialSwitchSetting;


// Get the initial switch setting detected at boot.
uint32 targetGetInitialSW (void)
{
	return initialSwitchSetting;
}


// The CPLD on the DICE002 board implements functions to
// control LED's read Switches and pass through SPI commands
// to the Codecs.
// The hardware guide contains a detailed description of the
// functions.
//
// The CPLD has 8 internal registers. Those registers can either
// be accessed through SPI commands or parrellel access. By default
// only the SPI access is enabled.
//
// Parallel is much faster than SPI and especially on the DICE II
// where SPI is based on software. The CPLD will be put into parallel
// mode during initialization and use that mode for all LED and SW access.
//
// Before enabling any of the modes in the CPLD it is determined if the
// code running in the CPLD is compatible with the software. At the time of
// writing this module the current version was 18.

static uint32 cpldVer;
static bool cpldSupported;

void targetGetCPLDInfo (uint8 * ver, BOOL * bSupported)
{
	*ver = (uint8)cpldVer;
	*bSupported = cpldSupported;
}


void targetSpiInit(void) 
{

	//Initialize the DICE II SPI module to use the following GPIO
	//MOSI = GPIO11
	//MISO = GPIO12
	//CLK  = GPIO8
	spiInitialize(11, 12, 8);
	
	//Create the two slave devices
	spiGetSlaveDevice(&cpldSSId);
	spiGetSlaveDevice(&codecSSId);

	//Configure each of the devices and the slave select call back.
	spiConfigure(0,         SPI_SET_SS_ROUTINE, spi_slave_select);
	spiConfigure(codecSSId, SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(codecSSId, SPI_WSIZE, (void *)SPI_WSIZE_16);
	spiConfigure(cpldSSId,  SPI_RATE, (void *)SPI_SPEED);
	spiConfigure(cpldSSId,  SPI_WSIZE, (void *)SPI_WSIZE_16);	

	//at init time, let's talk to CPLD and initialize
	//read the CPLD version
	spiOpBlock(cpldSSId, CPLD_SPI_RD_CMD(CPLD_VER_REG,0), &cpldVer);
	if (cpldVer>=255) cpldVer = 0;
	cpldSupported = ((cpldVer >= 12) && (cpldVer < 128));
	
	if (cpldSupported) 
	{
		//Enable parallel mode and passthrough of SPI to both Codecs
		spiOpBlock(cpldSSId, CPLD_SPI_WR_CMD(CPLD_CTRL_REG, CPLD_CTRL_PAR_EN | 
															CPLD_CTRL_CODEC_EN | 
															CPLD_CTRL_SPI1_EN | 
															CPLD_CTRL_SPI2_EN | 
															CPLD_CTRL_USER_EN), NULL);
 		//read the initial switches
		spiOpBlock(cpldSSId, CPLD_SPI_RD_CMD(CPLD_SW_REG,0), &initialSwitchSetting);
	}
	else
		initialSwitchSetting = 0x1; //Flash mode
	
}	

//Virtual LED and SW functions
volatile uint8 * pTargetCpld = ((volatile uint8 *) 0x07000000);

static uint8 ledMsk;

static void updateLED(void)
{
	if (!cpldSupported) return;
	pTargetCpld[CPLD_LED_REG] = ledMsk;
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
	
	return pTargetCpld[CPLD_SW_REG] & 0x0f;
}



// This function is called by the system when it needs to perform a
// soft reset. As we are using CS6 which becomes A21 during boot we would
// actually chip select the CPLD when trying to boot from the Flash and that
// would make the system crash.
// To avoid this we disable the CPLD parallel mode before resetting.
void targetBoardPrepareReset(void)
{
	//this function will assure that the board is ready for reset
	if (cpldSupported) 
	{
			//we do not want the CPLD to respond to parallel requests at this point
			spiOpBlock(cpldSSId, CPLD_SPI_WR_CMD(CPLD_CTRL_REG, CPLD_CTRL_CODEC_EN | 
																CPLD_CTRL_SPI1_EN | 
																CPLD_CTRL_SPI2_EN | 
																CPLD_CTRL_USER_EN), NULL);
			//disable the CS6
			*((volatile uint32 *) MEM_SMSKR6) = 0x00000020;//SRAM, off, set 0

	}
	targetSetMinConfigROM();
	lhlSetIRMEnabled(FALSE);
  	lhlBriForceBusReset();

  	TCTaskWait(1200);  //enough for computer to determine the device has gone with new LLC fix.
}			


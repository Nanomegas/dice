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


/****************************************************************************
USE_UART1_FOR_MIDI, this define is used by this application to determine
if the board should use UART1 for MIDI.
IF YOU ARE USING GDB TO DEBUG OVER THIS SERIAL PORT YOU MUST NOT DEFINE THIS
IDENTIFIER.
****************************************************************************/
#define USE_UART1_FOR_MIDI



/****************************************************************************
isChipDiceJR, this function detects if the chip is a DICE JR or a 
DICE Mini. It is used by this application to determine how to setup the 
audio ports and streaming.
****************************************************************************/
extern BOOL  isChipDiceJR (void);


/****************************************************************************
HPLL_CLK_HZ, this indicates which frequency is used for the JET PLL. Typically
the SCLK input from the 1394 PHY is used in which case it should be defined as
#define HPLL_CLK_HZ   (2*24576000) // HPLL clock 49.152 Mhz
If the PLL register is programmed to use the internal clock doubler this define
should be set to 2xXTAL frequency.
If this define is incorrect the DAL will report the wrong rate
****************************************************************************/
#define HPLL_CLK_HZ   (2*24576000) // HPLL clock 49.152 Mhz



/****************************************************************************
targetBoardInit, Initialize the GPCSR registers, Chip Selects etc.
This function is called at early boot and should not make OS calls.
****************************************************************************/
extern void targetBoardInit(void);


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



/****************************************************************************
targetChangeAudioPorts, This application uses the Optical port (Port0-2) for 
two purposes. In some cases it is used for ADAT in other it is used for
SPDIF/TOS. This function changes the GPCSR accordingly. There are also modes
for experimentation which program all ports for InS.
We suggest that changes to GPCSR are located in this module.
****************************************************************************/
typedef enum
{
	APM_NORMAL, //Default for the EVM
	APM_TOS,	//Default, but use Optical for TOS
	APM_ALL_INS //All ports are InS for experiments.
}APM_MODES;

extern void targetChangeAudioPorts (uint8 mode);


/****************************************************************************
SPI and CPLD functions. The use of the SPI for this target is defined in
this module. Two SPI devices are instantiated, one is the CPLD the other is
the Codec's. Only one SS signal is used and the 'address' is used to determine
if the command is for the CPLD or the Codec's.
****************************************************************************/
extern void targetSpiInit(void);
extern void targetWriteCPLDSPIWord (uint32 w);
extern uint32 targetReadCPLDSPIWord (uint32 w);
extern void targetWriteCodecSPIWord (uint32 w);
extern uint32 targetGetInitialSW (void);

/****************************************************************************
targetGetCPLDInfo, the CPLD code must be of a certain version to support
LED and SW support.
****************************************************************************/
extern void targetGetCPLDInfo (uint8 * ver, BOOL * bSupported);

/****************************************************************************
If the chip is a DICE JR it is possible to access the CPLD functions
through the parallel port using CS2. The memory space can be accessed through
the pTargetCpld pointer if the chip is DICE JR. Using this pointer with DICE Mini
will not work.
****************************************************************************/


//Register offsets used for SPI and Parallel modes
#define CPLD_CTRL_REG	0
#define CPLD_STAT_REG	1
#define CPLD_UIB_D_REG	2
#define CPLD_LED_REG	3
#define CPLD_SW_REG		4
#define CPLD_VER_REG	7

//Definition of CTRL_REG fields
#define CPLD_CTRL_PAR_EN	0x80
#define CPLD_CTRL_CODEC_EN	0x40
#define CPLD_CTRL_SPI1_EN	0x20 //SS for Codec 1
#define CPLD_CTRL_SPI2_EN	0x10 //SS for Codec 2
#define CPLD_CTRL_USER_EN	0x08 //enables control of LED and SW (stop running LED's)
#define CPLD_CTRL_LCD_EN	0x02 //use UIB-D as an LCD port
#define CPLD_CTRL_LCD_RS	0x01 //set LCD command or data	


//SPI 'address' used for CPLD
#define CPLD_SPI_ADDR			0xc000
#define CPLD_SPI_WR				0x2000
#define CPLD_SPI_RD_CMD(a,d)	(CPLD_SPI_ADDR | (((uint16)(a))<<8) | d)
#define CPLD_SPI_WR_CMD(a,d)	(CPLD_SPI_ADDR | CPLD_SPI_WR | (((uint16)(a))<<8) | d)

extern volatile uint8 * pTargetCpld;



typedef enum
{
	TGT_LED_OFF,
	TGT_LED_ON,
	TGT_LED_TGL
} TGT_LED_STATE;

typedef enum
{
	TGT_LED1,
	TGT_LED2,
	TGT_LED3,
	TGT_LED4,
	TGT_LED5,
	TGT_LED6,
	TGT_LED7,
	TGT_LED8
} TGT_LED;

typedef enum
{
	TGT_SW1 = 0x01,
	TGT_SW2 = 0x02,
	TGT_SW3 = 0x04,
	TGT_SW4 = 0x08
} TGT_SW_MSK;

/****************************************************************************
The Switch and LED functions below virtualize the communication with the
CPLD. If the target chip is DICE JR the memory interface will be used, if the
chip is DICE Mini the SPI functions will be used.
PLEASE NOTE: These functions might block so they should
only be called from thread context. 
****************************************************************************/

extern void targetSetLED (TGT_LED led, TGT_LED_STATE state);
extern void targetSetAllLED (uint8 msk);
extern uint8 targetGetSw (void);


#endif //_TARGETBOARD_H

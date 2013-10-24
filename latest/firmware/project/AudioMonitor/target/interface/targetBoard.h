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
SPI and CPLD functions. The use of the SPI for this target is defined in
this module. Two SPI devices are instantiated, one is the CPLD the other is
the Codec's. Only one SS signal is used and the 'address' is used to determine
if the command is for the CPLD or the Codec's.
****************************************************************************/
extern void targetSpiInit(void);
extern void targetWriteDisplaySPIWord (uint32 w);
extern void targetWriteCodecSPIWord (uint32 w);


#endif //_TARGETBOARD_H

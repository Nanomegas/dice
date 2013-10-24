//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	myModes.c - Aggregates a number of configuration modes
	            which can be selected through CLI.
	
	Revisions:
		created 19/03/2005 ml
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "diceDriver.h"
#include "myModes.h"
#include "dicedriverInterface.h"
#include "i2c.h"

extern MODE_CFG modeCfg0;
extern MODE_CFG modeCfg1;
extern MODE_CFG modeCfg2;
extern MODE_CFG modeCfg3;
extern MODE_CFG modeCfg4;
extern MODE_CFG modeCfg5;
extern MODE_CFG modeCfg6;
extern MODE_CFG modeCfg7;
extern MODE_CFG modeCfg8;
extern MODE_CFG modeCfg9;
extern MODE_CFG modeCfg10;
extern MODE_CFG modeCfg11;
extern MODE_CFG modeCfg12;
extern MODE_CFG modeCfg13;
extern MODE_CFG modeCfg14;
extern MODE_CFG modeCfg15;

MODE_CFG * modeDesc[MYMODES_NBMODES] = {
	&modeCfg0,
	&modeCfg1,
	&modeCfg2,
	&modeCfg3,
	&modeCfg4,
	&modeCfg5,
	&modeCfg6,
	&modeCfg7,
	&modeCfg8,
	&modeCfg9,
	&modeCfg10,
	&modeCfg11,
	&modeCfg12,
	&modeCfg13,
	&modeCfg14,
	&modeCfg15,
};


/// Utility functions to get and set the EEPROM configuration

typedef struct {
  uint32 magic;  //should be 0xdeadbeef
  uint32 length; //=12, number of bytes in this structure
  uint32 mode;  //low 8 bits contain mode, upper bits should be 0
} MYMODE_EECONFIG;

static MYMODE_EECONFIG curConfig;

#define EEPROM_ADDRESS					0x50

static HRESULT ConfigParamReadQuadlet(uint32 offset, uint32 *pData)
{
	uint8 readBuf[4];
	uint32 page;
	page = (offset > 255);
	i2cWriteValue((uint16)(EEPROM_ADDRESS+page), offset&0xFF);
	i2cRead((uint16)(EEPROM_ADDRESS+page), readBuf, 4);
	*pData = (readBuf[0] << 24) + (readBuf[1] << 16) + (readBuf[2] << 8) + readBuf[3];
	return NO_ERROR;
}


static HRESULT ConfigParamWriteQuadletToEEPROM(uint32 offset, uint32 data)
{
	uint8 dataBuf[4];
	uint8 writebuf[2];
	uint32 page;
	int i, retry, status = 0;

	page = (offset > 255);

	dataBuf[0] = (data & 0xFF000000 ) >> 24;
	dataBuf[1] = (data & 0x00FF0000 ) >> 16;
	dataBuf[2] = (data & 0x0000FF00 ) >> 8;
	dataBuf[3] = (data ) & 0xFF;


	for(i = 0; i < 4; i++) {
		writebuf[0] = (offset + i);
		writebuf[1] = dataBuf[i];
		retry = 0;
		status = i2cWrite((uint16)(EEPROM_ADDRESS+page), writebuf, 2);
		// There is a 5ms write cycle time, so we wait a bit here
		TCTaskWait(1);
		while((status!= 0) && (retry < 20))
		{
			status = i2cWrite((uint16)(EEPROM_ADDRESS+page), writebuf, 2);
			retry++;
			TCTaskWait(1);
		}
		if (retry >= 20)
		{
			sysDebugPrintf("myMode: EEPROM write error!\n");
		}
	}	
	return NO_ERROR;
}

static void myModeCliInstallTools(void);

MODE_CFG * myModesInitialize (uint32 initFlags)
{
  //try to read the structure from EEPROM
  ConfigParamReadQuadlet(0, &curConfig.magic);
  if (curConfig.magic != 0xdeadbeef)
  {
	sysDebugPrintf("myMode: EEPROM not initialized. Initializing to MODE0\n");
	curConfig.magic = 0xdeadbeef;
	curConfig.length = 12;
	curConfig.mode = 0;
	ConfigParamWriteQuadletToEEPROM(0, curConfig.magic);
	ConfigParamWriteQuadletToEEPROM(4, curConfig.length);
	ConfigParamWriteQuadletToEEPROM(8, curConfig.mode);
  }
  ConfigParamReadQuadlet(8, &curConfig.mode);
  if (curConfig.mode >= MYMODES_NBMODES)
  {
    sysDebugPrintf("myMode: Illegal mode, mode set to MODE0\n");
    curConfig.mode = 0;
  }
  //initialize the selected mode
  modeDesc[curConfig.mode]->fInit(initFlags);
  myModeCliInstallTools();
  sysDebugPrintf("***************************************\n");
  sysDebugPrintf("* myMode board configuration          *\n");
  sysDebugPrintf("* Board is configured to mode: %-3i    *\n",curConfig.mode);
  sysDebugPrintf("* Use: dicedriver.dump                *\n");
  sysDebugPrintf("***************************************\n");
  return modeDesc[curConfig.mode];
}

HRESULT myModeSetMode (uint32 mode)
{
  if (mode>= MYMODES_NBMODES)
  {
    sysDebugPrintf("myMode: Illegal mode, mode set to MODE0\n");
    mode = 0;
  }
  curConfig.mode = mode;
  ConfigParamWriteQuadletToEEPROM(8, curConfig.mode);
  return NO_ERROR;
}

//CLI to help set the mode
static HRESULT	setModeCliFunction(uint32 mode)
{
  myModeSetMode (mode);
  cliPrintf("Reboot for mode change to take effect\n");
  cliPrintf("After reboot use dicedriver.dump to see current configuration\n");
  return NO_ERROR;
}

static HRESULT	dumpModeCliFunction(void)
{
  uint32 i;
  
  cliPrintf("Dump MODES\n");
  for (i=0; i<MYMODES_NBMODES; i++)
  {
    cliPrintf("%-3i %c %s\n",i,(curConfig.mode==i)?'*':' ',modeDesc[i]->modeName);
  }
  cliPrintf("\n");
  return NO_ERROR;
}


// My Application's CLI Useage String (help for the end user)
static  char	setModeCli_UsageString[]=\
				"setmode: \n\r"\
				"use: setmode <mode>\n\r"\
				"Set mode for the board\n\r"\
				"------------------------------------------------------------\n\r"\
				"";

static	CLIDescriptor	setModeCli_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&setModeCliFunction},
	{kCLI_UsageString, 		(uint32)setModeCli_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},					
	{kCLI_ConstantsString,	(uint32)"myConst=0"},
	{kCLI_ModuleName,		(uint32)"set mode Command Line Command"},
	{kCLI_GroupName,		(uint32)"mymode"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	dumpModeCli_UsageString[]=\
				"dump: \n\r"\
				"use: dump\n\r"\
				"Dump all modes and current mode\r"\
				"------------------------------------------------------------\n\r"\
				"";

static	CLIDescriptor	dumpModeCli_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dumpModeCliFunction},
	{kCLI_UsageString, 		(uint32)dumpModeCli_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},					
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"dump mode Command Line Command"},
	{kCLI_GroupName,		(uint32)"mymode"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static void myModeCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(setModeCli_Descriptor);
	hResult += cliInstallCLIDescriptor(dumpModeCli_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avs diceDriver tools");
	}
}

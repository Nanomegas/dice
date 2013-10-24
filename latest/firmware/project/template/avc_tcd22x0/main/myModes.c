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

#ifdef _DICE_DRIVER
#include "dicedriverInterface.h"
#include "diceDriver.h"
#elif defined _AVC
#include "avcdriverInterface.h"
#include "avcDriver.h"
#endif

#include "myModes.h"
#include "targetboard.h"
//#include "i2c.h"

extern MODE_CFG modeCfg0_JR;
extern MODE_CFG modeCfg1_JR;
extern MODE_CFG modeCfg2_JR;
extern MODE_CFG modeCfg3_JR;
extern MODE_CFG modeCfg4_JR;
extern MODE_CFG modeCfg5_JR;
extern MODE_CFG modeCfg6_JR;
extern MODE_CFG modeCfg7_JR;
extern MODE_CFG modeCfg8_JR;
extern MODE_CFG modeCfg6_JR;//Mode9 single test for backward compatibility
extern MODE_CFG modeCfg7_JR;//Mode10 single test for backward compatibility
extern MODE_CFG modeCfg0_Mini;
extern MODE_CFG modeCfg1_Mini;
extern MODE_CFG modeCfg2_Mini;
extern MODE_CFG modeCfg3_Mini;
extern MODE_CFG modeCfg4_Mini;
extern MODE_CFG modeCfg5_Mini;
extern MODE_CFG modeCfg6_Mini;
extern MODE_CFG modeCfg7_Mini;
extern MODE_CFG modeCfg8_Mini;
extern MODE_CFG modeCfg6_Mini;//Mode9 single test for backward compatibility
extern MODE_CFG modeCfg7_Mini;//Mode10 single test for backward compatibility



MODE_CFG * modeDesc[2][MYMODES_NBMODES] = {
 { //Mini modes
  &modeCfg0_Mini,
  &modeCfg1_Mini,
  &modeCfg2_Mini,
  &modeCfg3_Mini,
  &modeCfg4_Mini,
  &modeCfg5_Mini,
  &modeCfg6_Mini,
  &modeCfg7_Mini,
  &modeCfg8_Mini,
  &modeCfg6_Mini,
  &modeCfg7_Mini
 },
 { // JR modes
  &modeCfg0_JR,
  &modeCfg1_JR,
  &modeCfg2_JR,
  &modeCfg3_JR,
  &modeCfg4_JR,
  &modeCfg5_JR,
  &modeCfg6_JR,
  &modeCfg7_JR,
  &modeCfg8_JR,
  &modeCfg6_JR,
  &modeCfg7_JR
 }
};



static uint8 curMode=0;

MODE_CFG * myModesInitialize (uint32 mode, uint32 ac3bits)
{
	//try to read the structure from EEPROM
	curMode = mode;
	if (curMode >=	MYMODES_NBMODES)
	  curMode = 0;

  curMode = 3;	// *** only one mode implemented so far for AVC

  modeDesc[isChipDiceJR()?1:0][curMode]->fInit(ac3bits);
  
  sysDebugPrintf("***************************************\n");
  sysDebugPrintf("* myMode board configuration          *\n");
  sysDebugPrintf("* Board is hard coded to mode: %-3i    *\n",curMode);
  sysDebugPrintf("* Name: %-28s  *\n",modeDesc[isChipDiceJR()?1:0][curMode]->modeName);
#ifdef _DICE_DRIVER
  sysDebugPrintf("* Use: dicedriver.dump                *\n");
#elif defined _AVC
  sysDebugPrintf("* Use: avcdriver.dump                 *\n");
#endif
  sysDebugPrintf("***************************************\n");
  return modeDesc[isChipDiceJR()?1:0][curMode];
}

static void applyAC3OutStream (STREAM_CFG * pCfg, uint32 isoc1, uint32 isoc2)
{
	uint16 mask;

	if (pCfg->nbRxStreams)
	{
		mask = (1<<pCfg->pRxStreamCfg[0].AudioChannels)-1;
		pCfg->pRxStreamCfg[0].AC3Capabilities = isoc1 & mask;
	}
	if (pCfg->nbRxStreams>1)
	{
		mask = (1<<pCfg->pRxStreamCfg[1].AudioChannels)-1;
		pCfg->pRxStreamCfg[1].AC3Capabilities = isoc2 & mask;
	}
}

void myModesApplyAC3Out (MODE_CFG * pMode, uint32 isoc1, uint32 isoc2)
{
	applyAC3OutStream(pMode->low,isoc1,isoc2);
	applyAC3OutStream(pMode->mid,isoc1,isoc2);
	applyAC3OutStream(pMode->high,isoc1,isoc2);
}

#ifdef _CLI
HRESULT	myModesDump(void)
{
  uint32 i;
  
  cliPrintf("Dump MODES\n");
  for (i=0; i<MYMODES_NBMODES; i++)
  {
    cliPrintf("%-3i %c %s\n",i,(curMode==i)?'*':' ',modeDesc[isChipDiceJR()?1:0][i]->modeName);
  }
  cliPrintf("\n");
  return NO_ERROR;
}
#endif


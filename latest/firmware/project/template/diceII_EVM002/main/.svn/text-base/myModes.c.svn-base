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
#include "targetboard.h"
#include "dicedriverInterface.h"
//#include "i2c.h"

extern MODE_CFG modeCfg0;
extern MODE_CFG modeCfg1;
extern MODE_CFG modeCfg2;
extern MODE_CFG modeCfg3;
extern MODE_CFG modeCfg4;
extern MODE_CFG modeCfg5;
extern MODE_CFG modeCfg6;
extern MODE_CFG modeCfg7;



MODE_CFG * modeDesc[MYMODES_NBMODES] = {
  &modeCfg0,
  &modeCfg1,
  &modeCfg2,
  &modeCfg3,
  &modeCfg4,
  &modeCfg5,
  &modeCfg6,
  &modeCfg7,
  &modeCfg0, //mode 8 just use mode 0
  &modeCfg6, //mode 9 16x16 test mode for EVM backward compatibility
  &modeCfg7  //mode 10 32x32 test mode for EVM backward compatibility
};



static uint8 curMode=0;

MODE_CFG * myModesInitialize (uint32 mode, uint32 ac3bits)
{
	//try to read the structure from EEPROM
	curMode = mode;
	if (curMode >=	MYMODES_NBMODES)
	  curMode = 0;

  modeDesc[curMode]->fInit(ac3bits);
  
  sysDebugPrintf("***************************************\n");
  sysDebugPrintf("* myMode board configuration          *\n");
  sysDebugPrintf("* Board is configured to mode: %-3i    *\n",curMode);
  sysDebugPrintf("* Name: %-28s  *\n",modeDesc[curMode]->modeName);
  sysDebugPrintf("* Use: dicedriver.dump                *\n");
  sysDebugPrintf("***************************************\n");
  return modeDesc[curMode];
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
    cliPrintf("%-3i %c %s\n",i,(curMode==i)?'*':' ',modeDesc[i]->modeName);
  }
  cliPrintf("\n");
  return NO_ERROR;
}
#endif


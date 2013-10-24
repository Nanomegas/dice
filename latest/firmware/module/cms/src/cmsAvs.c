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
 * DESCRIPTION: cmsAvs

	Purpose:	Handle allocation and dealocation of Isoc channels and bandwidth
				and configuration of AVS
				

	Revisions:
		created 30/08/2007 ml

****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "TCDebug.h"
#include "diceDefs.h"
#include "avs.h"
#include "avsMidi.h"
#include "avsFmt.h"
#include "dal.h"
#include "lal.h"
#include "irm.h"
#include "cmsAvs.h"

static char 	moduleName[] = __FILE__ ;

// task (thread) ID for this module
static uint32				cmsAvsThreadID;
static const char*			cmsAvsThreadName = "cmsAvsThread";




/////////////////////////// Isoc allocation /////////////////////////////////////

/*
For each Tx the following information is keept:
- Systime at last BR
- Target Isoc Channel
- Target Isoc bw
- Allocated Isoc channel (0xff if not alloc)
- Allocated Isoc bw (0 if not alloc)
- AVS Cfg
	audioCh
	commCh
	rateMode
	speed
	
- State:
	On Bus reset, clear Alloc information, set lastBRsystime
	On Timer systime > lastBRsystime, stop streaming on non allocated channels
	On Timer if Target!=Allocated, try to allocate, if not try to deallocate, if success start streaming
	On Isoc change, stop streaming set targets
	On bw change, stop streaming set targets
	State reports:
	DISABLED
	STREAMING_NO_ALLOC
	STREAMING_ALLOC
	NO_ISOC
	NO_BW
	
	
	

*/


typedef struct
{
	uint8  targetIsoc;
	int32  targetBW;
	uint8  allocIsoc;
	int32  allocBW;
	uint8  audioChs;
	uint8  bCommCh;
	uint8  speed;
	CMS_ISO_STAT state;
} CMS_ISOC_CH_STATE;

typedef struct
{
	uint32	lastBRsystime;
	DAL_RATE_MODE_ENUM rateMode;
	uint32	semID;
	CMS_ISOC_CH_STATE isocState[NUM_AVS_TX];
} CMS_ISOC_SYS;

static CMS_ISOC_SYS isocSystem;

uint8 cmsAvsIsIsocConfigured(uint8 txID)
{
	return isocSystem.isocState[txID].targetIsoc != CMSAVS_ISOC_DISABLE;
}

uint8 cmsAvsGetIsocCh(uint8 txID)
{
	return isocSystem.isocState[txID].allocIsoc;
}

static void cmsBusResetIndicationCallback(void)
{
	uint8 txID;
	for (txID=0; txID<NUM_AVS_TX; txID++)
	{
		isocSystem.isocState[txID].allocBW = 0;
		isocSystem.isocState[txID].allocIsoc = CMSAVS_ISOC_DISABLE;
	}
	isocSystem.lastBRsystime = TCTimeGet();
	TCSemaphoreSignal(isocSystem.semID);
}



static HRESULT cmsReallocateBW (int32 bw)
{
	HRESULT hRes;
	
	if (bw==0) return NO_ERROR;
	if (bw < 0)
		hRes = irmFreeBandwidth((uint32)(-bw));
	else
		hRes = irmAllocateBandwidth((uint32)(bw));
	return hRes;
}

static HRESULT cmsDeallocateIsoc (uint8 ch)
{
	HRESULT hRes;
	
	if (ch!=CMSAVS_ISOC_DISABLE)
	{
		hRes = irmFreeChannel(ch);
		if (hRes == E_IRM_CHANNEL_NOT_ALLOCATED)
		{
			//this is weird, it should be
			SYS_TRACE1(SYSDEBUG_TRACE_GENERAL,ch);
			return NO_ERROR;
		}
		return hRes;
	}
	return NO_ERROR;
}

static HRESULT cmsAllocateIsoc (uint8 ch)
{
	HRESULT hRes = NO_ERROR;
		
	if (ch!=CMSAVS_ISOC_DISABLE)
	{
		//allocate the new channel
		hRes = irmAllocateChannel(ch);						
	}
	return hRes;
}
#define ISO_GOOD(alloc,target) (((alloc)==(target)) || (((alloc)!=CMSAVS_ISOC_DISABLE) && ((target)==IRM_ANY_AVAIL_ISOCH_CHANNEL)))

static void cmsAvsThread (void *dummy)
{
	BOOL bGraceExpired;
	uint8 txID;
	CMS_ISOC_CH_STATE * pState;
	HRESULT hRes;
	
	DO_FOREVER
	{
		TCSemaphoreWaitTimeout (isocSystem.semID, 50); //every 50ms check stuff or when signaled	
		// check if grace period for Isoc streaming after BR has run out
		bGraceExpired = (TCTimeElapsed(isocSystem.lastBRsystime) > 1000);
		// now go through each Tx and see if work needs to be done
		for (txID=0; txID<NUM_AVS_TX; txID++)
		{
			CMS_ISO_STAT state = CMS_ISO_DISABLED;
			pState = &isocSystem.isocState[txID];
			//collect current state
			if (ISO_GOOD(pState->allocIsoc,pState->targetIsoc) && (pState->allocBW >= pState->targetBW))
			{
				//we are good to go, might need to downsize BW and make sure Isoc channel is running
				if (pState->allocIsoc == CMSAVS_ISOC_DISABLE)
				{
					//if the target is disabled we already stopped the Tx
					if (pState->targetIsoc!=CMSAVS_ISOC_DISABLE) avsStopTx(txID);
					state = CMS_ISO_DISABLED;
				}
				else
				{
					if (!avsIsTxStarted(txID)) avsStartTx(txID);
					state = CMS_ISO_STRM;
				}
				//we are going to block so make sure state is updated
				pState->state = state;
				if (pState->allocBW > pState->targetBW)
				{
					int bw = pState->targetBW;
					if (cmsReallocateBW (bw - pState->allocBW)==NO_ERROR)
						pState->allocBW = bw; //we succeded
					//if we failed we will just try next time
				}
			}
			else 
			{
				//so are we asked to shut down the channel?
				if (pState->targetIsoc == CMSAVS_ISOC_DISABLE)
				{
					//let's stop the Tx and deallocate BW
					state = CMS_ISO_DISABLED;
					//we are going to block so make sure state is updated
					pState->state = state;

					if ((pState->allocIsoc == CMSAVS_ISOC_DISABLE) || (cmsDeallocateIsoc (pState->allocIsoc) == NO_ERROR))
					{
						pState->allocIsoc = CMSAVS_ISOC_DISABLE;
						//better free the BW as well
						if (cmsReallocateBW (-pState->allocBW) == NO_ERROR)
						{
							pState->allocBW = 0;
						}
					}
				}
				else
				{
					//our grace period is up and our allocation does not match
					//we are not allowed to send anymore. 
					if (bGraceExpired)
  					{	
  						//if the target is disabled we already stopped the Tx
						if (pState->targetIsoc!=CMSAVS_ISOC_DISABLE) avsStopTx(txID);
					}
					
					if(!ISO_GOOD(pState->allocIsoc,pState->targetIsoc))
					{
						//let's free the old channel if needed
						state = CMS_ISO_NO_CH;
						if ((pState->allocIsoc == CMSAVS_ISOC_DISABLE) || (cmsDeallocateIsoc (pState->allocIsoc) == NO_ERROR))
						{
							pState->allocIsoc = CMSAVS_ISOC_DISABLE;
							pState->state = state; //following functions could block
							//then allocate the new one
							uint32 iso = pState->targetIsoc;
							hRes = NO_ERROR;
							if (pState->targetIsoc == IRM_ANY_AVAIL_ISOCH_CHANNEL)
								hRes = irmFindChannelAvailable(&iso);
								
							if (hRes == NO_ERROR) hRes = cmsAllocateIsoc (iso);
							if (hRes == E_IRM_CHANNEL_NOT_AVAILABLE) 
							{
								SYS_TRACE1(SYSDEBUG_TRACE_GENERAL,pState->targetIsoc);
								//let's at least try to deallocate the BW, if it fails it
								//will happen later
								//if the target is disabled we already stopped the Tx
								if (pState->targetIsoc!=CMSAVS_ISOC_DISABLE) avsStopTx(txID);
								if (cmsReallocateBW (-pState->allocBW) == NO_ERROR)
								{
									pState->allocBW = 0;
								}										
							}
							else if (hRes == NO_ERROR)
							{
								//so we got the channel, let's get the BW
								state = CMS_ISO_NO_BW;
								pState->allocIsoc = iso;
								avsSetCh(txID,true,pState->allocIsoc);

								int bw = pState->targetBW;
								hRes = cmsReallocateBW (bw - pState->allocBW);
								if (hRes==NO_ERROR)
								{
									pState->allocBW = bw; //we succeded
									//targetIsoc might just have changed
									if ((pState->targetIsoc == CMSAVS_ISOC_DISABLE) && !avsIsTxStarted(txID)) avsStartTx(txID);									
									state = CMS_ISO_STRM;
								}
							}
						}
					}
					else
					{
						//we've got the channel down, must be BW that is too small						
						state = CMS_ISO_NO_BW;
						int bw = pState->targetBW;
						if (cmsReallocateBW (bw - pState->allocBW)==NO_ERROR)
						{
							pState->allocBW = bw; //we succeded							
							//targetIsoc might just have changed
							if ((pState->targetIsoc == CMSAVS_ISOC_DISABLE) && !avsIsTxStarted(txID)) avsStartTx(txID);
							state = CMS_ISO_STRM;
						}
					}
				}
			}
			pState->state = state;
		}
	}
}	

//when a Tx is set to isocCh=CMSAVS_ISOC_DISABLE it will immediately release the transmitter
//this is important if the transmitter is to be used by another system like DiceDriver.
//After that the cmsAvs statemachine will not touch the AVS device for that Tx
HRESULT cmsAvsSetIsocConfig (uint8 txID, uint8 isocCh, uint8 audioChs, uint8 bCommCh, uint8 speed)
{
	uint32 sytInt,coefDR;
	
	if ((isocSystem.isocState[txID].audioChs != audioChs) || (isocSystem.isocState[txID].bCommCh != bCommCh) ||
	    (isocSystem.isocState[txID].speed != speed) || (isocSystem.isocState[txID].targetIsoc != isocCh))
	{
		// something changed, we need to reconfigure
		isocSystem.isocState[txID].audioChs = audioChs;
		isocSystem.isocState[txID].bCommCh = bCommCh;
	    isocSystem.isocState[txID].speed = speed;
	    isocSystem.isocState[txID].targetIsoc = isocCh;
		avsStopTx(txID);
		avsSetMidi(txID,true,bCommCh?1:0);
		avsSetDBS(txID,true,audioChs);
		avsSetTxSpeed(txID,speed);
		//if (isocCh != CMSAVS_ISOC_DISABLE)	avsSetCh(txID,true,isocCh);					
		avsFmtConfig(txID,true,0,AVS_FMT_LABEL);
		// we need to calculate the BW needed
		switch (isocSystem.rateMode)
		{
			default:
			case eDAL_RATE_MODE_LOW:
				sytInt = 8;
				break;
			case eDAL_RATE_MODE_LOW_MID:
			case eDAL_RATE_MODE_MID:
				sytInt = 16;
				break;
			case eDAL_RATE_MODE_HIGH:
				sytInt = 32;
				break;	
		}			
		switch (speed)
		{
			case SPEED_100: coefDR = 16; break;
			case SPEED_200: coefDR = 8; break;
			default:
			case SPEED_400: coefDR = 4; break;
		}
		isocSystem.isocState[txID].targetBW = (isocCh==CMSAVS_ISOC_DISABLE) ? 0 : ((audioChs + (bCommCh?1:0) + 3)*sytInt*coefDR);
		TCSemaphoreSignal(isocSystem.semID);
	}
	return NO_ERROR;
}

HRESULT cmsAvsSetRateMode (DAL_RATE_MODE_ENUM rateMode)
{
	//all transmitters should be disabled before this call
	uint8 txID;
	for (txID=0; txID<NUM_AVS_TX; txID++)
	{
		if ((isocSystem.isocState[txID].targetIsoc != CMSAVS_ISOC_DISABLE) || (avsIsTxStarted(txID)))
		{
			sysLogError(E_GEN_WRONG_CONTEXT, __LINE__, moduleName);
			return E_GEN_WRONG_CONTEXT;
		}
	}
	isocSystem.rateMode = rateMode;
	return NO_ERROR;

}

CMS_ISO_STAT cmsAvsGetIsoState (uint8 txID)
{
	return isocSystem.isocState[txID].state;
}


HRESULT cmsAvsInitialize(void)
{
	uint8 txID;
	HRESULT hResult = NO_ERROR;
	
	//Init data structure
	for (txID=0; txID<NUM_AVS_TX; txID++)
	{
		isocSystem.isocState[txID].targetIsoc = CMSAVS_ISOC_DISABLE;
		isocSystem.isocState[txID].allocIsoc = CMSAVS_ISOC_DISABLE;		
	}
	
	//Create Semaphore
	hResult = TCSemaphoreOpen(&isocSystem.semID, 0);
	if (hResult != NO_ERROR) return hResult;
	
	//Resume the task
	hResult = cmsAvsResumeTasks ();
	if (hResult != NO_ERROR) return hResult;

	//register the bus reset indication
    hResult = lalRegisterBRIndicationCallback(cmsBusResetIndicationCallback);
    if (hResult != NO_ERROR) return hResult;
	
#ifdef _CLI
	//cmsAvsCliInstallTools();
#endif //_CLI	

	return hResult;
}



/*********************************************************
	Resume tasks for this module
*/	
HRESULT cmsAvsResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskResume(cmsAvsThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT cmsAvsSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskSuspend(cmsAvsThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT cmsAvsInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskCreate(&cmsAvsThreadID, cmsAvsThreadName, cmsAvsThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}



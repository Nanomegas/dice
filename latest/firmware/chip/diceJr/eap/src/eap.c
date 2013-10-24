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
 * DESCRIPTION: EAP

	Purpose:	Enhanced Application Protocol
				

	Revisions:
		created 20/02/2007 ml

****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "TCDebug.h"
#include "diceDefs.h"
#include "sps.h"
#include "eapInternals.h"
#include "eapPersist.h"
#include "mixfix.h"
#include "lal.h"
#include "eapCli.h"

static char 	moduleName[] = __FILE__ ;


// task (thread) ID for this module
static uint32				eapThreadID;
static const char*			eapThreadName = "eapThread";
static uint32				eapCmdSemID;


// Memory allocations for spaces

static EAP_OFFSET_STRUCT eapOffsets =
{
	EAP_CAPABILITY_OFF      ,EAP_CAPABILITY_SZ,
	EAP_COMMAND_OFF			,EAP_COMMAND_SZ,	
	EAP_MIXER_OFF			,EAP_MIXER_SZ,		
	EAP_PEAK_OFF			,EAP_PEAK_SZ,			
	EAP_NEW_ROUTING_OFF		,EAP_NEW_ROUTING_SZ,
	EAP_NEW_STRM_CFG_OFF	,EAP_NEW_STRM_CFG_SZ,
	EAP_CURR_CFG_OFF		,EAP_CURR_CFG_SZ,	
	EAP_STND_ALONE_CFG_OFF  ,EAP_STND_ALONE_CFG_SZ,
	EAP_APP_SPACE_OFF		,0, //space for application specific area
	0
};


EAP_CAPABILITIES eapCapabilities;

EAP_NEW_ROUTING eapNewRouting;

EAP_NEW_STRM_CFG eapNewStrmCfg;

EAP_CURR_CFG eapCurrCfg;

EAP_STND_ALONE_CFG eapStndAloneCfg;

EAP_COMMAND eapCommand;


EAP_DEVINFO devInfo;

EAP_STND_ALONE_CFG * eapGetStandaloneCfg(void)
{
	return &eapStndAloneCfg;
}

static void notifyApp (EAP_EVT_ENUM event, uint32 data)
{
	if (devInfo.appCB) devInfo.appCB(event,data);
}

static HRESULT storeFlash (void)
{
	HRESULT hResult = NO_ERROR;

	hResult = eapCopyCurrToPersist();
	if (hResult != NO_ERROR) return hResult;
	hResult = eapCopyPrivateToPersist();
	spsChanged ();
	return hResult;
}

HRESULT eapStoreFlash (void)
{
	return storeFlash();
}

//Generic helper for simple rw operations

static HRESULT eapHandleSimpleRW (PB *packetBlock, uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType,void * pArea, uint32 areaSize, BOOL bRO)
{
	HRESULT hResult;
	QUADLET			*pPayload = NULL;

	
	if ((packetType == PB_TYPE_READ_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST_QUADLET))
	{
		payloadSize = 4;
	}
	if ((packetType == PB_TYPE_READ_REQUEST_QUADLET) || (packetType == PB_TYPE_READ_REQUEST))
	{
		if (regionOffSet + payloadSize > areaSize)
		{
#ifdef _SYSDEBUG	
			sysDebugPrintf("EAP, illegal read command! at OffSet = %08X\n\r",regionOffSet);
#endif // _SYSDEBUG
			return lhlReplyErrorResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
		return lalReplyReadResponse (packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)pArea + (int)regionOffSet), TRUE);		
	
	}
	else if ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST))
	{
		if ((regionOffSet + payloadSize > areaSize) || bRO)
		{
#ifdef _SYSDEBUG	
			sysDebugPrintf("EAP, illegal write command! at OffSet = %08X\n\r",regionOffSet);
#endif // _SYSDEBUG
			return lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;

		memcpy((uint32*) ((int)pArea + (int)regionOffSet), pPayload, payloadSize);
		return lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
	}
	else 
	{
#ifdef _SYSDEBUG	
		sysDebugPrintf("EAP, illegal command! at OffSet = %08X\n\r",regionOffSet);
#endif // _SYSDEBUG
		
		return lhlReplyErrorResponse(packetBlock, RSP_TYPE_ERROR, TRUE);	
	}
}


static HRESULT eapHandleAddressError (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
#ifdef _SYSDEBUG	
		sysDebugPrintf("EAP, illegal command! at OffSet = %08X\n\r",regionOffSet);
#endif // _SYSDEBUG
		
		return lhlReplyErrorResponse(packetBlock, RSP_TYPE_ERROR, TRUE);	

}



// Handlers for each space

static HRESULT eapHandleOffsets		 (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, (void *)&eapOffsets, sizeof(eapOffsets), TRUE);
}
	

static HRESULT eapHandleCapability   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &eapCapabilities, sizeof(eapCapabilities), TRUE);
}


static HRESULT eapHandleMixer		 (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	if (regionOffSet < 4)
		hResult = eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, (void *)mDicePtr(DICE_MIXER_BASE+4), 4, true);
	else
	{
		hResult = eapHandleSimpleRW (packetBlock, regionOffSet-4, payloadSize, packetType, (void *)mDicePtr(DICE_MIXER_BASE+0x800), MAX_MIX_COEFF*4, !eapIsMixerWriteable());	
		if (eapIsAutoStoreSupported () && eapIsMixerStored() && ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST))) 
		{
			eapCopyMixerToPersist (); 
			spsChanged ();
		}
	}
	return hResult;

}

static HRESULT eapHandleNewRouting	 (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &eapNewRouting, sizeof(eapNewRouting), FALSE);
}

static HRESULT eapHandleNewStreamCfg (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &eapNewStrmCfg, sizeof(eapNewStrmCfg), FALSE);
}

static HRESULT eapHandleCurrCfg		 (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &eapCurrCfg, sizeof(eapCurrCfg), TRUE);
}

static HRESULT eapHandleStndAloneCfg (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	hResult = eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &eapStndAloneCfg, sizeof(eapStndAloneCfg), FALSE);
	if (eapIsAutoStoreSupported () && ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST))) storeFlash ();
	return hResult;
}


static HRESULT eapHandleAppSpace (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	hResult = eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, devInfo.pPrivatePersistData, devInfo.nbPrivatePersistData, FALSE);
	if (packetType == PB_TYPE_WRITE_REQUEST_QUADLET)
	{
		payloadSize = 4;
	}
	if ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST))
	{
		notifyApp (eEAP_EVT_PERSIST_HOST_CHG,((regionOffSet&0xffff)<<16) | (payloadSize&0xffff));	
	}
	if (eapIsAutoStoreSupported () && ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST))) 
	{
		eapCopyPrivateToPersist (); 
		spsChanged ();
	}
	return hResult;
}


static HRESULT eapHandleCommand		 (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	QUADLET	*pPayload = NULL;

	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	if (packetType == PB_TYPE_WRITE_REQUEST_QUADLET)
	{
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;
		memcpy(&eapCommand.opcode, pPayload, sizeof(eapCommand.opcode));		
		hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
		TCSemaphoreSignal(eapCmdSemID);
		
	} 
	else
	{	
		hResult = eapHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &eapCommand, sizeof(eapCommand), TRUE);
	}
	return hResult;
}


static EAP_PEAK fwPeaks;
static uint16 apPeaks[MAX_ROUTE_ENTRIES];

static void handlePeakRead (uint32 first, uint32 n)
{
	uint32 * pEntry = (uint32 * )mDicePtr(DICE_ROUTER1_ENTRY);
	uint32 * pfwPeaks = &fwPeaks.peak[first];
	uint16 * papPeaks = &apPeaks[first];
	uint32   val;
	
	pEntry += first;
	
	
	while (n--)
	{
		val = (*pEntry); //read and clear hardware
		if ((val & 0xffff0000) != 0x02020000)
		{
			if ((val & 0xffff0000) >= ((*pfwPeaks) & 0xffff0000))
				*pfwPeaks = val;
			val >>= 16;
			if (((uint16)val) > *papPeaks)
				*papPeaks = val;
		}
		pEntry++;
		pfwPeaks++;
		papPeaks++;
	}
}

static void handleFwPeakClear (uint32 first, uint32 n)
{
	uint32 * pfwPeaks = &fwPeaks.peak[first];
		
	while (n--)
	{
		*pfwPeaks &= 0x0000ffff;
		pfwPeaks++;
	}
}

HRESULT eapGetMeterPeak (uint16 entry, uint16 * pValue)
{
	HRESULT hResult = NO_ERROR;
	if (entry >= MAX_ROUTE_ENTRIES)
	{
		hResult = E_GEN_ILLEGAL_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	handlePeakRead (entry, 1);
	*pValue = apPeaks[entry];
	apPeaks[entry] = 0;
	return hResult;
}


static HRESULT eapHandlePeak		 (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult = NO_ERROR;
	
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	if (eapIsPeakSupported ())
	{
		if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
		{
			payloadSize = 4;
		}
		if ((packetType == PB_TYPE_READ_REQUEST_QUADLET) || (packetType == PB_TYPE_READ_REQUEST))
		{
			if (regionOffSet + payloadSize > sizeof(EAP_PEAK))
			{
#ifdef _SYSDEBUG	
				sysDebugPrintf("EAP, illegal read peak command! at OffSet = %08X\n\r",regionOffSet);
#endif // _SYSDEBUG
				return lhlReplyErrorResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
			}
			//let's collect the peak values from hardware
			handlePeakRead (regionOffSet>>2, payloadSize>>2);
			hResult = lalReplyReadResponse (packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*)((int)&fwPeaks + (int)regionOffSet), TRUE);		
			handleFwPeakClear (regionOffSet>>2, payloadSize>>2);
			return hResult;
		}
	}
	return lhlReplyErrorResponse(packetBlock, RSP_TYPE_ERROR, TRUE);	
}

	


static HRESULT eapCoreCallback(PB *packetBlock)
{
	HRESULT hResult = NO_ERROR;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	OFFSET_1394		OffSetDest;
	uint32			payloadSize = 0;
	uint32			RegionOffSet = 0;	

	hResult = pbGetPacketType(packetBlock,&packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetDestinationOffset(packetBlock, &OffSetDest);
	if (hResult != NO_ERROR) return hResult;
	
	RegionOffSet = OffSetDest.Low - EAP_BASE_START;	

	hResult = pbGetDataLen(packetBlock,&payloadSize);
	if(hResult != NO_ERROR) 
		return hResult;	
	
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,RegionOffSet,payloadSize);
	if (RegionOffSet < (EAP_CAPABILITY_OFF*4)) 
	{
		return eapHandleOffsets (packetBlock,RegionOffSet,payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_COMMAND_OFF*4))
	{
		return eapHandleCapability (packetBlock,RegionOffSet-(EAP_CAPABILITY_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_MIXER_OFF*4))
	{
		return eapHandleCommand (packetBlock,RegionOffSet-(EAP_COMMAND_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_PEAK_OFF*4))
	{
		return eapHandleMixer (packetBlock,RegionOffSet-(EAP_MIXER_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_NEW_ROUTING_OFF*4))
	{
		return eapHandlePeak (packetBlock,RegionOffSet-(EAP_PEAK_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_NEW_STRM_CFG_OFF*4))
	{
		return eapHandleNewRouting (packetBlock,RegionOffSet-(EAP_NEW_ROUTING_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_CURR_CFG_OFF*4))
	{
		return eapHandleNewStreamCfg (packetBlock,RegionOffSet-(EAP_NEW_STRM_CFG_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_STND_ALONE_CFG_OFF*4))
	{
		return eapHandleCurrCfg (packetBlock,RegionOffSet-(EAP_CURR_CFG_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (EAP_APP_SPACE_OFF*4))
	{
		return eapHandleStndAloneCfg (packetBlock,RegionOffSet-(EAP_STND_ALONE_CFG_OFF*4),payloadSize,packetType);	
	}
	else if (RegionOffSet < (EAP_APP_SPACE_OFF*4)+devInfo.nbPrivatePersistData)
	{
		return eapHandleAppSpace (packetBlock,RegionOffSet-(EAP_APP_SPACE_OFF*4),payloadSize,packetType);
	}
	else
	{
		return eapHandleAddressError (packetBlock,RegionOffSet,payloadSize,packetType);
	}
}



static HRESULT eapConfigureRouter (DAL_RATE_MODE_ENUM mode)
{
	EAP_NEW_ROUTING *	pRt;
	HRESULT				hResult = NO_ERROR;
	uint16				n;
	uROUTER_SETUP		setup;
	uROUTER_SETUP	*	pSetup = (uROUTER_SETUP *) mDicePtr(DICE_ROUTER1_SETUP);
	uROUTER_ENTRY	*	pEntry = (uROUTER_ENTRY *) mDicePtr(DICE_ROUTER1_ENTRY);
	uint32			*	pSrc;
	
	switch (mode)
	{
		default:
		case eDAL_RATE_MODE_LOW: 
			pRt = &eapCurrCfg.lowRouter;
			break;
			
		case eDAL_RATE_MODE_MID: 
			pRt = &eapCurrCfg.midRouter;
			break;
			
		case eDAL_RATE_MODE_HIGH: 
			pRt = &eapCurrCfg.highRouter;
			break;
	
	}
	pSrc = pRt->route;
	n = pRt->routeEntries;
#ifdef _MIXFIX_EAP
	if (mode == eDAL_RATE_MODE_HIGH)
	{
		while (n--)
		{
			uint32 r = *pSrc++;
			if ((r & 0xf000) == (DEV_MIXER_RX<<12))
			{
				r = (r & ~0xf000) | (DEV_APB_RX<<12);
			}
			pEntry->reg = r;
			pEntry++;
		}
		for (n=0;n<8;n++)
		{
			pEntry->bit.srcBlkID = DEV_MIXER_RX;
			pEntry->bit.dstBlkID = DEV_APB_TX;
#ifdef _MIXFIX_PERFECT4
			pEntry->bit.srcCh = ((n<4)?n:(n+4));
#else //_MIXFIX_PERFECT4
			pEntry->bit.srcCh = n;
#endif //_MIXFIX_PERFECT4
			pEntry->bit.dstCh = n;
			pEntry++;
		}
		setup.bit.numEntry = pRt->routeEntries+8-1;
		setup.bit.peakon = 1;
		setup.bit.on = 1;
		mixfixEnable (TRUE);
	}
	else
	{
    mixfixEnable (FALSE);
#endif //_MIXFIX_EAP

		while (n--)
		{
			pEntry->reg = *pSrc++;
			pEntry++;
		}
		if (!pRt->routeEntries)
		{
			setup.bit.on = 0;
		}
		else
		{
			setup.bit.numEntry = pRt->routeEntries-1;
			setup.bit.peakon = 1;
			setup.bit.on = 1;
		}
#ifdef _MIXFIX_EAP
	}
#endif //_MIXFIX_EAP
	*pSetup = setup;
	return hResult;
}
		

static HRESULT eapConfigureDriver (DAL_RATE_MODE_ENUM mode)
{
	EAP_NEW_STRM_CFG *	pCfg;
	STREAM_CONFIG		tx[MAX_TX_STREAMS];
	STREAM_CONFIG		rx[MAX_RX_STREAMS];
	HRESULT				hResult = NO_ERROR;
	uint8				i,n;
	
	switch (mode)
	{
		default:
		case eDAL_RATE_MODE_LOW: 
			pCfg = &eapCurrCfg.lowStrmCfg;
			break;
			
		case eDAL_RATE_MODE_MID: 
			pCfg = &eapCurrCfg.midStrmCfg;
			break;
			
		case eDAL_RATE_MODE_HIGH: 
			pCfg = &eapCurrCfg.highStrmCfg;
			break;
	
	}
	n=0;
	for (i=0, n=0; i<pCfg->nbTx; i++, n++)
	{
		tx[i].AC3Capabilities = pCfg->stmCfg[n].AC3Capabilities;
		tx[i].AudioChannels = pCfg->stmCfg[n].nbAudioChannels;
		tx[i].midiChannels = pCfg->stmCfg[n].nbMidiChannels;
		tx[i].names = pCfg->stmCfg[n].names;
	}
	for (i=0; i<pCfg->nbRx; i++, n++)
	{
		rx[i].AC3Capabilities = pCfg->stmCfg[n].AC3Capabilities;
		rx[i].AudioChannels = pCfg->stmCfg[n].nbAudioChannels;
		rx[i].midiChannels = pCfg->stmCfg[n].nbMidiChannels;
		rx[i].names = pCfg->stmCfg[n].names;
	}	
	
    hResult = diceDriverConfigureStreaming (tx, pCfg->nbTx, rx, pCfg->nbRx);

	return hResult;
}


static HRESULT loadRouter (uint32 op)
{
	HRESULT hResult = NO_ERROR;
	BOOL    updCurr = FALSE;
	
	if (!eapIsRouterWriteable())
	{
		hResult = E_GEN_ILLEGAL_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (eapNewRouting.routeEntries > MAX_ROUTE_ENTRIES)
	{
		hResult = E_GEN_ILLEGAL_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	//move to each of the selected routings
	if (op & EAP_OP_LD_LOW)
	{
		eapCurrCfg.lowRouter = eapNewRouting;
		if (devInfo.currRateMode == eDAL_RATE_MODE_LOW) updCurr = TRUE;
	}
	if (op & EAP_OP_LD_MID)
	{
		eapCurrCfg.midRouter = eapNewRouting;
		if (devInfo.currRateMode == eDAL_RATE_MODE_MID) updCurr = TRUE;
	}
	if (op & EAP_OP_LD_HIGH)
	{
		eapCurrCfg.highRouter = eapNewRouting;
		if (devInfo.currRateMode == eDAL_RATE_MODE_HIGH) updCurr = TRUE;
	}
	if (updCurr) 
	{
		hResult = eapConfigureRouter (devInfo.currRateMode);
		notifyApp (eEAP_EVT_ROUTE_CHANGE,0);
	}	
	return hResult;
}

static HRESULT loadStrmCfg (uint32 op)
{
	HRESULT hResult = NO_ERROR;
	BOOL    updCurr = FALSE;
	
	if (!eapAreStreamsConfigurable())
	{
		hResult = E_GEN_ILLEGAL_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if ((eapNewStrmCfg.nbTx > MAX_TX_STREAMS) || (eapNewStrmCfg.nbRx > MAX_RX_STREAMS))
	{
		hResult = E_GEN_ILLEGAL_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	//move to each of the selected routings
	if (op & EAP_OP_LD_LOW)
	{
		eapCurrCfg.lowStrmCfg = eapNewStrmCfg;
		if (devInfo.currRateMode == eDAL_RATE_MODE_LOW) updCurr = TRUE;
	}
	if (op & EAP_OP_LD_MID)
	{
		eapCurrCfg.midStrmCfg = eapNewStrmCfg;
		if (devInfo.currRateMode == eDAL_RATE_MODE_MID) updCurr = TRUE;
	}
	if (op & EAP_OP_LD_HIGH)
	{
		eapCurrCfg.highStrmCfg = eapNewStrmCfg;
		if (devInfo.currRateMode == eDAL_RATE_MODE_HIGH) updCurr = TRUE;
	}
	if (updCurr) 
	{
		hResult = eapConfigureDriver (devInfo.currRateMode);
		diceDriverNotifyHost (DD_NOTIFY_RX_CFG_CHG_BIT | DD_NOTIFY_TX_CFG_CHG_BIT);
		notifyApp (eEAP_EVT_CFG_CHANGE,0);
	}	
	return hResult;
}

static HRESULT loadRtrStrmCfg (uint32 op)
{
	HRESULT hResult = NO_ERROR;
	hResult = loadRouter(op);
	if (hResult == NO_ERROR) hResult = loadStrmCfg(op);
	return hResult;
}

static HRESULT loadFlash (void)
{
	HRESULT hResult = NO_ERROR;
	hResult = eapCopyPersistToPrivate();
	if (hResult != NO_ERROR) return hResult;
	hResult = eapCopyPersistToCurr ();
	notifyApp (eEAP_EVT_PERSIST_RECALL,0);
	return hResult;
}


static void ececuteOpcode(void)
{	
	EAP_OP_ENUM opcode;
	if (eapCommand.opcode & EAP_OP_EXECUTE)
	{
		opcode = eapCommand.opcode & EAP_OP_MASK;
		switch (opcode)
		{
			case EAP_OP_NOP:
				eapCommand.retVal = NO_ERROR;
				break;
	
			case EAP_OP_LD_ROUTER:
				eapCommand.retVal = loadRouter(eapCommand.opcode);
				if (eapIsAutoStoreSupported () && eapIsRouterStored()) storeFlash ();
				break;
			
			case EAP_OP_LD_STRM_CFG:
				eapCommand.retVal = loadStrmCfg(eapCommand.opcode);
				if (eapIsAutoStoreSupported () && eapIsStrmCfgStored()) storeFlash ();
				break;
			case EAP_OP_LD_RTR_STRM_CFG:
				eapCommand.retVal = loadRtrStrmCfg(eapCommand.opcode);
				if (eapIsAutoStoreSupported () && (eapIsRouterStored() || eapIsStrmCfgStored())) storeFlash ();
				break;
			case EAP_OP_LD_FLASH_CFG:
				eapCommand.retVal = loadFlash();
				break;
			case EAP_OP_ST_FLASH_CFG:
				eapCommand.retVal = storeFlash();
				break;
				
			default:
				eapCommand.retVal = E_GEN_ILLEGAL_PARAM;
				break;
		}
		// clear execute bit
		eapCommand.opcode &= ~EAP_OP_EXECUTE;			
	}
}



static void eapThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(eapCmdSemID);
		if (hResult == NO_ERROR) 
		{
			ececuteOpcode();	
		}				
	}
}


static void copyRouterToCurrent (EAP_NEW_ROUTING * pCurr, const EAP_RATE_MODE_INIT * pInit)
{
	uint16 i;
	pCurr->routeEntries = (uint32)pInit->nbRouterEntries;
	
	for (i=0; i<pInit->nbRouterEntries; i++)
	{
		pCurr->route[i] = (uint32)pInit->pRouting[i];
	}
}
	
static void copyStrmCfgToCurrent(EAP_NEW_STRM_CFG * pCurr, const EAP_RATE_MODE_INIT * pInit)
{
	uint16 i,n;
	pCurr->nbTx = (uint32)pInit->nbTxStreams;
	pCurr->nbRx = (uint32)pInit->nbRxStreams;

	for (i=0,n=0; i<pCurr->nbTx; i++,n++)
	{
		pCurr->stmCfg[n].AC3Capabilities	= (uint32)pInit->pTxStreams[i].AC3Capabilities;
		strncpy (pCurr->stmCfg[n].names, pInit->pTxStreams[i].names, DD_SIZE_OF_NAMES);
		pCurr->stmCfg[n].nbAudioChannels    = (uint32)pInit->pTxStreams[i].AudioChannels;
		pCurr->stmCfg[n].nbMidiChannels     = (uint32)pInit->pTxStreams[i].midiChannels;
	}
	for (i=0; i<pCurr->nbRx; i++,n++)
	{
		pCurr->stmCfg[n].AC3Capabilities	= (uint32)pInit->pRxStreams[i].AC3Capabilities;
		strncpy (pCurr->stmCfg[n].names, pInit->pRxStreams[i].names, DD_SIZE_OF_NAMES);
		pCurr->stmCfg[n].nbAudioChannels    = (uint32)pInit->pRxStreams[i].AudioChannels;
		pCurr->stmCfg[n].nbMidiChannels     = (uint32)pInit->pRxStreams[i].midiChannels;
	}
}

	
HRESULT eapInitializeInterface(const EAP_INIT_DESC * pInit)
{
	HRESULT	hResult = NO_ERROR;
	
	eapCapabilities.routerCaps |= pInit->routerCapabilities;
	eapCapabilities.mixerCaps |= pInit->mixerCapabilities;
	eapCapabilities.generalCaps |= pInit->generalCapabilities;
	
	devInfo.currRateMode = eDAL_RATE_MODE_LOW;
	devInfo.appCB = pInit->appCB;
	devInfo.nbPrivatePersistData = pInit->nbPrivatePersistData;
	devInfo.pPrivatePersistData = pInit->pPrivatePersistData;
	if (!devInfo.pPrivatePersistData) devInfo.nbPrivatePersistData = 0;
	
	//update current from initial
	copyRouterToCurrent (&eapCurrCfg.lowRouter,&pInit->lowInit);
	copyRouterToCurrent (&eapCurrCfg.midRouter,&pInit->midInit);
	copyRouterToCurrent (&eapCurrCfg.highRouter,&pInit->highInit);
	copyStrmCfgToCurrent(&eapCurrCfg.lowStrmCfg,&pInit->lowInit);
	copyStrmCfgToCurrent(&eapCurrCfg.midStrmCfg,&pInit->midInit);
	copyStrmCfgToCurrent(&eapCurrCfg.highStrmCfg,&pInit->highInit);
	
	//clear and initialize mixer
	{
		uint32 * pDst = (uint32 *)mDicePtr(DICE_MIXER_BASE+0x800);
		uint16   i;
		
		memset (pDst, 0, 4*MAX_MIX_COEFF);
		for (i=0; i<pInit->nbMixerCoeffs; i++)
		{
			*pDst++ = (uint32)pInit->pMixerCoeffs[i];
		}
		*mDicePtr(DICE_MIXER_BASE+0x8) = 16;
		*mDicePtr(DICE_MIXER_BASE+0x0) = 1;
	}
	eapStndAloneCfg = pInit->stdAloneCfg;

	
	devInfo.nbPersistData = eapGetPersistSize ();
	
	
	if (devInfo.nbPersistData)
	{
		//malloc some space for the settings.		
		devInfo.pPersistData = malloc(devInfo.nbPersistData);

		//eapOffsets.appSpaceOffset = EAP_CURR_CFG_OFF+(devInfo.nbPersistData)/4;
		eapOffsets.appSpaceSize = (devInfo.nbPrivatePersistData)/4;
	
		//move initial settings into buffer.		
		eapCopyCurrToPersist ();
		eapCopyPrivateToPersist ();
		
		//call sps to get flash data
		hResult = spsInitialize ("eap_vol", 2, devInfo.nbPersistData, devInfo.pPersistData, pInit->flashStoreHoldoffMs);

		//update current from buffer
		eapCopyPersistToCurr ();
		eapCopyPersistToPrivate ();
	}
	//now all structures are initialized, need to setup initial stream cfg, routing etc.
	eapConfigureDriver (devInfo.currRateMode);
	if (eapIsRouterEnabled ()) eapConfigureRouter (devInfo.currRateMode);
	return hResult;	
}

HRESULT eapDriverChangeRateMode(DAL_RATE_MODE_ENUM rateMode)
{
	HRESULT		hResult = NO_ERROR;

	//The driver changed rate mode, we expect that the DAL
	//is created and we need to update:
	//  Driver stream configuration
	//  Routing
	devInfo.currRateMode = rateMode;
	eapConfigureDriver (devInfo.currRateMode);
	if (eapIsRouterEnabled ()) eapConfigureRouter (devInfo.currRateMode);
	return hResult;
}



static HRESULT eapRegisterAddressCallbacks(void)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394  eapAddress, eapAddressHi;
	
	eapAddress.High = EAP_BASE_START_HI;  
	eapAddress.Low  = EAP_BASE_START;  

	eapAddressHi.High = EAP_BASE_END_HI;  
	eapAddressHi.Low  = EAP_BASE_END;

	hResult = lalAddressRangeRegisterCB(&eapCoreCallback,eapAddress,eapAddressHi);

	return hResult;
}

HRESULT eapInitialize(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32 chipID;
	
	eapCapabilities.routerCaps = (MAX_ROUTE_ENTRIES)<<EAP_CAP_ROUTER_MAX_SHFT;
	eapCapabilities.mixerCaps = ((DEV_MIXER_TX0)<<EAP_CAP_MIXER_IN_DEV_SHFT) |
								((DEV_MIXER_RX)<<EAP_CAP_MIXER_OUT_DEV_SHFT) |
								(18<<EAP_CAP_MIXER_INPUTS_SHFT) |
								(16<<EAP_CAP_MIXER_OUTPUTS_SHFT);
								
	chipID = *((volatile uint32 *) GPCSR_CHIP_ID);
	if (chipID == 0x0100000f)
		eapCapabilities.generalCaps = (EAP_CHIP_IS_TCD2220 << EAP_CAP_GEN_CHIP_SHFT);
	else if (chipID == 0x0100000e)
		eapCapabilities.generalCaps = (EAP_CHIP_IS_TCD2210 << EAP_CAP_GEN_CHIP_SHFT);
	else
		eapCapabilities.generalCaps = (EAP_CHIP_IS_UNKNOWN << EAP_CAP_GEN_CHIP_SHFT);

	//Create Semaphore
	hResult = TCSemaphoreOpen(&eapCmdSemID, 0);
	if (hResult != NO_ERROR) return hResult;
	
	//Resume the task
	hResult = eapResumeTasks ();
	if (hResult != NO_ERROR) return hResult;

	//register the address range
	hResult = eapRegisterAddressCallbacks();
	if (hResult != NO_ERROR) return hResult;

	
#ifdef _CLI
	eapCliInstallTools();
#endif //_CLI	

	return hResult;
}



/*********************************************************
	Resume tasks for this module
*/	
HRESULT eapResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskResume(eapThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT eapSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskSuspend(eapThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT eapInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskCreate(&eapThreadID, eapThreadName, eapThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}


// Helper functions to enquire on capabilities

BOOL eapIsMixerEnabled (void)
{
	return eapCapabilities.mixerCaps & EAP_CAP_MIXER_EXPOSED;
}

BOOL eapIsMixerWriteable (void)
{
	return eapIsMixerEnabled () & !(eapCapabilities.mixerCaps & EAP_CAP_MIXER_RO);
}

BOOL eapIsMixerStored (void)
{
	return eapIsMixerWriteable () && (eapCapabilities.mixerCaps & EAP_CAP_MIXER_FLS);
}


BOOL eapIsRouterEnabled (void)
{
	return eapCapabilities.routerCaps & EAP_CAP_ROUTER_EXPOSED;
}

BOOL eapIsRouterWriteable (void)
{
	return eapIsRouterEnabled () & !(eapCapabilities.routerCaps & EAP_CAP_ROUTER_RO);
}

BOOL eapIsRouterStored (void)
{
	return eapIsRouterWriteable && (eapCapabilities.routerCaps & EAP_CAP_ROUTER_FLS);
}

BOOL eapAreStreamsConfigurable (void)
{
	return eapCapabilities.generalCaps & EAP_CAP_GEN_STRCFG_EN;
}

BOOL eapIsStrmCfgStored (void)
{
	return eapAreStreamsConfigurable && (eapCapabilities.generalCaps & EAP_CAP_GEN_STRM_CFG_FLS);
}

BOOL eapIsFlashSupported (void)
{
	return eapCapabilities.generalCaps & EAP_CAP_GEN_FLASH_EN;
}

BOOL eapIsAutoStoreSupported (void)
{
	return eapCapabilities.generalCaps & EAP_CAP_GEN_AUTO_STORE_EN;
}

BOOL eapIsPeakSupported (void)
{
	return eapCapabilities.generalCaps & EAP_CAP_GEN_PEAK_EN;
}

uint8 eapMaxTxStreams (void)
{
	return (uint8)((eapCapabilities.generalCaps & EAP_CAP_GEN_MAX_TX_MSK) >> EAP_CAP_GEN_MAX_TX_SHFT);
}

uint8 eapMaxRxStreams (void)
{
	return (uint8)((eapCapabilities.generalCaps & EAP_CAP_GEN_MAX_RX_MSK) >> EAP_CAP_GEN_MAX_RX_SHFT);
}


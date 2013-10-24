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
 * DESCRIPTION: CMS

	Purpose:	Enhanced Application Protocol
				

	Revisions:
		created 20/02/2007 ml

****************************************************************************/

#ifndef _CMSAVSONLY

#include <string.h>
#include <stdlib.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "lal.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "TCDebug.h"
#include "diceDefs.h"
#include "avsMidi.h"
#include "sps.h"
#include "cms.h"
#include "cmsAvs.h"
#include "cmsInterface.h"
#include "cmsCli.h"


static char 	moduleName[] = __FILE__ ;

#if CMSAVS_ISOC_DISABLE!=CMS_ISOC_DISABLE
#error "INCOMAPTIBLE DEFINES"
#endif

#define CMS_MAX_AUDIO_DST 64
#define CMS_MAX_COMM_DST  2
#define CMS_MAX_APP_DATA  128 //Quadlets

// Memory allocations for spaces

#define CMS_CAPABILITY_SZ		(sizeof(CMS_CAPABILITIES)/4)
#define CMS_GLOBAL_PAR_SZ		(sizeof(CMS_GLOBAL_PAR)/4)
#define CMS_TX_PAR_SZ			(NUM_AVS_TX*sizeof(CMS_TX_PAR)/4)
#define CMS_RX_PAR_SZ			(NUM_AVS_RX*sizeof(CMS_RX_PAR)/4)
#define CMS_RX_ROUTE_SZ			(CMS_MAX_AUDIO_DST*sizeof(uint32)/4)
#define CMS_RX_COMM_SZ			(CMS_MAX_COMM_DST*sizeof(uint32)/4)
#define CMS_GENERAL_STAT_SZ		(sizeof(CMS_GENERAL_STAT)/4)
#define CMS_TX_STAT_SZ			(NUM_AVS_TX*sizeof(CMS_TX_STAT)/4)
#define CMS_RX_STAT_SZ			(NUM_AVS_RX*sizeof(CMS_RX_STAT)/4)
#define CMS_XML_SZ				(sizeof(CMS_XML)/4)
#define CMS_MAX_APP_SZ			(CMS_MAX_APP_DATA)

#define CMS_CAPABILITY_OFF		(sizeof(CMS_OFFSET_STRUCT)/4)
#define CMS_GLOBAL_PAR_OFF		(CMS_CAPABILITY_OFF+CMS_CAPABILITY_SZ)
#define CMS_TX_PAR_OFF			(CMS_GLOBAL_PAR_OFF+CMS_GLOBAL_PAR_SZ)
#define CMS_RX_PAR_OFF			(CMS_TX_PAR_OFF+CMS_TX_PAR_SZ)
#define CMS_RX_ROUTE_OFF		(CMS_RX_PAR_OFF+CMS_RX_PAR_SZ)
#define CMS_RX_COMM_OFF			(CMS_RX_ROUTE_OFF+CMS_RX_ROUTE_SZ)
#define CMS_GENERAL_STAT_OFF	(CMS_RX_COMM_OFF+CMS_RX_COMM_SZ)
#define CMS_TX_STAT_OFF			(CMS_GENERAL_STAT_OFF+CMS_GENERAL_STAT_SZ)
#define CMS_RX_STAT_OFF			(CMS_TX_STAT_OFF+CMS_TX_STAT_SZ)
#define CMS_XML_OFF				(CMS_RX_STAT_OFF+CMS_RX_STAT_SZ)
#define CMS_APP_SPACE_OFF		(CMS_XML_OFF+CMS_XML_SZ)
#define CMS_SPACE_OFF			(CMS_APP_SPACE_OFF+CMS_MAX_APP_SZ)
#define CMS_SPACE_SIZE			(CMS_APP_SPACE_OFF+CMS_MAX_APP_SZ)


#define CMS_BASE_END_HI			0xffff
#define CMS_BASE_END			(CMS_BASE_START + 4*CMS_SPACE_SIZE)


static CMS_OFFSET_STRUCT cmsOffsets =
{
	CMS_CAPABILITY_OFF		,CMS_CAPABILITY_SZ	,
	CMS_GLOBAL_PAR_OFF		,CMS_GLOBAL_PAR_SZ	,
	CMS_TX_PAR_OFF			,CMS_TX_PAR_SZ		,
	CMS_RX_PAR_OFF			,CMS_RX_PAR_SZ		,
	CMS_RX_ROUTE_OFF		,CMS_RX_ROUTE_SZ	,	
	CMS_RX_COMM_OFF			,CMS_RX_COMM_SZ		,
	CMS_GENERAL_STAT_OFF	,CMS_GENERAL_STAT_SZ,	
	CMS_TX_STAT_OFF			,CMS_TX_STAT_SZ		,
	CMS_RX_STAT_OFF			,CMS_RX_STAT_SZ		,
	CMS_XML_OFF				,CMS_XML_SZ			,
	CMS_APP_SPACE_OFF		,CMS_MAX_APP_SZ		,
	0
};

//read only space
static CMS_CAPABILITIES cmsCapabilities;

//config shadow spaces
typedef union
{
	CMS_GLOBAL_PAR	global;
	CMS_TX_PAR		txPar;
	CMS_RX_PAR		rxPar;
}CMS_SHADOW;

static CMS_SHADOW cmsShadow;

//read only spaces
static CMS_GENERAL_STAT cmsGeneralStat;
static CMS_TX_STAT		cmsTxStat[NUM_AVS_TX];
static CMS_RX_STAT		cmsRxStat[NUM_AVS_RX];
static CMS_XML			cmsXML;



//exported functions to set and change the state
typedef struct
{
	CMS_GLOBAL_PAR	global;
	CMS_TX_PAR		txPars[NUM_AVS_TX];
	CMS_RX_PAR		rxPars[NUM_AVS_RX];
	uint32			rxRoute[CMS_MAX_AUDIO_DST];
	uint32			rxComm[CMS_MAX_COMM_DST];
} CMS_CMS_CURR_SET;

typedef struct
{
	CMS_CMS_CURR_SET cms;
	uint32			 appData[CMS_MAX_APP_DATA];
}CMS_CURR_SET;

//The persistens data is a combination of CMS_CURR_SET followed by pConfiguration->nbPrivatePersistData
	
static CMS_CURR_SET	cmsCurSetting;
static CMS_INIT_DESC const * pConfiguration;


static void notifyApp (CMS_EVT_ENUM event, uint32 data)
{
	if (pConfiguration->appCB) pConfiguration->appCB(event,data);
}


static uint8 idxFromMode (DAL_RATE_MODE_ENUM rate_mode)
{
	switch (rate_mode)
	{
		default:
		case eDAL_RATE_MODE_LOW: return 0;
		case eDAL_RATE_MODE_LOW_MID: 
		case eDAL_RATE_MODE_MID: return 1;
		case eDAL_RATE_MODE_HIGH: return 2;
	}
}

static DAL_CLOCK_SOURCE_ENUM clksrcFromSync (uint32 syncSource)
{
	if (syncSource >=CMS_SYNC_SRC_SYT)
	{ //syt sources
		syncSource -=CMS_SYNC_SRC_SYT;
		if (syncSource >= NUM_AVS_RX)
		{
			//error, set to first
			syncSource = 0;
		}
		return eDAL_CLK_SRC_AVS_SYT1+syncSource;
	}
	else if (syncSource >=CMS_SYNC_SRC_EXT)
	{ //some external source
		syncSource -=CMS_SYNC_SRC_EXT;
		if (syncSource > eDAL_CLK_SRC_WORD)
		{
			//error, set to word
			syncSource = eDAL_CLK_SRC_WORD;
		}
		return syncSource;
	}
	else
	{//internal rates
		return eDAL_CLK_SRC_INTERNAL;
	}
}

static DAL_NOMINAL_RATE_ENUM clkrateFromSync (uint32 syncSource)
{
	if (syncSource >=CMS_SYNC_SRC_EXT)
	{ //syt or ext
		return eDAL_NOMINAL_RATE_ANY;
	}
	else
	{//internal rates
		if (syncSource > eDAL_NOMINAL_RATE_192)
			syncSource = eDAL_NOMINAL_RATE_48;
		return syncSource;
	}
}
		
#define   ROUTER_DST(dev,ch,type)   DSTROUTE(DEV_DESCR((dev)) | CH_DESCR((ch)) | TYPE_DESCR((type)))

static void updateRouterTxChannels (uint8 txID)
{
	uint8 txAudioCfg, txCommCfg,ch,i;
	uint8 idxMode = idxFromMode (cmsCurSetting.cms.global.rateMode);
	CMS_AUDIO_TX_CFG const * pAudioCfg;
	
	txAudioCfg = cmsCurSetting.cms.txPars[txID].audioCfg;
	txCommCfg = cmsCurSetting.cms.txPars[txID].commCfg;
	if (txAudioCfg >= pConfiguration->txCfg[idxMode]->audioModes) txAudioCfg = 0;
	if (txCommCfg >= pConfiguration->txCfg[idxMode]->commModes) txCommCfg = 0;
	pAudioCfg = &pConfiguration->txCfg[idxMode]->audioMode[txAudioCfg];
	
	cmsTxStat[txID].audioChs = pAudioCfg->channels;
	cmsTxStat[txID].commChs = pConfiguration->txCfg[idxMode]->commMode[txCommCfg].ports;
	
	
	for (i=0, ch=0; ch<pAudioCfg->channels;i++)
	{		
		dalSetRoute(eDAL_INTERFACE_1, ROUTER_DST(DEV_AVS_ADI1+txID,ch,pAudioCfg->srcList[i].bit_fields.type), pAudioCfg->srcList[i]);
		ch+=1<<pAudioCfg->srcList[i].bit_fields.type;
	}
	//ML zzz need to route comm channel
}

static void updateAvsRx (uint8 rxID)
{
	uint8 i=0;
	
	avsStopRx(rxID);
	for (i=0; i<cmsCurSetting.cms.rxPars[rxID].audioChs; i++) avsSetRxQSelSlot(rxID,i,i+1);	
	if (cmsCurSetting.cms.rxPars[rxID].commChs)
	{
		avsSetRxQSelSlot(rxID,i,i+1);
		i++;
	}
	while (i<QSEL_SLOTS)
		avsSetRxQSelSlot(rxID,i++,0);
	
	avsSetRxDBS(rxID,cmsCurSetting.cms.rxPars[rxID].audioChs);
	avsSetMidi (rxID,0,cmsCurSetting.cms.rxPars[rxID].commChs?1:0);
	if (cmsCurSetting.cms.rxPars[rxID].isocCh != CMS_ISOC_DISABLE)
	{
		avsSetRxCh(rxID,cmsCurSetting.cms.rxPars[rxID].isocCh);
		avsStartRx(rxID);
	}
}

HRESULT cmsSetRx (uint8 rxID, CMS_RX_PAR * pCfg)
{
	if (rxID >= NUM_AVS_RX)
	{
		sysLogError(E_GEN_ILLEGAL_PARAM, __LINE__, moduleName);
		return E_GEN_ILLEGAL_PARAM;
	}
	if ((pCfg->audioChs > 16) || (pCfg->commChs > 8) || ((pCfg->isocCh > 62) && (pCfg->isocCh != CMS_ISOC_DISABLE)))
	{
		sysLogError(E_GEN_ILLEGAL_PARAM, __LINE__, moduleName);
		return E_GEN_ILLEGAL_PARAM;		
	}
	if (memcmp(pCfg,&cmsCurSetting.cms.rxPars[rxID], sizeof(cmsCurSetting.cms.rxPars[rxID]))!=0)
	{
		cmsCurSetting.cms.rxPars[rxID] = *pCfg;
		updateAvsRx (rxID);
		spsChanged ();		
	}
	return NO_ERROR;
}

HRESULT cmsSetTx (uint8 txID, CMS_TX_PAR * pCfg)
{
	if (txID >= NUM_AVS_TX)
	{
		sysLogError(E_GEN_ILLEGAL_PARAM, __LINE__, moduleName);
		return E_GEN_ILLEGAL_PARAM;
	}
	if ((pCfg->isocCh > 62) && (pCfg->isocCh != CMS_ISOC_DISABLE))
	{
		sysLogError(E_GEN_ILLEGAL_PARAM, __LINE__, moduleName);
		return E_GEN_ILLEGAL_PARAM;		
	}
	
	if (memcmp(pCfg,&cmsCurSetting.cms.txPars[txID],sizeof(cmsCurSetting.cms.txPars[txID])))
	{
		if ((cmsCurSetting.cms.txPars[txID].audioCfg != pCfg->audioCfg) || (cmsCurSetting.cms.txPars[txID].commCfg != pCfg->commCfg))
		{
			cmsCurSetting.cms.txPars[txID] = *pCfg;
			updateRouterTxChannels(txID);			
		}
		cmsCurSetting.cms.txPars[txID] = *pCfg;
		cmsAvsSetIsocConfig (txID, cmsCurSetting.cms.txPars[txID].isocCh, cmsTxStat[txID].audioChs, cmsTxStat[txID].commChs, SPEED_400);//ML:should be configable
		spsChanged ();
	}
	return NO_ERROR;
}

#define DEST(dev,ch) DSTROUTE(DEV_DESCR((dev)) | CH_DESCR((ch)) | TYPE_DESCR(BLOCK_1_TYPE))

static void cmsUpdateRateMode (void)
{
	uint8 txID,ch;
	uint8 idxMode = idxFromMode (cmsCurSetting.cms.global.rateMode);
	HRESULT hRes;

	//close down all Isoc Tx;
	for (txID=0; txID<NUM_AVS_TX; txID++) cmsAvsSetIsocConfig (txID, CMS_ISOC_DISABLE, 0, 0, 0);
	hRes = dalCreateInterface(eDAL_INTERFACE_1, cmsCurSetting.cms.global.rateMode, pConfiguration->inDevices[idxMode], pConfiguration->outDevices[idxMode]);
	if (hRes != NO_ERROR) return;
	
	dalSetClockSource (eDAL_INTERFACE_1, clksrcFromSync(cmsCurSetting.cms.global.syncSource), clkrateFromSync(cmsCurSetting.cms.global.syncSource));
	
	//update the Tx routing
	for (txID=0; txID<NUM_AVS_TX; txID++)
	{
		updateRouterTxChannels (txID);
	}
	//update rx routing
	SRC_DESCRIPTION src;
	src.bit_fields.type = BLOCK_1_TYPE;
	cmsCapabilities.nbOutputs = pConfiguration->dstCfg[idxMode]->outputs;	
	for (ch=0; ch<pConfiguration->dstCfg[idxMode]->outputs; ch++)
	{
		src.bit_fields.channel = cmsCurSetting.cms.rxRoute[ch] & 0xf;
		src.bit_fields.moduleId = (cmsCurSetting.cms.rxRoute[ch]>>4) & 0xf;
		uint8 dst = pConfiguration->dstCfg[idxMode]->dstList[ch];
		dalSetRoute(eDAL_INTERFACE_1, DEST(dst>>4,dst&0x0f),src);	
	}
	dalStartInterface(eDAL_INTERFACE_1);
	
	//update the Tx Isoc 
	for (txID=0; txID<NUM_AVS_TX; txID++)
	{
		updateRouterTxChannels(txID);
		cmsAvsSetIsocConfig (txID, cmsCurSetting.cms.txPars[txID].isocCh, cmsTxStat[txID].audioChs, cmsTxStat[txID].commChs, SPEED_400);//ML:should be configable
	}
	notifyApp (eCMS_EVT_RATEMODE_CHG, cmsCurSetting.cms.global.rateMode);
	return;
}

static BOOL isRateModeValid (DAL_RATE_MODE_ENUM rateMode)
{
	uint32 cap = ((pConfiguration->capabilities & CMS_CAP_MAXRATE_MSK) >> CMS_CAP_MAXRATE_SHFT);
	if (cap == CMS_CAP_MAXRATE192K) return TRUE;
	if ((cap == CMS_CAP_MAXRATE96K) && (rateMode > eDAL_RATE_MODE_MID)) return FALSE;
	if ((cap == CMS_CAP_MAXRATE48K) && (rateMode > eDAL_RATE_MODE_LOW)) return FALSE;
	return TRUE;
}

	
HRESULT cmsSetGlobal (CMS_GLOBAL_PAR * pCfg)
{
	BOOL bStore = FALSE;
	if (!isRateModeValid(pCfg->rateMode))
	{
		sysLogError(E_GEN_ILLEGAL_PARAM, __LINE__, moduleName);
		return E_GEN_ILLEGAL_PARAM;
	}
	if (cmsCurSetting.cms.global.rateMode != pCfg->rateMode)
	{
		cmsCurSetting.cms.global.rateMode = pCfg->rateMode;
		cmsCurSetting.cms.global.syncSource = pCfg->syncSource;
		cmsUpdateRateMode ();
		bStore = TRUE;
	}
	else if (cmsCurSetting.cms.global.syncSource != pCfg->syncSource)
	{
		cmsCurSetting.cms.global.syncSource = pCfg->syncSource;
		dalSetClockSource (eDAL_INTERFACE_1, clksrcFromSync(cmsCurSetting.cms.global.syncSource), clkrateFromSync(cmsCurSetting.cms.global.syncSource));
		bStore = TRUE;
	}
	
	if (strncmp (cmsCurSetting.cms.global.nickname,pCfg->nickname,CMS_NICKNAME_MAX) != 0)
	{
		strncpy (cmsCurSetting.cms.global.nickname,pCfg->nickname,CMS_NICKNAME_MAX);	
		bStore = TRUE;
	}
	//ML zzzz uart initialization goes here
	if (pCfg->indicate)
		notifyApp (eCMS_EVT_INDICATE, pCfg->indicate);
	
	if (bStore) spsChanged ();
	return NO_ERROR;
}

HRESULT cmsSetRateMode (uint32 rateMode)
{
	if (!isRateModeValid(rateMode))
	{
		sysLogError(E_GEN_ILLEGAL_PARAM, __LINE__, moduleName);
		return E_GEN_ILLEGAL_PARAM;
	}
	if (cmsCurSetting.cms.global.rateMode != rateMode)
	{
		cmsCurSetting.cms.global.rateMode = rateMode;
		cmsUpdateRateMode ();
		spsChanged ();
	}
	return NO_ERROR;
}

HRESULT cmsSetSync (uint32 sync)
{
	if (cmsCurSetting.cms.global.syncSource != sync)
	{
		cmsCurSetting.cms.global.syncSource = sync;
		dalSetClockSource (eDAL_INTERFACE_1, clksrcFromSync(cmsCurSetting.cms.global.syncSource), clkrateFromSync(cmsCurSetting.cms.global.syncSource));
		spsChanged ();
	}
	return NO_ERROR;
}

HRESULT cmsSetNickName (char * nickName)
{
	if (strncmp (cmsCurSetting.cms.global.nickname,nickName,CMS_NICKNAME_MAX) != 0)
	{
		strncpy (cmsCurSetting.cms.global.nickname,nickName,CMS_NICKNAME_MAX);
		spsChanged ();
	}
	return NO_ERROR;
}

HRESULT cmsIndicate (uint32 indicate)
{
	notifyApp (eCMS_EVT_INDICATE, indicate);
	return NO_ERROR;
}


HRESULT setOutputRoute (uint32 * pRoutes, uint8 from, uint8 to)
{
	uint8 idxMode = idxFromMode (cmsCurSetting.cms.global.rateMode);
	uint8 ch,i;
	SRC_DESCRIPTION src;
	BOOL bStore = FALSE;
	
	src.bit_fields.type = BLOCK_1_TYPE;
	if (to > pConfiguration->dstCfg[idxMode]->outputs) to = pConfiguration->dstCfg[idxMode]->outputs;
	for (ch=from, i=0; ch<to; ch++,i++)
	{
		if (cmsCurSetting.cms.rxRoute[ch] != pRoutes[i])
		{
			cmsCurSetting.cms.rxRoute[ch] = pRoutes[i];
			src.bit_fields.channel = cmsCurSetting.cms.rxRoute[ch] & 0xf;
			src.bit_fields.moduleId = (cmsCurSetting.cms.rxRoute[ch]>>4) & 0xf;
			uint8 dst = pConfiguration->dstCfg[idxMode]->dstList[ch];
			dalSetRoute(eDAL_INTERFACE_1, DEST(dst>>4,dst&0x0f),src);	
			bStore = TRUE;	
		}
	}
	if (bStore) spsChanged ();
	return NO_ERROR;
}

static uint32 getActualPayload (uint32 payloadSize, PB_PACKETTYPE packetType)
{
	if ((packetType == PB_TYPE_READ_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST_QUADLET)) return 4;
	return payloadSize;
}

static BOOL isReadRequest (PB_PACKETTYPE packetType)
{
	return ((packetType == PB_TYPE_READ_REQUEST) || (packetType == PB_TYPE_READ_REQUEST_QUADLET));
}

static BOOL isWriteRequest (PB_PACKETTYPE packetType)
{
	return ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST));
}

//Generic helper for simple rw operations

static HRESULT cmsHandleSimpleRO (PB *packetBlock, uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType,void * pArea, uint32 areaSize)
{



	if (!isReadRequest (packetType))
	{
		return lhlReplyErrorResponse(packetBlock, RSP_TYPE_ERROR, TRUE);
	}
	
	payloadSize = getActualPayload (payloadSize, packetType);
	if (regionOffSet + payloadSize > areaSize)
	{
		return lhlReplyErrorResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
	}
	return lalReplyReadResponse (packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)pArea + (int)regionOffSet), TRUE);		
}


static HRESULT cmsHandleSimpleRW (PB *packetBlock, uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType,void * pArea, uint32 areaSize, void * pAreaWrite, BOOL * pWritten)
{
	HRESULT hResult;
	QUADLET			*pPayload = NULL;

	if (pWritten) *pWritten = false;

	payloadSize = getActualPayload (payloadSize, packetType);
	if (isReadRequest (packetType))
	{
		if (regionOffSet + payloadSize > areaSize)
		{
			return lhlReplyErrorResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
		return lalReplyReadResponse (packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)pArea + (int)regionOffSet), TRUE);		
	
	}
	else if (isWriteRequest (packetType))
	{
		if (regionOffSet + payloadSize > areaSize)
		{
			return lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;
		memcpy((uint32*) ((int)pAreaWrite + (int)regionOffSet), pPayload, payloadSize);
		if (pWritten) *pWritten = true;
		return lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
	}
	else 
	{		
		return lhlReplyErrorResponse(packetBlock, RSP_TYPE_ERROR, TRUE);	
	}
}


static HRESULT cmsHandleAddressError (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
#ifdef _SYSDEBUG	
		sysDebugPrintf("CMS, illegal command! at OffSet = %08X\n\r",regionOffSet);
#endif // _SYSDEBUG
		
		return lhlReplyErrorResponse(packetBlock, RSP_TYPE_ERROR, TRUE);	

}


// Handlers for each space

static HRESULT cmsHandleOffsets		 (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, (void *)&cmsOffsets, sizeof(cmsOffsets));
}

static HRESULT cmsHandleCapability   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, &cmsCapabilities, sizeof(cmsCapabilities));
}

static HRESULT cmsHandleGlobalPar   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	BOOL bWritten;
	cmsShadow.global = cmsCurSetting.cms.global;
	hResult = cmsHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &cmsCurSetting.cms.global, sizeof(cmsCurSetting.cms.global), &cmsShadow.global, &bWritten);
	if (bWritten)
	{
		cmsSetGlobal (&cmsShadow.global);
	}
	return hResult;
}

static HRESULT cmsHandleTxPar   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	BOOL bWritten;
	uint8 txID;
	
	txID = (uint8)(regionOffSet/sizeof(CMS_TX_PAR));
	regionOffSet -= txID*sizeof(CMS_TX_PAR);
	cmsShadow.txPar = cmsCurSetting.cms.txPars[txID];
	hResult = cmsHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &cmsCurSetting.cms.txPars[txID], sizeof(cmsCurSetting.cms.txPars[txID]), &cmsShadow.txPar, &bWritten);
	if (bWritten)
	{
		cmsSetTx (txID,&cmsShadow.txPar);
	}
	return hResult;
}

static HRESULT cmsHandleRxPar   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	BOOL bWritten;
	uint8 rxID;

	rxID = (uint8)(regionOffSet/sizeof(CMS_RX_PAR));
	regionOffSet -= rxID*sizeof(CMS_RX_PAR);
	cmsShadow.rxPar = cmsCurSetting.cms.rxPars[rxID];

	hResult = cmsHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, &cmsCurSetting.cms.rxPars[rxID], sizeof(cmsCurSetting.cms.rxPars[rxID]), &cmsShadow.rxPar, &bWritten);
	if (bWritten)
	{
		cmsSetRx (rxID,&cmsShadow.rxPar);
	}
	return hResult;
}

static HRESULT cmsHandleRxRouting   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	QUADLET			*pPayload = NULL;
	
	if (isReadRequest (packetType))
		return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, cmsCurSetting.cms.rxRoute, sizeof(uint32)*CMS_MAX_AUDIO_DST);
	else if (isWriteRequest (packetType))
	{
		payloadSize = getActualPayload(payloadSize,packetType);
		if (regionOffSet + payloadSize > sizeof(uint32)*CMS_MAX_AUDIO_DST)
		{
			return lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;
		
		setOutputRoute ((uint32*)pPayload, (uint8)(regionOffSet/4), (uint8)((regionOffSet+payloadSize)/4));		
		return lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
	}
	else 
	{		
		return lhlReplyErrorResponse(packetBlock, RSP_TYPE_ERROR, TRUE);	
	}
}


static HRESULT cmsHandleRxComm   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,regionOffSet,payloadSize);
	return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, cmsCurSetting.cms.rxComm, sizeof(uint32)*CMS_MAX_COMM_DST);
}

//and now the status spaces

static HRESULT cmsHandleGeneralStat   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//update status
	DAL_CONFIG config;
	uAESRX_ERROR aesStat;
	if (dalGetCurrentConfiguration  (eDAL_INTERFACE_1, &config) != NO_ERROR)
	{		
		cmsGeneralStat.lockInfo = 0;
	}
	else
	{
		cmsGeneralStat.lockInfo = (dalIsLocked (eDAL_INTERFACE_1)?1:0) | (config.rateMode<<8);
	}
	if (!aesIsMasterLocked ())
	{
		cmsGeneralStat.aesLockInfo = 0;
	}
	else
	{
		cmsGeneralStat.aesLockInfo = 1;
		aesGetRxError(0, &aesStat, true);
		cmsGeneralStat.aesLockInfo |= (aesStat.bit.lock?0x100:0) | (aesStat.bit.uo_run?0x1000:0);
		aesGetRxError(1, &aesStat, true);
		cmsGeneralStat.aesLockInfo |= (aesStat.bit.lock?0x200:0) | (aesStat.bit.uo_run?0x2000:0);
		aesGetRxError(2, &aesStat, true);
		cmsGeneralStat.aesLockInfo |= (aesStat.bit.lock?0x400:0) | (aesStat.bit.uo_run?0x4000:0);
		aesGetRxError(3, &aesStat, true);
		cmsGeneralStat.aesLockInfo |= (aesStat.bit.lock?0x800:0) | (aesStat.bit.uo_run?0x8000:0);
	}
#ifdef _DICE2
	cmsGeneralStat.adatLockInfo = adatIsLocked()?1:0;	
#elif defined (_DICEJR)
	cmsGeneralStat.adatLockInfo = adatIsLocked(0)?1:0;	
#endif
	return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, &cmsGeneralStat, sizeof(cmsGeneralStat));
}

static HRESULT cmsHandleTxStat   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//update status
	uint8 txID;
	
	for (txID=0; txID<NUM_AVS_TX; txID++)
	  cmsTxStat[txID].state = cmsAvsGetIsoState (txID);
	
	return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, &cmsTxStat[0], sizeof(CMS_TX_STAT)*NUM_AVS_TX);
}

static HRESULT cmsHandleRxStat   (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	//update status
	uint8 rxID;
	
	for (rxID=0; rxID<NUM_AVS_RX; rxID++)
		cmsRxStat[rxID].state = avsRxIsLocked(rxID)?1:0;
	
	return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, &cmsRxStat[0], sizeof(CMS_RX_STAT)*NUM_AVS_RX);
}


static HRESULT cmsHandleXMLSpace  (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	return cmsHandleSimpleRO (packetBlock, regionOffSet, payloadSize, packetType, &cmsXML, sizeof(cmsXML));
}


static HRESULT cmsHandleAppSpace (PB *packetBlock,uint32 regionOffSet, uint32 payloadSize, PB_PACKETTYPE packetType)
{
	HRESULT hResult;
	BOOL bWritten;

	hResult = cmsHandleSimpleRW (packetBlock, regionOffSet, payloadSize, packetType, cmsCurSetting.appData, pConfiguration->nbPrivatePersistData, cmsCurSetting.appData, &bWritten);
	if (bWritten)
	{
		payloadSize = getActualPayload (payloadSize, packetType);
		notifyApp (eCMS_EVT_PERSIST_HOST_CHG,((regionOffSet&0xffff)<<16) | (payloadSize&0xffff));	
		spsChanged ();
	}
	return hResult;
}



static HRESULT cmsCoreCallback(PB *packetBlock)
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
	
	RegionOffSet = OffSetDest.Low - CMS_BASE_START;	

	hResult = pbGetDataLen(packetBlock,&payloadSize);
	if(hResult != NO_ERROR) 
		return hResult;	

	//SYS_TRACE2(SYSDEBUG_TRACE_GENERAL,RegionOffSet,payloadSize);
	if (RegionOffSet < (CMS_CAPABILITY_OFF*4)) 
	{
		return cmsHandleOffsets (packetBlock,RegionOffSet,payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_GLOBAL_PAR_OFF*4)) 
	{
		return 	 cmsHandleCapability (packetBlock,RegionOffSet-(CMS_CAPABILITY_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_TX_PAR_OFF*4)) 
	{
		return 		 cmsHandleGlobalPar (packetBlock,RegionOffSet-(CMS_GLOBAL_PAR_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_RX_PAR_OFF*4)) 
	{
		return 		 cmsHandleTxPar (packetBlock,RegionOffSet-(CMS_TX_PAR_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_RX_ROUTE_OFF*4)) 
	{
		return 	 cmsHandleRxPar (packetBlock,RegionOffSet-(CMS_RX_PAR_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_RX_COMM_OFF*4)) 
	{
		return 		 cmsHandleRxRouting (packetBlock,RegionOffSet-(CMS_RX_ROUTE_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_GENERAL_STAT_OFF*4)) 
	{
		return  cmsHandleRxComm (packetBlock,RegionOffSet-(CMS_RX_COMM_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_TX_STAT_OFF*4)) 
	{
		return 		 cmsHandleGeneralStat (packetBlock,RegionOffSet-(CMS_GENERAL_STAT_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_RX_STAT_OFF*4)) 
	{
		return 		 cmsHandleTxStat (packetBlock,RegionOffSet-(CMS_TX_STAT_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_XML_OFF*4)) 
	{
		return 			 cmsHandleRxStat (packetBlock,RegionOffSet-(CMS_RX_STAT_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_APP_SPACE_OFF*4)) 
	{
		return 	 cmsHandleXMLSpace (packetBlock,RegionOffSet-(CMS_XML_OFF*4),payloadSize,packetType);
	}
	else if (RegionOffSet < (CMS_SPACE_OFF*4)) 
	{
		return 		 cmsHandleAppSpace (packetBlock,RegionOffSet-(CMS_APP_SPACE_OFF*4),payloadSize,packetType);
	}
	else
	{
		return cmsHandleAddressError (packetBlock,RegionOffSet,payloadSize,packetType);
	}
}

	
	
HRESULT cmsInitializeInterface(const CMS_INIT_DESC * pInit)
{
	HRESULT	hResult = NO_ERROR;
	int i;
	
	pConfiguration = pInit;	
	
	cmsCapabilities.generalCaps = pInit->capabilities;
	cmsCapabilities.nbComms = 1;
	//cmsCapabilities.nbOutputs - initialized by update rate mode
	cmsCapabilities.nbRx = NUM_AVS_RX;
	cmsCapabilities.nbTx = NUM_AVS_TX;
	cmsCapabilities.specVersion = CMS_SPEC_VERSION+1;
	
	//initialize curr setting struct from flash
	
	//ML: zzzz if (pInit->nbPrivatePersistData too large error
	memcpy (cmsCurSetting.appData, pInit->pPrivatePersistData, pInit->nbPrivatePersistData);
	
	cmsCurSetting.cms.global.com0Setup = (CMS_COMM_BAUD_115K2<<CMS_COMM_BAUD_SHFT) | (CMS_COMM_PARITY_NONE<<CMS_COMM_PARITY_SHFT) |
	                                 (CMS_COMM_BPC_8<<CMS_COMM_BPC_SHFT) | CMS_COMM_ENABLE;
	cmsCurSetting.cms.global.com1Setup = (CMS_COMM_BAUD_115K2<<CMS_COMM_BAUD_SHFT) | (CMS_COMM_PARITY_NONE<<CMS_COMM_PARITY_SHFT) |
	                                 (CMS_COMM_BPC_8<<CMS_COMM_BPC_SHFT) | CMS_COMM_ENABLE;
	strncpy (cmsCurSetting.cms.global.nickname,pInit->defNickName,CMS_NICKNAME_MAX);
	cmsCurSetting.cms.global.rateMode = CMS_RATE_32K_48K;
	cmsCurSetting.cms.global.syncSource = CMS_SYNC_SRC_INT48K;
	for (i=0; i<NUM_AVS_RX; i++) cmsCurSetting.cms.rxPars[i].isocCh = CMS_ISOC_DISABLE;
	for (i=0; i<NUM_AVS_TX; i++) cmsCurSetting.cms.txPars[i].isocCh = CMS_ISOC_DISABLE;
	for (i=0; i<CMS_MAX_AUDIO_DST; i++)
		cmsCurSetting.cms.rxRoute[i] = 0xf0; //muted source
	
	//call sps to get flash data
	hResult = spsInitialize ("cms_vol", 2, sizeof(cmsCurSetting.cms)+pInit->nbPrivatePersistData, &cmsCurSetting, 15000);

	//now we should do some sanity check of flash data
		
	//Then update everything accordingly
	cmsUpdateRateMode();
	for (i=0; i<NUM_AVS_RX; i++) updateAvsRx (i);
	
	//let's create the CMS info structure
	if (!pInit->pCmsInfo)
	{
		cmsXML.OUI = 0;
		cmsXML.PID = 0;
		strcpy(cmsXML.xmlFile,"cmsDefaultDevice.xml");
	}
	else
	{
		cmsXML = *pInit->pCmsInfo;
	}
	return hResult;	
}

//status enquiry functions

CMS_RX_STAT * cmsGetRxStat(uint8 rxID)
{
	cmsRxStat[rxID].state = avsRxIsLocked(rxID)?1:0;
	return &cmsRxStat[rxID];
}

CMS_TX_STAT * cmsGetTxStat(uint8 txID)
{
	cmsTxStat[txID].state = cmsAvsGetIsoState (txID);
	return &cmsTxStat[txID];
}


CMS_RX_PAR * cmsGetRxPar(uint8 rxID)
{
	return &cmsCurSetting.cms.rxPars[rxID];
}

CMS_TX_PAR * cmsGetTxPar(uint8 txID)
{
	return &cmsCurSetting.cms.txPars[txID];
}

uint8 cmsGetnbOutputs (void)
{
	uint8 idxMode = idxFromMode (cmsCurSetting.cms.global.rateMode);
	return pConfiguration->dstCfg[idxMode]->outputs;
}

uint8 cmsGetOutputsRoute (uint8 id)
{
	return cmsCurSetting.cms.rxRoute[id];
}

CMS_GLOBAL_PAR * cmsGetGlobalPar (void)
{
	return &cmsCurSetting.cms.global;
}

uint8 cmsGetnbModes (void)
{
	uint8 idxMode = idxFromMode (cmsCurSetting.cms.global.rateMode);
	return pConfiguration->txCfg[idxMode]->audioModes;
}


const char * cmsGetAudioModeName (uint8 mode)
{
	uint8 idxMode = idxFromMode (cmsCurSetting.cms.global.rateMode);

	if (mode >= pConfiguration->txCfg[idxMode]->audioModes) return "Illegal Mode";
	if (pConfiguration->pTextDesc)
		return pConfiguration->pTextDesc->modeNames[idxMode][mode];
	return "";
}

const char * cmsGetAudioOutputName (uint8 id)
{
	uint8 idxMode = idxFromMode (cmsCurSetting.cms.global.rateMode);

	if (id >= pConfiguration->dstCfg[idxMode]->outputs) return "???";
	if (pConfiguration->pTextDesc)
		return pConfiguration->pTextDesc->dstNames[idxMode][id];
	return "";
}

static HRESULT cmsRegisterAddressCallbacks(void)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394  cmsAddress, cmsAddressHi;
	
	cmsAddress.High = CMS_BASE_START_HI;  
	cmsAddress.Low  = CMS_BASE_START;  

	cmsAddressHi.High = CMS_BASE_END_HI;  
	cmsAddressHi.Low  = CMS_BASE_END;

	hResult = lalAddressRangeRegisterCB(&cmsCoreCallback,cmsAddress,cmsAddressHi);

	return hResult;
}

HRESULT cmsInitialize(void)
{
	HRESULT	hResult = NO_ERROR;
	
	//Resume the task
	hResult = cmsAvsInitialize();
	if (hResult != NO_ERROR) return hResult;

	//register the address range
	hResult = cmsRegisterAddressCallbacks();
	if (hResult != NO_ERROR) return hResult;

	
#ifdef _CLI
	cmsCliInstallTools();
#endif //_CLI	

	return hResult;
}



/*********************************************************
	Resume tasks for this module
*/	
HRESULT cmsResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = cmsAvsResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT cmsSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = cmsAvsSuspendTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT cmsInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = cmsAvsInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#endif // _CMSAVSONLY


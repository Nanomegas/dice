//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies  Ltd.(ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd. 
//
//===========================================================================

/****************************************************************************
 * DESCRIPTION: dalRemote.c

	Purpose:	Remote DAL for direct PC control.
				
	Revisions:
		created 05/02/2005 zo

****************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "TCDebug.h"
#include "lal.h"
#include "dal.h"
#include "targetboard.h"
#include "axm20.h"
#include "dalRemote.h"
#include "dalRemoteInterface.h"
#include "myApp.h"

#ifdef _DALREMOTE

// task (thread) ID for this module
uint32 uiRDALThreadID;
const char* RDALThreadName = "RDALThread";
uint32 uiRDALSemID;

static RDAL_1394MEMORY RDALMemory;

#define RDAL_OFFSET(a) ((uint32)(&RDALMemory.a) - (uint32)&RDALMemory)

// Below for debug output:
// static char moduleName[] = __FILE__ ;

//
// RDALCheckForWrite
//
static bool RDALCheckForWrite(uint32 RegionOffSet, uint32 payload)
{
	bool	Result = false;
	
	// sysDebugPrintf("RDALCheckForWrite()\n");

	if((RegionOffSet == RDAL_OFFSET(opcode)) && (payload == 4))
		Result = true;
	else if ((RegionOffSet >= RDAL_OFFSET(data)) && ((RegionOffSet + payload) <= RDAL_OFFSET(endOfData)))
		Result = true;
	else
		Result = false;
#ifdef _SYSDEBUG	
	if (Result == false) sysDebugPrintf("frmwLoad, illegal write command!\n\r");
#endif // _SYSDEBUG
	return Result;
}
//
// RDAL_LinkCallback
//
static HRESULT RDAL_LinkCallback(PB *packetBlock)
{
	HRESULT hResult = NO_ERROR;
	PB_HEADER *pHeader = NULL;
	PB_PACKETTYPE packetType = PB_TYPE_UNDEF;
	OFFSET_1394 OffSetDest;
	uint32 payloadSize = 0;
	uint32 RegionOffSet = 0;	
	QUADLET *pPayload = NULL;
	
	// sysDebugPrintf("RDAL_LinkCallback()\n");
	
	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketType(packetBlock,&packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetDestinationOffset(packetBlock, &OffSetDest);
	if (hResult != NO_ERROR) return hResult;

	RegionOffSet = OffSetDest.Low - RDAL_BASE_START;	

	hResult = pbGetDataLen(packetBlock,&payloadSize);
	if(hResult != NO_ERROR) 
		return hResult;	

	if (packetType == PB_TYPE_WRITE_REQUEST_QUADLET)
	{
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		payloadSize = 4;

		if (RDALCheckForWrite(RegionOffSet, payloadSize))
		{
			if (RegionOffSet == RDAL_OFFSET(opcode))
			{
				memcpy((uint32*) ((int)&RDALMemory + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				TCSemaphoreSignal(uiRDALSemID);
			}
			else
			{
				memcpy((uint32*) ((int)&RDALMemory + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);				
			}			
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);			
		}		
	}
	else if (packetType == PB_TYPE_WRITE_REQUEST)
	{
		hResult = pbGetPayload(packetBlock, (void **) &pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;

		if(RDALCheckForWrite(RegionOffSet, payloadSize))
		{			
			memcpy((uint32*) ((int)&RDALMemory + (int)RegionOffSet), pPayload, payloadSize);
			hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
	}
	else if (packetType == PB_TYPE_READ_REQUEST)
	{
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;

		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&RDALMemory + (int)RegionOffSet), TRUE);
	}
	else if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
	{
		payloadSize = 4;
		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&RDALMemory + (int)RegionOffSet), TRUE);
	}
	else
	{
		hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
	}
	return hResult;
}
//
// RDALCreateInterface
//
static HRESULT RDALCreateInterface(RDAL_CREATE_INTERFACE *pCreateInterface)
{
	HRESULT hr = NO_ERROR;
	
	hr = dalCreateInterface(pCreateInterface->uiInterfaceId, 
		pCreateInterface->uiRateMode,
		pCreateInterface->uiInputDevices,
		pCreateInterface->uiOutputDevices);

	return hr;
}
//
// RDALStartInterface
//
static HRESULT RDALStartInterface(RDAL_START_INTERFACE *pStartInterface)
{
	HRESULT hr = NO_ERROR;
	
	hr = dalStartInterface(pStartInterface->uiInterfaceId);
	
	return hr;
}
//
// RDALSetClockSource
//
static HRESULT RDALSetClockSource(RDAL_SET_CLOCK_SOURCE *pSetClockSource)
{
	HRESULT hr = NO_ERROR;
	
	hr = dalSetClockSource(pSetClockSource->uiInterfaceId,
		pSetClockSource->uiClockSource,
		pSetClockSource->uiNominalRate);
	
	return hr;
}
//
// RDALSetRoute
//
static HRESULT RDALSetRoute(RDAL_SET_ROUTE *pSetRoute)
{
	HRESULT hr = NO_ERROR;
	
	/*
	DEST_DESCRIPTION DestRoute;
	SRC_DESCRIPTION SrcRoute;
	
	DestRoute.entry = pSetRoute->ui16Destination;
	SrcRoute.entry = pSetRoute->ui16Source;
	*/
		
	hr = dalSetRoute(pSetRoute->uiInterfaceId,
		pSetRoute->DestRoute,
		pSetRoute->SrcRoute);
	
	return hr;
}
//
// RDALGetCurrentStatus
//
static HRESULT RDALGetCurrentStatus(RDAL_GET_CURRENT_STATUS *pGetCurrentStatus)
{
	HRESULT hr = NO_ERROR;
	
	// DAL_STATUS DalStatus = {0};
	
	hr = dalGetCurrentStatus(pGetCurrentStatus->uiInterfaceId, &pGetCurrentStatus->DalStatus);
	
	return hr;
}
//
// RDALIsLocked
//
static HRESULT RDALIsLocked(RDAL_IS_LOCKED *pIsLocked)
{
	HRESULT hr = NO_ERROR;
	
	hr = dalIsLocked(pIsLocked->uiInterfaceId);
	
	return hr;
}
//
// RDALGetCurrentConfig
//
static HRESULT RDALGetCurrentConfig(RDAL_GET_CURRENT_CONFIGURATION *pGetCurrentConfig)
{
	HRESULT hr = NO_ERROR;
	
	// DAL_CONFIG DalConfig = {0};
	
	hr = dalGetCurrentConfiguration(pGetCurrentConfig->uiInterfaceId, &pGetCurrentConfig->DalConfig);
	
	return hr;
}
//
// RDALCallBack
//
static HRESULT RDALCallBack (DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data)
{
	HRESULT hr = NO_ERROR;
	
	return hr;
}
//
// RDALInstallCallback
//
static HRESULT RDALInstallCallback(RDAL_INSTALL_CALLBACK *pInstallCallback)
{
	HRESULT hr = NO_ERROR;
	
	hr = dalInstallCallback(&pInstallCallback->DalEvents,
		pInstallCallback->uiHoldoffMS,
		RDALCallBack,
		0);
	
	return hr;
}
//
// RDALUnInstallCallback
//
static HRESULT RDALUnInstallCallback(void)
{
	HRESULT hr = NO_ERROR;
	
	hr = dalUninstallCallback(RDALCallBack);
	
	return hr;
}
//
// RDALGlobalMute
//
static HRESULT RDALGlobalMute(RDAL_GLOBAL_MUTE *pGlobalMute)
{
	HRESULT hr = NO_ERROR;
	
	hr = dalGlobalMute(pGlobalMute->uiInterfaceId,
		pGlobalMute->uiMute);
	
	return hr;
}
//
// RDALToggleLED
//
static HRESULT RDALToggleLED(RDAL_TOGGLE_LED *pToggleLED)
{
	HRESULT hr = NO_ERROR;
	
	if (pToggleLED->uiOn)
	{
		LED_ON(pToggleLED->uiLED_ID);
	}
	else
	{
		LED_OFF(pToggleLED->uiLED_ID);
	}
	
	return hr;
}

static HRESULT RDALGetEVMUI(RDAL_GET_EVM_UI_RETURN *pGetEVMui, int deltaOrAbs)
{
	HRESULT hr = NO_ERROR;

//	hr = EVMGetUI(pGetEVMui, deltaOrAbs);

	return hr;
}

static HRESULT RDALSetOutChannelVol(RDAL_SET_CHANNEL_VOL *pSetChannelVol)
{
	HRESULT hr = NO_ERROR;

	AXM20SetOutChVol(pSetChannelVol->uiChannelId, pSetChannelVol->uiLevel);

	return hr;
}

static HRESULT RDALSetInChannelVol(RDAL_SET_CHANNEL_VOL *pSetChannelVol)
{
	HRESULT hr = NO_ERROR;

	AXM20SetInChVol(pSetChannelVol->uiChannelId, pSetChannelVol->uiLevel);

	return hr;
}

static HRESULT RDALSetInChannelMute(RDAL_SET_CHANNEL_MUTE *pSetChannelMute)
{
	HRESULT hr = NO_ERROR;

	return hr;
}

static HRESULT RDALSetOutChannelMute(RDAL_SET_CHANNEL_MUTE *pSetChannelMute)
{
	HRESULT hr = NO_ERROR;

	AXM20SetOutChMute(pSetChannelMute->uiChannelId, pSetChannelMute->uiMute);

	return hr;
}

static HRESULT RDALGetInChannelVol(int ch, RDAL_GET_CHANNEL_VOL_RETURN *pGetChannelVol)
{
	HRESULT hr = NO_ERROR;
	int vol = 0;
	int muted = 0;

	hr = AXM20GetInChVol(ch, &muted, &vol);
	if (hr == NO_ERROR)
	{
		pGetChannelVol->uiChId = ch;
		pGetChannelVol->uiMuted = (uint32)muted;
		pGetChannelVol->uiVolume = (uint32)vol;
	}

	return hr;
}

static HRESULT RDALGetOutChannelVol(int ch, RDAL_GET_CHANNEL_VOL_RETURN *pGetChannelVol)
{
	HRESULT hr = NO_ERROR;
	int vol = 0;
	int muted = 0;

	hr = AXM20GetOutChVol(ch, &muted, &vol);
	if (hr == NO_ERROR)
	{
		pGetChannelVol->uiChId = ch;
		pGetChannelVol->uiMuted = (uint32)muted;
		pGetChannelVol->uiVolume = (uint32)vol;
	}

	return hr;
}

//
// RDALExecuteOpcode
//
static void RDALExecuteOpcode(void)
{	
	RDAL_OP_ENUM OpCode;
	
//	sysDebugPrintf("RDALExecuteOpcode()\n");
	
	if (RDALMemory.opcode & RDAL_OP_EXECUTE)
	{
		OpCode = RDALMemory.opcode & RDAL_OP_MASK;
		
		switch (OpCode)
		{
		case RDAL_OP_CREATE_INTERFACE:
			sysDebugPrintf("RDALThread::RDAL_OP_CREATE_INTERFACE\n");
			RDALMemory.error = RDALCreateInterface (&RDALMemory.data.parameters.CreateIterface);
			break;
			
		case RDAL_OP_START_INTERFACE:
			sysDebugPrintf("RDALThread::RDAL_OP_START_INTERFACE\n");
			RDALMemory.error = RDALStartInterface (&RDALMemory.data.parameters.StartInterface);
			break;
			
		case RDAL_OP_SET_CLOCK_SOURCE:
			sysDebugPrintf("RDALThread::RDAL_OP_SET_CLOCK_SOURCE\n");
			RDALMemory.error = RDALSetClockSource (&RDALMemory.data.parameters.SetClockSource);
			break;
			
		case RDAL_OP_SET_ROUTE:
			sysDebugPrintf("RDALThread::RDAL_OP_SET_ROUTE\n");
			RDALMemory.error = RDALSetRoute (&RDALMemory.data.parameters.SetRoute);
			break;
			
		case RDAL_OP_GET_CURRENT_STATUS:
			sysDebugPrintf("RDALThread::RDAL_OP_GET_CURRENT_STATUS\n");
			RDALMemory.error = RDALGetCurrentStatus (&RDALMemory.data.parameters.GetCurrentStatus);
			break;
			
		case RDAL_OP_IS_LOCKED:
			sysDebugPrintf("RDALThread::RDAL_OP_IS_LOCKED\n");
			RDALMemory.error = RDALIsLocked(&RDALMemory.data.parameters.IsLocked);
			break;
			
		case RDAL_OP_GET_CURRENT_CONFIG:
			sysDebugPrintf("RDALThread::RDAL_OP_GET_CURRENT_CONFIG\n");
			RDALMemory.error = RDALGetCurrentConfig (&RDALMemory.data.parameters.GetCurrentConfig);
			break;
			
		case RDAL_OP_INSTALL_CALLBACK:
			sysDebugPrintf("RDALThread::RDAL_OP_INSTALL_CALLBACK\n");
			RDALMemory.error = RDALInstallCallback(&RDALMemory.data.parameters.InstallCallback);
			break;
							
		case RDAL_OP_UNINSTALL_CALLBACK:
			sysDebugPrintf("RDALThread::RDAL_OP_UNINSTALL_CALLBACK\n");
			RDALMemory.error = RDALUnInstallCallback();
			break;
			
		case RDAL_OP_GLOBAL_MUTE:
			sysDebugPrintf("RDALThread::RDAL_OP_GLOBAL_MUTE\n");
			RDALMemory.error = RDALGlobalMute(&RDALMemory.data.parameters.GlobalMute);
			break;
			
		case RDAL_OP_TOGGLE_LED:
			sysDebugPrintf("RDALThread::RDAL_OP_TOGGLE_LED\n");
			RDALMemory.error = RDALToggleLED(&RDALMemory.data.parameters.ToggleLED);
			break;
					
		case RDAL_OP_GET_EVM_UI:
//			sysDebugPrintf("RDALThread::RDAL_OP_GET_EVM_UI\n");
			RDALMemory.error = RDALGetEVMUI(&RDALMemory.data.rtn.EVMui, RDALMemory.data.parameters.GetEVMui.uiDeltaOrAbs);
			break;

		case RDAL_OP_SET_OUT_CHANNEL_VOL:
//			sysDebugPrintf("RDALThread::RDAL_OP_SET_OUT_CHANNEL_VOL\n");
			RDALMemory.error = RDALSetOutChannelVol(&RDALMemory.data.parameters.SetChannelVol);
			break;

		case RDAL_OP_SET_IN_CHANNEL_VOL:
//			sysDebugPrintf("RDALThread::RDAL_OP_SET_IN_CHANNEL_VOL\n");
			RDALMemory.error = RDALSetInChannelVol(&RDALMemory.data.parameters.SetChannelVol);
			break;

		case RDAL_OP_SET_OUT_CHANNEL_MUTE:
//			sysDebugPrintf("RDALThread::RDAL_OP_SET_OUT_CHANNEL_MUTE\n");
			RDALMemory.error = RDALSetOutChannelMute(&RDALMemory.data.parameters.SetChannelMute);
			break;

		case RDAL_OP_SET_IN_CHANNEL_MUTE:
			sysDebugPrintf("RDALThread::RDAL_OP_SET_IN_CHANNEL_MUTE\n");
			RDALMemory.error = RDALSetInChannelMute(&RDALMemory.data.parameters.SetChannelMute);
			break;

		case RDAL_OP_GET_IN_CHANNEL_VOL:
//			sysDebugPrintf("RDALThread::RDAL_OP_GET_IN_CHANNEL_VOL\n");
			RDALMemory.error = RDALGetInChannelVol(RDALMemory.data.parameters.GetChannelVol.uiChannelId, &RDALMemory.data.rtn.ChannelVol);
			break;

		case RDAL_OP_GET_OUT_CHANNEL_VOL:
//			sysDebugPrintf("RDALThread::RDAL_OP_GET_OUT_CHANNEL_VOL\n");
			RDALMemory.error = RDALGetOutChannelVol(RDALMemory.data.parameters.GetChannelVol.uiChannelId, &RDALMemory.data.rtn.ChannelVol);
			break;

		case RDAL_OP_NOOP:
			sysDebugPrintf("RDALThread::RDAL_OP_NOOP\n");
			RDALMemory.error = NO_ERROR;
			break;
			
		default:
			sysDebugPrintf("RDALThread::E_GEN_ILLEGAL_PARAM\n");
			RDALMemory.error = E_GEN_ILLEGAL_PARAM;
			break;
		}
		// clear execute bit
		RDALMemory.opcode &= ~RDAL_OP_EXECUTE;			
	}
}
//
// RDALThread
//
static void RDALThread(void *pvDummy)
{
	HRESULT hResult = NO_ERROR;

	UNUSED_ARG(pvDummy);
	
//	sysDebugPrintf("RDALThread()\n");

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(uiRDALSemID);
		
		if (hResult == NO_ERROR) 
		{
			RDALExecuteOpcode();	
		}				
	}
}
//
// RDALRegisterAddressCallbacks
//
static HRESULT RDALRegisterAddressCallbacks(void)
{
	HRESULT hResult = NO_ERROR;
	OFFSET_1394 RDALAddress, RDALAddressHi;
	
//	sysDebugPrintf("RDALRegisterAddressCallbacks()\n");
	
	RDALAddress.High = RDAL_BASE_START_HI;  
	RDALAddress.Low = RDAL_BASE_START;  

	RDALAddressHi.High = RDAL_BASE_END_HI;  
	RDALAddressHi.Low = RDAL_BASE_END;

	hResult = lalAddressRangeRegisterCB(&RDAL_LinkCallback, RDALAddress, RDALAddressHi);

	return hResult;
}
//
// RDALInitialize
//
HRESULT RDALInitialize(void)
{
	HRESULT	hResult = NO_ERROR;

	/*	
	bufferMaxLen = 0x100000;	//max 1M buffer size.
	bufferBase = (uint8 *)malloc(bufferMaxLen);

    if (!bufferBase)
    {
		sysLogError(E_FIS_MEMORY_ERROR, __LINE__, moduleName);
	}
	*/
	
//	sysDebugPrintf("RDALInitialize()\n");
	
	//Create Semaphore
	hResult = TCSemaphoreOpen(&uiRDALSemID, 0);
	if (hResult != NO_ERROR) return hResult;
	
	//Resume the task
	hResult = RDALResumeTasks ();
	if (hResult != NO_ERROR) return hResult;

	//register the address range
	hResult = RDALRegisterAddressCallbacks();
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}
//
// RDALResumeTasks
//
HRESULT RDALResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;
	
//	sysDebugPrintf("RDALResumeTasks()\n");

	hResult = TCTaskResume(uiRDALThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
//
// RDALSuspendTasks
//
HRESULT RDALSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;
	
//	sysDebugPrintf("RDALSuspendTasks()\n");

	hResult = TCTaskSuspend(uiRDALThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
//
// RDALInitializeTasks
//
HRESULT RDALInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;
	
//	sysDebugPrintf("RDALInitializeTasks()\n");

	hResult = TCTaskCreate(&uiRDALThreadID, RDALThreadName, RDALThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#endif

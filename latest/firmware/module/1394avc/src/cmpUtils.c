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
* DESCRIPTION: Connection Management Procedures (CMP) Utilities

	This module implements the connection management procedures defined in
	IEC 61883-1.

	The following registers are implemented:
		iPCR
		oPCR
		iMPR
		oMPR

	Functions for reading and performing locks on the registers

	Revisions:
		created 05/22/2003 lm


****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lock.h"
#include "avs.h"
#include "targetPlugs.h"
#include "cmp.h"
#include "cmpDefs.h"
#include "cmpCli.h"
#include "lal.h"
#include "irm.h"
#include "avcDriver.h"

static CMP_CHANNEL_USAGE_CB		cmpChannelUsageCBs[ISOCH_CHANNELS_ITEMS];
static CMP_SIGNAL_FORMAT_CHANGE	cmpSignalFormatCBs[MAX_SIGNAL_FORMAT_ITEMS];

static uint32					cmpSignalFormatCBItems = 0;

/*********************************************************
	callback for plug channel changes
*/
HRESULT cmpRegisterChannelUsageCB(uint32 channel, CMP_CHANNEL_USAGE_CB callback)
{
	HRESULT		hResult = NO_ERROR;

	if (channel >= ISOCH_CHANNELS_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (cmpChannelUsageCBs[channel] != NULL)
	{
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP) ||
			sysDebugIsEnabled(SYSDEBUG_TRACE_WARNINGS))
		{
			sysDebugPrintf("cmp:cmpRegisterChannelUsageCB: already registered (overwriting), channel: %i\n\r", channel);
		}
#endif //_SYSDEBUG
	}
#ifdef _SYSDEBUG
	else if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
	{
		sysDebugPrintf("cmp:cmpRegisterChannelUsageCB: channel: %i\n\r", channel);
	}
#endif //_SYSDEBUG

	cmpChannelUsageCBs[channel] = callback;

	return hResult;
}

HRESULT cmpUnregisterChannelUsageCB(uint32 channel)
{
	HRESULT		hResult = NO_ERROR;

	if (channel >= ISOCH_CHANNELS_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (cmpChannelUsageCBs[channel] == NULL)
	{
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP) ||
			sysDebugIsEnabled(SYSDEBUG_TRACE_WARNINGS))
		{
			sysDebugPrintf("cmp:cmpUnregisterChannelUsageCB: not registered, channel: %i\n\r", channel);
		}
#endif //_SYSDEBUG
		return hResult;
	}

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
	{
		sysDebugPrintf("cmp:cmpUnregisterChannelUsageCB: channel: %i\n\r", channel);
	}
#endif //_SYSDEBUG

	cmpChannelUsageCBs[channel] = NULL;

	return hResult;
}

HRESULT cmpCheckChannelUsageCB(uint32 channel)
{
	HRESULT		hResult = NO_ERROR;

	if (cmpChannelUsageCBs[channel] != NULL)
	{
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysDebugPrintf("cmp:cmpCheckChannelUsageCB: calling callback for channel: %i\n\r", channel);
		}
#endif //_SYSDEBUG

		hResult = (* cmpChannelUsageCBs[channel]) (channel);
	}

	return hResult;
}

/*********************************************************
	callback for plug format changes
*/
HRESULT cmpRegisterSignalFormatCB (BOOL bIn, uint32 plugID, uint32 format, CMP_SIGNAL_FORMAT_CB callback)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	BOOL		bFound = FALSE;

	if (cmpSignalFormatCBItems >= MAX_SIGNAL_FORMAT_ITEMS)
	{
		hResult = E_CALLBACK_REQUEST_FAILED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	for (i = 0; i < cmpSignalFormatCBItems; i++)
	{
		if (cmpSignalFormatCBs[i].bIn == bIn &&
			cmpSignalFormatCBs[i].plugID == plugID)
		{
			bFound = TRUE;
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
			{
				sysDebugPrintf("cmp:cmpSignalFormatCB: registering signal format plug already registered (overwriting)\n\r");
			}
#endif //_SYSDEBUG
		}
	}

	cmpSignalFormatCBs[i].bIn = bIn;
	cmpSignalFormatCBs[i].plugID = plugID;
	cmpSignalFormatCBs[i].format = format;
	cmpSignalFormatCBs[i].callback = callback;

	if (bFound == FALSE) cmpSignalFormatCBItems++;		// adding new item (not overwriting)

	return hResult;
}

HRESULT cmpUnregisterSignalFormatCB (BOOL bIn, uint32 plugID)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	BOOL		bFound = FALSE;

	for (i = 0; i < cmpSignalFormatCBItems; i++)
	{
		if (cmpSignalFormatCBs[i].bIn == bIn &&
			cmpSignalFormatCBs[i].plugID == plugID)
		{
			bFound = TRUE;
		}
	}

	if (bFound == FALSE)
	{
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysDebugPrintf("cmp:cmpSignalFormatCB: unregistering signal format plug not registered\n\r");
		}
#endif //_SYSDEBUG
	}
	else
	{
		cmpSignalFormatCBItems--;						// removing item
	}

	return hResult;
}

HRESULT cmpCheckSignalFormatCB (BOOL bIn, uint32 plugID, uint32 format)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;

	for (i = 0; i < cmpSignalFormatCBItems; i++)
	{
		if (cmpSignalFormatCBs[i].bIn == bIn &&
			cmpSignalFormatCBs[i].plugID == plugID)
		{
			if (cmpSignalFormatCBs[i].format != format)
			{
				hResult = (* cmpSignalFormatCBs[i].callback) ();
			}
		}
	}

	return hResult;
}

HRESULT cmpAllocateIRMResources(uint32 *channel, uint32 bandwidth)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _IRM
	//channel == IRM_ANY_AVAIL_ISOCH_CHANNEL returns irmAllocateNextAvailableChannel()
	hResult = irmAllocateSpecifiedChannel(channel);
#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
	{
		sysPrintCurTime();
		sysDebugPrintf("cmp: allocate IRM channel: %i %s\n\r", *channel, (hResult == NO_ERROR) ? "OK" : "ERROR");
	}
#endif //_SYSDEBUG
	if (hResult != NO_ERROR) return hResult;

	hResult = irmAllocateBandwidth(bandwidth);
#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
	{
		sysPrintCurTime();
		sysDebugPrintf("cmp: allocate IRM bandwidth: %i %s\n\r", bandwidth, (hResult == NO_ERROR) ? "OK" : "ERROR");
	}
#endif //_SYSDEBUG
	if (hResult != NO_ERROR)
	{
		irmFreeChannel(*channel);
		return hResult;
	}
#else
	hResult = E_IRM_CHANNEL_NOT_ALLOCATED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
#endif
	
	return hResult;
}

HRESULT cmpDeallocateIRMResources(uint32 channel, uint32 bandwidth)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _IRM
	HRESULT		hResultFree = NO_ERROR;

	hResultFree = irmFreeChannel(channel);
	if (hResult == NO_ERROR) hResult = hResultFree; 
	
	hResultFree = irmFreeBandwidth(bandwidth);
	if (hResult == NO_ERROR) hResult = hResultFree; 
#else
	hResult = E_IRM_CHANNEL_NOT_ALLOCATED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
#endif
	
	return hResult;
}

BOOL cmpIsAnyoPCRConnected(void)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bIsConnected = FALSE;
	uint32		plugRegister = 0;
	uint32		index;

	for (index = 0; index < plugsGetNumIsochOutPlugs(); index++)
	{
		hResult = cmpGetoPCR(index, &plugRegister);
		if (hResult == NO_ERROR)
		{
			bIsConnected = (cmpGetPCRConnCount(plugRegister) > 0);
			if (bIsConnected) break;
		}
	}

	return bIsConnected;
}

BOOL cmpIsAnyiPCRConnected(void)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bIsConnected = FALSE;
	uint32		plugRegister = 0;
	uint32		index;

	for (index = 0; index < plugsGetNumIsochInPlugs(); index++)
	{
		hResult = cmpGetiPCR(index, &plugRegister);
		if (hResult == NO_ERROR)
		{
			bIsConnected = (cmpGetPCRConnCount(plugRegister) > 0);
			if (bIsConnected) break;
		}
	}

	return bIsConnected;
}

static HRESULT cmpHandleGetRegister(uint32 operation, uint32 index, PCR_REGISTER **cmpRegister)
{
	HRESULT			hResult = NO_ERROR;

	switch (operation & CMP_REGISTER_MASK)
	{
		case CMP_REGISTER_OMPR:		// register oMPR
			*cmpRegister = &oMPRRegister;
			break;

		case CMP_REGISTER_IMPR:		// register iMPR
			*cmpRegister = &iMPRRegister;
			break;

		case CMP_REGISTER_OPCR:		// register oPCR
			if (index >= plugsGetNumIsochOutPlugs())
			{
				hResult = E_CMP_PCR_INVALID;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			*cmpRegister = &oPCRRegisters[index];
			break;

		case CMP_REGISTER_IPCR:		// register iPCR
			if (index >= plugsGetNumIsochInPlugs())
			{
				hResult = E_CMP_PCR_INVALID;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			*cmpRegister = &iPCRRegisters[index];
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	if (*cmpRegister == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

#ifdef GHS
#pragma ghs nowarning 236
#endif //GHS

static HRESULT cmpHandleGetMask(uint32 operation, uint32 *mask)
{
	HRESULT			hResult = NO_ERROR;
	BOOL			bReset = FALSE;

	bReset = ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_RESET);

	switch (operation & CMP_REGISTER_MASK)
	{
		case CMP_REGISTER_OMPR:		// register oMPR
			*mask = (bReset) ? oMPR_RESET_MASK : oMPR_WRITE_MASK;
			break;

		case CMP_REGISTER_IMPR:		// register iMPR
			*mask = (bReset) ? iMPR_RESET_MASK : iMPR_WRITE_MASK;
			break;

		case CMP_REGISTER_OPCR:		// register oPCR
			*mask = (bReset) ? oPCR_RESET_MASK : oPCR_WRITE_MASK;
			break;

		case CMP_REGISTER_IPCR:		// register iPCR
			*mask = (bReset) ? iPCR_RESET_MASK : iPCR_WRITE_MASK;
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

#ifdef GHS
#pragma ghs endnowarning
#endif //GHS

static HRESULT cmpHandleGetOffset1394(uint32 operation, uint32 index, OFFSET_1394 *offset)
{
	HRESULT			hResult = NO_ERROR;
	uint32			offsetBase = 0;

	switch (operation & CMP_REGISTER_MASK)
	{
		case CMP_REGISTER_OMPR:		// register oMPR
			offsetBase = oMPR_OFFSET;
			break;

		case CMP_REGISTER_IMPR:		// register iMPR
			offsetBase = iMPR_OFFSET;
			break;

		case CMP_REGISTER_OPCR:		// register oPCR
			offsetBase = oPCR_RANGE_LOW;
			break;

		case CMP_REGISTER_IPCR:		// register iPCR
			offsetBase = iPCR_RANGE_LOW;
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	offset->High = REGISTER_SPACE_ADDR_HI;
	offset->Low = offsetBase + (index << 2);

	return hResult;
}

static HRESULT cmpHandleRegisterUpdatePCR(uint32 plugRegisterOld, PCR_REGISTER *cmpRegister, BOOL bUpdateAll)
{
	HRESULT			hResult = NO_ERROR;
	BOOL			bAVSSetOnline = FALSE;
	BOOL			bAVSStop = FALSE;
	BOOL			bAVSStart = FALSE;
	BOOL			bAVSSetCh = FALSE;
	BOOL			bAVSSetRate = FALSE;
	BYTE			channelNew = 0;
	BYTE			channelOld = 0;
	BYTE			rateNew = 0;
	BYTE			rateOld = 0;
	BOOL			bOnlineNew = FALSE;
	BOOL			bOnlineOld = FALSE;
	BOOL			bConnectedNew = FALSE;
	BOOL			bConnectedOld = FALSE;
	uint32			plugRegisterNew = cmpRegister->plugRegister;
	BOOL			bTx = (cmpRegister->bIn == FALSE);
//	BOOL			bStarted = FALSE;

	channelNew = (uint8) cmpGetPCRChannel(plugRegisterNew); 
	if (bTx)
	{
		rateNew = (uint8) cmpGetPCRRate(plugRegisterNew); 
	}
	bOnlineNew = cmpGetPCROnline(plugRegisterNew);
	bConnectedNew = (cmpGetPCRConnCount(plugRegisterNew) != 0);

	channelOld = (uint8) cmpGetPCRChannel(plugRegisterOld); 
	if (bTx)
	{
		rateOld = (uint8) cmpGetPCRRate(plugRegisterOld); 
	}
	bOnlineOld = cmpGetPCROnline(plugRegisterOld);
	bConnectedOld = (cmpGetPCRConnCount(plugRegisterOld) != 0);

	if (bUpdateAll)
	{
		bAVSSetCh = TRUE;
		bAVSSetRate = bTx;
		bAVSSetOnline = TRUE;
		if (bConnectedNew == TRUE)						// changed from un-connected to connected - AVSStart rx/tx
		{
			bAVSStart = TRUE;
		}
		else											// changed from connected to un-connected - AVSStop rx/tx
		{
			bAVSStop = TRUE;
		}
	}
	else
	{
		// check if channel number has changed
		if (channelNew != channelOld)
		{
			bAVSSetCh = TRUE;
		}

		// check if data rate has changed
		if (bTx && (rateNew != rateOld))
		{
			bAVSSetRate = TRUE;
		}

		// check if online/offline has changed
		if (bOnlineNew != bOnlineOld)
		{
			bAVSSetOnline = TRUE;
		}

		// check if connected state has changed
		if (bConnectedNew != bConnectedOld)
		{
			if (bConnectedNew == TRUE)						// changed from un-connected to connected - AVSStart rx/tx
			{
#if 0
				if ((channelNew != channelOld) ||
					(rateNew != rateOld))
				{
					bAVSStop = TRUE;
				}
#endif
				bAVSStart = TRUE;
			}
			else											// changed from connected to un-connected - AVSStop rx/tx
			{
				bAVSStop = TRUE;
			}
		}
	}

	if (bAVSSetOnline)
	{
		hResult = avsSetOnline(cmpRegister->plugID, bTx, bOnlineNew);

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysDebugPrintf("cmp:calling avsSetOnline%s %s for PlugID: %i\n\r", (bTx) ? "Tx": "Rx", (bOnlineNew) ? "TRUE": "FALSE", cmpRegister->plugID);
		}
#endif //_SYSDEBUG
	}

	if (bAVSStop)
	{
		avcDriverSetEnable(FALSE);
	}

	if (bAVSSetCh)
	{
		if (channelNew < 64)
		{
			hResult = avsSetCh(cmpRegister->plugID, bTx, channelNew);
			hResult += avcDriverSetCh(cmpRegister->plugID, bTx, channelNew);
		}
		if (hResult != NO_ERROR) return hResult;

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysDebugPrintf("cmp:calling %s with channel: %i for PlugID: %i\n\r", (bTx) ? "avsSetTxCh" : "avsSetRxCh", channelNew, cmpRegister->plugID);
		}
#endif //_SYSDEBUG
	}

	if (bAVSSetRate)
	{
		hResult = avsSetTxSpeed(cmpRegister->plugID, rateNew);
		if (hResult != NO_ERROR) return hResult;

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysDebugPrintf("cmp:calling avsSetTxRate with rate: %i for PlugID: %i\n\r", rateNew, cmpRegister->plugID);
		}
#endif //_SYSDEBUG
	}

	if (bAVSStart)
	{
		avcDriverSetEnable(TRUE);
	}
	if (bAVSStop && bConnectedOld)	// check Channel Usage callbacks (channel changed or stopped using channel)
	{
		cmpCheckChannelUsageCB (channelOld);
	}

	return hResult;
}

static HRESULT cmpHandleRegisterUpdateFormat(uint32 plugFormatOld, PCR_REGISTER *cmpRegister, BOOL bUpdateAll)
{
	HRESULT			hResult = NO_ERROR;
	uint32			plugFormatNew = cmpRegister->plugFormat;
	
	if ((plugFormatOld != plugFormatNew) || bUpdateAll)
	{
		// the device is all in one clock domain, set all input and output PCR's to this rate
		hResult = cmpHandleGetRegister(CMP_REGISTER_IPCR, 0, &cmpRegister);
		if (NO_ERROR == hResult)
		{
			cmpRegister->plugFormat = plugFormatNew;
			cmpSetPCROnline(&cmpRegister->plugRegister, 1);
		}
		hResult = cmpHandleGetRegister(CMP_REGISTER_OPCR, 0, &cmpRegister);
		if (NO_ERROR == hResult)
		{
			cmpRegister->plugFormat = plugFormatNew;
			cmpSetPCROnline(&cmpRegister->plugRegister, 1);
		}
/*
		hResult = cmpHandleGetRegister(CMP_REGISTER_IPCR, 1, &cmpRegister);
		if (NO_ERROR == hResult)
		{
			cmpRegister->plugFormat = plugFormatNew;
			cmpSetPCROnline(&cmpRegister->plugRegister, 1);
		}
		hResult = cmpHandleGetRegister(CMP_REGISTER_OPCR, 1, &cmpRegister);
		if (NO_ERROR == hResult)
		{
			cmpRegister->plugFormat = plugFormatNew;
			cmpSetPCROnline(&cmpRegister->plugRegister, 1);
		}
*/
		// check Input or Output Plug Signal Format callbacks (channel changed or stopped using channel)
// bk:		cmpCheckSignalFormatCB (cmpRegister->bIn, cmpRegister->plugID, plugFormatNew);
	}

	return hResult;
}

static HRESULT cmpHandleRegisterUpdate(uint32 operation, uint32 oldValue, PCR_REGISTER *cmpRegister)
{
	HRESULT			hResult = NO_ERROR;

	switch (operation & CMP_TYPE_MASK)
	{
		case CMP_TYPE_REGISTER:
			switch (operation & CMP_REGISTER_MASK)		
			{
				case CMP_REGISTER_OPCR:				// adjust avs for new oPCR value
				case CMP_REGISTER_IPCR:				// adjust avs for new iPCR value
// bkbk:??			hResult = cmpHandleRegisterUpdatePCR(oldValue, cmpRegister, FALSE);
					hResult = cmpHandleRegisterUpdatePCR(oldValue, cmpRegister, TRUE);
					break;
				
				default:
					hResult = E_NOT_IMPLEMENTED;
					sysLogError(hResult, __LINE__, moduleName);
					break;
			}
			break;

		case CMP_TYPE_FORMAT:
			switch (operation & CMP_REGISTER_MASK)		
			{
				case CMP_REGISTER_OPCR:				// adjust avs for new oPCR format
				case CMP_REGISTER_IPCR:				// adjust avs for new iPCR format
// bkbk:??			hResult = cmpHandleRegisterUpdateFormat(oldValue, cmpRegister, FALSE);
					hResult = cmpHandleRegisterUpdateFormat(oldValue, cmpRegister, TRUE);
					break;

				default:
					hResult = E_NOT_IMPLEMENTED;
					sysLogError(hResult, __LINE__, moduleName);
					break;
			}
			break;

		case CMP_TYPE_STATE:
			hResult = E_NOT_IMPLEMENTED;
			sysLogError(hResult, __LINE__, moduleName);
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

static HRESULT cmpHandleRegisterRead(uint32 operation, uint32 *value, PCR_REGISTER *cmpRegister)
{
	HRESULT			hResult = NO_ERROR;

	switch (operation & CMP_TYPE_MASK)
	{
		case CMP_TYPE_REGISTER:		// read plug register
			*value = cmpRegister->plugRegister;
			break;

		case CMP_TYPE_FORMAT:		// read plug format
			*value = cmpRegister->plugFormat;
			break;

		case CMP_TYPE_STATE:		// read plug state
			*value = cmpGetPCRState(cmpRegister->plugRegister);
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

static HRESULT cmpHandleRegisterWrite(uint32 operation, uint32 value, PCR_REGISTER *cmpRegister)
{
	HRESULT			hResult = NO_ERROR;
	uint32			valueOld = 0;

	switch (operation & CMP_TYPE_MASK)
	{
		case CMP_TYPE_REGISTER:		// write plug register
			hResult = E_NOT_IMPLEMENTED;
			sysLogError(hResult, __LINE__, moduleName);
			break;

		case CMP_TYPE_FORMAT:		// write plug format
			if (cmpRegister->plugFormat != value)
			{
				valueOld = cmpRegister->plugFormat;
				cmpRegister->plugFormat = value;
				cmpHandleRegisterUpdate(operation, valueOld, cmpRegister);
			}
			break;

		case CMP_TYPE_STATE:		// write plug state
			hResult = E_NOT_IMPLEMENTED;
			sysLogError(hResult, __LINE__, moduleName);
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

static HRESULT cmpHandleRegisterLock(uint32 operation, uint32 valueCompare, uint32 valueNew, uint32 *valueOld, PCR_REGISTER *cmpRegister)
{
	HRESULT			hResult = NO_ERROR;
	uint32			*valueReg;
	uint32			mask = 0;

	hResult = cmpHandleGetMask(operation, &mask);
	if (hResult != NO_ERROR) return hResult;

	switch (operation & CMP_TYPE_MASK)
	{
		case CMP_TYPE_REGISTER:		// do COMPARE_SWAP lock transaction on plug register
			valueReg = &cmpRegister->plugRegister;
			*valueOld = *valueReg;

			if (valueCompare == *valueReg)
			{
	 			*valueReg = ((valueNew & mask) | (*valueReg & (~mask))); 
			}

			if (*valueOld != *valueReg)
			{
				cmpHandleRegisterUpdate(operation, *valueOld, cmpRegister);
			}
			break;

		case CMP_TYPE_FORMAT:		// do COMPARE_SWAP lock transaction on plug format
			hResult = E_NOT_IMPLEMENTED;
			sysLogError(hResult, __LINE__, moduleName);
			break;

		case CMP_TYPE_STATE:		// do COMPARE_SWAP lock transaction on plug state
			hResult = E_NOT_IMPLEMENTED;
			sysLogError(hResult, __LINE__, moduleName);
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}
	
	return hResult;
}

HRESULT cmpHandleRegisterReset(uint32 operation, uint32 resetValue, PCR_REGISTER *cmpRegister)
{
	HRESULT			hResult = NO_ERROR;
	uint32			mask = 0;

	hResult = cmpHandleGetMask(operation, &mask);
	if (hResult != NO_ERROR) return hResult;

	switch (operation & CMP_REGISTER_MASK)
	{
		case CMP_REGISTER_OMPR:		// register oMPR
		case CMP_REGISTER_IMPR:		// register iMPR
			cmpRegister->plugRegister &= mask;
			// set MPRs to post bus reset values
			cmpRegister->plugRegister |= resetValue;
			break;

		case CMP_REGISTER_OPCR:		// register oPCR
		case CMP_REGISTER_IPCR:		// register iPCR

			switch (resetValue)
			{
				case CMP_RESET_MARK:
					// change any PLUG_ACTIVE plugs to PLUG_READY (and PLUG_SUSPENDED to PLUG_IDLE)
					// without stopping data flow tx/rx
					if (cmpGetPCRState(cmpRegister->plugRegister) == PLUG_ACTIVE)
					{
						cmpRegister->bMarked = TRUE;
					}
					else
					{
						cmpRegister->bMarked = FALSE;
					}
					cmpRegister->plugRegister &= mask;
					break;
				case CMP_RESET_STOP:
					// stopping data flow tx/rx for PLUG_READY plugs
					if (cmpRegister->bMarked == TRUE &&
						cmpGetPCRState(cmpRegister->plugRegister) == PLUG_READY)
					{
						uint32 oldValue = 0;
						cmpRegister->bMarked = FALSE;
						// stop tx/rx the isoc stream (since newValue is disconnected and by setting oldValue to connected)
						oldValue = cmpRegister->plugRegister | oPCR_MASK_BCCOUNT;
						hResult = cmpHandleRegisterUpdatePCR(oldValue, cmpRegister, FALSE);
					}
					break;
			}
			break;

		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

HRESULT cmpHandleRegister(uint32 operation, uint32 index, uint32 *value1, uint32 *value2, uint32 *value3)
{
	HRESULT			hResult = NO_ERROR;
	PCR_REGISTER	*cmpRegister = NULL;

	hResult = cmpHandleGetRegister(operation, index, &cmpRegister);
	if (hResult != NO_ERROR) return hResult;

	hResult = TCMutexLock(cmpRegister->plugMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_READ)				// read operation
	{
		hResult = cmpHandleRegisterRead(operation, value1, cmpRegister);
	}
	else if ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_WRITE)		// write operation
	{
		hResult = cmpHandleRegisterWrite(operation, *value1, cmpRegister);
	}
	else if ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_LOCK)		// lock operation
	{
		hResult = cmpHandleRegisterLock(operation, *value1, *value2, value3, cmpRegister);
	}
	else if ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_RESET)		// reset operation
	{
		hResult = cmpHandleRegisterReset(operation, *value1, cmpRegister);
	}
	else
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
	}

	TCMutexUnlock(cmpRegister->plugMutexSemID);

	return hResult;
}

// handling of local/remote plug registers
HRESULT cmpHandleNodeRegister(uint32 operation, uint32 index, uint16 nodeAddr, uint32 *value1, uint32 *value2, BOOL bHighPriority)
{
	HRESULT			hResult = NO_ERROR;

	if (lalIsThisNodeAddr(nodeAddr))										// local operation
	{
		uint32	oldValue;
		hResult = cmpHandleRegister(operation, index, value1, value2, &oldValue);
	
		if ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_LOCK)				// lock operation
		{
			if (*value1 != oldValue)
			{
				hResult = E_CMP_PCR_BUSY;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
		}
	}
	else																	// remote operation
	{
		OFFSET_1394	offset;

		hResult = cmpHandleGetOffset1394(operation, index, &offset);

		if ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_READ)				// read operation iPCR (remote)
		{
			if (bHighPriority)
			{
				hResult = lhlReadNodeTimeout(nodeAddr, offset, 4, value1, LHL_QUEUE_PRIORITY, LHL_TX_REQ_SHORT_TIMEOUT_MSECS);
			}
			else
			{
				hResult = lalReadNode(nodeAddr, offset, 4, value1);
			}
#ifdef _SYSDEBUG // debug
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
			{
				sysPrintCurTime();
				if (hResult == NO_ERROR)
				{
					sysDebugPrintf("cmp:readNode%s nodeAddr:0x%04x value:0x%08x\n\r", (bHighPriority) ? "HighPri" : "", nodeAddr, *value1);
				}
				else
				{
					sysDebugPrintf("cmp:readNode%s nodeAddr:0x%04x Error:0x%08x\n\r", (bHighPriority) ? "HighPri" : "", nodeAddr, hResult);
				}
			}
#endif //_SYSDEBUG
		}
		else if ((operation & CMP_COMMAND_MASK) == CMP_COMMAND_LOCK)		// lock operation
		{
			uint32	data[2];
			data[0] = *value1;
			data[1] = *value2;
			if (bHighPriority)
			{
				hResult = lhlLockNodeTimeout(nodeAddr, offset, 8, &(data[0]), LHL_QUEUE_PRIORITY, PB_LOCKTYPE_COMPARE_SWAP, LHL_TX_REQ_SHORT_TIMEOUT_MSECS);
			}
			else
			{
				hResult = lalLockNode(nodeAddr, offset, 8, &(data[0]), PB_LOCKTYPE_COMPARE_SWAP);
			}
#ifdef _SYSDEBUG // debug
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
			{
				sysPrintCurTime();
				if (hResult == NO_ERROR)
				{
					sysDebugPrintf("cmp:lockNode%s nodeAddr:0x%04x arg:0x%08x data:0x%08x\n\r", (bHighPriority) ? "HighPri" : "", nodeAddr, *value1, *value2);
				}
				else
				{
					sysDebugPrintf("cmp:lockNode%s nodeAddr:0x%04x Error:0x%08x\n\r", (bHighPriority) ? "HighPri" : "", nodeAddr, hResult);
				}
			}
#endif //_SYSDEBUG
		}
		else
		{
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
		}
	}

	return hResult;
}

HRESULT cmpUpdateoMPR(void)
{
	HRESULT			hResult = NO_ERROR;

	//LM???

	return hResult;
}

HRESULT cmpUpdateiMPR(void)
{
	HRESULT			hResult = NO_ERROR;

	//LM???

	return hResult;
}

HRESULT cmpUpdateoPCR(uint32 oPCRNumber)
{
	HRESULT			hResult = NO_ERROR;
	PCR_REGISTER	*cmpRegister = NULL;
	uint32			format = 0;

	hResult = cmpHandleGetRegister(CMP_REGISTER_OPCR, oPCRNumber, &cmpRegister);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpHandleRegisterUpdatePCR(0, cmpRegister, TRUE);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpGetoPCRFormat(oPCRNumber, &format);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpHandleRegisterUpdateFormat(format, cmpRegister, TRUE);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT cmpUpdateiPCR(uint32 iPCRNumber)
{
	HRESULT			hResult = NO_ERROR;
	PCR_REGISTER	*cmpRegister = NULL;
	uint32			format = 0;

	hResult = cmpHandleGetRegister(CMP_REGISTER_IPCR, iPCRNumber, &cmpRegister);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpHandleRegisterUpdatePCR(0, cmpRegister, TRUE);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpGetiPCRFormat(iPCRNumber, &format);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpHandleRegisterUpdateFormat(format, cmpRegister, TRUE);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}


uint32 cmpGetPCRConnCount(uint32 PCR)
{
	uint32	count = 0;

	count += cmpGetPCRBCCount(PCR);
	count += cmpGetPCRP2PCount(PCR);

	return count;
}

uint32 cmpGetPCRState(uint32 PCR)
{
	BOOL			online = FALSE;
	BOOL			connected = FALSE;
	uint32			state = PLUG_IDLE;

	online = cmpGetPCROnline(PCR);
	connected = (cmpGetPCRConnCount(PCR) > 0);

	if (online == TRUE && connected == TRUE)
	{
		state = PLUG_ACTIVE;
	}
	else if (online == TRUE && connected == FALSE)
	{
		state = PLUG_READY;
	}
	else if (online == FALSE && connected == TRUE)
	{
		state = PLUG_SUSPENDED;
	}
	else if (online == FALSE && connected == FALSE)
	{
		state = PLUG_IDLE;
	}

	return state;
}

uint32 cmpGetPCRBandwidth(uint32 PCR)										// only valid for oPCR
{
	HRESULT		hResult = NO_ERROR;
	uint32		bandwidth = 0;
	uint32		rate = 0;
	uint32		overheadID = 0;
	uint32		payload = 0;
	uint32		constC = 32;
	uint32		constK = 3;
	uint32		coefDR = 0;

	rate = cmpGetPCRRate(PCR);
	overheadID = cmpGetPCROverheadID(PCR);
	payload = cmpGetPCRPayload(PCR);

	switch (rate)
	{
		case SPEED_100: coefDR = 16; break;
		case SPEED_200: coefDR = 8; break;
		case SPEED_400: coefDR = 4; break;
		default:
			hResult = E_NCI_INVALID_SPEED;
			sysLogError(hResult, __LINE__, moduleName);
			return 0;
	}
	
	if (overheadID > 0)
	{
		bandwidth = overheadID * constC + (payload + constK) * coefDR;
	}
	else // overheadID = 0
	{
		bandwidth = 512 + (payload + constK) * coefDR;
	}

	return bandwidth;
}


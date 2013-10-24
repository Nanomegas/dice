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
 * DESCRIPTION: Link Hardware Abstraction Layer LLC (Link Layer Control)

		lhl LLC Tools and Variables
	
		for handling LLC specific cases

	Revisions:
		created 12/16/2002 kbj

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lhl.h"
#include "lhlLLC.h"
#include "lhlLLCDefs.h"
#include "llcInt.h"
#include "llcReg.h"
#include "nciIRM.h"
#include "csr.h"

#ifdef _LOOSE_ISO
#include "lalAsyncStream.h"
#endif

static uint32 lhlLLCHandleCSRMutexSemID; // mutual exclusion semaphore for handleCSR

HRESULT lhlLLCGetHeaderQuadSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumQuads)
{
	HRESULT	hResult = NO_ERROR;

	*headerNumQuads = 0;

	switch (tCode)
	{
		case TCODE_ISOBLOCK: // covering ASYNC STREAM too
			if (axType == LHL_RX)
			{
				*headerNumQuads = 1;	// receive stream packet
			}
			else
			{
				*headerNumQuads = 2;	// transmit stream packet
			}
			break;

		case TCODE_PHY_PACKET:
			*headerNumQuads = 3;
			break;

		default:
			hResult = lhl1394GetHeaderQuadSizeFromTCode(axType, tCode, headerNumQuads);
			break;
	}

	return hResult;
}

HRESULT lhlLLCGetPayloadSizeIndex(uint16 tCode, uint16 headerNumQuads, uint32 *index)
{
	HRESULT	hResult = NO_ERROR;
	BOOL bFound = FALSE;

	*index = 0;

	switch (tCode)
	{
		default:
			if (headerNumQuads > 3)
			{
				// at this point, the length is always in the same place
				*index = 3;
				bFound = TRUE;
			}
			break;
#ifdef _LOOSE_ISO
		case TCODE_ISOBLOCK: // covering ASYNC STREAM too
			if (headerNumQuads == 1)
			{
				*index = 0;	// receive stream packet
				bFound = TRUE;
			}
			else
			{
				*index = 1;	// transmit stream packet
				bFound = TRUE;
			}
			break;
#endif //_LOOSE_ISO
	}

	if (bFound == FALSE)
	{
		hResult = E_LHL_TX_INVALID_HEADER;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT lhlLLCGetPayloadSize(uint16 tCode, uint16 headerNumQuads, QUADLET *qHeader, uint16 *payloadNumBytes)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index;

	*payloadNumBytes = 0;

	hResult = lhlLLCGetPayloadSizeIndex(tCode, headerNumQuads, &index);
	if (hResult != NO_ERROR) return hResult;

	*payloadNumBytes = (uint16)((qHeader[index] & MASK_DATA_LEN) >> SHIFT_DATA_LEN);	// length in bytes

	return hResult;
}

HRESULT lhlLLCSetPayloadSize(uint16 tCode, uint16 headerNumQuads, QUADLET *qHeader, uint16 payloadNumBytes)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index;

	hResult = lhlLLCGetPayloadSizeIndex(tCode, headerNumQuads, &index);
	if (hResult != NO_ERROR) return hResult;

	qHeader[index] &= ~MASK_DATA_LEN;
	qHeader[index] = ((payloadNumBytes << SHIFT_DATA_LEN) & MASK_DATA_LEN);	// length in bytes

	return hResult;
}

#ifdef _LOOSE_ISO
HRESULT lhlLLCSetAsyncStreamEnabled(BOOL bIsoEnabled)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(bIsoEnabled);

	return hResult;
}

HRESULT lhlLLCSetAsyncStreamChannel(uint8 channel, BOOL bEnabled)
{
	HRESULT		hResult = NO_ERROR;
	uint32		asyncEnbReg;
	uint32		channels;
	uint32		channelBit = 1;

	if (channel >= ASYNC_STREAM_ALL_CH)
	{	// Handling of all stream channels
		if (bEnabled)
		{
			llcLinkRegWrite(ASYN_RX_ENB_REG_1_DP, 0xFFFFFFFF);
			llcLinkRegWrite(ASYN_RX_ENB_REG_2_DP, 0xFFFFFFFF);
		}
		else
		{
			llcLinkRegWrite(ASYN_RX_ENB_REG_1_DP, 0);
			llcLinkRegWrite(ASYN_RX_ENB_REG_2_DP, 0);
		}
		return hResult;
	}

	if (channel < 32)
	{	// Handling of channels 0 - 31
		asyncEnbReg = ASYN_RX_ENB_REG_1_DP;
		channelBit = channelBit << channel;
	}
	else
	{	// Handling of channels 31 - 63
		asyncEnbReg = ASYN_RX_ENB_REG_2_DP;
		channelBit = channelBit << (channel - 32);
	}

	llcLinkRegRead(asyncEnbReg, &channels);
	if (bEnabled)
	{
		channels |= channelBit;
	}
	else
	{
		channels &= ~channelBit;
	}
	llcLinkRegWrite(asyncEnbReg, channels);

	return hResult;
}

/*********************************************************
	Set header quads from async stream values
*/
HRESULT lhlLLCSetAsyncStreamHeader(QUADLET *qHeader, uint8 channel, uint8 tag, uint8 sy)
{
	HRESULT			hResult = NO_ERROR;
	
	hResult = lhlLLCSetAsyncStreamQuad(&qHeader[0], channel, tag, sy);

	return hResult;
}

/*********************************************************
	Get async stream values from header quads
*/
HRESULT lhlLLCGetAsyncStreamHeader(QUADLET *qHeader, uint8 *channel, uint8 *tag, uint8 *sy)
{
	HRESULT			hResult = NO_ERROR;

	hResult = lhlLLCGetAsyncStreamQuad(qHeader[0], channel, tag, sy);

	return hResult;
}

/*********************************************************
	Set header quad from async stream values
*/
HRESULT lhlLLCSetAsyncStreamQuad(QUADLET *quad, uint8 channel, uint8 tag, uint8 sy)
{
	HRESULT			hResult = NO_ERROR;
	
	*quad |= (tag & MASK_AS_TAG) << SHIFT_AS_TAG;
	*quad |= (channel & MASK_AS_CHANNEL) << SHIFT_AS_CHANNEL;
	*quad |= PB_TYPE_ASYNC_STREAM << SHIFT_TCODE;
	*quad |= (sy & MASK_AS_SY);

	return hResult;
}

/*********************************************************
	Get async stream values from header quad
*/
HRESULT lhlLLCGetAsyncStreamQuad(QUADLET quad, uint8 *channel, uint8 *tag, uint8 *sy)
{
	HRESULT			hResult = NO_ERROR;

	*tag = (uint8) ((quad >> SHIFT_AS_TAG) & MASK_AS_TAG);
	*channel = (uint8) ((quad >> SHIFT_AS_CHANNEL) & MASK_AS_CHANNEL);
	*sy = (uint8) ((quad) & MASK_AS_SY);

	return hResult;
}
#endif //_LOOSE_ISO

/*********************************************************
	Handle LLC CSR operations
*/
HRESULT lhlLLCHandleCSR(OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 reqType)
{
	HRESULT		hResult = NO_ERROR;
	uint32		llcType = LLC_CSR_UNDEF;

	// exclusive access for the handleCSR (mutex)
	hResult = TCMutexLock(lhlLLCHandleCSRMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	switch (destOffset.Low)
	{
		// required registers in the Architecture Space
		case CSR_ARCH_STATE_CLEAR:
		case CSR_ARCH_STATE_SET:
		case CSR_ARCH_NODEIDS:
		case CSR_ARCH_RESET_START:
		case CSR_ARCH_SPLIT_HI:
		case CSR_ARCH_SPLIT_LO:
			if ((reqType == LHL_REQ_READ) && 
				(numBytes == 4)) // only allow quadlet read
			{
				llcType = LLC_CSR_READ;
			}
			else if ((reqType == LHL_REQ_WRITE) &&
				(numBytes == 4)) // only allow quadlet write)
			{
				llcType = LLC_CSR_WRITE;
			}
			else
			{
				hResult = E_BAD_INPUT_PARAMETERS;
				sysLogError(hResult, __LINE__, moduleName);
			}
			{
				uint32		llcArch = LLC_ARCH_STATE_CLEAR;

				switch (destOffset.Low)
				{
					case CSR_ARCH_STATE_CLEAR:
						llcArch = LLC_ARCH_STATE_CLEAR;
						break;
					case CSR_ARCH_STATE_SET:
						llcArch = LLC_ARCH_STATE_SET;
						break;
					case CSR_ARCH_NODEIDS:
						llcArch = LLC_ARCH_NODEIDS;
						break;
					case CSR_ARCH_RESET_START:
						llcArch = LLC_ARCH_RESET_START;
						break;
					case CSR_ARCH_SPLIT_HI:
						llcArch = LLC_ARCH_SPLIT_TO_HI;
						break;
					case CSR_ARCH_SPLIT_LO:
						llcArch = LLC_ARCH_SPLIT_TO_LO;
						break;
				}
				hResult = llcHandleCSRarch(llcArch, llcType, data);
			}
			break;
		default:
			hResult = E_NOT_IMPLEMENTED;
			break;

	}

	// exclusive access for the handleCSR (mutex)
	TCMutexUnlock(lhlLLCHandleCSRMutexSemID);

	return hResult;
}

HRESULT lhlLLCConfigROMUpdated(void) 
{
	HRESULT hResult = NO_ERROR;

//LM???	llcConfigROMUpdated();

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT lhlLLCResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// llc Interrupts handler thread
	hResult = llcIntResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT lhlLLCSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// llc Interrupts handler thread
	hResult = llcIntSuspendTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlLLCInitializeTasks(void)
{
	HRESULT	hResult = NO_ERROR;

	// llc Interrupts handler thread
	hResult = llcIntInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlLLCInitialize(void)
{
	HRESULT	hResult = NO_ERROR;

	hResult = llcSoftwareReset();
	if (hResult != NO_ERROR) return hResult;

	// exclusive access for the handleCSR (mutex)
	hResult = TCMutexOpen(&lhlLLCHandleCSRMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}


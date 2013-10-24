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
 /*********************************************************************
 * DESCRIPTION: Function Control Protocol (FCP) 


	Revisions:
		created 09/17/2002 lm

***********************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "fcp.h"
#include "datastream.h"

// list of callbacks

static FCP_CB_ELEMENT	fcpCallbackList[MAX_FCP_CBS];

// how many callbacks are registered
static int32			fcpCallbackCount = 0;

OFFSET_1394		fcpOffsetCommand = FCP_OFFSET_COMMAND;
OFFSET_1394		fcpOffsetCommandEnd = FCP_OFFSET_COMMAND_END;
OFFSET_1394		fcpOffsetResponse = FCP_OFFSET_RESPONSE;
OFFSET_1394		fcpOffsetResponseEnd = FCP_OFFSET_RESPONSE_END;

HRESULT fcpInitialize(void)
{
	HRESULT			hResult = NO_ERROR;

	fcpCallbackCount = 0;

	hResult = lalAddressRangeRegisterCB(&fcpCallback, fcpOffsetCommand, fcpOffsetCommandEnd);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalAddressRangeRegisterCB(&fcpCallback, fcpOffsetResponse, fcpOffsetResponseEnd);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT fcpRegisterCallback(uint8 ctsCode, FCP_CB_FUNCTION callback)
{
	HRESULT		hResult = NO_ERROR;

	if (fcpCallbackCount < MAX_FCP_CBS)
	{
		fcpCallbackList[fcpCallbackCount].ctsCode  = ctsCode;
		fcpCallbackList[fcpCallbackCount].callback = callback;
		fcpCallbackCount += 1;
	}
	else
	{
		hResult = E_CALLBACK_REQUEST_FAILED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT fcpCallback(PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	int32 			callbackIndex = 0;
	UQUAD			*pPayload = NULL;			// pointer to first quadlet in incoming payload
	uint32			ctsCode = 0;				// ctsCode of incoming FCP packet
	uint32			payloadSize = 0;
	BOOL			bFcpCallbackFound = FALSE;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	RCODE_1394		responseCode = RSP_COMPLETE;

	hResult = pbGetPacketType(packetBlock, &packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayloadSize(packetBlock, &payloadSize);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(packetBlock, (void **) &pPayload);
	if (hResult != NO_ERROR) return hResult;

	// check that we have got a write request of length > 0
	if (((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) ||
		 (packetType == PB_TYPE_WRITE_REQUEST)) &&
		(payloadSize > 0))
	{
		// check the cts code and see if we have a callback registered for it (first 4 bits of payload data)
		ctsCode = (((pPayload->q) >> 28) & 0xF);
			
		for (callbackIndex = 0; callbackIndex < fcpCallbackCount; callbackIndex++)
		{
			if (fcpCallbackList[callbackIndex].ctsCode == ctsCode)
			{
				// got a matching CTS code
				bFcpCallbackFound = TRUE;
				break;
			}
		}
	}

	if (pbPacketIsBroadcast(packetBlock) == FALSE)
	{
		// if not bcast then send RSP_COMPLETE or RSP_TYPE_ERROR - depending on whether we handle this packet or not
		responseCode = (bFcpCallbackFound == TRUE) ? RSP_COMPLETE : RSP_TYPE_ERROR;
		hResult = lalReplyWriteResponse(packetBlock, responseCode, TRUE);
	}

	if (bFcpCallbackFound == TRUE)
	{
		// do the callback
		hResult = fcpCallbackList[callbackIndex].callback(packetBlock);
  	}

	return hResult;
}


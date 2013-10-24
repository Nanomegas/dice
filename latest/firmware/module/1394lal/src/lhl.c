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
 * DESCRIPTION: Link Hardware Abstraction Layer transmit packet i/o

	Purpose:	Link Hardware Abstraction Layer transmit packet i/o
						handling for asynchronous transmit packets
	
	Revisions:
		created 04/03/2002 lm

****************************************************************************/

#include "TCTasking.h"
#include "lhl.h"
#include "lhlARx.h"
#include "lhlATx.h"
#include "lhlBri.h"
#include "lhlCli.h"
#include "lhlIRM.h"
#include "lhlLLC.h"
#include "lhlLLCCli.h"
#include "lhlLLCDefs.h"
#include "lhlMsgQueue.h"
#include "lhlPendingTx.h"
#include "lhlPHY.h"
#include "lhlReq.h"
#include "lhlRsp.h"
#include "lhlService.h"
#include "lhlStatistics.h"
#include "lhlStatusQueue.h"
#include "lhlTLabel.h"
#ifdef _WATCH_THREAD
#include "lhlWatch.h"
#endif //_WATCH_THREAD

static char 		moduleName[] = __FILE__ ;

/*********************************************************
	Initializes this module (messageQueues, AR/AT Tasks (and resources)
*/
HRESULT lhlInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = lhlLLCInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlTxInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlRxInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlBriInitialize();
	if (hResult != NO_ERROR) return hResult;

#ifdef _WATCH_THREAD
	hResult = lhlWatchInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_WATCH_THREAD
	
	hResult = lhlServiceInitialize();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = lhlLLCResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlTxResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlRxResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlBriResumeTasks();
	if (hResult != NO_ERROR) return hResult;

#ifdef _WATCH_THREAD
	hResult = lhlWatchResumeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif //_WATCH_THREAD
	
	// enable interrupts before causing a bus reset in the lhlSevice task
	hResult = lhlServiceResumeAllIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	
	hResult = lhlServiceResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlInitialize(void)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bIRMEnabled = FALSE;

#ifdef _IRMC_CAPS // iso_resource_manager capable
	bIRMEnabled = TRUE;
#endif //_IRMC_CAPS

#ifdef _STATISTICS
	hResult = lhlStatisticsInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_STATISTICS

	hResult = lhlLLCInitialize();
	if (hResult != NO_ERROR) return hResult;

	lhlSetIRMEnabled(bIRMEnabled);

	hResult = lhlTLabelInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlMsgQueueInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlStatusQueueInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlPendingTxInitialize();
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_TOOL_LHL
	lhlCliInstallTools();
#endif //_CLI_TOOL_LHL

	return hResult;
}

HRESULT lhlDisable(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlServiceDisableLinkController();	// Disabling of Link Layer controller
	if (hResult != NO_ERROR) return hResult;

	//LM??? more...
	
	return hResult;
}

/*
	determine, when possible, the number of quadlets in a packet header based on tCode
*/
HRESULT lhlGetHeaderQuadSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumQuads)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlLLCGetHeaderQuadSizeFromTCode(axType, tCode, headerNumQuads);

	return hResult;
}

HRESULT lhl1394GetHeaderQuadSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumQuads) // must be called from lhlLLCGetHeaderQuadSizeFromTCode (for handling LLC specific cases)
{
	HRESULT	hResult = NO_ERROR;

	*headerNumQuads = 0;

	UNUSED_ARG(axType);

	switch (tCode)
	{
		case TCODE_RDQUAD:
		case TCODE_WRRESPONSE:
		case TCODE_RDQUADRSP:
		case TCODE_WRQUAD:
			*headerNumQuads = 3;
			break;

		case TCODE_RDBLOCK:
		case TCODE_WRBLOCK:
		case TCODE_RDBLOCKRSP:
		case TCODE_LOCKRQST:
		case TCODE_LOCKRSP:
			*headerNumQuads = 4;
			break;

		default:
			hResult = E_LHL_UNSUPPORTED_TCODE;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

HRESULT lhlGetHeaderSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumBytes)
{
	HRESULT	hResult = NO_ERROR;
	uint16 headerNumQuads;

	*headerNumBytes = 0;

	hResult = lhlGetHeaderQuadSizeFromTCode(axType, tCode, &headerNumQuads);

	if (hResult == NO_ERROR)
	{
		*headerNumBytes = (uint16) (headerNumQuads * 4);
	}

	return hResult;
}

BOOL lhlIsPayloadSizeValid(PB* packetBlock, uint16 numBytes, BOOL brx)
{
	// make sure request size can be handled by hardware limits
	if ((numBytes == 0) || 
		(numBytes > (brx?LINK_MAXRXPAYLOAD_SIZE_BYTES:LINK_MAXTXPAYLOAD_SIZE_BYTES)))
	{
		return FALSE;
	}

#if 1 //LM
	UNUSED_ARG(packetBlock);
#else
//LM??? check using speed info and speed map
#endif

	return TRUE;
}

/*
	determine, the number of quadlets in a packet payload based on tCode and PBHeader
*/
HRESULT lhlGetPayloadSize(uint16 tCode, uint16 headerNumQuads, QUADLET *qHeader, uint16 *payloadNumBytes)
{
	HRESULT	hResult = NO_ERROR;

	*payloadNumBytes = 0;

	// get payload size based on tcode
	hResult = lhlGetPayloadSizeFromTCode(tCode, payloadNumBytes);
	if (hResult != NO_ERROR)
	{
		return hResult;
	}

	// Get payload size in case we didn't have enough info earlier
	if (*payloadNumBytes == LHL_UNKNOWN_DATA_LEN) 
	{
		hResult = lhlLLCGetPayloadSize(tCode, headerNumQuads, qHeader, payloadNumBytes);
	}

	return hResult;
}

/*
	determine, when possible, the number of quadlets in a packet payload based on tCode
*/
HRESULT lhlGetPayloadSizeFromTCode(uint16 tCode, uint16 *payloadNumBytes)
{
	HRESULT hResult = NO_ERROR;

	*payloadNumBytes = 0;

	switch (tCode)
	{
		case TCODE_WRRESPONSE:
		case TCODE_RDQUAD:
		case TCODE_RDBLOCK:
		case TCODE_PHY_PACKET:
		case TCODE_CYCLESTART:
			*payloadNumBytes = 0;
			break;

		case TCODE_RDQUADRSP:
		case TCODE_WRQUAD:
			*payloadNumBytes = 4;
			break;

		case TCODE_WRBLOCK:
		case TCODE_LOCKRQST:
		case TCODE_RDBLOCKRSP:
		case TCODE_LOCKRSP:
		case TCODE_ISOBLOCK:
			*payloadNumBytes = LHL_UNKNOWN_DATA_LEN;	// unknown from tCode alone
			break;

		default:
			hResult = E_LHL_UNSUPPORTED_TCODE;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

HRESULT lhlCheckPacketType(uint32 genType, PB_PACKETTYPE packetType)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bPacketTypeOK = FALSE;

	switch (genType)
	{
		case LHL_REQ_READ:
			if (packetType == PB_TYPE_READ_REQUEST_QUADLET || 
				packetType == PB_TYPE_READ_REQUEST ||
				packetType == PB_TYPE_READ_RESPONSE_QUADLET ||
				packetType == PB_TYPE_READ_RESPONSE)
			{
				bPacketTypeOK = TRUE;
			}
			break;

		case LHL_REQ_WRITE:
			if (packetType == PB_TYPE_WRITE_REQUEST_QUADLET || 
				packetType == PB_TYPE_WRITE_REQUEST ||
				packetType == PB_TYPE_WRITE_RESPONSE ||
				packetType == PB_TYPE_WRITE_REQUEST_GENERIC)
			{
				bPacketTypeOK = TRUE;
			}
			break;

		case LHL_REQ_LOCK:
			if (packetType == PB_TYPE_LOCK_REQUEST || 
				packetType == PB_TYPE_LOCK_RESPONSE)
			{
				bPacketTypeOK = TRUE;
			}
			break;
#ifdef _LOOSE_ISO
		case LHL_REQ_STREAM:
			if (packetType == PB_TYPE_ASYNC_STREAM)
			{
				bPacketTypeOK = TRUE;
			}
			break;
#endif //_LOOSE_ISO

		case LHL_RSP_READ:
			if (packetType == PB_TYPE_READ_RESPONSE_QUADLET || 
				packetType == PB_TYPE_READ_RESPONSE)
			{
				bPacketTypeOK = TRUE;
			}
			break;

		case LHL_RSP_WRITE:
			if (packetType == PB_TYPE_WRITE_RESPONSE)
			{
				bPacketTypeOK = TRUE;
			}
			break;

		case LHL_RSP_LOCK:
			if (packetType == PB_TYPE_LOCK_RESPONSE)
			{
				bPacketTypeOK = TRUE;
			}
			break;

		case LHL_RSP_ERROR:
			if (packetType == PB_TYPE_READ_RESPONSE_QUADLET || 
				packetType == PB_TYPE_READ_RESPONSE || 
				packetType == PB_TYPE_WRITE_RESPONSE || 
				packetType == PB_TYPE_LOCK_RESPONSE)
			{
				bPacketTypeOK = TRUE;
			}
			break;
	}

	if (bPacketTypeOK == FALSE)
	{
		hResult = E_LHL_WRONG_PACKET_TYPE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT lhlGetRequestPacketType(uint32 genType, PB_PACKETTYPE packetTypeOrig, PB_PACKETTYPE *packetType)
{
	HRESULT		hResult = NO_ERROR;

	*packetType = PB_TYPE_UNDEF;

	switch (packetTypeOrig)
	{
		case PB_TYPE_READ_REQUEST_QUADLET:
		case PB_TYPE_READ_RESPONSE_QUADLET:
			*packetType = PB_TYPE_READ_REQUEST_QUADLET;
			break;

		case PB_TYPE_READ_REQUEST:
		case PB_TYPE_READ_RESPONSE:
			*packetType = PB_TYPE_READ_REQUEST;
			break;

		case PB_TYPE_WRITE_REQUEST_QUADLET:
		case PB_TYPE_WRITE_REQUEST:
		case PB_TYPE_WRITE_RESPONSE:
			*packetType = PB_TYPE_WRITE_REQUEST_GENERIC;
			break;

		case PB_TYPE_LOCK_REQUEST:
		case PB_TYPE_LOCK_RESPONSE:
			*packetType = PB_TYPE_LOCK_REQUEST;
			break;

#ifdef _LOOSE_ISO
		case PB_TYPE_ASYNC_STREAM:
			*packetType = PB_TYPE_ASYNC_STREAM;
			break;
#endif //_LOOSE_ISO
	}

	if (*packetType == PB_TYPE_UNDEF)
	{
		hResult = E_LHL_WRONG_PACKET_TYPE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lhlCheckPacketType(genType, *packetType);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlGetResponsePacketType(uint32 genType, PB_PACKETTYPE packetTypeOrig, PB_PACKETTYPE *packetType)
{
	HRESULT		hResult = NO_ERROR;

	*packetType = PB_TYPE_UNDEF;

	switch (packetTypeOrig)
	{
		case PB_TYPE_READ_REQUEST_QUADLET:
		case PB_TYPE_READ_RESPONSE_QUADLET:
			*packetType = PB_TYPE_READ_RESPONSE_QUADLET;
			break;

		case PB_TYPE_READ_REQUEST:
		case PB_TYPE_READ_RESPONSE:
			*packetType = PB_TYPE_READ_RESPONSE;
			break;

		case PB_TYPE_WRITE_REQUEST_QUADLET:
		case PB_TYPE_WRITE_REQUEST:
		case PB_TYPE_WRITE_RESPONSE:
			*packetType = PB_TYPE_WRITE_RESPONSE;
			break;

		case PB_TYPE_LOCK_REQUEST:
		case PB_TYPE_LOCK_RESPONSE:
			*packetType = PB_TYPE_LOCK_RESPONSE;
			break;
	}

	if (*packetType == PB_TYPE_UNDEF)
	{
		hResult = E_LHL_WRONG_PACKET_TYPE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lhlCheckPacketType(genType, *packetType);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

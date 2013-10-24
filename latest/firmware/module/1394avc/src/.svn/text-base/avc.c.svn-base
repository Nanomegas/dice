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

 DESCRIPTION:

	AV/C command/response handler

	Interfaces the AVC layer to the FCP layer.
	
  Revisions:
		created 21/03/2003 KBJ/LM

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "packetblock.h"
#include "fcp.h"
#include "lal.h"
#include "avc.h"
#include "avcDefs.h"
#include "datastream.h"
#include "rangemaps.h"
#include "aqm.h"
#include "cr.h"
#include "avcCmdCli.h"
#include "avcUnit.h"
#include "avcUnitCli.h"
#include "avcUnitReserve.h"
#include "systemCallbacks.h"
#ifdef _SYSDEBUG
#include "sysDebug.h"
#endif //_SYSDEBUG

// msgQueue IDs for this module											
uint32					avcHandlerQueueID;									// message queue for avc handler 
																		
// task (thread) IDs for this module									
uint32					avcHandlerThreadID;									// avc handler thread
const char*				avcHandlerThreadName = "avcHandlerThread";
																							
#define	AVC_QUEUE_SIZE	512													// number of queue elements * sizeof(quadlet)
BYTE					avcHandlerQueueMemory[AVC_QUEUE_SIZE];				// the queue memory space

static RANGEMAP avcDescriptorOpcodesRangeMap[] =
{
	{AVC_CMD_CREATE_DESCRIPTOR,RMEVAL_ONLY,0},
	{AVC_CMD_OPEN_DESCRIPTOR,RMEVAL_ONLY,0},
	{AVC_CMD_READ_DESCRIPTOR,RMEVAL_ONLY,0},
	{AVC_CMD_WRITE_DESCRIPTOR,RMEVAL_ONLY,0},
	{AVC_CMD_OPEN_INFOBLOCK,RMEVAL_ONLY,0},
	{AVC_CMD_READ_INFOBLOCK,RMEVAL_ONLY,0},
	{AVC_CMD_WRITE_INFOBLOCK,RMEVAL_ONLY,0},
	{AVC_CMD_SEARCH_DESCRIPTOR,RMEVAL_ONLY,0},
	{AVC_CMD_OBJECT_NUMBER_SELECT,RMEVAL_ONLY,0},
	{0,RMEVAL_END,0}														// All maps must be terminated in this fashion
};

HRESULT avcPadToNextQuadletInStream(pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	uint32			remainder = 0;
	uint32			payloadsize = 0;

	hResult = dsSwitchMode(pStream, dsMODE_WRITE);
	if (hResult != NO_ERROR) return hResult;

	payloadsize = dsGetPosition(pStream);
	
	remainder = payloadsize % 4;
	if (remainder)
	{
#if 0 //LM???
		dsWriteBits(pStream, 8 * (4 - remainder), 0xFFFFFFFF);
#else
		dsWriteBits(pStream, 8 * (4 - remainder), 0);
#endif
		payloadsize += (4 - remainder);
	}

	return hResult;
}
	
HRESULT avcPadToNextQuadlet(PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	pDataStream		pStream = NULL;

	hResult = pbGetApplicationDatastream(packetBlock, &pStream);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcPadToNextQuadletInStream(pStream);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
	
HRESULT avcSendRequest(uint32 ctype, PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;	

	// pad the data to the next full 32bit boundary. This is a requirement for AV/C Packets.
	// stream position not always at the end - avcPadToNextQuadlet(packetBlock);

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
		{
			if (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RAW))
			{
				avcUnitCliPrintPacketBlockRawData(ctype, packetBlock);
			}
		}
#endif //_SYSDEBUG
#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
	{
		if ((avcCtypeIsResponse(ctype) &&	// response
			((avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RESPONSE_ALL)) ||
			 (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RESPONSE_IMPLEMENTED) && ctype != AVC_RESPONSE_NOT_IMPLEMENTED) ||
			 (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RESPONSE_NOT_IMPLEMENTED) && ctype == AVC_RESPONSE_NOT_IMPLEMENTED))) ||
			(avcCtypeIsCommand(ctype) &&	// commands
			(avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_COMMAND_ALL))))
		{
			avcUnitCliPrintPacketBlock(packetBlock);
		}
	}
#endif //_SYSDEBUG

	hResult = fcpSend(packetBlock, avcCtypeIsResponse(ctype));

	return hResult;
}

HRESULT avcBlockingSendRequest(NODEHANDLE hHandle, uint8 ctype, uint16 byteLength, uint32 *bufPtr)
{
	HRESULT		hResult = NO_ERROR;	

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
		{
			if (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RAW))
			{
				avcUnitCliPrintRawData(ctype, byteLength, bufPtr);
			}
		}
#endif //_SYSDEBUG
#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
	{
		if ((avcCtypeIsResponse(ctype) &&	// response
			((avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RESPONSE_ALL)) ||
			 (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RESPONSE_IMPLEMENTED) && ctype != AVC_RESPONSE_NOT_IMPLEMENTED) ||
			 (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_RESPONSE_NOT_IMPLEMENTED) && ctype == AVC_RESPONSE_NOT_IMPLEMENTED))) ||
			(avcCtypeIsCommand(ctype) &&	// commands
			(avcUnitCliIsPrintMode(AVC_UNIT_PRINT_SND_COMMAND_ALL))))
		{
			DataStream  ds;
		
			hResult = dsOpenStream(&ds, bufPtr, byteLength, dsMODE_READ);
			if (hResult == NO_ERROR)
			{
				avcUnitCliPrintDataStream(&ds);
			}
		}
	}
#endif //_SYSDEBUG

	hResult = fcpBlockingSend(hHandle, byteLength, bufPtr, avcCtypeIsResponse(ctype));

	return hResult;
}

HRESULT avcCreateResponsePacket(PB *packetBlock, PB **respPacket, BOOL bNewPayloadSize, uint32 payloadSize)
{
	HRESULT		hResult = NO_ERROR;	

	if (bNewPayloadSize)
	{
		hResult = pbCreateDuplicatePacketWithSize(packetBlock, respPacket, (uint16) payloadSize, NULL, PB_CREATE_AVC_SEND_RSP);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		hResult = pbCreateDuplicatePacket(packetBlock, respPacket, NULL, PB_CREATE_AVC_SEND_RSP);
		if (hResult != NO_ERROR) return hResult;
	}

#if 1 
	pbSwapSrcDstNodeAddr(*respPacket);
#endif

	return hResult;
}

void avcDestroyResponsePacket(PB *packetBlock)
{
	// clean up the resp packetBlock
	if (pbIsValid(packetBlock))
	{
		if (pbIsNonBlocking(packetBlock) == FALSE)
		{
			packetBlock->creatorCBProc = NULL;
			pbPacketDone(packetBlock, PB_DONE_AVC_SEND_RSP);
		}
	}
}

HRESULT avcSendResponse(uint32 response, PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;	
	BOOL		bNonBlocking = TRUE;

	pbSetNonBlocking(packetBlock, bNonBlocking);

	// send the new PB
	hResult = avcSendRequest(response, packetBlock);

	if (bNonBlocking == FALSE)
	{
		// clean up the packetBlock
		avcDestroyResponsePacket(packetBlock);
	}

	return hResult;
}

HRESULT avcSendResponsePacket(uint32 response, PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;	
	pDataStream	pStream = NULL;

	hResult = pbGetApplicationDatastream(packetBlock, &pStream);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsReOpenStream(pStream, dsMODE_WRITE);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(pStream, (uint8) response);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcSendResponse(response, packetBlock);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcReplyResponse(uint32 response, PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;	
	PB*			respPacket;

	hResult = avcCreateResponsePacket(packetBlock, &respPacket, FALSE, 0);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcSendResponsePacket(response, respPacket);

	return hResult;
}

// soft glue for calling the first descriptor handler only
HRESULT	avcCallDescriptorHandler(AVC_HEADER *pHeader, PB* packetBlock)
{
	HRESULT							hResult = NO_ERROR;	
	CALLBACK_DESCRIPTOR*			cb = NULL;
	AVC_DESCRIPTORHANDLER_CALLBACK	callback = NULL;

	hResult = crGetCallback(AVC_DESCRIPTORHANDLER_TYPE, 0, &cb);
	if (hResult != NO_ERROR) return E_PKT_AVC_NOT_IMPLEMENTED;

	hResult = kvGetValue(cb, AVC_DESCRIPTORHANDLER_CALLBACK_KEY, (uint32 *) &callback);
	if (hResult != NO_ERROR) return E_PKT_AVC_NOT_IMPLEMENTED;

	// found the appropriate callback so let's make it.
	hResult = (callback) (pHeader, packetBlock);
	return hResult;
}

HRESULT avcHandleCallback(AVC_HEADER *pHeader, PB *packetBlock)
{
	HRESULT	 				hResult = NO_ERROR;
	uint32	 				index = 0;
	uint32					subunitidcounter = 0;
	CALLBACK_DESCRIPTOR*	cb = NULL;
	AVC_SUBUNIT_CALLBACK	callback = NULL;
	BOOL					bFound = FALSE;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
	{
		if (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_RAW))
		{
			avcUnitCliPrintPacketBlockRawData(pHeader->ctype, packetBlock);
		}
	}
#endif //_SYSDEBUG

	DO_FOREVER
	{
		hResult = crGetCallback(AVC_SUBUNIT_TYPE, index++, &cb);
		if (hResult != NO_ERROR) break;

		hResult = kvIsValue(cb, AVC_SUBUNIT_SUBUNITTYPE_KEY, pHeader->addrSubunit.subunit_type);
		if (hResult != NO_ERROR) continue;
		
		hResult = kvGetValue(cb, AVC_SUBUNIT_ID_KEY, &subunitidcounter);
		if (hResult != NO_ERROR) continue;

		if (subunitidcounter != AVC_SU_ID_IGNORE &&
			subunitidcounter != pHeader->addrSubunit.subunit_ID) continue;
			
		hResult = kvGetValue(cb, AVC_SUBUNIT_CALLBACK_KEY, (uint32 *) &callback);
		if (hResult != NO_ERROR) break;

		// found the appropriate callback so let's make it.
		hResult = (callback) (pHeader, packetBlock);
		bFound = TRUE;
		break;
	}

	if (bFound == FALSE)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_AVC | SYSDEBUG_TRACE_ERRORS, 
				  "avcHandleCallback: Could not find callback for registered subunittype\n\r");
#ifdef _SYSDEBUG
		if (avcCtypeIsResponse(pHeader->ctype))
		{
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				sysDebugPrintf("avcHandleCallback: Response\n\r");
				avcUnitCliPrintHeader(pHeader);
			}
		}
#endif //_SYSDEBUG
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT avcHandlePacketBlock(PB* packetBlock)
{
	HRESULT			hResult = NO_ERROR;	
	AVC_HEADER		avcHeader;
	uint32			response = 0;
	pDataStream		pStream = NULL;
	BOOL			bReceivedResponse = FALSE;
	BOOL			bSendResponse = FALSE;

	hResult = pbGetApplicationDatastream(packetBlock, &pStream);
	if (hResult != NO_ERROR) return hResult;

	// Log the AV/C message in
	hResult = avcDecodeHeader(pStream, &avcHeader);

	if (hResult != NO_ERROR)								// there is some fault with the packet
	{
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;
	}

	bReceivedResponse = avcCtypeIsResponse(avcHeader.ctype);

	if (hResult == NO_ERROR)
	{
		hResult = avcUnitReserveCheck(&avcHeader, packetBlock);	// check if (sub-) unit is reserved etc.
	}

	if (hResult == NO_ERROR)
	{
		if (rmInMap(avcHeader.opcode, avcDescriptorOpcodesRangeMap))
		{
			// pass descriptor command off to the DescriptorManager (includes responses from our descriptor reads)
			hResult = avcCallDescriptorHandler(&avcHeader, packetBlock);
		} 
		else
		{
			//	command is not mapped as a descriptor command so now we want to handle this normally
			hResult = avcHandleCallback(&avcHeader, packetBlock);
		}
	}

	if (hResult != NO_ERROR)
	{
		// need to take care of the response here

		// verify packet ctypes for response codes

		if (bReceivedResponse == FALSE)
		{
			bSendResponse = TRUE;
			switch (hResult)
			{
				case E_PKT_AVC_ACCEPTED:		response = AVC_RESPONSE_ACCEPTED;			break;
				case E_PKT_AVC_IMPLEMENTED:		response = AVC_RESPONSE_IMPLEMENTED;		break;	// command is supported - send IMPLEMENTED response
				case E_PKT_AVC_SUBUNIT_BUSY:	response = AVC_RESPONSE_REJECTED;			break;	// subunit cannot accept packet at this time - send REJECTED response
				case E_PKT_AVC_REJECTED:		response = AVC_RESPONSE_REJECTED;			break;	// send REJECTED response
				case E_PKT_AVC_NOT_IMPLEMENTED:	response = AVC_RESPONSE_NOT_IMPLEMENTED;	break;	// send NOT IMPLEMENTED response
				case E_PKT_AVC_INTERIM:			response = AVC_RESPONSE_INTERIM;			break;	// send INTERIM response
				case E_PKT_AVC_STABLE:			response = AVC_RESPONSE_STABLE;				break;	// send STABLE response
				default:						bSendResponse = FALSE;						break;
			}

			if (pbPacketIsBroadcast(packetBlock) == TRUE)
			{
				if (response == AVC_RESPONSE_NOT_IMPLEMENTED)
				{
					bSendResponse = FALSE;
				}
			}

			if (bSendResponse)
			{
				avcReplyResponse(response, packetBlock);
			}
		}
	}

	return hResult;
}

#define _USE_AVC_HANDLER_THREAD
//#undef _USE_AVC_HANDLER_THREAD // testing

// avcCallback is called by the transport layer when cts code FCP_CTS_CODE_AVC is written to the FCP registers.
// This function places a message in the queue of the avc handler thread (or handles the message itself)
// This function is registered with the transport layer by the avc handler thread.
HRESULT avcCallback(PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;	

#ifdef _USE_AVC_HANDLER_THREAD
	pbSetSelfDestruct(packetBlock, TRUE);
	hResult = aqmQueueSend(avcHandlerQueueID, (void *) &packetBlock, TC_NO_WAIT);
#else //_USE_AVC_HANDLER_THREAD
	hResult = avcHandlePacketBlock(packetBlock);
#endif //_USE_AVC_HANDLER_THREAD

	return hResult;
}

void avcHandlerThread(void *dummy)
{
	HRESULT				hResult = NO_ERROR;	
	PB*					packetBlock;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		// get a valid PB from the queue
		hResult = aqmQueueReceive(avcHandlerQueueID, &packetBlock, TC_WAIT_FOREVER);
		
		if (hResult == NO_ERROR)
		{
			hResult = avcHandlePacketBlock(packetBlock);

			if (pbIsSelfDestruct(packetBlock) == TRUE)
			{
				pbPacketDoneSelfDestruct(packetBlock, PB_DONE_AVC_HANDLE_THREAD);
			}
		}
	}
}

/*********************************************************
	Create message queues for this module
*/
HRESULT avcCreateMessageQueues(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = aqmQueueOpen(	&avcHandlerQueueID,			// the queue id
							1,							// size of queue element = 1 quadlet
							&avcHandlerQueueMemory,		// the memory to use
							AVC_QUEUE_SIZE);			// how big the queue is (in bytes)
	
	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT avcResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc handler thread
	hResult = TCTaskResume(avcHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	// avc unit threads
	hResult = avcUnitResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avcSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc handler thread
	hResult = TCTaskSuspend(avcHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	// avc unit threads
	hResult = avcUnitSuspendTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT avcInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc handler thread
	hResult = TCTaskCreate(&avcHandlerThreadID, avcHandlerThreadName, avcHandlerThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	// avc unit threads
	hResult = avcUnitInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	// initialize callback registry module
	hResult = crInitialize();
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCreateMessageQueues();
	if (hResult != NO_ERROR) return hResult;

	hResult = avcResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	hResult = fcpRegisterCallback(FCP_CTS_CODE_AVC, &avcCallback);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcUnitInitialize();
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_TOOL_AVC_CMD
	avcCmdCliInstallTools();
#endif //_CLI_TOOL_AVC_CMD
#ifdef _CLI_TOOL_AVC_UNIT
	avcUnitCliInstallTools();
#endif //_CLI_TOOL_AVC_UNIT

	return hResult;
}

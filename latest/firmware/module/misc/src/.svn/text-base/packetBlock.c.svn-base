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

	Purpose:	Packet Block utilities
	
				Packet blocks are a key data element in the firewire system.  This file 
				provides services to applications for creating, deleting and accessing
				data within packet blocks.

	Revisions:
		created 03/20/2002 LM/KBJ

*****************************************************************************/

#ifdef _1394CORE

#include "TCTasking.h"
#include "1394.h"
#include "ErrorCodes.h"
#include "packetBlock.h"
#include "mpm.h"
#include "bri.h"
#include "lhlStatistics.h"
#include "packetBlockCli.h"

static char	moduleName[] = __FILE__;

static uint32 pbMutexSemID; // mutual exclusion semaphore for freeing packetBlocks (mutex)

// sequence number, sequentially assigned to packet blocks
static uint32	nextSeqNum = 1;

uint32 pbAllocBlock = 0;
uint32 pbAllocPayload = 0;

// Initialize this module
HRESULT	pbInitialize (void)
{
	HRESULT hResult = NO_ERROR;

	// exclusive access for freeing packetBlocks (mutex)
	hResult = TCMutexOpen(&pbMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = mpmInitialize();

#ifdef _CLI_TOOL_PB
	pbCliInstallTools();
#endif //_CLI_TOOL_PB

	return hResult;
}

/***************************************************************************
	
	Local Helpers

***************************************************************************/
static HRESULT freePacketMemory(PB *pPB)
{
	HRESULT hResult = NO_ERROR;
	HRESULT hResultRelease = NO_ERROR;

	if (pPB == NULL)
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (pPB->pPayload == NULL)
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Exclusive access for freeing packetBlocks (mutex)
	hResult = TCMutexLock(pbMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (pbIsValid(pPB))
	{
		// We have exclusive rights to delete the packet from the memory pool.

		pPB->magicNumberFirst = PB_MAGIC_DELETED;
		pPB->magicNumberLast = PB_MAGIC_DELETED;
		
		pbAllocBlock--;
#ifdef _STATISTICS
		lhlStatistics.pbAllocBlock = pbAllocBlock;
#endif //_STATISTICS

		// payload
		if (pPB->bPayloadReferenced == FALSE)
		{
			// if payload is not contained within the packet block, free payload memory first
			if (pPB->pPayload != (uint8*)(pPB->localPayload))
			{
				pbAllocPayload--;
#ifdef _STATISTICS
				lhlStatistics.pbAllocPayload = pbAllocPayload;
#endif //_STATISTICS
				hResult = mpmBlockRelease(pPB->pPayload);
			}
		}

		hResultRelease = mpmBlockRelease(pPB);

		if (hResult == NO_ERROR)
		{
			hResult = hResultRelease;
		}
	}
	
	// Release exclusive for freeing packetBlocks (mutex)
	TCMutexUnlock(pbMutexSemID);

	return hResult;
}

/***************************************************************************
	
	Packet Block control functions

***************************************************************************/

/* Packet creation */

HRESULT	pbCreatePacket (
				PB			**pPB,				// pointer where pointer to block will be stored if NO_ERROR
				NODEHANDLE	nodeHandle,			// source or destination (incoming or outgoing respectively)
				PB_CALLBACK	callback,			// done callback, NULL if you don't want to be called back
				uint32		numPayloadBytes, 	// how large a payload would you like?
				uint32		callerId)
{
	return pbCreatePacketOptimize (pPB, nodeHandle, callback, numPayloadBytes, callerId, FALSE, NULL);
}

HRESULT	pbCreatePacketOptimize (
				PB			**pPB,				// pointer where pointer to block will be stored if NO_ERROR
				NODEHANDLE	nodeHandle,			// source or destination (incoming or outgoing respectively)
				PB_CALLBACK	callback,			// done callback, NULL if you don't want to be called back
				uint32		numPayloadBytes, 	// how large a payload would you like?
				uint32		callerId,
				BOOL		bPayloadReferenced,
				uint8		*pData)
{
	HRESULT	hResult = NO_ERROR;
	uint32	numPayloadBytesRequired;
	PB		*pTemp;
	uint32	busGeneration;

	*pPB = NULL;

	/* Round payload size up to even number of quadlets */
	numPayloadBytesRequired = (uint32) ((numPayloadBytes + 3) & (~3));

	/* Get some memory for the packet block */
	hResult = mpmBlockAllocate(sizeof(PB), (void **) &pTemp, MPM_CREATOR_ID_PB, FALSE);

	if (pTemp == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (hResult == NO_ERROR)
	{
		pbAllocBlock++;
#ifdef _STATISTICS
		lhlStatistics.pbAllocBlock = pbAllocBlock;
#endif //_STATISTICS

		/* initialize the packet block internal data -
			Many fields need to be zero'd, especially callbacks.
			For now, set the entire block to zero. */

		memset(pTemp, 0, sizeof(PB));

		briGetCurrentBusGeneration (&busGeneration);
		
		// set defined fields
		pTemp->magicNumberFirst = PB_MAGIC_NUMBER;
		pTemp->magicNumberLast = PB_MAGIC_NUMBER;
		pTemp->packetSeqNum = nextSeqNum++;
		pTemp->busGeneration = busGeneration;
		pTemp->nodeHandle = nodeHandle;
		pTemp->creatorCBProc = callback;
		pTemp->finalStatus = NO_ERROR;		// default
		pTemp->payloadByteSize = numPayloadBytes;
		pTemp->pCreateAddr = pPB;
		pTemp->callerIdCreate = callerId;
		pTemp->bPayloadReferenced = bPayloadReferenced;
		pTemp->bNonBlocking = FALSE;
		pTemp->bSelfDestruct = FALSE;
		pTemp->bSelfDestructCount = 0;

		// pTemp->callerIdDoneIndex initialized by memset
		// pTemp->callerIdDone[]	initialized by memset

		//  Payload
		if (bPayloadReferenced == TRUE)	// payload data only referenced
		{
			pTemp->pPayload = pData;
		}
		else
		{
			if (numPayloadBytesRequired <= sizeof(pTemp->localPayload))
			{
				// use the area within packet block if it fits
				pTemp->pPayload = (uint8 *) (&pTemp->localPayload[0]);
			}
			else
			{
				// need separate allocation for the payload
				hResult = mpmBlockAllocate(numPayloadBytesRequired, (void **) &pTemp->pPayload, MPM_CREATOR_ID_PB, FALSE);
				if (hResult == NO_ERROR)
				{
					pbAllocPayload++;
#ifdef _STATISTICS
					lhlStatistics.pbAllocPayload = pbAllocPayload;
#endif //_STATISTICS
#if 0
					// initialize the packet block payload data to zero
					memset(pTemp->pPayload, 0, numPayloadBytesRequired);
#endif
				}
				else
				{
					// give back the packet block if we can't get memory for payload
					pTemp->pPayload = (uint8*)(&pTemp->localPayload[0]);
					freePacketMemory(pTemp);
				}
			}
		}
#ifdef _DATASTREAM
		if (hResult == NO_ERROR)
		{
			hResult = dsOpenStream(&pTemp->applicationDataStream, pTemp->pPayload, numPayloadBytesRequired, dsMODE_READ);
			if (hResult != NO_ERROR)
			{
				freePacketMemory(pTemp);
			}
		}
#endif //_DATASTREAM
	}

	if (hResult == NO_ERROR)
	{
		*pPB = pTemp;
	}
	else
	{
		*pPB = (PB *) NULL;
	}	
	return hResult;
}	

/*********************************************************
	Packet done 
		Caller is passing us a packet which it has completed its handling of.
		We must either pass the packet to another module (via callback) or
		delete the packet block.
*/

void pbPacketDone (PB *pPB, uint32 callerId)  		// This function always succeeds.
{
	HRESULT		hResult = NO_ERROR;
	PB_CALLBACK	savedCB;

#if 0 //def _DEBUG
	if (pPB)
	{
		if (pPB->callerIdDoneIndex < PB_DONE_CALLER_INDEX_ITEMS)
		{
			pPB->callerIdDone[pPB->callerIdDoneIndex] = callerId;
			pPB->callerIdDoneIndex++;
		}
		else
		{
			hResult = E_PB_TRAP_LIST_FULL;
			sysLogError(hResult, __LINE__, moduleName);
		}
	}
#else
	UNUSED_ARG(callerId);
#endif

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return;
	}

	// check the creator callback
	if (pPB->creatorCBProc)
	{
		savedCB = pPB->creatorCBProc;
		pPB->creatorCBProc = NULL;
		hResult = (savedCB)(pPB);
		if (hResult == NO_ERROR)
		{
			return;		// callback now 'has' the packet
		}
	}

	freePacketMemory(pPB);
}	

/*********************************************************
	Packet duplication 
		This would typically be used to create a response packet based on a command packet.
		The following fields are copied into the newly created packet:
			nodeHandle
			payloadByteSize
			payloadData
		All other fields are initialized as created by pbCreatePacket.
		Returns:
			NO_ERROR - new packet has been created, caller now 'has' 2 packets, original and new
			Any other value, no new packet has been created, caller still 'has' the original packet 
*/
HRESULT	pbCreateDuplicatePacket (
				PB			*pPB,				// packet block to duplicate	
				PB			**pDuplicatePB,		// pointer where pointer to new PB will be stored if NO_ERROR
				PB_CALLBACK	callback, 			// done callback, NULL if you don't want to be called back
				uint32		callerId)
{
	HRESULT			hResult = NO_ERROR;

	*pDuplicatePB = NULL;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = pbCreatePacket (
					pDuplicatePB,
					pPB->nodeHandle, 
					callback,
					pPB->payloadByteSize,
					callerId);

	if (hResult == NO_ERROR)
	{
#if 1 //LM???
		memcpy_uint32 ((uint32 *) &((*pDuplicatePB)->packetHeader), (uint32 *) &pPB->packetHeader, sizeof((*pDuplicatePB)->packetHeader) / 4);
#else
		memcpy (&((*pDuplicatePB)->packetHeader), &pPB->packetHeader, sizeof((*pDuplicatePB)->packetHeader));
#endif		
#if 1 //LM???
#if 0 //LM??? shouldn't be needed
		if (((*pDuplicatePB)->payloadByteSize % 4) != 0)
		{
			hResult = E_PB_DIFFERENT_PAYLOAD_SIZES;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
#endif		
		memcpy_uint32 ((uint32 *) (*pDuplicatePB)->pPayload, (uint32 *) pPB->pPayload, (*pDuplicatePB)->payloadByteSize / 4);
#else
		memcpy ((*pDuplicatePB)->pPayload, pPB->pPayload, (*pDuplicatePB)->payloadByteSize);
#endif		
		
		(*pDuplicatePB)->packetParams.speed = pPB->packetParams.speed;
		(*pDuplicatePB)->packetParams.queueType = pPB->packetParams.queueType;
		(*pDuplicatePB)->packetType = pPB->packetType;

	}
	else
	{
		*pDuplicatePB = NULL;
	}

	return hResult;
}	

/*********************************************************
	Packet duplication, with size parameter
		This would typically be used to create a response packet based on a command packet.
		The following fields are copied into the newly created packet:
			nodeHandle
			payloadData
			packetType
		Note: payload data is copied from the original packet to the new packet, but only the 
		first N bytes will be copied, where N is the smaller of the 2 payload sizes.

		All other fields are initialized as created by pbCreatePacket.
		Returns:
			NO_ERROR - new packet has been created, caller now 'has' 2 packets, original and new
			Any other value, no new packet has been created, caller still 'has' the original packet 
*/
HRESULT	pbCreateDuplicatePacketWithSize (
				PB			*pPB,				// packet block to duplicate	
				PB			**pDuplicatePB,		// pointer where pointer to new PB will be stored if NO_ERROR
				uint16		payloadBytes,		// payload size for duplicate pb
				PB_CALLBACK	callback, 			// done callback, NULL if you don't want to be called back
				uint32		callerId)
{
	HRESULT			hResult = NO_ERROR;
	uint32			payloadByteCopySize;

	*pDuplicatePB = NULL;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = pbCreatePacket (
					pDuplicatePB,
					pPB->nodeHandle, 
					callback,
					payloadBytes,
					callerId);

	if (hResult == NO_ERROR)
	{
#if 1 //LM???
		memcpy_uint32 ((uint32 *) &((*pDuplicatePB)->packetHeader), (uint32 *) &pPB->packetHeader, sizeof((*pDuplicatePB)->packetHeader) / 4);
#else
		memcpy (&((*pDuplicatePB)->packetHeader), &pPB->packetHeader, sizeof((*pDuplicatePB)->packetHeader));
#endif		
		if (payloadBytes > pPB->payloadByteSize)
		{
			payloadByteCopySize = pPB->payloadByteSize;
		}
		else
		{
			payloadByteCopySize = payloadBytes;
		}

		if (payloadByteCopySize > 0)
		{
#if 1 //LM???
#if 0 //LM??? shouldn't be needed
			if ((payloadByteCopySize % 4) != 0)
			{
				hResult = E_PB_DIFFERENT_PAYLOAD_SIZES;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
#endif		
			memcpy_uint32 ((uint32 *) (*pDuplicatePB)->pPayload, (uint32 *) pPB->pPayload, payloadByteCopySize / 4);
#else
			memcpy ((*pDuplicatePB)->pPayload, pPB->pPayload, payloadByteCopySize);
#endif		
		}
		
		(*pDuplicatePB)->packetParams.speed = pPB->packetParams.speed;
		(*pDuplicatePB)->packetParams.queueType = pPB->packetParams.queueType;
		(*pDuplicatePB)->packetType = pPB->packetType;
	}
	else
	{
		*pDuplicatePB = NULL;
	}

	return hResult;
}	

/***************************************************************************
	
	Packet Block accessor functions

***************************************************************************/

#if defined (_OPTIMIZE) && !defined (_DEBUG)
#else //_OPTIMIZE && !_DEBUG
BOOL pbIsValid(PB *pPB)
{
	return ((pPB != NULL) && (PB_MAGIC_NUMBER == pPB->magicNumberFirst) && (PB_MAGIC_NUMBER == pPB->magicNumberLast));
}

/* GetSequenceNum */
HRESULT pbGetSequenceNum (PB* pPB, uint32* pSequenceNumber)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pSequenceNumber = pPB->packetSeqNum;

	return hResult;
}	

/* GetBusGeneration */
HRESULT pbGetBusGeneration (PB* pPB, uint32* pBusGeneration)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pBusGeneration = pPB->busGeneration;

	return hResult;
}	


/* GetSenderHandle, SetNodeHandle
		Returns the packet node handle.  For incoming packets, this is the node that sent the
		packet to us.  For outgoing packets, this is the node that we want the packet sent to.
*/
HRESULT pbGetNodeHandle (PB* pPB, NODEHANDLE* pNodeHandle)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pNodeHandle = pPB->nodeHandle;

	return hResult;
}	

HRESULT pbSetNodeHandle (PB* pPB, NODEHANDLE nodeHandle)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pPB->nodeHandle = nodeHandle;

	return hResult;
}	

#ifdef _DATASTREAM
/* GetApplicationDatastream */
HRESULT	pbGetApplicationDatastream (PB *pPB, DataStream  **pStream)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pStream = &pPB->applicationDataStream;

	return hResult;
}
#endif //_DATASTREAM

/* Get/set final status - note that the 'get' function return value is not the 
		final status. */
HRESULT	pbGetFinalStatus (PB *pPB, HRESULT *pFinalStatus)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pFinalStatus = pPB->finalStatus;

#if 0 //LM??? debug
	if (*pFinalStatus!= NO_ERROR)
	{
		*pFinalStatus = pPB->finalStatus;
	}
#endif

	return hResult;
}

HRESULT	pbSetFinalStatus (PB *pPB, HRESULT finalStatus)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#if 0 //LM??? debug
	if (finalStatus != NO_ERROR)
	{
		pPB->finalStatus = finalStatus;
	}
#endif

	pPB->finalStatus = finalStatus;

	return hResult;
}

/* GetPacketParams - puts pointer to packet parameter structure into caller's pointer var */
HRESULT	pbGetPacketParams (PB *pPB, PB_PARAMS **pPacketParams)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pPacketParams =  &pPB->packetParams;

	return hResult;
}


/* getPacketQueueType - gets packet transmit queue priority */
HRESULT pbGetPacketQueueType (PB *pPB, PB_QTYPE *qType)
{
	HRESULT		hResult = NO_ERROR;
	PB_PARAMS	*pbParams;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = pbGetPacketParams(pPB, &pbParams);

	if (hResult == NO_ERROR)
	{
		*qType = pbParams->queueType;
	}

	return hResult;
}

/* setPacketQueueType - sets packet transmit queue priority */
HRESULT pbSetPacketQueueType (PB *pPB, PB_QTYPE qType)
{
	HRESULT		hResult = NO_ERROR;
	PB_PARAMS	*pbParams;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (qType > PB_Q_TYPE_IMMEDIATE)
	{
		hResult = E_PB_INVALID_Q_SPECIFIED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = pbGetPacketParams(pPB, &pbParams);

	if (hResult == NO_ERROR)
	{
		pbParams->queueType = qType;
	}

	return hResult;
}

/* GetPacketSpeed - gets signaling speed of an incoming packet */
HRESULT	pbGetPacketSpeed (PB *pPB, uint32 *speed)
{
	HRESULT		hResult = NO_ERROR;
	PB_PARAMS	*pbParams;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = pbGetPacketParams(pPB, &pbParams);

	if (hResult == NO_ERROR)
	{
		*speed = pbParams->speed;
	}

	return hResult;
}

/* SetPacketSpeed - sets signaling speed of an outgoing packet */
HRESULT	pbSetPacketSpeed (PB *pPB, uint32 speed)
{
	HRESULT		hResult = NO_ERROR;
	PB_PARAMS	*pbParams;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (speed > (SPEED_400 << SHIFT_SPEEDCODE))
	{
		hResult = E_PB_INVALID_SPEED_CODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = pbGetPacketParams(pPB, &pbParams);

	if (hResult == NO_ERROR)
	{
		pbParams->speed = speed;
	}

	return hResult;
}


/* Get packet type */
HRESULT	pbGetPacketType (PB *pPB, PB_PACKETTYPE *pPacketType)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pPacketType = pPB->packetType;

	return hResult;
}

HRESULT	pbIsResponsePacket (PB *pPB, uint8 *response)
{
	HRESULT			hResult = NO_ERROR;
	PB_PACKETTYPE	packetType;

	*response = FALSE;

	hResult = pbGetPacketType(pPB, &packetType);
	if (hResult != NO_ERROR) return hResult;

	switch (packetType) 
	{
		case PB_TYPE_WRITE_RESPONSE:
		case PB_TYPE_READ_RESPONSE:
		case PB_TYPE_READ_RESPONSE_QUADLET:
		case PB_TYPE_LOCK_RESPONSE:
			*response = TRUE;
			break;
	}

	return hResult;
}

#ifdef _LOOSE_ISO
HRESULT	pbIsAsyncStreamPacket (PB *pPB, uint8 *asyncStream)
{
	HRESULT			hResult = NO_ERROR;
	PB_PACKETTYPE	packetType;

	*asyncStream = FALSE;

	hResult = pbGetPacketType(pPB, &packetType);
	if (hResult != NO_ERROR) return hResult;

	*asyncStream = (uint8) (packetType == PB_TYPE_ASYNC_STREAM);

	return hResult;
}
#endif //_LOOSE_ISO

/* Set packet type */
HRESULT	pbSetPacketType (PB *pPB, PB_PACKETTYPE packetType)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pPB->packetType = packetType;

	return hResult;
}


/* GetPacketHeader - puts pointer to raw 1394 header into caller's pointer var */
HRESULT pbGetPacketHeader (PB* pPB, PB_HEADER **pHeader)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pHeader = &pPB->packetHeader;

	return hResult;
}

HRESULT pbGetPacketTLabel (PB* pPB, uint32 *tLabel)
{
	HRESULT		hResult = NO_ERROR;
	PB_HEADER*	pHeader = 0;

	hResult = pbGetPacketHeader (pPB, &pHeader);
	if (hResult != NO_ERROR) return hResult;

	if (pHeader == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*tLabel = pHeader->quadlets[0] & MASK_TLABEL;

	return hResult;
}

/* pbGetPacketDstNodeAddr - extract the destination nodeAddress from the packet block header */
HRESULT pbGetPacketDstNodeAddr (PB* pPB, uint32 *pNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	PB_HEADER*	pHeader = 0;

	hResult = pbGetPacketHeader (pPB, &pHeader);
	if (hResult != NO_ERROR) return hResult;

	if (pHeader == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pNodeAddr = (pHeader->quadlets[0] & MASK_NODEADDR) >> SHIFT_NODEADDR;

	return hResult;
}

HRESULT pbSetPacketDstNodeAddr (PB* pPB, uint32 nodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	PB_HEADER*	pHeader = 0;

	hResult = pbGetPacketHeader (pPB, &pHeader);
	if (hResult != NO_ERROR) return hResult;

	if (pHeader == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pHeader->quadlets[0] = (pHeader->quadlets[0] & ~MASK_NODEADDR) | ((nodeAddr << SHIFT_NODEADDR) & MASK_NODEADDR);

	return hResult;
}

/* pbGetPacketSrcNodeAddr - extract the source nodeAddress from the packet block header */
HRESULT pbGetPacketSrcNodeAddr (PB* pPB, uint32 *pNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	PB_HEADER*	pHeader = 0;

	hResult = pbGetPacketHeader (pPB, &pHeader);
	if (hResult != NO_ERROR) return hResult;

	if (pHeader == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pNodeAddr = (pHeader->quadlets[1] & MASK_NODEADDR) >> SHIFT_NODEADDR;

	return hResult;
}

HRESULT pbSetPacketSrcNodeAddr (PB* pPB, uint32 nodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	PB_HEADER*	pHeader = 0;

	hResult = pbGetPacketHeader (pPB, &pHeader);
	if (hResult != NO_ERROR) return hResult;

	if (pHeader == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pHeader->quadlets[1] = (pHeader->quadlets[1] & ~MASK_NODEADDR) | ((nodeAddr << SHIFT_NODEADDR) & MASK_NODEADDR);

	return hResult;
}

HRESULT pbSwapSrcDstNodeAddr(PB* pPB)
{
	HRESULT		hResult = NO_ERROR;
	uint32		srcNodeAddr = 0;
	uint32		dstNodeAddr = 0;

	hResult = pbGetPacketSrcNodeAddr (pPB, &srcNodeAddr);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketDstNodeAddr (pPB, &dstNodeAddr);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbSetPacketSrcNodeAddr (pPB, dstNodeAddr);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbSetPacketDstNodeAddr (pPB, srcNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/* pbGetDataLen - extract data len of payload from packet block header */
HRESULT pbGetDataLen (PB* pPB, uint32 *pDataLen)
{
	HRESULT		hResult = NO_ERROR;
	PB_HEADER*	pHeader = 0;

	hResult = pbGetPacketHeader (pPB, &pHeader);
	if (hResult != NO_ERROR) return hResult;

	if (pHeader == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pDataLen = (pHeader->quadlets[3] & MASK_DATA_LEN) >> SHIFT_DATA_LEN;

	return NO_ERROR;
}

/* GetPayloadSize - puts size in BYTES into caller's var */
HRESULT pbGetPayloadSize (PB* pPB, uint32 *pPayloadSizeInBytes)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pPayloadSizeInBytes = pPB->payloadByteSize;

	return hResult;
}

/* SetPayloadSize - puts size in BYTES into packetBlock */
HRESULT pbSetPayloadSize (PB *pPB, uint32 payloadSizeInBytes)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pPB->payloadByteSize = payloadSizeInBytes;

	return hResult;
}

/* GetPayload - puts pointer to payload into caller's pointer var */
HRESULT pbGetPayload (PB* pPB, void **pPayload)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pPayload = pPB->pPayload;

	return hResult;
}

/***************************************************************************
	
	Packet Block miscellaneous functions


***************************************************************************/


/* PacketIsBroadcast - returns true if node id is BROADCAST_NODE_ID (0x3f=63). */

BOOL	pbPacketIsBroadcast (PB* pPB)
{
	HRESULT		hResult = NO_ERROR;
	PB_HEADER	*pHeader = NULL;
	uint32		nodeId;
	
	hResult = pbGetPacketHeader(pPB, &pHeader);
	if (hResult != NO_ERROR) return FALSE;

	nodeId = (pHeader->quadlets[0] >> SHIFT_NODEID) & MASK_NODEID;

	return (nodeId == BROADCAST_NODE_ID);
}	

HRESULT pbGetLockType(PB *pPB, PB_LOCKTYPE *pLockType)
{
	HRESULT		hResult = NO_ERROR;
#if 1 
	*pLockType = PB_LOCKTYPE_NONE;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (pPB->packetType == PB_TYPE_LOCK_REQUEST)
	{
		*pLockType = (PB_LOCKTYPE)((pPB->packetHeader.quadlets[3] & 0x0000FFFF));
	}
#else
	uint32		extendedTcode;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (pPB->packetType != PB_TYPE_LOCK_REQUEST)
	{
		hResult = E_LOCK_TRANSACTION_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	extendedTcode = (pPB->packetHeader.quad[3] & 0x0000FFFF);

	// check range of extended T code field

	if ((extendedTcode == 0) || (extendedTcode > 7))
	{
		hResult = E_LOCK_TYPE_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	*pLockType = (PB_LOCKTYPE)(extendedTcode);
#endif

	return hResult;
}

HRESULT pbGetDestinationOffset(PB *pPB, OFFSET_1394 *offset)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	offset->High = (uint16) ((pPB->packetHeader.quadlets[1]) & 0xFFFF);  // least two significant bytes of 2nd quadlet
	offset->Low  = (pPB->packetHeader.quadlets[2]);             // 3rd quadlet

	return hResult;
}

/*	PayloadReferenced - if flag is set payload data is only referenced to data - no allocation and no copying */
BOOL pbIsPayloadReferenced(PB *pPB)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}
	
	return pPB->bPayloadReferenced;
}

HRESULT pbSetPayloadReferenced(PB *pPB, BOOL bPayloadReferenced)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pPB->bPayloadReferenced = bPayloadReferenced;

	return hResult;
}

BOOL pbIsNonBlocking(PB *pPB)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}
	
	return pPB->bNonBlocking;
}
/*	NonBlocking - if flag is set the transfer is non-blocking and caller shouldn't free packet */
HRESULT	pbSetNonBlocking(PB *pPB, BOOL bNonBlocking)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pPB->bNonBlocking = bNonBlocking;

	return hResult;
}

BOOL pbIsSelfDestruct(PB *pPB)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}
	
	return pPB->bSelfDestruct;
}
/*	SelfDestruct - if flag is set the caller takes responsibility of destroying packet itself */
HRESULT	pbSetSelfDestruct(PB *pPB, BOOL bSelfDestruct)
{
	HRESULT		hResult = NO_ERROR;

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pPB->bSelfDestruct = bSelfDestruct;
	if (bSelfDestruct) pPB->bSelfDestructCount++;

	return hResult;
}

void pbPacketDoneSelfDestruct(PB *pPB, uint32 callerId)
{
	if (pPB->bSelfDestructCount)
	{
		pPB->bSelfDestructCount--;
		if (pPB->bSelfDestructCount == 0)
		{
			pbPacketDone(pPB, callerId);
		}
	}
}

#endif //_OPTIMIZE && !_DEBUG

#endif //_1394CORE

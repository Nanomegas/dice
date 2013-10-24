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


/** @addtogroup PacketBlocks

	Defines format and access functions for PB, the packet block that is
	used for passing packets between layers of the stack.

	This module can be used from the command prompt by the @ref cli_pb commands.

	Packet blocks are a key data element in the firewire system.  This file 
	provides services to applications for creating, deleting and accessing
	data within packet blocks.


	This module defines the structure of a packet block and the various 'accessor'
	functions which provide access to packet blocks.

	In general, a packet block consists of 2 parts:
		- raw 1394 asychronous packet and header
		- logical header, control and application information.

	The payload portion of the packet may or may not be contained within the packet
	block itself, depending on the payload size.  In all cases, the packet block 
	contains a pointer to the payload.  Applications reference the payload via an
	accessor function, so actual payload location transparent to applications.

	Applications should not access any packet block fields directly by structure
	field name. 

	Some general rules for using packet blocks:

		1. Only one module 'has' a packet block at any one point in time.

		2. Whichever module 'has' the packet block must pass the block on to
			some other module.  If there is no module to pass it to, the 
			packet block must be passed to the 'pbPacketDone' function.

		4. The module which has the packet last (not including any callbacks) must 
			update the packetResult	field before calling pbPacketDone.  

		3. Packets are not to be re-used directly, IE don't reuse the incoming
			packet to send your response.  There are copy functions provided
			for the purpose of creating a new response block based on a command
			block.

		4. All callbacks which receive a packetBlock must enqueue the PB for processing.
			Callbacks may not perform any packetBlock processing and in any case, they
			MUST NOT BLOCK.  In this context, 'block' means to call any	service which 
			may result in thread being suspended.  The idea is that callback's need to 
			be deterministic if we're going to build a deterministic system.

		5. Packets are read-only to the debug traps (callbacks).


	A day in the life of a packet:
		- packet is created via 'createNewPacket', creator optionally providing a  'done' callback.
		- packet is passed up/down through the various layers.
		- packet is passed to 'pbPacketDone' which either passes the packet to a
			callback registered in the packet or if there is no callback to pass
			the packet to, pbPacketDone deletes the packet and it's gone.


	Packet block service functions provided:
		- create a packet
	    - pbPacketDone
		- copy/duplicate packet payload
		- get/set functions for the appropriate fields in the packet

	Currently, the payload is embedded in the packetBlock and the PB therefore has to
	be sized for the largest possible payload.  Long term, we want to separate the PB
	structure from the payload in order to make more efficient use of memory.  At that
	time, the PB may be changed to contain a pointer to payload memory.
	To facilitate these sorts of optimizations it is essential that applications use
	the provided accessor functions, rather than directly accessing structure fields.

	@{
	
*/

#ifndef _PACKETBLOCK_H
#define _PACKETBLOCK_H

#include "TCTypes.h"
#include "PacketBlockDefs.h"
#include "ErrorCodes.h"
#include "1394.h"
#include "1394Defs.h"
#ifdef _DATASTREAM
#include "datastream.h"
#endif //_DATASTREAM

#define PB_NUM_PACKET_PARAMS			9			// this can change
#define PB_NUM_HEADER_QUADLETS			4			// the maximum number necessary to handle any packet type
#define PB_NUM_LOCAL_PAYLOAD_QUADLETS	8			// size of 'local' payload


/* Memory addresses
   Invalid memory address is defined so that we can point NULL pointers somewhere
  that will generate an exception of accessed. */

#define	INVALID_MEMORY_ADDRESS			0xFF000000	

/** packet block type and structure definitions */


// magic number 32-bits
#define	PB_MAGIC_NUMBER					0x49034448	// valid block has this magic
#define	PB_MAGIC_DELETED				0x476F6E65	// released memory has this magic

#define	PB_DONE_CALLER_INDEX_ITEMS		8

// node handle, logical source/destination
typedef uint32		NODEHANDLE;

/** Callbacks that use packet blocks. Callbacks all have status return value.  
	If the callback has accepted the packet (IE enqueued the packet), it returns NO_ERROR and 
	the thread associated with the callback 'has' the packet.
	If the callback cannot accept the packet (IE queue is full), it returns an error status
	and the pbPacketDone function still 'has' the packet. 
*/
typedef HRESULT (* PB_CALLBACK) (void *packetBlock);
 

// packet queue proority to use for transmit

typedef enum
{
	PB_Q_TYPE_GENERAL = 0,
	PB_Q_TYPE_IMMEDIATE = 1
} PB_QTYPE;

/** Packet block parameters
*/
typedef struct
{
	uint32			initialTxTime;
	uint32			lowLevelAck;
	uint32			speed;
	uint32			numTimeouts;
	uint32			lastRetryTxTime;
	uint32			headerSizeBytes;
	uint32			payloadSizeBytes;
	uint32			tLabel;
	PB_QTYPE		queueType;
} PB_PARAMS;



/** Packet block packet types
*/
typedef enum
{
	PB_TYPE_UNDEF = -1,
	PB_TYPE_WRITE_REQUEST = TCODE_WRBLOCK,
	PB_TYPE_WRITE_REQUEST_QUADLET = TCODE_WRQUAD,
	PB_TYPE_WRITE_RESPONSE = TCODE_WRRESPONSE,
	PB_TYPE_READ_REQUEST = TCODE_RDBLOCK,
	PB_TYPE_READ_REQUEST_QUADLET = TCODE_RDQUAD,
	PB_TYPE_READ_RESPONSE = TCODE_RDBLOCKRSP,
	PB_TYPE_READ_RESPONSE_QUADLET = TCODE_RDQUADRSP,
	PB_TYPE_LOCK_REQUEST = TCODE_LOCKRQST,
	PB_TYPE_LOCK_RESPONSE = TCODE_LOCKRSP,
	PB_TYPE_ASYNC_STREAM = TCODE_ISOBLOCK,
	PB_TYPE_WRITE_REQUEST_GENERIC = TCODE_RESERVED_4,	// used as generic type for PB_TYPE_WRITE_REQUEST and PB_TYPE_WRITE_REQUEST_QUADLET 
														// for associating request to shared PB_TYPE_WRITE_RESPONSE
	PB_TYPE_PHY_PACKET = TCODE_LLC_SPECIFIC //actually dummy, PHY packets have no tcode.
} PB_PACKETTYPE;

/** Packet block lock types
*/
typedef enum
{
	PB_LOCKTYPE_NONE = 0,
	PB_LOCKTYPE_MASK_SWAP = 1,
	PB_LOCKTYPE_COMPARE_SWAP = 2,
	PB_LOCKTYPE_FETCH_ADD = 3,
	PB_LOCKTYPE_LITTLE_ADD = 4,
	PB_LOCKTYPE_BOUNDED_ADD = 5,
	PB_LOCKTYPE_WRAP_ADD = 6,
	PB_LOCKTYPE_VENDOR_DEPENDENT = 7
} PB_LOCKTYPE;


//! packet header fields - several variants, all handled by the transaction layer.
typedef struct
{
	QUADLET			quadlets[PB_NUM_HEADER_QUADLETS];
} PB_HEADER;




typedef struct _PB
{
									/*	magic number - first
										Packet block processes use this to verify that a packet block pointer
										is in fact pointing at a valid packet block.  This will guard against
										errant pointers as well as detect access to a packet block which has
										been deleted (although we are still vulnerable if the packet block
										has been deleted and re-allocated, sigh...). */
	uint32			magicNumberFirst;

									/*	pointer where pointer to block will be stored if NO_ERROR
										solving deleted an re-allocated problem */
	struct _PB		**pCreateAddr;
										
	uint32			callerIdCreate;
	uint32			callerIdDoneIndex;
	uint32			callerIdDone[PB_DONE_CALLER_INDEX_ITEMS];

									/*	packet sequence number
										written by packet creation process
										read-only to all other processes */
	uint32			packetSeqNum;

									/*	bus generation, generation of bus at time packet was created
										written by packet creation process read-only to all other processes
										increments after each bus reset */
	uint32			busGeneration;

									/*	node handle: traveling up the stack, source node of packet (who sent it to us)
										traveling down the stack, destination node (who is packet to be sent to)
										written by packet creation process
										read-only to all other processes */
	NODEHANDLE		nodeHandle;

#ifdef _DATASTREAM
									/*	application datastream - usage is specific to the application modules.
										In this context, 'application' means software layers above the 
										transport level: AVC, HaVI, etc - AVC-specific dataStream */
	DataStream		applicationDataStream;
#endif //_DATASTREAM
									/*	creator callback
										Written to by packet creation process
										Invoked by packet done process after all debug traps have been invoked. */
	PB_CALLBACK		creatorCBProc;
							  
									/*	Final status.  The last module which 'has' the packet (not including callbacks) sets
										final status before calling pbPacketDone.  Defaults to NO_ERROR. */
	HRESULT			finalStatus;

									/*	packet parameters */
	PB_PARAMS		packetParams;

									/*	Packet type - maps closely to the packet header tcode.  Use is local to the transaction layer.
										Passes logical tcode from transaction layer api to transaction layer itself.
										Basically, the transaction layer reads this type and converts it a tcode which
										is placed into the header.  */
	PB_PACKETTYPE	packetType;

									/*	payloadByteSize - number of BYTESs in this packet block payload
										Written to by packet creation process. */
	uint32			payloadByteSize;

									/*	packet header - raw 1394 header 
										Written to by the transaction layer
										Not needed or used by other layers */
	PB_HEADER		packetHeader;

									/*	payload pointer - pointer to local payload or to separate memory allocated for payload*/
	uint8			*pPayload;

									/*	local packet payload - raw 1394 data, the entire packet minus header and CRC's
										If payload is too big to fit, a separate memory block will be allocated for the payload. */
	QUADLET			localPayload[PB_NUM_LOCAL_PAYLOAD_QUADLETS];

									/*	if flag is set payload data is only referenced to data - no allocation and no copying */
	BOOL			bPayloadReferenced;	

									/*	if flag is set the transfer is non-blocking and caller shouldn't free packet */
	BOOL			bNonBlocking;	

									/*	if flag is set the caller takes responsibility of destroying packet itself */
	BOOL			bSelfDestruct;	
	uint32			bSelfDestructCount; 

									/*	magic number - last packet block processes use this to guard against payload overflow. */
	uint32			magicNumberLast;
} PB;


/***************************************************************************
	
	Packet Block control functions

***************************************************************************/

/** Initialize this module
	@return		NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbInitialize (void);

/** Creates a packet block.
	@param pPB				Pointer where pointer to block will be stored if NO_ERROR
	@param nodeHandle		Source or destination (incoming or outgoing respectively)
	@param callback			Done callback, NULL if you don't want to be called back
	@param numPayloadBytes	How large a payload would you like?
	@param callerId			id for the calling function (debugging)	
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbCreatePacket (
				PB			**pPB,				
				NODEHANDLE	nodeHandle,
				PB_CALLBACK	callback,
				uint32		numPayloadBytes,
				uint32		callerId);

/** Create an optimized packet block.
	@param pPB					Pointer where pointer to block will be stored if NO_ERROR
	@param nodeHandle			Source or destination (incoming or outgoing respectively)
	@param callback				Done callback, NULL if you don't want to be called back
	@param numPayloadBytes		How large a payload would you like?
	@param callerId				id for the calling function (debugging)	
	@param bPayloadReferenced	If flag is set payload data is only referenced to data
	@param pData				Reference to data from calling functions			
	@return						NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbCreatePacketOptimize (
				PB			**pPB,	
				NODEHANDLE	nodeHandle,
				PB_CALLBACK	callback,
				uint32		numPayloadBytes,
				uint32		callerId,
				BOOL		bPayloadReferenced,	
				uint8		*pData);


/* Duplicate a packet block.
		This would typically be used to create a response packet based on a command packet.
		The following fields are copied into the newly created packet:
			nodeHandle
			packetHeader
			payloadByteSize
			payloadData
		All other fields are initialized as created by pbCreatePacket.
	@param PB				The packet block to duplicate
	@param pDuplicatePB		Pointer where the new PB will be stored if NO_ERROR is returned.
	@param callback			Done callback, NULL if you don't want to be called back
	@param callerID			ID for the calling function (debugging)
	@return	NO_ERROR - new packet has been created, caller now 'has' 2 packets, original and new
			Any other value, no new packet has been created, caller still 'has' the original packet 		
*/
HRESULT	pbCreateDuplicatePacket (
				PB			*pPB,				
				PB			**pDuplicatePB,		
				PB_CALLBACK	callback,			
				uint32		callerId);

/* Duplicate a packet block and change the payload size.
		This would typically be used to create a response packet based on a command packet.
		The following fields are copied into the newly created packet:
			nodeHandle
			packetHeader
			payloadByteSize
			payloadData
		All other fields are initialized as created by pbCreatePacket.
	@param PB				The packet block to duplicate
	@param pDuplicatePB		Pointer where the new PB will be stored if NO_ERROR is returned.
	@param payloadBytes		Payload size for duplicate packet block
	@param callback			Done callback, NULL if you don't want to be called back
	@param callerID			ID for the calling function (debugging)
	@return	NO_ERROR - new packet has been created, caller now 'has' 2 packets, original and new
			Any other value, no new packet has been created, caller still 'has' the original packet 		
*/
HRESULT	pbCreateDuplicatePacketWithSize (
				PB			*pPB,
				PB			**pDuplicatePB,
				uint16		payloadBytes,
				PB_CALLBACK	callback,
				uint32		callerId);

/** Packet done
	@param pPB		Packet block to be released.
	@param callerId	ID for the calling function (debugging)
*/

void	pbPacketDone(PB *pPB, uint32 callerId);	// This function always succeeds.


/***************************************************************************
	
	Packet Block accessor functions

		Unless otherwise noted, return:
			NO_ERROR - successful
			E_PB_BAD_PACKET_BLOCK_PTR - packet block pointer does not appear to point at a valid PB

***************************************************************************/
#if defined (_OPTIMIZE) && !defined (_DEBUG)

#ifdef _WINMP
#pragma warning( disable : 4514 )
#pragma warning( disable : 4702 )
#endif //_WINMP

_TC_INLINE BOOL pbIsValid(PB *pPB) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL pbIsValid(PB *pPB)
{
	return ((pPB != NULL) && (PB_MAGIC_NUMBER == pPB->magicNumberFirst) && (PB_MAGIC_NUMBER == pPB->magicNumberLast));
}

_TC_INLINE BOOL pbIsPayloadReferenced(PB *pPB) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL pbIsPayloadReferenced(PB *pPB)
{
	return pPB->bPayloadReferenced;
}

_TC_INLINE HRESULT pbSetPayloadReferenced(PB *pPB, BOOL bPayloadReferenced) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSetPayloadReferenced(PB *pPB, BOOL bPayloadReferenced)
{
	pPB->bPayloadReferenced = bPayloadReferenced;

	return NO_ERROR;
}

_TC_INLINE BOOL pbIsNonBlocking(PB *pPB) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL pbIsNonBlocking(PB *pPB)
{
	return pPB->bNonBlocking;
}

_TC_INLINE HRESULT pbSetNonBlocking(PB *pPB, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSetNonBlocking(PB *pPB, BOOL bNonBlocking)
{
	pPB->bNonBlocking = bNonBlocking;

	return NO_ERROR;
}

_TC_INLINE BOOL pbIsSelfDestruct(PB *pPB) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL pbIsSelfDestruct(PB *pPB)
{
	return pPB->bSelfDestruct;
}
/*	SelfDestruct - if flag is set the caller takes responsibility of destroying packet itself */
_TC_INLINE HRESULT pbSetSelfDestruct(PB *pPB, BOOL bSelfDestruct) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSetSelfDestruct(PB *pPB, BOOL bSelfDestruct)
{
	pPB->bSelfDestruct = bSelfDestruct;
	if (bSelfDestruct) pPB->bSelfDestructCount++;

	return NO_ERROR;
}

_TC_INLINE void pbPacketDoneSelfDestruct(PB *pPB, uint32 callerId) _TC_INLINE_ALWAYS;
_TC_INLINE void pbPacketDoneSelfDestruct(PB *pPB, uint32 callerId)
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

_TC_INLINE HRESULT pbGetSequenceNum (PB* pPB, uint32* pSequenceNumber) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetSequenceNum (PB* pPB, uint32* pSequenceNumber)
{
	*pSequenceNumber = pPB->packetSeqNum;

	return NO_ERROR;
}	

_TC_INLINE HRESULT pbGetBusGeneration (PB* pPB, uint32* pBusGeneration) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetBusGeneration (PB* pPB, uint32* pBusGeneration)
{
	*pBusGeneration = pPB->busGeneration;

	return NO_ERROR;
}	

_TC_INLINE HRESULT pbGetNodeHandle (PB* pPB, NODEHANDLE* pNodeHandle) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetNodeHandle (PB* pPB, NODEHANDLE* pNodeHandle)
{
	*pNodeHandle = pPB->nodeHandle;

	return NO_ERROR;
}	

_TC_INLINE HRESULT pbSetNodeHandle (PB* pPB, NODEHANDLE nodeHandle) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSetNodeHandle (PB* pPB, NODEHANDLE nodeHandle)
{
	pPB->nodeHandle = nodeHandle;

	return NO_ERROR;
}	

#ifdef _DATASTREAM
_TC_INLINE HRESULT pbGetApplicationDatastream (PB *pPB, DataStream **pStream) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetApplicationDatastream (PB *pPB, DataStream **pStream)
{
	*pStream = &pPB->applicationDataStream;

	return NO_ERROR;
}
#endif //_DATASTREAM

_TC_INLINE HRESULT pbGetFinalStatus (PB *pPB, HRESULT *pFinalStatus) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetFinalStatus (PB *pPB, HRESULT *pFinalStatus)
{
	*pFinalStatus = pPB->finalStatus;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbSetFinalStatus (PB *pPB, HRESULT finalStatus) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSetFinalStatus (PB *pPB, HRESULT finalStatus)
{
	pPB->finalStatus = finalStatus;

	return NO_ERROR;
}

_TC_INLINE HRESULT	pbGetPacketParams (PB *pPB, PB_PARAMS **pPacketParams) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbGetPacketParams (PB *pPB, PB_PARAMS **pPacketParams)
{
	*pPacketParams =  &pPB->packetParams;

	return NO_ERROR;
}

_TC_INLINE HRESULT	pbGetPacketSpeed (PB *pPB, uint32 *speed) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbGetPacketSpeed (PB *pPB, uint32 *speed)
{
	*speed = pPB->packetParams.speed;

	return NO_ERROR;
}

_TC_INLINE HRESULT	pbSetPacketSpeed (PB *pPB, uint32 speed) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbSetPacketSpeed (PB *pPB, uint32 speed)
{
	pPB->packetParams.speed = speed;

	return NO_ERROR;
}

_TC_INLINE HRESULT	pbGetPacketType (PB *pPB, PB_PACKETTYPE *pPacketType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbGetPacketType (PB *pPB, PB_PACKETTYPE *pPacketType)
{
	*pPacketType = pPB->packetType;

	return NO_ERROR;
}

_TC_INLINE HRESULT	pbIsResponsePacket (PB *pPB, uint8 *response) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbIsResponsePacket (PB *pPB, uint8 *response)
{
	PB_PACKETTYPE	packetType;

	*response = FALSE;

	packetType = pPB->packetType;

	if (packetType == PB_TYPE_WRITE_RESPONSE ||
		packetType == PB_TYPE_READ_RESPONSE ||
		packetType == PB_TYPE_READ_RESPONSE_QUADLET ||
		packetType == PB_TYPE_LOCK_RESPONSE)
	{
		*response = TRUE;
	}

	return NO_ERROR;
}

#ifdef _LOOSE_ISO
_TC_INLINE HRESULT	pbIsAsyncStreamPacket (PB *pPB, uint8 *asyncStream) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbIsAsyncStreamPacket (PB *pPB, uint8 *asyncStream)
{
	*asyncStream = (uint8) (pPB->packetType == PB_TYPE_ASYNC_STREAM);

	return NO_ERROR;
}
#endif //_LOOSE_ISO

_TC_INLINE HRESULT	pbSetPacketType (PB *pPB, PB_PACKETTYPE packetType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbSetPacketType (PB *pPB, PB_PACKETTYPE packetType)
{
	pPB->packetType = packetType;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetDestinationOffset(PB *pPB, OFFSET_1394 *offset) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetDestinationOffset(PB *pPB, OFFSET_1394 *offset)
{
	offset->High = (uint16) ((pPB->packetHeader.quadlets[1]) & 0xFFFF);  // least two significant bytes of 2nd quadlet
	offset->Low  = (pPB->packetHeader.quadlets[2]);             // 3rd quadlet

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetPacketHeader (PB* pPB, PB_HEADER **pHeader) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetPacketHeader (PB* pPB, PB_HEADER **pHeader)
{
	*pHeader = &pPB->packetHeader;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetPacketTLabel (PB* pPB, uint32 *tLabel) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetPacketTLabel (PB* pPB, uint32 *tLabel)
{
	*tLabel = pPB->packetHeader.quadlets[0] & MASK_TLABEL;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetPacketDstNodeAddr (PB* pPB, uint32 *pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetPacketDstNodeAddr (PB* pPB, uint32 *pNodeAddr)
{
	*pNodeAddr = (pPB->packetHeader.quadlets[0] & MASK_NODEADDR) >> SHIFT_NODEADDR;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbSetPacketDstNodeAddr (PB* pPB, uint32 nodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSetPacketDstNodeAddr (PB* pPB, uint32 nodeAddr)
{
	pPB->packetHeader.quadlets[0] = (pPB->packetHeader.quadlets[0] & ~MASK_NODEADDR) | ((nodeAddr << SHIFT_NODEADDR) & MASK_NODEADDR);

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetPacketSrcNodeAddr (PB* pPB, uint32 *pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetPacketSrcNodeAddr (PB* pPB, uint32 *pNodeAddr)
{
	*pNodeAddr = (pPB->packetHeader.quadlets[1] & MASK_NODEADDR) >> SHIFT_NODEADDR;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbSetPacketSrcNodeAddr (PB* pPB, uint32 nodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSetPacketSrcNodeAddr (PB* pPB, uint32 nodeAddr)
{
	pPB->packetHeader.quadlets[1] = (pPB->packetHeader.quadlets[1] & ~MASK_NODEADDR) | ((nodeAddr << SHIFT_NODEADDR) & MASK_NODEADDR);

	return NO_ERROR;
}

_TC_INLINE HRESULT pbSwapSrcDstNodeAddr(PB* pPB) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbSwapSrcDstNodeAddr(PB* pPB)
{
	uint32 srcNodeAddr = 0;
	uint32 dstNodeAddr = 0;

	pbGetPacketSrcNodeAddr (pPB, &srcNodeAddr);
	pbGetPacketDstNodeAddr (pPB, &dstNodeAddr);
	pbSetPacketSrcNodeAddr (pPB, dstNodeAddr);
	pbSetPacketDstNodeAddr (pPB, srcNodeAddr);

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetDataLen (PB* pPB, uint32 *pDataLen) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetDataLen (PB* pPB, uint32 *pDataLen)
{
	*pDataLen = (pPB->packetHeader.quadlets[3] & MASK_DATA_LEN) >> SHIFT_DATA_LEN;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetPayloadSize (PB* pPB, uint32 *pPayloadSizeInBytes) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetPayloadSize (PB* pPB, uint32 *pPayloadSizeInBytes)
{
	*pPayloadSizeInBytes = pPB->payloadByteSize;

	return NO_ERROR;
}

_TC_INLINE HRESULT	pbSetPayloadSize (PB *pPB, uint32 payloadSizeInBytes) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	pbSetPayloadSize (PB *pPB, uint32 payloadSizeInBytes)
{
	pPB->payloadByteSize = payloadSizeInBytes;

	return NO_ERROR;
}

_TC_INLINE HRESULT pbGetPayload (PB* pPB, void **pPayload) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetPayload (PB* pPB, void **pPayload)
{
	*pPayload = pPB->pPayload;

	return NO_ERROR;
}

_TC_INLINE BOOL pbPacketIsBroadcast (PB* pPB) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL pbPacketIsBroadcast (PB* pPB)
{
	return (((pPB->packetHeader.quadlets[0] >> SHIFT_NODEID) & MASK_NODEID) == BROADCAST_NODE_ID);
}	

_TC_INLINE HRESULT pbGetLockType(PB *pPB, PB_LOCKTYPE *pLockType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT pbGetLockType(PB *pPB, PB_LOCKTYPE *pLockType)
{
	*pLockType = PB_LOCKTYPE_NONE;

	if (pPB->packetType == PB_TYPE_LOCK_REQUEST)
	{
		*pLockType = (PB_LOCKTYPE)((pPB->packetHeader.quadlets[3] & 0x0000FFFF));
	}

	return NO_ERROR;
}

#else //_OPTIMIZE && !_DEBUG

/* IsValid - boolean */

/** Determine if the packet block is valid
	@param pPB	The packet block to check.
	@return		TRUE if the packet block appears to be valid.
*/
BOOL pbIsValid(PB *pPB);

/** Get the sequence number of a packet block
	@param pPB				Pointer to the packet block
	@param pSequenceNumber	The address to place the sequence number in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetSequenceNum (PB* pPB, uint32* pSequenceNumber);

/** Get the bus generation of a packet block
	@param pPB				Pointer to the packet block
	@param pBusGeneration	The address to place the bus generation in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetBusGeneration (PB* pPB, uint32* pBusGeneration);

/** Obtain the packet node handle. For incoming packets, this is the node that sent the
	packet to us.  For outgoing packets, this is the node that we want the packet sent to.
	@param pPB				Pointer to the packet block.
	@param pNodeHandle		The address to place the node handle in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetNodeHandle (PB* pPB, NODEHANDLE* pNodeHandle);

/** Set the packet node handle.  For outgoing packets, this is the node that we want the packet sent to.
	@param pPB				Pointer to the packet block.
	@param nodeHandle		The node handle to use.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbSetNodeHandle (PB* pPB, NODEHANDLE nodeHandle);

#ifdef _DATASTREAM
/** Obtain the application data stream for this packet block.  Note that data streams must be included
   in the build (_DATASTREAM is defined).  Refer to @ref DataStreams for more details.
   @param pPacketBlock		Pointer to the packet block.
   @param pStream			The address of the pointer that will be set to point to the data stream.
   @return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbGetApplicationDatastream (PB *pPacketBlock, DataStream **pStream);
#endif //_DATASTREAM

/* Get/set final status - note that the 'get' function return value is not the 
		final status. */
HRESULT	pbGetFinalStatus (PB *pPB, HRESULT *pFinalStatus);
HRESULT	pbSetFinalStatus (PB *pPB, HRESULT finalStatus);

/** Obtain pointer to packet parameter structure for a packet block
	@param pPB				Pointer to the packet block.
	@param pPacketParams	Address of pointer that will be set to the packet parameters.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPacketParams (PB *pPB, PB_PARAMS **pPacketParams);

/** Obtain the signalling speed of a packet
	@param pPB				Pointer to the packet block. 
	@param speed			Address to place speed in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPacketSpeed (PB *pPB, uint32 *speed);

/** Set the signalling speed of a packet
	@param pPB				Pointer to the packet block. 
	@param speed			The speed to use.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbSetPacketSpeed (PB *pPB, uint32 speed);

/** getPacketQueueType - gets packet transmit queue priority */
HRESULT pbGetPacketQueueType (PB *pPB, PB_QTYPE *qType);

/** setPacketQueueType - sets packet transmit queue priority */
HRESULT pbSetPacketQueueType (PB *pPB, PB_QTYPE qType);

/** Obtain the type of packet that a packet block is
	@param pPB				Pointer to the packet block. 
	@param pPacketType		The address to place the packet type in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbGetPacketType (PB *pPB, PB_PACKETTYPE *pPacketType);

/** Set the packet type for a packet block.
	@param pPB				Pointer to the packet block. 
	@param packetType		The packet type to use.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbSetPacketType (PB *pPB, PB_PACKETTYPE packetType);

/** Determinie if a packet block is a response packet
	@param pPB				Pointer to the packet block. 
	@param response			TRUE is the packet is a response.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbIsResponsePacket (PB *pPB, uint8 *response);
#ifdef _LOOSE_ISO

/** Determine if a packet block is an async stream packet
	@param pPB				Pointer to the packet block. 
	@param asyncStream		TRUE is the packet is an async stream packet.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbIsAsyncStreamPacket (PB *pPB, uint8 *asyncStream);
#endif //_LOOSE_ISO

/** Obtain the type of lock transaction for a packet block of type PB_TYPE_LOCK_REQUEST
	@param pPB				Pointer to the packet block. 
	@param pLockType		The address to place the lock type in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetLockType(PB *pPB, PB_LOCKTYPE *pLockType);

/** Obtain the destination offset of a packet block
	@param pPB				Pointer to the packet block. 
	@param offset			The address to place the destination offset in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetDestinationOffset(PB *pPB, OFFSET_1394 *offset);

/** Obtain a pointer to the packet header
	@param pPB				Pointer to the packet block. 
	@param pHeader			Address of the pointer to set to the packet header.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPacketHeader (PB* pPB, PB_HEADER **pHeader);

/** Obtain the tLabel from a packet block
	@param pPB				Pointer to the packet block. 
	@param tLabel			The address to place the tLabel in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPacketTLabel (PB* pPB, uint32 *tLabel);

/** Obtain the destination node address from a packet block
	@param pPB				Pointer to the packet block. 
	@param pNodeAddr		The address to place the destination node address in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPacketDstNodeAddr (PB* pPB, uint32 *pNodeAddr);

/** Set the destination node address in a packet block
	@param pPB				Pointer to the packet block. 
	@param nodeAddr			The destination node address to set.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbSetPacketDstNodeAddr (PB* pPB, uint32 nodeAddr);

/** Obtain the source node address from a packet block
	@param pPB				Pointer to the packet block. 
	@param pNodeAddr		The address to place the source node address in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPacketSrcNodeAddr (PB* pPB, uint32 *pNodeAddr);

/** Set the source node address in a packet block
	@param pPB				Pointer to the packet block. 
	@param nodeAddr			The source node address to set.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbSetPacketSrcNodeAddr (PB* pPB, uint32 nodeAddr);

/** Swap the source and destination node addresses in a packet block.
	@param pPB				Pointer to the packet block. 
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbSwapSrcDstNodeAddr(PB* pPB);

/** Obtain the value of the data length field of a packet block
	@param pPB				Pointer to the packet block. 
	@param pDataLen			The address to place the data length field in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetDataLen (PB* pPB, uint32 *pDataLen);

/** Obtain the payload size in bytes of a packet block
	@param pPB					Pointer to the packet block. 
	@param pPayloadSizeInBytes	The address to place the payload size in.
	@return						NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPayloadSize (PB* pPB, uint32 *pPayloadSizeInBytes);

/** Set the payload size in bytes of a packet block
	@param pPB					Pointer to the packet block. 
	@param payloadSizeInBytes	The payload size in bytes to set.
	@return						NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbSetPayloadSize (PB* pPB, uint32 payloadSizeInBytes);

/** Obtain a pointer to a packet block's payload
	@param pPB			Pointer to the packet block. 
	@param pPayload		Address of the pointer to set to the payload.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT pbGetPayload (PB* pPB, void **pPayload);

/** Check if a packet is a broadcast packet (node id = 0x3f)
	@param pPB			Pointer to the packet block.
	@return				TRUE is the packet is a broadcast packet.
*/
BOOL	pbPacketIsBroadcast (PB* pPB);

/** Check if a payload is referenced (no allocation and no copying)
	@param pPB			Pointer to the packet block.
	@return				TRUE is the payload is referenced.
*/
BOOL	pbIsPayloadReferenced(PB *pPB);

/** Set if the payload is referenced (no allocation and no copying)
	@param pPB					Pointer to the packet block.
	@param bPayloadReferenced	TRUE if the payload is referenced.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbSetPayloadReferenced(PB *pPB, BOOL bPayloadReferenced);

/** Check if the transfer is non-blocking and caller shouldn't free packet.
	@param pPB			Pointer to the packet block.
	@return				TRUE is the packet is non-blocking.
*/
BOOL	pbIsNonBlocking(PB *pPB);

/** Set if the transfer is non-blocking and caller shouldn't free packet.
	@param pPB				Pointer to the packet block.
	@param bNonBlocking		TRUE if the packet is non-blocking.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbSetNonBlocking(PB *pPB, BOOL bNonBlocking);

/** Check if the packet block is self destructing - if it is then the caller takes 
	responsibility of destroying the packet itself.
	@param pPB			Pointer to the packet block.
	@return				TRUE is the packet is self destructing.
*/
BOOL	pbIsSelfDestruct(PB *pPB);

/** Set if the packet block is self destructing - if it is then the caller takes 
	responsibility of destroying the packet itself.
	@param pPB				Pointer to the packet block.
	@param bSelfDestruct	TRUE if the packet is self destructing.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	pbSetSelfDestruct(PB *pPB, BOOL bSelfDestruct);

/** Destruct a packet block that is set for self destruct.
	@param pPB				Pointer to the packet block.
	@param callerId			Id of the caller (debugging).
*/
void	pbPacketDoneSelfDestruct(PB *pPB, uint32 callerId);	// This function always succeeds.
#endif //_OPTIMIZE && !_DEBUG

//! @}


#endif //_PACKETBLOCK_H

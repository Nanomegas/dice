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
* DESCRIPTION: Node Controller Interface

	Purpose: Node Controller Interface functions

	Revisions:
		created 04/09/2002 lm
****************************************************************************/

#ifndef _NCI_H
#define _NCI_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "ErrorCodes.h"
#include "packetBlock.h"
#include "bmdefs.h"
#include "lalDefs.h"

/*****************************************************************************

	nciInitialize
		
	Initialize the nci module.

******************************************************************************/
HRESULT	nciInitialize(void);


/*************************************************************

	nciVerifySelfIds

*****************************


	Description:
		This function verifies the array of self-id packets.  
		Validity checks that are performed include:
			- all fields in each packet contain valid data, except that
					'reserved' fields may contain any value
			- selfid packets occur in node-id order.
			- node id's are dense, starting with node #0
			- highest node number is 62
			- multiple selfid packets are correctly sequenced
			- root node has no parents
	Parameters:
		selfidPackets - INPUT, array of self-id packets from which to build
				the topo map.  The packets are assumed to occur in the same
				order as received.
		numSelfIDs - number of quadlets in the self-id array
		numNodes - number of nodes in the self-id array
	Return:
		NO_ERROR indicates success - the array of self-ids is valid and could be
				used to construct a valid topology map.
		Any other value indicates failure.  In the case of failure, the 
		specific error(s) have already been reported to the log device.

	External services used:
		sysLogError

****************************/

HRESULT	nciVerifySelfIDs (
				QUADLET		*pSelfIDs,
				uint32		numSelfIDs,
				uint32		numNodes ) ;

/*************************************************************

	nciGetBusInfoBlocks

*****************************

	Description:
		Get bus info blocks from all nodes and put them into the caller's
		node data structure.  Errors detected will be reported via the 
		sysLog module.  
		
		NOTE: async read failures - failure to read bus information from a 
		node may be a 'normal' event and is not treated as an error by this
		function.  That node's bib element is simply flagged as 'not available'.
		 
	Parameters:
		numNodes - INPUT, number of nodes
		bibs - OUPUT, pointer to an array of bus info data blocks to be filled in
	Return:
		NO_ERROR indicates success - bifs contains busInfoBlocks for all the nodes
		Any other value indicates failure and the bif array is not valid.
		In the case of failure, the specific error(s) have already been 
		reported to the log device.  

	External services used:
		sysLogError

****************************/

HRESULT	nciGetBusInfoBlocks (uint32 numNodes, BIB_DATA *bibs);


/*************************************************************

	nciUpdateBusInfoBlocks

*****************************

	Description:
		Same as nciGetBusInfoBlocks except that if bCLear is FALSE only 
		nodes which has not bene updated will be updated.
		
		 
	Parameters:
		numNodes - INPUT, number of nodes
		bibs - OUPUT, pointer to an array of bus info data blocks to be filled in
		bClear - INPUT, if true the previous cach will be cleared.
	
	Return:
		NO_ERROR indicates success - bifs contains busInfoBlocks for all the nodes
		Any other value indicates failure and the bif array is not valid.
		In the case of failure, the specific error(s) have already been 
		reported to the log device.  

	External services used:
		sysLogError

****************************/
HRESULT	nciUpdateBusInfoBlocks (uint32 numNodes, BIB_DATA *bibs, BOOL bClear);

				
/*************************************************************

	nciGetBusInfo

*****************************

	Description:
		provide a pointer to the current bus info.
	Return:
		NO_ERROR indicates success.  Any other values indicate that information
		is not available.

	External services used:
		sysLogError

****************************/

HRESULT nciGetBusInfo(LAL_BUS_RESET_INFO* plalInfoBlock);


/*************************************************************

	nciGetNodeInfo

*****************************

	Description:
		provide a pointer to node info based on nodeAddr.
	Return:
		NO_ERROR indicates success.  Any other values indicate that information
		is not available.

	External services used:
		sysLogError

****************************/

HRESULT nciGetNodeInfo(uint32 nodeAddr, LAL_DEVICE_INFO* plalDevInfo);

_TC_INLINE BOOL nciExtractSelfid2LinkActive (BYTE selfIdByte) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL nciExtractSelfid2LinkActive (BYTE selfIdByte)
{
	return ((selfIdByte & (LINK_ACTIVE_MASK >> 16)) == (LINK_ACTIVE_MASK >> 16));
}

_TC_INLINE BOOL nciExtractSelfid3Contender (BYTE selfIdByte) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL nciExtractSelfid3Contender (BYTE selfIdByte)
{
	return ((selfIdByte & (CONTENDER_MASK >> 8)) == (CONTENDER_MASK >> 8));
}

_TC_INLINE BOOL nciExtractSelfidLinkActive (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL nciExtractSelfidLinkActive (QUADLET selfIdPacket)
{
	return nciExtractSelfid2LinkActive ((BYTE) ((selfIdPacket >> 16) & 0x000000FF));
}

_TC_INLINE BOOL nciExtractSelfidContender (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL nciExtractSelfidContender (QUADLET selfIdPacket)
{
	return nciExtractSelfid3Contender ((BYTE) ((selfIdPacket >> 8) & 0x000000FF));
}

_TC_INLINE int32 nciExtractSelfidNodeId (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE int32 nciExtractSelfidNodeId (QUADLET selfIdPacket)
{
	return ((int32) (selfIdPacket & PHY_ID_MASK) >> 24);
}

_TC_INLINE int32 nciExtractSelfidGapCnt (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE int32 nciExtractSelfidGapCnt (QUADLET selfIdPacket)
{
	return ((int32) (selfIdPacket & 0x003F0000) >> 16);
}

_TC_INLINE int32 nciExtractSelfidSpeed (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE int32 nciExtractSelfidSpeed (QUADLET selfIdPacket)
{
	return ((int32) (selfIdPacket & 0x0000C000) >> 14);
}

_TC_INLINE int32 nciExtractSelfidSeqNum (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE int32 nciExtractSelfidSeqNum (QUADLET selfIdPacket)
{
	return ((int32) (selfIdPacket & SEQ_NUM_MASK) >> 20);
}

_TC_INLINE BOOL nciExtractSelfidInitiator (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL nciExtractSelfidInitiator (QUADLET selfIdPacket)
{
	return ((BOOL) (selfIdPacket & BIT1) == BIT1);
}
	
_TC_INLINE BOOL nciExtractSelfidMorePackets (QUADLET selfIdPacket) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL nciExtractSelfidMorePackets (QUADLET selfIdPacket)
{
	return ((BOOL) (selfIdPacket & MORE_MASK) == MORE_MASK);
}
	
#endif

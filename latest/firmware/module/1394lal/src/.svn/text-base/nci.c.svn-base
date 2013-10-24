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

static char 		moduleName[] = __FILE__;

#include "TCTasking.h"
#include "lhlBri.h"
#include "lhlCli.h"
#include "lhlMsgQueue.h"
#include "lhlReq.h"
#include "nci.h"
#include "nciBM.h"
#include "nciCB.h"
#include "nciCli.h"
#include "nciHandles.h"
#include "nciIRM.h"
#include "bri.h"
#include "csr.h"

LAL_BUS_RESET_INFO		nciInfoBlock;

/*****************************************************************************
 
  Helper functions
 
******************************************************************************/

int32 nciPortTypeCount (QUADLET selfId, int32 numPortFields, uint32 portType)
{
	/* 	selfId:	self-id packet shifted to right-justify the array of 'port number' fields
	   	numPortFields: how many 2-bit port number are there
	I.E.:	packet #0 - right shift 2, numFields = 3
	packet #n - right shift 2, numFields = 8
	portType: type of port to count: parent, child, etc

	*/

	int32 	i;
	int32	count = 0;

	for (i = 0; i < numPortFields; ++i)
	{
		if (portType == (selfId & 3))
		{
			++count;
		}
		selfId >>= 2;
	}
	return count;

}		

_TC_INLINE_STATIC int32 nciCountParents (QUADLET selfId, int32 numPortFields) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int32 nciCountParents (QUADLET selfId, int32 numPortFields)
{
	return (nciPortTypeCount(selfId, numPortFields, PORT_PARENT));
}		

_TC_INLINE_STATIC int32 nciCountChildren (QUADLET selfId, int32 numPortFields) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int32 nciCountChildren (QUADLET selfId, int32 numPortFields)
{
	return (nciPortTypeCount(selfId, numPortFields, PORT_CHILD));
}		

_TC_INLINE_STATIC int32 nciNumParentsPacket0(QUADLET selfId) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int32 nciNumParentsPacket0(QUADLET selfId)
{
	return (nciCountParents(selfId >> 2, 3));
}

_TC_INLINE_STATIC int32 nciNumParentsExtendedPacket(QUADLET selfId) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int32 nciNumParentsExtendedPacket(QUADLET selfId)
{
	return (nciCountParents(selfId >> 2, 8));
}

_TC_INLINE_STATIC int32 nciNumChildrenPacket0(QUADLET selfId) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int32 nciNumChildrenPacket0(QUADLET selfId)
{
	return (nciCountChildren(selfId >> 2, 3));
}

_TC_INLINE_STATIC int32 nciNumChildrenExtendedPacket(QUADLET selfId) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int32 nciNumChildrenExtendedPacket(QUADLET selfId)
{
	return (nciCountChildren(selfId >> 2, 8));
}

/*****************************************************************************

	nciInitialize
		
	Initialize the nci module.

******************************************************************************/
HRESULT	nciInitialize(void)
{
	HRESULT	hResult = NO_ERROR;

	if (hResult == NO_ERROR)
	{
		hResult = nciCBInitialize();
	}

	if (hResult == NO_ERROR)
	{
		hResult = nciHandlesInitialize();
	}

#ifdef _IRMC_CAPS
	if (hResult == NO_ERROR)
	{
		hResult = nciIRMInitialize();
	}
#endif //_IRMC_CAPS

#ifdef _BM
	if (hResult == NO_ERROR)
	{
		hResult = nciBMInitialize();
	}
#endif //_BM

	// Initialize data structures
	memset (&nciInfoBlock, 0, sizeof(LAL_BUS_RESET_INFO));

#ifdef _CLI_TOOL_NCI
	nciCliInstallTools();
#endif //_CLI_TOOL_NCI

	return hResult;
}

/*****************************************************************************

	nciVerifySelfIDs
		
	Validates a list of self-id packets.

******************************************************************************/

HRESULT	nciVerifySelfIDs (QUADLET *pSelfIDs, uint32 numSelfIDs, uint32 numNodes)
{
	HRESULT	hResult = NO_ERROR;
	uint32	i;
	QUADLET	curSelfid;				// current packet
	int32	prevNodeId = -1;		// id of previous packet
	int32	curNodeId;				// id of current packet
	int32	curSeqNum;				// sequence number of current extended packet
	int32	rootNodeId = -1;		// id of root node
	int32	irmNodeId = -1;			// id of irm node
	int32	numParents;
	int32	numChildren;
	BOOL  	expectMore = FALSE;		// next self-id s/b extended selfid from same node
	int32	expectedSeqNum = -1;	// expected sequence number if expectMore
	int32	numContenders = 0;		// how many nodes contending to be manager
	BOOL	bHandleBit23;

	// verify caller's parms

	if (!(numNodes > 0) ||
		!(numNodes < MAX_NODES_PER_BUS) ||
		!(numSelfIDs >= numNodes))
	{
		hResult = E_NCI_NODE_COUNT_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (pSelfIDs == NULL)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	memset(&nciInfoBlock, 0, sizeof(nciInfoBlock));

	for (i = 0; i < numSelfIDs; ++i)
	{
		curSelfid = pSelfIDs[i];
		curNodeId = nciExtractSelfidNodeId(curSelfid);
		curSeqNum = nciExtractSelfidSeqNum(curSelfid);
		bHandleBit23 = FALSE;

		if ((curSelfid & (BIT31 | BIT30)) != BIT31)				// self-id packet has bit31=1 and bit30 = 0
		{
			hResult = E_NCI_PACKET_NOT_SELFID;
			sysLogError(hResult, __LINE__, moduleName);
		}
																// verify packet format
		if (i == 0)												// 1st packet is a special case
		{
			if (curNodeId == 0)									// must be node 0
			{
				bHandleBit23 = TRUE;
			}
			else
			{
				hResult = E_NCI_ID_SEQ_ERROR_0;  				// 1st selfid packet in error
				sysLogError(hResult, __LINE__, moduleName);
			}
		}
		else													// not first self id packet, we have some context to work from 
		{
			if (expectMore)
			{
				if ((curNodeId == prevNodeId)		&&			// must have same node id as previous packet
					(expectedSeqNum == curSeqNum)	&&			// sequence number must increase
					((curSelfid & BIT23) != 0))					// extended packet can't have bit 23 set
				{
					expectedSeqNum = curSeqNum+1;				// if there is another extended packet
					expectMore = (BOOL)(curSelfid & MORE_MASK);
				}
				else											// packet is verified OK, set up for next
				{
					hResult = E_NCI_EXT_SELFID_ERROR;		
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			else												// we're expecting packet #0 from the next node
			{
				if (curNodeId == (prevNodeId+1))
				{
					bHandleBit23 = TRUE;
				}
				else											// packet is from next node, check it out
				{
					hResult = E_NCI_NODE_SEQ_ERROR;				// node id's out of sequence
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
		}

		if (bHandleBit23 == TRUE)
		{
			if ((curSelfid & BIT23) == 0)						// packet #0 has bit23 = 0
			{
				rootNodeId = curNodeId;							// save for later	
				expectedSeqNum = 0;
				expectMore = (BOOL)(curSelfid & MORE_MASK);
				if (nciExtractSelfidLinkActive(curSelfid) &&
					nciExtractSelfidContender(curSelfid))
				{
					numContenders++;
					irmNodeId = curNodeId;
				}
			}
			else
			{
				hResult = E_NCI_PACKET0_FORMAT_ERROR;  			// 1st selfid packet in error
				sysLogError(hResult, __LINE__, moduleName);
			}
		}

		prevNodeId = curNodeId;
	}

	// Individual packets have had their format and sequence verified,
	// now check for system consistency:
	// - there must be a contender for bus/isoc manager
	// - our node count must match caller's

	if (hResult == NO_ERROR)
	{
		if (!numContenders)
		{
#if 1 //LM???
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
			{
				sysDebugPrintf("nci VerifySelfIds: no contenders\n\r");
			}
#endif //_SYSDEBUG
#else
			hResult = E_NCI_NO_CONTENDERS;						// gotta have at least one contender
			sysLogError(hResult, __LINE__, moduleName);
#endif //LM???
		}
		else if (numNodes != (uint32) (rootNodeId + 1))
		{
			hResult = E_NCI_NODE_COUNT_ERROR;
			sysLogError(hResult, __LINE__, moduleName);
		}
	}

	// More consistency checks
	// - root must have no parents
	// - branch and leaf nodes must have exactly 1 parent
	// - node 0 must be a leaf (no children)

	i = 0;
	while ((hResult == NO_ERROR) && (i < numSelfIDs))
	{
		curSelfid = pSelfIDs[i];
		curNodeId = nciExtractSelfidNodeId(curSelfid);
		numParents = nciNumParentsPacket0(curSelfid);
		numChildren = nciNumChildrenPacket0(curSelfid);
		while (pSelfIDs[i] & MORE_MASK)
		{
			++i;
			numParents += nciNumParentsExtendedPacket(pSelfIDs[i]);
			numChildren += nciNumChildrenExtendedPacket(pSelfIDs[i]);
			if (curNodeId != nciExtractSelfidNodeId(pSelfIDs[i]))
			{
				hResult = E_NCI_NODE_COUNT_ERROR;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
		}	

		if ((curNodeId == rootNodeId) && (numParents > 0))
		{
			hResult = E_NCI_ROOT_HAS_PARENTS;
			sysLogError(hResult, __LINE__, moduleName);
			sysDebugPrintf("numParents: %i\n\r", numParents);
		}
		else if ((curNodeId == 0) && (numChildren > 0))
		{
			hResult = E_NCI_NODE0_HAS_KIDS;
			sysLogError(hResult, __LINE__, moduleName);
			sysDebugPrintf("numChildren: %i\n\r", numChildren);
		}
		if (numParents > 1)
		{
			hResult = E_NCI_TOO_MANY_PARENTS;
			sysLogError(hResult, __LINE__, moduleName);
			sysDebugPrintf("curNodeId: %i\n\r", curNodeId);
		}
		++i;
	}

	if (hResult == NO_ERROR)
	{
		nciInfoBlock.numNodes = numNodes;
		nciInfoBlock.rootAddr = (uint32) (LOCAL_BUS_ID_MASK | rootNodeId);
		nciInfoBlock.irmAddr = (uint32) (LOCAL_BUS_ID_MASK | irmNodeId);
		nciInfoBlock.numSelfIDs = numSelfIDs;
		nciInfoBlock.SIDBundle = pSelfIDs;
		nciInfoBlock.bValid = TRUE;
	}

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
	{
		sysDebugPrintf("selfids validated\n\r");
	}
#endif //_SYSDEBUG

	return hResult;
}

/*****************************************************************************

	nciReadNodeGeneralROMFormat

******************************************************************************/
HRESULT	nciReadNodeGeneralROMFormat (uint32 nodeNum, uint32 busId, BIB_DATA *pCurBib)
{
	HRESULT		hResult = NO_ERROR;
	int32		quadNum = 0;	// current bus info quadlet index
	OFFSET_1394	destOffset;
	uint32		i;
	uint32		maxRetry = 3;	//LM???
	QUADLET		busInfoBlock[BIB_QUAD_ITEMS];
	destOffset.High = 0xffff;

	for (quadNum = 0; (quadNum < BIB_QUAD_ITEMS) && (hResult == NO_ERROR); quadNum++)
	{
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
		{
			sysDebugPrintf("read quad %d for node %d\n\r", quadNum+1, nodeNum);
		}
#endif //_SYSDEBUG

		destOffset.Low = BUS_INFO_BLOCK_ADDR + (quadNum * 4);

		for (i = 0; i < maxRetry; i++)
		{
			hResult = lhlReadNodeTimeout((uint32) (nodeNum | busId), destOffset, 4, &busInfoBlock[quadNum], LHL_QUEUE_PRIORITY, LHL_TX_REQ_SHORT_TIMEOUT_MSECS);
			if (hResult == NO_ERROR)
			{
				break;
			}
#if 1 //LM???
			if (lhlBriNewBusResetDetected(FALSE))
			{
				return E_BRI_NEW_BUS_RESET;
			}
#endif

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
			{
				sysDebugPrintf("failed read quad %d for node %d\n\r", quadNum+1, nodeNum);
			}
#endif //_SYSDEBUG

#if 1 //LM???
			TCTaskWait(10);
#endif
		}
	}	

	if (hResult == NO_ERROR)
	{
		for (quadNum = 0; quadNum < BIB_QUAD_ITEMS; quadNum++)
		{
			pCurBib->busInfoBlock[quadNum] = busInfoBlock[quadNum];
		}
#ifdef _SIMULATE
		pCurBib->busInfoBlock[BIB_WWUIDLO_QUAD] += nodeNum;
#endif //_SIMULATE
	}

	return hResult;
}

#if 1
static BOOL nciIsNodeLinkActive(uint32 nodeNum)
{
	uint32		i;
	QUADLET		*selfIDs = 0;
	BOOL		bActive = FALSE;

	selfIDs = briGetSIDs();

	for (i = 0; i < briGetNumSIDQuads(); i++)
	{
		if (nciExtractSelfidNodeId(selfIDs[i]) == (int32) nodeNum)
		{
			bActive = nciExtractSelfidLinkActive(selfIDs[i]);
			break;
		}
	}

	return bActive;
}
#endif


HRESULT	nciUpdateBusInfoBlocks (uint32 numNodes, BIB_DATA *bibs, BOOL bClear)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeNum = 0;	// current node, index into bifs
	UQUAD		romHeader;		// 1st quadlet in rom
	BIB_DATA	*pCurBib;		// I.E. bib[nodeNum]
	uint16		busId = 0;
	OFFSET_1394	destOffset;
	uint32		i;
// bk:	uint32		maxRetry = 5;	//LM???		3,..,5?
//  retries are blocking fcp outgoing traffic, reducing them to 1 is a temporary fix.
//	AV/C should be reworked to use nodeID-based API's then defer bib reads to the
//  application level. A bib should only be read whenever a handle is required for
//	a specific node
	uint32		maxRetry = 1;
	BOOL		bRetry = FALSE;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
	{
		sysDebugPrintf("Getting bibs for %d nodes\n\r", numNodes);
	}
#endif //_SYSDEBUG

	hResult = briGetBusID(&busId);
	if (hResult != NO_ERROR) return hResult;

	if (bClear) for (nodeNum = 0; nodeNum < numNodes; ++nodeNum)
	{
		// all devices may be accessed by (bus_id | node_id). Insert this
		// here in case the node does not implement general ROM format or
		// does not respond to bus info block reads.
		pCurBib = &bibs[nodeNum]; 
		memset(pCurBib, 0, sizeof(BIB_DATA));
		pCurBib->bibDataRetry = TRUE; //LM??? retry reading bibData
		pCurBib->busInfoBlock[BIB_WWUIDLO_QUAD] = (busId | nodeNum);
	}

	destOffset.High = 0xffff;

	for (i = 0; i < maxRetry; i++)
	{
		bRetry = FALSE;
		for (nodeNum = 0; nodeNum < numNodes; ++nodeNum)
		{
			pCurBib = &bibs[nodeNum]; 

			if (pCurBib->bibDataRetry == TRUE)
			{
				// bail if this bus reset was interrupted
				if (briNewBusReset() == TRUE)
				{
					hResult = E_BRI_NEW_BUS_RESET;
					sysLogError(hResult, __LINE__, moduleName); //LM???
					return hResult; // new bus reset detected elsewhere
				}

#if 1
				if (nciIsNodeLinkActive (nodeNum) == TRUE)
#endif
				{
					// read the first quadlet in config rom for the length byte.
					// If length = 1, the ROM format is minimal and there is no bus info block.
					// If length > 1 (general ROM format), read the 4 quadlets of the bus info block. (see 1394-1995, Section 8.2.3.5)
#ifdef _SYSDEBUG
					if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
					{
						sysDebugPrintf("read ROM base for node %d\n\r", nodeNum);
					}
#endif //_SYSDEBUG

					destOffset.Low = CONFIG_ROM_BASE_ADDR;
					hResult = lhlReadNodeTimeout(nodeNum | busId, destOffset, 4, (QUADLET*)&romHeader, LHL_QUEUE_PRIORITY, LHL_TX_REQ_SHORT_TIMEOUT_MSECS);

					if (hResult != NO_ERROR)
					{
#ifdef _SYSDEBUG
						if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
						{
							sysDebugPrintf("failed read ROM base for node %d\n\r", nodeNum);
						}
#endif //_SYSDEBUG
						// retry but be aware that non-compliant devices will have errors 
						// - this is normal - just skip those devices
						bRetry = TRUE;
					}
					else if (romHeader.b.msb == 0)				// examine length
					{
						hResult = E_NCI_ZERO_INFO_LENGTH;		// 0 is not a valid value	
						sysLogError(hResult, __LINE__, moduleName);
						pCurBib->bibDataRetry = FALSE;
					}
					else if (romHeader.b.msb == 1)
					{
						// minimal format
						pCurBib->bibDataRetry = FALSE;
					}
					else // length > 1, general rom format
					{
						hResult = nciReadNodeGeneralROMFormat (nodeNum, busId, pCurBib);
						if (hResult == NO_ERROR)
						{
							pCurBib->bibDataRetry = FALSE;
							bRetry = TRUE;
						}
					}
				}
#if 1
				else
				{
					pCurBib->bibDataRetry = FALSE;
				}
#endif
			}
		}

		if (bRetry == FALSE)
		{
			break;
		}

		TCTaskWait((i*2+1)*(i*2+1)*10);
	}

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
	{
		sysDebugPrintf("nodeinfos done\n\r");
	}
#endif //_SYSDEBUG

	return hResult;
}


/*****************************************************************************

	nciGetBusInfoBlocks

******************************************************************************/

HRESULT	nciGetBusInfoBlocks (uint32 numNodes, BIB_DATA *bibs)
{
	return nciUpdateBusInfoBlocks (numNodes, bibs, TRUE);
}	

/*****************************************************************************

	nciGetBusInfo

*****************************************************************************/

HRESULT nciGetBusInfo(LAL_BUS_RESET_INFO* plalInfoBlock)
{
	HRESULT		hResult = NO_ERROR;
	uint32		busGen = 0;

	if (briIsSelfIDsValid() != TRUE)
	{
		hResult = E_NCI_BUS_INFO_NOT_AVAILABLE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (nciInfoBlock.bValid != TRUE)
	{
		hResult = E_NCI_NODE_INFO_NOT_AVAILABLE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = briGetCurrentBusGeneration(&busGen);
	if (hResult != NO_ERROR) return hResult;
	nciInfoBlock.busGeneration = busGen;

	hResult = briGetThisNodeAddr(&nciInfoBlock.nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = briGetBusID(&nciInfoBlock.busId);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlBriGetCycleTime(&nciInfoBlock.currentTime);
	if (hResult != NO_ERROR) return hResult;

	plalInfoBlock->currentTime = nciInfoBlock.currentTime;
	plalInfoBlock->busGeneration = nciInfoBlock.busGeneration;
	plalInfoBlock->numNodes = nciInfoBlock.numNodes;
	plalInfoBlock->rootAddr = nciInfoBlock.rootAddr;
	plalInfoBlock->irmAddr = nciInfoBlock.irmAddr;
	plalInfoBlock->numSelfIDs = nciInfoBlock.numSelfIDs;
	plalInfoBlock->nodeAddr = nciInfoBlock.nodeAddr;
	plalInfoBlock->busId = nciInfoBlock.busId;
	plalInfoBlock->SIDBundle = nciInfoBlock.SIDBundle;

	return hResult;
}

/*****************************************************************************

	nciGetNodeInfo

*****************************************************************************/

HRESULT nciGetNodeInfo(uint32 nodeAddr, LAL_DEVICE_INFO *plalDevInfo)
{
	HRESULT	hResult = NO_ERROR;
	uint32	nodeId = (nodeAddr & MASK_NODEID);

	if (plalDevInfo == NULL)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// LM??? this will not work in bridged environment
	if (briIsSelfIDsValid() != TRUE)
	{
		hResult = E_NCI_NODE_INFO_NOT_AVAILABLE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#if 1 //LM???
	else if (nodeAddr == BROADCAST_NODE_ADDR)
	{
		plalDevInfo->active = TRUE;
		plalDevInfo->nodeAddr = nodeAddr;
		plalDevInfo->WWUID.hi = 0;
		plalDevInfo->WWUID.low = BROADCAST_NODE_ADDR;
		plalDevInfo->maxRec = BUSINFO_MAXREC_NONE; //LM???
		return hResult;
	}
#endif
	else if ((nodeAddr < LOCAL_BUS_ID_MASK) || 
			 ((nodeId & MASK_NODEID) >= briGetNumNodes()))
	{
		hResult = E_NCI_NODE_ID_OUT_OF_RANGE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	plalDevInfo->active = TRUE;
	plalDevInfo->nodeAddr = nodeAddr;
	plalDevInfo->WWUID.hi = briGetBusInfoBlockCacheData(nodeId, BIB_WWUIDHI_QUAD);
	plalDevInfo->WWUID.low = briGetBusInfoBlockCacheData(nodeId, BIB_WWUIDLO_QUAD);
	plalDevInfo->maxRec = briGetBusInfoBlockCacheData(nodeId, BIB_NODECAPS_QUAD) & BUSINFO_MAXREC;

	return hResult;
}


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

	Purpose: Node Controller Interface 
				functions for handling topology and speed maps etc.
				
	The functions 'nciBuildNodeTree' and 'nciBuildSpeedMap' borrow heavily from code
	developed by Phillips and distributed with their products.  Hence the following:

	>> Copyright (c) 1999 Philips Semiconductors Inc.
	>> 
	>> Philips hereby grants you permission to copy, modify, and distribute this
	>> software and its documentation.  Philips grants this permission provided
	>> that the above copyright notice appears in all copies and that both the
	>> copyright notice and this permission notice appear in supporting
	>> documentation.  In addition, Philips grants this permission provided that
	>> you prominently mark as "not part of the original" any modifications
	>> made to this software or documentation, and that the name of Philips
	>> Semiconductors, Inc. not be used in advertising or publicity pertaining to
	>> distribution of the software or the documentation without specific,
	>> written prior permission.

	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#ifdef _BM

static char 		moduleName[] = __FILE__;

#include "TCTasking.h"
#include "nciBM.h"
#include "lhlCli.h"
#include "lhlDefs.h"
#include "crc16.h"
#include "csr.h"
#include "lal.h"
#include "bri.h"

#ifdef _IRM
#include "irm.h"
#endif //_IRM

SPEED_MAP				nciSpeedMap;
BOOL					nciSpeedMapValid;
TOPOLOGY_MAP			nciTopologyMap;
BOOL					nciTopologyMapValid;
NODE_DATA				nciNodeData;

#ifdef _BMC_CAPS

SPEED_MAP				nciBMSpeedMap;
BOOL					nciBMSpeedMapValid;
TOPOLOGY_MAP			nciBMTopologyMap;
BOOL					nciBMTopologyMapValid;
NODE_DATA				nciBMNodeData;					// list of all the node's data

static const int32 gapHopTable[] =						// gap count hops table (1394-1995 spec, table 8.7)
{
//	gapCount	  maxHops
		1,	    //	1
		4,	    //	2
		6,	    //	3
		9,	    //	4
	   12,	    //	5
	   14,	    //	6
	   17,	    //	7
	   20,	    //	8
	   23,	    //	9
	   25,	    // 10
	   28,	    // 11
	   31,	    // 12
	   33,	    // 13
	   36,	    // 14
	   39,	    // 15
	   42,	    // 16
};
#endif //_BMC_CAPS

HRESULT	nciBMInitialize(void)
{
	return nciBMReset(TRUE);
}

HRESULT	nciBMBusResetIndication(void)
{
	return nciBMReset(FALSE);
}

HRESULT	nciBMReset(BOOL bInitialize)
{
	HRESULT		hResult = NO_ERROR;

	memset(&nciNodeData, 0, sizeof(nciNodeData));
	memset(&nciTopologyMap, 0, sizeof(nciTopologyMap));
	memset(&nciSpeedMap, 0, sizeof(nciSpeedMap));
	nciTopologyMapValid = FALSE;
	nciSpeedMapValid = FALSE;

#if 1 //LM??? not sure if we should clear these structs on bus reset?
	UNUSED_ARG(bInitialize);
#else
#ifdef _BMC_CAPS 
	if (bInitialize) 
	{
		memset(&nciBMNodeData, 0, sizeof(nciBMNodeData));
		memset(&nciBMTopologyMap, 0, sizeof(nciBMTopologyMap));
		memset(&nciBMSpeedMap, 0, sizeof(nciBMSpeedMap));
		nciBMTopologyMapValid = FALSE;
		nciBMSpeedMapValid = FALSE;
	}
#endif //_BMC_CAPS
#endif

	return hResult;
}

#ifdef _BMC_CAPS
static void getQuadletFromDoublets(QUADLET *quad, DOUBLET lo, DOUBLET hi)
{
	*quad = lo | (hi << 16);
}
#endif //_BMC_CAPS

static void getDoubletsFromQuadlet(QUADLET quad, DOUBLET *lo, DOUBLET *hi)
{
	*lo = (DOUBLET) (quad & 0xFFFF);
	*hi = (DOUBLET) ((quad >> 16) & 0xFFFF);
}

BOOL nciBMIsThisNodeBM(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeAddr = 0;

	hResult = nciBMGetNodeAddrBM(&nodeAddr);
	if (hResult != NO_ERROR) return FALSE;

	return briIsThisNodeAddr(nodeAddr);
}

HRESULT nciBMGetNodeAddrBM(uint32 *pNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint16		busId = 0;

#ifdef _IRM
	hResult = irmGetBMNodeId(pNodeAddr);
	if (hResult != NO_ERROR) return hResult;
#else //_IRM
	hResult = E_NOT_IMPLEMENTED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
#endif //_IRM

	hResult = briGetBusID(&busId);
	if (hResult != NO_ERROR) return hResult;

	if (*pNodeAddr == BUS_MANAGER_ID_UNDEF)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
//		sysLogError(hResult, __LINE__, moduleName);
	}

	*pNodeAddr |= busId;

	return hResult;
}

HRESULT nciBMCheckNodeDataValid(BOOL bBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bValid = FALSE;

	if (bBusMaster == FALSE)
	{
		bValid = nciNodeData.treeInfoValid;
	}
	else
	{
#ifdef _BMC_CAPS
		bValid = nciBMNodeData.treeInfoValid;
#else //_BMC_CAPS
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
#endif //_BMC_CAPS
	}

	if (bValid == FALSE)
	{
		hResult = E_NCI_INVALID_NODE_TREE;
//		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT nciBMCheckSpeedMapValid(BOOL bBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	uint32		generation_number;
	BOOL		bValid = FALSE;
	SPEED_MAP*	speedMap = NULL;
	
	if (bBusMaster == FALSE)
	{
		speedMap = &nciSpeedMap;
		bValid = nciSpeedMapValid;
		hResult = nciBMGetCurrentSpeedMapGeneration (&generation_number, bBusMaster);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
#ifdef _BMC_CAPS
		speedMap = &nciBMSpeedMap;
		bValid = nciBMSpeedMapValid;
		hResult = briGetCurrentBusGeneration (&generation_number);
		if (hResult != NO_ERROR) return hResult;
#else //_BMC_CAPS
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
#endif //_BMC_CAPS
	}

	if (speedMap->generation_number != generation_number)
	{
		bValid = FALSE;
	}

	if (bValid == FALSE)
	{
		hResult = E_NCI_BAD_SPEED_MAP;
//		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT nciBMCheckTopologyMapValid(BOOL bBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	uint32		generation_number;
	BOOL		bValid = FALSE;
	TOPOLOGY_MAP*	topologyMap = NULL;
	
	if (bBusMaster == FALSE)
	{
		topologyMap = &nciTopologyMap;
		bValid = nciTopologyMapValid;
		hResult = nciBMGetCurrentTopologyMapGeneration (&generation_number);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
#ifdef _BMC_CAPS
		topologyMap = &nciBMTopologyMap;
		bValid = nciBMTopologyMapValid;
		hResult = briGetCurrentBusGeneration (&generation_number);
		if (hResult != NO_ERROR) return hResult;
#else //_BMC_CAPS
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
#endif //_BMC_CAPS
	}

	if (topologyMap->generation_number != generation_number)
	{
		bValid = FALSE;
	}

	if (bValid == FALSE)
	{
		hResult = E_NCI_BAD_TOPOLOGY_MAP;
//		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT nciBMGetTopologyMapIndex(BOOL bBusMaster, uint32 index, uint32 *data)
{
	HRESULT			hResult = NO_ERROR;
	uint16			length;
	uint16			crc;
	uint32*			pReg;
	TOPOLOGY_MAP*	topologyMap = NULL;
	
	if (bBusMaster == FALSE)
	{
		topologyMap = &nciTopologyMap;
	}
	else
	{
#ifdef _BMC_CAPS
		topologyMap = &nciBMTopologyMap;
#else //_BMC_CAPS
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
#endif //_BMC_CAPS
	}

	getDoubletsFromQuadlet(topologyMap->length_crc, &crc, &length);
	if (index > length) // length does not include first quadlet
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pReg = (uint32 *) topologyMap;
	*data = pReg[index];

	return hResult;
}

HRESULT nciBMGetSpeedMapIndex(BOOL bBusMaster, uint32 index, uint32 *data)
{
	HRESULT			hResult = NO_ERROR;
	uint16			length;
	uint16			crc;
	uint32*			pReg;
	SPEED_MAP*		speedMap = NULL;
	
	if (bBusMaster == FALSE)
	{
		speedMap = &nciSpeedMap;
	}
	else
	{
#ifdef _BMC_CAPS
		speedMap = &nciBMSpeedMap;
#else //_BMC_CAPS
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
#endif //_BMC_CAPS
	}

	getDoubletsFromQuadlet(speedMap->length_crc, &crc, &length);
	if (index > length) // length does not include first quadlet
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pReg = (uint32 *) speedMap;
	*data = pReg[index];

	return hResult;
}

NODE_TREE *nciBMGetNodeTreeIndex(BOOL bBusMaster, uint32 index)
{
	NODE_TREE	*nodeTree = NULL;

	if (index < MAX_NODES_PER_BUS)
	{
		if (bBusMaster == FALSE)
		{
			nodeTree = &(nciNodeData.nodeTree[index]);
		}
		else
		{
#ifdef _BMC_CAPS
			nodeTree = &(nciBMNodeData.nodeTree[index]);
#else //_BMC_CAPS
			HRESULT 	hResult = E_NOT_IMPLEMENTED;
			sysLogError(hResult, __LINE__, moduleName);
#endif //_BMC_CAPS
		}
	}

	return nodeTree;
}

uint8 nciBMGetSpeedCode(BOOL bBusMaster, uint32 i, uint32 j)
{
	uint32		index;
	uint32		value;
	uint32		shift;
	SPEED_MAP*	speedMap = NULL;
	
	if (i == (uint32)NODE_NONE  || j == (uint32)NODE_NONE)
	{
		return lalSpeedUndef;
	}

	if (bBusMaster == FALSE)
	{
		speedMap = &nciSpeedMap;
	}
	else
	{
#ifdef _BMC_CAPS
		speedMap = &nciBMSpeedMap;
#else //_BMC_CAPS
		HRESULT 	hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return 0;
#endif //_BMC_CAPS
	}

	index = i * 64 + j;
	shift = (3 - (index % 4)) * 8;
	value = speedMap->speed_code[index / 4];
	value >>= shift;
	value &= 0xff;

	return (uint8) value;
}

HRESULT nciBMGetNodeSpeed(uint32 nodeIndex, LAL_SPEED *speed)
{
	HRESULT			hResult = NO_ERROR;
	LOCAL_NODEINFO	localNodeInfo;
	uint8			spd;
	BOOL			bBusMaster = FALSE;

	*speed = lalSpeedDefault;

#ifdef _BMC_CAPS
	bBusMaster = TRUE; // we use local information
#endif //_BMC_CAPS

	hResult = nciBMCheckSpeedMapValid(bBusMaster);
	if (hResult != NO_ERROR) return hResult;

	briGetLocalNodeInfo(&localNodeInfo);
	
	spd = nciBMGetSpeedCode(bBusMaster, localNodeInfo.nodeId, nodeIndex);

	switch (spd)
	{
		case lal100:
		case lal200:
		case lal400:
			*speed = (LAL_SPEED) spd;
			break;
		case lalSpeedFastest:
			*speed = lal400;
			break;
		default:
		case lalSpeedUndef:
			break;
	}

	return hResult;
}

HRESULT nciBMBuildTopologyMapForNode(void)
{
	return nciBMHandleTopologyMapForNode(NCI_BM_BUILD_MAP, NULL);
}

HRESULT nciBMBuildSpeedMapForNode(BOOL bBusMaster)
{
	return nciBMHandleSpeedMapForNode(NCI_BM_BUILD_MAP, NULL, bBusMaster);
}

HRESULT nciBMGetCurrentSpeedMapGeneration (uint32 *generation_number, BOOL bBusMaster)
{
	return nciBMHandleSpeedMapForNode(NCI_BM_GET_GENERATION, generation_number, bBusMaster);
}

HRESULT nciBMGetCurrentTopologyMapGeneration (uint32 *generation_number)
{
	return nciBMHandleTopologyMapForNode(NCI_BM_GET_GENERATION, generation_number);
}

HRESULT nciBMHandleTopologyMapForNode(uint32 mode, uint32 *generation_number)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		channelOffset;
	OFFSET_1394		channelOffsetTemp;
	LAL_NODE_HANDLE	bmHandle = 0;
	uint32			firstQuad1 = 0;
	uint32			firstQuad2 = 0;
	uint16			length = 0;
	uint16			crc = 0;
	BOOL			bValid = FALSE;

	bValid = nciTopologyMapValid;
	nciTopologyMapValid = FALSE;

	// find out BM node address
	hResult = lalGetHandleBM(&bmHandle);
	if (hResult != NO_ERROR) return hResult;

	channelOffset.High = REGISTER_SPACE_ADDR_HI;
	channelOffset.Low  = CSR_TOPOLOGY_MAP_START;
	
	hResult = lalRead1394(bmHandle, channelOffset, 4, &firstQuad1); // read first quadlet length and crc
	if (hResult != NO_ERROR) return hResult;
	TCTaskWait(1);

	getDoubletsFromQuadlet(firstQuad1, &crc, &length);

	if (length == 0 || length > TOPOLOGY_MAP_SELF_ID_QUADLETS + 2)
	{
		hResult = E_NCI_BAD_TOPOLOGY_MAP;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	channelOffsetTemp = channelOffset;
	channelOffsetTemp.Low += 4;

	switch (mode)
	{
		case NCI_BM_BUILD_MAP:
			nciTopologyMap.length_crc = firstQuad1;
			hResult = lalRead1394(bmHandle, channelOffsetTemp, (uint16) (4 * length), (uint32 *) &nciTopologyMap.generation_number); //LM??? block reads ok?
			if (hResult != NO_ERROR) return hResult;
			TCTaskWait(1);
			if (generation_number) *generation_number = nciTopologyMap.generation_number;
			break;
		case NCI_BM_GET_GENERATION:
			hResult = lalRead1394(bmHandle, channelOffsetTemp, 4, generation_number);
			if (hResult != NO_ERROR) return hResult;
			TCTaskWait(1);
			break;
	}

	hResult = lalRead1394(bmHandle, channelOffset, 4, &firstQuad2); // read first quadlet again and compare
	if (hResult != NO_ERROR) return hResult;
	TCTaskWait(1);

	if (firstQuad1 != firstQuad2)
	{
		hResult = E_NCI_BAD_TOPOLOGY_MAP;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (bValid || mode == NCI_BM_BUILD_MAP)
	{
		nciTopologyMapValid = TRUE;
	}

	return hResult;
}

HRESULT nciBMHandleSpeedMapForNode(uint32 mode, uint32 *generation_number, BOOL bBusMaster)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		channelOffset;
	OFFSET_1394		channelOffsetTemp;
	LAL_NODE_HANDLE	bmHandle = 0;
	uint32			firstQuad1 = 0;
	uint32			firstQuad2 = 0;
	uint16			length = 0;
	uint16			crc = 0;
	BOOL			bValid = FALSE;

	bValid = nciSpeedMapValid;
	nciSpeedMapValid = FALSE;

	// find out BM node address
	if (bBusMaster)
	{
		hResult = lalGetThisHandle(&bmHandle);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		hResult = lalGetHandleBM(&bmHandle);
		if (hResult != NO_ERROR) return hResult;
	}

	channelOffset.High = REGISTER_SPACE_ADDR_HI;
	channelOffset.Low  = CSR_SPEED_MAP_START;
	
	hResult = lalRead1394(bmHandle, channelOffset, 4, &firstQuad1); // read first quadlet length and crc
	if (hResult != NO_ERROR) return hResult;
	TCTaskWait(1);

	getDoubletsFromQuadlet(firstQuad1, &crc, &length);

	if (length == 0 || length > SPEED_MAP_SPEED_CODE_QUADLETS + 1)
	{
		hResult = E_NCI_BAD_SPEED_MAP;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	channelOffsetTemp = channelOffset;
	channelOffsetTemp.Low += 4;

	switch (mode)
	{
		case NCI_BM_BUILD_MAP:
			{
#if 1 //LM???
				uint32	numNodes = briGetNumNodes();
				nciNodeData.treeInfoValid = FALSE;
				nciNodeData.numNodes = 0;
				nciNodeData.maxHops = 0;
				hResult = nciBMBuildNodeTree (&nciTopologyMap, nciNodeData.nodeTree);
				if (hResult != NO_ERROR) return hResult;
				nciNodeData.treeInfoValid = TRUE;
				nciNodeData.numNodes = numNodes;
#endif
				if (bValid)
				{
					memset(&nciSpeedMap, 0, sizeof(nciSpeedMap));
				}
				nciSpeedMap.length_crc = firstQuad1;
#if 1 //LM???
				hResult = lalRead1394(bmHandle, channelOffsetTemp, 4, (uint32 *) &nciSpeedMap.generation_number);
				if (hResult != NO_ERROR) return hResult;
				TCTaskWait(1);
				if (generation_number) *generation_number = nciSpeedMap.generation_number;

				{	
					uint32	i, j, index;
					for (i = 0; i < numNodes; i++)
					{
						for (j = i + 1; j < numNodes; j++)
						{
							index = (i * 64 + j) / 4;
							if (nciSpeedMap.speed_code[index] == 0) // not already obtained
							{
								channelOffsetTemp.Low = channelOffset.Low + (index + 2) * 4;
								hResult = lalRead1394(bmHandle, channelOffsetTemp, 4, (uint32 *) &nciSpeedMap.speed_code[index]);
								if (hResult != NO_ERROR) return hResult;
								TCTaskWait(1);
							}
						}
					}
				}
#else
				hResult = lalRead1394(bmHandle, channelOffsetTemp, (uint16) (4 * length), (uint32 *) &nciSpeedMap.generation_number); //LM??? block reads ok?
				if (hResult != NO_ERROR) return hResult;
				TCTaskWait(1);
				if (generation_number) *generation_number = nciSpeedMap.generation_number;
#endif
			}
			break;
		case NCI_BM_GET_GENERATION:
			hResult = lalRead1394(bmHandle, channelOffsetTemp, 4, generation_number);
			if (hResult != NO_ERROR) return hResult;
			TCTaskWait(1);
			break;
	}

	hResult = lalRead1394(bmHandle, channelOffset, 4, &firstQuad2); // read first quadlet again and compare
	if (hResult != NO_ERROR) return hResult;
	TCTaskWait(1);

	if (firstQuad1 != firstQuad2)
	{
		hResult = E_NCI_BAD_SPEED_MAP;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (bValid || mode == NCI_BM_BUILD_MAP)
	{
		nciSpeedMapValid = TRUE;
	}

	return hResult;
}

/*****************************************************************************

	nciBMBuildNodeTree

	Description: Builds a tree from the Topology Map.
	The results are kept in the nodeTree package global.
 
	Originally from Phillips (details at top of this file).  Most of the code 
	"not part of the original" in that names and types have been modified

 **********************************************************************/

HRESULT nciBMBuildNodeTree(	TOPOLOGY_MAP	*pTopologyMap,
							NODE_TREE		*pNodeTree)
{
	HRESULT		hResult = NO_ERROR;
	int16		node, selfid, previous_child, packetnum, fieldsleft, i;
	QUADLET		self_id = 0;
	uint16		numSelfIDs;
	uint16		numNodes;

	if (pTopologyMap == NULL ||
		pNodeTree == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// build the tree
	selfid = 0;
	for (node = 0; node < 64; node++)
	{
		// first, clear the node
		pNodeTree[node].parent = NODE_NONE;
		pNodeTree[node].firstChild = NODE_NONE;
		pNodeTree[node].sibling = NODE_NONE;

		// find the last quadlet of selfid data in this node
		packetnum=0;
		while (pTopologyMap->self_id[selfid+packetnum] & MORE_MASK)
		{
			packetnum++;
		}

		// go from highest numbered child to lowest numbered child
		previous_child = NODE_NONE;
		fieldsleft = 0;

		DO_FOREVER
		{
			// if done current packet, get next one
			if (fieldsleft == 0)
			{
				if (packetnum == -1) break; // done
				self_id = pTopologyMap->self_id[selfid+packetnum];
				
				// if extended packet, has 8 fields, otherwise has 3
				if (packetnum != 0)
				{
					fieldsleft = 8;
				}
				else
				{
					fieldsleft = 3;
					pNodeTree[node].selfid_2 = (uint8)((self_id>>16) & 0xFF);
					pNodeTree[node].selfid_3 = (uint8)((self_id>>8) & 0xFF);
				}
				packetnum--;
			}

			// shift to next child
			self_id >>= 2;
			fieldsleft--;

			if ((self_id & 0x03) == PORT_CHILD)
			{
				// as specified in IEEE 1394-1995, E.3.4, this child will be
				// the highest numbered node less than the current node that
				// does not already have a parent assigned

				for (i = (int16)(node-1); ; i--)
				{
					if (i == -1)
					{
						hResult = E_NCI_BAD_TOPOLOGY_MAP;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
					if (pNodeTree[i].parent == NODE_NONE)
					{
						// set child's parent to this node
						pNodeTree[i].parent = node;
						break;
					}
				}
				// add child to linked list
				if (previous_child == NODE_NONE)
				{
					previous_child = i;
					pNodeTree[node].firstChild = i;
				}
				else
				{
					pNodeTree[previous_child].sibling = i;
					previous_child = i;
				}
			} // end if child
		} // next do for current node

		// advance to the next node in the self id data
		while (pTopologyMap->self_id[selfid] & MORE_MASK)
		{
			selfid++;
		}
		selfid++;

	getDoubletsFromQuadlet(pTopologyMap->node_self_id, &numSelfIDs, &numNodes);

	// did we just finish the last node?
		if (selfid == numSelfIDs)
		{
			if (numNodes != (node + 1))
			{
				hResult = E_NCI_BAD_TOPOLOGY_MAP;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			break;
		}

	} // next for node=0->64

	// build the depth map
	nciBMRecurseDepth(pNodeTree, node, 0);

	// consistency checks
	if ((pNodeTree[0].firstChild != NODE_NONE)	||	// node 0 must be a leaf
		(pNodeTree[node].sibling != NODE_NONE)	||	// root can have no siblings
		(pNodeTree[node].parent != NODE_NONE))		// root can have no parents
	{
		hResult = E_NCI_INVALID_NODE_TREE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

/**********************************************************************
   Function: nciBMRecurseDepth
  
   Description: Recursive function to build depth map.
 
   Originally from Phillips (details at top of this file).  Most of the code 
   "not part of the original" in that names and types have been modified
 
**********************************************************************/
void nciBMRecurseDepth(NODE_TREE *pNodeTree, int16 root, int16 depth)
{
	if (root == NODE_NONE) return;

	pNodeTree[root].depth = depth;

	// children
	nciBMRecurseDepth(pNodeTree, pNodeTree[root].firstChild, (int16)(depth + 1));

	// siblings
	nciBMRecurseDepth(pNodeTree, pNodeTree[root].sibling, depth);
}

#ifdef _BMC_CAPS
HRESULT	nciBMSetBusMaster(void)
{
	HRESULT			hResult = NO_ERROR;
	LOCAL_NODEINFO	localNodeInfo;

	briGetLocalNodeInfo(&localNodeInfo);
	hResult = irmSetBMNodeId(localNodeInfo.nodeId);

	return hResult;
}

void nciBMSetSpeedCode(uint32 i, uint32 j, uint8 speed_code)
{
	uint32 index;
	uint32 value;
	uint32 shift;

	index = i * 64 + j;
	shift = (3 - (index % 4)) * 8;
	value = nciBMSpeedMap.speed_code[index / 4];
	value &= ~(0xff << shift);
	value |= speed_code << shift;

	nciBMSpeedMap.speed_code[index / 4] = value;
}

/*****************************************************************************

	nciBMBuildTopologyMap

	Function builds the Topology map based on a list of self-id packets.
	It performs very little, if any, error checking on the packets.  The
	assumption is that the packets have been validated before calling this 
	function.

******************************************************************************/
HRESULT	nciBMBuildTopologyMap (	QUADLET			*pSelfIDs,
								int32			numSelfIDs,
								int32			numNodes,
								uint32			generation )
{
	HRESULT		hResult = NO_ERROR;
	int32		i;
	uint16		length;
	uint16		crc;

	if (nciBMTopologyMapValid == TRUE)
	{
		memset(&nciBMTopologyMap, 0, sizeof(nciBMTopologyMap));
		nciBMTopologyMapValid = FALSE;
	}

	if (numNodes <= 0 || 
		numNodes >= MAX_NODES_PER_BUS ||
		numSelfIDs < numNodes)
	{
		hResult = E_NCI_NODE_COUNT_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (pSelfIDs == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// copy all self-id packets into the topo map

	for (i = 0; i < numSelfIDs; ++i)
	{
		nciBMTopologyMap.self_id[i] = pSelfIDs[i];
	}

	// fill in header info
	// generation, numNodes and self-id count come from caller's parms
	nciBMTopologyMap.generation_number = generation;
	getQuadletFromDoublets(&nciBMTopologyMap.node_self_id, (uint16) numSelfIDs, (uint16) numNodes);

	length = (uint16)(numSelfIDs + 2);
	// length = number of quadlets _following_ the quadlet containing length, 
	// I.E. 2 + number of self-id packets
	crc = generateCrc16(&nciBMTopologyMap.generation_number, (int32) length);
	// CRC = crc16 of all quadlets following the quadlet containing the crc,
	// I.E. same number of quadlets as length element
	getQuadletFromDoublets(&nciBMTopologyMap.length_crc, crc, length);

	SYS_DEBUG(SYSDEBUG_TRACE_BUSRESET, "topology map done\n\r");

	nciBMTopologyMapValid = TRUE;

	return hResult;
}	

/*****************************************************************************

	nciBMBuildSpeedMap

	Speed map code is lifted from Phillips and modified.  Most of the code 
	"not part of the original" in that names and types have been modified

	Description:  Given the results from BMBusTree, build the speed map.

******************************************************************************/
HRESULT	nciBMBuildSpeedMap(int32 numNodes, uint32 generation)
{
	HRESULT		hResult = NO_ERROR;
	NODE_TREE	*nodeTree;
	int16		m, n, minspeed, curdepth, root, mbranch, nbranch;
	int32		numHops = 0;
	int32		maxHops = 0;
	uint16		length;
	uint16		crc;

	if (nciBMSpeedMapValid == TRUE)
	{
		memset(&nciBMSpeedMap, 0, sizeof(nciBMSpeedMap));
		nciBMSpeedMapValid = FALSE;
	}
	if (nciBMNodeData.treeInfoValid == TRUE)
	{
		memset(&nciBMNodeData, 0, sizeof(nciBMNodeData));
		nciBMNodeData.treeInfoValid = FALSE;
	}

	if (numNodes <= 0 || 
		numNodes >= MAX_NODES_PER_BUS)
	{
		hResult = E_NCI_NODE_COUNT_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = nciBMBuildNodeTree (&nciBMTopologyMap, nciBMNodeData.nodeTree);
	if (hResult != NO_ERROR) return hResult;

	nciBMNodeData.treeInfoValid = TRUE;
	nciBMNodeData.numNodes = numNodes;

	nodeTree = nciBMNodeData.nodeTree;

	root = (int16) (numNodes - 1);

	if (root == 0)
	{  
		nciBMSetSpeedCode(0, 0, (uint8)(nodeTree[0].selfid_3 >> SELF_ID_SPEED_BYTE_SHIFT));
	}
	else
	{
		for (m = 0; m <= root; m++)
		{
			for (n = 0; n <= root - 1; n++)
			{
				curdepth=(int16)MAX(nodeTree[m].depth, nodeTree[n].depth);
				mbranch = m;
				nbranch = n;
				// we're assuming that the speed is contained in the most
				// significant bits of selfid_3, which is true
				minspeed = (int16)(MIN(nodeTree[m].selfid_3, nodeTree[n].selfid_3));
				// climb up the tree until the m branch and the n branch is at
				// the same node
				while (mbranch != nbranch)
				{
					if (nodeTree[mbranch].depth == curdepth)
					{
						mbranch = nodeTree[mbranch].parent;
						minspeed=(int16)(MIN(minspeed, nodeTree[mbranch].selfid_3));
					}
					if (nodeTree[nbranch].depth == curdepth)
					{
						nbranch = nodeTree[nbranch].parent;
						minspeed=(int16)(MIN(minspeed, nodeTree[nbranch].selfid_3));
					}
					curdepth--;
				} 
				numHops = (nodeTree[m].depth - curdepth) +
					(nodeTree[n].depth - curdepth);
				if (numHops > maxHops)
				{
					maxHops = numHops;
				}
				minspeed >>= SELF_ID_SPEED_BYTE_SHIFT;
				nciBMSetSpeedCode(m, n, (uint8) minspeed);
				nciBMSetSpeedCode(n, m, (uint8) minspeed);
			}
		}
	}

	nciBMSpeedMap.generation_number = generation;

	length = (uint16)(SPEED_MAP_SPEED_CODE_QUADLETS + 1);
	// length is in quadlets following the quadlet with length
	// I.E. 1 + number of speed codes
	crc = generateCrc16(&nciBMSpeedMap.generation_number, (int32) length);
	// CRC = crc16 of all quadlets following the quadlet containing the crc,
	// I.E. same number of quadlets as length element
	getQuadletFromDoublets(&nciBMSpeedMap.length_crc, crc, length);

	nciBMNodeData.maxHops = maxHops;

	SYS_DEBUG(SYSDEBUG_TRACE_BUSRESET, "speed map done\n\r");

	nciBMSpeedMapValid = TRUE;

	return hResult;
}

/*****************************************************************************

	nciBMCountMaxHops

	Counts max hops between any 2 nodes.

******************************************************************************/
HRESULT	nciBMCountMaxHops (NODE_DATA *pNodeData, int32 *pMaxHops)
{
	HRESULT		hResult = NO_ERROR;

	if (pNodeData == NULL ||
		pMaxHops == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (pNodeData->numNodes <= 0 || 
		pNodeData->numNodes >= MAX_NODES_PER_BUS)
	{	
		hResult = E_NCI_NODE_COUNT_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (pNodeData->treeInfoValid == FALSE)
	{
		hResult = E_NCI_INVALID_NODE_TREE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pMaxHops = nciBMCountHops(&pNodeData->nodeTree[0], pNodeData->numNodes);

	return hResult;
}

int32 nciBMCountHops(NODE_TREE *nodeTree, int32 numNodes) // same as nciBMBuildSpeedMap above, without the speed map
{
	int16		m, n, curdepth, root, mbranch, nbranch;
	int32		numHops;
	int32		maxHops = 0;

	root = (int16) (numNodes - 1);

	if (root != 0)
	{
		for(m = 0; m <= root; m++)
		{
			for(n = 0; n <= root - 1; n++)
			{
				curdepth = (int16)MAX(nodeTree[m].depth, nodeTree[n].depth);
				mbranch = m;
				nbranch = n;
				// climb up the tree until the m branch and the n branch is at the same node
				while(mbranch!=nbranch)
				{
					if (nodeTree[mbranch].depth == curdepth)
					{
						mbranch = nodeTree[mbranch].parent;
					}
					if (nodeTree[nbranch].depth == curdepth)
					{
						nbranch = nodeTree[nbranch].parent;
					}
					curdepth--;
				} 
				numHops = (nodeTree[m].depth - curdepth) +
					(nodeTree[n].depth - curdepth);
				if (numHops > maxHops)
				{
					maxHops = numHops;
				}
			}
		}

	}
	return maxHops;
}

/*****************************************************************************

	nciBMGapCountViaHops

	Compute gap count based on hops - 
	basically a table lookup, table is in the 1394-1995 spec, table 8.7

******************************************************************************/
HRESULT	nciBMGapCountViaHops (int32 maxHops, int32 *pGapCount)
{
	HRESULT	hResult = NO_ERROR;

	if ((maxHops >= 1) && (maxHops <= 16))
	{
		*pGapCount = gapHopTable[maxHops - 1]; // 0,..,15 in what way?
	}
	else
	{
		*pGapCount = DEFAULT_GAP_COUNT;
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}	

#endif //_BMC_CAPS
#endif //_BM

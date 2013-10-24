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
 * DESCRIPTION: Node Controller Interface Bus Manager functions

	Purpose: Node Controller Interface 
				functions for handling Bus Manager capabilities
				like topology and speed maps etc.
				
	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#ifndef _NCIBM_H
#define _NCIBM_H

#ifdef _BM

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "ErrorCodes.h"
#include "packetBlock.h"
#include "bmdefs.h"
#include "lalDefs.h"

enum
{
	NCI_BM_BUILD_MAP,
	NCI_BM_GET_GENERATION
};

HRESULT		nciBMInitialize(void);
HRESULT		nciBMBusResetIndication(void);
HRESULT		nciBMReset(BOOL bInitialize);
BOOL		nciBMIsThisNodeBM(void);
HRESULT		nciBMGetNodeAddrBM(uint32 *pNodeAddr);

HRESULT		nciBMCheckNodeDataValid(BOOL bBusMaster);
HRESULT		nciBMCheckSpeedMapValid(BOOL bBusMaster);
HRESULT		nciBMCheckTopologyMapValid(BOOL bBusMaster);
HRESULT		nciBMGetTopologyMapIndex(BOOL bBusMaster, uint32 index, uint32 *data);
HRESULT		nciBMGetSpeedMapIndex(BOOL bBusMaster, uint32 index, uint32 *data);
NODE_TREE*	nciBMGetNodeTreeIndex(BOOL bBusMaster, uint32 index);
uint8		nciBMGetSpeedCode(BOOL bBusMaster, uint32 i, uint32 j);
HRESULT		nciBMGetNodeSpeed(uint32 nodeIndex, LAL_SPEED *speed);

HRESULT		nciBMBuildTopologyMapForNode(void);
HRESULT		nciBMBuildSpeedMapForNode(BOOL bBusMaster);
HRESULT		nciBMGetCurrentSpeedMapGeneration (uint32 *generation_number, BOOL bBusMaster);
HRESULT		nciBMGetCurrentTopologyMapGeneration (uint32 *generation_number);
HRESULT		nciBMHandleTopologyMapForNode(uint32 mode, uint32 *generation_number);
HRESULT		nciBMHandleSpeedMapForNode(uint32 mode, uint32 *generation_number, BOOL bBusMaster);
HRESULT		nciBMBuildNodeTree(	TOPOLOGY_MAP *pTopologyMap, NODE_TREE *pNodeTree); // Builds a node tree from a topology map
void 		nciBMRecurseDepth(NODE_TREE *pNodeTree, int16 root, int16 depth);	

#ifdef _BMC_CAPS

void		nciBMSetSpeedCode(uint32 i, uint32 j, uint8 speed_code);
HRESULT		nciBMSetBusMaster(void);

/*************************************************************

	nciBMCountMaxHops

*****************************

	Description:
		Given a valid node tree structure, this function finds the longest path 
		between any 2 nodes.
	Parameters:
		numNodes - INPUT, number of nodes
		nodeData - INPUT, pointer to a node tree structure
		maxHops -  OUTPUT, pointer to variable where maximum hop count will be stored
	Return:
		NO_ERROR indicates success - maxHops contains a valid count
		Any other value indicates failure and the maxHops has a default value
		In the case of failure, the specific error(s) have already been 
		reported to the log device.

	External services used:
		sysLogError

****************************/

HRESULT		nciBMCountMaxHops(	NODE_DATA	*nodeData,
								int32		*maxHops);
					
/*************************************************************

	nciBMBuildTopologyMap

*****************************


	Description:
		Given an array of self-id's, this function builds a topology
		map as defined by the 1394 spec.
		Errors detected will be reported via the sysLog module.  

	Parameters:
		selfIDs - INPUT, array of self-id's from which to build the topo map.
				Self-id's are the actual self-id packets without the logical
				inverse quadlet.  Self-id's are assumed to occur in the order
				they were received.
		numSelfidPackets - number of self-id's in the input array. 
		numNodes - number of nodes in the self-id array
		TopologyMap - OUTPUT, location where the topo map will be built.
		generation - generation field value to use in the topo map.

	Return:
		NO_ERROR indicates success - TopologyMap contains a valid topology map.
		Any other value indicates failure.  In the case of failure, the 
		specific error(s) have already been reported to the log device.

	External services used:
		generateCrc16
		sysLogError

****************************/

HRESULT		nciBMBuildTopologyMap(	QUADLET			*pSelfIDs,
									int32			numSelfIDs,
									int32			numNodes,
									uint32			generation);

/*************************************************************

	nciBMBuildSpeedMap

*****************************

	Description:
		Given a valid topology map this function will build a node data struct
		(incl. a node tree) and from this it builds a speed map 
		as defined by the 1394 spec.
		A by-product of building the speed map is the hop count between each 2 nodes.
		Errors detected will be reported via the sysLog module.  
	Parameters:
		numNodes - INPUT, number of nodes
		generation - INPUT, generation field value to use in the speed map.
	Return:
		NO_ERROR indicates success - speedMap contains a valid speed map.
		Any other value indicates failure and the speed map is not valid.
		In the case of failure, the specific error(s) have already been 
		reported to the log device.

	External services used:
		generateCrc16
		sysLogError

****************************/

HRESULT		nciBMBuildSpeedMap(	int32	numNodes,
								uint32	generation);

/*************************************************************

	nciBMGapCountViaHops

*****************************

	Description:
		Function computes gap count based on maximum number of hops between
		any 2 nodes.  The computation is a simple lookup based on the description
		in the 1394 spec, section 8.4.6.2.
	Parameters:
		hops - INPUT, maximum number of hops between any 2 nodes.  This value is
				normally obtained via BuildSpeedMap or CalculateMaxHops.
		gapCount - OUTPUT, pointer to caller's var where gapCount will be stored.
	Return:
		NO_ERROR indicates success - gapCount is based on the hops computation.
		Any other value indicates failure and gapCount has the default value.
		In the case of failure, the specific error(s) have already been 
		reported to the log device.

	External services used:
		sysLogError

****************************/

HRESULT		nciBMGapCountViaHops(int32 maxHops, int32 *pGapCount);
int32		nciBMCountHops(NODE_TREE *nodeTree, int32 nodeCount);

#endif //_BMC_CAPS
#endif //_BM

#endif

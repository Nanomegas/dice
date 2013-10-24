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
 * DESCRIPTION: Bus Master Defs

	Purpose:
 		Definitions local to the the Bus Management modules
	
	Revisions:
		created 04/09/2002 lm
**********************************************************************/

#ifndef _BMDEFS_H
#define _BMDEFS_H

#include "TCTypes.h"
#include "1394defs.h"

// self-id packet
#define PHY_ID_MASK			(BIT29|BIT28|BIT27|BIT26|BIT25|BIT24)
#define SEQ_NUM_MASK		(BIT22|BIT21|BIT20)
#define	LINK_ACTIVE_MASK	BIT22
#define CONTENDER_MASK		BIT11
#define MORE_MASK			BIT0

// port connection status
#define PORT_NOT_PRESENT	0
#define PORT_NOT_ACTIVE		1
#define PORT_PARENT			2
#define PORT_CHILD			3

/* Gap Count
		The 1394-1995 spec, 8.4.6.2 has conflicting information about what is an
		appropriate default gap count. Either 33 or 42?? 
*/

#define DEFAULT_GAP_COUNT	42


// our bus address
typedef struct
{
	uint16	busId;
	uint16	nodeId;
} LOCAL_NODEINFO;


#define 	SELFID_BUNDLE_SID_ITEMS		189	//LM???

/*
	Self ID bundle
*/
typedef struct
{
	BOOL	bValid;
	int32	numSIDQuads;
	int32	numNodes; 
	QUADLET	sid[SELFID_BUNDLE_SID_ITEMS];
} SELFID_BUNDLE;


/* Node Tree
		All node id's are 6-bit values.  0xffff is used to signal a value of 'none'.
		Most elements are 16 bits because that is the arm's minimum	alignment.
*/	
			 
typedef struct
{
	int16	parent;				// node id of this node's parent
	int16	firstChild;			// node id of child on the highest numbered port
	int16	sibling;			// node id of child on parent's next highest port
	int16	depth;				// depth from root (same as hops from root)
	uint8	selfid_2;			// 2nd byte of this node's self id (contains gap count)
	uint8	selfid_3;			// 3rd byte of this node's self id (contains sp, del, etc)
}	NODE_TREE;

#define NODE_NONE	((int16)-1)

#define BIB_BUSNAME_QUAD		0	// bus name
#define BIB_NODECAPS_QUAD		1	// node capabilities
#define	BIB_WWUIDHI_QUAD		2	// WWUID.High or (0x00000000) if can't obtain
#define	BIB_WWUIDLO_QUAD		3	// WWUID.Low or (busId | nodeNum) if can't obtain
#define	BIB_QUAD_ITEMS			4

typedef struct 
{
	BOOL	bibDataRetry;					// retry flag for obtaining bib
	QUADLET	busInfoBlock[BIB_QUAD_ITEMS];	// bib data
} BIB_DATA;

/* Node Data

	Basically a list of all the node's data.
	Both node tree data and bus info data are ordered by node number, 
	I.E. node number is array index.
*/	

typedef struct 
{
	int32		numNodes;								// how many nodes do we really have, 0 indicates invalid struture
	int32		maxHops;								// what is the max hops in the tree, 0 indicates invalid struture
	BOOL		treeInfoValid;							// validity of tree/topology info, busInfo has its own indicator
	NODE_TREE	nodeTree[MAX_NODES_PER_BUS];			// node data
} NODE_DATA;										

// Topology Map Registers
#define TOPOLOGY_MAP_SELF_ID_QUADLETS	252				// The 1394a draft 3.0 recommends providing for 252 (63*4) packets for forward compatibility

typedef struct
{														//	only using uint32 for platform-independence
	uint32	length_crc;									//	length:16		crc:16
	uint32	generation_number;							//	generation_number:32
	uint32	node_self_id;								//	node_count:16	self_id_count:16
	uint32	self_id[TOPOLOGY_MAP_SELF_ID_QUADLETS];		//	number of self-id packets depends on the actual topology
} TOPOLOGY_MAP;

#define SPEED_MAP_SPEED_CODE_QUADLETS	((64*63)/4)

// Speed Map Registers
typedef struct
{														//	only using uint32 for platform-independence
	uint32	length_crc;									//	length:16		crc:16
	uint32	generation_number;							//	generation_number:32
	uint32	speed_code[SPEED_MAP_SPEED_CODE_QUADLETS];	//	speed_code[64][63]:8
} SPEED_MAP;

/***************************************************************************/

#endif

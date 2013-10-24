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
 * DESCRIPTION: Node Controller Interface Command Line Interface

		nci BM CLI Tools and Variables
	
		These are used by the CLI to view nci BM info.

	Revisions:
		created 13/10/2002 lm
****************************************************************************/
#ifdef _BM
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_NCI_BM
#include "TCTasking.h"
#include "bri.h"
#include "cli.h"
#include "csr.h"
#include "irm.h"
#include "nci.h"
#include "nciBM.h"
#include "nciBMCli.h"
#include "nciCli.h"
#include "targetVendorDefs.h"

static char 		moduleName[] = __FILE__;

enum
{
	TREE_BUF_CONN_DIR = 0,
	TREE_BUF_CONN_SPEED,
	TREE_BUF_CONN_ARROW,
	TREE_BUF_INDEX,
	TREE_BUF_WWUID_HI,
	TREE_BUF_WWUID_LO,
	TREE_BUF_WWUID_NAME,
	TREE_BUF_CAPS,
	TREE_BUF_FRAME,
	TREE_BUF_ITEMS
};

#define	UNKNOWN_DEVICE_NAME		" unknown " // "         "

#define TREE_MAX_HEIGHT		8		// quite small topologies only

#define _TREE_INVERSE_ORDER
//#undef _TREE_INVERSE_ORDER

enum
{
	TREE_DIR_UNDEF = -1,
	TREE_DIR_LEFT,
	TREE_DIR_LEFT_FIRST,
	TREE_DIR_CENTER,
	TREE_DIR_RIGHT,
	TREE_DIR_RIGHT_FIRST,
#ifdef _TREE_INVERSE_ORDER
	TREE_DIR_LEFT_DISP = TREE_DIR_RIGHT,
	TREE_DIR_LEFT_FIRST_DISP = TREE_DIR_RIGHT_FIRST,
	TREE_DIR_RIGHT_DISP = TREE_DIR_LEFT,
	TREE_DIR_RIGHT_FIRST_DISP = TREE_DIR_LEFT_FIRST
#else
	TREE_DIR_LEFT_DISP = TREE_DIR_LEFT,
	TREE_DIR_LEFT_FIRST_DISP = TREE_DIR_LEFT_FIRST,
	TREE_DIR_RIGHT_DISP = TREE_DIR_RIGHT,
	TREE_DIR_RIGHT_FIRST_DISP = TREE_DIR_RIGHT_FIRST
#endif
};

#define	TREE_MAX_POSITION 64
#ifdef _TREE_INVERSE_ORDER
#define TREE_NODE_POSITION(j)	(TREE_MAX_POSITION - 1 - j)
#else
#define TREE_NODE_POSITION(j)	j
#endif

NCI_CLI_NODE_TREE	nciBMCliNodeTree[MAX_NODES_PER_BUS];
char				nciBMCliDumpTreebuf[TREE_MAX_HEIGHT][TREE_BUF_ITEMS][TREE_MAX_POSITION];

uint32	nciBMCliBMNodeId = BUS_MANAGER_ID_UNDEF;
uint32	nciBMCliCMNodeId = CYCLE_MASTER_ID_UNDEF;
uint32	nciBMCliPMNodeId = BUS_MANAGER_ID_UNDEF;
uint32	nciBMCliIRMNodeId = ROOT_ID_UNDEF;

static HRESULT nciBMCliCheckTopologyMapValid(BOOL bBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	
	hResult = nciBMCheckTopologyMapValid(bBusMaster);
	if (hResult != NO_ERROR)
	{
		hResult = nciBMBuildTopologyMapForNode();
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

static HRESULT nciBMCliCheckSpeedMapValid(BOOL bBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	
	hResult = nciBMCheckNodeDataValid(bBusMaster);
	if (hResult == NO_ERROR)
	{
		hResult = nciBMCheckSpeedMapValid(bBusMaster);
	}

	if (hResult != NO_ERROR)
	{
		hResult = nciBMBuildSpeedMapForNode(bBusMaster);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

HRESULT nciBMCliCheckBMInfo(BOOL *pbBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bBusMaster = FALSE;
	
	if (pbBusMaster) *pbBusMaster = bBusMaster;

#ifdef _BMC_CAPS
	bBusMaster = nciBMIsThisNodeBM();
#endif //_BMC_CAPS

	hResult = nciBMCliCheckTopologyMapValid(bBusMaster);
	if (hResult != NO_ERROR)
	{
		if (bBusMaster == FALSE)
		{
			bBusMaster = TRUE;
			hResult = nciBMCliCheckTopologyMapValid(bBusMaster);
			if (hResult != NO_ERROR) return hResult;
		}
	}

	hResult = nciBMCliCheckSpeedMapValid(bBusMaster);
	if (hResult != NO_ERROR)
	{
		if (bBusMaster == FALSE)
		{
			bBusMaster = TRUE;
			hResult = nciBMCliCheckSpeedMapValid(bBusMaster);
			if (hResult != NO_ERROR) return hResult;
		}
	}

	if (pbBusMaster) *pbBusMaster = bBusMaster;

	return hResult;
}

HRESULT nciBMCliDumpBMInfo(void)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bBusMaster = FALSE;

	hResult = nciBMCliCheckBMInfo(&bBusMaster);
	if (hResult != NO_ERROR) return hResult;

	cliOutputLine("");
	if (bBusMaster == TRUE)
	{
		cliOutputLine("**********************************************************");
		cliOutputLine("                        Bus Master                        ");
		cliOutputLine("**********************************************************");
	}
	else
	{
		cliOutputLine("**********************************************************");
		cliOutputLine("                           Node                           ");
		cliOutputLine("**********************************************************");
	}
	hResult = nciBMCliDumpSpeedMap(bBusMaster);
	hResult = nciBMCliDumpNodeTreeTable(bBusMaster);
	hResult = nciBMCliDumpNodeTree(bBusMaster, TREE_INFO_BASIC);
	hResult = nciBMCliDumpNodeTree(bBusMaster, TREE_INFO_ALL);
	cliOutputLine("");

	return hResult;
}

HRESULT nciBMCliDumpSpeedInfo(void)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bBusMaster = FALSE;

	hResult = nciBMCliCheckBMInfo(&bBusMaster);
	if (hResult != NO_ERROR) return hResult;

	hResult = nciBMCliDumpSpeedMap(bBusMaster);

	return hResult;
}

HRESULT nciBMCliDumpTreeInfo(BOOL bAllInfo)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bBusMaster = FALSE;

	hResult = nciBMCliCheckBMInfo(&bBusMaster);
	if (hResult != NO_ERROR) return hResult;

	hResult = nciBMCliDumpNodeTree(bBusMaster, (bAllInfo) ? TREE_INFO_ALL : TREE_INFO_BASIC);

	return hResult;
}

HRESULT nciBMCliDumpSpeedMap(BOOL bBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i, j;
	char		buf[64];

	cliOutputLine("");
	cliOutputLine("node[n] node[m] speed");
	cliOutputLine("---------------------");

	for (i = 0; i < briGetNumNodes(); i++)
	{
		for (j = i + 1; j < briGetNumNodes(); j++)
		{
			sprintf(buf, "%6i  %6i    ", i, j);
			cliOutputRaw(buf);
			nciCliGetSpeedTxt(nciBMGetSpeedCode(bBusMaster, i, j), buf);
			cliOutputLine(buf);
		}
	}
	cliOutputLine("");

	return hResult;
}

HRESULT nciBMCliDumpNodeTreeTable(BOOL bBusMaster)
{
	HRESULT		hResult = NO_ERROR;
	NODE_TREE	*nodeTree;
	uint32		index = 0;

	cliOutputLine("");
	cliOutputLine("id parent 1.child 1.sibling depth selfid(2) selfid(3)");
	cliOutputLine("------------------------------------------------------------");

	for (index = 0; index < briGetNumNodes(); index++)
	{
		nodeTree = nciBMGetNodeTreeIndex(bBusMaster, index);
		nciBMCliDumpNodeTreeEntry(nodeTree, index);
	}
	cliOutputLine("");
	cliOutputLine("------------------------------------------------------------");
	cliOutputLine("");

	return hResult;
}

HRESULT nciBMCliDumpNodeTreeEntry(NODE_TREE *nodeTree, uint32 index)
{
	HRESULT		hResult = NO_ERROR;
	char		buf[64];

	if (nodeTree == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	sprintf(buf, "%2i %6i  %6i  %8i  %4i      0x%02x      0x%02x ",
										index,
										nodeTree->parent,			// node id of this node's parent
										nodeTree->firstChild,		// node id of child on the highest numbered port
										nodeTree->sibling,			// node id of child on parent's next highest port
										nodeTree->depth,			// depth from root (same as hops from root)
										nodeTree->selfid_2,			// 2nd byte of this node's self id (contains gap count)
										nodeTree->selfid_3);		// 3rd byte of this node's self id (contains sp, del, etc)
	cliOutputRaw(buf);
	cliOutputLine("");
	
	return hResult;
}

HRESULT nciBMCliDumpCliNodeTreeTable(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index = 0;

	cliOutputLine("");
	cliOutputLine("id wwuid.hi wwuid.lo dev.name  caps     wdt pos dir spd ");
	cliOutputLine("------------------------------------------------------------");

	for (index = 0; index < briGetNumNodes(); index++)
	{
		nciBMCliDumpCliNodeTreeEntry(index);
	}
	cliOutputLine("");
	cliOutputLine("------------------------------------------------------------");
	cliOutputLine("");

	return hResult;
}

HRESULT nciBMCliDumpCliNodeTreeEntry(uint32 index)
{
	HRESULT		hResult = NO_ERROR;
	char		buf[64];
	char		temp[16];

	nciCliGetSpeedTxt(nciBMCliNodeTree[index].connectionSpeed, temp);

	sprintf(buf, "%2i %08x %08x %s %08x %3i %3i %3i %s",
										index,
										nciBMCliNodeTree[index].info.wwuid.hi,
										nciBMCliNodeTree[index].info.wwuid.low,
										nciBMCliNodeTree[index].info.name,
										nciBMCliNodeTree[index].caps,
										nciBMCliNodeTree[index].width,
										nciBMCliNodeTree[index].position,
										nciBMCliNodeTree[index].connectionDir,
										temp);
	cliOutputRaw(buf);
	cliOutputLine("");
	
	return hResult;
}

HRESULT nciBMCliDumpNodeTreeBuf(uint32 depth, uint32 mode)
{
	HRESULT			hResult = NO_ERROR;
	uint32			i, j;

	for (i = 0; i <= depth; i++)
	{
		for (j = 0; j < TREE_BUF_ITEMS; j++)
		{
			if ((mode & TREE_INFO_ALL) || 
				j == TREE_BUF_CONN_DIR || 
				j == TREE_BUF_CONN_SPEED || 
				j == TREE_BUF_CONN_ARROW || 
				j == TREE_BUF_INDEX)
			{
				if ((j != TREE_BUF_CONN_DIR &&
					 j != TREE_BUF_CONN_SPEED && 
					 j != TREE_BUF_CONN_ARROW) || 
					(i != 0))
				{
					nciBMCliDumpTreebuf[i][j][TREE_MAX_POSITION-1] = 0;
					cliOutputLine(nciBMCliDumpTreebuf[i][j]);
				}
			}
		}
	}

	return hResult;
}

HRESULT nciBMCliDumpNodeTree(BOOL bBusMaster, uint32 mode)
{
	HRESULT			hResult = NO_ERROR;
	NODE_TREE*		nodeTree = NULL;
	uint32			index = 0;
	int16			maxDepth = 0;

	nciBMCliBMNodeId = BUS_MANAGER_ID_UNDEF;
	nciBMCliCMNodeId = CYCLE_MASTER_ID_UNDEF;
	nciBMCliPMNodeId = BUS_MANAGER_ID_UNDEF;

#ifdef _IRM
	irmGetBMNodeId(&nciBMCliBMNodeId);
	irmGetCMNodeId(&nciBMCliCMNodeId);
	irmGetIRMNodeId(&nciBMCliIRMNodeId);
#endif //_IRM

	for (index = 0; index < briGetNumNodes(); index++)
	{
		nodeTree = nciBMGetNodeTreeIndex(bBusMaster, index);
		if (nodeTree == NULL)
		{
			hResult = E_NULL_PTR;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		if (maxDepth < nodeTree->depth)
		{
			maxDepth = nodeTree->depth;
		}
	}

	if (maxDepth >= TREE_MAX_HEIGHT)
	{
		hResult = E_NCI_BAD_TOPOLOGY_MAP;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	memset (nciBMCliNodeTree, 0, sizeof(nciBMCliNodeTree));
	memset (nciBMCliDumpTreebuf, ' ', sizeof(nciBMCliDumpTreebuf));
	nciBMCliDumpSubTree(bBusMaster, briGetNumNodes()-1, 0, NODE_NONE, 0, 0, 0, 0, TREE_CREATE_INFO | (mode & TREE_INFO_ALL));
	nciBMCliDumpSubTree(bBusMaster, briGetNumNodes()-1, 0, NODE_NONE, 0, 0, 0, 0, TREE_CREATE_POS | (mode & TREE_INFO_ALL));
	nciBMCliDumpSubTree(bBusMaster, briGetNumNodes()-1, 0, NODE_NONE, 0, 0, 0, 0, TREE_ADJUST_POS | (mode & TREE_INFO_ALL));
	nciBMCliDumpSubTree(bBusMaster, briGetNumNodes()-1, 0, NODE_NONE, 0, 0, 0, 0, TREE_ADJUST_POS_II | (mode & TREE_INFO_ALL));
	nciBMCliDumpCliNodeTreeTable();
	nciBMCliDumpSubTree(bBusMaster, briGetNumNodes()-1, 0, NODE_NONE, 0, 0, 0, 0, TREE_DISPLAY_INFO | (mode & TREE_INFO_ALL));
	nciBMCliDumpNodeTreeBuf(maxDepth, mode);

	return hResult;
}

static uint32 nciBMCliCalculateSubTreeWidthAtDepth(BOOL bBusMaster, uint32 index, uint32 sibcalculate, uint32 depth)
{
	NODE_TREE			*nodeTree = NULL;
	uint32				widthAtDepth = 1;

	nodeTree = nciBMGetNodeTreeIndex(bBusMaster, index);

	if (depth > 0 && nodeTree->firstChild != NODE_NONE)
	{
		widthAtDepth = nciBMCliCalculateSubTreeWidthAtDepth(bBusMaster, nodeTree->firstChild, TRUE, depth - 1);
	}
	if (sibcalculate && nodeTree->sibling != NODE_NONE)
	{
		widthAtDepth += nciBMCliCalculateSubTreeWidthAtDepth(bBusMaster, nodeTree->sibling, TRUE, depth);
	}

	return widthAtDepth;
}

HRESULT nciBMCliDumpSubTree(BOOL bBusMaster, uint32 index, uint32 number, int16 sibId, uint32 sibwidth, uint32 sibposition, uint32 sibwidth1Depth, uint32 sibwidth1Leaf, uint32 mode)
{
	HRESULT				hResult = NO_ERROR;
	NCI_CLI_NODE_TREE	*nodeTreeCli = NULL;
	NODE_TREE			*nodeTree = NULL;
	NODE_TREE			*tempTree = NULL;
	uint32				siblings = 0;
	int32				width = -1;
	int32				position = 0;
	uint32				depth = 0;
	uint32				scaledwidth = 0;

	nodeTreeCli = &(nciBMCliNodeTree[index]);
	nodeTree = nciBMGetNodeTreeIndex(bBusMaster, index);

	UNUSED_ARG(sibId);

	if (nodeTree == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (mode & TREE_INFO_ALL)
	{
		scaledwidth = DEVICE_LONG_NAME + 3;
	}
	else
	{
		scaledwidth = DEVICE_SHORT_NAME + 1;
	}
	if (mode & TREE_CREATE_POS)
	{
		if (nodeTree->parent == NODE_NONE) // root
		{
			tempTree = nciBMGetNodeTreeIndex(bBusMaster, 0);
			if (tempTree == NULL)
			{
				hResult = E_NULL_PTR;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			nodeTreeCli->position = TREE_MAX_POSITION;
		}
		else
		{
			if (sibposition == 0)
			{
				width = nodeTreeCli->width - nciBMCliNodeTree[nodeTree->parent].width;
				width *= scaledwidth / 2;
				nodeTreeCli->position = nciBMCliNodeTree[nodeTree->parent].position + width;
			}
			else
			{
				BOOL bUpdate = FALSE;

				if (nciBMCliNodeTree[nodeTree->parent].width > nciBMCliNodeTree[nodeTree->parent].children)
				{

					if (index == 7 || index == 2)
					{
						index = index;
					}
					if (nodeTreeCli->width1Leaf)
					{
						if (nodeTreeCli->width1Depth < sibwidth1Depth)
						{
							width = scaledwidth;
							nodeTreeCli->position = sibposition + width;
							bUpdate = TRUE;
						}
					}
					if (sibwidth1Leaf)
					{
						if (sibwidth1Depth < nodeTreeCli->width1Depth)
						{
							width = scaledwidth;
							nodeTreeCli->position = sibposition + width;
							bUpdate = TRUE;
						}
					}
				}

				if (bUpdate == FALSE)
				{
					width = sibwidth + nodeTreeCli->width;
					width *= scaledwidth / 2;
					nodeTreeCli->position = sibposition + width;
				}
			}

			nciBMCliNodeTree[nodeTree->parent].adjusted += nodeTreeCli->position;
		}
	}
	if (mode & TREE_ADJUST_POS)
	{
		if (nodeTreeCli->children > 1)
		{
			nodeTreeCli->adjusted = nodeTreeCli->position - (nodeTreeCli->adjusted + nodeTreeCli->children - 1) / nodeTreeCli->children;
		}
		else
		{
			nodeTreeCli->adjusted = 0;
		}
		if (nodeTree->parent != NODE_NONE)
		{
			if (nciBMCliNodeTree[nodeTree->parent].adjusted != 0)
			{
				nodeTreeCli->position += nciBMCliNodeTree[nodeTree->parent].adjusted;
				nodeTreeCli->adjusted += nciBMCliNodeTree[nodeTree->parent].adjusted;
			}
		}
	}
	if (mode & TREE_DISPLAY_INFO)
	{
		position = TREE_NODE_POSITION(nodeTreeCli->position) - 1;
		if (position < 0 || position >= TREE_MAX_POSITION)
		{
			hResult = E_UNKNOWN;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}

		depth = nodeTree->depth;
		hResult = nciBMCliDisplayNodeConnections(index, depth, position, mode);
		if (hResult != NO_ERROR) return hResult;
		hResult = nciBMCliDisplayNodeInfo(index, depth, position, mode);
		if (hResult != NO_ERROR) return hResult;
		if (mode & TREE_INFO_ALL)
		{
			hResult = nciBMCliDisplayNodeCapabilites(index, depth, position);
			if (hResult != NO_ERROR) return hResult;
			hResult = nciBMCliDisplayNodeFrame(index, depth, position);
			if (hResult != NO_ERROR) return hResult;
		}
	}

	if (nodeTree->firstChild != NODE_NONE)
	{
		hResult = nciBMCliDumpSubTree(bBusMaster, nodeTree->firstChild, 0, NODE_NONE, 0, 0, 0, 0, mode);
		if (hResult != NO_ERROR) return hResult;
	}

	if (nodeTree->sibling != NODE_NONE)
	{
		hResult = nciBMCliDumpSubTree(bBusMaster, nodeTree->sibling, number + 1, (int16) index, nodeTreeCli->width, nodeTreeCli->position, nodeTreeCli->width1Depth, nodeTreeCli->width1Leaf, mode);
		if (hResult != NO_ERROR) return hResult;
	}
	
	if (mode & TREE_CREATE_INFO)
	{
		uint32 siblingId = nodeTree->sibling;
		while (siblingId != (uint32)NODE_NONE)
		{
			tempTree = nciBMGetNodeTreeIndex(bBusMaster, siblingId);
			if (tempTree == NULL)
			{
				hResult = E_NULL_PTR;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			siblingId = tempTree->sibling;
			siblings++;
		}

		if (nodeTree->firstChild == NODE_NONE)
		{
			nodeTreeCli->width = 1;
		}
		if (nodeTree->parent != NODE_NONE)
		{
			nciBMCliNodeTree[nodeTree->parent].width += nodeTreeCli->width;
			nciBMCliNodeTree[nodeTree->parent].children += 1;
		}
		if (nodeTreeCli->children == 1)
		{
			nodeTreeCli->width1Depth += 1;
			nodeTreeCli->width1Leaf = nciBMCliNodeTree[nodeTree->firstChild].width1Leaf;
		}
		else
		{
			nodeTreeCli->width1Depth = 0;
			nodeTreeCli->width1Leaf = (nodeTreeCli->children == 0);
		}
		nodeTreeCli->position = 0; 
		hResult = nciBMCliCreateNodeInfo(index);
		if (hResult != NO_ERROR) return hResult;
		hResult = nciBMCliCreateNodeCapabilities(index, nodeTree);
		if (hResult != NO_ERROR) return hResult;
		hResult = nciBMCliCreateNodeConnections(bBusMaster, nodeTree, index, number, siblings);
		if (hResult != NO_ERROR) return hResult;
	}

	if (mode & TREE_ADJUST_POS_II)
	{
#ifdef _TREE_INVERSE_ORDER
		{
			if (index != 0) 
			{
				nodeTreeCli->position += TREE_NODE_POSITION(nciBMCliNodeTree[0].position - 2);
				if (nodeTreeCli->position < 0)
				{
					hResult = E_UNKNOWN;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				}
			}
			if (nodeTree->parent == NODE_NONE)
			{
				nciBMCliNodeTree[0].position = TREE_NODE_POSITION(2); // done adjusting
			}
		}
#else
		{
			int16 firstChildId = (int16) (briGetNumNodes() - 1);
			while (firstChildId != NODE_NONE)
			{
				tempTree = nciBMGetNodeTreeIndex(bBusMaster, firstChildId);
				if (tempTree == NULL)
				{
					hResult = E_NULL_PTR;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				}
				if (tempTree->firstChild == NODE_NONE)
				{
					break;
				}
				firstChildId = tempTree->firstChild;
			}
			if (firstChildId != NODE_NONE)
			{
				if (index != (uint32) firstChildId) 
				{
					nodeTreeCli->position -= TREE_NODE_POSITION(nciBMCliNodeTree[firstChildId].position - 2);
					if (nodeTreeCli->position < 0)
					{
						hResult = E_UNKNOWN;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
				}
				if (nodeTree->parent == NODE_NONE)
				{
					nciBMCliNodeTree[firstChildId].position = TREE_NODE_POSITION(2); // done adjusting
				}
			}
		}
#endif
	}

	return hResult;
}

HRESULT nciBMCliCreateNodeInfo(uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	LAL_DEVICE_INFO		lalDevInfo;
	uint32				i;
	uint16				busId = 0;
	KNOWN_DEVICE_INFO	*info;						// node info

	info = &nciBMCliNodeTree[index].info;

	briGetBusID(&busId);

	info->wwuid.hi = 0;
	info->wwuid.low = (busId | index);

	hResult = nciGetNodeInfo(info->wwuid.low, &lalDevInfo);
	if (hResult == NO_ERROR)
	{
		info->wwuid.hi = lalDevInfo.WWUID.hi;
		info->wwuid.low = lalDevInfo.WWUID.low;
	}

	if (info->wwuid.hi != 0 ||
		info->wwuid.low != (busId | index))
	{
		targetVendorDefsGetNameFromWWUID(info);
	}
	else
	{
		strcpy(info->name, UNKNOWN_DEVICE_NAME);
	}

	return hResult;
}

HRESULT nciBMCliCreateNodeCapabilities(uint32 index, NODE_TREE *nodeTree)
{
	HRESULT				hResult = NO_ERROR;
	uint32				caps = 0;
	uint32				*nodeCaps = NULL;
	LOCAL_NODEINFO		localNodeInfo;
						
	if (nodeTree == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	briGetLocalNodeInfo(&localNodeInfo);

	nodeCaps = &nciBMCliNodeTree[index].caps;

	caps = briGetBusInfoBlockCacheData(index, BIB_NODECAPS_QUAD);

	*nodeCaps = 0; 

	if (localNodeInfo.nodeId == index)
	{
		*nodeCaps |= LOCAL_NODE_BIT;
	}
	if (index + 1 == briGetNumNodes())
	{
		*nodeCaps |= ROOT_BIT;
	}
	if (nciExtractSelfid2LinkActive (nodeTree->selfid_2))
	{
		*nodeCaps |= LINK_BIT;
	}
	if (nciExtractSelfid3Contender (nodeTree->selfid_3))
	{
		*nodeCaps |= CONTENDER_BIT;
	}
	if (caps & BUSINFO_BMC)
	{
		*nodeCaps |= BMC_BIT;
	}
	if (index == nciBMCliBMNodeId)
	{
		*nodeCaps |= BMID_BIT;
	}
	if (index == nciBMCliBMNodeId)
	{
		*nodeCaps |= BMID_BIT;
	}
	if (caps & BUSINFO_CMC)
	{
		*nodeCaps |= CMC_BIT;
	}
	if (index == nciBMCliIRMNodeId)
	{
		*nodeCaps |= IRMID_BIT;
	}
	if (index == nciBMCliCMNodeId)
	{
		*nodeCaps |= CMID_BIT;
	}
	if (caps & BUSINFO_PMC)
	{
		*nodeCaps |= PMC_BIT;
	}
	if (index == nciBMCliPMNodeId)
	{
		*nodeCaps |= PMID_BIT;
	}
	if (caps & BUSINFO_ISC)
	{
		*nodeCaps |= ISOCH_BIT;
	}

	return hResult;
}

HRESULT nciBMCliCreateNodeConnections(BOOL bBusMaster, NODE_TREE *nodeTree, uint32 index, uint32 number, uint32 siblings)
{
	HRESULT				hResult = NO_ERROR;
	int32				dir = TREE_DIR_UNDEF;

	if (number == siblings)			// directly under
	{
		dir = TREE_DIR_CENTER;
	}
	else if (number < siblings)		// to the left
	{
		if (siblings == 1)
		{
			dir = TREE_DIR_LEFT;
		}
		else
		{
			dir = TREE_DIR_LEFT_FIRST;
		}
	}
	else							// to the right
	{
		if (number == 1)
		{
			dir = TREE_DIR_RIGHT;
		}
		else
		{
			dir = TREE_DIR_RIGHT_FIRST;
		}
	}

	nciBMCliNodeTree[index].connectionSpeed = nciBMGetSpeedCode(bBusMaster, index, nodeTree->parent);
	nciBMCliNodeTree[index].connectionDir = dir;

	return hResult;
}

HRESULT nciBMCliDisplayNodeInfo(uint32 index, uint32 depth, uint32 pos, uint32 mode)
{
	HRESULT				hResult = NO_ERROR;
	KNOWN_DEVICE_INFO	*info;						// node info
	char				temp[16];
	char				*buf1 = NULL;
	char				*buf2 = NULL;
	char				*buf3 = NULL;
	char				*buf4 = NULL;
	LOCAL_NODEINFO		localNodeInfo;
						
	briGetLocalNodeInfo(&localNodeInfo);

	info = &nciBMCliNodeTree[index].info;

	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_INDEX][pos];
	buf2 = &nciBMCliDumpTreebuf[depth][TREE_BUF_WWUID_HI][pos];
	buf3 = &nciBMCliDumpTreebuf[depth][TREE_BUF_WWUID_LO][pos];
	buf4 = &nciBMCliDumpTreebuf[depth][TREE_BUF_WWUID_NAME][pos];
	temp[0] = 0;

	if (mode & TREE_INFO_ALL)
	{
		if (localNodeInfo.nodeId == index)
		{
			sprintf(temp, "  {%2i }  ", index);
		}
		else
		{
			sprintf(temp, "  [%2i ]  ", index);
		}
		memcpy(buf1, temp, DEVICE_LONG_NAME);

		sprintf(temp, "%08x ", info->wwuid.hi);
		memcpy(buf2, temp, DEVICE_LONG_NAME);

		sprintf(temp, "%08x ", info->wwuid.low);
		memcpy(buf3, temp, DEVICE_LONG_NAME);

		memcpy(buf4, info->name, DEVICE_LONG_NAME);
	}
	else
	{
		if (localNodeInfo.nodeId == index)
		{
			sprintf(temp, "{%i}", index);
		}
		else
		{
			sprintf(temp, "[%i]", index);
		}
		memcpy(buf1, temp, DEVICE_SHORT_NAME);
	}

	return hResult;
}

HRESULT nciBMCliDisplayNodeCapabilites(uint32 index, uint32 depth, uint32 pos)
{
	HRESULT				hResult = NO_ERROR;
	uint32				*nodeCaps = NULL;
	uint32				cnt = 0;
	uint32				precnt = 0;
	uint32				postcnt = 0;
	char				temp[16];
	char				temp2[16];
	char				*buf1 = NULL;
	uint32				maxcnt = DEVICE_LONG_NAME;

	nodeCaps = &nciBMCliNodeTree[index].caps;
	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_CAPS][pos];
	temp[0] = 0;
	temp2[0] = 0;

	if ((*nodeCaps & IRM_BITS) == IRM_BITS)
	{
		strcat(temp, "I");
		cnt++;
		*nodeCaps &= ~IRM_BITS;
	}
	if ((*nodeCaps & CONTENDER_BIT) == CONTENDER_BIT)
	{
		strcat(temp, "i");
		cnt++;
		*nodeCaps &= ~CONTENDER_BIT;
	}
	if ((*nodeCaps & BM_BITS) == BM_BITS)
	{
		strcat(temp, "B");
		cnt++;
		*nodeCaps &= ~BM_BITS;
	}
	if ((*nodeCaps & BMC_BIT) == BMC_BIT)
	{
		strcat(temp, "b");
		cnt++;
		*nodeCaps &= ~BMC_BIT;
	}
	if ((*nodeCaps & CM_BITS) == CM_BITS)
	{
		strcat(temp, "C");
		cnt++;
		*nodeCaps &= ~CM_BITS;
	}
	if ((*nodeCaps & CMC_BIT) == CMC_BIT)
	{
		strcat(temp, "c");
		cnt++;
		*nodeCaps &= ~CMC_BIT;
	}
	if ((*nodeCaps & PM_BITS) == PM_BITS)
	{
		strcat(temp, "P");
		cnt++;
		*nodeCaps &= ~PM_BITS;
	}
	if ((*nodeCaps & PMC_BIT) == PMC_BIT)
	{
		strcat(temp, "p");
		cnt++;
		*nodeCaps &= ~PMC_BIT;
	}
	if ((*nodeCaps & ISOCH_BIT) == ISOCH_BIT)
	{
		strcat(temp, "s");
		cnt++;
		*nodeCaps &= ~ISOCH_BIT;
	}
	if (cnt == 0)
	{
		if ((*nodeCaps & LINK_BIT) == LINK_BIT)
		{
			strcat(temp, "L");
			cnt++;
			*nodeCaps &= ~LINK_BIT;
		}
	}
	if (cnt > maxcnt)
	{
		cnt = maxcnt;
		precnt = 0;
		postcnt = 0;
	}
	else
	{
		precnt = (maxcnt - cnt) / 2;
		postcnt = maxcnt - cnt - precnt;
	}

	while (precnt-- > 0)
	{
		strcat(temp2, " ");
	}
	strncat(temp2, temp, cnt);
	
	if (postcnt == 0)
	{
		postcnt = 0;
	}

	while (postcnt-- > 0)
	{
		strcat(temp2, " ");
	}

	memcpy(buf1, temp2, DEVICE_LONG_NAME);
	
	return hResult;
}

HRESULT nciBMCliDisplayNodeConnections(uint32 index, uint32 depth, uint32 pos, uint32 mode)
{
	HRESULT				hResult = NO_ERROR;
	char				temp[16];
	char				temp2[16];
	char				*buf1 = NULL;
	char				*buf2 = NULL;
	char				*buf3 = NULL;

	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_CONN_DIR][pos];
	buf2 = &nciBMCliDumpTreebuf[depth][TREE_BUF_CONN_SPEED][pos];
	buf3 = &nciBMCliDumpTreebuf[depth][TREE_BUF_CONN_ARROW][pos];
	temp[0] = 0;
	temp2[0] = 0;

	if (mode & TREE_INFO_ALL)
	{
		switch (nciBMCliNodeTree[index].connectionDir)
		{
			case TREE_DIR_LEFT_DISP:
			case TREE_DIR_LEFT_FIRST_DISP:
				sprintf(temp, "     /   ");
				break;
			case TREE_DIR_CENTER:
				sprintf(temp, "    |    ");
				break;
			case TREE_DIR_RIGHT_DISP:
			case TREE_DIR_RIGHT_FIRST_DISP:
				sprintf(temp, "   \\     ");
				break;
			default:
				sprintf(temp, "         ");
				break;
		}
		memcpy(buf1, temp, DEVICE_LONG_NAME);

		nciCliGetSpeedTxt(nciBMCliNodeTree[index].connectionSpeed, temp);
		sprintf(temp2, "  (%s)  ", temp);
		memcpy(buf2, temp2, DEVICE_LONG_NAME);

		sprintf(temp, "    |    ");
		memcpy(buf3, temp, DEVICE_LONG_NAME);
	}
	else
	{
		switch (nciBMCliNodeTree[index].connectionDir)
		{
			case TREE_DIR_LEFT_DISP:
			case TREE_DIR_LEFT_FIRST_DISP:
				sprintf(temp, "  /");
				break;
			case TREE_DIR_CENTER:
				sprintf(temp, " | ");
				break;
			case TREE_DIR_RIGHT_DISP:
			case TREE_DIR_RIGHT_FIRST_DISP:
				sprintf(temp, "\\  ");
				break;
			default:
				sprintf(temp, "   ");
				break;
		}
		memcpy(buf1, temp, DEVICE_SHORT_NAME);

		nciCliGetSpeedTxt(nciBMCliNodeTree[index].connectionSpeed, temp);
		memcpy(buf2, temp, DEVICE_SHORT_NAME);

		sprintf(temp, " | ");
		memcpy(buf3, temp, DEVICE_SHORT_NAME);
	}

	return hResult;
}

HRESULT nciBMCliDisplayNodeFrame(uint32 index, uint32 depth, uint32 pos)
{
	HRESULT				hResult = NO_ERROR;
	char				temp[16];
	char				*buf1 = NULL;
	char				*buf2 = NULL;

	UNUSED_ARG(index);

	temp[0] = 0;
	pos -= 1;

	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_INDEX][pos];
	sprintf(temp, "x--");
	memcpy(buf1, temp, 3);
	buf1 += 3 + 5;
	sprintf(temp, "--x ");
	memcpy(buf1, temp, 4);

	buf2 = &nciBMCliDumpTreebuf[depth][TREE_BUF_FRAME][pos];
	sprintf(temp, "x---------x ");
	memcpy(buf2, temp, DEVICE_LONG_NAME + 3);
	
	sprintf(temp, "|");
	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_WWUID_HI][pos];
	memcpy(buf1, temp, 1);
	buf1 += 10;
	memcpy(buf1, temp, 1);
	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_WWUID_LO][pos];
	memcpy(buf1, temp, 1);
	buf1 += 10;
	memcpy(buf1, temp, 1);
	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_WWUID_NAME][pos];
	memcpy(buf1, temp, 1);
	buf1 += 10;
	memcpy(buf1, temp, 1);
	buf1 = &nciBMCliDumpTreebuf[depth][TREE_BUF_CAPS][pos];
	memcpy(buf1, temp, 1);
	buf1 += 10;
	memcpy(buf1, temp, 1);

	return hResult;
}

/*
	Get BM Info Tool
*/
static  char	nciBMInfo_UsageString[]=\
				"nci:bmi: \n\r"\
				"use: nci:bmi\n\r"\
				"------------------------------------------------------------\n\r"\
				"get bus master info\n\r";
static	CLIDescriptor	nciBMInfo_Descriptor[]=
{
	{kCLI_Name,				(uint32)"bmi"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciBMCliDumpBMInfo},
	{kCLI_UsageString, 		(uint32)nciBMInfo_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"bm info"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Speed Info Tool
*/
static  char	nciBMSpeed_UsageString[]=\
				"nci:speed: \n\r"\
				"use: nci:speed\n\r"\
				"------------------------------------------------------------\n\r"\
				"get speed info\n\r";
static	CLIDescriptor	nciBMSpeed_Descriptor[]=
{
	{kCLI_Name,				(uint32)"speed"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciBMCliDumpSpeedInfo},
	{kCLI_UsageString, 		(uint32)nciBMSpeed_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"speed info"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Tree Node Tool
*/
static  char	nciBMTree_UsageString[]=\
				"nci:tree: \n\r"\
				"use: nci:tree <0:basic|1:all>\n\r"\
				"------------------------------------------------------------\n\r"\
				"get tree topology info\n\r";
static	CLIDescriptor	nciBMTree_Descriptor[]=
{
	{kCLI_Name,				(uint32)"tree"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciBMCliDumpTreeInfo},
	{kCLI_UsageString, 		(uint32)nciBMTree_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"topology info"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void nciBMCliInstallTools(void)
{
	HRESULT			hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(nciBMInfo_Descriptor);
	hResult += cliInstallCLIDescriptor(nciBMSpeed_Descriptor);
	hResult += cliInstallCLIDescriptor(nciBMTree_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing nci BM cli tools");
	}
}
#endif //_CLI_TOOL_NCI_BM
#endif //_CLI
#endif //_BM 
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

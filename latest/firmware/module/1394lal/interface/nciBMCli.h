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

#ifndef _NCIBMCLI_H
#define _NCIBMCLI_H
#ifdef _BM
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_NCI_BM
#include "TCTypes.h"
#include "lalDefs.h"
#include "targetVendorDefs.h"

typedef struct
{
	KNOWN_DEVICE_INFO	info;						// node info
	uint32				caps;						// capabilities
	uint32				width;						// width of subtree
	uint32				children;					// number of child nodes
	int32				position;					// position of node
	int32				adjusted;					// adjusted position of node
	uint32				connectionDir;				// to parent node
	uint32				connectionSpeed;			// to parent node
	uint32				width1Depth;				// depth of subtree part with width 1
	uint32				width1Leaf;					// is end of subtree part with width 1 a leaf
} NCI_CLI_NODE_TREE;

enum
{
	TREE_CREATE_INFO		= 0x00000001,
	TREE_CREATE_POS			= 0x00000002,
	TREE_ADJUST_POS			= 0x00000004,
	TREE_ADJUST_POS_II		= 0x00000008,
	TREE_DISPLAY_INFO		= 0x00000010,

	TREE_INFO_BASIC			= 0x00000100,
	TREE_INFO_ALL			= 0x00000200
};

#define LOCAL_NODE_BIT		BIT0
#define ROOT_BIT			BIT1
#define LINK_BIT			BIT2
#define CONTENDER_BIT		BIT3
#define BMC_BIT				BIT5
#define BMID_BIT			BIT6
#define CMC_BIT				BIT7
#define CMID_BIT			BIT8
#define PMC_BIT				BIT9
#define PMID_BIT			BIT10
#define ISOCH_BIT			BIT11
#define IRMID_BIT			BIT12

#define IRM_BITS			(LINK_BIT | CONTENDER_BIT | IRMID_BIT)
#define BM_BITS				(BMC_BIT | BMID_BIT)
#define CM_BITS				(CMC_BIT | CMID_BIT)
#define PM_BITS				(PMC_BIT | PMID_BIT)

HRESULT			nciBMCliCheckBMInfo(BOOL *pbBusMaster);

HRESULT			nciBMCliDumpBMInfo(void);
HRESULT			nciBMCliDumpSpeedInfo(void);
HRESULT			nciBMCliDumpTreeInfo(BOOL bAllInfo);

HRESULT			nciBMCliDumpSpeedMap(BOOL bBusMaster);

HRESULT			nciBMCliDumpNodeTreeBuf(uint32 depth, uint32 mode);
HRESULT			nciBMCliDumpNodeTreeTable(BOOL bBusMaster);
HRESULT			nciBMCliDumpNodeTreeEntry(NODE_TREE *nodeTree, uint32 index);
HRESULT			nciBMCliDumpCliNodeTreeTable(void);
HRESULT			nciBMCliDumpCliNodeTreeEntry(uint32 index);

HRESULT			nciBMCliDumpNodeTree(BOOL bBusMaster, uint32 mode);
HRESULT			nciBMCliDumpSubTree(BOOL bBusMaster, uint32 index, uint32 number, int16 sibId, uint32 sibwidth, uint32 sibposition, uint32 sibwidth1Depth, uint32 sibwidth1Leaf, uint32 mode);
HRESULT			nciBMCliCreateNodeInfo(uint32 index);
HRESULT			nciBMCliCreateNodeCapabilities(uint32 index, NODE_TREE *nodeTree);
HRESULT			nciBMCliCreateNodeConnections(BOOL bBusMaster, NODE_TREE *nodeTree, uint32 index, uint32 number, uint32 siblings);
HRESULT			nciBMCliDisplayNodeInfo(uint32 index, uint32 depth, uint32 pos, uint32 mode);
HRESULT			nciBMCliDisplayNodeCapabilites(uint32 index, uint32 depth, uint32 pos);
HRESULT			nciBMCliDisplayNodeConnections(uint32 index, uint32 depth, uint32 pos, uint32 mode);
HRESULT			nciBMCliDisplayNodeFrame(uint32 index, uint32 depth, uint32 pos);

void			nciBMCliInstallTools(void);

#endif //_CLI_TOOL_NCI_BM
#endif //_CLI
#endif //_BM
#endif //_NCIBMCLI_H

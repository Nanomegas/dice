//===========================================================================
//
// This confidential and proprietary software may be used only as authorized
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2008 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	Config ROM Builder
  
	Description:       
  
	Revisions:
		created 05/10/2008 brian

****************************************************************************/
#ifndef _CRB_H
#define _CRB_H

/** @addtogroup ConfigRomBuilder

	This module is used to build the device Configuration ROM
	@{	
*/

#include "TCTypes.h"

#define CSR_4CHARS_TO_QUADLET(c1,c2,c3,c4) ((c1<<24)|(c2<<16)|(c3<<8)|(c4))

//===========================================================================
// private module structures and functions
/** CRBElement is the node type for all nodes in the binary tree, which represents the ROM

The root element is a static singleton. Generally, the tree looks like:
	
	root_element->pNext = 0 (always)
				|
				->data.pList->immediate_element->pNext->dir_element->pNext->leaf_element->pNext->immediate_element-> etc...		root directory entries
											   |				   |						   |
											   |				   ->data.pLeafData			   ->data.immVal
											   |
											   ->data.pList->element->pNext-> etc...											sub directory entries
																    |
																    ->data.pList->element
*/
typedef struct CRBElement_t
{
	struct CRBElement_t		* pNext;		//!< next element in the directory
	uint32					nElements;		//!< track number of Dir items so we can limit to 256
	uint32					keyType;		//!< upper two bits of key = immediate,dir,offset,leaf
	uint32					keyId;			//!< the rest of the key upper byte

	union
	{
		uint32				immVal;			//!< if immediate entry: lower 3 bytes of entry = value
		struct CRBElement_t	* pList;		//!< if directory entry: point to first item of content
		uint32				* pLeafData;	//!< if leaf: text or raw bytes, formatted/quadlet-aligned for direct copy to ROM
	} data;

	uint32					nodeNum;		//!< count, in order of creation

	// fixup helpers, not used for immediate
	uint32					* pRefFrom;		//!< address in ROM this is refereced from
	uint32					* pImage;		//!< addres in ROM this is stored
} CRBElement;

/// tree node handle
typedef CRBElement * CRBHANDLE;




//===========================================================================
// public module functions

/// module init
HRESULT crbInitialize(void);

/// return the elemet htat represents the root directory
CRBHANDLE crbGetRoot(void);

/// number of quadlets in ROM
uint32 crbGetRomSize(void);

/// deallocate old tree and start a new configuration
HRESULT crbNewConfiguration(BOOL bMinimal, uint32 wwuid_hi, uint32 wwuid_low);

/// change the bus information capability flags
HRESULT crbSetBusInfoCapabilities(uint32 caps);

/// add an immediate directory entry to an existing directory
HRESULT crbAddImmediateEntry(CRBHANDLE dir, uint32 key_id, uint32 value);

/// add a sub directory entry to an existing directory
CRBHANDLE crbAddSubDirectoryEntry(CRBHANDLE dir, uint32 key_id);

/// add a text leaf entry to an existing directory
HRESULT crbAddTextLeafEntry(CRBHANDLE dir, char * str);

// tbd generic leaf entry
// HRESULT crbAddAnyLeafEntry(CRBHANDLE dir, uint32 * data, uint32 numBytes);


//This element function is used by the WalkTree functions below
typedef HRESULT crbElementFunc (CRBHANDLE node, uint32 depth, void * pData);


/// Bredth first tree walk, the root directory is not enumerated, it starts with
/// the members of the Root. The depth variable is always 0 for this walk.
HRESULT crbWalkTreeBFirst(crbElementFunc func, void * pData);

/// Depth first tree walk, the root directory is not enumerated, it starts with
/// the members of the Root. The depth variable indicates the depth of the object.
/// Members of Root dir will have depth 0.
HRESULT crbWalkTreeDFirst(crbElementFunc func, void * pData);

/// Calculate the size needed for the ROM image in quadlets
HRESULT crbCalculateRomSize (uint32 * pSize);

/// builds the ROM using the element tree
HRESULT crbCreateRom(uint32 * pRom, uint32 * pRomQuadSize);



//! @}

#endif // _CRB_H

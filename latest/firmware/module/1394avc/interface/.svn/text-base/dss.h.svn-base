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
  FILE PURPOSE:  

	Descriptor Specifier Services
	-----------------------------
	
	This file implements the "DSS" which manages addressing for Descriptors. 

  Revisions:
		created 06/05/2003 KBJ/LM

****************************************************************************/
#ifndef	_DSS_H
#define _DSS_H
#ifdef _EDS

#include "TCTypes.h"
#include "avcDefs.h"
#include "avcUtils.h"
#include "datastream.h"
#include "packetBlock.h"


#define		DSS_MARKER_ENDOFADDRESS		'dssm'

/*
	Element Structures
	
	These are structures which are returned when an element is added or retrieved.
*/

typedef	struct
{
	uint32	type;	// Type specified in avcDefs.h	
											
	uint32	list_id;	
	uint32	entry_position;
	uint32	object_id;

	uint8	entry_type;
	uint8	list_type;
	
	uint16	info_block_type;
	uint8	info_block_instance_count;
	uint8	info_block_position;
	
} DSS_DESCRIPTOR_SPECIFIER;


/*
	DSSAddress
	
	This structure contains the physical information which describes the
	address to a descriptor on a node.
*/

typedef	struct
{
	BOOL				is_infoblock;
	uint16				size_of_list_id;
	uint16				size_of_object_id;
	uint16				size_of_entry_position;
	uint16				usage_count;
	AVC_ADDRESS_SUBUNIT subunit;
	uint16				entry_count;
	NODEHANDLE			node;
	char				data[500];	
} DSSAddress;

/*
	DSSAddressReference
	
	This is used to describe an INDEX to a DSSAddress. DO NOT USE IT AS A POINTER
	IT IS NOT A POINTER TO A DSSADDRESS!

*/
typedef	uint32	DSSAddressReference;

/*
	
	INITIALIZATION AND ENTRYPOINTS

*/

HRESULT	dssInitialize(void);

HRESULT	dssCreateFromPacket(DSSAddress*	dsa, PB* packet);

HRESULT	dssCreateFromStream(DSSAddress*	dsa, DataStream* ds);

HRESULT	dssGetNthElement(DSSAddress* dsa, uint32 element, DSS_DESCRIPTOR_SPECIFIER* specifier);

HRESULT	dssCountElements(DSSAddress* dsa, uint32* elementcount);

HRESULT	dssGetDataSize(DSSAddress* dsa, uint32* size);

#if 0 // KBJ???, not implemented

HRESULT	dssCreateAsEmpty(DSSAddress* dsa, AVC_ADDRESS_SUBUNIT subunit);

HRESULT	dssAddElement(DSSAddress* dsa, DSS_DESCRIPTOR_SPECIFIER* specifier);

HRESULT	dssSetAsInfoblockReferencePath(DSSAddress* dsa);

HRESULT	dssSetNodeHandle(DSSAddress* dsa, NODEHANDLE nh);

HRESULT	dssSetSubunitIdentifierInfo(DSSAddress*	dsa, uint16 sizeoflistID, 
									uint16 sizeofobjectID, uint16 sizeofentryposition);
#endif

#endif //_EDS
#endif //_DSS_H

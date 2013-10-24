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

	Embedded Descriptor Services (EDS)
	
	This file is the public interface for the EDS. Primarily it describes
	the the interface for the client. There are a few interfaces that are
	used for controlling the EDS server as well.
	
	EDS Client:
		The Client API allows for the construction of descriptors, and for
		construction of the descriptor trees. (Tree are not an actual object
		but rather the description of how the descriptors are arranged 
		relative to one another.)  
		
	EDS Server:
		The Server is used to handle AV/C commands which enable another device
		to query and change the contents of descriptors on this system. 
		
		The server also manages descriptor connections from other devices, and
		makes sure that descriptor reads, occur in a mode which is 
		consistent with the originating node of the read.
		
		Primarily the EDS Server only shares data with the EDS Client interface.

  Revisions:
		created 06/05/2003 KBJ/LM

****************************************************************************/
#ifndef	_EDS_H
#define _EDS_H
#ifdef _EDS

#include	"TCTypes.h"
#include 	"edstypes.h"
#include	"edsServer.h"

HRESULT	edsInitialize(void);
HRESULT	edsInitializeTasks(void);

/***************************************************************************
							Registration/Creating Functions
***************************************************************************/	
HRESULT	edsRegisterSubunitClientSession(EDS_CLIENT_SESSION*	session);

HRESULT	edsCreateSubunitIdentifier(EDS_CLIENT_SESSION* session, EDS_SUBUNITIDENTIFIER* initdata);

HRESULT	edsCreateListDescriptor(EDS_CLIENT_SESSION*	session, uint32 list_type, 
								uint32 list_id, uint8 attribute, 
								uint32 buffersize, void* buffer, 
								EDS_DESCRIPTOR_REF*	  reference);

HRESULT	edsCreateEntryDescriptor(EDS_CLIENT_SESSION* session, uint32 entry_type,
								 uint32	object_id,  uint32 buffersize, void* buffer,
								 EDS_DESCRIPTOR_REF* reference);

HRESULT	edsCreateInfoBlock(EDS_CLIENT_SESSION*	session,  uint32 infoblock_type, 
						   uint32 buffersize, void*	buffer,
						   EDS_DESCRIPTOR_REF*	reference);
								   
HRESULT	edsCreateOpaqueDescriptor(EDS_CLIENT_SESSION*	session, uint32 descriptor_type, 
								  uint32 buffersize, void*  buffer,
								  EDS_DESCRIPTOR_REF* reference);
								   
HRESULT	edsRegisterCustomResolver(EDS_CLIENT_SESSION* session, EDS_CUSTOM_RESOLVER resolver);
								   
/***************************************************************************
							Association Functions
****************************************************************************/

HRESULT	edsSetDataBuffer(EDS_DESCRIPTOR_REF	descriptor, EDS_BUFFER_TYPE buffertype,
						 uint32	buffersize, void* buffer);
								 
HRESULT	edsAssociateChildWithParent(EDS_DESCRIPTOR_REF parent, EDS_DESCRIPTOR_REF child, 
									EDS_BUFFER_TYPE	placement);

HRESULT	edsRemoveParentalAssociation(EDS_DESCRIPTOR_REF child);

HRESULT	edsSetVolatileCallback(EDS_DESCRIPTOR_REF descriptor, EDS_VOLATILE_CALLBACK callback);
		
/***************************************************************************
							Attribute Functions
***************************************************************************/

#if 0 // KBJ???, not implemented

HRESULT	edsListUsesObjectID(EDS_DESCRIPTOR_REF	listdescriptor);

HRESULT	edsSetWritableRanges(EDS_DESCRIPTOR_REF	descriptor, RANGEMAP* writable_buffer_mask,
							 EDS_BUFFER_TYPE	buffertype);

HRESULT	edsGetDescriptorDataLength(EDS_DESCRIPTOR_REF descriptor, EDS_BUFFER_TYPE type,
								   uint32*	bufferlength);

/**************************************************************************
							Data Manipulation Functions
**************************************************************************/

HRESULT	edsReadDescriptor (EDS_DESCRIPTOR_REF	descriptor,  EDS_BUFFER_TYPE type,
						   uint32 startpos, uint32 bytes_to_read, void* data);

HRESULT	edsWriteDescriptor(EDS_DESCRIPTOR_REF	descriptor,  EDS_BUFFER_TYPE type,
						   uint32 startpos, uint32 bytes_to_write, void* data);
								   
/**************************************************************************
							Lookup Routines
**************************************************************************/

HRESULT	edsLookupListID (EDS_CLIENT_SESSION*  session,  uint32 list_id, 
						 EDS_DESCRIPTOR_REF*  found_list);
								 
HRESULT	edsGetNthEntryFromList(EDS_DESCRIPTOR_REF list, uint32 position, 
							   EDS_DESCRIPTOR_REF*	found_entry);
									   
HRESULT	edsGetObjectIDFromList(EDS_DESCRIPTOR_REF list, uint32 object_id,
							   EDS_DESCRIPTOR_REF*	found_entry);

#endif

/**************************************************************************
							Cleanup Functions
**************************************************************************/

HRESULT	edsReleaseDescriptor(EDS_DESCRIPTOR_REF descriptor, BOOL release_children);
						   
#endif //_EDS
#endif //_EDS_H

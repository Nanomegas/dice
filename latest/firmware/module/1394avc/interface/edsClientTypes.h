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

	defines the structures used for most archetypal descriptors.

  Revisions:
		created 06/05/2003 KBJ/LM
****************************************************************************/
#ifndef	_EDS_CLIENT_TYPES_H
#define _EDS_CLIENT_TYPES_H
#ifdef _EDS

#include "TCTypes.h"
#include "rangemaps.h"

// SUPPORTED GENERATION ID 
//#define	EDS_SUPPORTED_GENERATION_ID				1 // AV/C General Spec. 3.0 
												  // Enhancement version 1.1
//KBJ??? when updated
#define	EDS_SUPPORTED_GENERATION_ID				2 // AV/C Descriptor Mech. 1.1

#define	EDS_DESCRIPTORTYPE_LIST					1
#define	EDS_DESCRIPTORTYPE_ENTRY				2
#define	EDS_DESCRIPTORTYPE_INFOBLOCK			3	
#define	EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER	4
#define EDS_DESCRIPTORTYPE_OPAQUE				5
#define	EDS_DESCRIPTORTYPE_NULL					0

#define	EDS_BUFFERTYPE_SUBUNITINFO				10
#define	EDS_BUFFERTYPE_MANUFACTURERINFO			11
#define	EDS_BUFFERTYPE_EXTENDEDINFO				12
#define	EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO	13
#define	EDS_BUFFERTYPE_UNSPECIFIED				0

#define EDS_CALLBACKMODE_VOLATILE				4
#define EDS_CALLBACKMODE_NONE					0

#define	EDS_ACCESSLEVEL_LEVEL0					0		// Flat list access only. 
#define EDS_ACCESSLEVEL_LEVEL1					1		// Entries in a list are directly accessible.
#define EDS_ACCESSLEVEL_LEVEL2					2		// Entries, Lists, and Infoblocks are directly accessible.

#define EDS_CALLBACKOPERATION_WRITE				1		// The data has been written to the descriptor.
#define EDS_CALLBACKOPERATION_READ				2		// The data is about to be read from the descriptor.

/*
	SIZE DEFINITIONS
	
	These describe a size of something in the context of bytes.
*/
#define	EDS_INFOBLOCK_HEADER_SIZE				6


typedef	uint32	EDS_DESCRIPTOR_REF;
typedef BYTE	EDS_BUFFER_TYPE;

													// Fields marked ´*´ is not part of AV/C Desc. specification 
typedef	struct
{ 
	uint8				type;						// list_type
	uint8				attributes;					// Attributes
	uint32				list_id;					// child_list_ID
	
	uint16				si_size;					// list_specific_information_length
	uint16				si_infoblock_count;			// *Parent Child Association counter
	RANGEMAP*			si_writable_mask;			// *
	void*				specific_info;				// list_specific_information

	uint16				number_of_entries;			// Number_of_entry_descriptors
	uint16				extended_infoblock_count;	// Extended_information
} EDS_LISTDESCRIPTOR;

typedef	struct
{
	uint8				type;						// entry_type
	uint16				position;					// *
													// Attributes created in edsReadFromEntryDescriptor
	EDS_DESCRIPTOR_REF	child_list_reference;		// *reference to List (child_list_ID)
	uint32				object_id;					// object_ID
	
	uint16				si_size;					// entry_specific_information_length
	uint16				si_infoblock_count;			// *Parent Child Association counter
	RANGEMAP*			si_writable_mask;			// *
	void*				specific_info;				// entry_specific_information

	uint16				extended_infoblock_count;	// extended_information
} EDS_ENTRYDESCRIPTOR;

typedef	struct
{
	uint16				type;						// info_block_type
	uint16				position; 					// *Position independent field type, so why would I use it?
	uint16				buffertype;					// *Which buffer this attached to
	uint16				primary_fields_length;		// primary_fields_length	
	RANGEMAP*			primary_fields_writable_mask; // *
	void*				primary_fields;				// primary_fields
	uint16				extended_infoblock_count;	// secondary_fields 
} EDS_INFOBLOCK;

typedef	struct
{
	uint8				size_of_list_id;			// size_of_list_ID
	uint8				size_of_object_id;			// size_of_object_ID
	uint8				size_of_entry_position;		// size_of_entry_position
	uint16				number_of_root_lists;		// number_of_root_lists
	
	uint16				si_length;					// subunit_type_dependent_information_length
	uint16				si_infoblock_count;			// *Parent Child Association counter
	RANGEMAP*			si_writable_mask;			// *
	void*				subunit_information;		// subunit_dependent_information
	
	uint16				mi_length;					// manufacturer_dependent_information_length
	uint16				mi_infoblock_count;			// *Parent Child Association counter
	RANGEMAP*			mi_writable_mask;			// *
	void*				manufacturer_information;	// manufacturer_dependent_information

	uint16				extended_infoblock_count;	// extended_information
} EDS_SUBUNITIDENTIFIER;	

typedef	struct										// Custom descriptor
{
	uint32				type; 
	uint16				si_size;
	void*				specific_info;
	uint16				extended_infoblock_count;
} EDS_OPAQUEDESCRIPTOR;

typedef	struct
{
	uint8							type;
	AVC_ADDRESS_SUBUNIT				subunit;
	EDS_DESCRIPTOR_REF				parent_reference;	// Parent descriptor reference
	void*							callback;			// The volatile callback function.
	uint32							callback_mode;
	uint32							writebase;			// when an infoblock or other type of descriptor we add this to every descriptor
														// write, so that it is possible to write to the end of the descriptor.
	union	
	{
		EDS_LISTDESCRIPTOR			list_descriptor;
		EDS_ENTRYDESCRIPTOR			entry_descriptor;
		EDS_INFOBLOCK				info_block;
		EDS_SUBUNITIDENTIFIER		subunit_identifier;
		EDS_OPAQUEDESCRIPTOR		opaque_descriptor;
	}u;
} EDS_DESCRIPTOR;

#endif //_EDS
#endif //_EDS_CLIENT_TYPES_H

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

	defines the structures used for local storage of descriptors. 

  Revisions:
		created 06/05/2003 KBJ/LM
	
****************************************************************************/
#ifndef	_EDSTYPES_H
#define _EDSTYPES_H
#ifdef _EDS

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"
#include "dss.h"
#include "edsClientTypes.h"

/*
	EDS_VOLATILE_CALLBACK
*/
typedef	HRESULT (*EDS_VOLATILE_CALLBACK)(uint16 operation, EDS_DESCRIPTOR_REF  descriptor, EDS_BUFFER_TYPE  buffertype);

/*
	EDS_CUSTOM_RESOLVER
*/
typedef HRESULT (*EDS_CUSTOM_RESOLVER)(DataStream*	ds, EDS_DESCRIPTOR_REF* result);  

/*
	EDS Client API Types
*/	

typedef	struct
{
	AVC_ADDRESS_SUBUNIT	 subunit;
	EDS_DESCRIPTOR_REF	 subunit_desc_ref;
	EDS_CUSTOM_RESOLVER  custom_resolver;
} EDS_CLIENT_SESSION;


/*	
	SUB STRUCTURES for command type.
*/
typedef	struct
{
	BYTE				subfunction;
	BYTE				reserved;
	uint32				node_id;
	BYTE				status;
} EDS_OPEN_DESCRIPTOR;

typedef struct
{
	BYTE				subfunction;
	BYTE				reserved;
	BYTE				read_result_status;
	uint16				data_length;
	uint16				address;
} EDS_READ_DESCRIPTOR;

typedef	struct
{
	BYTE				subfunction;
	BYTE				grouptag;
	uint16				data_length;
	uint16				address;
	uint16				replacement_data_length;
	uint16				original_data_length;
} EDS_WRITE_DESCRIPTOR;

typedef	 struct	 
{
	EDS_DESCRIPTOR_REF	descriptor;
	uint32				startpos;
	uint32				length;
} EDS_DESCRIPTOR_MAPENTRY;


typedef struct		
{
	/*
		FLAGS
		
		THE FLAGS ARE ALL SINGLE BITFIELDS
	*/
	int		in_use	       :1;  		// when this record is actually in use.
	int		busy	       :1;			/* This is used to mark connections as busy. This means that a 
										   request of some sort has come in, but has not yet been responded to. */  
	int		close	       :1;			// Set to true when the connection should be closed in an active manner.
	int		opened_for_read:1;			// When a descriptor has been opened for read we set it here.
	int 	is_finished    :1;			// Set when a connection has been serviced, and now should be
										// repacked and sent as a response.  This is reset to 0 each
										// time a connection is used.
										

	BYTE					avc_headersize;		// When a packet comes in, this tells how long the header
												// of the data was incoming.  This will match in all response
												// cases.
	
	uint16					timeout_countdown;	// Allows for closure via timeouts.

	uint8					accessmode;			// read only, read/write, write only

	/*
		These are the fields of the packet which define the unique connection.
	*/
	AVC_HEADER				avcheader;
	NODEHANDLE				node;
	EDS_DESCRIPTOR_REF		toplevel;			// Top level descriptor set by open command
	EDS_DESCRIPTOR_REF		infoBlock;			// Info block descriptor used by read/write infoBlocks

	/*
		These are data storage fields.
	*/
	DSSAddress				parsed_address;
	EDS_DESCRIPTOR_REF		subunitidentifier_ref;

	
	// This is used for read and write data. All the 
	// data which needs to be written is pushed into here. (minus the AV/C Header)
	// data which is being read by the controller is pushed into here. 
	BYTE					iodata[512];
	
	DataStream				iods;
		

	// 
	//	This buffer is used to create an AV/C response frame.
	//  We used to use the AV/C Handler but it didn't work
	//  logically enough for us.
	//
	BYTE					response[512];	// A copy of the entire contents of the incoming data.
											// BUG BUG THIS SHOULD BE 512 BYTES for a full FCP command.	

	DataStream				response_ds;	// DataStream which points to iodata. This is used
											// when writing out the return descriptor.
	
	EDS_DESCRIPTOR_MAPENTRY	map[100];		// A map of the toplevel descriptor. This map is kept
											// in the context of the connection so that we don't have
											// to create this map each time a read/write operation is
											// made on an open connection.
	uint32					mapcount;

	union
	{
		EDS_OPEN_DESCRIPTOR		open;
		EDS_READ_DESCRIPTOR		read;
		EDS_WRITE_DESCRIPTOR	write;
	}parsed_command;
	
	
} EDS_SERVER_CONNECTION;

#endif //_EDS
#endif //_EDSTYPES_H

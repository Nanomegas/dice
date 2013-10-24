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

	Embedded Descriptor Services Server, Write operations
	
	This file contains the Write Server routines used in the EDS.  
	

  Revisions:
		created 08/08/2003 KBJ
****************************************************************************/

#ifdef _EDS

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCDebug.h"
#include "TCSemaphore.h"
#include "TCTasking.h"
#include "TCTaskingDefs.h"

#include "eds.h"
#include "edstypes.h"
#include "drs.h"
#include "edsServer.h"

#include "lal.h"
#include "fcp.h"
#include "avcDefs.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "packetBlock.h"

#include "sbm.h"
#include "cli.h"

#include "listmanager.h"
#include "1394defs.h"

static	char 						moduleName[] = __FILE__ ;

HRESULT			edsWriteToListDescriptor(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
										  	   uint32* number_of_bytes, void* buffer);
HRESULT			edsWriteToEntryDescriptor(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
										  	   uint32* number_of_bytes, void* buffer);
HRESULT			edsWriteToInfoblockDescriptor(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
										  	   uint32* number_of_bytes, void* buffer);
HRESULT			edsWriteToSubunitIdentifier(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
										  	   uint32* number_of_bytes, void* buffer);
HRESULT			edsWriteToDescriptor(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
										  	   uint32* number_of_bytes, void* buffer);
HRESULT			edsWriteToOpaqueDescriptor(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
										  	   uint32* number_of_bytes, void* buffer);
HRESULT			edsWriteToMap(EDS_SERVER_CONNECTION*	connection, uint32 start, uint32* length);

HRESULT			edsHandleWriteCallback(EDS_DESCRIPTOR_REF	ref, EDS_BUFFER_TYPE buffertype);


/*
	WRITE COMMANDS
	
	These functions are used to make reading writing into descriptors possible.
*/

/*	
	Finds the appropriate object in the map, and position from which to read from it.
	The data read goes to connection->iods, so that we don't have to many more datastreams.
*/
HRESULT	edsWriteToMap(EDS_SERVER_CONNECTION* connection, uint32 start, uint32* length)
{
	uint32						counter=0;
	EDS_DESCRIPTOR_MAPENTRY*	map=connection->map;
	uint32						writelength=0;
	HRESULT						hResult=NO_ERROR;
	uint32						lengthleft=*length;
	uint32						curobjpos=0;
	
	
	// First find the appropriate descriptor in the list
	// 
	while (map->descriptor)
	{
		if (counter+map->length < start)
		{
			counter+=map->length;
			map++;
			continue;	
		}
		
		// If the global start is greater than the global counter, 
		// then we need to adjust the position into the descriptor
		// from which we start the read command.
		if (start > counter)
		{
			curobjpos=start-counter;
		}
		else
		{
			curobjpos=0;	
		} 
		break;	
	}
	
	while (map->descriptor !=0)
	{		
		if (lengthleft+curobjpos > map->length)
		{
			writelength=map->length-curobjpos;
		}		
		else
		{
			writelength=lengthleft;
		}
		
		if (writelength)
		{
			hResult=dsReadChunk(&connection->iods,writelength,(char*)EdsTempRead);	
			if (NO_ERROR !=hResult)
			{
				break;	
			}
			edsWriteToDescriptor(map->descriptor,map->startpos+curobjpos,&writelength,EdsTempRead);	
			// Subtract this here so we can determine how much data has actually been written.
			lengthleft-=writelength;
		}
		
		if (lengthleft==0)
		{
			break;	
		}
		
		map++;
		curobjpos=0; // We only need this to be set the first time.
	}	

	if (map->descriptor==0 && counter+map->length < start)
	{
		// User attempted to read starting from beyond the end of the
		// descriptor. 
		
		hResult=E_EDS_OUT_OF_RANGE;
		sysLogError(hResult, __LINE__, moduleName);
	}
	
	// return the length written.
	(*length)-=lengthleft;

	return hResult;
}


/*
	This function writes an individual descriptor's data.
	It does not write any attached info blocks, or entries.  Those are handled
	by the Descriptor Mapping functions.
*/
HRESULT edsWriteToDescriptor(EDS_DESCRIPTOR_REF	ref, 
							 uint32 startpos,
							 uint32* number_of_bytes, 
							 void* buffer)
{
	HRESULT			hResult=NO_ERROR;
	EDS_DESCRIPTOR*	ddata=NULL;
	uint32			dtype=0;
	
	drsLockDescriptor(ref);
	drsGetDescriptor(ref,&ddata);
	dtype=ddata->type;
	drsUnlockDescriptor(ref);
	
	switch(dtype)
	{
		case EDS_DESCRIPTORTYPE_INFOBLOCK:
		{
			hResult=edsWriteToInfoblockDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;	
		case EDS_DESCRIPTORTYPE_ENTRY:
		{
			hResult=edsWriteToEntryDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;
		case EDS_DESCRIPTORTYPE_OPAQUE:
		{
			hResult=edsWriteToOpaqueDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;
		case EDS_DESCRIPTORTYPE_LIST:
		{
			hResult=edsWriteToListDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;
		case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
		{
			hResult=edsWriteToSubunitIdentifier(ref,startpos,number_of_bytes,buffer);
		}
		break;
	}
	return hResult;
}


/*  
	Writes the infoblock descriptor's fields.
*/
HRESULT	edsWriteToInfoblockDescriptor(EDS_DESCRIPTOR_REF eref, 
									  uint32 startpos,
									  uint32* number_of_bytes, 
									  void* buffer)
{
	HRESULT			hResult=NO_ERROR;
	EDS_DESCRIPTOR*	edata=NULL;
	uint32 			descriptorsize=0;
	uint32			si_start=0;
	uint32			si_end=0;


	SBM_BUFFER		dmap[10];
	
	hResult=sbmCreateNewBuffer(dmap,ARRAYSIZE(dmap));
			
	if (NO_ERROR==hResult)
	{
		hResult=drsLockDescriptor(eref);
		if (NO_ERROR==hResult)
		{
			hResult=drsGetDescriptor(eref,&edata);	
		}
	}
	
	
	// Get the size of the descriptor.
	if (NO_ERROR==hResult)
	{
		hResult=edsGetDescriptorSize(eref, TRUE, &descriptorsize);
		if (NO_ERROR==hResult)
		{
			hResult=sbmAddElementToBuffer(dmap,2,(descriptorsize-2));
		}
	}
	
	//	Infoblock type
	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,2,edata->u.info_block.type);
	}
			
	
	//
	//	Infoblock Specific Info Length
	//
	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,2,edata->u.info_block.primary_fields_length);
	}
	
	if (NO_ERROR==hResult)
	{
		if (edata->u.info_block.primary_fields_length)
		{
			si_start=sbmGetLength(dmap);
			hResult=sbmAddElementToBuffer(dmap,edata->u.info_block.primary_fields_length,(uint32)edata->u.info_block.primary_fields);
			si_end=sbmGetLength(dmap);
		}
	}

	if (NO_ERROR==hResult)
	{		
		hResult=sbmWriteToBuffer(dmap,startpos,number_of_bytes,buffer);	
		if ((startpos >= si_start && startpos <=si_end) ||
			 (startpos+(*number_of_bytes) >= si_start && startpos+(*number_of_bytes) <=si_end))
		{
			edsHandleWriteCallback(eref, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
		}
	}

	
	drsUnlockDescriptor(eref);	
	return hResult;	
}


HRESULT	edsWriteToOpaqueDescriptor(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
										  	   uint32* number_of_bytes, void* buffer)
{
	HRESULT			hResult=NO_ERROR;
	EDS_DESCRIPTOR*		edata=NULL;
	uint32 			descriptorsize=0;
	SBM_BUFFER			dmap[5];
	
	hResult=sbmCreateNewBuffer(dmap,ARRAYSIZE(dmap));
	
	if (NO_ERROR==hResult)
	{
		hResult=drsLockDescriptor(eref);
		if (NO_ERROR==hResult)
		{
			hResult=drsGetDescriptor(eref,&edata);	
		}
	}
	
	// Get the size of the descriptor.
	if (NO_ERROR==hResult)
	{
		hResult=edsGetDescriptorSize(eref, TRUE, &descriptorsize);
		if (NO_ERROR==hResult)
		{
			hResult=sbmAddElementToBuffer(dmap,2,(descriptorsize-2));
		}
	}

	if (NO_ERROR==hResult)
	{
		if (edata->u.opaque_descriptor.si_size)
		{
			sbmAddElementToBuffer(dmap,edata->u.opaque_descriptor.si_size,(uint32)edata->u.opaque_descriptor.specific_info);
		}	
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmWriteToBuffer(dmap,startpos,number_of_bytes,buffer);
		if (startpos >= 2 || startpos+(*number_of_bytes) >= 2)
		{
			edsHandleWriteCallback(eref, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);			
		}	
	}

	return hResult;
}



/*
	Writes the entry descriptor's fields.	
*/
HRESULT	edsWriteToEntryDescriptor(EDS_DESCRIPTOR_REF eref, 
								  uint32 startpos,
								  uint32* number_of_bytes, 
								  void* buffer)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		edata=NULL;
	
	EDS_DESCRIPTOR_REF	si_ref=0;
	EDS_DESCRIPTOR*		si_data=NULL;
	uint8				listidsize=0;
	uint8				objectidsize=0;
	uint32				si_start=0;
	uint32				si_end=0;
	uint32 				descriptorsize=0;
	SBM_BUFFER			dmap[15];

	
	hResult=sbmCreateNewBuffer(dmap,ARRAYSIZE(dmap));

	// 
	if (NO_ERROR==hResult)
	{
		hResult=drsLockDescriptor(eref);
		if (NO_ERROR==hResult)
		{
			hResult=drsGetDescriptor(eref,&edata);	
		}
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=drsFindSubunitIdentifier(edata->subunit,&si_ref);
	}
	
	// Get the size of the descriptor.
	if (NO_ERROR==hResult)
	{
		hResult=edsGetDescriptorSize(eref, TRUE, &descriptorsize);
		if (NO_ERROR==hResult)
		{
			hResult=sbmAddElementToBuffer(dmap,2,(descriptorsize-2));
		}
	}
	
	// Entry type field
	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,1,edata->u.entry_descriptor.type);
	}
	
	// Attributes field
	// Should be doing active data filling here, but currently we don't.
	if (NO_ERROR==hResult)
	{
		uint8	attributebyte=AVC_DESC_ATTRIB_UP_TO_DATE;
		
		if (edata->u.entry_descriptor.child_list_reference)
		{
			attributebyte+=AVC_DESC_ATTRIB_CHILD_ID;	
		}
		
		hResult=sbmAddElementToBuffer(dmap,1,attributebyte);
	}
	
	//	Get subunit identifier stuff
	//
	
	if (NO_ERROR==hResult)
	{
		drsLockDescriptor(si_ref);
		drsGetDescriptor(si_ref,&si_data);
		
		listidsize=si_data->u.subunit_identifier.size_of_list_id;
		objectidsize=si_data->u.subunit_identifier.size_of_object_id;
		
		
		drsUnlockDescriptor(si_ref);
	}
	
	// 
	//	Child List ID
	//
	//	If there is a child list descriptor, then we lookup the descriptor
	//  and get the descriptor's real id.
	//
	if (NO_ERROR==hResult)
	{
		if (edata->u.entry_descriptor.child_list_reference)
		{
			EDS_DESCRIPTOR*			cdata=NULL;
			
			if (NO_ERROR==drsLockDescriptor(edata->u.entry_descriptor.child_list_reference))
			{
				drsGetDescriptor(edata->u.entry_descriptor.child_list_reference,&cdata);
				
				hResult=sbmAddElementToBuffer(dmap,listidsize,cdata->u.list_descriptor.list_id);
			
				drsUnlockDescriptor(edata->u.entry_descriptor.child_list_reference);
			}
		}
	}
	
	//
	//	Object ID
	//
	//  If the object id field is specified as an attribute in the list, then we will
	//  put an object id field in here, unless the objectidsize is 0.
	//
	if (NO_ERROR==hResult && objectidsize)
	{
		EDS_DESCRIPTOR_REF		pref=0;
		EDS_DESCRIPTOR*			pdata=NULL;
		
		pref=edata->parent_reference;
		
		if (NO_ERROR==drsLockDescriptor(pref))
		{
			drsGetDescriptor(pref,&pdata);
			if (pdata->u.list_descriptor.attributes & AVC_DESC_ATTRIB_OBJECT_ID)
			{
				// Only now that we know that this list has object id's
				// and that the subunit defines support for object id's will
				// we actually spit it out.
				sbmAddElementToBuffer(dmap,objectidsize,edata->u.entry_descriptor.object_id);
			}
			drsUnlockDescriptor(pref);	
		}
	}
	
	//
	//	Entry Specific Info Length
	//
	if (NO_ERROR==hResult)
	{
		uint32		si_size=0;		
		edsGetBufferTypeSize(eref, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO,&si_size);		
		
		sbmAddElementToBuffer(dmap,2,si_size);
	}
	
	if (NO_ERROR==hResult)
	{
		if (edata->u.entry_descriptor.si_size)
		{
			si_start=sbmGetLength(dmap);
			sbmAddElementToBuffer(dmap,edata->u.entry_descriptor.si_size,(uint32)edata->u.entry_descriptor.specific_info);	
			si_end=sbmGetLength(dmap);
		}	
	}
	
	if (NO_ERROR==hResult)
	{		
		hResult=sbmWriteToBuffer(dmap,startpos,number_of_bytes,buffer);	

		if ((startpos >= si_start && startpos <=si_end) ||
			 (startpos+(*number_of_bytes) >= si_start && startpos+(*number_of_bytes) <=si_end))
		{
			edsHandleWriteCallback(eref, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
		}
	}
	
	drsUnlockDescriptor(eref);
	
	return hResult;

}


/*
	Writes the entry descriptor's fields.	
*/
HRESULT	edsWriteToSubunitIdentifier(EDS_DESCRIPTOR_REF	eref, 
									uint32 startpos,
								    uint32* number_of_bytes, 
									void* buffer)
{
	HRESULT			hResult=NO_ERROR;
	EDS_DESCRIPTOR*	edata=NULL;
	uint32			mi_start=0;
	uint32			mi_end=0;
	uint32			si_start=0;
	uint32			si_end=0;
		

	uint32 			descriptorsize=0;
		
	SBM_BUFFER		dmap[15];
	
	hResult=sbmCreateNewBuffer(dmap,ARRAYSIZE(dmap));
	
	
	if (NO_ERROR==hResult)
	{
		hResult=drsLockDescriptor(eref);
		if (NO_ERROR==hResult)
		{
			hResult=drsGetDescriptor(eref,&edata);	
		}
	}
	
	// Get the size of the descriptor.
	if (NO_ERROR==hResult)
	{
		hResult=edsGetDescriptorSize(eref, TRUE, &descriptorsize);
		if (NO_ERROR==hResult)
		{
			sbmAddElementToBuffer(dmap,2,(descriptorsize-2));
		}
	}
	
	// Generation ID
	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,1,EDS_SUPPORTED_GENERATION_ID);	
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,1,edata->u.subunit_identifier.size_of_list_id);
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,1,edata->u.subunit_identifier.size_of_object_id);
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,1,edata->u.subunit_identifier.size_of_entry_position);
	}

	if (NO_ERROR==hResult)
	{
		hResult=sbmAddElementToBuffer(dmap,2,edata->u.subunit_identifier.number_of_root_lists);
	}
	
	// Now add root list id's to the dmap
	
	if (NO_ERROR==hResult)
	{
		EDS_DESCRIPTOR_REF		iref;
		EDS_DESCRIPTOR*			idata=NULL;
		uint32					index=1;
		
		while (NO_ERROR==drsGetNthDescriptor(index++,&iref))
		{
			drsLockDescriptor(iref);
			drsGetDescriptor(iref,&idata);
			
			if (idata->parent_reference==eref && idata->type==EDS_DESCRIPTORTYPE_LIST)
			{
				sbmAddElementToBuffer(dmap,edata->u.subunit_identifier.size_of_list_id,idata->u.list_descriptor.list_id);
			}
			
			drsUnlockDescriptor(iref);	
		}
	}	
	
	// Add the subunit and manufacturer specific information to the buffer.
	if (NO_ERROR==hResult)
	{
		uint32	subunitinfo_size=0;
		uint32	manfactinfo_size=0;

		// Here we are getting the size of the subunitinfo field and all attached infoblocks.
		edsGetBufferTypeSize(eref, EDS_BUFFERTYPE_SUBUNITINFO,&subunitinfo_size);		
		sbmAddElementToBuffer(dmap,2,subunitinfo_size);

		if (edata->u.subunit_identifier.subunit_information)
		{
			si_start=sbmGetLength(dmap);
			sbmAddElementToBuffer(dmap,edata->u.subunit_identifier.si_length,(uint32)edata->u.subunit_identifier.subunit_information);	
			si_end=sbmGetLength(dmap);
		}
		
		// Again here we need to know what the sizes of all the attached infoblocks is.
		edsGetBufferTypeSize(eref, EDS_BUFFERTYPE_MANUFACTURERINFO, &manfactinfo_size);
		sbmAddElementToBuffer(dmap,2,manfactinfo_size);
		
		if (edata->u.subunit_identifier.mi_length)
		{
			mi_start=sbmGetLength(dmap);
			sbmAddElementToBuffer(dmap,edata->u.subunit_identifier.mi_length,(uint32)edata->u.subunit_identifier.manufacturer_information);	
			mi_end=sbmGetLength(dmap);
		}
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmWriteToBuffer(dmap,startpos,number_of_bytes,buffer);
		if ((startpos >= si_start && startpos <=si_end) ||
			 (startpos+(*number_of_bytes) >= si_start && startpos+(*number_of_bytes) <=si_end))
		{
			edsHandleWriteCallback(eref, EDS_BUFFERTYPE_SUBUNITINFO);
		}

		if ((startpos >= mi_start && startpos <=mi_end) ||
			 (startpos+(*number_of_bytes) >= mi_start && startpos+(*number_of_bytes) <=mi_end))
		{
			edsHandleWriteCallback(eref, EDS_BUFFERTYPE_MANUFACTURERINFO);
		}
	}
	
	drsUnlockDescriptor(eref);
	
	return hResult;
}


HRESULT	edsWriteToListDescriptor(EDS_DESCRIPTOR_REF	eref, 
								 uint32 startpos,
								 uint32* number_of_bytes, 
								 void* buffer)
{
	HRESULT			hResult=NO_ERROR;
	EDS_DESCRIPTOR*	edata=NULL;
	uint32			bufferstart=0;
	uint32			bufferend=0;

	uint32 			descriptorsize=0;
		
	SBM_BUFFER		dmap[10];
	
	hResult=sbmCreateNewBuffer(dmap,ARRAYSIZE(dmap));
	
	if (NO_ERROR==hResult)
	{
		hResult=drsLockDescriptor(eref);
		if (NO_ERROR==hResult)
		{
			hResult=drsGetDescriptor(eref,&edata);	
		}
	}
	
	// Get the size of the descriptor.
	if (NO_ERROR==hResult)
	{
		hResult=edsGetDescriptorSize(eref, TRUE, &descriptorsize);
		if (NO_ERROR==hResult)
		{
			hResult=sbmAddElementToBuffer(dmap,2,(descriptorsize-2));
		}
	}
	
	if (NO_ERROR==hResult)
	{
		sbmAddElementToBuffer(dmap,1,edata->u.list_descriptor.type);
	}
	
	if (NO_ERROR==hResult)
	{
		sbmAddElementToBuffer(dmap,1,edata->u.list_descriptor.attributes);	
	}
	
	if (NO_ERROR==hResult)
	{
		uint32	si_size=0;		
		edsGetBufferTypeSize(eref, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO,&si_size);		

		sbmAddElementToBuffer(dmap,2,si_size);	
	}
	
	if (NO_ERROR==hResult)
	{
		bufferstart=sbmGetLength(dmap);
		if (edata->u.list_descriptor.si_size)
		{
			sbmAddElementToBuffer(dmap,edata->u.list_descriptor.si_size,(uint32)edata->u.list_descriptor.specific_info);
		}	
		bufferend=sbmGetLength(dmap);
	}
	
	if (NO_ERROR==hResult)
	{
		sbmAddElementToBuffer(dmap,2,edata->u.list_descriptor.number_of_entries);	
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmWriteToBuffer(dmap,startpos,number_of_bytes,buffer);
		if ((startpos >= bufferstart && startpos <=bufferend) ||
			 (startpos+(*number_of_bytes) >= bufferstart && startpos+(*number_of_bytes) <=bufferend))
		{
			edsHandleWriteCallback(eref, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
		}
	}

	return hResult;
}

HRESULT	edsHandleWriteDescriptor(EDS_SERVER_CONNECTION*	connection)
{
	HRESULT		hResult=NO_ERROR;
	uint32		desc_size=0;
	uint32		amountwritten=0;
	uint32		startwrite=0;
	
	/*
		Do initial checks on the command fields.
	*/ 
	hResult=edsGetDescriptorSize(connection->toplevel,TRUE,&desc_size);
	
	if (NO_ERROR==hResult)
	{
		// Check so see if the start position is in the appropriate place.
		if (connection->parsed_command.write.address >= desc_size)
		{
			connection->avcheader.ctype=AVC_RESPONSE_REJECTED;
			return hResult;	
		}
		
		// Check to see if the subfunction is an acceptable format.
		if (connection->parsed_command.write.subfunction!=AVC_DESC_SUBFUNCTION_PARTIAL_REPLACE)
		{
			connection->avcheader.ctype=AVC_RESPONSE_REJECTED;
			return hResult;	
		}
		
		// Check to see if appropriate variables have valid ranges
		// you cannot have a replacement size of 0 with an original size of 0.
		if (connection->parsed_command.write.original_data_length==0 &&
			connection->parsed_command.write.replacement_data_length==0)
		{
			connection->avcheader.ctype=AVC_RESPONSE_REJECTED;
			return hResult;					
		}
		
		// We cannot use different sizes for the original and replacement 
		// sizes. 
		if (connection->parsed_command.write.original_data_length != connection->parsed_command.write.replacement_data_length)
		{
			connection->avcheader.ctype=AVC_RESPONSE_REJECTED;
			return hResult;					
		}
		
		// Check to make sure that the grouptag is set to immediate mode, 
		// otherwise we will return "NOT IMPLEMENTED"
		if (connection->parsed_command.write.grouptag != AVC_DESC_GROUPTAG_IMMEDIATE)
		{
			connection->avcheader.ctype=AVC_RESPONSE_NOT_IMPLEMENTED;
			return hResult;					
		}
		
	}	
	
	/*
		
		WE ONLY SUPPORT PARTIAL REPLACEMENT WRITES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		IF WE GET HERE THAT MEANS THAT THIS IS A PARTIAL REPLACEMENT WRITE!!!!!!!!!!!!!
		THUS ASSUME THAT IS THE CASE FOR THE MEANTIME, IN THE FUTURE WE WILL LIKELY 
		SUPPORT DELETE/INSERT BUT NOT YET!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
	*/
	
	if (connection->parsed_command.write.data_length)
	{
		// Read from a specified address and datalength
		startwrite=connection->parsed_command.write.address;
		amountwritten=connection->parsed_command.write.data_length;
	}

	// When writing to an infoblock, we up the position to start
	// writing from by the size of an infoblock header. (6 bytes)
	// this sets the correct offset.
	if (connection->avcheader.opcode==AVC_CMD_WRITE_INFOBLOCK)
	{
		edsWriteToMap(connection, startwrite+EDS_INFOBLOCK_HEADER_SIZE, &amountwritten);
	}
	else
	{
		edsWriteToMap(connection, startwrite, &amountwritten);	
	}


	/*
		HANDLE THE READ_STATUS_RESULT field of the outgoing packet.
	*/
	connection->avcheader.ctype=AVC_RESPONSE_ACCEPTED;	

	connection->parsed_command.write.subfunction=AVC_DESC_WRITE_SUCCESSFUL;
	

	return hResult;		
}


#ifdef _WINMP
#pragma warning( disable : 4055 )
#endif //_WINMP

/*
	CALLBACK FUNCTIONS
*/
HRESULT	edsHandleWriteCallback(EDS_DESCRIPTOR_REF ref, EDS_BUFFER_TYPE buffertype)
{
	EDS_DESCRIPTOR*		ddata=NULL;
	
	if(NO_ERROR==drsLockDescriptor(ref))
	{
		drsGetDescriptor(ref,&ddata);
		
		switch(ddata->callback_mode)
		{
			case EDS_CALLBACKMODE_VOLATILE:
			{
				if (ddata->callback)
				{
					EDS_VOLATILE_CALLBACK	cb = (EDS_VOLATILE_CALLBACK) ddata->callback;
					uint32					cbresult = NO_ERROR;
				
					cbresult=(*cb)(EDS_CALLBACKOPERATION_WRITE,ref,buffertype);
					if (cbresult != NO_ERROR)
						sysLogError(cbresult, __LINE__, moduleName);
				}
			}
			break;	
		}
		drsUnlockDescriptor(ref);
	}	
	return NO_ERROR;
}

#endif //_EDS

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

	Embedded Descriptor Services Server, Read operations
	
	This file contains the Read Server routines used in the EDS.  
	

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

HRESULT	edsReadFromListDescriptor(EDS_DESCRIPTOR_REF eref, uint32 startpos,
								  uint32* number_of_bytes, void* buffer);
HRESULT	edsReadFromEntryDescriptor(EDS_DESCRIPTOR_REF eref, uint32 startpos,
							  	   uint32* number_of_bytes, void* buffer);
HRESULT	edsReadFromInfoblockDescriptor(EDS_DESCRIPTOR_REF eref, uint32 startpos,
								  	   uint32* number_of_bytes, void* buffer);
HRESULT	edsReadFromSubunitIdentifier(EDS_DESCRIPTOR_REF	eref, uint32 startpos,
							  	     uint32* number_of_bytes, void* buffer);
HRESULT	edsReadFromDescriptor(EDS_DESCRIPTOR_REF eref, uint32 startpos,
							  uint32* number_of_bytes, void* buffer);
HRESULT	edsReadFromOpaqueDescriptor(EDS_DESCRIPTOR_REF eref, uint32 startpos,
	 						    	uint32* number_of_bytes, void* buffer);
HRESULT	edsReadFromMap(EDS_SERVER_CONNECTION* connection, uint32 start, uint32* length);

HRESULT	edsHandleReadCallback(EDS_DESCRIPTOR_REF ref, EDS_BUFFER_TYPE buffertype);

/*	
	Finds the appropriate object in the map, and position from which to read from it.
	The data read goes to connection->iods, so that we don't have to many more datastreams.
*/
HRESULT	edsReadFromMap(EDS_SERVER_CONNECTION* connection, 
					   uint32 start, 
					   uint32* length)
{
	uint32						counter=0;
	EDS_DESCRIPTOR_MAPENTRY*	map=connection->map;
	uint32						readlength=0;
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
	
	// Start reading data from descriptors from here and beyond,
	// the first read, we use curobjpos to make sure that we are reading
	// from the appropriate starting point in the current descriptor, afterwards
	// we know that we are just reading from the beginning of descriptors.
	while (map->descriptor !=0)
	{
		/* KBJ???
		if (lengthleft > map->length)
		{
			readlength=map->length;
		}	
		else 
		*/
		if (lengthleft+curobjpos > map->length)
		{
			readlength=map->length-curobjpos;	
		}	
		else
		{
			readlength=lengthleft;
		}
		
		
		
		edsReadFromDescriptor(map->descriptor,map->startpos+curobjpos,&readlength,EdsTempRead);	
		
		// Subtract this here so we can determine how much data has actually been read.
		lengthleft-=readlength;
		
		if (readlength)
		{
			hResult=dsWriteChunk(&connection->iods,readlength,(char*)EdsTempRead);	
			if (NO_ERROR !=hResult)
			{
				break;	
			}
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
// bkbk: not always an error, refine this
//		sysLogError(hResult, __LINE__, moduleName);
	}
	
	// return the length read.
	(*length)-=lengthleft;

	return hResult;
}

/*
	This function reads an individual descriptor's data.
	It does not read any attached info blocks, or entries.  Those are handled
	by the Descriptor Mapping functions.
*/
HRESULT	edsReadFromDescriptor(EDS_DESCRIPTOR_REF ref, 
							  uint32 startpos,
							  uint32* number_of_bytes, 
							  void* buffer)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		ddata=NULL;
	uint32				dtype=0;
	
	drsLockDescriptor(ref);
	drsGetDescriptor(ref,&ddata);
	dtype=ddata->type;
	drsUnlockDescriptor(ref);
	
	switch(dtype)
	{
		case	EDS_DESCRIPTORTYPE_INFOBLOCK:
		{
			hResult=edsReadFromInfoblockDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;	
		case 	EDS_DESCRIPTORTYPE_ENTRY:
		{
			hResult=edsReadFromEntryDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;
		case 	EDS_DESCRIPTORTYPE_OPAQUE:
		{
			hResult=edsReadFromOpaqueDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;
		case	EDS_DESCRIPTORTYPE_LIST:
		{
			hResult=edsReadFromListDescriptor(ref,startpos,number_of_bytes,buffer);
		}
		break;
		case	EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
		{
			hResult=edsReadFromSubunitIdentifier(ref,startpos,number_of_bytes,buffer);
		}
		break;
	}
	return hResult;
}


/*  
	Reads the infoblock descriptor's fields.
*/
HRESULT	edsReadFromInfoblockDescriptor(EDS_DESCRIPTOR_REF	eref, 
									   uint32 startpos,
									   uint32*  number_of_bytes, 
									   void* buffer)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		edata=NULL;
	uint32 				descriptorsize=0;

	SBM_BUFFER			dmap[10];
	
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
			hResult=sbmAddElementToBuffer(dmap,edata->u.info_block.primary_fields_length,(uint32)edata->u.info_block.primary_fields);
		}
	}

	if (NO_ERROR==hResult)
	{
		hResult=sbmReadFromBuffer(dmap,startpos,number_of_bytes,buffer);	
	}

	
	drsUnlockDescriptor(eref);	
	return hResult;	
}



/*
	Reads the entry descriptor's fields.	
*/
HRESULT	edsReadFromEntryDescriptor(EDS_DESCRIPTOR_REF eref, 
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
		hResult=drsFindSubunitIdentifier(edata->subunit, &si_ref);
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
			sbmAddElementToBuffer(dmap,edata->u.entry_descriptor.si_size,(uint32)edata->u.entry_descriptor.specific_info);	
		}	
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmReadFromBuffer(dmap,startpos,number_of_bytes,buffer);	
	}
	
	drsUnlockDescriptor(eref);
	
	return hResult;

}


/*
	Reads the entry descriptor's fields.	
*/
HRESULT	edsReadFromSubunitIdentifier(EDS_DESCRIPTOR_REF	eref, 
									 uint32 startpos,
									 uint32* number_of_bytes, 
									 void* buffer)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		edata=NULL;
	uint32 				descriptorsize=0;
	SBM_BUFFER			dmap[30];
	
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
		uint32				index=1;
		
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
			sbmAddElementToBuffer(dmap,edata->u.subunit_identifier.si_length,(uint32)edata->u.subunit_identifier.subunit_information);	
		}
		
		// Again here we need to know what the sizes of all the attached infoblocks is.
		edsGetBufferTypeSize(eref, EDS_BUFFERTYPE_MANUFACTURERINFO, &manfactinfo_size);
		sbmAddElementToBuffer(dmap,2,manfactinfo_size);
		
		if (edata->u.subunit_identifier.mi_length)
		{
			sbmAddElementToBuffer(dmap,edata->u.subunit_identifier.mi_length,(uint32)edata->u.subunit_identifier.manufacturer_information);	
		}
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmReadFromBuffer(dmap,startpos,number_of_bytes,buffer);	
	}
	
	drsUnlockDescriptor(eref);
	
	return hResult;
}


HRESULT	edsReadFromListDescriptor(EDS_DESCRIPTOR_REF eref, 
								  uint32 startpos,
								  uint32* number_of_bytes, 
								  void* buffer)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		edata=NULL;
	
	uint32 				descriptorsize=0;
		
	SBM_BUFFER			dmap[10];
	
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
		if (edata->u.list_descriptor.si_size)
		{
			sbmAddElementToBuffer(dmap,edata->u.list_descriptor.si_size,(uint32)edata->u.list_descriptor.specific_info);
		}	
	}
	
	if (NO_ERROR==hResult)
	{
		sbmAddElementToBuffer(dmap,2,edata->u.list_descriptor.number_of_entries);	
	}
	
	if (NO_ERROR==hResult)
	{
		hResult=sbmReadFromBuffer(dmap,startpos,number_of_bytes,buffer);	
	}

	return hResult;
}

/*
	edsReadFromOpaqueDescriptor
	
	This descriptor is almost entirely a custom descriptor.
*/

HRESULT	edsReadFromOpaqueDescriptor(EDS_DESCRIPTOR_REF eref, 
									uint32 startpos,
									uint32* number_of_bytes, 
									void* buffer)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		edata=NULL;
	

	uint32 				descriptorsize=0;
		
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
		hResult=sbmReadFromBuffer(dmap,startpos,number_of_bytes,buffer);	
	}

	return hResult;
}

/*
		edsHandleReadDescriptor
		
		This function is responsible for determining where the read is starting from
		and if it is at the same point we left off at last time, then we start from
		the cached object's position.
*/

HRESULT	edsHandleReadDescriptor(EDS_SERVER_CONNECTION*	connection)
{
	HRESULT		hResult=NO_ERROR;
	uint32		desc_size=0;
	uint32		amountread=0;
	uint32		startread=0;
	uint32		amounttoread=0;
	uint32		moretoread = 0;

	/*
		Check to see if the position is valid, no 
		starting outside of the descriptor's length.
	*/ 
	hResult=edsGetDescriptorSize(connection->toplevel,TRUE,&desc_size);
	
	if (NO_ERROR==hResult)
	{
		// Okay bad start position.
		if (connection->parsed_command.read.address >= desc_size)
		{
			connection->avcheader.ctype=AVC_RESPONSE_REJECTED;
			return hResult;	
		}
	}	
	

	// Setup startread and amount read, depending on the field values
	// of the address and the data_length
	if (connection->parsed_command.read.data_length)
	{
		// Read from a specified address and datalength
		startread=connection->parsed_command.read.address;
		amountread=connection->parsed_command.read.data_length;
		amounttoread=amountread;
	}
	else
	{
		// Read as much from the descriptor as is possible, and attempt to return it.
		startread=0;
		amountread=desc_size;
		amounttoread=amountread;
	}

	edsReadFromMap(connection, startread, &amountread);


	/*
		HANDLE THE READ_STATUS_RESULT field of the outgoing packet.
	*/
	connection->avcheader.ctype=AVC_RESPONSE_ACCEPTED;	
	
	moretoread = desc_size-connection->parsed_command.read.address-amountread;

	// When there has been an overread.
	if (amounttoread+connection->parsed_command.read.address > desc_size)
	{
// bk: host fails on this
//		connection->parsed_command.read.read_result_status=AVC_DESC_STATUS_DATA_LENGTH_TOO_LARGE;
		connection->parsed_command.read.read_result_status=AVC_DESC_STATUS_COMPLETE_READ;
		connection->parsed_command.read.data_length=(uint16)amountread;
	}
	else
	// When there is more left to read.
	if (moretoread > 0)
	{
		connection->parsed_command.read.read_result_status=AVC_DESC_STATUS_MORE_TO_READ;
		connection->parsed_command.read.data_length=(uint16)amountread;			
	} 
	else
	// When the read was absolutely complete. (amounttoread == amountread)
	{
		connection->parsed_command.read.read_result_status=AVC_DESC_STATUS_COMPLETE_READ;	
	}
	
		
	return hResult;
}

#ifdef _WINMP
#pragma warning( disable : 4055 )
#endif //_WINMP

HRESULT	edsHandleReadCallback(EDS_DESCRIPTOR_REF ref, EDS_BUFFER_TYPE buffertype)
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
				
					cbresult=(*cb)(EDS_CALLBACKOPERATION_READ,ref,buffertype);
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

HRESULT	edsDoReadCallbackChildren(EDS_DESCRIPTOR_REF descriptor)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		ddata=NULL;
	
	hResult=drsLockDescriptor(descriptor); 
	
	if (NO_ERROR !=hResult)
	{
		return hResult;		
	}
	hResult=drsGetDescriptor(descriptor,&ddata);
	if (hResult)
	{
		goto cleanup;	
	}
	
	switch (ddata->type)
	{
		case EDS_DESCRIPTORTYPE_ENTRY:
		{
			edsHandleReadCallback(descriptor, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
		}	
		break;

		case EDS_DESCRIPTORTYPE_OPAQUE:
		{
			edsHandleReadCallback(descriptor, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
		}	
		break;
		
		case EDS_DESCRIPTORTYPE_LIST:
		{
			edsHandleReadCallback(descriptor, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);				
		}
		break;
		
		case EDS_DESCRIPTORTYPE_INFOBLOCK:
		{
			edsHandleReadCallback(descriptor, EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
		}
		break;
		
		case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
		{
			edsHandleReadCallback(descriptor, EDS_BUFFERTYPE_SUBUNITINFO);
			edsHandleReadCallback(descriptor, EDS_BUFFERTYPE_MANUFACTURERINFO);
		}
		break;
	}
		/*
			Search for all the descriptors in the DRS which use this as a parent, then 
			include the size of each child.
		*/
		
		
	{
		EDS_DESCRIPTOR_REF	cref=0;
		EDS_DESCRIPTOR*		cdata=NULL;
		uint32				cindex=1;
		
		while (NO_ERROR==hResult && NO_ERROR==drsGetNthDescriptor(cindex++,&cref))
		{
			drsLockDescriptor(cref);
			drsGetDescriptor(cref,&cdata);
				
			if (cdata->parent_reference==descriptor)
			{
				if (cdata->type!=EDS_DESCRIPTORTYPE_LIST)
				{
// bk					edsDoReadCallbackChildren(descriptor);
					edsDoReadCallbackChildren(cref);
				}
			}	
			drsUnlockDescriptor(cref);
		}//while	
	}// if
	
cleanup:
	drsUnlockDescriptor(descriptor);
	
	return hResult;
}
#endif //_EDS

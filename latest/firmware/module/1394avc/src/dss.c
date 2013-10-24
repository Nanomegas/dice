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
#ifdef _EDS
static	char	moduleName[] = __FILE__ ;

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "listmanager.h"
#include "edstypes.h"
#include "rangemaps.h"
#include "avcDefs.h"
#include "datastream.h"
#include "drs.h"
#include "dss.h"

static	RANGEMAP mapInfoblockCommands[]=\
{
	{AVC_CMD_OPEN_INFOBLOCK, RMEVAL_ONLY,0}, 
	{AVC_CMD_READ_INFOBLOCK, RMEVAL_ONLY,0}, 
	{AVC_CMD_WRITE_INFOBLOCK,RMEVAL_ONLY,0},
	{0,0,0}
};

/*
	INITIALIZATIONS AND ENTRYPOINTS
*/

HRESULT	dssInitialize(void)
{
	HRESULT	hResult=NO_ERROR;
					
	return hResult;
}

/*
	dssCreateFromStream
	
	Creates an address from an incoming packet.
*/
HRESULT	dssCreateFromPacket(DSSAddress*	dsa, PB* packet)
{
	DataStream*	ds=NULL;
	HRESULT	hResult=NO_ERROR;
	
	hResult=pbGetApplicationDatastream(packet,&ds);	

	if (NO_ERROR==hResult)
	{
		hResult=dssCreateFromStream(dsa,ds);	
	}
	
	return hResult;
}

HRESULT	dssCreateFromStream	(DSSAddress* dsa, DataStream* ds)
{
		
	HRESULT				hResult=NO_ERROR;
	AVC_HEADER			header;
	EDS_DESCRIPTOR_REF	si_ref=0;	
	EDS_DESCRIPTOR*		si_data=0;
				
	memset(dsa,0,sizeof(DSSAddress));
	memset(&header,0,sizeof(header));
	
	if (NO_ERROR==hResult)
	{
		hResult=dsReset(ds);
	}
	if (NO_ERROR==hResult)
	{
		hResult=avcDecodeHeader(ds,&header);	
	}
	if (NO_ERROR==hResult)
	{
		hResult=drsFindSubunitIdentifier(header.addrSubunit, &si_ref);	
	}
	if (NO_ERROR==hResult)
	{
		hResult=drsLockDescriptor(si_ref);	
	}
	if (NO_ERROR==hResult)
	{
		// Lookup the descriptor subunit identifier.
		// This gives us the size of fields that we require in order to
		// actually interpret this info.
		drsGetDescriptor(si_ref,&si_data);
		
		if (si_data->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
		{
			uint32	index=1;
			
			dsa->size_of_list_id=si_data->u.subunit_identifier.size_of_list_id;
			dsa->size_of_object_id=si_data->u.subunit_identifier.size_of_object_id;
			dsa->size_of_entry_position=si_data->u.subunit_identifier.size_of_entry_position;
			
			/*
				We take the first byte of the command packet here if the 
				command type is an infoblock opcode. This gives us the
				count of items in the address.
			*/
			if (rmInMap(header.opcode,mapInfoblockCommands))
			{
				hResult=dsReadBits(ds,8,&index);
				
				dsa->is_infoblock=1;				
			}
			dsa->entry_count=(uint16)index;
			
			/* 
			   We do light parsing here, to push the pointer of the
			   data stream to the end of the address header.
			*/
			while (index-- && NO_ERROR==hResult)
			{
				uint8	specifiertype=AVC_DS_UNUSED_DESCRIPTOR_SPECIFIER; 
				// Read the specifier type 
				dsRead8Bits(ds,&specifiertype);
				
				switch (specifiertype)
				{
					case AVC_DS_SUBUNIT_IDENTIFIER:
					{
						/* 
							Do nothing, this is just a placeholder so that no one
							thinks I forgot about this case.
						*/
					}
					break;


					case AVC_DS_SUBUNIT_STATUS_DESCRIPTOR:
					{
						// this specifier_type only used by the Music Subunit
						if (AVC_SU_TYPE_MUSIC != header.addrSubunit.subunit_type)
						{
							hResult = E_DSS_BAD_DS;
						}
					}
					break;

					case AVC_DS_LIST_ID:
					{
						hResult=dsFastForward(ds,dsa->size_of_list_id);
					}	
					break;
					
					case AVC_DS_LIST_TYPE:
					{
						hResult=dsFastForward(ds,1); // List Types are always one byte.
					}
					break;
					
					case AVC_DS_ENTRY_POSITION:
					{
						hResult=dsFastForward(ds,dsa->size_of_list_id+dsa->size_of_entry_position);
					}
					break;
					
					case AVC_DS_ENTRY_OBJECT_ID:
					{
						hResult=dsFastForward(ds,dsa->size_of_list_id+dsa->size_of_object_id+1);
					}
					break;
					
					case AVC_DS_ENTRY_TYPE:
					{
						hResult=dsFastForward(ds,1);
					}
					break;

					case AVC_DS_OBJECT_ID:
					{
						hResult=dsFastForward(ds,dsa->size_of_object_id);
					}
					break;
					
					case AVC_DS_INFOBLOCK_INSTANCE:
					{
						hResult=dsFastForward(ds,3);
					}
					break;					
					
					case AVC_DS_INFOBLOCK_POSITION:
					{
						hResult=dsFastForward(ds,1);
					}
					break;
					
					default:
					{
						hResult=E_DSS_BAD_DS;
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
				} // switch
			} // while
		}
		else
		{
			hResult=E_DSS_BAD_DSSADDRESSREFERENCE;	
			sysLogError(hResult, __LINE__, moduleName);
		}
	}
	drsUnlockDescriptor(si_ref);	
	
	if (NO_ERROR==hResult)
	{
		hResult=dsSetMarker(ds,DSS_MARKER_ENDOFADDRESS);	
	}

	// Copy the address data from the incoming packet stream, into our data buffer.
	if (NO_ERROR==hResult)
	{
		uint32	position;
		
		position=dsGetPosition(ds);
		dsGotoMarker(ds,DSMARKER_OPERAND_0);
		position-=dsGetPosition(ds);
		
		hResult=dsReadChunk(ds,position,dsa->data);	
	}
		
	return hResult;
}


HRESULT	dssGetNthElement(DSSAddress* dsa, uint32 element, 
						 DSS_DESCRIPTOR_SPECIFIER* specifier)
{
	HRESULT			hResult=NO_ERROR;
	DataStream		ds;

	hResult=dsOpenStream(&ds,dsa->data,sizeof(dsa->data),dsMODE_READ | dsMODE_PLATFORMENDIAN);		

	if (NO_ERROR==hResult)
	{	
		uint32	specifiers=1;
		uint32	index=0;
		
		/*
			We take the first byte of the command packet here if the 
			command type is an infoblock opcode. This gives us the
			count of items in the address.
		*/
		if (dsa->is_infoblock)
		{
			hResult=dsReadBits(&ds,8,&specifiers);
		}
		
		if (element >= specifiers)
		{
			return E_DSS_ELEMENT_OUT_OF_RANGE;	
		}
		
		/* We do light parsing here, to push the pointer of the
		   data stream to the end of the address header.
		*/
		while (index <= element && NO_ERROR==hResult)
		{
			uint8	specifiertype=AVC_DS_UNUSED_DESCRIPTOR_SPECIFIER; 
			// Read the specifier type 
			memset(specifier,0,sizeof(DSS_DESCRIPTOR_SPECIFIER));
			dsRead8Bits(&ds,&specifiertype);

			specifier->type = specifiertype;
						
			switch (specifiertype)
			{
				case AVC_DS_SUBUNIT_IDENTIFIER:
				{
				}
				break;
				
				case AVC_DS_SUBUNIT_STATUS_DESCRIPTOR:
				{
				}
				break;

				case AVC_DS_LIST_ID:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_list_id,&specifier->list_id);
				}	
				break;
				
				case AVC_DS_LIST_TYPE:
				{
					hResult=dsRead8Bits(&ds,&specifier->list_type);
				}
				break;
				
				case AVC_DS_ENTRY_POSITION:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_list_id,&specifier->list_id);
					hResult=dsReadBits(&ds,8*dsa->size_of_entry_position,&specifier->entry_position);
				}
				break;
				
				case AVC_DS_ENTRY_OBJECT_ID:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_list_id,&specifier->list_id);
					hResult=dsRead8Bits(&ds,&specifier->list_type);
					hResult=dsReadBits(&ds,8*dsa->size_of_object_id, &specifier->object_id);
				}
				break;
				
				case AVC_DS_ENTRY_TYPE:
				{
					hResult=dsRead8Bits(&ds,&specifier->entry_type);
				}
				break;

				case AVC_DS_OBJECT_ID:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_object_id, &specifier->object_id);
				}
				break;
				
				case AVC_DS_INFOBLOCK_INSTANCE:
				{
					hResult=dsRead16Bits(&ds,&specifier->info_block_type);
					hResult=dsRead8Bits(&ds,&specifier->info_block_instance_count);
				}
				break;					
				
				case AVC_DS_INFOBLOCK_POSITION:
				{
					hResult=dsRead8Bits(&ds,&specifier->info_block_position);
				}
				break;
				
				default:
				{
					hResult=E_DSS_BAD_DS;
					sysLogError(hResult, __LINE__, moduleName);
				}
				break;
			} // switch

			index++;

		} // while
	}
			
	return hResult;
}

HRESULT	dssCountElements(DSSAddress* dsa, uint32* elementcount)
{
	if (!dsa)
	{
		return E_DSS_BAD_DSSADDRESSREFERENCE;	
	}
	
	*elementcount=dsa->entry_count;
	
	return NO_ERROR;
}

HRESULT	dssGetDataSize(DSSAddress* 	dsa, uint32* size)
{
	HRESULT			hResult=NO_ERROR;
	uint32			mysize=0;	// The size is always 1 or greater
	DataStream		ds;

	hResult=dsOpenStream(&ds,dsa->data,sizeof(dsa->data),dsMODE_READ | dsMODE_PLATFORMENDIAN);		
	
	if (!dsa)
	{
		hResult=E_DSS_BAD_DSSADDRESSREFERENCE;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (NO_ERROR==hResult)
	{	
		uint32	specifiers=1;
		uint32	element=0;

		/*
			We take the first byte of the command packet here if the 
			command type is an infoblock opcode. This gives us the
			count of items in the address.
		*/
		if (dsa->is_infoblock)
		{
			hResult=dsReadBits(&ds,8,&specifiers);
			mysize++;
		}
		
		if (element >= specifiers)
		{
			*size=mysize;
			return NO_ERROR;
		}
		
		/* We do light parsing here, to push the pointer of the
		   data stream to the end of the address header.
		*/
		while (element < specifiers && NO_ERROR==hResult)
		{
			uint8 specifiertype = AVC_DS_UNUSED_DESCRIPTOR_SPECIFIER; 
			DSS_DESCRIPTOR_SPECIFIER specifier;

			// Read the specifier type 
			memset(&specifier,0,sizeof(DSS_DESCRIPTOR_SPECIFIER));
			dsRead8Bits(&ds,&specifiertype);
			mysize++;			
			
			switch (specifiertype)
			{
				case AVC_DS_SUBUNIT_IDENTIFIER:
				{
				}
				break;

				case AVC_DS_SUBUNIT_STATUS_DESCRIPTOR:
				{
				}
				break;

				case AVC_DS_LIST_ID:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_list_id,&specifier.list_id);
					mysize+=dsa->size_of_list_id;
				}	
				break;
				
				case AVC_DS_LIST_TYPE:
				{
					hResult=dsRead8Bits(&ds,&specifier.list_type);
					mysize++;
				}
				break;
				
				case AVC_DS_ENTRY_POSITION:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_list_id,&specifier.list_id);
					mysize+=dsa->size_of_list_id;
					hResult=dsReadBits(&ds,8*dsa->size_of_entry_position,&specifier.entry_position);
					mysize+=dsa->size_of_entry_position;
				}
				break;
				
				case AVC_DS_ENTRY_OBJECT_ID:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_list_id,&specifier.list_id);
					mysize+=dsa->size_of_list_id;
					hResult=dsRead8Bits(&ds,&specifier.list_type);
					mysize++;
					hResult=dsReadBits(&ds,8*dsa->size_of_object_id,&specifier.object_id);
					mysize+=dsa->size_of_object_id;
				}
				break;
				
				case AVC_DS_ENTRY_TYPE:
				{
					hResult=dsRead8Bits(&ds,&specifier.entry_type);
					mysize++;
				}
				break;
				
				case AVC_DS_OBJECT_ID:
				{
					hResult=dsReadBits(&ds,8*dsa->size_of_object_id, &specifier.object_id);
					mysize+=dsa->size_of_object_id;
				}
				break;
				
				case AVC_DS_INFOBLOCK_INSTANCE:
				{
					hResult=dsRead16Bits(&ds,&specifier.info_block_type);
					mysize+=2;
					hResult=dsRead8Bits(&ds,&specifier.info_block_instance_count);
					mysize++;
				}
				break;					
				
				case AVC_DS_INFOBLOCK_POSITION:
				{
					hResult=dsRead8Bits(&ds,&specifier.info_block_instance_count);
					mysize++;
				}
				break;
				
				default:
				{
					hResult=E_DSS_BAD_DS;
					sysLogError(hResult, __LINE__, moduleName);
				}
				break;
			} // switch
		
			element++;
		} // while
	}


	*size=mysize;
	return hResult;
	
}

HRESULT	dssCreateAsEmpty(DSSAddress* dsa, 
						 AVC_ADDRESS_SUBUNIT subunit)
{
	HRESULT	hResult=NO_ERROR;
	
	UNUSED_ARG(dsa);
	UNUSED_ARG(subunit);
		
	hResult=E_NOT_IMPLEMENTED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;	
}

HRESULT	dssAddElement(DSSAddress* dsa, DSS_DESCRIPTOR_SPECIFIER* specifier)
{
	HRESULT	hResult=NO_ERROR;

	UNUSED_ARG(dsa);
	UNUSED_ARG(specifier);
	
	hResult=E_NOT_IMPLEMENTED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;	
}

HRESULT	dssSetAsInfoblockReferencePath(DSSAddress*	dsa)
{
	HRESULT	hResult=NO_ERROR;

	UNUSED_ARG(dsa);
	
	hResult=E_NOT_IMPLEMENTED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;	
}

HRESULT	dssSetNodeHandle(DSSAddress* dsa, NODEHANDLE nh)
{
	HRESULT	hResult=NO_ERROR;

	UNUSED_ARG(dsa);
	UNUSED_ARG(nh);
	
	hResult=E_NOT_IMPLEMENTED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;	
}

HRESULT	dssSetSubunitIdentifierInfo(DSSAddress*	dsa, uint16 sizeoflistID, 
									uint16 sizeofobjectID, uint16 sizeofentryposition)
{
	HRESULT	hResult=NO_ERROR;

	UNUSED_ARG(dsa);
	UNUSED_ARG(sizeoflistID);
	UNUSED_ARG(sizeofobjectID);
	UNUSED_ARG(sizeofentryposition);

	hResult=E_NOT_IMPLEMENTED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;	
}
#endif //_EDS

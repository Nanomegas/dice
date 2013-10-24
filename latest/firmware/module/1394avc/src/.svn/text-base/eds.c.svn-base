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
#ifdef _EDS

static	char 						moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "drs.h"
#include "dss.h"
#include "eds.h"
#include "edstypes.h"
#include "edsServer.h"
#include "edsCli.h"
#include "format.h"

/*
	Function: edsInitialize
	
	This function takes care of initializing all the code that it uses.
*/
HRESULT	edsInitialize(void)
{
	HRESULT	hResult=NO_ERROR;
	
	hResult = drsInitialize();

	if (hResult == NO_ERROR)
	{
		hResult = dssInitialize();
	}
	
	if (hResult == NO_ERROR)
	{
		hResult = edsServerInitialize();
	}
	
#ifdef _CLI_TOOL_EDS
	edsCliInstallTools();
#endif //_CLI_TOOL_EDS
	
	return hResult;
}

/*
	Function edsResgisterSubunitClientSession
	
	This function registers stuff.
*/
HRESULT	edsRegisterSubunitClientSession(EDS_CLIENT_SESSION*	session)
{
	return edsServerRegisterSession(session);
}

/*
	Function edsRegisterCustomResolver
	
	Allows for subunit specific descriptor specifiers to be passed to a custom resolver.
	If the custom resolver for the subunit cannot deal with the address then it will return
	an error.
*/

HRESULT	edsRegisterCustomResolver(EDS_CLIENT_SESSION* session, 
								  EDS_CUSTOM_RESOLVER resolver)
{
	if (session)
	{
		session->custom_resolver=resolver;
	}
	
	return NO_ERROR;
}

/*
	Function edsCreateOpaqueDescriptor
	
	This creates what essentially is considered a custom descriptor.
	Custom Descriptors are usually subunit dependant.  These are not accessible using any
	of the normal addressing modes, and thus a CUSTOM_RESOLVER must be implemented in order
	to find descriptors of this type.
	
	Only infoblocks may be associated with this descriptor type, and they will be attached at the 
	end of the descriptor's data. 
*/

HRESULT	edsCreateOpaqueDescriptor(EDS_CLIENT_SESSION* session, 
								  uint32 descriptor_type, 
								  uint32 buffersize, 
								  void* buffer,
								  EDS_DESCRIPTOR_REF* reference)
{
	HRESULT		 		hResult=NO_ERROR;
	EDS_DESCRIPTOR	 	newdesc;
	EDS_DESCRIPTOR_REF	newref=0;
	
	memset(&newdesc,0,sizeof(newdesc));
	newdesc.type=EDS_DESCRIPTORTYPE_OPAQUE;
	newdesc.subunit=session->subunit;
	
	formatSwapStrBytes(buffer, buffersize);	// convert the struct to network byte order

	newdesc.u.opaque_descriptor.si_size=(uint16)buffersize;
	newdesc.u.opaque_descriptor.specific_info=(char*)buffer;
	newdesc.u.opaque_descriptor.type=descriptor_type;
	
	hResult=drsRegisterDescriptor(&newdesc,&newref);
	if (NO_ERROR==hResult)
	{
		*reference=newref;
	}

	return hResult;
}

/* 
	Function edsCreateSubunitIdentifier
	
	This function creates a subunitidentifer descriptor, based on the data which you specified
	in the session, and the initdata fields. 
	It places a reference to the new subunitidentifier in the session->subunit_identifier field.
*/
HRESULT	edsCreateSubunitIdentifier(EDS_CLIENT_SESSION* session, 
								   EDS_SUBUNITIDENTIFIER*  initdata)
{
	HRESULT		 		hResult=NO_ERROR;
	EDS_DESCRIPTOR	 	newsi;
	EDS_DESCRIPTOR_REF	newref=0;
	
	memset(&newsi,0,sizeof(newsi));
	memcpy(&newsi.u.subunit_identifier,initdata,sizeof(newsi.u.subunit_identifier));
	newsi.type=EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER;
	newsi.subunit=session->subunit;
	
	hResult=drsRegisterDescriptor(&newsi,&newref);
	if (NO_ERROR==hResult)
	{
		session->subunit_desc_ref=newref;	
	}

	return hResult;	
}

HRESULT	edsCreateListDescriptor(EDS_CLIENT_SESSION*	session, 
								uint32 list_type, 
								uint32 list_id, 
								uint8 attribute,
								uint32 buffersize, 
								void* buffer, 
								EDS_DESCRIPTOR_REF*	 reference)
{
	HRESULT		 		hResult=NO_ERROR;
	EDS_DESCRIPTOR	 	newlist;
	EDS_DESCRIPTOR_REF	newref=0;
	
	memset(&newlist,0,sizeof(newlist));
	newlist.type=EDS_DESCRIPTORTYPE_LIST;
	newlist.subunit=session->subunit;
	newlist.u.list_descriptor.type=(uint8)list_type;
	newlist.u.list_descriptor.attributes=(uint8)(attribute | AVC_DESC_ATTRIB_UP_TO_DATE);
	newlist.u.list_descriptor.list_id=list_id;
	newlist.u.list_descriptor.si_size=(uint16)buffersize;
	newlist.u.list_descriptor.specific_info=buffer;
	
	hResult=drsRegisterDescriptor(&newlist,&newref);
	if (NO_ERROR==hResult)
	{
		*reference=newref;
	}

	return hResult;		
}

HRESULT	edsCreateEntryDescriptor(EDS_CLIENT_SESSION* session, 
								 uint32 entry_type,
								 uint32	object_id, 
								 uint32 buffersize, 
								 void* buffer,
								 EDS_DESCRIPTOR_REF* reference)
{
	HRESULT		 		hResult=NO_ERROR;
	EDS_DESCRIPTOR	 	newentry;
	EDS_DESCRIPTOR_REF	newref=0;
	
	memset(&newentry,0,sizeof(newentry));
	newentry.type=EDS_DESCRIPTORTYPE_ENTRY;
	newentry.subunit=session->subunit;
	newentry.u.entry_descriptor.type=(uint8)entry_type;
	newentry.u.entry_descriptor.object_id=object_id;
	newentry.u.entry_descriptor.si_size=(uint16)buffersize;
	newentry.u.entry_descriptor.specific_info=buffer;
	
	hResult=drsRegisterDescriptor(&newentry,&newref);
	if (NO_ERROR==hResult)
	{
		*reference=newref;
	}

	return hResult;				
}

HRESULT	edsCreateInfoBlock(EDS_CLIENT_SESSION*	session, 
						   uint32 infoblock_type, 
						   uint32 buffersize, 
						   void* buffer,
						   EDS_DESCRIPTOR_REF*	reference)
{
	HRESULT		 		hResult=NO_ERROR;
	EDS_DESCRIPTOR	 	newinfoblock;
	EDS_DESCRIPTOR_REF	newref=0;
	
	memset(&newinfoblock,0,sizeof(newinfoblock));
	newinfoblock.type=EDS_DESCRIPTORTYPE_INFOBLOCK;
	newinfoblock.subunit=session->subunit;
	newinfoblock.u.info_block.type=(uint16)infoblock_type;
	newinfoblock.u.info_block.primary_fields_length=(uint16)buffersize;
	newinfoblock.u.info_block.primary_fields=buffer;
	
	hResult=drsRegisterDescriptor(&newinfoblock,&newref);
	if (NO_ERROR==hResult)
	{
		*reference=newref;
	}

	return hResult;					
}

HRESULT	edsSetDataBuffer(EDS_DESCRIPTOR_REF	descriptor, 
						 EDS_BUFFER_TYPE buffertype,
						 uint32	buffersize, 
						 void* buffer)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		descdata=NULL;
	
	hResult=drsLockDescriptor(descriptor);
	if (NO_ERROR==hResult)	
	{
		hResult=drsGetDescriptor(descriptor,&descdata);
	}
	
	if (NO_ERROR==hResult)
	{
		/*
			Do something with our data.
		*/
		switch (descdata->type)
		{
			case (EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER):
			{
				switch (buffertype)
				{
					case (EDS_BUFFERTYPE_SUBUNITINFO):
					{
						descdata->u.subunit_identifier.si_length=(uint16)buffersize;
						descdata->u.subunit_identifier.subunit_information=buffer;
					}
					break;
					
					case (EDS_BUFFERTYPE_MANUFACTURERINFO):
					{
						descdata->u.subunit_identifier.manufacturer_information=buffer;
						descdata->u.subunit_identifier.mi_length=(uint16)buffersize;
					}
					break;
					
					default:
					{
						hResult=E_EDS_UNKNOWN_BUFFERTYPE;	
						sysLogError(hResult, __LINE__, moduleName);
					}
				}
			}
			break;
			
			case EDS_DESCRIPTORTYPE_ENTRY:
			case EDS_DESCRIPTORTYPE_INFOBLOCK:
			case EDS_DESCRIPTORTYPE_LIST:
			case EDS_DESCRIPTORTYPE_OPAQUE:
			{
				switch (buffertype)
				{
					case EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
					{
						if (descdata->type==EDS_DESCRIPTORTYPE_OPAQUE)
						{
							descdata->u.opaque_descriptor.specific_info=buffer;
							descdata->u.opaque_descriptor.si_size=(uint16)buffersize;
						}
						if (descdata->type==EDS_DESCRIPTORTYPE_ENTRY)
						{
							descdata->u.entry_descriptor.specific_info=buffer;
							descdata->u.entry_descriptor.si_size=(uint16)buffersize;	
						}
						if (descdata->type==EDS_DESCRIPTORTYPE_LIST)
						{
							descdata->u.list_descriptor.specific_info=buffer;
							descdata->u.list_descriptor.si_size=(uint16)buffersize;	
						}
						if (descdata->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
						{
							descdata->u.info_block.primary_fields=buffer;
							descdata->u.info_block.primary_fields_length=(uint16)buffersize;	
						}
					}
					break;
					
					default:
					{
						hResult=E_EDS_UNKNOWN_BUFFERTYPE;	
						sysLogError(hResult, __LINE__, moduleName);
					}					
				}
			}
			break;
			default:
			{
				hResult=E_EDS_UNKNOWN_BUFFERTYPE;	
				sysLogError(hResult, __LINE__, moduleName);
			}
		}	
	}
	drsUnlockDescriptor(descriptor);
	
	return hResult;
}
								 

HRESULT	edsAssociateChildWithParent(EDS_DESCRIPTOR_REF parent, 
									EDS_DESCRIPTOR_REF child, 
									EDS_BUFFER_TYPE	placement)
{
	HRESULT				hResult=NO_ERROR;
	
	EDS_DESCRIPTOR*		pdata=NULL;
	EDS_DESCRIPTOR*		cdata=NULL;
	
	/*
		First get locks on both the parent and child.
	*/
	hResult=drsLockDescriptor(parent);
	if (NO_ERROR==hResult)
	{
		hResult=drsLockDescriptor(child);	
	}
	
	if (NO_ERROR==hResult)
	{
		drsGetDescriptor(parent,&pdata);
		drsGetDescriptor(child,&cdata);	
	}
	
	if (NO_ERROR==hResult)
	{
		switch (pdata->type)
		{
			case EDS_DESCRIPTORTYPE_OPAQUE:
			{
				switch (cdata->type)
				{
					case EDS_DESCRIPTORTYPE_INFOBLOCK:
					{
						switch (placement)
						{ 
							case EDS_BUFFERTYPE_EXTENDEDINFO:
							{
								cdata->parent_reference=parent;
								pdata->u.opaque_descriptor.extended_infoblock_count++;
							}
							break;

							default:
							{
								hResult=E_EDS_UNKNOWN_BUFFERTYPE;	
								sysLogError(hResult, __LINE__, moduleName);
							}
							break;
						}
					}
					break;

					default:
					{
						hResult=E_EDS_INCOMPATIBLE_TYPES;	
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
					}
			}
			break;
			
			case EDS_DESCRIPTORTYPE_LIST:
			{
				switch (cdata->type)
				{
					default:
					{
						hResult=E_EDS_INCOMPATIBLE_TYPES;	
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
						
					case EDS_DESCRIPTORTYPE_ENTRY:
					{
						cdata->parent_reference=parent;
						cdata->u.entry_descriptor.position=++(pdata->u.list_descriptor.number_of_entries);
					}
					break;
					
					case EDS_DESCRIPTORTYPE_INFOBLOCK:
					{
						cdata->parent_reference=parent;
						cdata->u.info_block.buffertype=placement;
						switch (placement)
						{
							case EDS_BUFFERTYPE_EXTENDEDINFO:
							{
								cdata->parent_reference=parent;
								pdata->u.list_descriptor.extended_infoblock_count++;
							}
							break;
							
							case EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
							{
								cdata->parent_reference=parent;
								pdata->u.list_descriptor.si_infoblock_count++;
							}
							break;
							
							default:
							{
								hResult=E_EDS_UNKNOWN_BUFFERTYPE;	
								sysLogError(hResult, __LINE__, moduleName);
							}
						}
					}
					break;				
				}		
			}
			break;
			
			case EDS_DESCRIPTORTYPE_ENTRY:
			{
				switch (cdata->type)
				{		
					default:
					{
						hResult=E_EDS_INCOMPATIBLE_TYPES;	
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
					
					case EDS_DESCRIPTORTYPE_LIST:
					{
						pdata->u.entry_descriptor.child_list_reference=child;
					}
					break;
		
					case EDS_DESCRIPTORTYPE_INFOBLOCK:
					{
						cdata->parent_reference=parent;
						cdata->u.info_block.buffertype=placement;
						switch (placement)
						{
							case EDS_BUFFERTYPE_EXTENDEDINFO:
							{
								pdata->u.entry_descriptor.extended_infoblock_count++;
							}
							break;
							
							case EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
							{
								pdata->u.entry_descriptor.si_infoblock_count++;
							}
							break;
							
							default:
							{
								hResult=E_EDS_UNKNOWN_BUFFERTYPE;	
								sysLogError(hResult, __LINE__, moduleName);
							}
						}
					}
					break;
				}				
			}
			break;
			
			case EDS_DESCRIPTORTYPE_INFOBLOCK:
			{
				switch (cdata->type)
				{
					case EDS_DESCRIPTORTYPE_INFOBLOCK:
					{
						cdata->parent_reference=parent;
						cdata->u.info_block.extended_infoblock_count++;		
					}
					break;
					
					default:
					{
						hResult=E_EDS_UNKNOWN_BUFFERTYPE;							
						sysLogError(hResult, __LINE__, moduleName);
					}	
				}
			}
			break;
			
			case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
			{
				switch (cdata->type)
				{
					case EDS_DESCRIPTORTYPE_INFOBLOCK:
					{
						cdata->parent_reference=parent;
						cdata->u.info_block.buffertype=placement;
						switch (placement)
						{
							case EDS_BUFFERTYPE_EXTENDEDINFO:
							{
								pdata->u.subunit_identifier.extended_infoblock_count++;
							}
							break;
							
							case EDS_BUFFERTYPE_SUBUNITINFO:
							{
								pdata->u.subunit_identifier.si_infoblock_count++;
							}
							break;
							
							case EDS_BUFFERTYPE_MANUFACTURERINFO:
							{
								pdata->u.subunit_identifier.mi_infoblock_count++;
							}
							break;
							
							default:
							{
								hResult=E_EDS_UNKNOWN_BUFFERTYPE;	
								sysLogError(hResult, __LINE__, moduleName);
							}
						}
					}
					break;
					
					case EDS_DESCRIPTORTYPE_LIST:
					{
						cdata->parent_reference=parent;
						pdata->u.subunit_identifier.number_of_root_lists++;
					}
					break;

					case EDS_DESCRIPTORTYPE_OPAQUE:
					{
						cdata->parent_reference=parent;
						pdata->u.subunit_identifier.extended_infoblock_count++;
					}
					break;

					default:
					{
						hResult=E_EDS_UNKNOWN_DESCRIPTOR;	
						sysLogError(hResult, __LINE__, moduleName);
					}	
					break;

				}
			}
			break;		
		} //switch 
	} // if
	// Release the parent and the child so that 
	// they can be used by another thread.
	drsUnlockDescriptor(child);
	drsUnlockDescriptor(parent);
	
	return hResult;	
}

HRESULT	edsRemoveParentalAssociation(EDS_DESCRIPTOR_REF child)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		cdata=NULL;

	hResult=drsLockDescriptor(child);	
	if (NO_ERROR==hResult)
	{
		EDS_DESCRIPTOR_REF	parent;
		EDS_DESCRIPTOR*		pdata=NULL;
		// Remove the parental reference.
		// Now if this is an infoblock or an entry, we need to search for all the
		// children of the parent and lower the "position" of all the children.
	
		drsGetDescriptor(child, &cdata);
		parent = cdata->parent_reference;
		
		// Each type of descriptor, has different things it must do to remove the
		// the reference to the parent descriptor.
		switch (cdata->type)
		{
			
			case EDS_DESCRIPTORTYPE_LIST:
			{
				// We must actually go back to the PARENT and remove the
				// the reference to this list.   This is only important 
				// if the parent is an entry in a list.
				
				hResult=drsLockDescriptor(parent);
				if (NO_ERROR==hResult)
				{
					drsGetDescriptor(parent,&pdata);
					if (pdata->type==EDS_DESCRIPTORTYPE_ENTRY)
					{
						pdata->u.entry_descriptor.child_list_reference=0;
						cdata->parent_reference=0;	
					}
					if (pdata->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
					{
						pdata->u.subunit_identifier.number_of_root_lists--;
						cdata->parent_reference=0;	
					}
				}
				drsUnlockDescriptor(parent);
			}
			break;	
			
			case EDS_DESCRIPTORTYPE_ENTRY:
			{
				HRESULT				searchresult=NO_ERROR;
				uint32				position=cdata->u.entry_descriptor.position;
				uint32				index=1;
				EDS_DESCRIPTOR_REF      sibling=0;
				EDS_DESCRIPTOR*			sdata=NULL;
				// Here we must find all of the entry descriptors associated 
				// with the parent, and then change the position of those
				// which are greater than this element, to have a lesser position.
				
			
				while (NO_ERROR==searchresult && NO_ERROR==hResult)
				{
					searchresult=drsGetNthDescriptor(index++,&sibling);

					hResult=drsLockDescriptor(sibling);
					if (NO_ERROR==hResult)
					{
						drsGetDescriptor(sibling,&sdata);
						if (sdata->type==EDS_DESCRIPTORTYPE_ENTRY && cdata->parent_reference==sdata->parent_reference)
						{
							if (sdata->u.entry_descriptor.position > position)
							{
								sdata->u.entry_descriptor.position--;	
							}	
						}
						drsUnlockDescriptor(sibling);
					}
				}
				// Remove the reference to the parent.
				// decrement the entry counter of the parent list as well.
				if (NO_ERROR==hResult)
				{
					hResult=drsLockDescriptor(parent);
				}
				if (NO_ERROR==hResult)
				{
					drsGetDescriptor(parent,&pdata);
					if (pdata->type==EDS_DESCRIPTORTYPE_LIST)
					{
						pdata->u.list_descriptor.number_of_entries--;	
					}
					drsUnlockDescriptor(parent);
				}
				cdata->parent_reference=0;
				
			}
			break;
			
			case EDS_DESCRIPTORTYPE_INFOBLOCK:
			{
				HRESULT				searchresult=NO_ERROR;
				uint32				position=cdata->u.entry_descriptor.position;
				uint32				index=1;
				EDS_DESCRIPTOR_REF      sibling=0;
				EDS_DESCRIPTOR*			sdata=NULL;
				// Here we must find all of the entry descriptors associated 
				// with the parent, and then change the position of those
				// which are greater than this element, to have a lesser position.
				
				
				while (NO_ERROR==searchresult && NO_ERROR==hResult)
				{
					searchresult=drsGetNthDescriptor(index++,&sibling);

					hResult=drsLockDescriptor(sibling);
					if (NO_ERROR==hResult)
					{
						drsGetDescriptor(sibling,&sdata);
						if (sdata->type==EDS_DESCRIPTORTYPE_INFOBLOCK && sdata->parent_reference==cdata->parent_reference)
						{
							if (sdata->u.info_block.position > position)
							{
								sdata->u.info_block.position--;	
							}	
						}
						drsUnlockDescriptor(sibling);
					}
				}// while
				
				// Remove the reference to the parent.
				// decrement the counter of the parent descriptor as well.
				if (NO_ERROR==hResult)
				{
					hResult=drsLockDescriptor(parent);
				}
				if (NO_ERROR==hResult)
				{
					drsGetDescriptor(parent,&pdata);
					switch(cdata->u.info_block.buffertype)
					{
							case  EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
							{
								if (pdata->type==EDS_DESCRIPTORTYPE_LIST)
								{
									pdata->u.list_descriptor.si_infoblock_count--;
								}
								if (pdata->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
								{
									hResult=E_EDS_BAD_BUFFERTYPE;
									sysLogError(hResult, __LINE__, moduleName);
									break;	
								}
								if (pdata->type==EDS_DESCRIPTORTYPE_ENTRY)
								{
									pdata->u.entry_descriptor.si_infoblock_count--;	
								}
								if (pdata->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
								{
									pdata->u.subunit_identifier.si_infoblock_count--;	
								}
							}	
							break;
							
							case  EDS_BUFFERTYPE_EXTENDEDINFO:
							{
								if (pdata->type==EDS_DESCRIPTORTYPE_LIST)
								{
									pdata->u.list_descriptor.extended_infoblock_count--;
								}
								if (pdata->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
								{
									pdata->u.info_block.extended_infoblock_count--;	
								}
								if (pdata->type==EDS_DESCRIPTORTYPE_ENTRY)
								{
									pdata->u.entry_descriptor.extended_infoblock_count--;	
								}
								if (pdata->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
								{
									pdata->u.subunit_identifier.extended_infoblock_count--;	
								}
								if (pdata->type==EDS_DESCRIPTORTYPE_OPAQUE)
								{
									pdata->u.opaque_descriptor.extended_infoblock_count--;	
								}
							}
							break;
							
							case  EDS_BUFFERTYPE_SUBUNITINFO:
							{
								if (pdata->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
								{
									pdata->u.subunit_identifier.si_infoblock_count--;	
								}
							}
							break;

							case  EDS_BUFFERTYPE_MANUFACTURERINFO:
							{
								if (pdata->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
								{
									pdata->u.subunit_identifier.mi_infoblock_count--;	
								}
							}
							break;


						}
					}								
					drsUnlockDescriptor(parent);
				}
				cdata->parent_reference=0;
			}//switch
		}
	
	drsUnlockDescriptor(child);
	
	return hResult;
}



HRESULT	edsReleaseDescriptor(EDS_DESCRIPTOR_REF descriptor, 
							 BOOL release_children)
{
	HRESULT			hResult=NO_ERROR;
	uint32			index=1;
	HRESULT			searchresult=NO_ERROR;
	
	hResult=drsLockDescriptor(descriptor);
	if (NO_ERROR==hResult)
	{
		while (NO_ERROR==searchresult && NO_ERROR==hResult)
		{
			EDS_DESCRIPTOR_REF		item=0;
			EDS_DESCRIPTOR*			  idata=NULL;	
			
			searchresult=drsGetNthDescriptor(index++,&item);

			hResult=drsLockDescriptor(item);
			if (NO_ERROR==hResult)
			{
				drsGetDescriptor(item,&idata);
				if (descriptor==idata->parent_reference)
				{
					idata->parent_reference=0;
					
					if (release_children==TRUE)
					{
						hResult=edsReleaseDescriptor(item,TRUE);	
					}
				}
				drsUnlockDescriptor(item);
			}
		}
		hResult=drsUnregisterDescriptor(descriptor);
	}
	
	return hResult;
}

#ifdef _WINMP
#pragma warning( disable : 4054 )
#endif //_WINMP

HRESULT	edsSetVolatileCallback(EDS_DESCRIPTOR_REF descriptor, 
							   EDS_VOLATILE_CALLBACK callback)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		ddata;
	
	hResult=drsLockDescriptor(descriptor);
	if (NO_ERROR==hResult)
	{
		drsGetDescriptor(descriptor,&ddata);
		ddata->callback = (void *)callback;
		ddata->callback_mode = EDS_CALLBACKMODE_VOLATILE;
	}
	drsUnlockDescriptor(descriptor);
	
	return hResult;		
}
#endif //_EDS
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

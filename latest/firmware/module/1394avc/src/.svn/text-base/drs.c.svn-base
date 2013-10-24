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

	Descriptor Registry Services
	
	This file implements the ability to register descriptors, or really buffers
	in a global directory.

  Revisions:
		created 06/05/2003 KBJ/LM

****************************************************************************/
#ifdef _EDS
static	char	moduleName[] = __FILE__ ;

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "TCTypes.h"
#include "TCTasking.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "listmanager.h"
#include "keyvalue.h"
#include "format.h"
#include "edstypes.h"
#include "drs.h"

/* SCOPE RANGE MACROS */
#define			DRS_LISTTYPE_SCOPE_ALLSUBUNITS(x) (x >=0x00 && x<=0x7F)
#define			DRS_LISTTYPE_SCOPE_SUBUNITTYPE(x) (x >=0x80 && x<=0xFF)

#define 		DRS_ENTRYTYPE_SCOPE_ALLSUBUNITS(x) DRS_LISTTYPE_SCOPE_ALLSUBUNITS(x)
#define			DRS_ENTRYTYPE_SCOPE_SUBUNITTYPE(x) DRS_LISTTYPE_SCOPE_SUBUNITTYPE(x)

#define			DRS_INFOBLOCKTYPE_SCOPE_ALLSUBUNITS(x)	(x >=0x0000 && x<=0x7FFF)
#define			DRS_INFOBLOCKTYPE_SCOPE_SUBUNITTYPE(x)	(x >=0x8000 && x<=0xFFFF)

#define			DRS_LISTID_SCOPE_SUBUNITTYPE (x) ((x>=0x1000 && x <=0x3FFF) || (x>=0x10000))


typedef	struct
{
	uint16			inuse;
	uint16			usagecount;
	uint32			owner_taskID; // Thread which currently has the lock.
	EDS_DESCRIPTOR	descriptor;
} DRS_LISTENTRY;


static		DRS_LISTENTRY	descriptorList[_DRS_DESCRIPTOR_COUNT];
static		uint32			drsLockMutexSemID;

_TC_INLINE_STATIC BOOL isSubunitsEqual(AVC_ADDRESS_SUBUNIT subunit1, AVC_ADDRESS_SUBUNIT subunit2) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC BOOL isSubunitsEqual(AVC_ADDRESS_SUBUNIT subunit1, AVC_ADDRESS_SUBUNIT subunit2)
{
	return (subunit1.subunit_ID == subunit2.subunit_ID &&
			subunit1.subunit_type == subunit2.subunit_type);
}

/*
	drsInitialize
	
	This function needs to be called before the drs can be used.
*/

HRESULT	drsInitialize(void)
{
	HRESULT		hResult=NO_ERROR;
	
	// Initialize the list
	memset(descriptorList,0,sizeof(descriptorList));
	
	hResult = TCMutexOpen(&drsLockMutexSemID);
	return hResult;	
}

/*	
	drsFindSubunitIdentifier
	
	This function is a specialized search function, which finds the 
	specified subunit identifier.
	
*/

HRESULT	drsFindSubunitIdentifier(AVC_ADDRESS_SUBUNIT subunit, EDS_DESCRIPTOR_REF*  descriptor)
{
	HRESULT		hResult=NO_ERROR;
	uint32		index=0;
	BOOL		found=0;
	
	hResult = TCMutexLock(drsLockMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	while (index < ARRAYSIZE(descriptorList))
	{
		if (descriptorList[index].inuse)
		{
			if (descriptorList[index].descriptor.type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
			{
				if (isSubunitsEqual(descriptorList[index].descriptor.subunit, subunit))
				{
					*descriptor=index+1;
					found=1;
					break;	
				}
			}
		}
		index++;
	}
	
	if (!found)
	{
		hResult=E_DRS_NO_DESCRIPTOR;	
		sysLogError(hResult, __LINE__, moduleName);
	}
	
	TCMutexUnlock(drsLockMutexSemID);
	
	return hResult;
}



/*
	drsRegisterDescriptor
	
	This function registers the descriptor which is specified, with the DRS. 
*/
HRESULT	drsRegisterDescriptor(EDS_DESCRIPTOR*	descriptor, EDS_DESCRIPTOR_REF*	 reference)
{
	HRESULT		hResult=NO_ERROR;
	uint32		index=0;
	BOOL		found=0;
	
	while (index < ARRAYSIZE(descriptorList) && (!found))
	{
		if (descriptorList[index].inuse==FALSE)
		{
			hResult = TCMutexLock(drsLockMutexSemID);
			if (hResult != NO_ERROR) return hResult;

			if (descriptorList[index].inuse==FALSE)
			{
				memcpy(&descriptorList[index].descriptor,descriptor,sizeof(EDS_DESCRIPTOR));
				descriptorList[index].inuse=TRUE;
				found=1;	
				*reference=index+1;
			}
			
			TCMutexUnlock(drsLockMutexSemID);
		}
		index++;
	}
	
	if (!found)
	{
		hResult=E_DRS_NO_DESCRIPTOR_SPACE;	
		sysLogError(hResult, __LINE__, moduleName);
	}
		
	return hResult;
}

/*	
	drsUnregisterDescriptor
	
	This function unregisters the descriptor which is specified.
	This frees LOCKED descriptors as well, so DO NOT ALLOW THIS TO BE CALLED
	BEFORE the  
*/
HRESULT	drsUnregisterDescriptor(EDS_DESCRIPTOR_REF	reference)
{
	HRESULT		hResult=NO_ERROR;
	uint32		index=reference-1;
	BOOL			found=0;

	if (index >= ARRAYSIZE(descriptorList))
	{
		hResult=E_DRS_BAD_REFERENCE;		
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (descriptorList[index].inuse==TRUE)
	{
		hResult = TCMutexLock(drsLockMutexSemID);
		if (hResult != NO_ERROR) return hResult;

		if (descriptorList[index].inuse==TRUE)
		{
			descriptorList[index].inuse=FALSE;
			found=1;
		}

		TCMutexUnlock(drsLockMutexSemID);
	}
	
	if (!found)
	{
		hResult=E_DRS_NO_DESCRIPTOR;	
		sysLogError(hResult, __LINE__, moduleName);
	}
		
	return hResult;
}
	
/*	
	drsFindDescriptor
	
	This function finds a specified descriptor, dependent on the
	arguments passed in the arguments parameter.
	
	The parent can be a descriptor of any type, also allow a search via
	a linear method. 

*/
HRESULT	drsFindDescriptor(EDS_DESCRIPTOR_REF  parent,
						  KEYVALUE*	   arguments,
						  EDS_DESCRIPTOR_REF* result)
{
	HRESULT			hResult=NO_ERROR;
	EDS_DESCRIPTOR*	parentdata=NULL;
	uint32			childtype=0;

	if (NO_ERROR!=kvIsKey(arguments,DRS_FINDKEY_EDS_TYPE))
	{
		hResult=E_DRS_FIND_REQUIRES_EDSTYPE;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	/*
		1st Method of searching, linear search for lists. 
		This is only valid for lists, you can't search for
		entries and infoblocks in this manner, as they are 
		not free floating objects.
	*/
	if (parent==EDS_DESCRIPTORTYPE_NULL)
	{
		uint32				index=0;
		BOOL				found=0;
		AVC_ADDRESS_SUBUNIT subunit= {0,0};
		uint32				listtype=0;
		uint32				searchonlisttype=0;
		uint32				listid=0;
		uint32				objectid=0;
		uint32				searchonlistid=0;
		uint32				searchonobjectid=0;
	
		/*
			Only lists can be found without having to know about the parents.
		*/
		if (NO_ERROR!=kvIsValue(arguments,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_LIST))
		{
			if (NO_ERROR==kvIsValue(arguments,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_ENTRY))
			{
				// Search for entry with object id
				searchonobjectid = 1;
			}		
			else
			{
				hResult=E_DRS_FIND_REQUIRES_EDSTYPE_LIST;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
		}		

		
		if (NO_ERROR!=kvGetValue(arguments,DRS_FINDKEY_BY_SUBUNITID,&subunit.subunit_ID))
		{
			hResult=E_DRS_FIND_REQUIRES_SUBUNITID;	
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		
		if (NO_ERROR!=kvGetValue(arguments, DRS_FINDKEY_BY_SUBUNITTYPE, &subunit.subunit_type))
		{
			hResult=E_DRS_FIND_REQUIRES_SUBUNITTYPE;	
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		

		if (searchonobjectid)
		{
			if (NO_ERROR!=kvGetValue(arguments, DRS_FINDKEY_BY_OBJECTID, &objectid))
			{
				hResult=E_DRS_FIND_MISSING_KEY_ARGUMENT;	
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}	
		}
		else
		{
			/*
				Check for the key information fields for this search
				one of these key types is required. If both exist then
				only the objectid is used.
			*/
			searchonlistid=1;
			if (NO_ERROR!=kvGetValue(arguments, DRS_FINDKEY_BY_OBJECTID, &listid))
			{
				searchonlisttype=1;
				searchonlistid=0;
				if (NO_ERROR!=kvGetValue(arguments, DRS_FINDKEY_BY_OBJECTTYPE, &listtype))
				{
					hResult=E_DRS_FIND_MISSING_KEY_ARGUMENT;	
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				}	
			}
		}
	
		hResult = TCMutexLock(drsLockMutexSemID);
		if (hResult != NO_ERROR) return hResult;
		
		/*
			Search the list.
		*/
		while (index < ARRAYSIZE(descriptorList))
		{
			if (descriptorList[index].inuse)
			{
				if (searchonobjectid)
				{
					// Search on entries
					if (descriptorList[index].descriptor.type==EDS_DESCRIPTORTYPE_ENTRY)
					{
						if (descriptorList[index].descriptor.u.entry_descriptor.object_id==objectid)
						{
							if (isSubunitsEqual(descriptorList[index].descriptor.subunit, subunit))
							{
								*result=index+1;
								found=1;
								break;	
							}
						}
					}
				}
				else
				{
					// Search on lists
					if (descriptorList[index].descriptor.type==EDS_DESCRIPTORTYPE_LIST)
					{
					
						if (searchonlistid)
						{
							if (descriptorList[index].descriptor.u.list_descriptor.list_id==listid)
							{
								if (isSubunitsEqual(descriptorList[index].descriptor.subunit, subunit))
								{
									*result=index+1;
									found=1;
									break;	
								}
							}
						}
					
						if (searchonlisttype)
						{
							if (descriptorList[index].descriptor.u.list_descriptor.type==listtype)
							{
								if (isSubunitsEqual(descriptorList[index].descriptor.subunit, subunit))
								{
									*result=index+1;
									found=1;
									break;	
								}
							}
						}
					
					}
				}
			}
			index++;
		}
		
		if (!found)
		{
			hResult=E_DRS_NO_DESCRIPTOR;	
			sysLogError(hResult, __LINE__, moduleName);
		}
		else
		{
			hResult=NO_ERROR;	
		}
		
		TCMutexUnlock(drsLockMutexSemID);

		return hResult;
	}

	/*
		Okay the next search is done by switching on the type of the parent.
		We are interested in searching for the parent's child of the specified
		type.  Since we are using a parent, we don't require a subunitid, or subunittype.
		
		First get the parent's EDS_DESCRIPTOR* from the EDS_DESCRIPTOR_REF
	*/
	hResult=drsLockDescriptor(parent);
	if (NO_ERROR==hResult)
	{
		hResult=drsGetDescriptor(parent,&parentdata);
	}

	/*
		Check to see if the child descriptor specifier, and the parent descriptor, are
		indeed a valid search.
	*/
	if (NO_ERROR==hResult)
	{
		kvGetValue(arguments,DRS_FINDKEY_EDS_TYPE,&childtype);
		switch(parentdata->type)
		{
			case EDS_DESCRIPTORTYPE_LIST:
			{
				if (childtype==EDS_DESCRIPTORTYPE_LIST ||
					childtype==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
				{
					hResult=E_DRS_FIND_MISMATCHED_CHILD_PARENT;
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			break;
			
			case EDS_DESCRIPTORTYPE_ENTRY:
			{
				if (childtype==EDS_DESCRIPTORTYPE_ENTRY ||
				 	childtype==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
				{
					hResult=E_DRS_FIND_MISMATCHED_CHILD_PARENT;
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			break;
					
			case EDS_DESCRIPTORTYPE_INFOBLOCK:
			{
				if (childtype==EDS_DESCRIPTORTYPE_ENTRY ||
					childtype==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER ||
					childtype==EDS_DESCRIPTORTYPE_LIST)
				{
					hResult=E_DRS_FIND_MISMATCHED_CHILD_PARENT;	
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			break;
					
			case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
			{
				if (childtype==EDS_DESCRIPTORTYPE_ENTRY ||
					childtype==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
				{
					hResult=E_DRS_FIND_MISMATCHED_CHILD_PARENT;	
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			break;	
		}
	}
	
	/*
		ENTRY SEARCHES
		
		This section of the code searches for children of type ENTRY
		This only works of course if the parent we are searching is of
		a list.
		
		Since we compared parents and children to the respective parent, 
		we don't need to another comparison here to determine if the 
		parent is compatible.
	*/ 
	if (hResult==NO_ERROR && childtype==EDS_DESCRIPTORTYPE_ENTRY)
	{
		uint32				index=1;
		EDS_DESCRIPTOR_REF	curdescref=0;
		EDS_DESCRIPTOR*		curdesc=NULL;
		uint32				found=0;
		
		/*
			Search using the position as the key.
		*/
		if (NO_ERROR==kvIsKey(arguments,DRS_FINDKEY_BY_POSITION))
		{
			uint32 	positionargument=0;
			kvGetValue(arguments, DRS_FINDKEY_BY_POSITION, &positionargument);
			
			while (NO_ERROR==drsGetNthDescriptor(index,&curdescref) && !found)
			{
				drsLockDescriptor(curdescref);
				drsGetDescriptor(curdescref,&curdesc);
				
				if (curdesc->type==EDS_DESCRIPTORTYPE_ENTRY)
				{
					if (isSubunitsEqual(curdesc->subunit, parentdata->subunit) &&
						curdesc->u.entry_descriptor.position==positionargument)
					{
						*result=curdescref;
						found=1;
					}	
				}
				drsUnlockDescriptor(curdescref);
				
				index++;	
			}
			
			if (found)
			{
				/* We found the entry designated by the position 
				   passed in.
				*/
				return NO_ERROR;	
			}
			else
			{
				/*
					We couldn't find the specified descriptor
				*/
				hResult=E_DRS_NO_DESCRIPTOR;		
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
		}
		
		/*
			Search for an entry based on the object id.
		*/
		if (NO_ERROR==kvIsKey(arguments,DRS_FINDKEY_BY_OBJECTID))
		{
			uint32	objectargument=0;
			kvGetValue(arguments, DRS_FINDKEY_BY_OBJECTID, &objectargument);
			
			while (NO_ERROR==drsGetNthDescriptor(index,&curdescref) && !found)
			{
				drsLockDescriptor(curdescref);
				drsGetDescriptor(curdescref,&curdesc);
				
				if (curdesc->type==EDS_DESCRIPTORTYPE_ENTRY)
				{
					if (isSubunitsEqual(curdesc->subunit, parentdata->subunit) &&
						curdesc->u.entry_descriptor.object_id==objectargument)
					{
						*result=curdescref;
						found=1;
					}	
				}
				drsUnlockDescriptor(curdescref);
				
				index++;	
			}
			
			if (found)
			{
				/* We found the entry designated by the position 
				   passed in.
				*/
				return NO_ERROR;	
			}
			else
			{
				/*
					We couldn't find the specified descriptor
				*/
				hResult=E_DRS_NO_DESCRIPTOR;		
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
		}

		hResult=E_DRS_FIND_MISSING_KEY_ARGUMENT;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	/*
		LIST SEARCH
		
		This section of the code searches for children of type LIST
		This only works of course if the parent we are searching is of
		an entry or a subunitidentifier.
	
		When the parent is an entry, an only an index search is allowed.
		
		Since we compared parents and children to the respective parent, 
		we don't need to another comparison here to determine if the 
		parent is compatible.
	*/ 
	if (hResult==NO_ERROR && childtype==EDS_DESCRIPTORTYPE_LIST)
	{
		uint32				index=1;
		EDS_DESCRIPTOR_REF	curdescref=0;
		EDS_DESCRIPTOR*		curdesc=NULL;
		uint32				found=0;
		
		/*
			Search using the position as the key.
			
			But only when the parent is as subunitidentifier.
		*/
		if (parentdata->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER && NO_ERROR==kvIsKey(arguments,DRS_FINDKEY_BY_POSITION))
		{
			uint32 	positionargument=0;
			uint32	counter=0;
			kvGetValue(arguments, DRS_FINDKEY_BY_POSITION, &positionargument);
			
			while (NO_ERROR==drsGetNthDescriptor(index,&curdescref) && !found)
			{
				drsLockDescriptor(curdescref);
				drsGetDescriptor(curdescref,&curdesc);
				
				
				if (curdesc->type==EDS_DESCRIPTORTYPE_LIST)
				{
					if (isSubunitsEqual(curdesc->subunit, parentdata->subunit) &&
						curdesc->parent_reference==parent )
					{
						counter++;
						if (counter==positionargument)
						{
							*result=curdescref;
							found=1;
						}
					}	
				}
				drsUnlockDescriptor(curdescref);
				
				index++;	
			}
			
			if (found)
			{
				/* We found the entry designated by the position 
				   passed in.
				*/
				return NO_ERROR;	
			}
			else
			{
				/*
					We couldn't find the specified descriptor
				*/
				hResult=E_DRS_NO_DESCRIPTOR;		
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
		}
		
		/*
			Search for an entry based on the object id.\
			We do this by using the list ID search mechanism, and building
			a set of search arguments, which we feed into this function
			again.
		*/
		if (parentdata->type==EDS_DESCRIPTORTYPE_ENTRY)
		{
			*result=parentdata->u.entry_descriptor.child_list_reference;
			return NO_ERROR;
		}

		hResult=E_DRS_FIND_MISSING_KEY_ARGUMENT;		
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	/*
		INFOBLOCK Search
		
		Okay this is the cincher of all searches,
		
		this section code searches for infoblocks amongst the various
		parents.  It can be a search which is based upon a particular
		buffer of the parent, or may be amongst ALL buffers of the 
		parent.
	
		This code is quite  unique per parent descriptor. 
	*/
	if (NO_ERROR==hResult && childtype==EDS_DESCRIPTORTYPE_INFOBLOCK) 
	{
		uint32				buffertype=EDS_BUFFERTYPE_UNSPECIFIED;
		EDS_DESCRIPTOR_REF	curdescref=0;
		EDS_DESCRIPTOR*		curdesc=NULL;
		uint32				index=1;
		BOOL				found=0;
		
		kvGetValue(arguments,DRS_FINDKEY_BY_BUFFERTYPE,&buffertype);
		
		/*
			Lookup the infoblock by instance and type.
		*/
		if (NO_ERROR==kvIsKey(arguments,DRS_FINDKEY_BY_POSITION) &&
			NO_ERROR==kvIsKey(arguments,DRS_FINDKEY_BY_OBJECTID))
		{
			uint32 	positionargument=0;
			uint32	objectargument=0;
			uint32	counter=0;
			
			kvGetValue(arguments, DRS_FINDKEY_BY_POSITION, &positionargument);
			kvGetValue(arguments, DRS_FINDKEY_BY_OBJECTID, &objectargument);
			
			while (NO_ERROR==drsGetNthDescriptor(index,&curdescref) && !found)
			{
				drsLockDescriptor(curdescref);
				drsGetDescriptor(curdescref,&curdesc);
					
				if (curdesc->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
				{
					
					if (isSubunitsEqual(curdesc->subunit, parentdata->subunit) &&
						curdesc->parent_reference==parent &&
						(buffertype ? buffertype==curdesc->u.info_block.buffertype : TRUE) )
					{
						
						if (curdesc->u.info_block.type==objectargument)
						{	
							if (counter==positionargument)
							{
								*result=curdescref;
								found=1;
							}
							counter++;
						}
					}	
				}
				drsUnlockDescriptor(curdescref);
				
				index++;	
			}
			
			if (found)
			{
				return NO_ERROR;	
			}
			else
			{
				hResult=E_DRS_NO_DESCRIPTOR;		
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}				
		}
		
		/*
			Lookup the infoblock by the position specified.
		*/	
		if (NO_ERROR==kvIsKey(arguments,DRS_FINDKEY_BY_POSITION))
		{
			uint32 	positionargument=0;
			uint32	counter=0;
			kvGetValue(arguments, DRS_FINDKEY_BY_POSITION, &positionargument);
			
			while (NO_ERROR==drsGetNthDescriptor(index,&curdescref) && !found)
			{
				drsLockDescriptor(curdescref);
				drsGetDescriptor(curdescref,&curdesc);
					
				if (curdesc->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
				{
					if (isSubunitsEqual(curdesc->subunit, parentdata->subunit) &&
						curdesc->parent_reference==parent &&
						(buffertype ? buffertype==curdesc->u.info_block.buffertype : TRUE) )
					{
						counter++;
						if (counter==positionargument)
						{
							*result=curdescref;
							found=1;
						}
					}	
				}
				drsUnlockDescriptor(curdescref);
				
				index++;	
			}
			
			if (found)
			{
				return NO_ERROR;	
			}
			else
			{
				hResult=E_DRS_NO_DESCRIPTOR;		
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}			
		}


		/*
			Search for the infoblock by infoblock type.
		*/
		if (NO_ERROR==kvIsKey(arguments,DRS_FINDKEY_BY_POSITION))
		{
			uint32 	typeargument=0;
			kvGetValue(arguments, DRS_FINDKEY_BY_OBJECTTYPE, &typeargument);
			
			while (NO_ERROR==drsGetNthDescriptor(index,&curdescref) && !found)
			{
				drsLockDescriptor(curdescref);
				drsGetDescriptor(curdescref,&curdesc);
					
				if (curdesc->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
				{
					if (isSubunitsEqual(curdesc->subunit, parentdata->subunit) &&
						curdesc->parent_reference==parent &&
						curdesc->u.info_block.type==typeargument && 
						(buffertype ? buffertype==curdesc->u.info_block.buffertype : TRUE) )
					{

						*result=curdescref;
						found=1;
					}	
				}
				drsUnlockDescriptor(curdescref);
				
				index++;	
			}
			
			if (found)
			{
				return NO_ERROR;	
			}
			else
			{
				hResult=E_DRS_NO_DESCRIPTOR;		
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}			
		}
		
		hResult=E_DRS_FIND_MISSING_KEY_ARGUMENT;		
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	/*
		OPAQUE DESCRIPTOR SEARCHES
	*/ 
	if (hResult==NO_ERROR && childtype==EDS_DESCRIPTORTYPE_OPAQUE)
	{
		uint32				index=1;
		EDS_DESCRIPTOR_REF	curdescref=0;
		EDS_DESCRIPTOR*		curdesc=NULL;
		uint32				found=0;
		
		/*
			Search assumes there is one opaque descriptor
		*/
		while (NO_ERROR==drsGetNthDescriptor(index,&curdescref) && !found)
		{
			drsLockDescriptor(curdescref);
			drsGetDescriptor(curdescref,&curdesc);
			
			if (curdesc->type==EDS_DESCRIPTORTYPE_OPAQUE)
			{
				if (isSubunitsEqual(curdesc->subunit, parentdata->subunit))
				{
					*result=curdescref;
					found=1;
				}	
			}
			drsUnlockDescriptor(curdescref);
			
			index++;	
		}
			
		if (found)
		{
			/* We found the entry designated by the position 
				passed in.
			*/
			return NO_ERROR;	
		}
		else
		{
			/*
				We couldn't find the specified descriptor
			*/
			hResult=E_DRS_NO_DESCRIPTOR;		
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		
		hResult=E_DRS_FIND_MISSING_KEY_ARGUMENT;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	
	return hResult;
}


/*
	drsLockDescriptor
	
	This function locks the descriptor so that is can be opened
	and read.
	
*/

HRESULT	drsLockDescriptor(EDS_DESCRIPTOR_REF	reference)
{
	HRESULT		hResult=NO_ERROR;
	uint32		index=reference-1;
	uint32		sleepcount=0;
	BOOL		locktrip=FALSE;

	if (index >= ARRAYSIZE(descriptorList))
	{
		hResult=E_DRS_BAD_REFERENCE;		
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}


	if (descriptorList[index].inuse==FALSE)
	{
		hResult=E_DRS_NO_DESCRIPTOR;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

/*
	Determine if this thread has already locked this descriptor.
*/	
	if (descriptorList[index].owner_taskID==TCTaskGetMyID())
	{
		descriptorList[index].usagecount++;
		return NO_ERROR;	
	}

	hResult = TCMutexLock(drsLockMutexSemID);
	if (hResult != NO_ERROR) return hResult;

/*
	Loop until we acquire the lock this descriptor,
	or until we timeout (which is set to 100 ms)
*/
	while (descriptorList[index].owner_taskID!=0)
	{
		TCMutexUnlock(drsLockMutexSemID);
		TCTaskWait(4); // Sleep 4 milliseconds. Giving other threads the ability to run.

		sleepcount+=4;
		if (sleepcount >= 100 && sleepcount < 104)
		{
			sysDebugPrintf("DRS: Thread 0x%p failed to acquire lock on descriptor %d  after 100 msecs.",TCTaskGetMyID(),reference);		
			hResult=E_DRS_ALREADY_LOCKED;
			return hResult;
		}
		if (descriptorList[index].inuse==FALSE && !locktrip)
		{
			sysDebugPrintf("DRS: Thread 0x%p failed to acquire lock on descriptor %d because it was unregistered.",TCTaskGetMyID(),reference);		
			hResult=E_DRS_ALREADY_LOCKED;
			return hResult;
		}

		hResult = TCMutexLock(drsLockMutexSemID);
		if (hResult != NO_ERROR) return hResult;
	}
	
/*
	Now we must have the descriptor's thread lock.
*/
	descriptorList[index].owner_taskID=TCTaskGetMyID();
	descriptorList[index].usagecount++;
	
	TCMutexUnlock(drsLockMutexSemID);
		
	return hResult;
}


/*
	drsUnlockDescriptor
	
	
	This function unlocks the descriptor so that it can now be 
	locked by another thread.
*/
HRESULT	drsUnlockDescriptor(EDS_DESCRIPTOR_REF	reference)
{
	HRESULT		hResult=NO_ERROR;
	uint32		index=reference-1;

	if (index >= ARRAYSIZE(descriptorList))
	{
		hResult=E_DRS_BAD_REFERENCE;		
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (descriptorList[index].inuse==FALSE)
	{
		hResult=E_DRS_NO_DESCRIPTOR;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

/*
	Determine if this thread has already locked this descriptor.
*/	
	if (descriptorList[index].owner_taskID!=TCTaskGetMyID())
	{
		hResult=E_DRS_NOT_YOUR_LOCK;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = TCMutexLock(drsLockMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	// Only unlock the descriptor if it is unlocked the same number of
	// times it is locked.
	if (descriptorList[index].usagecount)
	{
		descriptorList[index].usagecount--;
	}
	if (descriptorList[index].usagecount == 0)
	{
		descriptorList[index].owner_taskID=0;
	}
	
	TCMutexUnlock(drsLockMutexSemID);	
	
	return NO_ERROR;
}



// THis function is used for debugging purposes only.  It allows for a developer to
// peak into a device and see if locks are being left on descriptors.

uint32	drsLockCount(EDS_DESCRIPTOR_REF	reference)
{
	uint32		index=reference-1;
	uint32		hResult=NO_ERROR;

	if (index >= ARRAYSIZE(descriptorList))
	{
		return 0;		
	}

	if (descriptorList[index].inuse==FALSE)
	{
		return 0;	
	}

	hResult = TCMutexLock(drsLockMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	// Only unlock the descriptor if it is unlocked the same number of
	// times it is locked.
	
	hResult=descriptorList[index].usagecount;
	
	TCMutexUnlock(drsLockMutexSemID);	
		
	return hResult;
}

/*
	drsGetDescriptor
	
	This function gets the pointer of a locked descriptor, so that you 
	can access the actual data structure of the descriptor.
*/

HRESULT	drsGetDescriptor(EDS_DESCRIPTOR_REF	  reference,  EDS_DESCRIPTOR**	descriptor)
{
	uint32		index=reference-1;
	uint32		hResult=NO_ERROR;

	if (index >= ARRAYSIZE(descriptorList))
	{
		hResult=E_DRS_BAD_REFERENCE;		
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (descriptorList[index].inuse==FALSE)
	{
		hResult=E_DRS_NO_DESCRIPTOR;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

/*
	Determine if this thread has already locked this descriptor.
*/	
	if (descriptorList[index].owner_taskID==0)
	{
		hResult=E_DRS_NOT_LOCKED_YET;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (descriptorList[index].owner_taskID!=TCTaskGetMyID())
	{
		hResult=E_DRS_NOT_YOUR_LOCK;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = TCMutexLock(drsLockMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	*descriptor=&(descriptorList[index].descriptor);	
	
	TCMutexUnlock(drsLockMutexSemID);	
	
	return NO_ERROR;
}

/*
	drsGetNthDescriptor
	
	Function for allowing fetching of descriptors.
	
*/

HRESULT	drsGetNthDescriptor(uint32	index, EDS_DESCRIPTOR_REF*	  reference)
{
	HRESULT		hResult=NO_ERROR;
	uint32		counter=0;
	uint32		localindex=0;
	BOOL		found=0;
	
	while (localindex < ARRAYSIZE(descriptorList) && (!found))
	{
		if (descriptorList[localindex].inuse==TRUE)
		{
			hResult = TCMutexLock(drsLockMutexSemID);
			if (hResult != NO_ERROR) return hResult;

			if (descriptorList[localindex].inuse==TRUE)
			{
				counter++;
				if (counter==index)
				{
					*reference=localindex+1;
					found=1;
					TCMutexUnlock(drsLockMutexSemID);	
					break;
				}
			}

			TCMutexUnlock(drsLockMutexSemID);	
		}
		localindex++;
	}
	
	if (!found)
	{
		hResult=E_DRS_INDEX_OUT_OF_RANGE;	
	}
		
	return hResult;	
}
#endif //_EDS

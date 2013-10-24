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
#ifndef	_DRS_H
#define _DRS_H
#ifdef _EDS

#include "TCTypes.h"
#include "avcDefs.h"
#include "edstypes.h"

#ifndef		_DRS_DESCRIPTOR_COUNT	
#define		_DRS_DESCRIPTOR_COUNT	100
#endif

/*
	FindSubunitIdentifier
*/

#define		DRS_FINDKEY_EDS_TYPE		0x01 // Which type of EDS descriptor do we want to find.
#define		DRS_FINDKEY_BY_POSITION		0x05 // How to find the wanted descriptor, when used the data is position from 0 to n.
#define		DRS_FINDKEY_BY_OBJECTID		0x06 // Only applies to the entry descriptors.  
#define		DRS_FINDKEY_BY_OBJECTTYPE 	0x07 // Find by the "list type" or the "entry type" or the "infoblock type"
#define		DRS_FINDKEY_BY_SUBUNITID	0x09 // When no parent is defined in the find descriptor function, use this subunitid.
#define		DRS_FINDKEY_BY_SUBUNITTYPE	0x0A // When no parent is defined in the find descriptor function, use this subunittype.
#define		DRS_FINDKEY_BY_BUFFERTYPE	0x0D 


/*******************************************************************************
Function: drsInitialize  

	Initializes the DRS. 
	
	ARGUMENTS:
	----------
					VOID
	
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is happy. 
	
	?					 - Other errors of unknown origin.

********************************************************************************/
HRESULT	drsInitialize(void);


/*******************************************************************************
Function: drsFindSubunitIdentifier  

	Looks up a subunit identifier for a particular subunit.

	Monitor Output:
	---------------
	"DRS: SubunitIdentifier  ID:%d Type:%d could not be found."
	
	ARGUMENTS:
	----------
		subunittype- the TYPE of the subunit to look up the SubunitIdentifier
		subunitid  - the ID of the subunit to look up the SubunitIdentifier
		descriptor - Reference of the descriptor. 
		
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is happy. 
	
	E_DRS_NO_DESCRIPTOR - Descriptor not found.

********************************************************************************/
HRESULT	drsFindSubunitIdentifier(AVC_ADDRESS_SUBUNIT subunit, EDS_DESCRIPTOR_REF*  descriptor);

/*******************************************************************************
Function: drsRegisterDescriptor  

	Copies the descriptor as specified, into the DRS.  This allocates 
	a buffer from the lists which are allocated statically.  
	
	Monitor Output:
	---------------
	"DRS: Cannot register duplicate descriptor"
	"DRS: Out of descriptor space."
	
	ARGUMENTS:
	----------
		descriptor	-  pointer to a EDS_DESCRIPTOR.   
		reference   -  pointer to an EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is fine.
	E_DRS_NO_DESCRIPTOR_SPACE - The DRS does not have enough allocated memory to 
						   register the descriptor you passed in.
						   
	

********************************************************************************/
HRESULT	drsRegisterDescriptor(EDS_DESCRIPTOR*	descriptor, EDS_DESCRIPTOR_REF*	 reference);

/*******************************************************************************
Function: drsUnregisterDescriptor  

	This function releases the memory used by the specified descriptor.
	This does not release the memory used by the buffers which this descriptor
	uses to store data related to the descriptor.  
	
	You are responsible for any buffer management, in order to figure out what buffers
	are currently associated with the specified descriptor, you must use the 
	"drsFindDescriptor" command to retrieve the appropriate descriptor's buffer
	pointers.
	
	Monitor Output:
	---------------
	"DRS: Specified descriptor reference %d could not be found."
	
	ARGUMENTS:
	----------
		reference   -  pointer to an EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is fine.
	E_DRS_NO_DESCRIPTOR - Descriptor not found.
						   
	

********************************************************************************/
HRESULT	drsUnregisterDescriptor(EDS_DESCRIPTOR_REF	reference);

/*******************************************************************************
Function: drsFindDescriptor  

	This function allows for a descriptor to be found based on the data
	given.  This allows for easy lookup of descriptors, making it easy
	to do address translation.  
	
	
	Monitor Output:
	---------------
	"DRS: Specified descriptor reference %d could not be found."
	"DRS: Search arguments passed in were illegal."
	
	ARGUMENTS:
	----------
		parent		-  A descriptor reference to the parent which you wish
					   to a find a child descriptor for. This will search only
					   the top level of the list descriptor, and will not recurse
					   down to the next level of the lists.  This is the same for
					   infoblocks.  This argument cannot be an ENTRY descriptor,
					   only an INFOBLOCK or a LIST descriptor.
		
		arguments	-  A KEYVALUE list which contains the information you wish
					   to search for. These arguments are currently undefined.
					   
		reference   -  pointer to an EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is fine.
	E_DRS_NO_DESCRIPTOR - Descriptor not found.
	E_DRS_ILLEGAL_SEARCH- The arguments passed in were illegal.						   
	

********************************************************************************/
HRESULT	drsFindDescriptor(EDS_DESCRIPTOR_REF	parent,
						  KEYVALUE*				arguments,
						  EDS_DESCRIPTOR_REF*	result);

/*******************************************************************************
Function: drsLockDescriptor  

	This function makes it impossible for another thread to get the descriptor's
	contents.  The second thread is suspended indefinitely until the the
	initial thread to "Lock" this descriptor, calls the drsUnlockDescriptor 
	function.
	
	Warnings will be output when the function fails to get a lock.   	
	
	Monitor Output:
	---------------
	"DRS: Specified descriptor reference %d could not be found."
	"DRS: Descriptor %d is already locked, waiting indefinitely."
	"DRS: Descriptor %d has been locked for more than 10 seconds by thread %d."
	
	ARGUMENTS:
	----------
		reference   -  pointer to an EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is fine.
	E_DRS_NO_DESCRIPTOR - Descriptor not found.
	E_DRS_ALREADY_LOCKED- The specified descriptor is already locked.
	

********************************************************************************/
HRESULT	drsLockDescriptor(EDS_DESCRIPTOR_REF	reference);

/*******************************************************************************
Function: drsUnlockDescriptor  

	This function makes it impossible for another thread to get the descriptor's
	contents.  The second thread is suspended indefinitely until the the
	initial thread to "Lock" this descriptor, calls the drsUnlockDescriptor 
	function.
	
	Warnings will be output when the function fails to get a lock.   	
	
	Monitor Output:
	---------------
	"DRS: Specified descriptor reference %d could not be found."
	"DRS: Descriptor %d was not locked by thread %d."
	
	ARGUMENTS:
	----------
		reference   -  pointer to an EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is fine.
	E_DRS_NO_DESCRIPTOR - Descriptor not found.
	E_DRS_NOT_YOUR_LOCK - The thread which locked this descriptor was not the 
						   same as the current thread.	

********************************************************************************/
HRESULT	drsUnlockDescriptor(EDS_DESCRIPTOR_REF	reference);


/*******************************************************************************
Function: drsGetDescriptor  

	When a descriptor is "locked" the data associated with it will be available
	for the thread which locked the descriptor to access.  This function 
	will return an EDS_DESCRIPTOR* to the caller which can be used to 
	access the data.
	
	When you are done with this descriptor, you must call "drsUnlockDescriptor", 
	so that other threads will have access to this thread as well.
	
	Monitor Output:
	---------------
	"DRS: Specified descriptor reference %d could not be found."
	"DRS: Descriptor %d was not locked by this thread %d."
	"DRS: Descriptor %d is still unlocked. Access Denied."
	
	ARGUMENTS:
	----------
		reference   -  EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
		
		descriptor  -  pointer to an EDS_DESCRIPTOR* which will contain
					   a pointer to the actual data of the descriptor. 
					   (Actual data does not include infoblocks, and entries.)
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is fine.
	E_DRS_NO_DESCRIPTOR - Descriptor not found.
	E_DRS_NOT_YOUR_LOCK - The thread which locked this descriptor was not the 
						   same as the current thread.	
	E_DRS_NOT_LOCKED_YET- The descriptor you were attempting to access, has not
						   been locked at all yet.

********************************************************************************/
HRESULT	drsGetDescriptor(EDS_DESCRIPTOR_REF	  reference,  EDS_DESCRIPTOR**	descriptor);


/*******************************************************************************
Function: drsGetNthDescriptor  

	Returns the descriptor reference of the NTH requested. 
	
	Monitor Output:
	---------------
	"DRS: Specified descriptor reference %d could not be found."
	"DRS: Descriptor %d was not locked by this thread %d."
	"DRS: Descriptor %d is still unlocked. Access Denied."
	
	ARGUMENTS:
	----------
		reference   -  EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
		
		descriptor  -  pointer to an EDS_DESCRIPTOR* which will contain
					   a pointer to the actual data of the descriptor. 
					   (Actual data does not include infoblocks, and entries.)
			
	RETURNVALUE:  HRESULT
	-------------------------
						   
	NO_ERROR				 - Everything is fine.
	E_DRS_NO_DESCRIPTOR - Descriptor not found.

********************************************************************************/
HRESULT	drsGetNthDescriptor(uint32	index, EDS_DESCRIPTOR_REF*	  reference);



/*******************************************************************************
Function: drsGetNthDescriptor  

	Returns the number of locks on a specific descriptor. These locks are 
	created when the function "drsLockDescriptor" is issued. This function 
	is also DEBUG Only.
	
	ARGUMENTS:
	----------
		reference   -  EDS_DESCRIPTOR_REF which will contain a
					   valid reference to the registered EDS_DESCRIPTOR.
					
	RETURNVALUE:  HRESULT
	-------------------------
						   
	>= 0

********************************************************************************/
uint32 drsLockCount(EDS_DESCRIPTOR_REF	reference);


#endif //_EDS
#endif //_DRS_H

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

 DESCRIPTION: 


		Implementation of a global callback registry.
		This allows for a single point of failure for allocating, retrieving 
		of callbacks.  Also it gives us another method for looking into a 
		AVC device and determining the state of the device from a command line.

  Revisions:
		created 21/03/2003 KBJ/LM

****************************************************************************/
static char 		moduleName[] = __FILE__ ;

#include    "TCTasking.h"
#include	"cr.h"
#include 	"crCli.h"

// semaphore IDs for this module
uint32							crListMutexSemID;		// mutual exclusion semaphore for cr list access

static	CALLBACK_DESCRIPTOR*	crDescHead = NULL;

HRESULT crInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	// exclusive access for the cr list (mutex)
	hResult = TCMutexOpen(&crListMutexSemID);
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_TOOL_AVC_CR
	crCliInstallTools();
#endif //_CLI_TOOL_AVC_CR

	return hResult;
}

BOOL crFindCallback(uint32 mode, uint32 type, uint32 index, CALLBACK_DESCRIPTOR* findcallback, CALLBACK_DESCRIPTOR** callback)
{
	HRESULT					hResult = NO_ERROR;
	CALLBACK_DESCRIPTOR*	current = crDescHead;
	CALLBACK_DESCRIPTOR*	next = NULL;
	uint32					counter = 0;
	uint32					foundtype = 0;
	BOOL					bFound = FALSE;

	// exclusive access for the cr list (mutex)
	hResult = TCMutexLock(crListMutexSemID);
	if (hResult != NO_ERROR) return FALSE;
	
	while (current)
	{
		switch (mode)
		{
			case CR_FIND_CALLBACK_TYPE:
				kvGetValue(current, CALLBACK_DESCRIPTOR_TYPE, &foundtype);
				if (foundtype == type)
				{
					bFound = (counter == index);
					counter++;	
				}
				break;
			
			case CR_FIND_CALLBACK_FIRST:
				bFound = TRUE;
				break;

			case CR_FIND_CALLBACK_THIS:
				bFound = (current == findcallback);
				break;

			case CR_FIND_CALLBACK_PREV:
				kvGetValue(current, CALLBACK_DESCRIPTOR_NEXT, (uint32 *) &next);
				if (!next) break;
				bFound = (next == findcallback);
				break;

			case CR_FIND_CALLBACK_NEXT:
				kvGetValue(findcallback, CALLBACK_DESCRIPTOR_NEXT, (uint32 *) &next);
				current = next;
				if (!current) continue;
				bFound = TRUE;
				break;
		}
		
		if (bFound == TRUE)
		{
			*callback = current;
			break;
		}

		kvGetValue(current, CALLBACK_DESCRIPTOR_NEXT, (uint32 *) &next);
		if (!next) break;
						
		current = next;
	}
	
	// exclusive access for the cr list (mutex)
	TCMutexUnlock(crListMutexSemID);

	return bFound;
}	

HRESULT crAddCallback(CALLBACK_DESCRIPTOR* callback)
{
	HRESULT		hResult = NO_ERROR;

	// First check for required key types, these are described in the cr.h
	hResult = kvIsKey(callback, CALLBACK_DESCRIPTOR_TYPE);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = kvIsKey(callback, CALLBACK_DESCRIPTOR_NEXT);
	if (hResult != NO_ERROR) return hResult;
	
#ifdef _SYSDEBUG
	// check in debug mode never to allow adding of the same element twice
	if (crFindCallback(CR_FIND_CALLBACK_THIS, 0, 0, callback, &callback) == TRUE)
	{
		char*	name = NULL;
		kvGetValue(callback, CALLBACK_DESCRIPTOR_NAME, (uint32*) &name);
		sysDebugPrintf("cr error: can't add callback '%s'(0x%08X) twice\n\r", (name) ? name : "un-named", callback);
	}
#endif //_SYSDEBUG
	
	// exclusive access for the cr list (mutex)
	hResult = TCMutexLock(crListMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	// need to link this descriptor into the list, by making the crDescHead the next element, and making this the crDescHead
	kvSetValue(callback, CALLBACK_DESCRIPTOR_NEXT, (uint32) crDescHead);
	crDescHead = callback;
	
	// exclusive access for the cr list (mutex)
	TCMutexUnlock(crListMutexSemID);

	return hResult;
}

HRESULT	crGetCallback(uint32 type, uint32 index, CALLBACK_DESCRIPTOR** callback)
{
	HRESULT		hResult = NO_ERROR;

	if (crFindCallback(CR_FIND_CALLBACK_TYPE, type, index, NULL, callback) == FALSE)
	{
		hResult = E_AVC_CR_CALLBACK_NOT_FOUND;
	}
	
	return hResult;
}

HRESULT	crRemoveCallback(CALLBACK_DESCRIPTOR* callback)
{
	HRESULT					hResult = NO_ERROR;
	CALLBACK_DESCRIPTOR*	prev = NULL;
	CALLBACK_DESCRIPTOR*	next = NULL;

	if (crFindCallback(CR_FIND_CALLBACK_PREV, 0, 0, callback, &prev) == FALSE)
	{
		hResult = E_AVC_CR_CALLBACK_NOT_FOUND;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	// exclusive access for the cr list (mutex)
	hResult = TCMutexLock(crListMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	kvGetValue(callback, CALLBACK_DESCRIPTOR_NEXT, (uint32 *) &next);
	kvSetValue(callback, CALLBACK_DESCRIPTOR_NEXT, 0);
	kvSetValue(prev, CALLBACK_DESCRIPTOR_NEXT, (uint32) next);
	if (crDescHead == callback)
	{
		crDescHead = next;
	}

	// exclusive access for the cr list (mutex)
	TCMutexUnlock(crListMutexSemID);
	
	return hResult;	
}


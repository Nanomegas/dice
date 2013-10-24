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
 * DESCRIPTION: List Manager

	Purpose: 

		This file gives a common interface to a LISTManager.
		This listmanager is useful for dealing with lists which contain
		randomly accessed items, that can be added and removed. 
		
		The primary purpose I saw in this was for making the TXL keep
		track of packets, and allocations, as I needed to be able keep
		the list of data different from the idea if a list element was
		filled.
		
		The list created in unsorted, using the NTH element functions
		does not mean that the element is in position N of the physical
		list, it means that it was the NTH found item in the list.
		
		Also this list implementation is intended to be thread safe, by
		allowing only exclusive access to the contents of a particular
		element. Thus when a when you GET and element, you must make
		sure to RELEASE that element when you are finished with it. Otherwise
		any further access to that particular element will be suspended
		forever.

	Revisions:
		created 18/10/2002 lm
****************************************************************************/

static char			moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "ErrorCodes.h"
#include "listmanager.h"

#ifdef _CLI
#include "cli.h"
#endif

HRESULT lmCreateList(LM_CONTEXT* newlist,
					uint32 list_sizeinbytes,
					uint32 elem_sizeinbyte,
					uint32* elementcount)
{
	HRESULT		hResult = NO_ERROR;
	uint32		internalelementsize = 0;
	uint32		myelemcount = 0;
	uint32		counter = 0;
	LM_ELEMENT	*curelem = NULL;

	if (!newlist || list_sizeinbytes < sizeof(LM_CONTEXT))
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	memset(newlist, 0, list_sizeinbytes);
	
	internalelementsize = elem_sizeinbyte + sizeof(LM_ELEMENT);
	
	myelemcount = (list_sizeinbytes - sizeof(LM_CONTEXT)) / internalelementsize;
	if (elementcount)
	{
		*elementcount = myelemcount;
	}

	curelem = (LM_ELEMENT*) &(newlist->elements);
	
	while (counter++ < myelemcount)
	{
		if (curelem == NULL)
		{
			hResult = E_NULL_PTR;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		curelem->flags += ELEMFLAG_AVAILABLE;
		lmMoveAheadElementSize(elem_sizeinbyte, &curelem);
	}
	
	newlist->listsize = list_sizeinbytes;
	newlist->nextfree = 0;
	newlist->elemsize = elem_sizeinbyte;
	newlist->elemcount = myelemcount; // Number of allocated elements
	newlist->mode = LM_LISTMODE_EXCLUSIVEACCESS;
	
	// setup semaphores for the list (mutex)
	hResult = TCMutexOpen(&(newlist->lmMutexSemID));
		
	return hResult;
}

HRESULT lmDisposeList(LM_CONTEXT* list)
{
	HRESULT		hResult = NO_ERROR;

	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	// Exclusive access for accessing list (mutex)
	hResult = TCMutexLock(list->lmMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	memset(list, 0, list->listsize);
	
	// Release exclusive for accessing list (mutex)
	TCMutexUnlock(list->lmMutexSemID);
	
	return hResult;
}

#ifdef _CLI
HRESULT lmPrintList(LM_CONTEXT* list, LM_PRINT_LIST_ELEM_CB callback)
{
	HRESULT		hResult = NO_ERROR;
	LM_ELEMENT*	elem = 0;
	uint32		count = 0;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	cliPrintf("lmPrintList:\n\r");
	cliPrintf("listsize:\t%i\n\r", list->listsize);
	cliPrintf("nextfree:\t%i\n\r", list->nextfree);
	cliPrintf("elemsize:\t%i\n\r", list->elemsize);
	cliPrintf("allocated:\t%i\n\r", list->allocated);
	cliPrintf("elemcount:\t%i\n\r", list->elemcount);
	cliPrintf("mode:\t\t%s\n\r", (list->mode & LM_LISTMODE_EXCLUSIVEACCESS) ? "LM_LISTMODE_EXCLUSIVEACCESS" : "unknown");
	cliPrintf("lmMutexSemID:\t%i\n\r", list->lmMutexSemID);

	// walk through the list elements
	elem = (LM_ELEMENT*) &(list->elements);

	for (count = 0; count < list->elemcount; count++)
	{
		if (lmCheckElementAllocated(elem) == NO_ERROR)
		{
			cliOutputLine("------------------------------------");
			cliPrintf("element[%i]:\n\r", count);
			cliPrintf("position:\t%i\n\r", elem->position);
			cliPrintf("flags:\t\t%s%s\n\r", (elem->flags & ELEMFLAG_LOCKED) ? "ELEMFLAG_LOCKED" : "", (elem->flags & ELEMFLAG_AVAILABLE) ? "ELEMFLAG_AVAILABLE" : "");
			if (callback)
			{
				(callback) (&(elem->data));
			}
		}

		lmMoveAheadElementSize(list->elemsize, &elem);
	}
	cliOutputLine("------------------------------------");
	
	return hResult;
}
#endif //_CLI

HRESULT lmAddElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *pIndex)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(arg1);

	if (data == NULL ||
		*data == NULL)	
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lmCheckElementAvailable(elem);
	if (hResult != NO_ERROR) return E_LM_ELEMENT_NOT_FOUND;

	// found the first available element

	memcpy(&(elem->data), *data, list->elemsize);
	elem->position = (uint16) (lmGetHighestElementPosition(list) + 1);
	elem->flags -= ELEMFLAG_AVAILABLE;
	list->allocated++;

	if (pIndex)
	{
		*pIndex = index;
	}

	return hResult;
}

HRESULT lmInsertElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *pPos, uint32 *pIndex)
{
	HRESULT		hResult = NO_ERROR;

	if (data == NULL ||
		pPos == NULL ||
		*data == NULL)	
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lmCheckElementAvailable(elem);
	if (hResult != NO_ERROR) return E_LM_ELEMENT_NOT_FOUND;

	// found the first available element

	lmMoveElementPositions(list, *pPos, +1);
	
	memcpy(&(elem->data), *data, list->elemsize);
	elem->position = (uint16) *pPos;
	elem->flags -= ELEMFLAG_AVAILABLE;
	list->allocated++;

	if (pIndex)
	{
		*pIndex = index;
	}

	return hResult;
}

HRESULT lmFindElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *pIndex)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(arg1);

	if (data == NULL ||
		*data == NULL)	
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lmCheckElementAllocated(elem);
	if (hResult != NO_ERROR) return E_LM_ELEMENT_NOT_FOUND;

	if (memcmp(*data, &(elem->data), list->elemsize)) return E_LM_ELEMENT_NOT_FOUND;

	if (pIndex)
	{
		*pIndex = index;
	}

	return hResult;
}

HRESULT lmGetNthElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *pPos, uint32 *pIndex)
{
	HRESULT		hResult = NO_ERROR;

	if (data == NULL ||
		pPos == NULL)	
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lmCheckElementAllocated(elem);
	if (hResult != NO_ERROR) return E_LM_ELEMENT_NOT_FOUND;

	if (elem->position != *pPos) return E_LM_ELEMENT_NOT_FOUND;

	hResult = lmCheckElementLocked(list, elem);
	if (hResult != NO_ERROR) return hResult;

	elem->flags += ELEMFLAG_LOCKED;	
	*data = &(elem->data);

	if (pIndex)
	{
		*pIndex = index;
	}

	return hResult;
}

HRESULT lmGetElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *arg2)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(index);
	UNUSED_ARG(arg1);
	UNUSED_ARG(arg2);

	if (data == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lmCheckElementAllocated(elem);
	if (hResult != NO_ERROR) return hResult;

	hResult = lmCheckElementLocked(list, elem);
	if (hResult != NO_ERROR) return hResult;

	elem->flags += ELEMFLAG_LOCKED;	
	*data = &(elem->data);

	return hResult;
}

HRESULT lmReleaseElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *arg2)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(index);
	UNUSED_ARG(data);
	UNUSED_ARG(arg1);
	UNUSED_ARG(arg2);

	hResult = lmCheckElementAllocated(elem);
	if (hResult != NO_ERROR) return hResult;

	hResult = lmCheckElementLocked(list, elem);
	if (hResult == E_LM_ELEMENT_LOCKED)
	{
		elem->flags -= ELEMFLAG_LOCKED;
		hResult = NO_ERROR;
	}

	return hResult;
}

HRESULT lmRemoveElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *arg2)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(index);
	UNUSED_ARG(data);
	UNUSED_ARG(arg1);
	UNUSED_ARG(arg2);

	hResult = lmCheckElementAllocated(elem);
	if (hResult != NO_ERROR) return hResult;

	hResult = lmCheckElementLocked(list, elem);
	if (hResult != NO_ERROR) return hResult;

	elem->flags += ELEMFLAG_AVAILABLE;
	list->allocated--;

	lmMoveElementPositions(list, elem->position, -1);

	return hResult;
}

HRESULT lmHandleListElementWait(LM_CONTEXT* list, BOOL bIndexed, uint32 index, LM_HANDLE_INDEX_ELEMENT_CB callback, void **data, uint32 *arg1, uint32 *arg2)
{
	HRESULT		hResult = NO_ERROR;
	uint32		count = 0;
	
	DO_FOREVER
	{
		hResult = lmHandleListElement(list, bIndexed, index, callback, data, arg1, arg2);
		if (hResult != E_LM_ELEMENT_LOCKED) break;
		count++;
		if (count == 50)
		{
			TCTaskWait(1);
			count = 0;
		}
	}
	
	return hResult;
}

HRESULT lmHandleListElement(LM_CONTEXT* list, BOOL bIndexed, uint32 index, LM_HANDLE_INDEX_ELEMENT_CB callback, void **data, uint32 *arg1, uint32 *arg2)
{
	HRESULT			hResult = NO_ERROR;
	LM_ELEMENT*		elem = 0;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	// Exclusive access for accessing list (mutex)
	hResult = TCMutexLock(list->lmMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	elem = (LM_ELEMENT*) &(list->elements);

	if (bIndexed)
	{
		// call indexed elem
		if (index < list->elemcount)
		{

			lmMoveAheadElementSizeItems(list->elemsize, &elem, index);
			hResult = (callback) (list, index, elem, data, arg1, arg2);
		}
		else
		{
			hResult = E_LM_ELEMENT_UNAVAILABLE;
		}
	}
	else
	{
		// walk through the list elements and call for each elem
		uint32	count = 0;

		for (count = 0; count < list->elemcount; count++)
		{
			hResult = (callback) (list, count, elem, data, arg1, arg2);
			if (hResult != E_LM_ELEMENT_NOT_FOUND) break;
			lmMoveAheadElementSize(list->elemsize, &elem);
		}
		if (count == list->elemcount)
		{
			hResult = E_LM_ELEMENT_UNAVAILABLE;
		}
	}

	// Release exclusive for accessing list (mutex)
	TCMutexUnlock(list->lmMutexSemID);

	return hResult;
}

HRESULT lmCheckElementAvailable(LM_ELEMENT* elem)
{
	HRESULT		hResult = NO_ERROR;

	if (!(elem->flags & ELEMFLAG_AVAILABLE)) return E_LM_ELEMENT_UNAVAILABLE;

	return hResult;
}

HRESULT lmCheckElementAllocated(LM_ELEMENT* elem)
{
	HRESULT		hResult = NO_ERROR;

	if (elem->flags & ELEMFLAG_AVAILABLE) return E_LM_ELEMENT_NOT_ALLOCATED;

	return hResult;
}

HRESULT lmCheckElementLocked(LM_CONTEXT* list, LM_ELEMENT* elem)
{
	HRESULT		hResult = NO_ERROR;

	if (list->mode & LM_LISTMODE_EXCLUSIVEACCESS)
	{
		if (elem->flags & ELEMFLAG_LOCKED) return E_LM_ELEMENT_LOCKED;
	}

	return hResult;
}

HRESULT lmGetElementSize (LM_CONTEXT* list, uint32* elemsize)
{
	HRESULT		hResult = NO_ERROR;

	if (list == NULL || elemsize == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	*elemsize = list->elemsize;
	
	return hResult;
}


HRESULT lmGetAllocatedCount (LM_CONTEXT* list, uint32* allocated)
{
	HRESULT		hResult = NO_ERROR;

	if (list == NULL || allocated == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	*allocated = list->allocated;
	
	return hResult;
}

HRESULT lmSetMode(LM_CONTEXT* list, uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	list->mode = mode;
	
	return hResult;
}

HRESULT lmMoveAheadElementSizeItems(uint32 elemsize, LM_ELEMENT **element, uint32 items)
{
	HRESULT		hResult = NO_ERROR;

	while (items--)
	{
		hResult = lmMoveAheadElementSize(elemsize, element);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

HRESULT lmMoveAheadElementSize(uint32 elemsize, LM_ELEMENT **element)
{
	HRESULT		hResult = NO_ERROR;
	uint32		newpos = 0;
	
	if (*element == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	// move ahead the actual size of the element;
	newpos = (uint32) (*element);
	newpos += sizeof(LM_ELEMENT);
	newpos += elemsize;
	*element = (LM_ELEMENT*) newpos;
	
	return hResult;
}

HRESULT lmMoveElementPositions(LM_CONTEXT* list, uint32 pos, int16 delta)
{
	HRESULT		hResult = NO_ERROR;
	LM_ELEMENT*	elem = 0;
	uint32		count = 0;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	// walk through the list elements
	elem = (LM_ELEMENT*) &(list->elements);

	for (count = 0; count < list->elemcount; count++)
	{
		if (lmCheckElementAllocated(elem) == NO_ERROR)
		{
			if (elem->position >= pos)
			{
				elem->position = (uint16) (elem->position + delta);
			}
		}
		lmMoveAheadElementSize(list->elemsize, &elem);
	}

	return hResult;
}

int32 lmGetHighestElementPosition(LM_CONTEXT* list)
{
	HRESULT		hResult = NO_ERROR;
	LM_ELEMENT*	elem = 0;
	uint32		count = 0;
	int32		highest = -1;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return highest;
	}
	
	// walk through the list elements
	elem = (LM_ELEMENT*) &(list->elements);

	for (count = 0; count < list->elemcount; count++)
	{
		if (lmCheckElementAllocated(elem) == NO_ERROR)
		{
			if (elem->position > highest)
			{
				highest = elem->position;
			}
		}
		lmMoveAheadElementSize(list->elemsize, &elem);
	}
	
	return highest;
}

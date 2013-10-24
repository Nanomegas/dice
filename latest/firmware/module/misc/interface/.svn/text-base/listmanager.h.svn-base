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
		
		The list created in unsorted, using the NTH element functions
		does not mean that the element is in position N of the physical
		list, it means that it was the NTH found item in the list.

	Revisions:
		created 18/10/2002 lm
***************************************************************************/


#ifndef _LISTMANAGER_H
#define _LISTMANAGER_H

#include "TCTypes.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

#define		ELEMFLAG_AVAILABLE				BIT0
#define		ELEMFLAG_LOCKED					BIT1

#define		LM_LISTMODE_EXCLUSIVEACCESS		BIT0	// Default setting
#define		LM_LISTMODE_UNPROTECTEDACCESS	BIT1

//	LM_LISTSIZE - This macro makes calculating the size of the buffer required to hold x items of m size, simple.

#define		LM_LISTSIZE(ElementCount,ElementSize) sizeof(LM_CONTEXT)+(ElementCount*sizeof(LM_ELEMENT))+(ElementSize*ElementCount)

typedef	struct LM_ELEMENT
{
	BYTE		flags;
	uint16		position;
	void		*data;
//	BYTE		data[];
} LM_ELEMENT;

typedef	struct LM_CONTEXT
{
	uint32		listsize;
	uint32		nextfree;
	uint32		elemsize;
	uint32		allocated;	// Number of elements allocated.
	uint32		elemcount;
	uint32		mode;
	uint32		lmMutexSemID;
	void		*elements;
}	LM_CONTEXT;

/*******************************************************************************
Function: lmCreateList

	This function is used for creating a thread context out of a buffer.
	The basic idea, is that you pass in the list size, the element size, and 
	are returned a total number of elements that will fit into the buffer,
	with the context header. You are then free to access the list with the other
	listmanager calls.
	
	ARGUMENTS:
	----------
	newlist			-	Databuffer of list_sizeinbytes, which will be used to store the
						context for the list. You may freely cast any buffer to this type
						as long as you are the only user of the data.
	list_sizeinbyte	-	Number of BYTES that the buffer specified in the "newlist"
						argument.
	elem_sizeinbyte	-	Number of BYTES you intend for each element of the list to occupy.
	elemmentcount	-	Pointer to a uint32 which will if the return value is NO_ERROR 
						return the number of possible elements for this list.
********************************************************************************/


HRESULT		lmCreateList(LM_CONTEXT* newlist,
						uint32 list_sizeinbytes,
						uint32 elem_sizeinbyte,
						uint32* elementcount);


typedef HRESULT (* LM_HANDLE_INDEX_ELEMENT_CB)(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *arg2);

HRESULT		lmHandleListElementWait(LM_CONTEXT* list, BOOL bIndexed, uint32 index, LM_HANDLE_INDEX_ELEMENT_CB callback, void **data, uint32 *arg1, uint32 *arg2);

/*******************************************************************************
Function: lmAddElement

	This function adds an element to the specified list, and returns an index 
	to the element.
	
	ARGUMENTS:
	----------
	list			-	The list to which the "element" will be added.
	
	element			-	the element to be added to the list, the copy size 
						into the list element, will be dependant on the 
						list's specified element list. No larger. No Smaller.
	
	index			-	Element's indexed position.
						This is the physical position that the element occupies.
	
	
********************************************************************************/
	
HRESULT				lmAddElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *pIndex);

_TC_INLINE HRESULT	lmAddElement(LM_CONTEXT* list, void* element, uint32* index) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lmAddElement(LM_CONTEXT* list, void* element, uint32* index)
								{
									return lmHandleListElementWait(list, FALSE, 0, lmAddElementCB, &element, NULL, index);
								}

/*******************************************************************************
Function: lmFindElement

	This function determines if the element specified by the "compare_element" 
	argument is somewhere in the list itself. It does this by scanning all the
	allocated elements and doing a byte for byte compare. Obviously this is
	slow.
	
	ARGUMENTS:
	----------
	list			-	The list to which the "element" will be added.
	
	compare_element	-	the element to be located in the list.
	
	index			-	Element's indexed position.
						This is the physical position that the element occupies.
	
********************************************************************************/

HRESULT				lmFindElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *pIndex);

_TC_INLINE HRESULT	lmFindElement(LM_CONTEXT* list, void* element, uint32* index) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lmFindElement(LM_CONTEXT* list, void* element, uint32* index)
								{
									return lmHandleListElementWait(list, FALSE, 0, lmFindElementCB, &element, NULL, index);
								}

/*******************************************************************************
Function: lmGetNTHElement

	This function finds the NTH element in the list which has been allocated.
	This is useful for going through a list of 50 element spaces, and finding
	each of the elements which has actually been allocated.
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	element			-	a void** which is of the appropriate size to contain
						all the data in the element. Use lmGetElementSize,
						when allocating this buffer.
						
	pos				-	This argument specifies the element from element 0,
						to access.
	
	index			-	Element's indexed position.
						This is the physical position that the element occupies.
						Use this value with the lmReleaseElement function.

********************************************************************************/

HRESULT				lmGetNthElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *pPos, uint32 *pIndex);

_TC_INLINE HRESULT	lmGetNthElement(LM_CONTEXT* list, void** element, uint32 pos, uint32* index) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lmGetNthElement(LM_CONTEXT* list, void** element, uint32 pos, uint32* index)
								{
									return lmHandleListElementWait(list, FALSE, 0, lmGetNthElementCB, element, &pos, index);
								}

/*******************************************************************************
Function: lmGetElement

	This function returns the data of the element specified by the ABSOLUTE
	element index.
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	element			-	a void** which is of the appropriate size to contain
						all the data in the element. Use lmGetElementSize,
						when allocating this buffer.
	
	index			-	Element's indexed position.
						You must supply this to release the appropriate element.
	
********************************************************************************/

HRESULT				lmGetElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *arg2);

_TC_INLINE HRESULT	lmGetElement(LM_CONTEXT* list, void** element, uint32 index) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lmGetElement(LM_CONTEXT* list, void** element, uint32 index)
								{
									return lmHandleListElementWait(list, TRUE, index, lmGetElementCB, element, NULL, NULL);
								}

/*******************************************************************************
Function: lmReleaseElement

	This function releases the exclusive access lock, set by the GET method used
	to retrieve this chunk. Failure to call this function on an element which has
	been GOTTEN, will cause the permanent suspension of other threads attempting to
	retrieve this element from this list.
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	index			-	Element's indexed position.
						You must supply this to release the appropriate element.
	
********************************************************************************/

HRESULT				lmReleaseElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *arg2);

_TC_INLINE HRESULT	lmReleaseElement(LM_CONTEXT* list, uint32 index) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lmReleaseElement(LM_CONTEXT* list, uint32 index)
								{
									return lmHandleListElementWait(list, TRUE, index, lmReleaseElementCB, NULL, NULL, NULL);
								}

/*******************************************************************************
Function: lmRemoveElement

	This function removes the element specified by the index. 
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	index			-	Element's indexed position.
						This will remove the specified element.
	
********************************************************************************/

HRESULT				lmRemoveElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *arg1, uint32 *arg2);

_TC_INLINE HRESULT	lmRemoveElement(LM_CONTEXT* list, uint32 index) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lmRemoveElement(LM_CONTEXT* list, uint32 index)
								{
									return lmHandleListElementWait(list, TRUE, index, lmRemoveElementCB, NULL, NULL, NULL);
								}

/*******************************************************************************
Function: lmDisposeList

	This function removes the element specified by the index. 
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	index			-	Element's indexed position.
						This will remove the specified element.
	
********************************************************************************/

HRESULT		lmDisposeList(LM_CONTEXT* list);

/*******************************************************************************
Function: lmGetElementSize

	This function removes the element specified by the index. 
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	elemsize		-	The physical size of each element in the list.
	
********************************************************************************/

HRESULT		lmGetElementSize (LM_CONTEXT* list, uint32* elemsize);

/*******************************************************************************
Function: lmGetAllocatedCount

	This function returns the count of elements which are currently
	allocated.
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	allocated		-	The count of elements which are allocated.
	
********************************************************************************/

HRESULT		lmGetAllocatedCount (LM_CONTEXT* list, uint32* allocated);

/*******************************************************************************
Function: lmSetMode

	This function allows the user to switch between EXCLUSIVE and UNPROTECTED
	modes, as well as setting other modes for the list, this should be done
	immediately after the list is created. 
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	mode			-	The mode which the list will be se to.
	
********************************************************************************/
HRESULT		lmSetMode (LM_CONTEXT* list, uint32 mode);


/*******************************************************************************
Function: lmInsertElement

	This function makes it possible to insert in an acceptable order, into 
	place n of the list. If the position is already taken then all other elements 
	are moved out of the way. 
	
	ARGUMENTS:
	----------
	list			-	The list in which the element will be found.
	
	element			-	The element data to be added to the list.
	
	position		-	The position to put the element into, starts at 0. 
	
	index			-	a pointer of the physical position of the element.
	
********************************************************************************/

HRESULT				lmInsertElementCB(LM_CONTEXT* list, uint32 index, LM_ELEMENT* elem, void **data, uint32 *pPos, uint32 *pIndex);

_TC_INLINE HRESULT	lmInsertElement(LM_CONTEXT* list, void* element, uint32 pos, uint32* index) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lmInsertElement(LM_CONTEXT* list, void* element, uint32 pos, uint32* index)
								{
									return lmHandleListElementWait(list, FALSE, 0, lmInsertElementCB, &element, &pos, index);
								}


// private

HRESULT		lmHandleListElement(LM_CONTEXT* list, BOOL bIndexed, uint32 index, LM_HANDLE_INDEX_ELEMENT_CB callback, void **data, uint32 *arg1, uint32 *arg2);

HRESULT		lmCheckElementAvailable(LM_ELEMENT* elem);
HRESULT		lmCheckElementAllocated(LM_ELEMENT* elem);
HRESULT		lmCheckElementLocked(LM_CONTEXT* list, LM_ELEMENT* elem);

HRESULT		lmMoveAheadElementSize(uint32 elemsize, LM_ELEMENT **element);
HRESULT		lmMoveAheadElementSizeItems(uint32 elemsize, LM_ELEMENT **element, uint32 items);

HRESULT		lmMoveElementPositions(LM_CONTEXT* list, uint32 pos, int16 delta);	
int32		lmGetHighestElementPosition(LM_CONTEXT* list);

#ifdef _CLI
typedef void (* LM_PRINT_LIST_ELEM_CB)(void *data);

HRESULT		lmPrintList(LM_CONTEXT* list, LM_PRINT_LIST_ELEM_CB callback);
#endif //_CLI

#endif //_LISTMANAGER_H

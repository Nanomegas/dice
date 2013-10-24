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

	Functions for manufacturing, and manipulating a "Segmented Buffer Manager".

 DESCRIPTION: 
	segmented buffers are buffers which occupy a logical space of X,
	but the data it contains is spread out to many different locations,
	and are not really contiguous.   However when using the sbm, you are 
    able to access an sbm buffer, as if it were one entirely contiguos 
	buffer.

	Currently this only supports readonly buffers.

  Revisions:
		created 06/05/2003 KBJ/LM

****************************************************************************/
#ifndef _SBM_H
#define _SBM_H
#ifdef _EDS

#include "TCTypes.h"
#include "datastream.h"

/*
	SegmentedBuffer structures
*/
typedef  	struct
{
	uint32	size;
	void*	data;		// When a segment is less than or equal to 4 bytes in length, then we
						// will store that data immediately in the segmented buffer
} SBM_BUFFER;

HRESULT	sbmCreateNewBuffer(SBM_BUFFER*	list, uint32	count);

HRESULT	sbmAddElementToBuffer(SBM_BUFFER* list, uint32 size, uint32 buffer);

HRESULT	sbmReadFromBuffer(SBM_BUFFER* list, uint32 start, uint32* count, void* buffer);

HRESULT	sbmWriteToBuffer(SBM_BUFFER* list, uint32 start, uint32* count, void* buffer);

uint32 sbmGetLength(SBM_BUFFER* list);

#endif //_EDS
#endif //_SBM_H

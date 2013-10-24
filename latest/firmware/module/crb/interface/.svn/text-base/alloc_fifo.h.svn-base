//===========================================================================
//
// This confidential and proprietary software may be used only as authorized
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2008 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	 Fifo of 32-bit pointers
	
	Description: Used with CRB to track element and leaf-data allocations
  
	Revisions:
		created 05/10/2008 brian

****************************************************************************/
#ifndef _ALLOC_FIFO_H
#define _ALLOC_FIFO_H

#include "TCTypes.h"

#define ALLOC_FIFOSIZE	256

typedef void	alloc_fifo_init_fn(void);
typedef uint32	alloc_fifo_push_fn(void *);
typedef void*	alloc_fifo_pop_fn(void);

typedef struct 
{
	uint32*				fifo[ALLOC_FIFOSIZE+1];
	uint32				num_items;
	uint32				first;
	uint32				last;
	alloc_fifo_init_fn*	init;
	alloc_fifo_push_fn*	push;
	alloc_fifo_pop_fn*	pop;
} alloc_fifo_t;

extern alloc_fifo_t alloc_fifo;

#endif // _ALLOC_FIFO_H

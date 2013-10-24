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

#include "alloc_fifo.h"

static void init(void);
static uint32 push(void *);
static void * pop(void);

alloc_fifo_t alloc_fifo = { { 0 }, 0, 0, 0, init, push, pop };

static void init(void)
{
	alloc_fifo.first = 0;
	alloc_fifo.last = ALLOC_FIFOSIZE-1;
	alloc_fifo.num_items = 0;
}

uint32 push(void * ptr)
{
    if (alloc_fifo.num_items >= ALLOC_FIFOSIZE)
	{
		return (1); // error, full
	}
	else
	{
        alloc_fifo.last = (alloc_fifo.last+1) % ALLOC_FIFOSIZE;
        alloc_fifo.fifo[alloc_fifo.last] = ptr;    
        alloc_fifo.num_items = alloc_fifo.num_items + 1;
		return (0);
    }
}

void * pop (void)
{
    void * ret;

    if (alloc_fifo.num_items <= 0)
	{
		return (0); // empty
	}
    else 
	{
        ret = alloc_fifo.fifo[alloc_fifo.first];
        alloc_fifo.first = (alloc_fifo.first+1) % ALLOC_FIFOSIZE;
        alloc_fifo.num_items = alloc_fifo.num_items - 1;
    }

    return(ret);
}

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
	
	Purpose:	 Fifo of CRB element pointers
	
	Description: Used with CRB for ROM element tree traversal
  
	Revisions:
		created 05/10/2008 brian

****************************************************************************/

#include "crb.h"
#include "traverse_fifo.h"

static void init(void);
static uint32 push(CRBHANDLE);
static CRBHANDLE pop(void);

traverse_fifo_t traverse_fifo = { { 0 }, 0, 0, 0, init, push, pop };

static void init(void)
{
	traverse_fifo.first = 0;
	traverse_fifo.last = TRAVERSE_FIFOSIZE-1;
	traverse_fifo.num_items = 0;
}

uint32 push(CRBHANDLE ptr)
{
    if (traverse_fifo.num_items >= TRAVERSE_FIFOSIZE)
	{
		return (1); // error, full
	}
	else
	{
        traverse_fifo.last = (traverse_fifo.last+1) % TRAVERSE_FIFOSIZE;
        traverse_fifo.fifo[traverse_fifo.last] = ptr;    
        traverse_fifo.num_items = traverse_fifo.num_items + 1;
		return (0);
    }
}

CRBHANDLE pop (void)
{
    CRBHANDLE ret;

    if (traverse_fifo.num_items <= 0)
	{
		return (NULL); // empty
	}
    else 
	{
        ret = traverse_fifo.fifo[traverse_fifo.first];
        traverse_fifo.first = (traverse_fifo.first+1) % TRAVERSE_FIFOSIZE;
        traverse_fifo.num_items = traverse_fifo.num_items - 1;
    }

    return(ret);
}

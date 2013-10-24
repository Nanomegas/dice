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
	
	Description: Used with CRB for ROM element tree traversal. Directory 
	elements stored in this fifo will be written immediately after the root
	directory in ROM. Intended for use with Unit directories.
  
	Revisions:
		created 05/10/2008 brian

****************************************************************************/

#include "crb.h"
#include "unit_fifo.h"

static void init(void);
static uint32 push(CRBHANDLE);
static CRBHANDLE pop(void);

unit_fifo_t unit_fifo = { { 0 }, 0, 0, 0, init, push, pop };

static void init(void)
{
	unit_fifo.first = 0;
	unit_fifo.last = UNIT_FIFOSIZE-1;
	unit_fifo.num_items = 0;
}

uint32 push(CRBHANDLE ptr)
{
    if (unit_fifo.num_items >= UNIT_FIFOSIZE)
	{
		return (1); // error, full
	}
	else
	{
        unit_fifo.last = (unit_fifo.last+1) % UNIT_FIFOSIZE;
        unit_fifo.fifo[unit_fifo.last] = ptr;    
        unit_fifo.num_items = unit_fifo.num_items + 1;
		return (0);
    }
}

CRBHANDLE pop (void)
{
    CRBHANDLE ret;

    if (unit_fifo.num_items <= 0)
	{
		return (NULL); // empty
	}
    else 
	{
        ret = unit_fifo.fifo[unit_fifo.first];
        unit_fifo.first = (unit_fifo.first+1) % UNIT_FIFOSIZE;
        unit_fifo.num_items = unit_fifo.num_items - 1;
    }

    return(ret);
}

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
	
	Description: Used with CRB for crc fixups
  
	Revisions:
		created 05/10/2008 brian

****************************************************************************/

#include "crc_fifo.h"

static void init(void);
static uint32 push(uint32 *);
static uint32 * pop(void);

crc_fifo_t crc_fifo = { { 0 }, 0, 0, 0, init, push, pop };

static void init(void)
{
	crc_fifo.first = 0;
	crc_fifo.last = CRC_FIFOSIZE-1;
	crc_fifo.num_items = 0;
}

uint32 push(uint32 * ptr)
{
    if (crc_fifo.num_items >= CRC_FIFOSIZE)
	{
		return (1); // error, full
	}
	else
	{
        crc_fifo.last = (crc_fifo.last+1) % CRC_FIFOSIZE;
        crc_fifo.fifo[crc_fifo.last] = ptr;    
        crc_fifo.num_items = crc_fifo.num_items + 1;
		return (0);
    }
}

uint32 * pop (void)
{
    uint32 * ret;

    if (crc_fifo.num_items <= 0)
	{
		return (0); // empty
	}
    else 
	{
        ret = crc_fifo.fifo[crc_fifo.first];
        crc_fifo.first = (crc_fifo.first+1) % CRC_FIFOSIZE;
        crc_fifo.num_items = crc_fifo.num_items - 1;
    }

    return(ret);
}

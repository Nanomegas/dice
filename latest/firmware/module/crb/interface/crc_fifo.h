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
#ifndef _CRC_FIFO_H
#define _CRC_FIFO_H

#include "TCTypes.h"

#define CRC_FIFOSIZE	256

typedef void	crc_fifo_init_fn(void);
typedef uint32	crc_fifo_push_fn(uint32 *);
typedef uint32*	crc_fifo_pop_fn(void);

typedef struct 
{
	uint32*				fifo[CRC_FIFOSIZE+1];
	uint32				num_items;
	uint32				first;
	uint32				last;
	crc_fifo_init_fn*	init;
	crc_fifo_push_fn*	push;
	crc_fifo_pop_fn*	pop;
} crc_fifo_t;

extern crc_fifo_t crc_fifo;

#endif // _CRC_FIFO_H

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
#ifndef _UNIT_FIFO_H
#define _UNIT_FIFO_H

#include "crb.h"
#include "TCTypes.h"

#define UNIT_FIFOSIZE	100

typedef void		unit_fifo_init_fn(void);
typedef uint32		unit_fifo_push_fn(CRBHANDLE);
typedef CRBHANDLE	unit_fifo_pop_fn(void);

typedef struct 
{
	CRBHANDLE			fifo[UNIT_FIFOSIZE+1];
	uint32				num_items;
	uint32				first;
	uint32				last;
	unit_fifo_init_fn*	init;
	unit_fifo_push_fn*	push;
	unit_fifo_pop_fn*	pop;
} unit_fifo_t;

extern unit_fifo_t unit_fifo;

#endif // _UNIT_FIFO_H

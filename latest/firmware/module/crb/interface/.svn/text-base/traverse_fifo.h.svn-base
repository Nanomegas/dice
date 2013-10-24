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
#ifndef _TRAVERSE_FIFO_H
#define _TRAVERSE_FIFO_H

#include "crb.h"
#include "TCTypes.h"

#define TRAVERSE_FIFOSIZE	256

typedef void		traverse_fifo_init_fn(void);
typedef uint32		traverse_fifo_push_fn(CRBHANDLE);
typedef CRBHANDLE	traverse_fifo_pop_fn(void);

typedef struct 
{
	CRBHANDLE				fifo[TRAVERSE_FIFOSIZE+1];
	uint32					num_items;
	uint32					first;
	uint32					last;
	traverse_fifo_init_fn*	init;
	traverse_fifo_push_fn*	push;
	traverse_fifo_pop_fn*	pop;
} traverse_fifo_t;

extern traverse_fifo_t traverse_fifo;

#endif // _TRAVERSE_FIFO_H

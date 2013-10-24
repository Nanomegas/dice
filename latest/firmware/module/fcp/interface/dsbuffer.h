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
  DESCRIPTION:  
 
		Functions for creating a DATABUFFER which allows for reading
		of individual bytes from an array of QUADLET info.   
		This simplifies such logic for the DataStream class.

		This should be used internally to the DataStream only.  There are
		no errors returned for the boundary conditions of end of data buffer.

  Revisions:
		created 20/03/2003 KBJ/LM

*****************************************************************************/
#ifndef _DSBUFFER_H
#define _DSBUFFER_H

#include "TCTypes.h"

typedef struct	
{
	QUADLET*   quadbase;
	int32	   curbyte;
	uint32	   curquad;
	uint32	   mode;
} DS_BUFFER;

void	dsbCreate(DS_BUFFER* buffer, void* base, uint32 mode);

BYTE	dsbGetByte(DS_BUFFER* buffer);
void	dsbPutByte(DS_BUFFER* buffer, BYTE byte); 

void	dsbSkipAhead(DS_BUFFER* buffer, int32 offset);
void	dsbSkipBack(DS_BUFFER* buffer, int32 offset);

void	dsbReset(DS_BUFFER* buffer);

#endif // _DSBUFFER_H


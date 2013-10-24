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
/*********************************************************************
 * DESCRIPTION:

		Functions for creating a DATABUFFER which allows for reading
		of individual bytes from an array of QUADLET info.   This simplifies
		such logic for the DataStream class.

		This should be used internally to the DataStream only.  There are
		no errors returned for the boundary conditions of end of data buffer.

	Revisions:
		created 20/03/2003 KBJ/LM

***********************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "datastream.h"
#include "dsbuffer.h"

void dsbCreate(DS_BUFFER* buffer, void* base, uint32 mode)
{
	memset(buffer,0,sizeof(DS_BUFFER)); 
	buffer->mode = mode;
	buffer->quadbase = (QUADLET *) base;
}

BYTE dsbGetByte(DS_BUFFER* buffer)
{
	uint32		result = 0;
	uint32		bytemask = 0;
	char		*quad = NULL;

	switch (buffer->mode)
	{
		case dsMODE_NATIVE:
			bytemask = (uint32) (0xFF << ((3 - (buffer->curbyte)) * 8));
			result = buffer->quadbase[buffer->curquad];
			result &= bytemask;
			result >>= ((3 - (buffer->curbyte)) * 8);
			break;

		case dsMODE_BIGENDIAN:
			quad = (char *) &(buffer->quadbase[buffer->curquad]);
			result = quad[buffer->curbyte];
			break;

		case dsMODE_LITTLEENDIAN:
			quad = (char *) &(buffer->quadbase[buffer->curquad]);
			result = quad[3 - (buffer->curbyte)];
			break;
	}	

	buffer->curbyte++; 
	
	if (buffer->curbyte == 4)
	{
		buffer->curbyte = 0;
		buffer->curquad++;
	}
	
	return (BYTE) result;
}

void dsbPutByte(DS_BUFFER* buffer, BYTE byte)
{
	uint32		bytemask = 0;
	uint32		clearmask = 0;
	QUADLET		quaddata = 0;
	BYTE		*quad = NULL;

	switch (buffer->mode)
	{
		case dsMODE_NATIVE:
			bytemask = (uint32) (0xFF << ((3 - (buffer->curbyte)) * 8));
			quaddata = (uint32) (byte << ((3 - (buffer->curbyte)) * 8));
															// might intend to write over a buffer therefore perform some mask operations to clear the current buffer.
			clearmask = bytemask ^ 0xFFFFFFFF;				// Create the inverse mask
			buffer->quadbase[buffer->curquad] &= clearmask;	// Copy all the other bytes in this quad.
			buffer->quadbase[buffer->curquad] |= quaddata;	// OR the data into the appropriate place
			break;

		case dsMODE_BIGENDIAN:
			quad = (BYTE *) &(buffer->quadbase[buffer->curquad]);
			quad[buffer->curbyte] = byte;
			break;

		case dsMODE_LITTLEENDIAN:
			quad = (BYTE *) &(buffer->quadbase[buffer->curquad]);
			quad[3 - (buffer->curbyte)] = byte;
			break;
	}	

	buffer->curbyte++; 

	if (buffer->curbyte == 4)
	{
		buffer->curbyte = 0;
		buffer->curquad++;
	}
}

void dsbSkipAhead(DS_BUFFER* buffer, int32 offset)
{
	uint32	quads = 0;
	uint32	bytes = 0;
	
	quads = (uint32) (offset / 4);
	bytes = offset - (quads * 4);
	buffer->curquad += quads;

	if (bytes + buffer->curbyte > 3)
	{
		buffer->curbyte = (int32) (((bytes + buffer->curbyte) - 3) - 1);
		buffer->curquad++;
	}
	else
	{
		buffer->curbyte += bytes;
	}
}

void dsbSkipBack(DS_BUFFER*	buffer, int32	offset)
{
	uint32	quads = 0;
	uint32	bytes = 0;
	
	quads = (uint32) (offset / 4);
	bytes = offset - (quads * 4);
	buffer->curquad -= quads;

	if ((int32) (buffer->curbyte - bytes) < 0)
	{
		buffer->curbyte = (int32) (4 + (buffer->curbyte - bytes));
		buffer->curquad--;
	}
	else
	{
		buffer->curbyte -= bytes;
	}
}

void dsbReset(DS_BUFFER* buffer)
{
	buffer->curbyte = 0;
	buffer->curquad = 0;
}

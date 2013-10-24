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
 FILE PURPOSE: 

	Functions for manufacturing, and manipulating a "Segmented Buffer Manager".

 DESCRIPTION: 
	segmented buffers are buffers which occupy a logical space of X,
	but the data it contains is spread out to many different locations,
	and are not really contiguous.   However when using the sbm, you are 
    able to access an sbm buffer, as if it were one entirely contiguos 
	buffer.

	Currently this only supports readonly buffers.

 Revisions:
		created 06/05/2003 KBJ/LM

****************************************************************************/

#ifdef _EDS

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "sbm.h"
#include "datastream.h"


HRESULT	sbmCreateNewBuffer(SBM_BUFFER*	list, uint32	count)
{
	memset(list,0,sizeof(SBM_BUFFER)*count);
	
	list[count].size=0xFFFFFFFF;
	
	return NO_ERROR;	
}


HRESULT	sbmReadFromBuffer(SBM_BUFFER* list, uint32 start, uint32* count, void* buffer)
{
	HRESULT		hResult=NO_ERROR;
	DataStream	ds;
	uint32		counter=0;
	SBM_BUFFER*	cur=list;
	DataStream	curds;
	
	hResult=dsOpenStream(&ds,buffer,*count,dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
	
	if (NO_ERROR==hResult)
	{
		while (cur->size!=0)
		{
			//
			//	First check to see if we are looking at an object late enough in the stream.
			//
			if (counter + cur->size < start)
			{
				counter=counter+cur->size;
				cur++;
				continue;	
			}
				
			// Open the local data stream.
			if (cur->size <=4)
			{
				dsOpenStream(&curds,&(cur->data),cur->size,dsMODE_READ | dsMODE_PLATFORMENDIAN);	
			}
			else
			{
				dsOpenStream(&curds,cur->data,cur->size,dsMODE_READ | dsMODE_PLATFORMENDIAN); 
			}
			
			if (counter < start)
			{
				dsFastForward(&curds,start-counter);
			}
			
			
			counter+=cur->size;
			/*
				Copy data 1 byte at a time until either we reach the end
				of the buffer, or total length of the outgoing buffer.
			*/
			DO_FOREVER
			{
				HRESULT		writeresult=NO_ERROR;
				HRESULT		readresult=NO_ERROR;
				BYTE		tempbyte=0;
				
				readresult=dsRead8Bits(&curds,&tempbyte);
				if (readresult != NO_ERROR)
				{
					break;	
				}
				
				if (ds.cur_offset * 8 + ds.bits + 8 > ds.total_size * 8)
				{
					writeresult = E_AVC_DS_NO_ROOM_TO_GROW;
				}
				else
				{
					writeresult=dsWrite8Bits(&ds,tempbyte);
				}
				if (writeresult != NO_ERROR)
				{
					*count=dsGetPosition(&ds);
					return NO_ERROR;	
				}
				counter++;
			}
			
			cur++;
		}
	}
	return hResult;	
}

HRESULT	sbmWriteToBuffer(SBM_BUFFER* list, uint32 start, uint32* count, void* buffer)
{
	HRESULT		hResult=NO_ERROR;
	DataStream	ds;
	uint32		counter=0;
	SBM_BUFFER*	cur=list;
	DataStream	curds;
	
	
	hResult=dsOpenStream(&ds,buffer,*count,dsMODE_READ | dsMODE_PLATFORMENDIAN);
	
	if (NO_ERROR==hResult)
	{
		while (cur->size!=0)
		{
			//
			//	First check to see if we are looking at an object late enough in the stream.
			//
			if (counter + cur->size < start)
			{
				counter=counter+cur->size;
				cur++;
				continue;	
			}
				
			// Open the local data stream.
			if (cur->size <=4)
			{
				dsOpenStream(&curds,&(cur->data),cur->size,dsMODE_WRITE | dsMODE_PLATFORMENDIAN);	
			}
			else
			{
				dsOpenStream(&curds,cur->data,cur->size,dsMODE_WRITE | dsMODE_PLATFORMENDIAN); 
			}
			
			if (counter < start)
			{
				dsFastForward(&curds,start-counter);
			}
			
			
			counter+=cur->size;
			/*
				Copy data 1 byte at a time until either we reach the end
				of the buffer, or total length of the outgoing buffer.
			*/
			DO_FOREVER
			{
				HRESULT		writeresult=NO_ERROR;
				HRESULT		readresult=NO_ERROR;
				BYTE		tempbyte=0;
				
				readresult=dsRead8Bits(&ds,&tempbyte);
				if (readresult != NO_ERROR)
				{
					*count=dsGetPosition(&ds);
					return NO_ERROR;	
				}
				
				if (ds.cur_offset * 8 + ds.bits + 8 > ds.total_size * 8)
				{
					writeresult = E_AVC_DS_NO_ROOM_TO_GROW;
				}
				else
				{
					writeresult=dsWrite8Bits(&curds,tempbyte);
				}
				if (writeresult != NO_ERROR)
				{
					dsRewind(&ds,1); // Do this because if we found the end of descriptor
									 // we now need to pop back one byte in the input data.
					break;	
				}
				counter++;
			}
			
			cur++;
		}
	}
	return hResult;	
}


HRESULT	sbmAddElementToBuffer(SBM_BUFFER* list, uint32 size, uint32 buffer)
{
	SBM_BUFFER*		cur=list;

	while (cur->size > 0 && cur->size < 0xFFFFFFFF)
	{
		cur=cur+1;	
	}
	
	if (cur->size == 0)
	{
		cur->size=size;

		if (size <=4)
		{
			DataStream 	ds;
			dsOpenStream(&ds,&(cur->data),4,dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
			dsWriteBits(&ds,size*8,buffer);	
		}
		else
		{
			cur->data=(void *)buffer;
		}
		return NO_ERROR;	
	}
	return E_SBM_OUT_OF_SPACE;
}


uint32	sbmGetLength(SBM_BUFFER* list)
{
	SBM_BUFFER*		cur=list;
	uint32			size=0;
	
	while (cur->size > 0 && cur->size < 0xFFFFFFFF)
	{
		size+=cur->size;	
		cur++;
	}
	
	return size;
		
}

#endif //_EDS

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
 
		Common functions for parsing and building the AVC packets.
		Given a chunk of memory this will allow the caller to parse
		a complex AVC payload.  
		
		All incoming data is expected to be in QUADLETS (in the LOCAL 
		byte order,) and will be dealt with in that matter.  If your
		data does not match this form, then you are responsible for 
		manipulating the data yourself.

  Revisions:
		created 20/03/2003 KBJ/LM

*****************************************************************************/
static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "keyvalue.h"
#include "dsbuffer.h"
#include "datastream.h"

/*	DataStream Creation Functions */
HRESULT	dsOpenStream(pDataStream pStream, void* data, uint32 size, uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
#if 0 //LM??? always FALSE
	if ((mode & dsRW_MASK) != dsMODE_WRITE && 
		(mode & dsRW_MASK) != dsMODE_READ)
	{
		hResult = E_AVC_DS_BAD_MODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#endif //LM??? always FALSE
		
	// Fill in the block.
	memset(pStream, 0, sizeof(DataStream));
	dsbCreate(&(pStream->dsbBuffer), data, mode & dsENDIAN_MASK);
	pStream->mode = mode;
	pStream->total_size = size;
	
	// Setup the empty marker field
	kvCreate(pStream->markers, KV_LISTSIZE(3));
	
	return hResult;
}

HRESULT	dsCloseStream(pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	memset(pStream, 0xFE, sizeof(DataStream));
		
	return hResult;
}

HRESULT	dsReadBits(pDataStream pStream, uint32 len, uint32 *data)
{
	HRESULT		hResult = NO_ERROR;
	BYTE		c = 0;
	uint32		mask = 0;
	uint32		n = 0;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (data == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (((pStream->mode) & dsRW_MASK) != dsMODE_READ)
	{
		hResult = E_AVC_DS_BAD_MODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (len == 0) return hResult;
	
	if (len > 32)
	{
		hResult = E_AVC_DS_TOO_MUCH;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
#if 0 //def _SYSDEBUG
	if (dataStreamLogging)
	{
		sysDebugPrintf("dsReadBits: len:0x%08x cur_offset:0x%08x\n\r", len, pStream->cur_offset);
		sysDebugPrintf("dsbGetByte bitbuf cur_offset bits\n\r");
	}
#endif //_SYSDEBUG

	// Make sure that we have enough bytes of data.
	while (pStream->bits < len)
	{
		c = dsbGetByte(&(pStream->dsbBuffer));
		if (pStream->cur_offset == pStream->total_size)
		{
			hResult = E_AVC_DS_EOF;
			//KBJ??? sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		pStream->cur_offset++;
		pStream->bitbuf = ((pStream->bitbuf) << 8) + c;
		pStream->bits = pStream->bits + 8;

#if 0 //def _SYSDEBUG
		if (dataStreamLogging)
		{
			sysDebugPrintf("0x%08x 0x%08x 0x%08x 0x%08x\n\r", c, pStream->bitbuf, pStream->cur_offset, pStream->bits);
		}
#endif //_SYSDEBUG
	}
	
	mask = (uint32) (1L << len) - 1;
	if (mask == 0L && len == 32)
	{
		mask = 0xFFFFFFFF;
	}
#if 0 //def _SYSDEBUG
	if (dataStreamLogging)
	{
		sysDebugPrintf("mask mask' data cur_offset bitbuf bits\n\r");
		sysDebugPrintf("0x%08x ", mask);
	}
#endif //_SYSDEBUG
	n = (pStream->bitbuf >> (pStream->bits - len)) & mask;
	pStream->bits = pStream->bits - len;
	mask = (uint32) (1L << pStream->bits) - 1;
	pStream->bitbuf = pStream->bitbuf & mask;
	
#if 0 //def _SYSDEBUG
	if (dataStreamLogging)
	{
		sysDebugPrintf("0x%08x 0x%08x 0x%08x 0x%08x\n\r", mask, n, pStream->cur_offset, pStream->bitbuf, pStream->bits);
	}
#endif //_SYSDEBUG

	*data = n;
	
	return hResult;
}

HRESULT	dsRead8Bits(pDataStream pStream, BYTE* data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		tempdata = 0L;

	hResult = dsReadBits(pStream, 8, &tempdata);
	*data = (BYTE) tempdata;
	
	return hResult;
}

HRESULT	dsRead16Bits(pDataStream pStream, uint16* data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		tempdata = 0L;

	hResult = dsReadBits(pStream, 16, &tempdata);
	*data = (uint16) tempdata;
	
	return hResult;
}

HRESULT	dsRead24Bits(pDataStream pStream, uint32* data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		tempdata = 0L;

	hResult = dsReadBits(pStream, 24, &tempdata);
	*data = tempdata;
	
	return hResult;
}

HRESULT	dsRead32Bits(pDataStream pStream, uint32* data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		tempdata = 0L;

	hResult = dsReadBits(pStream, 32, &tempdata);
	*data = tempdata;
	
	return hResult;
}

HRESULT dsWriteBits(pDataStream pStream, uint32 len, uint32 data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		usemask = 0;
	uint32		savemask = 0;
	uint32		leftover = 0;
	uint32		usedata = 0; 
	uint32		savedata = 0;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (((pStream->mode) & dsRW_MASK) != dsMODE_WRITE)
	{
		hResult = E_AVC_DS_BAD_MODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (len == 0) return hResult;

	if (len > 32)
	{
		hResult = E_AVC_DS_TOO_MUCH;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}	

	if (pStream->cur_offset * 8 + pStream->bits + len > pStream->total_size * 8)
	{
		hResult = E_AVC_DS_NO_ROOM_TO_GROW;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#if 0 //def _SYSDEBUG
	if (dataStreamLogging)
	{
		sysDebugPrintf("dsWriteBits: len:0x%08x data:0x%08x cur_offset:0x%08x\n\r", len, data, pStream->cur_offset);
		sysDebugPrintf("leftover savemask savedata usedata bitbuf' cur_offset len\n\r");
	}
#endif //_SYSDEBUG

	while (len + pStream->bits >= 8)
	{
		leftover = (len + pStream->bits) - 8;
		savemask = (uint32) (1L << leftover) - 1;
		savedata = data & savemask;
		usedata = (data) >> (leftover);

		pStream->bitbuf |= usedata;

#if 0 //def _SYSDEBUG
		if (dataStreamLogging)
		{
			sysDebugPrintf("0x%08x 0x%08x 0x%08x 0x%08x 0x%08x ", leftover, savemask, savedata, usedata, pStream->bitbuf);
		}
#endif //_SYSDEBUG

		dsbPutByte(&(pStream->dsbBuffer), (BYTE) pStream->bitbuf);
		pStream->bitbuf = 0;
		pStream->cur_offset++;
		len -= 8 - pStream->bits;
		pStream->bits = 0;
		
#if 0 //def _SYSDEBUG
		if (dataStreamLogging)
		{
			sysDebugPrintf("0x%08x 0x%08x\n\r", pStream->cur_offset, len);
		}
#endif //_SYSDEBUG

		// Keep the number of bits which are left over, and toss away the rest.
		data = savedata;
	}
	
	if (len > 0) // len: ]0;8[
	{
		// Copy the bits out of the original data first
		usemask = (uint32) (1L << len) - 1;
		usedata = ((data & usemask)) << (8 - (pStream->bits + len));
		pStream->bitbuf |= usedata;
		pStream->bits += len;

#if 0 //def _SYSDEBUG
	if (dataStreamLogging)
	{
		sysDebugPrintf("len: ]0;8[\n\r");
		sysDebugPrintf("usemask usedata bitbuf' bits\n\r");
		sysDebugPrintf("0x%08x 0x%08x 0x%08x 0x%08x\n\r", usemask, usedata, pStream->bitbuf, pStream->bits);
	}
#endif //_SYSDEBUG
	}

	return hResult;
}

HRESULT dsAddStream(pDataStream	pDstStream, pDataStream pSrcStream)
{
	HRESULT		hResult = NO_ERROR;
	DataStream  ds;
	BYTE		temp = 0;

	if (pDstStream == NULL ||
		pSrcStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (pDstStream->bits > 0 ||
		pSrcStream->bits > 0)
	{
		hResult = E_AVC_DS_NOT_ON_BYTE_BOUNDARY;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Check enough room available in the dst pStream for copy src pStream
	if (pDstStream->total_size - pDstStream->cur_offset < pSrcStream->total_size)
	{
		hResult = E_AVC_DS_EOF;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	memcpy(&ds, pSrcStream, sizeof(DataStream));
	
	hResult = dsReOpenStream(&ds, dsMODE_READ);
	if (hResult != NO_ERROR) return hResult;
	
	DO_FOREVER
	{
		hResult = dsRead8Bits(&ds, &temp);
		if (hResult != NO_ERROR) break;

		hResult = dsWrite8Bits(pDstStream, temp);
		if (hResult != NO_ERROR) break;
	}

	// an EOF here is not invalid.
	if (hResult == E_AVC_DS_EOF) hResult = NO_ERROR;

	return hResult;
}

uint32 dsGetPosition(pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	uint32		pos = 0;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return 0;
	}

	pos = pStream->cur_offset;
	
	return pos;
}

void dsSetPosition(pDataStream pStream, uint32 pos)
{
	HRESULT		hResult = NO_ERROR;

	hResult = dsReset(pStream);
	if (hResult != NO_ERROR) return;

	dsFastForward(pStream, pos);
}

void dsTrimSize(pDataStream pStream, uint32 newsize)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return;
	}

	pStream->total_size = newsize;
}

HRESULT	dsFastForward(pDataStream pStream, uint32 count)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (!count)
	{
		return hResult;
	}
	
	if (pStream->bits > 0)
	{
		hResult = E_AVC_DS_NOT_ON_BYTE_BOUNDARY;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (pStream->cur_offset + count > pStream->total_size)
	{
		pStream->cur_offset = pStream->total_size;
		hResult = E_AVC_DS_EOF;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pStream->cur_offset += count;
	dsbSkipAhead(&(pStream->dsbBuffer), (int32) count);
	
	return hResult;
}

HRESULT	dsRewind(pDataStream pStream, uint32 count)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (!count)
	{
		return hResult;
	}
	
	if (pStream->bits > 0)
	{
		hResult = E_AVC_DS_NOT_ON_BYTE_BOUNDARY;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (pStream->cur_offset < count)
	{
		pStream->cur_offset = 0;
		hResult = E_AVC_DS_BOF;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pStream->cur_offset -= count;
	dsbSkipBack(&(pStream->dsbBuffer), (int32) count);

	return hResult;
}

HRESULT	dsReset(pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pStream->bits = 0;
	pStream->cur_offset = 0;
	pStream->bitbuf = 0;
	dsbReset(&(pStream->dsbBuffer));
	
	return hResult;
}

HRESULT	dsReOpenStream(pDataStream pStream, uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#if 0 //LM??? always FALSE
	if ((mode & dsRW_MASK) != dsMODE_WRITE && 
		(mode & dsRW_MASK) != dsMODE_READ)
	{	
		hResult = E_AVC_DS_BAD_MODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#endif //LM??? always FALSE

	hResult = dsReset(pStream);
	if (hResult != NO_ERROR) return hResult;
	
	pStream->mode &= 0xFFFFFFF0;
	pStream->mode |= mode;
	
	return hResult;
}

// dsSwitchMode, switches the mode of a pStream from read to write or otherwise.
HRESULT	dsSwitchMode(pDataStream pStream, uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#if 0 //LM??? always FALSE
	if ((mode & dsRW_MASK) != dsMODE_WRITE && 
		(mode & dsRW_MASK) != dsMODE_READ)
	{
		hResult = E_AVC_DS_BAD_MODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#endif //LM??? always FALSE

	if (pStream->bits)
	{
		hResult = E_AVC_DS_NOT_ON_BYTE_BOUNDARY;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pStream->mode &= 0xFFFFFFF0;
	pStream->mode |= mode;

	return hResult;
}

// dsSetMarker, remembers a particular point in the datastream which is marked using a uint32 	
// Up to 3 markers may be used in any datastream
HRESULT	dsSetMarker(pDataStream	pStream, uint32 markerid)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (pStream->bits)
	{
		hResult = E_AVC_DS_NOT_ON_BYTE_BOUNDARY;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return kvSetValue(pStream->markers, markerid, pStream->cur_offset);	
}

// dsKillMarker removes a marker which was already remembered in the datastream
HRESULT	dsKillMarker(pDataStream pStream, uint32 markerid)
{
	HRESULT		hResult = NO_ERROR;

	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	kvFreeKey(pStream->markers, markerid);
	
	return hResult;	
}

// dsGotoMarker goes to a particular marker set in this pStream.
HRESULT	dsGotoMarker(pDataStream pStream, uint32 markerid)
{
	HRESULT		hResult = NO_ERROR;
	uint32		newpos = 0;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = kvIsKey(pStream->markers, markerid);
	if (hResult != NO_ERROR) return hResult;

	hResult = kvGetValue(pStream->markers, markerid, &newpos);
	if (hResult != NO_ERROR) return hResult;

	dsSetPosition(pStream, newpos);

	return hResult;
}

// dsReadChunk allows for large chunks of BYTES to be read into the waiting buffer (no change to the byte order)
HRESULT	dsReadChunk(pDataStream	pStream, uint32 size, char* buffer)
{
	HRESULT		hResult = NO_ERROR;
	DataStream	ds;
	BYTE		temp = 0;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = dsOpenStream(&ds, buffer, size, dsMODE_WRITE);
	if (hResult != NO_ERROR) return hResult;
	
	while (size--) //DO_FOREVER
	{
		hResult = dsRead8Bits(pStream, &temp);
		if (hResult != NO_ERROR) break;

		hResult = dsWrite8Bits(&ds, temp);
		if (hResult != NO_ERROR) break;
	}
	
	// an EOF here is not invalid.
	if (hResult == E_AVC_DS_EOF) hResult = NO_ERROR;

	return hResult;		
}

// dsWriteChunk allows for large chunks of BYTES to be written from the waiting buffer (no change to the byte order)
HRESULT	dsWriteChunk(pDataStream pStream, uint32 size, char* buffer)
{
	HRESULT		hResult = NO_ERROR;
	DataStream	ds;
	BYTE		temp = 0;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = dsOpenStream(&ds, buffer, size, dsMODE_READ);
	if (hResult != NO_ERROR) return hResult;
	
	while (size--) //DO_FOREVER
	{
		hResult = dsRead8Bits(&ds, &temp);
		if (hResult != NO_ERROR) break;

		hResult = dsWrite8Bits(pStream, temp);
		if (hResult != NO_ERROR) break;
	}
	
	// an EOF here is not invalid.
	if (hResult == E_AVC_DS_EOF) hResult = NO_ERROR;

	return hResult;		
}

HRESULT	dsReadBuffer(pDataStream pStream, uint32 size, void *data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	BYTE		*pData = (BYTE *) data;

	for (i = 0; i < size; i++)
	{
		hResult = dsRead8Bits(pStream, &pData[i]);
		if (hResult != NO_ERROR) return hResult;
	}
	
	return hResult;
}

HRESULT	dsWriteBuffer(pDataStream pStream, uint32 size, void *data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	BYTE		*pData = (BYTE *) data;

	for (i = 0; i < size; i++)
	{
		hResult = dsWrite8Bits(pStream, pData[i]);
		if (hResult != NO_ERROR) return hResult;
	}
	
	return hResult;
}

HRESULT	dsHandleBits(pDataStream pStream, uint32 len, uint32 *data, BOOL bWrite)
{
	if (bWrite)
	{
		return dsWriteBits(pStream, len, *data);
	}
	else
	{
		return dsReadBits(pStream, len, data);
	}
}

HRESULT	dsHandle8Bits(pDataStream pStream, BYTE *data, BOOL bWrite)
{
	if (bWrite)
	{
		return dsWrite8Bits(pStream, *data);
	}
	else
	{
		return dsRead8Bits(pStream, data);
	}
}

HRESULT	dsHandle16Bits(pDataStream pStream, uint16 *data, BOOL bWrite)
{
	if (bWrite)
	{
		return dsWrite16Bits(pStream, *data);
	}
	else
	{
		return dsRead16Bits(pStream, data);
	}
}

HRESULT	dsHandle24Bits(pDataStream pStream, uint32 *data, BOOL bWrite)
{
	if (bWrite)
	{
		return dsWrite24Bits(pStream, *data);
	}
	else
	{
		return dsRead24Bits(pStream, data);
	}
}

HRESULT	dsHandle32Bits(pDataStream pStream, uint32 *data, BOOL bWrite)
{
	if (bWrite)
	{
		return dsWrite32Bits(pStream, *data);
	}
	else
	{
		return dsRead32Bits(pStream, data);
	}
}

HRESULT	dsHandleBuffer(pDataStream pStream, uint32 size, void *data, BOOL bWrite)
{
	if (bWrite)
	{
		return dsWriteBuffer(pStream, size, data);
	}
	else
	{
		return dsReadBuffer(pStream, size, data);
	}
}

// dsCopyStream copies data and markers from one pStream to another. 
// A failure of this function, leaves pDstStream in an unknown state, and it should be either reset, or reopened, by the caller.
HRESULT	dsCopyStream(pDataStream pDstStream, pDataStream pSrcStream)
{
	HRESULT		hResult = NO_ERROR;
	uint32		maxcopysize = 0;
	
	if (pDstStream == NULL ||
		pSrcStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	maxcopysize = pDstStream->total_size;
	if (pSrcStream->total_size < maxcopysize)
	{
		maxcopysize = pSrcStream->total_size;	
	}	
	
	// Copy the physical data of the pSrcStream.
	memcpy(pDstStream->dsbBuffer.quadbase, pSrcStream->dsbBuffer.quadbase, maxcopysize);	

	// Copy markers which fit into the pDstStream. This erases all old data in the markers section of the pDstStream.
	memcpy(pDstStream->markers, pSrcStream->markers, sizeof(pDstStream->markers));
	
	dsTrimSize(pDstStream, maxcopysize);

	hResult = dsReset(pDstStream);	
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// dsPadToEndOfQuadlet allows you to write 'padbyte' character, until you reach the end of the current quadlet.  This is useful for padding fields in packets.
HRESULT	dsPadToEndOfQuadlet(pDataStream pStream, BYTE padbyte)
{
	HRESULT		hResult = NO_ERROR;
	uint32		leftcounter = 0;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (pStream->dsbBuffer.curbyte <= 0) return hResult;
	
	leftcounter = (uint32) (pStream->dsbBuffer.curbyte);
	
	if (leftcounter)
	{
		leftcounter = 4 - leftcounter;
		
		if (leftcounter > (pStream->total_size - pStream->cur_offset))
		{
			leftcounter = pStream->total_size - pStream->cur_offset;
		}
		
		while (leftcounter--)
		{
			hResult = dsWrite8Bits(pStream, padbyte);
			if (hResult != NO_ERROR) return hResult;
		}
	}
	
	return hResult;
}

HRESULT	dsPadToEndOfBuffer(pDataStream pStream, BYTE padbyte)
{
	HRESULT		hResult = NO_ERROR;
	uint32		leftcounter = 0;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	leftcounter = pStream->total_size - pStream->cur_offset;
	
	while (leftcounter--)
	{
		hResult = dsWrite8Bits(pStream, padbyte);
		if (hResult != NO_ERROR) return hResult;
	}
	
	return hResult;
}

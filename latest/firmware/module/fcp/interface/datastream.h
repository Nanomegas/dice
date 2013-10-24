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
		
		The "MODE" in which any stream operates, is entirely meant for the
		protection of data.  Basically the idea is, that you don't want to
		be reading data which isn't valid, and you don't want to be writing 
		over data which is already valid.
		
		The DATASTREAM now uses the dsbBuffer to read and write bytes in
		a QUADLET form.  This means that in order for data to be written into
		the buffer, you must COMPLETE ENTIRE BYTE OF DATA.  If you do not 
		then the data is not written.  

  Revisions:
		created 20/03/2003 KBJ/LM

*****************************************************************************/

/** @addtogroup DataStreams 


		Utility functions for parsing tightly bit-packed packets, such as
		AV/C. Given a chunk of memory this will allow the caller to parse
		a complex payload.

		This module can be used from the command prompt by the @ref cli_ds commands.
		
		The "MODE" in which any stream operates, is entirely meant for the
		protection of data.  Basically the idea is, that you don't want to
		be reading data which isn't valid, and you don't want to be writing 
		over data which is already valid.
		
		The DATASTREAM now uses the dsBuffer to read and write bytes in
		a QUADLET form.  This means that in order for data to be written into
		the buffer, you must COMPLETE AN ENTIRE BYTE OF DATA.  If you do not 
		then the data is not written.

		@{
*/

#ifndef _DATASTREAM_H
#define _DATASTREAM_H

#include "TCTypes.h"
#include "keyvalue.h"
#include "dsbuffer.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

typedef	struct
{
	DS_BUFFER	dsbBuffer;							// Used for quadlet buffer access.
	uint32		cur_offset;							// Current offset
	uint32		total_size;							// Size of the block of data
	uint32		bitbuf;								// Carried bits for the next read operation
	uint32		bits;								// number of bits in the bitbuf field
	uint32		mode;								// This stream is being used for READ or WRITE, not both.
	KEYVALUE	markers[4];							// Storage for markers
} DataStream, *pDataStream;

// DS R/W Modes, are in bit 1
#define		dsRW_MASK					0x00000001
#define		dsMODE_WRITE				1			//!< Write Bytes in Native Byte order
#define		dsMODE_READ					0			//!< Read  Bytes in Native Byte order

// Endianness modes stored in bits 2 and 3
#define		dsENDIAN_MASK				0x00000006 	

#if 0 //LM???
#define		dsMODE_BIGENDIAN			0			//!< R/W bytes as if they were all in Big Endian byte order. This is the default.
#define		dsMODE_LITTLEENDIAN			2			//!< R/W bytes as if they were all in Little Endian byte order.
#define		dsMODE_NATIVE				4			//!< R/W bytes in whatevere the native format is. 
#define		dsMODE_PLATFORMENDIAN		dsMODE_BIGENDIAN
#else
#define		dsMODE_BIGENDIAN			2			//!< R/W bytes as if they were all in Big Endian byte order.
#define		dsMODE_LITTLEENDIAN			4			//!< R/W bytes as if they were all in Little Endian byte order.
#define		dsMODE_NATIVE				0			//!< R/W bytes in whatevere the native format is. This is the default.
#define		dsMODE_PLATFORMENDIAN		dsMODE_NATIVE
#endif

/**
	This function takes a DataStream* in, and initializes it for use by dsWrite/Read functions.

	@param pStream 	The new datastream
	@param data		This is the raw data which you intend to read as if it were a stream.
	@param size		This is the size in BYTES, of the data buffer.
	@param mode		Must be either dsMODE_READ or dsMODE_WRITE, depending upon the operations you wish to perform with this stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsOpenStream(pDataStream pStream, void* data, uint32 size, uint32 mode);

/** Re-opens and re-initializes an already open datastream.  This function would
	typically be used to reuse a command packetBlock as the response packetBlock.  This function
	also calls dsRESET so the stream will now be at the beginning. 
		
	@param pStream	An existing datastream
	@param mode		Must be either dsMODE_READ or dsMODE_WRITE, depending upon the operations 	you wish to perform with this stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsReOpenStream(pDataStream pStream, uint32 mode);

/**	This function switches the mode of a stream from Reading to Writing. 
	This is made to be an explicit change, so that you can't so easily go
	about mucking with valid values.
		
	@param pStream	An existing datastream
	@param mode		Must be either dsMODE_READ or dsMODE_WRITE, depending upon the operations you wish to perform with this stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsSwitchMode(pDataStream pStream, uint32 mode);

/** Closes a stream context. Use this function when you are done with a stream
	and have no more use for it.  This does not delete the data block passed into the dsOpenStream.
	Also it sets the stream context to invalid, so that any reads or writes using this stream context
	will cause an exception.
		
	@param pStream	An existing datastream to be closed.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/		
HRESULT	dsCloseStream(pDataStream pStream);

/*	DataStream Read Functions */

/** Read the specified number of bits from the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param len		The number of bits to read.
	@param data		The address to copy the read data into.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsReadBits(pDataStream pStream, uint32 len, uint32* data);

/** Read 8 bits from the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The address to copy the read data into.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsRead8Bits(pDataStream pStream, BYTE* data);

/** Read 16 bits from the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The address to copy the read data into.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsRead16Bits(pDataStream pStream, uint16* data);

/** Read 24 bits from the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The address to copy the read data into.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsRead24Bits(pDataStream pStream, uint32* data);

/** Read 32 bits from the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The address to copy the read data into.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsRead32Bits(pDataStream pStream, uint32* data);


/*  DataStream Write Functions */

/** Write the specified number of bits to the data stream and adjust the position of the stream.
	Note that an entire byte must be written before the data is actually written.
	@param pStream	An existing datastream to be used.
	@param len		The number of bits to write.
	@param data		The data to write to the stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	dsWriteBits(pDataStream pStream, uint32 len, uint32  data);

/** Write 8 bits to the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The data to write to the stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	dsWrite8Bits(pDataStream pStream, BYTE data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	dsWrite8Bits(pDataStream pStream, BYTE data)
{
	return dsWriteBits(pStream,8,data); 
}

/** Write 16 bits to the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The data to write to the stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	dsWrite16Bits(pDataStream pStream, uint16 data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	dsWrite16Bits(pDataStream pStream, uint16 data)
{
	return dsWriteBits(pStream,16,data); 
}

/** Write 24 bits to the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The data to write to the stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	dsWrite24Bits(pDataStream pStream, uint32 data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	dsWrite24Bits(pDataStream pStream, uint32 data)
{
	return dsWriteBits(pStream,24,data); 
}

/** Write 32 bits to the data stream and adjust the position of the stream.
	@param pStream	An existing datastream to be used.
	@param data		The data to write to the stream.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	dsWrite32Bits(pDataStream pStream, uint32 data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	dsWrite32Bits(pDataStream pStream, uint32 data)
{
	return dsWriteBits(pStream,32,data); 
}

   
/** This function is used for adding two streams together. It may only be used on a whole byte
    boundary.  Meaning neither stream can be anywhere except on a byte boundary.
    If the srcStream is too large to completely fit into the destination stream, you will 
    receive an error, and the destination stream will not be affected.
	@param pDstStream	The destination stream to add on to.
	@param pSrcStream	The source stream to add.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsAddStream(pDataStream	pDstStream, pDataStream pSrcStream);

// fast-forward and re-wind functions

/** This function moves the current pointer ahead, 'count' bytes. 
	You will get an error if you run over the physical end of the data block passed into 
	OpenStream.

	This function requires that your current position is at the beginning of a complete byte.
	You may not be in the process of writing 2 bits, and then call this function. It will FAIL!
	
	@param pStream		The stream to advance.
	@param count		Number of BYTES to move forward.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsFastForward(pDataStream pStream, uint32 count);		

/**	This function moves the current pointer back, 'count' bytes. 
	You will get an error if you run over the physical start of the data block passed into 
	OpenStream.
		
	This function requires that your current position is at the beginning of a complete byte.
	You may not be in the process of writing 2 bits, and then call this function. It will FAIL!
	
	@param	pStream		The stream to rewind.
	@param	count		Number of BYTES to move back.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsRewind(pDataStream pStream, uint32 count);		// MOVE BACK 'count' bytes.

/**	This function resets the current position to the beginning of the data. 
	This works no matter which state the data stream is currently in. Unless of course the
	stream has been closed.
		
	@param pStream		The stream to reset
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT dsReset(pDataStream pStream);					// Reset the stream to the beginning.


// Position functions

/**	This function returns the current position pointer of the stream.
	@param pStream		The stream to use.
	@return				The position in the stream.
*/
uint32 dsGetPosition(pDataStream pStream);

/** This sets the position to the indexed position specified.  
	This is indexed from the beginning of the data buffer.
	@param pStream		The stream to use.
	@param pos			The position to set.
*/
void dsSetPosition(pDataStream pStream, uint32  pos);

// Trim functions

/**	This function changes the "SIZE" of the buffer, to the specified number.
	WARNING :  If you use this to GROW the buffer, make sure that the buffer you are using is big enough.
	@param pStream		The stream to trim.
	@param newsize		The new size for the data buffer.
*/
void dsTrimSize(pDataStream pStream, uint32 newsize);

// Marker functions

/** Sets a marker in a data stream.  This allows for one function to pass
	a datastream to another function, and both functions would be able to know about
	the variable postion which marked the beginning of some position which would
	be well known.
	@param pStream		The stream to mark.
	@param markerid		The mark to set at the current position.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsSetMarker(pDataStream	pStream, uint32 markerid);

/** Remvoes a marker from a data stream.
	@param pStream		The stream to remove a marker from.
	@param markerid		The mark to remove.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsKillMarker(pDataStream pStream, uint32 markerid);

/** Set the position of a stream to an existing marker.
	@param pStream		The stream to use.
	@param markerid		The marker to locate to.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsGotoMarker(pDataStream pStream, uint32 markerid);

// Chunk functions

/** Read a chunk of data from the stream.
	NOTICE: THE BUFFER SPECIFIED BY THE "buffer" PARAMETER, ** MUST ** START ON 4 BYTE BOUNDARY.
	@param pStream		The stream to use.
	@param size			The number of bytes to read.
	@param buffer		The buffer to place the read data into.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsReadChunk (pDataStream pStream, uint32 size, char* buffer);

/** Write a chunk of data into the stream.
	NOTICE: THE BUFFER SPECIFIED BY THE "buffer" PARAMETER, ** MUST ** START ON 4 BYTE BOUNDARY.
	@param pStream		The stream to use.
	@param size			The number of bytes to write.
	@param buffer		The buffer to obtain data from.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT	dsWriteChunk(pDataStream pStream, uint32 size, char* buffer);

HRESULT	dsReadBuffer(pDataStream pStream, uint32 size, void *data);
HRESULT	dsWriteBuffer(pDataStream pStream, uint32 size, void *data);

HRESULT	dsHandleBits(pDataStream pStream, uint32 len, uint32 *data, BOOL bWrite);
HRESULT	dsHandle8Bits(pDataStream pStream, BYTE *data, BOOL bWrite);
HRESULT	dsHandle16Bits(pDataStream pStream, uint16 *data, BOOL bWrite);
HRESULT	dsHandle24Bits(pDataStream pStream, uint32 *data, BOOL bWrite);
HRESULT	dsHandle32Bits(pDataStream pStream, uint32 *data, BOOL bWrite);
HRESULT	dsHandleBuffer(pDataStream pStream, uint32 size, void *data, BOOL bWrite);

// Copy functions

/** This function copy one stream's data to another stream, also copying the
	markers (which are in a valid range of the stream). This does not cause an error
	if the sizes of the streams do not match. Instead the most data which can be copied, 
	will be copied, and the extra data will be dropped silently.
	@param destStream	The destination data stream
	@param srcStream	The source data stream
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT		dsCopyStream(pDataStream destStream, pDataStream srcStream);

// Padding functions

/** Pads the data stream to a quadlet boundary.
	@param pStream		The stream to pad.
	@param padbyte		The value to pad with.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT		dsPadToEndOfQuadlet(pDataStream	pStream, BYTE padbyte);

/** Pads the data stream to the end of its buffer.
	@param pStream		The stream to pad.
	@param padbyte		The value to pad with.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT		dsPadToEndOfBuffer(pDataStream pStream, BYTE padbyte);

#endif //_DATASTREAM_H


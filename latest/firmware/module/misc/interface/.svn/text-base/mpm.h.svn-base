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

/** @addtogroup MemoryPoolManager

	This module provides dynamic memory access for packetblocks, packetblock 
	payload data, and applications in a deterministic manner.

	This module can be used from the command prompt by the @ref cli_mpm commands.

	To the caller, it looks a lot like 'malloc' and 'free'.

	To allocate a block of memory, call

	mpmBlockAllocate()
	
	To release a previously allocated block of memory, call
	
	mpmBlockRelease()

	Internally, the memory pool manager:

	-	consists of MPM_POOL_ITEMS pools 
	-	defined by their blockSize and blockItems (in the MPM_POOL_SIZES struct)
	-	for each pool there's a descriptor for handling the pool allocation
		mpmPoolDesc array of MPM_POOL_DESC structs consisting of:
		
			ptr:		pointer to next candidate for allocating a block
			blockSize:	size of block
			blockItems:	number of blocks
			allocPtr:	pointer to array mpmPoolDescAllocPtr
						of flags specifying the alloc state of a block
			blockPtr:	pointer to array mpmPoolDescBlockPtr
						of pointers into the data block mpmPoolData

	-	the data blocks in the mpmPoolData each consists of
			
			MPM_BLOCK_HEADER
			guard:		guard value to ensure correctness of block
			poolDescPtr:pointer to pool descriptor
			index:		Index in pool descriptor

			block data:	this is the data the user will get a pointer to
			
			MPM_BLOCK_TRAILER
			guard:		guard value to ensure correctness of block
	
	@{
	
*/

#ifndef _MPM_H
#define _MPM_H

#define _CHECK_MPM_POOL_PB
#undef _CHECK_MPM_POOL_PB

#define _CHECK_MPM_POOL_LEAK
//#undef _CHECK_MPM_POOL_LEAK

#include "TCTypes.h"
#include "packetBlock.h"

#define MPM_BLOCK_HEADER_GUARD	0x53657879
#define MPM_BLOCK_TRAILER_GUARD	0x424C4F43



typedef struct
{
	uint32			ptr;
	uint32			indexPool;
	uint32			inuse;
	uint32			blockSize;
	uint32			blockItems;
	void*			allocPtr;
	unsigned char**	blockPtr;
} MPM_POOL_DESC;

typedef struct
{
	uint32			guard;
	MPM_POOL_DESC*	poolDescPtr;
	uint32			allocTime;
	uint32			used;
	uint32			indexBlock;
#ifdef _CHECK_MPM_POOL_PB
	int32			indexCheckPB;
#endif //_CHECK_MPM_POOL_PB
#ifdef _CHECK_MPM_POOL_LEAK
	int32			creatorID;
#endif //_CHECK_MPM_POOL_LEAK
} MPM_BLOCK_HEADER;

typedef struct
{
	uint32			guard;
} MPM_BLOCK_TRAILER;

typedef struct
{
	uint32			blockSize;
	uint32			blockItems;
} MPM_POOL_SIZES;

#define MPM_BLOCK_ENV_SIZE	(sizeof(MPM_BLOCK_HEADER)+sizeof(MPM_BLOCK_TRAILER))

#define MPM_POOL_ITEMS		8

#ifdef _MPM_POOL_SMALL

#define MPM_POOL_1_SIZE		32			// 32
#define MPM_POOL_1_ITEMS	4			// 32
#define MPM_POOL_2_SIZE		64			// 64
#define MPM_POOL_2_ITEMS	4			// 32
#define MPM_POOL_3_SIZE		128			// 128
#define MPM_POOL_3_ITEMS	4			// 32
#define MPM_POOL_4_SIZE		sizeof(PB)	// sizeof(PB)
#define MPM_POOL_4_ITEMS	10			// 64
#define MPM_POOL_5_SIZE		256			// 256
#define MPM_POOL_5_ITEMS	4			// 16
#define MPM_POOL_6_SIZE		512			// 520
#define MPM_POOL_6_ITEMS	2			// 16
#define MPM_POOL_7_SIZE		1024		// 1032
#define MPM_POOL_7_ITEMS	2			// 16
#define MPM_POOL_8_SIZE		2048		// 2080
#define MPM_POOL_8_ITEMS	2			// 16

#else

#define MPM_POOL_1_SIZE		32			// 32
#define MPM_POOL_1_ITEMS	16			// 32
#define MPM_POOL_2_SIZE		64			// 64
#define MPM_POOL_2_ITEMS	16			// 32
#define MPM_POOL_3_SIZE		128			// 128
#define MPM_POOL_3_ITEMS	16			// 32
#define MPM_POOL_4_SIZE		sizeof(PB)	// sizeof(PB)
#define MPM_POOL_4_ITEMS	32			// 64
#define MPM_POOL_5_SIZE		256			// 256
#define MPM_POOL_5_ITEMS	16			// 16
#define MPM_POOL_6_SIZE		512			// 520
#define MPM_POOL_6_ITEMS	8			// 16
#define MPM_POOL_7_SIZE		1024		// 1032
#define MPM_POOL_7_ITEMS	8			// 16
#define MPM_POOL_8_SIZE		2048		// 2080
#define MPM_POOL_8_ITEMS	8			// 16

#endif

#ifdef _CHECK_MPM_POOL_PB 
#define MPM_POOL_PB_SIZE	sizeof(PB)	// update index values if change to pool sizes
#define MPM_POOL_PB_INDEX	(4-1)
#define MPM_POOL_PB_ITEMS	MPM_POOL_4_ITEMS

typedef struct
{
	BOOL				allocated;	
	PB					*pPB;
	MPM_POOL_DESC		*poolDescPtr;
	MPM_BLOCK_HEADER	*blockHeaderPtr;
	MPM_BLOCK_TRAILER	*blockTrailerPtr;
	uint32				*DescAllocPtr;
	unsigned char		**DescBlockPtr;
} MPM_POOL_PB_CHECK;

extern MPM_POOL_PB_CHECK	mpmPoolPBCheck[];
#endif //_CHECK_MPM_POOL_PB

#define MPM_CREATOR_ID_TEST				0x00001000
#define MPM_CREATOR_ID_PB				0x00002000
#define MPM_CREATOR_ID_AKE				0x00010000

/**	Initialize this module by creating all necessary pools.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT mpmInitialize(void);

/** Allocate a block of memory
	@param numBytesRequested	The number of bytes being requested
	@param blockPtr				Address of the pointer that will be set to an available block
	@param creatorID			The ID of the creator (debugging)
	@param bClear				TRUE if block is to be cleared to 0.
	@return						NO_ERROR (0) or an @ref ErrorCodes "error code".
*/

HRESULT mpmBlockAllocate(uint32 numBytesRequested, void **blockPtr, uint32 creatorID, BOOL bClear);

/**	Release a block of previously allocated memory.
	Caller gives us a pointer to the application block portion of 
	the block that we allocated.
	Verify that guard zones are undisturbed.
		
	If there are any errors, this module reports the error.
	The reasoning is that errors in release are due to programming 
	errors which cannot be ignored - and applications calling the 
	'release' function may not check the return value since we might
	reasonably expect it to always succeed.
	@param blockPtr				Pointer to the previously allocated block.
	@return						NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT mpmBlockRelease(void *blockPtr);

uint32	mpmGetNumberOfPools(void);
uint32	mpmGetNumberOfUsedBlocksInPool(uint32 indexPool);
HRESULT mpmBlockIsInUse(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, BOOL *inUse);

HRESULT mpmGetBlockPointer(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, void **blockPtr);
HRESULT mpmGetBlockHeader(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, MPM_BLOCK_HEADER **blockHeaderPtr);
HRESULT mpmGetBlockTrailer(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, MPM_BLOCK_TRAILER **blockTrailerPtr);

extern MPM_POOL_DESC			mpmPoolDesc[];				// pool descriptors

#endif

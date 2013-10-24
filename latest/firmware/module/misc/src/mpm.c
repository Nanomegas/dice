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

	Purpose:	API to dynamic memory allocation
	
	This module provides dynamic memory access for packetblocks 
	and packetblock payload data.
	To the packetBlock module, it looks a lot like 'malloc' and 'free'.

	API:
	mpmBlockAllocate - allocates a block of memory for the caller
	mpmBlockRelease - returns an allocated block to the memory pool

	Revisions:
		created 11/09/2002 lm

***************************************************************************/

#ifdef _1394CORE

#include "TCTasking.h"
#include "packetBlock.h"
#include "mpm.h"
#include "mpmCli.h"
#include "mpmStatistics.h"
#include "lhlCli.h"
#include "lhlDefs.h"

static char 			moduleName[] = __FILE__ ;

static uint32 mpmMutexSemID; // mutual exclusion semaphore for access to memory pools

static const MPM_POOL_SIZES mpmPoolSizes[MPM_POOL_ITEMS] =
{
	{MPM_POOL_1_SIZE, MPM_POOL_1_ITEMS}, // blockSize, blockItems
	{MPM_POOL_2_SIZE, MPM_POOL_2_ITEMS}, // blockSize, blockItems
	{MPM_POOL_3_SIZE, MPM_POOL_3_ITEMS}, // blockSize, blockItems
	{MPM_POOL_4_SIZE, MPM_POOL_4_ITEMS}, // blockSize, blockItems
	{MPM_POOL_5_SIZE, MPM_POOL_5_ITEMS}, // blockSize, blockItems
	{MPM_POOL_6_SIZE, MPM_POOL_6_ITEMS}, // blockSize, blockItems
	{MPM_POOL_7_SIZE, MPM_POOL_7_ITEMS}, // blockSize, blockItems
	{MPM_POOL_8_SIZE, MPM_POOL_8_ITEMS}, // blockSize, blockItems
};

#define MPM_POOL_DATA_SIZE	(((MPM_POOL_1_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_1_ITEMS)+\
							 ((MPM_POOL_2_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_2_ITEMS)+\
							 ((MPM_POOL_3_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_3_ITEMS)+\
							 ((MPM_POOL_4_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_4_ITEMS)+\
							 ((MPM_POOL_5_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_5_ITEMS)+\
							 ((MPM_POOL_6_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_6_ITEMS)+\
							 ((MPM_POOL_7_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_7_ITEMS)+\
							 ((MPM_POOL_8_SIZE+MPM_BLOCK_ENV_SIZE)*MPM_POOL_8_ITEMS))

#define MPM_BLOCK_ITEMS		(MPM_POOL_1_ITEMS+\
							 MPM_POOL_2_ITEMS+\
							 MPM_POOL_3_ITEMS+\
							 MPM_POOL_4_ITEMS+\
							 MPM_POOL_5_ITEMS+\
							 MPM_POOL_6_ITEMS+\
							 MPM_POOL_7_ITEMS+\
							 MPM_POOL_8_ITEMS)

static unsigned char	mpmPoolData[MPM_POOL_DATA_SIZE];			// pool of all blocks header,data,trailer
static uint32			mpmPoolDescAllocPtr[MPM_BLOCK_ITEMS];		// array of desc allocate flags
static unsigned char	*mpmPoolDescBlockPtr[MPM_BLOCK_ITEMS];		// array of desc block pointers
MPM_POOL_DESC			mpmPoolDesc[MPM_POOL_ITEMS];				// pool descriptors

#ifdef _CHECK_MPM_POOL_PB 
MPM_POOL_PB_CHECK		mpmPoolPBCheck[MPM_POOL_PB_ITEMS];
#endif //_CHECK_MPM_POOL_PB

/*****************************************************************************

	memPoolManagerInitialize

	Initialize this module by creating all necessary pools.

*****************************************************************************/
HRESULT mpmInitialize(void)
{
	HRESULT				hResult = NO_ERROR;
	uint32				indexPool = 0;
	uint32				indexBlock = 0;
	uint32				descPos = 0;
	uint32				blockPos = 0;
	uint32				blockItemsPos = 0;
	uint32				blockSize = 0;
	uint32				blockItems = 0;
	uint32				*DescAllocPtr = NULL;
	unsigned char		**DescBlockPtr = NULL;
	unsigned char		*poolDataPtr = NULL;
	MPM_POOL_DESC		*poolDescPtr = NULL;
	MPM_BLOCK_HEADER	*blockHeaderPtr = NULL;
	MPM_BLOCK_TRAILER	*blockTrailerPtr = NULL;

#ifdef _CHECK_MPM_POOL_PB
	// check PB defines are up to date
	if ((mpmPoolSizes[MPM_POOL_PB_INDEX].blockSize != MPM_POOL_PB_SIZE) || 
		(mpmPoolSizes[MPM_POOL_PB_INDEX].blockItems != MPM_POOL_PB_ITEMS))
	{
		hResult = E_MPM_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	memset(mpmPoolPBCheck, 0, sizeof(mpmPoolPBCheck));
#endif //_CHECK_MPM_POOL_PB


	// exclusive access for the memory block pool (mutex)
	hResult = TCMutexOpen(&mpmMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	descPos = 0;
	blockItemsPos = 0;
	for (indexPool = 0; indexPool < MPM_POOL_ITEMS; indexPool++)
	{
		poolDescPtr = &(mpmPoolDesc[indexPool]);
		DescAllocPtr = &(mpmPoolDescAllocPtr[blockItemsPos]);
		DescBlockPtr = &(mpmPoolDescBlockPtr[blockItemsPos]);
		poolDataPtr = &(mpmPoolData[descPos]);

		blockSize = mpmPoolSizes[indexPool].blockSize;
		blockItems = mpmPoolSizes[indexPool].blockItems;

		// pool descriptors
		poolDescPtr->ptr = 0;
		poolDescPtr->indexPool = indexPool;
		poolDescPtr->inuse = 0;
		poolDescPtr->blockSize = blockSize;
		poolDescPtr->blockItems = blockItems;
		poolDescPtr->allocPtr = DescAllocPtr;
		poolDescPtr->blockPtr = DescBlockPtr;

		blockPos = 0;
		for (indexBlock = 0; indexBlock < blockItems; indexBlock++)
		{
			// block header
			blockHeaderPtr = (MPM_BLOCK_HEADER *) &(poolDataPtr[blockPos]);
			blockHeaderPtr->guard = MPM_BLOCK_HEADER_GUARD;
			blockHeaderPtr->indexBlock = indexBlock;
			blockHeaderPtr->allocTime = 0;
			blockHeaderPtr->used = 0;
			blockHeaderPtr->poolDescPtr = poolDescPtr;
#ifdef _CHECK_MPM_POOL_PB
			blockHeaderPtr->indexCheckPB = -1;
#endif //_CHECK_MPM_POOL_PB
			blockPos += sizeof(MPM_BLOCK_HEADER);

			// desc allocate flags
			DescAllocPtr[indexBlock] = 0;
			// desc block pointers
			DescBlockPtr[indexBlock] = &(poolDataPtr[blockPos]);
#if 0
			memset(&(poolDataPtr[blockPos]), 0, blockSize);
#endif
			blockPos += blockSize;

			// block trailer
			blockTrailerPtr = (MPM_BLOCK_TRAILER *) &(poolDataPtr[blockPos]);
			blockTrailerPtr->guard = MPM_BLOCK_TRAILER_GUARD;
			blockPos += sizeof(MPM_BLOCK_TRAILER);
		}
		descPos += blockPos;
		blockItemsPos += blockItems;
	}

#ifdef _STATISTICS
	hResult = mpmStatisticsInitialize() ;
	if (hResult != NO_ERROR) return hResult;
#endif //_STATISTICS

#ifdef _CLI_TOOL_MPM
	mpmCliInstallTools();
#endif //_CLI_TOOL_MPM

	return hResult;
}	

/*****************************************************************************

	memBlockAllocate

	Attempt to allocate a block of memory for caller.

*****************************************************************************/

HRESULT mpmBlockAllocate(uint32 numBytesRequested, void **blockPtr, uint32 creatorID, BOOL bClear)
{
	HRESULT				hResult = NO_ERROR;
	BOOL				bFound = FALSE;
	uint32				ptr = 0;
	uint32				indexPool = 0;
	uint32				indexPoolStart = MPM_POOL_ITEMS;
	uint32				indexBlock = 0;
	uint32				blockSize = 0;
	uint32				blockItems = 0;
	uint32				*DescAllocPtr = NULL;
	unsigned char		**DescBlockPtr = NULL;
	MPM_POOL_DESC		*poolDescPtr = NULL;
	MPM_BLOCK_HEADER	*blockHeaderPtr = NULL;
	MPM_BLOCK_TRAILER	*blockTrailerPtr = NULL;
	
	*blockPtr = NULL;

	// Search descriptor table for the first pool with blocks large enough for caller
	for (indexPool = 0; indexPool < MPM_POOL_ITEMS; indexPool++)
	{
		if (numBytesRequested <= mpmPoolSizes[indexPool].blockSize)
		{
			indexPoolStart = indexPool;
			break;
		}
	}

	if (indexPoolStart >= MPM_POOL_ITEMS)
	{
		hResult = E_MPM_REQ_SIZE_TOO_LARGE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Exclusive access to memory pool manager
	hResult = TCMutexLock(mpmMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	// Try to allocate a block for caller. If unsuccessful, try to allocate from the next larger pool.
	for (indexPool = indexPoolStart; indexPool < MPM_POOL_ITEMS && !bFound; indexPool++)
	{
		poolDescPtr = &(mpmPoolDesc[indexPool]);

		ptr = poolDescPtr->ptr;
		blockSize = poolDescPtr->blockSize;
		blockItems = poolDescPtr->blockItems;
		DescBlockPtr = poolDescPtr->blockPtr;
		DescAllocPtr = poolDescPtr->allocPtr;

		if (numBytesRequested <= blockSize)
		{
			if (poolDescPtr->inuse < poolDescPtr->blockItems)
			{
				for (indexBlock = 0; indexBlock < blockItems; indexBlock++)
				{
					if (DescAllocPtr[ptr] == FALSE)
					{
						blockHeaderPtr = (MPM_BLOCK_HEADER *) (DescBlockPtr[ptr] - sizeof(MPM_BLOCK_HEADER));
						blockTrailerPtr = (MPM_BLOCK_TRAILER *) (DescBlockPtr[ptr] + blockSize);

						if ((blockHeaderPtr->guard == MPM_BLOCK_HEADER_GUARD) &&
							(blockHeaderPtr->indexBlock == ptr) &&
							(blockHeaderPtr->poolDescPtr == poolDescPtr) &&
							(blockTrailerPtr->guard == MPM_BLOCK_TRAILER_GUARD))
						{
							DescAllocPtr[ptr] = TRUE;
							*blockPtr = (void *) DescBlockPtr[ptr];
							if (bClear)
							{
								memset(*blockPtr, 0, numBytesRequested);
							}
							blockHeaderPtr->allocTime = TCTimeGet();
							blockHeaderPtr->used++;
							bFound = TRUE;
#ifdef _CHECK_MPM_POOL_PB
							if (numBytesRequested == MPM_POOL_PB_SIZE &&
								indexPool == MPM_POOL_PB_INDEX) // assume this is a PB
							{
								uint32 index;

								for (index = 0; index < MPM_POOL_PB_ITEMS; index++)
								{
									if (mpmPoolPBCheck[index].allocated == FALSE)
									{
										mpmPoolPBCheck[index].allocated = TRUE;
										mpmPoolPBCheck[index].blockHeaderPtr = blockHeaderPtr;
										mpmPoolPBCheck[index].blockTrailerPtr = blockTrailerPtr;
										mpmPoolPBCheck[index].DescAllocPtr = &DescAllocPtr[ptr];
										mpmPoolPBCheck[index].DescBlockPtr = &DescBlockPtr[ptr];
										mpmPoolPBCheck[index].pPB = *blockPtr;
										blockHeaderPtr->indexCheckPB = index;
										break;
									}
								}
							}
#endif // _CHECK_MPM_POOL_PB
#ifdef _CHECK_MPM_POOL_LEAK
							blockHeaderPtr->creatorID = creatorID;
#else //_CHECK_MPM_POOL_LEAK
							UNUSED_ARG(creatorID);
#endif //_CHECK_MPM_POOL_LEAK
						}
						else
						{
							hResult = E_MPM_GUARD_BAND_VIOLATION;
							sysLogError(hResult, __LINE__, moduleName);
							//LM??? restore situation
						}
					}
					ptr++;
					if (ptr >= blockItems)
					{
						ptr = 0;
					}
					if (bFound == TRUE)
					{
						poolDescPtr->ptr = ptr;
						poolDescPtr->inuse++;
#ifdef _STATISTICS
						mpmStatistics.inuse[indexPool] = poolDescPtr->inuse;
						mpmStatistics.used[indexPool]++;
						if (mpmStatistics.inuseMax[indexPool] < poolDescPtr->inuse)
						{
							mpmStatistics.inuseMax[indexPool] = poolDescPtr->inuse;
						}
#endif //_STATISTICS
						break;
					}
				}
			}
		}
		else
		{
			hResult = E_MPM_DESCRIPTOR_ORDER;
			sysLogError(hResult, __LINE__, moduleName);
		}
	}

	// Release exclusive access to memory pool manager
	TCMutexUnlock(mpmMutexSemID);

	if (bFound == FALSE)
	{
		hResult = E_MPM_NO_BLOCK_AVAILABLE ;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

/*****************************************************************************

	memBlockRelease

	Release a block of previously allocated memory.
	Caller gives us a pointer to the application block portion of the block that
	we allocated.  Verify that guard zones are undisturbed.
		
	If there are any errors, this module reports the error.  The reasoning is that 
	errors in release are due to programming errors which cannot be ignored - and 
	applications calling the 'release' function may not check the return value since 
	we might reasonably expect it to always succeed.

*****************************************************************************/

HRESULT mpmBlockRelease(void *blockPtr)
{
	HRESULT				hResult = NO_ERROR;
	uint32				indexBlock = 0;
	uint32				blockSize = 0;
	uint32				*DescAllocPtr = NULL;
	unsigned char		*DescBlockPtr = NULL;
	MPM_POOL_DESC		*poolDescPtr = NULL;
	MPM_BLOCK_HEADER	*blockHeaderPtr = NULL;
	MPM_BLOCK_TRAILER	*blockTrailerPtr = NULL;

	if (blockPtr == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Exclusive access to memory pool manager
	hResult = TCMutexLock(mpmMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	DescBlockPtr = blockPtr;

	blockHeaderPtr = (MPM_BLOCK_HEADER *) (DescBlockPtr - sizeof(MPM_BLOCK_HEADER));

	if (blockHeaderPtr->guard != MPM_BLOCK_HEADER_GUARD)
	{
		hResult = E_MPM_GUARD_BAND_VIOLATION;
		sysLogError(hResult, __LINE__, moduleName);
		//LM??? restore situation
	}
#if 1 //LM???
	else
#endif
	{
		indexBlock = blockHeaderPtr->indexBlock;
		poolDescPtr = blockHeaderPtr->poolDescPtr;
		if (poolDescPtr)
		{
			blockSize = poolDescPtr->blockSize;
			blockTrailerPtr = (MPM_BLOCK_TRAILER *) (DescBlockPtr + blockSize);

			if (blockTrailerPtr->guard != MPM_BLOCK_TRAILER_GUARD)
			{
				hResult = E_MPM_GUARD_BAND_VIOLATION;
				sysLogError(hResult, __LINE__, moduleName);
				//LM??? restore situation
				blockTrailerPtr->guard = MPM_BLOCK_TRAILER_GUARD;
				//LM??? check next block...
			}
#if 0 //LM???
			else
#endif
			{
				if (indexBlock < blockSize)
				{
					DescAllocPtr = poolDescPtr->allocPtr;
					if (DescAllocPtr[indexBlock] == TRUE)
					{
						DescAllocPtr[indexBlock] = FALSE;
						poolDescPtr->ptr = indexBlock;
						poolDescPtr->inuse--;
#ifdef _STATISTICS
						mpmStatistics.inuse[poolDescPtr->indexPool] = poolDescPtr->inuse;
#endif //_STATISTICS

#ifdef _CHECK_MPM_POOL_PB
						{
							if (blockHeaderPtr->indexCheckPB != -1)
							{
								uint32 index = blockHeaderPtr->indexCheckPB;

								if (mpmPoolPBCheck[index].allocated == TRUE)
								{
									mpmPoolPBCheck[index].allocated = FALSE;
									mpmPoolPBCheck[index].blockHeaderPtr = NULL;
									mpmPoolPBCheck[index].blockTrailerPtr = NULL;
									mpmPoolPBCheck[index].DescAllocPtr = NULL;
									mpmPoolPBCheck[index].DescBlockPtr = NULL;
									mpmPoolPBCheck[index].pPB = NULL;
									blockHeaderPtr->indexCheckPB = - 1;
								}
							}
						}
#endif // _CHECK_MPM_POOL_PB
					}
					else
					{
						SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "mpm pool freeing item already free\n\r");
					}
				}
				else
				{
					hResult = E_MPM_BAD_BLOCK_SIZE;
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
		}
		else
		{
			hResult = E_MPM_DESCRIPTOR_ORDER;
			sysLogError(hResult, __LINE__, moduleName);
		}
	}

	// Release exclusive access to memory pool manager
	TCMutexUnlock(mpmMutexSemID);

	return hResult;
}	

uint32 mpmGetNumberOfPools(void)
{
	return MPM_POOL_ITEMS;
}

uint32 mpmGetNumberOfUsedBlocksInPool(uint32 indexPool)
{
	HRESULT				hResult = NO_ERROR;

	uint32 inuse = 0;

	if (indexPool >= MPM_POOL_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
	}
	else
	{
		inuse = mpmPoolDesc[indexPool].inuse;
	}

	return inuse;
}

HRESULT mpmBlockIsInUse(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, BOOL *inUse)
{
	HRESULT				hResult = NO_ERROR;
	uint32				*DescAllocPtr = NULL;

	*inUse = FALSE;
		
	if ((poolDescPtr == NULL) ||
		((DescAllocPtr = poolDescPtr->allocPtr) == NULL))
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (indexBlock >= poolDescPtr->blockItems)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*inUse = (BOOL) DescAllocPtr[indexBlock];

	return hResult;
}	

HRESULT mpmGetBlockPointer(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, void **blockPtr)
{
	HRESULT				hResult = NO_ERROR;
	unsigned char		**DescBlockPtr = NULL;

	*blockPtr = NULL;

	if ((poolDescPtr == NULL) ||
		((DescBlockPtr = poolDescPtr->blockPtr) == NULL))
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (indexBlock >= poolDescPtr->blockItems)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*blockPtr = (void *) DescBlockPtr[indexBlock];

	return hResult;
}	

HRESULT mpmGetBlockHeader(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, MPM_BLOCK_HEADER **blockHeaderPtr)
{
	HRESULT				hResult = NO_ERROR;
	unsigned char		**DescBlockPtr = NULL;

	*blockHeaderPtr = NULL;

	if ((poolDescPtr == NULL) ||
		((DescBlockPtr = poolDescPtr->blockPtr) == NULL))
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*blockHeaderPtr = (MPM_BLOCK_HEADER *) (DescBlockPtr[indexBlock] - sizeof(MPM_BLOCK_HEADER));

	return hResult;
}	

HRESULT mpmGetBlockTrailer(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, MPM_BLOCK_TRAILER **blockTrailerPtr)
{
	HRESULT				hResult = NO_ERROR;
	unsigned char		**DescBlockPtr = NULL;

	*blockTrailerPtr = NULL;

	if ((poolDescPtr == NULL) ||
		((DescBlockPtr = poolDescPtr->blockPtr) == NULL))
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*blockTrailerPtr = (MPM_BLOCK_TRAILER *) (DescBlockPtr[indexBlock] + poolDescPtr->blockSize);

	return hResult;
}	

#endif //_1394CORE

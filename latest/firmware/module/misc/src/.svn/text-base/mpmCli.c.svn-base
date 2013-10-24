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

	Purpose:	API to dynamic memory allocation CLI
	
				Command Line Interface

	Revisions:
		created 28/05/2002 lm

***************************************************************************/

#ifdef _1394CORE


#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_MPM
#include "TCTasking.h"
#include "packetBlock.h"
#include "packetBlockCli.h"
#include "mpm.h"
#include "mpmCli.h"
#include "mpmStatistics.h"
#include "cli.h"

#ifdef _STATISTICS
static char 			moduleName[] = __FILE__ ;
#endif //_STATISTICS

#define _MPM_TEST
#undef _MPM_TEST

/*****************************************************************************

	Statistics

*****************************************************************************/

HRESULT mpmCliStatisticsShow(void)
{
	HRESULT		hResult = NO_ERROR;
#ifdef _STATISTICS
	uint32		curTime;
	curTime = TCTimeGet();
	cliOutputLine("MPM Statistics") ;
	cliPrintf("time[%5is] :\n\r", curTime / 1000);
	hResult = mpmCliStatisticsDisplay(&mpmStatistics);
#endif //_STATISTICS

	return hResult;
}

#ifdef _STATISTICS
HRESULT mpmCliStatisticsDisplay(MPM_STATISTICS *statistics)
{
	HRESULT			hResult = NO_ERROR;
	uint32			indexPool = 0;

	if (statistics == NULL)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cliPrintf("  Block  Size   Items   Inuse  Cur     Max     Total\n\r") ;
	for (indexPool = 0; indexPool < MPM_POOL_ITEMS; indexPool++)
	{
		cliPrintf("       %6i  %6i       %6i  %6i  %8i\n\r",
						statistics->blockSize[indexPool],
						statistics->blockItems[indexPool],
						statistics->inuse[indexPool],
						statistics->inuseMax[indexPool],
						statistics->used[indexPool]);
	}

	return hResult;
}
#endif //_STATISTICS

/*****************************************************************************

   	Memory Pool Dump

*****************************************************************************/

static void mpmCliDumpBlockTrailer(uint32 blocksShown)
{
	cliPrintf("Total blocks dumped: %d\n\r", blocksShown);
}	

static void mpmCliDumpBlockHeader(void)
{
#ifdef _CHECK_MPM_POOL_LEAK
	cliPrintf("blk  block in      use            packet       alloc     creator\n\r");
	cliPrintf("size index use    count     PB?   seq num      time         id  \n\r");
#else //_CHECK_MPM_POOL_LEAK
	cliPrintf("blk  block in      use            packet       alloc      alloc \n\r");
	cliPrintf("size index use    count     PB?   seq num      time       thread\n\r");
#endif //_CHECK_MPM_POOL_LEAK
//	cliPrintf("dddd  ddd  xxx  dddddddddd  xxx  dddddddddd  dddddddddd  xxxxxxxx");
}	

static void mpmCliShowBlockSummary(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, BOOL inUse, PB *pPb)
{
	HRESULT				hResult = NO_ERROR;
	MPM_BLOCK_HEADER	*blockHeaderPtr = NULL;

//	cliPrintf("dddd  ddd  xxx  dddddddddd  xxx  dddddddddd  dddddddddd  xxxxxxxx");

	if (poolDescPtr)
	{
		hResult = mpmGetBlockHeader(poolDescPtr, indexBlock, &blockHeaderPtr);
		if (hResult != NO_ERROR) return;

		cliPrintf("%4d  ", poolDescPtr->blockSize);
		cliPrintf("%3d  ", indexBlock);
		cliPrintf("%s  ", inUse ? "yes" : "no ");
		cliPrintf("%10d  ",	blockHeaderPtr->used); 
		cliPrintf("%s  ", (pPb) ? "yes" : "no ");
		cliPrintf("%10d  ",	(pPb) ? pPb->packetSeqNum : 0);
		cliPrintf("%10d  ",	blockHeaderPtr->allocTime);
#ifdef _CHECK_MPM_POOL_LEAK
		cliPrintf("%08X  ",	blockHeaderPtr->creatorID);
#else //_CHECK_MPM_POOL_LEAK
		cliPrintf("%s",	/*(pPb) ? blockHeaderPtr->threadName :*/ " ");
#endif //_CHECK_MPM_POOL_LEAK

		cliPrintf("\n\r");
	}
}

static void mpmCliShowBlockPbSummary(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, BOOL inUse, PB *pPb)
{
	if (poolDescPtr && pPb)
	{
		cliPrintf("Block size %d, index %d, inuse %s, address %#x, packetSeqNum %d\n\r",
						poolDescPtr->blockSize,
						indexBlock,
						inUse ? "yes" : "no ",
						pPb,
						pPb->packetSeqNum);
	}
}	

static char showPbPrefaceStr[] = "    " ;

static void mpmCliDumpPacketDetails(PB *pPb)
{
	cliPrintf("Packet details:\n\r");
#ifdef _SYSDEBUG
	pbCliPrintPB (pPb, showPbPrefaceStr);
#else //_SYSDEBUG
	UNUSED_ARG(pPb);
	cliPrintf("not available\n\r");
#endif //_SYSDEBUG
}	

static BOOL mpmCliDumpBlock(MPM_POOL_DESC *poolDescPtr, uint32 indexBlock, DUMP_TYPE dumpType)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		dumped = FALSE;
	BOOL		blockIsPb = FALSE;
	BOOL		inUse = FALSE;	
	void		*pBlock = NULL;
	PB			*pPb = NULL;
		
	hResult = mpmBlockIsInUse(poolDescPtr, indexBlock, &inUse);
	if (hResult != NO_ERROR)
	{
		return dumped;
	}
	hResult = mpmGetBlockPointer(poolDescPtr, indexBlock, &pBlock);
	if (hResult != NO_ERROR)
	{
		return dumped;
	}

	if (pBlock)
	{
		pPb = (PB *) pBlock;
		if ((pPb->magicNumberFirst == PB_MAGIC_NUMBER &&
			 pPb->magicNumberLast == PB_MAGIC_NUMBER) ||
			(pPb->magicNumberFirst == PB_MAGIC_DELETED &&
			 pPb->magicNumberLast == PB_MAGIC_DELETED))
		{
			blockIsPb = TRUE;
		}
		else
		{
			pPb = NULL;
		}
	}

	switch (dumpType)
	{
		case DUMP_ALL_SUMMARY:
		case DUMP_AVAIL_SUMMARY:
		case DUMP_INUSE_SUMMARY:
				if ((dumpType == DUMP_ALL_SUMMARY) ||
					(dumpType == DUMP_AVAIL_SUMMARY && (inUse == FALSE)) ||
					(dumpType == DUMP_INUSE_SUMMARY && (inUse == TRUE)))
				{
					mpmCliShowBlockSummary(poolDescPtr, indexBlock, inUse, pPb);
					dumped = TRUE ;
				}
				break;

		case DUMP_ALL_PB:
		case DUMP_AVAIL_PB:
		case DUMP_INUSE_PB:
				if ((dumpType == DUMP_ALL_PB && blockIsPb) ||
					(dumpType == DUMP_AVAIL_PB && blockIsPb && (inUse == FALSE)) ||
					(dumpType == DUMP_INUSE_PB && blockIsPb && (inUse == TRUE)))
				{
					mpmCliShowBlockPbSummary(poolDescPtr, indexBlock, inUse, pPb);
					mpmCliDumpPacketDetails (pPb);
					dumped = TRUE ;
				}
				break;

		default:
				cliPrintf("mpmCliDumpBlock - unknown dumpType (%d)\n\r", dumpType);
				break;
	}
	return dumped;
}	

uint32 mpmCliDumpPool(MPM_POOL_DESC *poolDescPtr, DUMP_TYPE dumpType)
{
	uint32		indexBlock = 0;
	uint32 		blocksDumped = 0 ;

	if (poolDescPtr)
	{
		for (indexBlock = 0; indexBlock < poolDescPtr->blockItems; indexBlock++)
		{
			if (mpmCliDumpBlock(poolDescPtr, indexBlock, dumpType))
			{
				blocksDumped++;
			}
		}
	}

	return blocksDumped;
}

HRESULT	mpmCliDump(DUMP_TYPE dumpType)
{
	HRESULT			hResult = NO_ERROR;
	uint32			indexPool = 0;
	uint32			blocksShown = 0 ;

	mpmCliDumpBlockHeader();

	for (indexPool = 0; indexPool < MPM_POOL_ITEMS; indexPool++)
	{
		blocksShown += mpmCliDumpPool(&mpmPoolDesc[indexPool], dumpType); 
	}

	mpmCliDumpBlockTrailer(blocksShown);

	return hResult;
}

HRESULT	mpmCliDumpByIndex(uint32 blockSizeDesired, uint32 indexBlockDesired)
{
	HRESULT			hResult = NO_ERROR;
	uint32			indexPool = 0;
	BOOL			bFound = FALSE;
	BOOL			inUse = FALSE;	
	BOOL			blockIsPb = FALSE;
	void			*pBlock = NULL;
	PB				*pPb = NULL;
	MPM_POOL_DESC	*poolDescPtr = NULL;

	for (indexPool = 0; indexPool < MPM_POOL_ITEMS; indexPool++)
	{
		if (mpmPoolDesc[indexPool].blockSize == blockSizeDesired)
		{
			poolDescPtr = &(mpmPoolDesc[indexPool]);
			bFound = TRUE;
		}
	}

	if (bFound == FALSE)
	{
		cliPrintf("There are no blocks of size %d\n\r", blockSizeDesired);
		return hResult;
	}
	
	if (indexBlockDesired >= poolDescPtr->blockItems)
	{
		cliPrintf("There is no block with the index %d in that pool\n\r", indexBlockDesired);
		return hResult;
	}

	
	hResult = mpmGetBlockPointer(poolDescPtr, indexBlockDesired, &pBlock);
	if (hResult != NO_ERROR)
	{
		return hResult;
	}

	if (pBlock)
	{
		pPb = (PB *) pBlock;
		if ((pPb->magicNumberFirst == PB_MAGIC_NUMBER &&
			 pPb->magicNumberLast == PB_MAGIC_NUMBER) ||
			(pPb->magicNumberFirst == PB_MAGIC_DELETED &&
			 pPb->magicNumberLast == PB_MAGIC_DELETED))
		{
			blockIsPb = TRUE;
		}
		else
		{
			pPb = NULL;
		}
	}

	hResult = mpmBlockIsInUse(poolDescPtr, indexBlockDesired, &inUse);
	if (hResult != NO_ERROR)
	{
		return hResult;
	}

	mpmCliDumpBlockHeader();
	mpmCliShowBlockSummary(poolDescPtr, indexBlockDesired, inUse, pPb);
	if (blockIsPb)
	{
		mpmCliShowBlockPbSummary(poolDescPtr, indexBlockDesired, inUse, pPb);
	}
	mpmCliDumpPacketDetails (pPb);

	return hResult;
}	

static  char	mpmCliStatisticsShow_UsageString[]=\
				"mpm:stats: \n\r"\
				"use: mpm:stats\n\r"\
				"------------------------------------------------------------\n\r"\
				"memory pool manager statistics";

static	CLIDescriptor	mpmCliStatisticsShow_Descriptor[]=
{
	{kCLI_Name,				(uint32)"stats"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mpmCliStatisticsShow},
	{kCLI_UsageString, 		(uint32)mpmCliStatisticsShow_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"mpm statistics"},
	{kCLI_GroupName,		(uint32)"mpm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	mpmCliDumpByIndex_UsageString[]=\
				"mpm:dumpByIndex: \n\r"\
				"use: mpm:dumpByIndex <block size> <block index>\n\r"\
				"------------------------------------------------------------\n\r"\
				"memory pool manager - dump block by index";

static	CLIDescriptor	mpmCliDumpByIndex_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dumpByIndex"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mpmCliDumpByIndex},
	{kCLI_UsageString, 		(uint32)mpmCliDumpByIndex_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"mpm dump block by index"},
	{kCLI_GroupName,		(uint32)"mpm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char CLI_DUMP_TYPE_STRING[] = "all=1,avail=2,inuse=3,allPB=4,availPB=5,inusePB=6" ;	

static  char	mpmCliDump_UsageString[]=								\
				"mpm:dump: \n\r"\
				"use: mpm:dump <dumpType>\n\r"							\
				"where <dumpType> is one of the following tokens:\n\r"	\
				"     all       (all blocks, summary)\n\r"				\
				"     allPB     (all blocks, PB details)\n\r"			\
				"     inuse     (blocks in use, summary)\n\r"			\
				"     inusePB   (blocks in use, PB details)\n\r"		\
				"     avail     (available blocks, summary)\n\r"		\
				"     availPB   (available blocks, PB details)]\n\r" 	\
				"------------------------------------------------------------\n\r"\
				"memory pool manager - dump block information";

static	CLIDescriptor	mpmCliDump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mpmCliDump},
	{kCLI_UsageString, 		(uint32)mpmCliDump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)CLI_DUMP_TYPE_STRING},
	{kCLI_ModuleName,		(uint32)"mpm dump block information"},
	{kCLI_GroupName,		(uint32)"mpm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

#ifdef _MPM_TEST
static  char	mpmCliAlloc_UsageString[] =											\
				"mpm:alloc: \n\r"\
				"use: mpm:alloc <numBytes> <numBlocks>\n\r"							\
				"------------------------------------------------------------\n\r"\
				"memory pool manager - allocation of a block of memory of desired size"						\
				"- you will need the address in order to release the block\n\r" ;
 
static	CLIDescriptor	mpmCliAlloc_Descriptor[]=
{
	{kCLI_Name,				(uint32)"alloc"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mpmTestAlloc},
	{kCLI_UsageString, 		(uint32)mpmCliAlloc_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"mpm - alloc"},
	{kCLI_GroupName,		(uint32)"mpm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	mpmCliRelease_UsageString[]=								\
				"mpm:release: \n\r"\
				"use: mpm:release <blockAddress>\n\r"							\
				"------------------------------------------------------------\n\r"\
				"memory pool manager - releasing of a previously allocated block";

static	CLIDescriptor	mpmCliRelease_Descriptor[]=
{
	{kCLI_Name,				(uint32)"release"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mpmTestRelease},
	{kCLI_UsageString, 		(uint32)mpmCliRelease_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"mpm - release"},
	{kCLI_GroupName,		(uint32)"mpm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
#endif //_MPM_TEST

void mpmCliInstallTools(void)
{
	HRESULT			hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(mpmCliDump_Descriptor);
	hResult += cliInstallCLIDescriptor(mpmCliDumpByIndex_Descriptor);
	hResult += cliInstallCLIDescriptor(mpmCliStatisticsShow_Descriptor);
#ifdef _MPM_TEST
	hResult += cliInstallCLIDescriptor(mpmCliAlloc_Descriptor);
	hResult += cliInstallCLIDescriptor(mpmCliRelease_Descriptor);
#endif //_MPM_TEST
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing mpm cli tools");
	}
}
#endif //_CLI_TOOL_MPM
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

#endif //_1394CORE


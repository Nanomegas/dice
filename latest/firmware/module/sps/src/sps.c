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
	
	Purpose:	sps.c
  
	Description:       
		Small Persistent System utility
	Revisions:
		created 02/19/2007 zluo

****************************************************************************/
//The initial version will be based on fis, system, but it can be built directly upon 
//flash.h


#ifdef _SPS
#include <cyg/io/flash.h>

#ifdef _FIS_SPS 
#include "fis.h"
static FIS_IMAGE *spsImage; //the container image for sps
#else
#error "Block SPS not implemented yet"
#endif //_FIS_SPS

#include "sps.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "crc16.h"

//sps only support data structure < Blocksize(0x10000) - 4 bytes,

/*
All flash blocks start with a MAGIC number and block size in bytes
All data blocks start with magic number
Initial:
	All blocks are empty and have magic numbers
Boot:
	Confirm blocks formatted, otherwise format
	Find end of flash block, if empty look through for first empty
	If all flash blocks are full, format first block
	if data found copy out advance pointer, otherwise mark as dirty
	Invariant: Pointer always point to next free block
Thread:
	Wait for dirty, if dirty:
		Clear dirty
		Program block, only 1k at a time, wait(10) between, use temp buffer
		Forward pointer
		If not free, format, set pointer


	Any error from the flash system will set the spsFlashBase=NULL and all sps 
	functionality will stop. The last error can be optained with spsGetError.
	
*/

static uint32 spsLength; //this is the word aligned length of the settings
static BOOL bSpsDirty = FALSE; //data has changed, thread will program

static uint8 *spsFlashBase;			//flash base address for sps
static uint32 spsBlock = 2;			//by default it uses 2 blocks
static uint32 spsTicks  = 10000;		//10s waiting time by defaul

typedef struct
{
	uint32 block;
	uint32 id;
} BLOCKID;

static BLOCKID spsNextEmpty;		//spsNextEmpty is the next place to store data.
static int flash_block_size;        //we need flash block info to perform sps
static uint32 setsPerBlock = 0;		//how many settings fit into one block

static HRESULT spsLastError = 0;	//last error from flash operations
static void * spsSetting;

#define BLKADDR(blk,id) (spsFlashBase+(blk)*flash_block_size+sizeof(sps_format)+(id)*(spsLength+sizeof(uint32)))
#define BLKSTARTADDR(blk) (spsFlashBase+(blk)*flash_block_size)

static uint8 prgbuffer[1024];

typedef struct
{
	uint32 blockMagic;
	uint32 length;
} sps_format;

static void sps_flash_cb(void)
{
	TCTaskWait(10);
}


void spsChanged (void)
{
	bSpsDirty = TRUE;
}

static int memempty(const uint32 * p, uint32 size)
{
	while (size--) 
	{
		if (*p++!=0xffffffff) return false;
	}
	return true;
}

//program the current setting to flash, do the Magic last so we know that a block with 
//magic is a good block.
//Only do 1k at a time to assure yielding every 7ms.
static bool spsProgramSetting(void *base)
{
	uint8 *err_address;
	int ret;
	uint32 sizeLeft = spsLength;
	uint32 magic = SPS_SET_MAGIC;
	uint8 * psrc = (uint8 *)spsSetting;
	uint32 ofs=0;
	
	while (sizeLeft)
	{
		if (sizeLeft > 1024)
		{
			memcpy(prgbuffer,psrc,1024);
			SYS_DEBUG(SYSDEBUG_TRACE_SPS, "Program block %p, %d\n", ((uint8*)base+sizeof(uint32)+ofs),1024);
			ret = flash_program_with_cb(((uint8*)base+sizeof(uint32)+ofs), (void*)prgbuffer, 1024, (void **)(&err_address), &sps_flash_cb);
			sizeLeft-=1024;
			psrc+=1024;
			ofs+=1024;
		}
		else
		{
			memcpy(prgbuffer,psrc,sizeLeft);
			SYS_DEBUG(SYSDEBUG_TRACE_SPS, "Program block %p, %d\n", ((uint8*)base+sizeof(uint32)+ofs),sizeLeft);
			ret = flash_program_with_cb(((uint8*)base+sizeof(uint32)+ofs), (void*)prgbuffer, sizeLeft, (void **)(&err_address), &sps_flash_cb);
			sizeLeft=0;
		}						
		if (ret != 0) 
		{
			spsLastError = SPS_ERROR_PROGRAM_ERR;
			return false;			
		}
	}
	ret = flash_program_with_cb(base, (void*)&magic, sizeof(magic), (void **)(&err_address), &sps_flash_cb);
	if(ret != 0) {
		spsLastError = SPS_ERROR_PROGRAM_ERR;
		return false;
	}

	return true;
}

//Format a clean flash block
static bool spsFormatBlock(void *base)
{
	char *err_address;
	sps_format format;
	sps_format * pformat;
	int ret;

	format.blockMagic = SPS_BLOCK_MAGIC;
	format.length = spsLength;
	
	//check if already formatted
	pformat = (sps_format*)base;
	if ((pformat->blockMagic == format.blockMagic) && (pformat->length == format.length))
	{
		if (memempty(((uint32 *)base)+2, (flash_block_size/4)-2)) return true;	
	}

	if((ret = flash_erase_with_cb(base, flash_block_size, (void **)(&err_address), sps_flash_cb)) != 0) {
		SYS_DEBUG(SYSDEBUG_TRACE_SPS, "%p, %d, %s\n", err_address, __LINE__, __func__);
		spsLastError = SPS_ERROR_ERASE_ERR;
		return false;
	}
	if((ret = flash_program_with_cb(base, (void*)&format, sizeof(format), (void **)(&err_address), sps_flash_cb)) != 0) {
		SYS_DEBUG(SYSDEBUG_TRACE_SPS, "%p, %d, %s\n", err_address, __LINE__, __func__);
		spsLastError = SPS_ERROR_PROGRAM_ERR;
		return false;
	}
	return true;
}


static bool storeSetting (void)
{
	if (!spsFlashBase) return false;
	
	//we know we can store at sps pointer
	if (!spsProgramSetting(BLKADDR(spsNextEmpty.block,spsNextEmpty.id))) return false;
	
	//forward the pointer
	spsNextEmpty.id++;
	if (spsNextEmpty.id >= setsPerBlock)
	{
		//need to move to a new flash block
		spsNextEmpty.id = 0;
		spsNextEmpty.block = (spsNextEmpty.block+1)%spsBlock;
		//need to format that block
		if (!spsFormatBlock(BLKSTARTADDR(spsNextEmpty.block))) return false;
	}
	return true;	
}






static void checkBlock (void *base, uint32 * pLastGood, uint32 * pFirstEmpty)
{
	uint32 * pw = (uint32 *)base;
	uint32 bid=0,lastGood=setsPerBlock, firstEmpty=setsPerBlock;
	uint32 bwsize = 1+spsLength/sizeof(uint32);
	
	pw+=2;
	while (bid < setsPerBlock)
	{
		if (*pw ==SPS_SET_MAGIC)
		{
			//valid setting
			lastGood= bid;
			firstEmpty = setsPerBlock;
		}
		else if (*pw == 0xffffffff)
		{
			//might be empty or partial
			if (memempty(pw,bwsize))
			{
				if (firstEmpty == setsPerBlock) firstEmpty = bid;
			}
			else
			{
			//message, something went wrong
			}
		}
		else
		{
			//something went wrong, let's skip
			firstEmpty = setsPerBlock;
		}
		pw+=bwsize;
		bid++;
	}
	*pLastGood = lastGood;
	*pFirstEmpty = firstEmpty;
}	
	

static bool findSetting (void)
{
	int i;
	sps_format * pformat;
	//first check that the format is correct and format if not
	if (!spsFlashBase) return false; //initialization failed
	char *base = spsFlashBase;
	
	
	for (i = 0, base = spsFlashBase; i < spsBlock; i++, base += flash_block_size) 
	{
		pformat = (sps_format *)base;
		if ((pformat->blockMagic != SPS_BLOCK_MAGIC) || (pformat->length != spsLength))
		{
			SYS_DEBUG(SYSDEBUG_TRACE_SPS, "Format Block at %p.\n",base);
			if (!spsFormatBlock(base)) return false;
		}
	}
	//now all blocks are formatted, find the latest setting
	//Ideally there will be a number of full blocks and a partially full or empty block after.
	//In case all blocks are empty, there is no current setting
	//In case all blocks are full, something went wrong when storing the last setting
	uint32 lastGood,firstEmpty;		
	uint32 goodId=setsPerBlock;
	uint32 goodBlock=spsBlock;
	uint32 freeId=setsPerBlock;
	uint32 freeBlock=spsBlock;
	

	for (i = 0, base = spsFlashBase; i < spsBlock; i++, base += flash_block_size) 
	{		
		checkBlock (base, &lastGood, &firstEmpty);
		SYS_DEBUG(SYSDEBUG_TRACE_SPS, "Check block at %p return %d,%d.\n",base,lastGood,firstEmpty);
		if ((lastGood != setsPerBlock) && (firstEmpty != setsPerBlock))
		{
			//we found it, there is a good setting and some free space
			goodId=lastGood;
			goodBlock=i;
			freeId=firstEmpty;
			freeBlock=i;			
			break;
		}
		else if (lastGood != setsPerBlock)
		{
			//we have a good setting, but will only use it of there is none in the next
			//we don't know the next free yet
			goodId=lastGood;
			goodBlock=i;
		}
		else if (firstEmpty != setsPerBlock)
		{
			//we have an empty place, if the last block was good we have a winner
			if (goodBlock!=spsBlock)
			{
				freeId=firstEmpty;
				freeBlock=i;			
				break;
			}
		}
	}
	char * pGoodSetting = NULL;
	
	SYS_DEBUG(SYSDEBUG_TRACE_SPS, "Done check good=%d,%d  free=%d,%d\n",goodBlock,goodId,freeBlock,freeId);
	
	//we can have two unresolved situations
	//1) There were no good blocks, just pick the first block and erase it
	//2) There were no empty block after a good, it could be the last block is good
	//   and the first is empty, let's assumne that
	if (goodBlock==spsBlock)
	{
		//we have no good settings
		//let's format the first block and set the pointer
		if (!spsFormatBlock(spsFlashBase)) return false;
		//next position to write to
		spsNextEmpty.block = 0;
		spsNextEmpty.id = 0;
	}
	else if (freeBlock==spsBlock)
	{
		//Let's update the good setting pointer
		pGoodSetting = BLKADDR(goodBlock,goodId);
		//let's format the first block and set the pointer
		if (!spsFormatBlock(spsFlashBase)) return false;
		//next position to write to			
		spsNextEmpty.block = 0;
		spsNextEmpty.id = 0;		
	}
	else
	{
		pGoodSetting = BLKADDR(goodBlock,goodId);
		spsNextEmpty.block = freeBlock;
		spsNextEmpty.id = freeId;
	}
	//if we have a good setting copy it out
	if (pGoodSetting)
	{
		memcpy(spsSetting, pGoodSetting+sizeof(uint32), spsLength);		
	} 
	else
		return storeSetting ();
	
	
	return true;	
}



static void spsThread (void *dummy)
{
	DO_FOREVER
	{
		if(bSpsDirty) {
			bSpsDirty = false;
			if (!storeSetting ()) spsFlashBase = 0;
		}

		TCTaskWait(spsTicks);
	}
}

HRESULT spsGetError ()
{
	return spsLastError;
}



// task (thread) ID for this module									
uint32						spsThreadID = 0;
const char*					spsThreadName = "spsThread";

HRESULT spsInitialize (char * name, uint8 nbSectors, uint32 nbData, void * pCurrentSetting, uint16 updateTicks)
{
	int		index;
	HRESULT result = NO_ERROR;

	
	if(updateTicks) 
		spsTicks = updateTicks;

	spsSetting = pCurrentSetting;
	spsLength = ((nbData+3)/4)*4; //round it up to 4 byte boundry
	if(nbData % 4 != 0) {
		sysDebugPrintf("Warning: the setting size = %d %d.is not round up to 4 byte boundry. \n", nbData, spsLength);
	}
	else
	{	
		SYS_DEBUG(SYSDEBUG_TRACE_SPS, "The setting size = %d. \n", spsLength);
	}
	spsBlock = nbSectors;

	flash_get_block_info(&flash_block_size, &index); //index is a dummy, we are not interested in number of flash blocks

	setsPerBlock = (flash_block_size-sizeof(sps_format))/(spsLength+sizeof(uint32));
	
	SYS_DEBUG(SYSDEBUG_TRACE_SPS, "setsPerBlock %d\n", setsPerBlock);
	
	SYS_DEBUG(SYSDEBUG_TRACE_SPS, "flash block size = 0x%x, %d %d\n", flash_block_size, index, sizeof(CYG_ADDRESS));

	/////////////////////spsFlashBase=NULL;return NO_ERROR;
	
#ifdef _FIS_SPS
	spsImage = fis_lookup(name, &index);
	if (spsImage && (spsImage->size/flash_block_size != nbSectors))
	{
		//wrong size, we need to delete
		SYS_DEBUG(SYSDEBUG_TRACE_SPS, "Wrong sps size is found, deleting old.\n");
		fis_delete(name);	
		spsImage = NULL;
	}
	if(spsImage == NULL)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_SPS, "No sps is found, creating the container fis image.\n");
		fis_create(0xffffffff, nbSectors*flash_block_size, 0, 0, name);
		spsImage = fis_lookup(name, &index);
		if(spsImage == NULL) {
			sysDebugPrintf("Panic, no flash space available.\n");
			spsFlashBase = NULL;
			return E_GEN;
		}
	}
	spsFlashBase = (uint8 *)spsImage->flash_base;
#else
#error "Block SPS not implemented yet"
#endif
	SYS_DEBUG(SYSDEBUG_TRACE_SPS, "No we got the flash base=%p.\n",spsFlashBase);
	if (!findSetting ()) spsFlashBase = 0;

	result = TCTaskCreate(&spsThreadID, spsThreadName, spsThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (result != NO_ERROR) return result;
	TCTaskResume(spsThreadID);
	return result;
}


#endif //_SPS

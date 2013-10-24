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
 * DESCRIPTION: FrmwLoad

	Purpose:	Remote firmware loader
				

	Revisions:
		created 20/03/2005 ml

****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "TCDebug.h"
#include <cyg/io/flash.h>
#include "fis.h"
#include "lal.h"
#include "frmwLoad.h"
#include "frmwLoadInterface.h"
#include "version.h"

#ifdef _CLI
#include "cliSIO.h"
#endif

#ifdef _FRMWLOAD

static char 	moduleName[] = __FILE__ ;

static FRMW_CALLBACK frmwLoadResetCB;


// task (thread) ID for this module
uint32				frmwLoadThreadID;
const char*			frmwLoadThreadName = "frmwLoadThread";
uint32				frmwLoadSemID;



static FL_1394MEMORY flDriverMemory;

#define FL_DRIVER_OFFSET(a)		((uint32)(&flDriverMemory.a) - (uint32)&flDriverMemory)




//we need some memory to dump our image into. for now we will use the space from the end of BSS
extern uint8 * __bss_end;

static uint8 *	bufferBase;
static uint32   bufferMaxLen;

extern uint32 serialNo;

static bool CheckForWrite(uint32 RegionOffSet, uint32 payload)
{
	bool	Result = false;

	if((RegionOffSet == FL_DRIVER_OFFSET(opcode)) && (payload == 4))
		Result = true;
	else if ((RegionOffSet >= FL_DRIVER_OFFSET(data)) && ((RegionOffSet + payload) <= FL_DRIVER_OFFSET(endOfData)))
		Result = true;
	else if ((RegionOffSet == FL_DRIVER_OFFSET(testDelay)) && (payload==4))
		Result = true;
	else
		Result = false;
#ifdef _SYSDEBUG	
	if (Result == false) sysDebugPrintf("frmwLoad, illegal write command!\n\r");
#endif // _SYSDEBUG
	return Result;
}
//
// flDriverCallback
//
static HRESULT flDriverCallback(PB *packetBlock)
{
	HRESULT hResult = NO_ERROR;
	PB_HEADER		*pHeader = NULL;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	OFFSET_1394		OffSetDest;
	uint32			payloadSize = 0;
	uint32			RegionOffSet = 0;	
	QUADLET			*pPayload = NULL;

	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketType(packetBlock,&packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetDestinationOffset(packetBlock, &OffSetDest);
	if (hResult != NO_ERROR) return hResult;

	RegionOffSet = OffSetDest.Low - FL_DRIVER_BASE_START;	

	hResult = pbGetDataLen(packetBlock,&payloadSize);
	if(hResult != NO_ERROR) 
		return hResult;	

	if (packetType == PB_TYPE_WRITE_REQUEST_QUADLET)
	{
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		payloadSize = 4;

		if(CheckForWrite(RegionOffSet, payloadSize))
		{
			if( RegionOffSet == FL_DRIVER_OFFSET(opcode))
			{
				memcpy((uint32*) ((int)&flDriverMemory + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				TCSemaphoreSignal(frmwLoadSemID);
			}
			else
			{
				memcpy((uint32*) ((int)&flDriverMemory + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);				
			}			
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);			
		}		
	}
	else if (packetType == PB_TYPE_WRITE_REQUEST)
	{
		hResult = pbGetPayload(packetBlock, (void **) &pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;

		if(CheckForWrite(RegionOffSet, payloadSize))
		{			
			memcpy((uint32*) ((int)&flDriverMemory + (int)RegionOffSet), pPayload, payloadSize);
			hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
	}
	else if (packetType == PB_TYPE_READ_REQUEST)
	{
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		
		if (RegionOffSet == FL_DRIVER_OFFSET(testbuf[0]))
		{
			if (flDriverMemory.testDelay) TCTaskWait(flDriverMemory.testDelay);
		}
			
		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&flDriverMemory + (int)RegionOffSet), TRUE);
	}
	else if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
	{
		payloadSize = 4;
		if (RegionOffSet == FL_DRIVER_OFFSET(testbuf[0]))
		{
			if (flDriverMemory.testDelay) TCTaskWait(flDriverMemory.testDelay);
		}

		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&flDriverMemory + (int)RegionOffSet), TRUE);
	}
	else
	{
		hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
	}
	return hResult;
}
//
// getCompileFlags
//
static HRESULT getCompileConfigFlags (uint32 *puiConfigFlags)
{
	*puiConfigFlags = 0;


// Driver
// _MLAN does not exist any more in the new model, please note that this way of reporting
// the driver model to the firmware loader is not prefered. The unit directory should be
// examined for driver model.

#ifdef _DICE_DRIVER
	*puiConfigFlags |= CFG_DICE_DRIVER;
#elif defined _AVC
	*puiConfigFlags |= CFG_AVC_DRIVER;
#endif
	
	return 0;
}
//
// getImageDesc
//
static HRESULT getImageDesc (void)
{
	HRESULT hResult = E_GEN_NOMATCH;
	uint32	fisidx, idx;
	FIS_IMAGE imageDesc;
	
	// check parameters
	idx = flDriverMemory.data.parameters.getImage.imageId;
	fisidx = FIS_INIT_GET_IMAGE;
	while (1)
	{
		hResult = fis_get_next_image(&imageDesc, &fisidx);
		if (hResult != NO_ERROR) break; //no more images
		if (!idx) break; // found the right image
		idx--; // try one more time			
	}	
	if (hResult == NO_ERROR)
	{
		strncpy (flDriverMemory.data.rtn.getImage.name, imageDesc.name, 16);
		flDriverMemory.data.rtn.getImage.flashBase = imageDesc.flash_base;
		flDriverMemory.data.rtn.getImage.memBase = imageDesc.mem_base;
		flDriverMemory.data.rtn.getImage.size = imageDesc.size;
		flDriverMemory.data.rtn.getImage.entryPoint = imageDesc.entry_point;
		flDriverMemory.data.rtn.getImage.length = imageDesc.data_length;
		flDriverMemory.data.rtn.getImage.chkSum = imageDesc.desc_cksum;

		flDriverMemory.data.rtn.getImage.uiBoardSerialNumber = serialNo;
		flDriverMemory.data.rtn.getImage.uiVersionHigh = diceApp_major_rev();
		flDriverMemory.data.rtn.getImage.uiVersionLow = diceApp_minor_rev();
		getCompileConfigFlags(&flDriverMemory.data.rtn.getImage.uiConfigurationFlags);
				
		strcpy(flDriverMemory.data.rtn.getImage.BuildTime, diceApp_time());
		strcpy(flDriverMemory.data.rtn.getImage.BuildDate, diceApp_date());
	}
	return hResult;
}
//
// getRunningImageVendorDesc
//

static HRESULT getRunningImageVendorDesc (void)
{
	HRESULT hResult = NO_ERROR;

	flDriverMemory.data.rtn.getImageVInfo.uiVProductID = diceApp_product_id();
	strncpy(flDriverMemory.data.rtn.getImageVInfo.uiVendorID, diceApp_vendor_id(), 7);
	flDriverMemory.data.rtn.getImageVInfo.uiVMajor = diceApp_vendor_major_rev();
	flDriverMemory.data.rtn.getImageVInfo.uiVMinor = diceApp_vendor_minor_rev();
	flDriverMemory.data.rtn.getImageVInfo.user1 = 0;
	flDriverMemory.data.rtn.getImageVInfo.user2 = 0;

	return hResult;
}

#ifndef _USE_OLD_VERSION_INFO

static HRESULT getAppInfo (void)
{
	HRESULT hResult = NO_ERROR;
	flDriverMemory.data.rtn.GetAppInfo.uiBaseSDKVersion = diceApp_full_rev();
	flDriverMemory.data.rtn.GetAppInfo.uiApplicationVersion = diceApp_vendor_full_rev();
	flDriverMemory.data.rtn.GetAppInfo.uiVendorID = diceApp_vendor_id_num();
	flDriverMemory.data.rtn.GetAppInfo.uiProductID = diceApp_product_id();
	strncpy (flDriverMemory.data.rtn.GetAppInfo.BuildTime,diceApp_time(),64);
	strncpy (flDriverMemory.data.rtn.GetAppInfo.BuildDate,diceApp_date(),64);
	flDriverMemory.data.rtn.GetAppInfo.uiBoardSerialNumber = serialNo;

	return hResult;
}

#endif

static HRESULT progressProc(uint32 promax, uint32 procurr)
{
	if (promax > 0xfff) promax=0xfff;
	if (procurr > 0xfff) procurr=0xfff;
	flDriverMemory.progress = FL_PRGS_FLAG | (3<<FL_PRGS_TOUT_SHFT) | (promax<<FL_PRGS_MAX_SHFT) | (procurr<<FL_PRGS_CURR_SHFT);
	return NO_ERROR;
}

//
// deleateImage
//	
static HRESULT deleteImage (void)
{
#ifdef USE_OLD_FRMWLOAD
	return fis_delete(flDriverMemory.data.parameters.deleteImage.name);
#else
	return fis_delete_progress(flDriverMemory.data.parameters.deleteImage.name, progressProc);
#endif
}
//
// createImage
//
static HRESULT createImage (void)
{
#ifdef USE_OLD_FRMWLOAD
	return fis_create((uint32)bufferBase, flDriverMemory.data.parameters.createImage.length, 
																				flDriverMemory.data.parameters.createImage.execAddr, 
																				flDriverMemory.data.parameters.createImage.entryAddr,
																				flDriverMemory.data.parameters.createImage.name);
#else
	return fis_create_progress((uint32)bufferBase,  flDriverMemory.data.parameters.createImage.length, 
																									flDriverMemory.data.parameters.createImage.execAddr, 
																									flDriverMemory.data.parameters.createImage.entryAddr,
																									flDriverMemory.data.parameters.createImage.name,
																									TRUE, progressProc);
#endif

}
//
// upload
//
static HRESULT upload (void)
{	
	if (flDriverMemory.data.parameters.upload.index + flDriverMemory.data.parameters.upload.length > bufferMaxLen)
	{
		return E_DICE_BAD_INPUT_PARAM;
	}
	memcpy (bufferBase + flDriverMemory.data.parameters.upload.index, flDriverMemory.data.parameters.upload.buffer, flDriverMemory.data.parameters.upload.length);
	return NO_ERROR;	 
}
//
// uploadStat
//
static HRESULT uploadStat (void)
{
	uint32 i;
	uint32  chksum = 0;
	uint8  * pb = bufferBase;
	
	i = flDriverMemory.data.parameters.uploadStat.length;
	while (i--)
	{
		chksum += *pb++;
	}
	flDriverMemory.data.rtn.uploadStat.checksum = chksum;
	return NO_ERROR;
}
//
// GetFlashInfo
//
static HRESULT GetFlashInfo (void)
{
	void *pvFlashStart, *pvFlashEnd;
	int iFlashBlockSize, iNumFlashBlocks; 
	
	/*
	flash_get_limits((void *)0, (void **)&flash_start, (void **)&flash_end);

	// Keep 'end' address as last valid location, to avoid wrap around problems
	flash_end = (void *)((CYG_ADDRESS)flash_end - 1);
	flash_get_block_info(&flash_block_size, &flash_num_blocks);
	*/
	
	flash_get_limits((void *)0, &pvFlashStart, &pvFlashEnd);
	flash_get_block_info(&iFlashBlockSize, &iNumFlashBlocks);

	flDriverMemory.data.rtn.FlashInfo.uiStartAddress = (uint32)pvFlashStart;
	flDriverMemory.data.rtn.FlashInfo.uiEndAddress = (uint32)pvFlashEnd;
	flDriverMemory.data.rtn.FlashInfo.uiBlockSize = iFlashBlockSize;
	flDriverMemory.data.rtn.FlashInfo.uiNumBlocks = iNumFlashBlocks;
	
	return NO_ERROR;	
}
//
// ReadMemory
//
static HRESULT ReadMemory (void)
{
	if (flDriverMemory.data.parameters.ReadMemory.uiLen > 4000)
	{
		return E_DICE_BAD_INPUT_PARAM;
	}
	
	memcpy (flDriverMemory.data.rtn.ReadMemory.ReadBuffer, 
		(void *)flDriverMemory.data.parameters.ReadMemory.uiStartAddress, 
		flDriverMemory.data.parameters.ReadMemory.uiLen);
	
	return NO_ERROR;	
}
//
// ececuteOpcode
//
static void ececuteOpcode(void)
{	
	FL_OP_ENUM opcode;
	if (flDriverMemory.opcode & FL_OP_EXECUTE)
	{
		opcode = flDriverMemory.opcode & FL_OP_MASK;
		switch (opcode)
		{
			case FL_OP_GET_IMAGE_DESC:
				flDriverMemory.error = getImageDesc ();
				break;
				
			case FL_OP_GET_RUNNING_IMAGE_VINFO:
				flDriverMemory.error = getRunningImageVendorDesc ();
				break;
				
			case FL_OP_DELETE_IMAGE:
				flDriverMemory.error = deleteImage ();
				break;
				
			case FL_OP_CREATE_IMAGE:
				flDriverMemory.error = createImage ();
				break;
				
			case FL_OP_UPLOAD:
				flDriverMemory.error = upload ();
				break;
				
			case FL_OP_UPLOAD_STAT:
				flDriverMemory.error = uploadStat ();
				break;
								
			case FL_OP_RESET_IMAGE:
				flDriverMemory.error = frmwLoad_RESET();
				break;
				
			case FL_OP_TEST_ACTION:
				flDriverMemory.error = hostTestRequest();
				break;

			case FL_OP_GET_FLASH_INFO:
				flDriverMemory.error = GetFlashInfo();
				break;
				
			case FL_OP_READ_MEMORY:
				flDriverMemory.error = ReadMemory();
				break;
				
			case FL_OP_NOOP:
				flDriverMemory.error = NO_ERROR;
				break;
#ifndef _USE_OLD_VERSION_INFO
			case FL_OP_GET_APP_INFO:
				flDriverMemory.error = getAppInfo();
				break;
#endif
			default:
				flDriverMemory.error = E_GEN_ILLEGAL_PARAM;
				break;
		}
		// clear execute bit
		flDriverMemory.opcode &= ~FL_OP_EXECUTE;
		flDriverMemory.progress = 0;
	}
}


static void flDriverThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(frmwLoadSemID);
		if (hResult == NO_ERROR) 
		{
			ececuteOpcode();	
		}				
	}
}

static HRESULT frmwLoadRegisterAddressCallbacks(void)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394  frmwLoadAddress, frmwLoadAddressHi;
	
	frmwLoadAddress.High = FL_DRIVER_BASE_START_HI;  
	frmwLoadAddress.Low  = FL_DRIVER_BASE_START;  

	frmwLoadAddressHi.High = FL_DRIVER_BASE_END_HI;  
	frmwLoadAddressHi.Low  = FL_DRIVER_BASE_END;

	hResult = lalAddressRangeRegisterCB(&flDriverCallback, frmwLoadAddress,frmwLoadAddressHi);

	return hResult;
}

HRESULT frmwLoadInitialize(void)
{
	HRESULT	hResult = NO_ERROR;
	
	//frmwLoad Initialization
	bufferMaxLen = 0x100000;	//max 1M buffer size.
	bufferBase = (uint8 *)malloc(bufferMaxLen);
    if (!bufferBase)
    {
		sysLogError(E_FIS_MEMORY_ERROR, __LINE__, moduleName);
	}
#ifdef USE_OLD_FRMWLOAD
		flDriverMemory.capabilities = 0;
#else
		flDriverMemory.capabilities = FL_CAP_AUTOERASE | FL_CAP_PROGRESS;
#endif
	//Create Semaphore
	hResult = TCSemaphoreOpen(&frmwLoadSemID, 0);
	if (hResult != NO_ERROR) return hResult;
	
	//Resume the task
	hResult = frmwLoadResumeTasks ();
	if (hResult != NO_ERROR) return hResult;

	//register the address range
	hResult = frmwLoadRegisterAddressCallbacks();
	if (hResult != NO_ERROR) return hResult;
	
	hResult = frmwLoadSetResetCB(frmwLoadStartBoot);

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/	
HRESULT frmwLoadResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskResume(frmwLoadThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT frmwLoadSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskSuspend(frmwLoadThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT frmwLoadInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskCreate(&frmwLoadThreadID, frmwLoadThreadName, flDriverThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT frmwLoadStartBoot(void)
{
	// S/W Reset Power manager

	*((volatile uint32 *) PWRMGRBASE) = 0xF;
	
	TCTaskWait(500); // Reset 0.5 sec.

	return E_FAIL;
}

HRESULT	frmwLoadSetResetCB(FRMW_CALLBACK resetCB)
{
	HRESULT	hResult = NO_ERROR;

	frmwLoadResetCB = resetCB;

	return hResult;
}

HRESULT	frmwLoad_RESET(void)
{
	HRESULT	hResult = NO_ERROR;
	
	sysDebugPrintf("\n\nSoft-reset requested by Host...\n\n");

	if (frmwLoadResetCB != NULL)
	{
		hResult = (frmwLoadResetCB)();
	}

	hResult = E_FAIL;

	return hResult;
}

#ifdef _FTM
#include "avsInt.h"
extern void avsDumpEventCounters(uint32*data);
extern HRESULT myModeSetMode (uint32 mode);
extern HRESULT myModeBypassMidi (uint32 bOn);
extern HRESULT dalGetAVSPhaseOffset (uint32 rxId, int32 * pPhase10us, int8 * pPhasePercent);
#ifdef _AVSTRIG
extern void targetEnableAVSTrigger (void);
extern void targetSignalAVSTrigger (void);
#endif
#endif //_FTM

HRESULT hostTestRequest(void)
{
	HRESULT hr = NO_ERROR;

#ifdef _FTM
	switch(flDriverMemory.data.parameters.testParam.cmdID) {
		case FL_TEST_CMD_POKE:// set value
			*((uint32 *)flDriverMemory.data.parameters.testParam.lvalue0) = flDriverMemory.data.parameters.testParam.lvalue1;
			break;
		case FL_TEST_CMD_PEEK: //get value
			flDriverMemory.data.rtn.testRtn.data[0] = *((uint32 *)flDriverMemory.data.parameters.testParam.lvalue0);
			break;
		case FL_TEST_CMD_GET_AVS_CNT: //get avs stats;
#ifdef _CLI
			avsDumpEventCounters((void *) (&flDriverMemory.data.rtn.testRtn.data[0]));
#endif // CLI_
			break;
		case FL_TEST_CMD_CLR_AVS_CNT: //clear avs stats
			avsIntClearCounters();
			break;
		case FL_TEST_CMD_SET_MODE: //Change myMode
			myModeSetMode ((uint32)flDriverMemory.data.parameters.testParam.lvalue0);
			break;
			
		case FL_TEST_CMD_SET_MIDIBP: //Change midi passthru
			myModeBypassMidi ((uint32)flDriverMemory.data.parameters.testParam.lvalue0);
			break;

		case FL_TEST_CMD_GET_AVS_PHASE: //Ask for AVS1/2 phase offset
			{
				int32 phase10us;
				int8  phasePercent;
				dalGetAVSPhaseOffset ((uint32)flDriverMemory.data.parameters.testParam.lvalue0, &phase10us, &phasePercent);
				flDriverMemory.data.rtn.testRtn.data[0] = (uint32)phase10us;
				flDriverMemory.data.rtn.testRtn.data[1] = (uint32)((int)phasePercent);
			}
			break;

#ifdef _AVSTRIG		
		case FL_TEST_CMD_ENABLE_BNC_SYNC:
			targetEnableAVSTrigger ();
			break;

		case FL_TEST_CMD_PULSE_BNC_SYNC:
			targetSignalAVSTrigger ();
			break;

#endif //_AVSTRIG

		case FL_TEST_CMD_EMUL_SLOW_CMD:
			//test slow response, value == ms to wait.
			{
				uint32 start = TCTimeGet();
				while (TCTimeElapsed(start)<flDriverMemory.data.parameters.testParam.lvalue0);			
			}
			break;
		default:
			hr = E_FAIL;
			break;

	}
#endif //_FTM

	return hr;
}

#endif //_FRMWLOAD

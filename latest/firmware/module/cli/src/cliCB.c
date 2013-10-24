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
   * DESCRIPTION: Command Line Interface - CB (for remote CLI Calls)

	Purpose: 

		This file describes the Command Line Interpreter (cli) CB
	
  Revisions:
		created 22/10/2002 lm
****************************************************************************/

#ifdef _CLI
#ifdef _CLI_CB

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "ErrorCodes.h"
#include "lal.h"
#include "nciHandles.h"
#include "csr.h"
#include "cli.h"
#include "cliCB.h"
#include "cliCBCli.h"
#include "cliSIO.h"
#include "format.h"
#include "packetBlock.h"

const OFFSET_1394 cliDestOffsetHi = {CLI_CB_BASE_END, CLI_CB_BASE_HI};
const OFFSET_1394 cliDestOffsetLo = {CLI_CB_BASE_START, CLI_CB_BASE_HI};

// Global variables used to redirect CLI output to the 1394 interface

BOOL	cliCBSysLogErrorEnabled = FALSE;				// This instructs cliCBSysErrorLogItem to collect a sysLogError
uint32	cliCBSysLogErrorStrLen = 0;						// This is the length of a sysLogError text string
char	cliCBSysLogErrorBuffer[CLI_MAX_MSG_SIZE];		// 
uint32	cliCBVerbose = FALSE;							// This specifies if output printf during cli commands is enabled
uint32	cliCBDeferred = FALSE;							// This specifies if command should be deferred to task

uint32	cliCBRespCount = 0;
uint32	cliCBController[2] = {0xFFFF, 0};
uint32	cliCBResponseBuffer[(CLI_MAX_MSG_SIZE+3)/4];	// We copy responses to here - we may need to byte swap them...

uint32	cliCBResult = 0;
uint32	cliCBStatus = 0;

#define _CLICB_DEFERRED_TASK	
//#undef _CLICB_DEFERRED_TASK	

#ifdef _CLICB_DEFERRED_TASK	
// task (thread) IDs for this module
uint32					cliCBThreadID = 0;				// thread cli CB deferred task
const char*				cliCBThreadName = "cliCBThread";

uint32					cliCBDeferredQueueID;			// cli CB deferred handler thread message queue
																		
#define CLICB_DEFERRED_ITEMS			16
#define CLICB_DEFERRED_QUEUE_SIZE		(CLICB_DEFERRED_ITEMS * 4)

static BYTE cliCBDeferredEventQueueMemory[CLICB_DEFERRED_QUEUE_SIZE];	// the queue memory space

typedef struct				
{							
	char					cmdStr[kMaxCommandLine];
	BOOL					allocated;
} CLICB_CMD;

typedef struct				
{							
	CLICB_CMD				cmd[CLICB_DEFERRED_ITEMS];
	uint32					ptr;
} CLICB_DEFERRED_CMD;

static CLICB_DEFERRED_CMD	cliCBDeferredCmd;

static HRESULT cliCBAllocateDeferredCmd(CLICB_CMD **cmd, char *cliCommandStr)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		ptr;
	BOOL		bFound = FALSE;
	
	if (strlen(cliCommandStr) > kMaxCommandLine - 1)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	ptr = cliCBDeferredCmd.ptr;

	for (i = 0; i < CLICB_DEFERRED_ITEMS; i++)
	{
		if (cliCBDeferredCmd.cmd[ptr].allocated == FALSE)
		{
			cliCBDeferredCmd.cmd[ptr].allocated = TRUE;
			strcpy(cliCBDeferredCmd.cmd[ptr].cmdStr, cliCommandStr);
			*cmd = &cliCBDeferredCmd.cmd[ptr];
			bFound = TRUE;
		}

		ptr++;
		if (ptr > CLICB_DEFERRED_ITEMS - 1)
		{
			ptr = 0;
		}
		if (bFound == TRUE)
		{
			cliCBDeferredCmd.ptr = ptr;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_QUEUE_FULL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

static HRESULT cliCBFreeDeferredCmd(CLICB_CMD *cmd)
{
	HRESULT		hResult = NO_ERROR;
	
	if (cmd->allocated == FALSE)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "cliCBFreeDeferredCmd: freeing something already free\n\r");
	}

	cmd->allocated = FALSE;

	return hResult;
}
#endif //_CLICB_DEFERRED_TASK

HRESULT	cliCBNodeVerbose(BOOL bVerbose)
{
	HRESULT		hResult = NO_ERROR;

	cliCBVerbose = bVerbose;

	return hResult;
}

HRESULT	cliCBNodeDeferred(BOOL bDeferred)
{
	HRESULT		hResult = NO_ERROR;

	cliCBDeferred = bDeferred;

	return hResult;
}

HRESULT cliCBControllerReg(PB * incomingPacket, PB_PACKETTYPE packetType, RCODE_1394 *errorResponse)
// Handle the CLI Controller register, which is two quads long
// It allows CompareSwap locks only to those two quads.
// The controller register contains the bus address of the device
// controlling this device. The lower bits specify an address on that
// device to inform it of the completion of a command.

{
	HRESULT			hResult = E_FAIL;
	PB_LOCKTYPE		lockType;
	uint32			payloadSize = 0;
	uint32*			pPayload = NULL;

	*errorResponse = RSP_TYPE_ERROR;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "Controller Register\n\r");

	if ((packetType == PB_TYPE_READ_REQUEST_QUADLET) || (packetType == PB_TYPE_READ_REQUEST))
	{
		if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
		{
			payloadSize = 4;
		} 
		else
		{
			hResult = pbGetDataLen(incomingPacket,&payloadSize);
			if (hResult != NO_ERROR) return hResult;
		}
		if (payloadSize > 8)
		{
			*errorResponse = RSP_DATA_ERROR;
		} 
		else
		{
			SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "\tRead request, %u bytes\n\r", payloadSize);
		
			hResult = lalReplyReadResponse(incomingPacket, RSP_COMPLETE, (uint16) payloadSize, &(cliCBController[0]), TRUE); // Reply
			*errorResponse = RSP_COMPLETE;
		}
	}

	if (packetType == PB_TYPE_LOCK_REQUEST)
	{
		hResult = pbGetLockType(incomingPacket, &lockType);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(incomingPacket,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetPayload(incomingPacket, (void **) &pPayload);
		if (hResult != NO_ERROR) return hResult;

		SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "\tLock request, %u bytes\n\r", payloadSize);

		if (payloadSize == 16)
		{
			if (lockType == PB_LOCKTYPE_COMPARE_SWAP)
			{
				QUADLET lockArgHi  = pPayload[0];
				QUADLET lockArgLo  = pPayload[1];
				QUADLET lockDataHi = pPayload[2];
				QUADLET lockDataLo = pPayload[3];

				pPayload[0] = cliCBController[0];
				pPayload[1] = cliCBController[1];
				payloadSize /= 2;
			
				if ((lockArgHi == cliCBController[0]  ) && (lockArgLo == cliCBController[1]))
				{
					cliCBController[0] = lockDataHi;
					cliCBController[1] = lockDataLo;
				}
				hResult = lalReplyLockResponse(incomingPacket, RSP_COMPLETE, (uint16)payloadSize, pPayload, TRUE);
				*errorResponse = RSP_COMPLETE;
			}
		}
	}

	return hResult;
}

HRESULT cliCBResponseClearReg(PB * incomingPacket, PB_PACKETTYPE packetType, RCODE_1394 *errorResponse)
// Clear the CLI response area if this address is written. Also
// resets the CLI output pointer to tbe beginning of the output buffer
// FYI: Seems the HAL driver insists on sending 4 byte Write BLOCK commands
// instead of Write Quadlet commands, so I decided to handle that case here too.
{
	HRESULT			hResult = E_FAIL;

	*errorResponse = RSP_TYPE_ERROR;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "Response Clear Register\n\r");

	if ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST)) {
		// This is a strobe address. We do something if it is written.
		// We dont care about what is written

		cliCBStatus = 0;
		cliCBRespCount = 0;
#ifdef _CLICB_SYSLOGERROR
		cliCBSysLogErrorStrLen = 0;
#endif //_CLICB_SYSLOGERROR
		hResult = lalReplyWriteResponse(incomingPacket, RSP_COMPLETE, TRUE);
		*errorResponse = RSP_COMPLETE;
	}

	return hResult;
}

HRESULT cliCBResponseCountReg(PB * incomingPacket, PB_PACKETTYPE packetType, RCODE_1394 *errorResponse)
// Return the value of the response count register
// This register contains the length of the command response including the zero
// termination on the response. If the command has not yet completed, it will return
// zero. If a command returns no response, it will return 1 and there is no response.
{
	HRESULT			hResult = E_FAIL;
	
	*errorResponse = RSP_TYPE_ERROR;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "Response Count Register\n\r");

	if (packetType == PB_TYPE_READ_REQUEST_QUADLET) {
		hResult = lalReplyReadResponse(incomingPacket, RSP_COMPLETE, 4, &cliCBRespCount, TRUE);
		*errorResponse = RSP_COMPLETE;
	}

	return hResult;
}

HRESULT cliCBResultReg(PB * incomingPacket, PB_PACKETTYPE packetType, RCODE_1394 *errorResponse)
// Return the result of the last CLI call.
// Only read this when you know the call has completed, otherwise it may
// be a result for a previous call. Zero means no error.
{
	HRESULT			hResult = E_FAIL;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "Result Register\n\r");

	*errorResponse = RSP_TYPE_ERROR;

	if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
	{
		hResult = lalReplyReadResponse(incomingPacket, RSP_COMPLETE, 4, &cliCBResult, TRUE);
		*errorResponse = RSP_COMPLETE;
	}
	else if (packetType == PB_TYPE_READ_REQUEST)
	{
		uint32	payloadSize = 0;
		// include status of command in packet
		hResult = pbGetDataLen(incomingPacket,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		if (payloadSize == 8)
		{
			uint32	data[2];
			data[0] = cliCBStatus;
			data[1] = cliCBResult;
			hResult = lalReplyReadResponse(incomingPacket, RSP_COMPLETE, 8, data, TRUE);
			*errorResponse = RSP_COMPLETE;
		} 
		else
		{
			*errorResponse = RSP_DATA_ERROR;
		}
	}

	return hResult;
}

#ifdef _CLICB_SYSLOGERROR
void cliCBSysErrorLogItem(char *text)
{
	if (cliCBSysLogErrorEnabled == TRUE)
	{
		uint32	len;

		cliCBSysLogErrorEnabled = FALSE;			// collect the first sysLogError

		len = strlen(text);
		if (len < CLI_MAX_MSG_SIZE - 1)
		{
			memcpy(cliCBSysLogErrorBuffer, text, len);
			cliCBSysLogErrorBuffer[len] = '\0'; // Make sure we are always zero terminated
			cliCBSysLogErrorStrLen = len;
		}
	}
}
#endif //_CLICB_SYSLOGERROR

HRESULT cliCBResponseBufferCB(PB * incomingPacket, PB_PACKETTYPE packetType, RCODE_1394 *errorResponse)
// Called when the other node wants to read the result of an operation
{
	HRESULT			hResult = E_FAIL;
	uint32			payloadSize = 0;
	OFFSET_1394		offsetDest;
	uint32			regionOffset;

	*errorResponse = RSP_TYPE_ERROR;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "Controller Response Buffer\n\r");

	hResult = pbGetDestinationOffset(incomingPacket, &offsetDest);
	if (hResult != NO_ERROR) return hResult;

	regionOffset = offsetDest.Low - CLI_CB_BASE_START - CLI_RESPONSE_AREA;

	if ((regionOffset+payloadSize) > CLI_MAX_MSG_SIZE) {
		*errorResponse = RSP_ADDRESS_ERROR;
	} else {
		if ((packetType == PB_TYPE_READ_REQUEST_QUADLET) || (packetType == PB_TYPE_READ_REQUEST))
		{

			if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
			{
				payloadSize = 4;
			} else {
				hResult = pbGetDataLen(incomingPacket,&payloadSize);
				if (hResult != NO_ERROR) return hResult;
			}

			if (payloadSize >= CLI_MAX_MSG_SIZE) {	// Shouldnt be possible to trigger this, but anyway...
				*errorResponse = RSP_DATA_ERROR;
			} else {
				memcpy(cliCBResponseBuffer, cliCBSysLogErrorBuffer, payloadSize);	// Copy the response data
				formatSwapStrBytes((char *)cliCBResponseBuffer, payloadSize);			// and endian swap if needed
				hResult = lalReplyReadResponse(incomingPacket, RSP_COMPLETE, (uint16) payloadSize, &(cliCBResponseBuffer[0]), TRUE); // Reply
				*errorResponse = RSP_COMPLETE;
			}
		}
	}

	return hResult;
}

HRESULT cliCBCallArea(PB * incomingPacket, PB_PACKETTYPE packetType, RCODE_1394 *errorResponse)
// Called when a CLI remote call arrives at the CLI_CALL_AREA for the new CLI interface
// Only block writes are allowed to this register
{
	HRESULT			hResult = NO_ERROR;
	uint32			payloadSize = 0;
	uint32*			pPayload = NULL;
	char			cliCommandStr[kTempBufferSize] = {0};

	*errorResponse = RSP_TYPE_ERROR;

	hResult = pbGetPayloadSize(incomingPacket,&payloadSize);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(incomingPacket, (void **) &pPayload);
	if (hResult != NO_ERROR) return hResult;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "CLI command call: ");
	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "packetType: %i, ", packetType);
	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "payloadSize: %i", payloadSize);
	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "\n\r");

	if ((packetType == PB_TYPE_WRITE_REQUEST) && payloadSize > 4)
	{
		// Get the command and execute it
		if (payloadSize <= kTempBufferSize)
		{
			memcpy(cliCommandStr, pPayload, payloadSize);
			formatSwapStrBytes(cliCommandStr, payloadSize);

			SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "CLI command call: ");
			SYS_DEBUG(SYSDEBUG_TRACE_CLICB, cliCommandStr);
			SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "\n\r");

#ifdef _CLICB_DEFERRED_TASK	
			if (cliCBDeferred == TRUE)
			{
				CLICB_CMD*	cmd = NULL;

				hResult = lalReplyWriteResponse(incomingPacket, RSP_COMPLETE, TRUE);
				*errorResponse = RSP_COMPLETE;

				SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "CLI command call: deferring to cli CB task \n\r");
				
				hResult = cliCBAllocateDeferredCmd(&cmd, cliCommandStr);
				if (hResult != NO_ERROR) return hResult;

				// thread cli CB deferred task message queue
				hResult = TCMsgQueueSend(cliCBDeferredQueueID, (void *) &cmd, TC_NO_WAIT);
				if (hResult != NO_ERROR)
				{
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			else
#endif //_CLICB_DEFERRED_TASK
			{
				cliCBResult = cliCBHandleCommand(cliCommandStr);
				if (cliCBResult == NO_ERROR)
				{
					hResult = lalReplyWriteResponse(incomingPacket, RSP_COMPLETE, TRUE);
					*errorResponse = RSP_COMPLETE;
				}
				else
				{
					*errorResponse = RSP_DATA_ERROR;
				}
			}
		}
		else
		{
			*errorResponse = RSP_DATA_ERROR;
		}
	}
	else 
	{
		if (payloadSize < 4) {
			// <4 = DATA ERROR, =4 = TYPE ERROR, >4 <max = ok, >max = DATA ERROR
			*errorResponse = RSP_DATA_ERROR;
		}
	}

	return hResult;
}

HRESULT cliCBCallback(PB * incomingPacket)
// Called when a CLI remote call arrives
{
	HRESULT			hResult = NO_ERROR;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	OFFSET_1394		offsetDest;
	uint32			regionOffset;
	RCODE_1394		errorResponse = RSP_ADDRESS_ERROR;

	hResult = pbGetPacketType(incomingPacket,&packetType);
	if (hResult != NO_ERROR) return hResult;

	hResult = pbGetDestinationOffset(incomingPacket, &offsetDest);
	if (hResult != NO_ERROR) return hResult;

	regionOffset = offsetDest.Low - CLI_CB_BASE_START;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "Access to Remote CLI memory:\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "offset: %04hx_%08x, regionOffset:%08x\n\r", offsetDest.High, offsetDest.Low, regionOffset);

	switch (regionOffset) {
		case CLI_CONTROLLER_HREG :
			hResult = cliCBControllerReg(incomingPacket, packetType, &errorResponse);		// All in one block lock.
			break;
		case CLI_RESP_CLEAR_REG :
			hResult = cliCBResponseClearReg(incomingPacket, packetType, &errorResponse);	// Quadlet write only
			break;
		case CLI_RESP_COUNT_REG :
			hResult = cliCBResponseCountReg(incomingPacket, packetType, &errorResponse);	// Quadlet read only
			break;
		case CLI_RESULT_REG :
			hResult = cliCBResultReg(incomingPacket, packetType, &errorResponse);			// Quadlet read only
			break;
		case CLI_CALL_AREA :
			hResult = cliCBCallArea(incomingPacket, packetType, &errorResponse);			// This has to be an all in one blockwrite
			break;
		default :
			if ((regionOffset >= CLI_RESPONSE_AREA) && (regionOffset < (CLI_RESPONSE_AREA + CLI_MAX_MSG_SIZE))) {
				// Deal with any kind of read to the response area
				hResult = cliCBResponseBufferCB(incomingPacket, packetType, &errorResponse);
			}
			break;
	}

	if (errorResponse != RSP_COMPLETE)
	{	
		// If something went wrong above, return the appropriate error response
		hResult = lalReplyErrorResponse(incomingPacket, errorResponse, TRUE);
		SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "\tReplied with error response:0x%08x\n\r", errorResponse);
	}

	return hResult;
}

HRESULT cliCBHandleCommand(char *cliCommandStr)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bVerbose = cliCBVerbose;

#ifdef _CLICB_SYSLOGERROR
	cliCBSysLogErrorEnabled = TRUE;
	cliCBSysLogErrorStrLen = 0;
#endif //_CLICB_SYSLOGERROR
	if (bVerbose == FALSE)
	{
		sysPrintfSetEnable(FALSE);
	}
	cliCBStatus = 0;
	cliCBRespCount = 0;	// Response is zero bytes long, as we are about to execute the command
						// Do this before we complete the transaction, to prevent any chance of a race

	hResult = cliProcessCommand(cliCommandStr);
	cliCBRespCount = cliCBSysLogErrorStrLen + 1;	// Set the output count for the other side to read the response
	cliCBStatus = 1;
	if (bVerbose == FALSE)
	{
		sysPrintfSetEnable(TRUE);
	}
#ifdef _CLICB_SYSLOGERROR
	cliCBSysLogErrorEnabled = FALSE;
#endif //_CLICB_SYSLOGERROR

	return hResult;
}

#ifdef _CLICB_DEFERRED_TASK	
/*********************************************************
	thread cli CB deferred task
*/
void cliCBThread(void *dummy)
{
	HRESULT		hResult = NO_ERROR;
	CLICB_CMD*	cmd = NULL;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		// thread cli CB deferred task message queue
		hResult = TCMsgQueueReceive(cliCBDeferredQueueID, (void *) &cmd, TC_WAIT_FOREVER);
		
		if (hResult == NO_ERROR)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "CLI command call from deferred task: ");
			SYS_DEBUG(SYSDEBUG_TRACE_CLICB, cmd->cmdStr);
			SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "\n\r");

			cliCBResult = cliCBHandleCommand(cmd->cmdStr);
			cliCBFreeDeferredCmd(cmd);
		}
	}
}
#endif //_CLICB_DEFERRED_TASK

/*********************************************************
	Resume tasks for this module
*/
HRESULT cliCBResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

#ifdef _CLICB_DEFERRED_TASK	
	// thread cli CB deferred task
	hResult = TCTaskResume(cliCBThreadID);
	if (hResult != NO_ERROR) return hResult;
#endif //_CLICB_DEFERRED_TASK

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT cliCBSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

#ifdef _CLICB_DEFERRED_TASK	
	// thread cli CB deferred task
	hResult = TCTaskSuspend(cliCBThreadID);
	if (hResult != NO_ERROR) return hResult;
#endif //_CLICB_DEFERRED_TASK

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT cliCBInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

#ifdef _CLICB_DEFERRED_TASK	
	uint32	priority = TASK_PRIORITY_BELOW_NORMAL; //TASK_PRIORITY_DEFAULT;

	// thread cli CB deferred task
	hResult = TCTaskCreate(&cliCBThreadID, cliCBThreadName, cliCBThread, priority, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;
#endif //_CLICB_DEFERRED_TASK

	return hResult;
}

/*********************************************************
	Create message queues for this module
*/
HRESULT cliCBCreateMessageQueues(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _CLICB_DEFERRED_TASK	
	// thread cli CB deferred task message queue
	hResult = TCMsgQueueOpen(&cliCBDeferredQueueID,				// the queue id
							1,									// size of queue element = 1 quadlet
							&cliCBDeferredEventQueueMemory,		// the memory to use
							CLICB_DEFERRED_QUEUE_SIZE,			// how big the queue is (in bytes)
							TRUE);
#endif //_CLICB_DEFERRED_TASK
	
	return hResult;
}

HRESULT cliCBInitialize(void)
{
	HRESULT			hResult = NO_ERROR;

	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "Remote CLI interface active\n\r");
	SYS_DEBUG(SYSDEBUG_TRACE_CLICB, "CB Range is: %04hx_%08x to %04hx_%08x\n\r", cliDestOffsetLo.High, cliDestOffsetLo.Low, cliDestOffsetHi.High, cliDestOffsetHi.Low);

	// Install callbacks for address ranges handled by the CLI
	hResult = lalAddressRangeRegisterCB (cliCBCallback, cliDestOffsetLo, cliDestOffsetHi);
	if (hResult != NO_ERROR) return hResult;

	hResult = cliCBCreateMessageQueues();
	if (hResult != NO_ERROR) return hResult;

	cliCBCliInstallTools();

	return hResult;
}

#endif //_CLI_CB
#endif //_CLI

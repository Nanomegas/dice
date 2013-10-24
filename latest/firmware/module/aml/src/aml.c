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
	
	Purpose:	Abstract MIDI Layer
  
  Description:       
  
	Revisions:
		created 09/18/2006 zluo

****************************************************************************/
#include "TCTypes.h"
#include "TCTasking.h"
#include "aml.h"
#include "TCSemaphore.h"
#include "TCDebug.h"
#include "diceTimer2.h"

#ifdef _CLI
#include "amlCli.h"
#endif

#include <string.h>

typedef struct {
	uint32	timestamp;
	uint8	data;
} midi_byte_t;

typedef struct {
	uint8				head;				//point to first data in
	uint8				tail;				//point to first empty slot
	midi_byte_t			data[AML_BUF_SIZE]; //buf can hole AML_BUF_SIZE - 1 bytes
	uint32				semId;				//wait semaphore for rx.
} midi_buf_t;

typedef struct {
	aml_dev_type_t	type;
	uint32			state;
	midi_buf_t		buf;
	midi_dev_cb_t	*cbs;
	BOOL			bMonitor;				//flag to copy the data to application.
	uint32			dest;
	uint32			drvHandle;				//some dev specific data use to configure dev
	char			name[16];
	uint32          nberr;                  //counts tx overflow errors for diagnostics
} midi_dev_t;

static midi_dev_t amlDevTable[AML_DEV_NUM + 1];

#ifdef _CLI
HRESULT amlDisplayDev(uint32 dev)
{
	
	if(dev >= AML_DEV_NUM) 
		return E_FAIL;
	if(amlDevTable[dev].state){ //if the device is enabled.
		sysDebugPrintf("%16s %4d ", amlDevTable[dev].name, dev);
		if(amlDevTable[dev].type == AML_DEV_TX) sysDebugPrintf("   AML_TX_DEV    ");
		if(amlDevTable[dev].type == AML_DEV_RX) sysDebugPrintf("   AML_RX_DEV    ");

		sysDebugPrintf("0x%08x %4d %3d %7d %8d\n", 
			amlDevTable[dev].cbs, 
			amlDevTable[dev].dest, 
			amlDevTable[dev].bMonitor, 
			amlDevTable[dev].drvHandle,
			amlDevTable[dev].nberr);
		amlDevTable[dev].nberr = 0;
	}
	return NO_ERROR;
}
#endif //_CLI

BOOL midi_buf_full(midi_buf_t *buf)
{
	return ((buf->tail+1)%AML_BUF_SIZE) == buf->head;
}
BOOL midi_buf_empty(midi_buf_t *buf)
{
	return buf->head == buf->tail;
}

HRESULT midi_buf_put_char(midi_buf_t *buf, char c, uint32 timeStamp)
{
	if(!midi_buf_full(buf)) {
		//There is chance an interrupt can happen here to cause inconsistency of buffer
		//caller need to make sure the exclusive write to it. 
		if(timeStamp)
			buf->data[buf->tail].timestamp = timeStamp;
		else
			buf->data[buf->tail].timestamp = diceTimer2GetLow();
		buf->data[buf->tail].data = c;
		buf->tail = (buf->tail + 1) % AML_BUF_SIZE;
		TCSemaphoreSignal(buf->semId);
		return NO_ERROR;
	} 
	return E_QUEUE_FULL; // buf is full.
}
HRESULT midi_buf_get_char(midi_buf_t *buf, char *data)
{
	//Pass semaphore means there is some data in it.
	TCSemaphoreWait(buf->semId);
	if(midi_buf_empty(buf)) {
		SYS_DEBUG(SYSDEBUG_TRACE_AML, "try to get data from an empty buf.\n");
		return E_FAIL;
	}
	*data = buf->data[buf->head].data;
	buf->head = (buf->head+1)%AML_BUF_SIZE;
	return NO_ERROR;
}

HRESULT amlCreateDev(uint32 *midi_dev_id, midi_dev_cb_t *cbs, aml_dev_type_t type, uint32 arg, char *name)
{
	int i;
	//item [0] is reserved as unused. 
	for(i = 1; i < AML_DEV_NUM; i++) {
		if(amlDevTable[i].state == 0) {			// dev unused
			strncpy(amlDevTable[i].name, name, 16);
			amlDevTable[i].state = 1;			// dev created
			amlDevTable[i].cbs = cbs;
			amlDevTable[i].type = type;	
			amlDevTable[i].bMonitor = FALSE;	//no monitor 
			*midi_dev_id = i;
			//call the driver init function
			(*((amlDevTable[i].cbs)->init))(i, arg, &amlDevTable[i].drvHandle);
			if(type == AML_DEV_RX) {
				amlDevTable[i].dest = 0;			// by default to drop the packet
				TCSemaphoreOpen(&amlDevTable[i].buf.semId, 0);
			}
			if(type == AML_DEV_TX) {
				amlDevTable[i].dest = 0;			// no dest for TX device
				TCSemaphoreOpen(&amlDevTable[i].buf.semId, 1);
			}
			return NO_ERROR;
		}
	}
	return E_FAIL;
}

//When bMonitor == TRUE, the meaning is to Tx device, user can send data to it. 
//for RX device, the Rx'ed data is copied to user buffer.
HRESULT amlSetDevMonitorMode(uint32 id, BOOL enable)
{
	//if device is not valie or it is an tx device
	if(amlDevTable[id].state == 0) {
		return E_FAIL; 
	}
	amlDevTable[id].bMonitor = enable;
	return NO_ERROR;
}

HRESULT amlStartDev(uint32 midi_dev_id)
{
	//MPTR(ICTL_ENABLE) |= (1 < 	
	(*((amlDevTable[midi_dev_id].cbs)->start))(amlDevTable[midi_dev_id].drvHandle);
	return NO_ERROR;
}

HRESULT amlStopDev(uint32 midi_dev_id)
{
	(*((amlDevTable[midi_dev_id].cbs)->stop))(amlDevTable[midi_dev_id].drvHandle);
	return NO_ERROR;
}

HRESULT amlDestroyDev(uint32 midi_dev_id)
{
	//zluo, should we release the remain data?
	if(amlDevTable[midi_dev_id].state != 0) {
		TCSemaphoreClose(amlDevTable[midi_dev_id].buf.semId);
	}
	memset(&amlDevTable[midi_dev_id], 0, sizeof(midi_dev_t));
	return NO_ERROR;
}


HRESULT amlAppGetByte(uint32 midi_dev_id, uint8 *buf)
{
	HRESULT hResult = NO_ERROR;
	if(amlDevTable[midi_dev_id].type != AML_DEV_RX)
		return E_FAIL;
	if(!amlDevTable[midi_dev_id].bMonitor)
		return E_FAIL;
	hResult = midi_buf_get_char(&amlDevTable[midi_dev_id].buf, buf);
	return hResult;
}

HRESULT amlAppPutByte(uint32 midi_dev_id, uint8 value)
{
	//HRESULT hResult = NO_ERROR;
	//char c;
	if(amlDevTable[midi_dev_id].type != AML_DEV_TX)
		return E_FAIL;
	if(!amlDevTable[midi_dev_id].bMonitor)
		return E_FAIL;
	
	//ML: The code below does not make sense as transmit is not interrupt driven,
	//  for now we will Yield and retry until transmitter is ready.
	
	//hResult = midi_buf_put_char(&amlDevTable[midi_dev_id].buf, value, 0);
	//if(hResult != NO_ERROR) 
	//	return hResult;

	while (1)
	{
		if ((*(amlDevTable[midi_dev_id].cbs)->putchar)(amlDevTable[midi_dev_id].drvHandle, value)) break;
		//SYS_TRACE2(SYSDEBUG_TRACE_AML, midi_dev_id, value);
		TCTaskYield();
	}
	return NO_ERROR;
}
//driver rxed a byte, add it into a queue or directly tx it onto destiation
HRESULT amlDrvIndRxByte(uint32 midi_dev_id, uint8 byte)
{
	HRESULT hResult = NO_ERROR;
	//actually, routing can be done here, 2 desitinations:
	//1: rx queue if it is to application, 
	//2: tx quueu of some other device (include itself).
	if(amlDevTable[midi_dev_id].bMonitor == TRUE)
		midi_buf_put_char(&amlDevTable[midi_dev_id].buf, byte, 0);
	// direct send the byte
	uint32 dest_id = amlDevTable[midi_dev_id].dest;
	//SYS_TRACE3(SYSDEBUG_TRACE_AML, midi_dev_id, byte, dest_id);
	//dest_id == 0 mean to drop the byte;
	if(dest_id == 0)
		return hResult; 
	//If input check is needed, it should be done here. 
	if (!(*((amlDevTable[dest_id].cbs)->putchar))(amlDevTable[dest_id].drvHandle, byte))
		amlDevTable[dest_id].nberr ++;
	return hResult;
}

//driver can tx data, transmit it or return not the right time
HRESULT amlDrvIndTxEmpty(uint32 midi_dev_id, uint8 *data)
{
	HRESULT hResult = NO_ERROR;
	return hResult;
}

HRESULT amlSetDestination(uint32 src, uint32 dest)
{
	//To drop off data
	if(dest == 0) {
		amlDevTable[src].dest = 0;
		return NO_ERROR;
	}

	if((amlDevTable[src].state == 0) || (amlDevTable[dest].state == 0)) {
		sysDebugPrintf("Device is not active.\n");
		return E_FAIL;
	}
	if(amlDevTable[src].type != AML_DEV_RX) {
		sysDebugPrintf("source has to be a RX device.\n");
		return E_FAIL;
	}

	if(amlDevTable[dest].type != AML_DEV_TX) {
		sysDebugPrintf("dest has to be a RX device.\n");
		return E_FAIL;
	}
	amlDevTable[src].dest = dest;
	return NO_ERROR;
}

//*********************************************************************************
//* now the calling code for aml
//*********************************************************************************
HRESULT amlInitialize(void)
{
#ifdef _CLI
	amlCliInstallTools();
#endif //_CLI
	return NO_ERROR;
}


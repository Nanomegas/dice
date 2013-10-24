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
	
	Purpose:	DICE JR SPI interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/
#ifdef _SPI

#include <string.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCDebug.h"
#include "coredefs.h"
#include "TCInterrupt.h"
#include "TCSemaphore.h"
#include <cyg/hal/hal_platform_ints.h>

#include "spi.h"
#include "spiDefs.h"

static char 		moduleName[] = __FILE__ ;

static spi_ss_cb_t spiSSRoutine= NULL;

static spi_slave_t ssTable[SPI_MAX_SLAVE_COUNT];

//spi op queue, controled by spiBufHead/Tail
//This needs to be considered volatile as one execution context
//is changing the Head and another the Tail.

static volatile spi_op_t spiBuf[SPI_MAX_BUF_SIZE];
static volatile uint32 spiBufHead, spiBufTail;
// Last spic slave ID. 
static uint32 lastSpiSSId = SPI_MAX_SLAVE_COUNT; //mean this is the first spiOp

static BOOL spiBufEmpty(void) 
{
	return spiBufHead == spiBufTail;
}

static BOOL spiBufFull(void) 
{
	return (spiBufTail + 1) % SPI_MAX_BUF_SIZE == spiBufHead;
}

#define spiSetInterrupt(value) MPTR(SPI_INT_MASK)=value

//Use this assume it passes !spiBufFull condition. so there is at least 1 space in the buf.
BOOL _spiBufAdd(uint32 slaveId, uint32 outData, uint32 *indata) 
{
	uint32 i;

	//this area need to be protected if it is a pre-emptyive OS, 
	//or if we allow ISR/DSR to call. We will protect with disabling
	//interrupts.
	
#ifdef _SPI_INTERRUPT_SAFE
	TCInterruptGlobalDisable();
#endif
	if(spiBufFull()) 
	{	
#ifdef _SPI_INTERRUPT_SAFE
		TCInterruptGlobalRestore();
#endif

		return FALSE;
	}
	i = spiBufTail;
	
	spiBuf[i].ssId = slaveId;
	spiBuf[i].outData = outData;
	spiBuf[i].inData = indata;
	spiBuf[i].bPending = FALSE;
	
	spiBufTail = (spiBufTail+1) % SPI_MAX_BUF_SIZE;

	spiSetInterrupt(SPI_TX_EMPTY | SPI_RX_FULL);
	
#ifdef _SPI_INTERRUPT_SAFE
	TCInterruptGlobalRestore();
#endif

	return TRUE;
}


HRESULT spiGetSlaveDevice(uint32 *slaveId)
{
	int i;
	for(i = 0; i < SPI_MAX_SLAVE_COUNT; i++) {
		if (!ssTable[i].bValid) {
			ssTable[i].bValid = TRUE;
			*slaveId = i;
			ssTable[i].rate.reg = 0x3E8;		//set the default value
			ssTable[i].config.reg = 0x75;	//
			ssTable[i].complete_cb = NULL;	// NO callback
			return NO_ERROR;
		}
	}
	sysLogError(E_SPI_NO_FREE_DEVICE, __LINE__, moduleName);
	return E_SPI_NO_FREE_DEVICE;
}

HRESULT spiFreeSlaveDevice(uint32 slaveId)
{
	HRESULT hResult = NO_ERROR;
	if((slaveId >= SPI_MAX_SLAVE_COUNT) || !ssTable[slaveId].bValid )
	{
		hResult = E_SPI_INVALID_SLAVEID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	ssTable[slaveId].bValid = FALSE;
	return hResult;
}

HRESULT spiConfigure(uint32 slaveId, spi_cmd_t cmd, void *arg)
{
	HRESULT hResult = NO_ERROR;
	int i = slaveId;

	if((slaveId >= SPI_MAX_SLAVE_COUNT) || !ssTable[slaveId].bValid )
	{
		hResult = E_SPI_INVALID_SLAVEID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	switch(cmd) {
		case SPI_SET_SS_ROUTINE:
			spiSSRoutine = (spi_ss_cb_t)arg;
			break;
		case SPI_COMPLETE_CB:
			ssTable[i].complete_cb = (spi_complete_cb_t)arg;
			break;
		case SPI_RATE:
			ssTable[i].rate.bit.rate = (uint32)arg;
			break;
		case SPI_MSBF:
			ssTable[i].config.bit.msbf = (uint32)arg & 0x1;
			break;
		case SPI_CLRIUR:
			ssTable[i].config.bit.clriur = (uint32)arg & 0x1;
			break;
		case SPI_KPOLD:
			ssTable[i].config.bit.kpold = (uint32)arg & 0x1;
			break;
		case SPI_POLARITY:
			ssTable[i].config.bit.clkpl = (uint32)arg & 0x1;
			break;
		case SPI_PHASE:
			ssTable[i].config.bit.clkphase = (uint32)arg & 0x1;
			break;
		case SPI_WSIZE:
			ssTable[i].config.bit.wsize = (uint32)arg & 0x3;
			break;
		default:
			return SPI_ERROR_UNKNOWN_CMD;
			break;
	}
	return hResult;
}


void spiStartTx(void)
{
	//atomic as it only sets a bit
	spiSetInterrupt(SPI_TX_EMPTY | SPI_RX_FULL);
}

void spiSetSlaveConfig(uint32 ssID)
{
	MPTR(SPI_CTRL) = ssTable[ssID].config.reg;
	MPTR(SPI_BAUD_RATE) = ssTable[ssID].rate.reg;
}

//This routine can be used before tasking has started
//the spoSSRoutine will be called if needed
//the complete function is not called

HRESULT spiOpBlockNoTask(uint32 slaveId, uint32 outData, uint32 *inData)
{
	HRESULT hResult = NO_ERROR;
	if (TCTaskStarted ())
	{
		hResult = E_GEN_WRONG_CONTEXT;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;		
	}
	if((slaveId >= SPI_MAX_SLAVE_COUNT) || !ssTable[slaveId].bValid )
	{
		hResult = E_SPI_INVALID_SLAVEID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	// this is all based on busy waiting
	//Wait for previous command done
	while ((MPTR(SPI_STAT)&0x12) != 0x12);
	//clear any received data
	uint32 val = MPTR(SPI_DATA);
	
	if(lastSpiSSId != slaveId) {
		lastSpiSSId = slaveId;
		if(spiSSRoutine)
			(*spiSSRoutine)(lastSpiSSId); 
		spiSetSlaveConfig(lastSpiSSId);
	}
	//we are ready to send data
	MPTR(SPI_DATA) = outData;
	//Wait for this command done
	while ((MPTR(SPI_STAT)&0x12) != 0x12);
	//get any received data
	val = MPTR(SPI_DATA);
	if (inData) *inData = val;
	return hResult;	
}


//This is the interrupt version of SPI, if user wants an blocking version of op, an 
//user defined semaphore needs to be signalled in the complete_cb. 
HRESULT spiOpNonBlock(uint32 slaveId, uint32 outData, uint32 *inData)
{
	HRESULT hResult = NO_ERROR;

	if (!TCTaskStarted ())
	{
		hResult = E_GEN_WRONG_CONTEXT;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;		
	}

	if((slaveId >= SPI_MAX_SLAVE_COUNT) || !ssTable[slaveId].bValid )
	{
		hResult = E_SPI_INVALID_SLAVEID;
		sysLogError(hResult, __LINE__, moduleName);
	}
	else 
	{
		if (!_spiBufAdd(slaveId, outData, inData))
		{
			sysLogError(E_SPI_BUF_FULL, __LINE__, moduleName);
			return E_SPI_BUF_FULL;
		}
	}
	return hResult;
}


//This routine checks pending Ops and dispatch the head into SPI, it can be called 
//either in TX_DONE (for input/output) or TX_EMPTY (for output only)
static void spiProcessOp(void) 
{
	if(spiBufEmpty()) {
		spiSetInterrupt(SPI_RX_FULL);
		//there is nothing to do, we don't need interrupts
		return;
	}
	if(lastSpiSSId != spiBuf[spiBufHead].ssId) {
		uint32 d;

		d = MPTR(SPI_STAT);
		if ((d&0x12)!=0x12) {
			//if data received then it need to enable TX_EMPTY for TX_DONE
			spiSetInterrupt(SPI_TX_EMPTY | SPI_RX_FULL); //ML:if empty but not done re will keep getting interrupts until done
											                 //   ugly psoudo busy waiting but only when ssId is changing???
			//SYS_TRACE3(SYSDEBUG_TRACE_SPI, lastSpiSSId, MPTR(SPI_INT_MASK),  d);
			return; //can't change slave if there are pending ops
		}

		lastSpiSSId = spiBuf[spiBufHead].ssId;
		//SYS_TRACE3(SYSDEBUG_TRACE_SPI, lastSpiSSId, ssTable[lastSpiSSId].config.reg,  d);
		//application should make mapping between slaveId and internal ID
		//see AM's targetBoard.c for the example.
		if(spiSSRoutine)
			(*spiSSRoutine)(lastSpiSSId); 
		spiSetSlaveConfig(lastSpiSSId);
		//SYS_TRACE3(SYSDEBUG_TRACE_SPI, lastSpiSSId, MPTR(SPI_INT_MASK),  0);
	}
	//SYS_TRACE3(SYSDEBUG_TRACE_SPI, lastSpiSSId, MPTR(SPI_STAT), spiBuf[spiBufHead].outData);
	spiBuf[spiBufHead].bPending = TRUE; //pending for transaction done.
	MPTR(SPI_DATA) = spiBuf[spiBufHead].outData;
	spiSetInterrupt(SPI_RX_FULL); //we only need that for now, we know it will happen
}

void spiIntDsr(void)
{
	uint32 raw_stat = MPTR(SPI_STAT);
	uint32 int_stat;
	//disable interrupt
	//TCInterruptGlobalDisable(); //ML: Not needed due to head/tail mechanism
	//SYS_TRACE3(SYSDEBUG_TRACE_SPI, int_stat, spiBufHead, spiBufTail);
	
	//Only get the interrupt intended
	int_stat = raw_stat & MPTR(SPI_INT_MASK);
	if(int_stat & SPI_TX_DONE) 
	{ // TX_DONE
		//SYS_TRACE3(SYSDEBUG_TRACE_SPI, int_stat, spiBufHead, spiBufTail);
	} 

	if (int_stat & SPI_TX_UNDER_RUN) 
	{
		//error for master mode
	} 
	if (int_stat & SPI_RX_OVERFLOW) 
	{
		//This shouldn't happen if ONLY TX_DONE is triggered.
	} 

	if (int_stat & SPI_RX_FULL) 
	{ //ML: Moved above check to TX_EMPTY as the DSR could be late
		//it alwasy come before a TX DONE, it is cleared by read of SPI_DATA
		//if the input data is important, it should be processed in tx_done.
		//Tx is done and it always means rx full, 2 clock delay.
		spi_slave_t *s = &ssTable[spiBuf[spiBufHead].ssId];
		uint32 val = MPTR(SPI_DATA);
		if (spiBuf[spiBufHead].inData)
			*spiBuf[spiBufHead].inData = val;
		if(s->complete_cb) 
			(*(s->complete_cb))(spiBuf[spiBufHead].ssId, spiBuf[spiBufHead].inData); 
		spiBufHead = (spiBufHead + 1) % SPI_MAX_BUF_SIZE;
		//Now send next op if there is any.
		spiProcessOp();
	}
	else if (int_stat & SPI_TX_EMPTY) 
	{
		//We got here to wait for TX_DONE or to poll for a new command after having
		//been empty 
		if(spiBuf[spiBufHead].bPending)
		{
			//doesn't really matter, RX_FULL will do the job
			spiSetInterrupt(SPI_RX_FULL); //we only need that for now, we know it will happen
			return;
		}
		//okay, so we got no pending RX time to look if we can issue a new command
		spiProcessOp(); //this will do all the work. We might keep coming back here if we have to 
						// wait for TX_DONE
		
	} 

	//TCInterruptGlobalRestore(); //ML: Not needed due to head/tail mechanism
}

HRESULT spiInitialize(void)
{
	//SYS_TRACE2(SYSDEBUG_TRACE_SPI, spiBufHead, spiBufTail);
	TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_SPI, NULL, spiIntDsr);
	return NO_ERROR;
}

#endif //_SPI

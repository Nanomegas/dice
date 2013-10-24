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
#ifdef _I2C
#include "TCTypes.h"
#include "TCDebug.h"
#include "coreDefs.h"
#include "i2c.h"
#include <cyg/hal/hal_platform_ints.h>
#include "TCTasking.h"
#include "TCSemaphore.h"
#include "TCInterrupt.h"
#include "targetBoard.h"
#include "sysDebug.h"

#ifdef _CLI
#include "i2cCli.h"
#endif

#define MPTR(x) (*((volatile uint32 *) (x)))
#define ERROR_STATUS (MPTR(IC_RAW_INTR_STAT) & 0x4a)

uint16 i2cCurrentAddress;

typedef struct {
	uint16					target;		//target address
	uint8					type;		//0:read, 1:write
	uint8					status;		//0:free, 1:init, 2:pending, 3:completed
	uint8					cur_pos;	//current position
	uint8					length;		//length of the buffer
	uint8					*data;		//buffer, user data
} i2c_tran_t;

#define I2C_READ_FIFO_SIZE 8  //not to read more than fifo size
static uint32 i2cBufSemId, i2cCompleteSemId;
static i2c_tran_t tempTran; //1 buffer for each now. 

static uint32 i2c_tx_err_counter = 0, i2c_tx_previous_count;
static uint32 i2c_last_tx_error = 0;

#define ERROR_STATUS (MPTR(IC_RAW_INTR_STAT) & 0x4a)
uint32 i2cErrorCounter(void)
{
	return i2c_tx_err_counter;
}

uint32 i2cLastError(void)
{
	return i2c_last_tx_error;
}
_TC_INLINE_STATIC void _i2cIntEnable(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void _i2cIntEnable(void)
{
	MPTR(IC_ENABLE) = 0x1;
};
_TC_INLINE_STATIC void _i2cIntDisable(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void _i2cIntDisable(void)
{
	MPTR(IC_ENABLE) = 0x0;
};

_TC_INLINE_STATIC void _i2cClearIntr(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void _i2cClearIntr(void)
{
	uint32 dummy;
	dummy = MPTR(IC_CLR_INTR);
};

//add a command to tx FIFO if it is not empty.
_TC_INLINE_STATIC void _i2cIntWriteCmd(uint16 value) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void _i2cIntWriteCmd(uint16 value)
{
	//wait for tx-buffer is not empty.
	MPTR(IC_DATA_CMD) = value;
}

static void _i2cIntSetTargetAddress(uint16 target)
{
	if(i2cCurrentAddress != (target & 0xFFF)) {
		//Wait until bus is idle
		//while((MPTR(IC_STATUS) & 0x1) == 1);
		_i2cIntDisable();
		MPTR(IC_TAR) = target & 0xFFF;
		i2cCurrentAddress = target & 0xFFF;
		_i2cIntEnable();
	}
}

static void startI2cTran(void)
{
	if(tempTran.status != 0) { //if tempTran is valid
		_i2cIntSetTargetAddress(tempTran.target);
		tempTran.status = 2; //pending for completion.
		//SYS_TRACE0(SYSDEBUG_TRACE_I2C);
		if(tempTran.type == 0) {
			//Read transaction, enable tx AND rx buffer interrupt
			//Tx is for the interrupt routine to start
			MPTR(IC_INTR_MASK) = BIT2 | BIT4 | BIT6;
		} else {
			//Write transacation, enable tx
			MPTR(IC_INTR_MASK) = BIT4 |BIT6;
		}
	}
}

static void resetTran(i2c_tran_t *pTran) {
	if(pTran) {
		memset((void *)pTran, 0, sizeof(i2c_tran_t));
	}
}

static HRESULT addI2cTrans(i2c_tran_t *pTran, 
						   uint8 type, uint16 target, uint8 *buf, uint8 length)
{
	if(!pTran) return NO_ERROR;

	pTran->type = type;
	pTran->target = target;
	pTran->data = buf;
	pTran->length = length;
	pTran->status = 1;
	pTran->cur_pos = 0;
	return NO_ERROR;
}

BOOL i2cErrorOccur(void) {
	if(i2c_tx_err_counter != i2c_tx_previous_count) {
		SYS_DEBUG(SYSDEBUG_TRACE_I2C, "Error %d , cause = %x\n", i2c_tx_err_counter, i2c_last_tx_error);
		i2c_tx_previous_count  = i2c_tx_err_counter;
		return TRUE;
	}
	return FALSE;
}


static HRESULT i2cOp(uint16 target, uint8 type, uint8 *buf, uint8 length)
{
	TCSemaphoreWait(i2cBufSemId);
	if(tempTran.status != 0)  {//busy
		TCSemaphoreSignal(i2cBufSemId);
		return E_I2C_TRY_AGAIN; //some error
	}
	addI2cTrans(&tempTran, type, target, buf, length);
	TCSemaphoreSignal(i2cBufSemId);
	
	startI2cTran();
	if(TCTaskStarted())
		TCSemaphoreWait(i2cCompleteSemId);
	else
		TCTaskWait(10);
	//SYS_TRACE3(SYSDEBUG_TRACE_I2C, target, type, length);
	
	if(i2cErrorOccur()) {
		SYS_DEBUG(SYSDEBUG_TRACE_I2C, "error = %d \n", i2cErrorCounter());
		return E_FAIL;
	}
	else {
		return NO_ERROR;
	}
}

HRESULT i2cReadBuf(uint16 target, uint8 *buf, uint8 length)
{
	HRESULT hResult;
	while((hResult = i2cOp(target, 0, buf, length)) == E_I2C_TRY_AGAIN)
		TCTaskWait(1);
	return hResult;
}

HRESULT i2cWriteBuf(uint16 target, uint8 *buf, uint8 length)
{
	HRESULT hResult;
	while((hResult = i2cOp(target, 1, buf, length)) == E_I2C_TRY_AGAIN)
		TCTaskWait(1);
	return hResult;
}

void i2cIntDsr(void)
{
	uint32 int_stat = MPTR(IC_INTR_STAT);
	uint32 int_raw_stat = MPTR(IC_RAW_INTR_STAT);
	//disable interrupt
	TCInterruptGlobalDisable();
	//SYS_TRACE3(SYSDEBUG_TRACE_I2C, int_stat, int_raw_stat, tempTran.status);
	if((int_stat & BIT4) && ((int_raw_stat & BIT8) == 0)) { // TX empty and no activity
		switch(tempTran.status) {
			int i;
			case 2:
				//we are guranteed to have enough buf
				for(i = 0; i < tempTran.length; i++) {
					switch (tempTran.type) {
					case 0:
						_i2cIntWriteCmd(0x100);
						break;
					case 1:
						_i2cIntWriteCmd(tempTran.data[tempTran.cur_pos]);
						tempTran.cur_pos++;
						break;
					} 
				}
				tempTran.status++;
				break;
			case 3:
				MPTR(IC_INTR_MASK) &= ~BIT4; //clear TX empty interrupt
				if(tempTran.type == 1) {
					resetTran(&tempTran);
					MPTR(IC_INTR_MASK) = 0;
					//SYS_TRACE0(SYSDEBUG_TRACE_I2C);
					TCSemaphoreSignal(i2cCompleteSemId);
				}
				break;
			default:
				break;
		}
	}
	if(int_stat & BIT2) { // Rx not empty
		//while(MPTR(IC_STATUS) & BIT3) {//rx not 
		while(MPTR(IC_RXFLR) != 0) {//rx not empty
			tempTran.data[tempTran.cur_pos] = MPTR(IC_DATA_CMD);
			tempTran.cur_pos++;
		}
		if(tempTran.cur_pos >= tempTran.length) {
				resetTran(&tempTran);
				MPTR(IC_INTR_MASK) = 0;
				//SYS_TRACE1(SYSDEBUG_TRACE_I2C, tempTran.cur_pos);
				TCSemaphoreSignal(i2cCompleteSemId);
		}
	}
	
	if(int_stat & BIT6) { //Tx Error
		i2c_tx_err_counter++;
		i2c_last_tx_error = MPTR(IC_TX_ABRT_SOURCE);

		resetTran(&tempTran);
		MPTR(IC_INTR_MASK) = 0;
		//SYS_TRACE2(SYSDEBUG_TRACE_I2C, i2c_last_tx_error, i2c_tx_err_counter);
		TCSemaphoreSignal(i2cCompleteSemId);
	}
	_i2cClearIntr();
	TCInterruptGlobalRestore();
}

static uint32 i2cPollWrite(uint16 targetAddress, uint8 *buf, uint8 bufLength) 
{
	uint32 i;
	if(targetAddress != i2cCurrentAddress) {
		_i2cIntSetTargetAddress(targetAddress);
	}

	if(bufLength >= I2C_RX_TX_BUF_SIZE) //can't handle more than FIFO size
		return 1; //return error.

	while((MPTR(IC_TXFLR) & 0x1) != 0); //wait until tx fifo is empty
	_i2cClearIntr();
	for(i = 0; i < bufLength; i++) {
		while((MPTR(IC_STATUS) & 0x2) == 0);
		_i2cIntWriteCmd(buf[i]);
	}
	while((MPTR(IC_RAW_INTR_STAT) & 0x200) == 0); //Wait until stop detected to conclude the transaction.

	return ERROR_STATUS;
}

static uint32 i2cPollRead(uint16 targetAddress, uint8 *buf, uint32 bufLength) 
{
	uint32 i;
	if(targetAddress != i2cCurrentAddress) {
		_i2cIntSetTargetAddress(targetAddress);
	}

	while(MPTR(IC_RXFLR) != 0) //if there any Rxed data in FIFO
		i = MPTR(IC_DATA_CMD); //i used as a dummy

	_i2cClearIntr();
	//The read is slow because we don't do buffering.
	for(i = 0; i < bufLength; i++) {
		while((MPTR(IC_STATUS) & 0x1) == 1); //wait for idle
		_i2cIntWriteCmd(0x100);
		//wait until there is data in or an error happens
		while(MPTR(IC_RXFLR) == 0) {
			if(ERROR_STATUS) {
				SYS_DEBUG(SYSDEBUG_TRACE_I2C, "%s, %x\n", __func__, ERROR_STATUS);
				return ERROR_STATUS;
			}
		};
		while((MPTR(IC_STATUS) & 0x1) == 1);
		buf[i] = MPTR(IC_DATA_CMD);
	}
	return 0;
}

uint32 i2cIntRead(uint16 targetAddress, uint8 *buf, uint32 bufLength) 
{
	uint32 len, remain = bufLength;
	uint8 *ptr = buf;

	while(remain > 0) {
		len = (remain > 8) ? 8 : remain;
		if(i2cReadBuf(targetAddress, ptr, len)) {
			SYS_DEBUG(SYSDEBUG_TRACE_I2C, "Error happened during reading, read %d out of %d\n", bufLength - remain, bufLength);
			return bufLength - remain;
		}

		ptr += len;
		remain -= len;
	}
	return bufLength;
}

uint32 i2cWrite(uint16 targetAddress, uint8 *buf, uint8 bufLength) 
{
	if(TCTaskStarted())
		return i2cWriteBuf(targetAddress, buf, bufLength);
	else
		return i2cPollWrite(targetAddress, buf, bufLength);
}

uint32 i2cRead(uint16 targetAddress, uint8 *buf, uint32 bufLength) 
{
	if(TCTaskStarted())
		return i2cIntRead(targetAddress, buf, bufLength);
	else
		return i2cPollRead(targetAddress, buf, bufLength);
}

uint32 i2cWriteValue(uint16 targetAddress, uint8 value)
{
	uint8 buf = value;
	i2cWrite(targetAddress, &buf, 1);
	return 0;
}

uint32 i2cWrite2Value(uint16 targetAddress, uint8 v1, uint8 v2)
{
	uint8 buf[2];
	buf[0] = v1; 
	buf[1] = v2;
	i2cWrite(targetAddress, &buf[0], 2);
	return 0;
}

//always master, never a slave
void i2cSetup(i2c_mode_t addrMode, i2c_speed_t speed)
{
	_i2cIntDisable();
	i2cCurrentAddress = MPTR(IC_TAR) & 0xFFF;
	// Disable slave, enable restart, disable 10bit slave, master enabled
	MPTR(IC_CON) = 0x41 | ((speed & 0x3) << 1);	

	//disable all interrupts
	MPTR(IC_INTR_MASK) = 0x0;	
	if(addrMode == I2C_10_BIT_ADDR) 
		MPTR(IC_CON) |= 0x10;

	switch(speed) {
	case I2C_STANDARD_SPEED:
		MPTR(IC_SS_HCNT) = 0xF5;  // Setup scl generator to 100kHz @ 49.152MHZ pclk
		MPTR(IC_SS_LCNT) = 0xF5;
		break;
	case I2C_FAST_SPEED:
		// Setup scl generator to 400kHz @ 49.152MHZ pclk
		// High is 1.2 us, low is 1.3 us (1.3 is the minial required by spec for low.)
		MPTR(IC_FS_HCNT) = 0x3b;
		MPTR(IC_FS_LCNT) = 0x40;
		break;
	case I2C_HIGH_SPEED:
	default:
		//not supported 
		break;
	};
	_i2cIntEnable();
};

HRESULT i2cInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	//enabel trace to debug the startup.
	//sysTraceStart(0);

	MPTR(IC_INTR_MASK) = 0; //disable all i2c interrupt
	MPTR(IC_RX_TL) = 0; //rx threshhold to 1, to correct error in the databook
	hResult = TCSemaphoreOpenIRQ(&i2cBufSemId, 1);
	if (hResult != NO_ERROR) 
		return hResult;

	hResult = TCSemaphoreOpenIRQ(&i2cCompleteSemId, 0);
	if (hResult != NO_ERROR) 
		return hResult;

	TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_I2C, NULL, i2cIntDsr);

	i2cSetup(I2C_7_BIT_ADDR, I2C_STANDARD_SPEED);
#ifdef _CLI
	hResult += i2cInstallCLITools();
#endif

	return hResult;
}

#endif //_I2C

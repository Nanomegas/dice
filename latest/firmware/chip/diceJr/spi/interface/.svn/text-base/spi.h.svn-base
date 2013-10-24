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

#include "TCTypes.h"
#include "ErrorCodes.h"

#ifdef _SPI
#ifndef _H_SPI_H_
#define _H_SPI_H_

typedef enum {
	SPI_MSBF_MSB_FIRST = 0,
	SPI_MSBF_LSB_FIRST,
} spi_msbf_t;

typedef enum {
	SPI_CLRIUR_TX_LAST = 0,
	SPI_CLRIUR_TX_ZERO,
} spi_clriur_t;

typedef enum {
	SPI_KPOLD_OVERWRITE = 0,
	SPI_KPOLD_KEEP_OLD
} spi_kpold_t;

typedef enum {
	SPI_PHSASE_0 = 0,
	SPI_PHSASE_1,
} spi_phase_t;

typedef enum {
	SPI_POLARITY_0 = 0,
	SPI_POLARITY_1,
} spi_polarity_t;

typedef enum {
	SPI_WSIZE_8 =0,
	SPI_WSIZE_16,
	SPI_WSIZE_24,
	SPI_WSIZE_32
} spi_wsize_t;

typedef enum {
	SPI_SET_SS_ROUTINE = 0,
	SPI_COMPLETE_CB,
	SPI_RATE,
	SPI_MSBF,
	SPI_CLRIUR,
	SPI_KPOLD,
	SPI_PHASE,
	SPI_POLARITY,
	SPI_WSIZE,
	SPI_CMD_MAX
} spi_cmd_t;

//defines for backward compatibility
#define SPI_ERROR_INVALID_SLAVEID		E_SPI_INVALID_SLAVEID		
#define SPI_ERROR_TX_ERROR				E_SPI_TX_ERROR			
#define SPI_ERROR_RX_ERROR				E_SPI_RX_ERROR			
#define SPI_ERROR_NO_FREE_DEVICE		E_SPI_NO_FREE_DEVICE		
#define SPI_ERROR_UNKNOWN_CMD			E_SPI_UNKNOWN_CMD			
#define SPI_ERROR_BUF_FULL				E_SPI_BUF_FULL			

typedef void (*spi_ss_cb_t)(uint32);
typedef void (*spi_complete_cb_t)(uint32 sid, uint32 * value); //ML: changed to return pointer to value

HRESULT spiGetSlaveDevice(uint32 *slaveId);
HRESULT spiFreeSlaveDevice(uint32 slaveId);

HRESULT spiConfigure(uint32 slaveId, spi_cmd_t cmd, void *arg);
HRESULT spiOpNonBlock(uint32 slaveId, uint32 outData, uint32 *inData);
HRESULT spiOpBlockNoTask(uint32 slaveId, uint32 outData, uint32 *inData);

//Primitive SPI add to buffer, this is interrupt safe if _SPI_INTERRUPT_SAFE is defined
BOOL _spiBufAdd(uint32 slaveId, uint32 outData, uint32 *indata);

HRESULT spiInitialize(void);
#endif //_H_SPI_H_
#endif //_SPI

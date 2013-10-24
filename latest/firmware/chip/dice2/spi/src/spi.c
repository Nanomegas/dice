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
#include "TCDebug.h"
#include "coredefs.h"
#include "spi.h"

static char 		moduleName[] = __FILE__ ;

#define SPI_MAX_SLAVE_COUNT 10

typedef struct {
	BOOL				bValid;
	uint16				rate;
	uint8				clkph;
	uint8				clkpl;
	uint8				wsize;
	uint8				lsbfrst;	
} spi_slave_t;


static spi_ss_cb_t spiSSRoutine= NULL;

static spi_slave_t ssTable[SPI_MAX_SLAVE_COUNT];



// Last spic slave ID. 
static uint32 lastSpiSSId = SPI_MAX_SLAVE_COUNT; //mean this is the first spiOp


static uint32 spi_ck_msk;
static uint32 spi_mosi_msk;
static uint32 spi_miso_msk;
static uint8  spi_clkph;
static uint8  spi_clkpl;
static uint16 spi_wait;
static uint8  spi_wsize;
static uint8  spi_lsbfrst;	


_TC_INLINE_STATIC void spiWait(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void spiWait(void)
{
volatile uint16 i;
	i = spi_wait;
	while (i--);
}

#define GPIO_DOUT (*((volatile uint32 *) GPIO_A_DR))
#define GPIO_DIN  (*((volatile uint32 *) GPIO_EXT_A))
#define CLK_ON	(GPIO_DOUT|=spi_ck_msk)
#define CLK_OFF	(GPIO_DOUT&=~spi_ck_msk)
#define CLK_TGL	(GPIO_DOUT^=spi_ck_msk)
#define MOSI_DATA(d) \
	{\
		if (d) {GPIO_DOUT|=spi_mosi_msk;}\
		else {GPIO_DOUT&=~spi_mosi_msk;}\
	}

#define MISO_DATA	(GPIO_DIN & spi_miso_msk)

void spiSetSlaveConfig(uint32 ssID)
{
	spi_clkph = ssTable[ssID].clkph;
	spi_clkpl = ssTable[ssID].clkpl;
	spi_wait = ssTable[ssID].rate;
	spi_wsize = (ssTable[ssID].wsize+1)*8;
	spi_lsbfrst = ssTable[ssID].lsbfrst;

	if (spi_clkpl)
	{
		CLK_ON;
	}
	else
	{
		CLK_OFF;
	}	
}


static uint32 bangBits(uint32 out)
{
	uint32 msk;
	uint32 in=0;
	uint8  n=spi_wsize;
	
	if (spi_lsbfrst)
	{
		msk = 1;
	}
	else
	{
		msk = 1<<(spi_wsize-1);
	}
	
	if (spi_clkph)
	{
		(*spiSSRoutine)(lastSpiSSId); 
		spiWait();
		while (n--)
		{
			CLK_TGL;
			MOSI_DATA(out&msk);
			spiWait();
			if (MISO_DATA) in|=msk;
			CLK_TGL;
			spiWait();
			if (spi_lsbfrst)
				msk<<=1;
			else
				msk>>=1;
		}
		spiWait();
		(*spiSSRoutine)(0xffffffff);
		spiWait();
	}
	else
	{
		(*spiSSRoutine)(lastSpiSSId); 
		spiWait();
		while (n--)
		{
			MOSI_DATA(out&msk);
			spiWait();
			if (MISO_DATA) in|=msk;
			CLK_TGL;
			spiWait();
			if (spi_lsbfrst)
				msk<<=1;
			else
				msk>>=1;
			CLK_TGL;
		}
		spiWait();
		(*spiSSRoutine)(0xffffffff);
		spiWait();	
	}
	return in;
}



HRESULT spiGetSlaveDevice(uint32 *slaveId)
{
	int i;
	for(i = 0; i < SPI_MAX_SLAVE_COUNT; i++) {
		if (!ssTable[i].bValid) {
			ssTable[i].bValid = TRUE;
			*slaveId = i;
			ssTable[i].rate = 0;		//set the default value
			ssTable[i].clkph = 1;
			ssTable[i].clkpl = 1;
			ssTable[i].wsize = 1;
			ssTable[i].lsbfrst = 0;	
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

	if (cmd==SPI_SET_SS_ROUTINE)
	{
		spiSSRoutine = (spi_ss_cb_t)arg;
		return hResult;
	}
	if((slaveId >= SPI_MAX_SLAVE_COUNT) || !ssTable[slaveId].bValid )
	{
		hResult = E_SPI_INVALID_SLAVEID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	switch(cmd) 
	{
		case SPI_RATE:
			ssTable[i].rate = (uint32)arg;
			break;
		case SPI_MSBF:
			ssTable[i].lsbfrst = (uint32)arg & 0x1;
			break;
		case SPI_POLARITY:
			ssTable[i].clkph = (uint32)arg & 0x1;
			break;
		case SPI_PHASE:
			ssTable[i].clkph = (uint32)arg & 0x1;
			break;
		case SPI_WSIZE:
			ssTable[i].wsize = (uint32)arg & 0x3;
			break;
		default:
			return SPI_ERROR_UNKNOWN_CMD;
			break;
	}
	return hResult;
}





HRESULT spiOpBlock(uint32 slaveId, uint32 outData, uint32 *inData)
{
	uint32 w;
	
	HRESULT hResult=NO_ERROR;
	if((slaveId >= SPI_MAX_SLAVE_COUNT) || !ssTable[slaveId].bValid)
	{
		hResult = E_SPI_INVALID_SLAVEID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if(!spiSSRoutine)
	{
		hResult = E_SPI_INVALID_SLAVEID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
		
	if(lastSpiSSId != slaveId)
	{
		lastSpiSSId = slaveId;
		spiSetSlaveConfig(lastSpiSSId);
	}
	w = bangBits(outData);
	if (inData) *inData = w;
	return hResult;
}


HRESULT spiInitialize(uint8 gpioMOSI, uint8 gpioMISO, uint8 gpioCK)
{
	spi_ck_msk = (uint32)1<<(gpioCK-1);
	spi_mosi_msk = (uint32)1<<(gpioMOSI-1);
	spi_miso_msk = (uint32)1<<(gpioMISO-1);	
	return NO_ERROR;
}

#endif //_SPI

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
		SPI internal definations.
	Revisions:
		created 02/03/2007 zluo

****************************************************************************/
#ifndef _SPIDEFS_H_
#define _SPIDEFS_H_

#define MPTR(x) (*((volatile uint32 *) (x)))
// SPI addresses
#define SPI_CTRL				(SPIBASE + 0x0)
#define SPI_STAT				(SPIBASE + 0x4)
#define SPI_DATA				(SPIBASE + 0xc)
#define	SPI_INT_MASK			(SPIBASE + 0x8)
#define SPI_BAUD_RATE			(SPIBASE + 0x10)

#define SPI_TX_DONE				BIT4
#define SPI_TX_UNDER_RUN		BIT3
#define SPI_RX_OVERFLOW			BIT2
#define SPI_TX_EMPTY			BIT1
#define SPI_RX_FULL				BIT0

typedef	union 
{
	uint32 reg;
	struct
	{
		unsigned	mstr		: 1;	//master mode
		unsigned	msbf		: 1;	//bit tranmit first: 0: MSB first, 1: LSB first
		unsigned	clriur		: 1;	//data fill at underrun: 0: tx last, 1: tx all 0s.
		unsigned	kpold		: 1;	//rx overrun: 0: overwrite old, 1:discard new
		unsigned	clkphase	: 1;	//spi clock phase
		unsigned	clkpl		: 1;	//spi clock polarity
		unsigned	wsize		: 2;	//word size: 00:8bit, 01:16bit, 10:24bit, 11:32bit.
		unsigned				: 24;	// Not used
	} bit;
} uSPI_CTRL;

typedef	union 
{
	uint32 reg;
	struct
	{
		unsigned	rate		: 16;	//rate
		unsigned				: 16;	// Not used
	} bit;
} uSPI_BAUD_RATE;

typedef struct {
	BOOL				bValid;
	uSPI_CTRL			config;
	uSPI_BAUD_RATE		rate;
	spi_complete_cb_t	complete_cb;		
} spi_slave_t;

// Now is the op queue for SPI.
typedef struct {
	uint32				ssId;
	BOOL				bPending;
	uint32				outData;
	uint32				*inData;
} spi_op_t;

#ifndef SPI_MAX_SLAVE_COUNT
#define SPI_MAX_SLAVE_COUNT 10
#endif //SPI_MAX_SLAVE_COUNT

#ifndef SPI_MAX_BUF_SIZE
#define SPI_MAX_BUF_SIZE 128
#endif // SPI_MAX_BUF_SIZE

#endif //_SPIDEFS_H_

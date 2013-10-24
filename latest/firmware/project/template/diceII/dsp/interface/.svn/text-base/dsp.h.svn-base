/*===================================================================*
  Copyright (c) 2002 TC Applied Technologies Ltd.
  -------------------------------------------------------------------
  $Header: /usr/local/cvsroot/firmware/project/projectEVM/dsp/interface/dsp.h,v 1.2 2007/02/08 17:59:33 zluo Exp $
 *===================================================================*/ 

 /*********************************************************************
 * DESCRIPTION:

	This module implements the 
		
	DSP Module for DSP HI08 interface
		
	Revisions:
		created 12/11/2004 lm

***********************************************************************/

#ifndef _DSP_H
#define _DSP_H

#include "TCTypes.h"

/* *************************************************************** */

#define X_MEM 0x02
#define Y_MEM 0x04
#define P_MEM 0x00

// move to ..ErrorCodes
/**************************** DSP **************************************/

#define E_DSP					0xC4000000	// DSP Errors

/**************************** DSP **************************************/
#define E_DSP_HOST_OK			(E_DSP+1)
#define E_DSP_SETADDR			(E_DSP+2)
#define E_DSP_PUTDATA			(E_DSP+3)
#define E_DSP_GETDATA			(E_DSP+4)
#define E_DSP_ISDSP				(E_DSP+5)
#define E_DSP_WAITTRDY			(E_DSP+6)
#define E_DSP_WAITNOTHC			(E_DSP+7)
#define E_DSP_WAITTXDE			(E_DSP+8)
#define E_DSP_WAITRXDF			(E_DSP+9)

/**************************** DSP **************************************/

#define DSP_ICR_REG				0x00
#define DSP_CVR_REG				0x01
#define DSP_ISR_REG				0x02
#define DSP_IVR_REG				0x03
#define DSP_NOT_USED			0x04
#define DSP_H_REG				0x05
#define DSP_M_REG				0x06
#define DSP_L_REG				0x07
								
#define ISR_RXDF_BIT			0x01
#define ISR_TXDE_BIT			0x02
#define ISR_TRDY_BIT			0x04
								
#define ICR_HF0_BIT				0x08        // Used for commands from ARM -> DSP
#define ICR_HF1_BIT				0x10        // Used for data frame ready signal from ARM -> DSP
								
#define ISR_HF2_BIT				0x08        // Used for commands from DSP -> ARM
#define ISR_HF3_BIT				0x10        // Used for data frame ready signal from DSP -> ARM
								
#define DSP_ADDR				(0x64 / 2)
#define DSP_READX				(0x66 / 2)
#define DSP_READY				(0x68 / 2)
#define DSP_READP				(0x6a / 2)
#define DSP_WRITEX				(0x6c / 2)
#define DSP_WRITEY				(0x6e / 2)
#define DSP_WRITEP				(0x70 / 2)
								
#define DSP_TIMEOUT_MSECS		100

#define	DSP_BASE_ADDR			0x05000000	// HI08 interface

__inline uint32	hi08Read(uint32 offset)
				{
					return ((uint32) ((*((uint16 *) (DSP_BASE_ADDR + 2 * offset))) >> 8));
				}

__inline void	hi08Write(uint32 offset, uint32 value)
				{
					(*((uint16 *) (DSP_BASE_ADDR + 2 * offset))) = (uint16) (value << 8);
				}

__inline uint32	dspGetMem(BYTE m, uint32 a) { return m; };
__inline uint32	dspGetAddr(BYTE m, uint32 a) { return a; };

BOOL			dspIsPresent(void);
BOOL			dspIsPresentAndRunning(void);
HRESULT			dspCheckPresentAndRunning(void);
HRESULT			dspReset(void);

HRESULT			dspReadMem(uint8 mem, uint32 addr, uint32 *data);
HRESULT			dspWriteMem(uint8 mem, uint32 addr, uint32 data);
HRESULT			dspLoadCode(BYTE *code, uint32 size, uint32 start);
				
HRESULT			dspInitialize(void);

#endif //_DSP_H


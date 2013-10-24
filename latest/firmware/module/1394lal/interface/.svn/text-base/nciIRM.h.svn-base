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
 * DESCRIPTION: Node Controller Interface Isochronous Resource Manager

	Purpose: Node Controller Interface 
				Isochronous Resource Manager functions
				
	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#ifndef _NCIIRM_H
#define _NCIIRM_H

#include "TCTypes.h"
#include "1394defs.h"

/*****************************************************************************

	Implemented Serial Bus Registers
		
		IRM Registers

		CSR_SB_BM_ID			0xF000021C
		CSR_SB_BW_AVAIL			0xF0000220
		CSR_SB_CH_AVAIL_HI		0xF0000224	channels 0  (BIT0) thru 31 (BIT31)
		CSR_SB_CH_AVAIL_LO		0xF0000228	channels 32 (BIT0) thru 63 (BIT31)
			a zero in a bit position indicates the channel is in use

*****************************************************************************/

#ifdef _IRM
BOOL		nciIRMIsThisNodeIRM(void);
HRESULT		nciIRMGetNodeAddrIRM(uint32 *pAddr);
#endif //_IRM

#ifdef _IRMC_CAPS

/* Serial Bus IRM Registers */
typedef struct
{
	uint32	BM_Id;
	uint32	BW_Avail;
	uint32	CH_AvailHi;
	uint32	CH_AvailLo;
	uint32	Broadcast_Chan;
} IRM_REGS;

/*************************************************************

	nciIRMInitialize

*****************************

	Description:
		Function initializes the module
		mutex for lock access and sets CSR registers to their power-on/bus-reset defaults.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT	nciIRMInitialize(void);

/*************************************************************

	nciIRMReset

*****************************

	Description:
		Function initializes the CHANNELS_AVAILABLE and BANDWIDTH_AVAILABLE
		CSR registers to their power-on/bus-reset defaults.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT	nciIRMReset(void);

/*************************************************************

	nciIRMRead

*****************************

	Description:
		Function provides read access to IRM registers.
	Return:
		RSP_COMPLETE indicates success.  Any other values indicate an error.

****************************/
RCODE_1394	nciIRMRead(uint32 irmReg, uint32* readVal);


/*************************************************************

	nciIRMLock

*****************************

	Description:
		Function provides threadsafe lock access to IRM registers.
	Return:
		RSP_COMPLETE indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/
RCODE_1394 nciIRMLock(uint32 irmReg,
						uint32 lockArg,
						uint32 lockData,
						uint32* lockReturnData);

/*************************************************************

	nciIRMWrite

*****************************

	Description:
		Function provides threadsafe write access to IRM registers.
	Return:
		RSP_COMPLETE indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/
RCODE_1394 nciIRMWrite(uint32 irmReg,
						uint32 data);


/***************************************************************************/
#endif //_IRMC_CAPS

#endif

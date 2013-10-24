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
	
	Purpose:	AVC Driver Ext Status. This file defines the extended
				status bits used in the communication
	Revisions:
		adapted to AVC 01/10/2006 bk
****************************************************************************/

#ifndef _AVC_DRIVER_EXTST_H_
#define	_AVC_DRIVER_EXTST_H_


#define	DD_EXT_STATUS_AES0_LOCKED		0x00000001
#define	DD_EXT_STATUS_AES1_LOCKED		0x00000002
#define	DD_EXT_STATUS_AES2_LOCKED		0x00000004
#define	DD_EXT_STATUS_AES3_LOCKED		0x00000008
#define	DD_EXT_STATUS_ADAT_LOCKED		0x00000010
#define	DD_EXT_STATUS_TDIF_LOCKED		0x00000020
#define	DD_EXT_STATUS_ARX1_LOCKED		0x00000040
#define	DD_EXT_STATUS_ARX2_LOCKED		0x00000080
#define	DD_EXT_STATUS_ARX3_LOCKED		0x00000100
#define	DD_EXT_STATUS_ARX4_LOCKED		0x00000200
#define	DD_EXT_STATUS_AES0_SLIP			0x00010000
#define	DD_EXT_STATUS_AES1_SLIP			0x00020000
#define	DD_EXT_STATUS_AES2_SLIP			0x00040000
#define	DD_EXT_STATUS_AES3_SLIP			0x00080000
#define	DD_EXT_STATUS_ADAT_SLIP			0x00100000
#define	DD_EXT_STATUS_TDIF_SLIP			0x00200000
#define	DD_EXT_STATUS_ARX1_SLIP			0x00400000
#define	DD_EXT_STATUS_ARX2_SLIP			0x00800000
#define	DD_EXT_STATUS_ARX3_SLIP			0x01000000
#define	DD_EXT_STATUS_ARX4_SLIP			0x02000000



#endif //_AVC_DRIVER_EXTST_H_

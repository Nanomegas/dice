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

 DESCRIPTION:

	Defines for packetBlock Create and Destroy Identifier

  Revisions:
		created 14/10/2003 LM

****************************************************************************/

#ifndef _PACKETBLOCKDEFS_H
#define _PACKETBLOCKDEFS_H

#define	PB_CREATE_LAL_ASYNC				0x00000100
#define	PB_DONE_LAL_ASYNC				0x00000100

#define	PB_DONE_LHL_PENDING_CLEAR		0x00000200
#define	PB_DONE_LHL_PENDING_AGEOUT		0x00000201

#define	PB_CREATE_LHL_REQ				0x00000300
#define	PB_DONE_LHL_REQ					0x00000300

#define	PB_CREATE_LHL_RSP				0x00000400
#define	PB_DONE_LHL_RSP					0x00000400

#define	PB_DONE_LHL_ARX_REQ				0x00000500
#define	PB_DONE_LHL_ARX_RSP_1			0x00000501
#define	PB_DONE_LHL_ARX_RSP_2			0x00000502
#define	PB_DONE_LHL_ARX_RSP_3			0x00000503
#define	PB_DONE_LHL_ARX_RSP_4			0x00000504
#define	PB_DONE_LHL_ARX_STREAM			0x00000505

#define	PB_DONE_LHL_ATX_SND_PEND_1		0x00000600
#define	PB_DONE_LHL_ATX_SND_PEND_2		0x00000601
#define	PB_DONE_LHL_ATX_SND_PEND_3		0x00000602
#define	PB_DONE_LHL_ATX_SND_DONE		0x00000603

#define	PB_CREATE_NCI_CB				0x00000700
#define	PB_DONE_NCI_CB					0x00000700
										
#define	PB_CREATE_OHCI_ARDMA			0x00000800
#define	PB_DONE_OHCI_ARDMA				0x00000800

#define	PB_CREATE_PB_TEST_CREATE		0x00000900
#define	PB_DONE_PB_TEST_DONE			0x00000900
#define	PB_DONE_PB_CB_CREATOR			0x00000901

#define	PB_CREATE_CLI_CB				0x00000A00
#define	PB_DONE_CLI_CB					0x00000A00

#define	PB_CREATE_TEST_PCORE_CB_RD		0x00001000
#define	PB_CREATE_TEST_PCORE_CB_WR		0x00001001
#define	PB_DONE_TEST_PCORE_CB			0x00001000
#define	PB_CREATE_TXP_PCORE_CB			0x00001100
#define	PB_DONE_TXP_PCORE_CB			0x00001100
#define	PB_DONE_DEBUG_PCORE_CB_DATA		0x00001200
#define	PB_DONE_DEBUG_PCORE_CB_DEBUG	0x00001201

#define	PB_CREATE_CMP_GEN_RSP			0x00002000
#define	PB_DONE_CMP_CB					0x00002000

#define	PB_CREATE_AVC_SEND_RSP			0x00003000
#define	PB_DONE_AVC_SEND_RSP			0x00003000
#define	PB_CREATE_AVC_CB				0x00003001
#define	PB_DONE_AVC_CB					0x00003001
#define	PB_DONE_AVC_HANDLE_THREAD		0x00003002

#define	PB_CREATE_AKE_CB				0x00004000
#define	PB_DONE_AKE_CB					0x00004000

#define	PB_CREATE_GENERIC				0x00005000
#define	PB_DONE_GENERIC					0x00005000

#endif //_PACKETBLOCKDEFS_H

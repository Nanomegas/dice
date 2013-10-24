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
//===========================================================================
#ifndef _AVS_AML_H_
#define _AVS_AML_H_

#define AVS_TX_STREAM     2
#define AVS_RX_STREAM     2
#define AVS_STREAM				(AVS_TX_STREAM+AVS_RX_STREAM)      //2 in and 2 out 
#define AVS_RX_FIRST      AVS_TX_STREAM
#define AVS_CHAN_PER_STREAM		8  //DO NOT CHANGE

/////
///// Assume port=dev%8
///// Assume tx_strm=dev/8
///// Assume rx_strm=dev/8-AVS_RX_FIRST

#define		AVS_AML_TX1		0x00
#define		AVS_AML_TX2		0x08
#define		AVS_AML_RX1		0x10
#define		AVS_AML_RX2		0x18

extern midi_dev_cb_t avs_midi_dev_cbs;

#endif //_AVS_AML_H_

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
#ifndef _UART_AML_H_
#define _UART_AML_H_

extern midi_dev_cb_t uart1_midi_dev_cbs;


//API to get activity since last call.

#define UART_RX_ACTIVITY  0x01
#define UART_TX_ACTIVITY  0x02

extern uint8 uart1_activity (void);


#endif //_UART_AML_H_

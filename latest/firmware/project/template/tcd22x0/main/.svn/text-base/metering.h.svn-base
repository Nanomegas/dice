//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies  Ltd.(ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	metering.h - Example of a configurable metering system which
				utilize the DICE Jr/Mini metering functionality.
	
	Revisions:
		created 30/10/2007 ml

****************************************************************************/
#ifndef _METERING_H_
#define	_METERING_H_

//Must be called when the router changes
void meterUpdateIndices (void);

//Returns the bit mask for the meter LED's
uint8 meterGetLED (void);

//Sets the multy (8ch) mode of the meter
void meterMulti(uint8 ismulti);

//Sets the channel information for the meter(s)
HRESULT meterSetCh (uint8 meter, uint8 in,uint8 dev, uint8 ch);

#endif//_METERING_H_

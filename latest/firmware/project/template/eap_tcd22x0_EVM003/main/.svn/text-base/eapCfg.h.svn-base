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
	
	Purpose:	facDefJR.h - Header for Factory Default DICE JR definition
	
	Revisions:
		created 03/03/2007 ml



****************************************************************************/
#ifndef _EAP_CFG_H_
#define	_EAP_CFG_H_

#include "eap.h"

extern const EAP_INIT_DESC eapInitDesc;

extern HRESULT eapCallBack (EAP_EVT_ENUM event, uint32 data);


// Add device specific storage here, these variables can
// also be accessed by the host. When changed from the host
// the eapCallBack function will be called.
// The data are backed up in persistent storage if enabled in
// EAP. If thae values are changed by the firmware application
// and need to be stored eapPrivatePersistChanged() should be called
typedef struct {
	uint32 cfgBits; //just an example
	uint32 reserved[3];
} APP_SPACE;

extern APP_SPACE appSpace;




#endif//_EAP_CFG_H_

//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004-2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	DICE  Enhanced Application Protocol Internal structures
	
	Revisions:
		created 19/02/2007 ml
****************************************************************************/
/** @addtogroup EAP

	Exposes an Enhanced Application configuration protocol for control panel control
	of routing, mixing, peak detection, storage of settings etc.

	This module can be used from the command prompt by the @ref cli_eap commands.

	The EAP module takes advantage of DAL and DiceDriver. The main application 
	configures EAP and pass on information from DiceDriver and DAL,
	@{
*/

#ifndef _EAP_INTERNALS_H_
#define _EAP_INTERNALS_H_

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "dal.h"
#include "diceDriver.h"
#include "eap.h"


extern EAP_CAPABILITIES eapCapabilities;

extern EAP_NEW_ROUTING eapNewRouting;

extern EAP_NEW_STRM_CFG eapNewStrmCfg;

extern EAP_CURR_CFG eapCurrCfg;

extern EAP_STND_ALONE_CFG eapStndAloneCfg;

extern EAP_COMMAND eapCommand;

typedef struct
{
	EAP_CALLBACK_FUNC	appCB;
	DAL_RATE_MODE_ENUM  currRateMode;
	uint16				nbPrivatePersistData; 
	void *				pPrivatePersistData; //only applicable with FLASH_EN
	uint16				nbPersistData;
	void *				pPersistData;
} EAP_DEVINFO;

extern EAP_DEVINFO devInfo;


extern BOOL eapIsMixerEnabled (void);
extern BOOL eapIsMixerWriteable (void);
extern BOOL eapIsMixerStored (void);
extern BOOL eapIsRouterEnabled (void);
extern BOOL eapIsRouterWriteable (void);
extern BOOL eapIsRouterStored (void);
extern BOOL eapAreStreamsConfigurable (void);
extern BOOL eapIsFlashSupported (void);
extern BOOL eapIsPeakSupported (void);
extern uint8 eapMaxTxStreams (void);
extern uint8 eapMaxRxStreams (void);
extern BOOL eapIsStrmCfgStored (void);
extern BOOL eapIsAutoStoreSupported (void);

//
#endif  //_EAP_INTERNALS_H_

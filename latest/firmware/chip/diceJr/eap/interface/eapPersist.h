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
	
	Purpose:	DICE  Enhanced Application Protocol Persistent Storage
	
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

#ifndef _EAP_PERSIST_H_
#define _EAP_PERSIST_H_

#include "TCTypes.h"
#include "ErrorCodes.h"

extern uint16 eapGetPersistSize (void);

extern HRESULT eapCopyPrivateToPersist (void);
extern HRESULT eapCopyPersistToPrivate (void);
extern HRESULT eapCopyCurrToPersist (void);
extern HRESULT eapCopyPersistToCurr (void);
extern HRESULT eapCopyMixerToPersist (void);



#endif  //_EAP_PERSIST_H_

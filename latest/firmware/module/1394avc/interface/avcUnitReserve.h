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

	Implementation of SUBUNIT INFO avc unit command
	used by the avcHandler, this module handles reservations.
	including setup/teardown, and filtering.

  Revisions:
		created 08/04/2003 LM

****************************************************************************/
#ifndef _AVCUNITRESERVE_H
#define _AVCUNITRESERVE_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcDefs.h"
#include "avcUtils.h"
#include "avcUnitUtils.h"
#include "lal.h"
#include "listmanager.h"

#define AVC_RESERVE_NO_TEXT_STRING		"\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"

enum
{
	RESERVE_MODE_NOTIFY		= 0,
	RESERVE_MODE_RESERVE,
	RESERVE_MODE_BUSRESET
};

typedef	struct
{
	uint32				priority;
	char				text[AVC_RESERVE_TEXT_SIZE];
	AVC_ADDRESS_SUBUNIT	addrSubunit;
	uint32				mode;
} RESERVE_DATA;

LM_CONTEXT*	avcUnitReserveGetReservationList(void);
LM_CONTEXT*	avcUnitReserveGetNotifyList(void);

HRESULT		avcUnitReserveGetStoredPriority(uint32 priority, uint32 *store);

HRESULT		avcUnitReserveCheck(AVC_HEADER *pHeader, PB *packetBlock);

HRESULT		avcUnitReserveStatus(AVC_ADDRESS_SUBUNIT *addrSubunit, BYTE *priority, BYTE *text_data);
HRESULT		avcUnitReserveNotify(PB *packetBlock, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 priority, BYTE *text_data);
HRESULT		avcUnitReserveControl(PB *packetBlock, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 priority, BYTE *text_data);

HRESULT		avcUnitReserveNotifyCheck(void);

void		avcUnitReserveRemoveNotifications(void);
void		avcUnitReserveMarkAsUnEstablished(void);
void		avcUnitReserveRemoveNonEstablished(void);

void		avcUnitReserveThread(void *dummy);

HRESULT		avcUnitReserveResumeTasks(void);									// Resume tasks for this module
HRESULT		avcUnitReserveSuspendTasks(void);									// Suspend tasks for this module
HRESULT		avcUnitReserveInitializeTasks(void);								// Create tasks for this module
HRESULT		avcUnitReserveInitialize(void);										// Initialization of this module

#endif //_AVCUNITRESERVE_H


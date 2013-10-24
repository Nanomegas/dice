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

	Implementation of SUBUNIT RESERVE avc unit command
	used by the avcHandler, this module handles reservations.
	including setup/teardown, and filtering.

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcUnitReserve.h"
#include "avcUnitCli.h"
#include "avcUnitNotify.h"
#include "fcp.h"
#include "listmanager.h"

// task (thread) IDs for this module
uint32					avcUnitReserveThreadID;			// avcUnitReserve handler thread
const char*				avcUnitReserveThreadName = "avcUnitReserveThread";

#define ACR_MAX_RESERVATIONS		10
#define	ACR_MAX_NOTIFICATONS		10

static	BYTE			reservationList_BUFFER[LM_LISTSIZE(ACR_MAX_RESERVATIONS, sizeof(RESERVE_NOTIFY))];
static	LM_CONTEXT*		reservationList = (LM_CONTEXT *) reservationList_BUFFER;

static	BYTE			notificationList_BUFFER[LM_LISTSIZE(ACR_MAX_NOTIFICATONS, sizeof(RESERVE_NOTIFY))];
static	LM_CONTEXT*		notificationList = (LM_CONTEXT *) notificationList_BUFFER;

static	BOOL			avcUnitReserveInitialized = FALSE;

#define ACR_POST_BUS_RESET_WAITTIME		10000		// wait 10 seconds af bus reset

LM_CONTEXT*	avcUnitReserveGetReservationList(void)
{
	return reservationList;
}
LM_CONTEXT*	avcUnitReserveGetNotifyList(void)
{
	return notificationList;
}

// control entry point for the avchandler: all AVC packets are filtered through here
// check to see if the specified subunit is reserved by another controller, or other type of node
HRESULT avcUnitReserveCheck(AVC_HEADER *pHeader, PB *packetBlock)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	RESERVE_DATA*		reservation = NULL;		
	uint32				pos = 0;
	uint32				idx = 0;
	BOOL				bFound = FALSE;

	if (pHeader->opcode == AVC_CMD_RESERVE ||
		pHeader->ctype != AVC_CTYPE_CONTROL) return hResult;	// no RESERVE and no non-control commands are filtered

	// determine if there is a reservation on the specified subunit
	DO_FOREVER
	{
		hResult = lmGetNthElement(reservationList, (void **) &notify, pos++, &idx);
		if (hResult != NO_ERROR) return NO_ERROR;

		reservation = &notify->data;

		if (reservation->mode == RESERVE_MODE_BUSRESET)
		{
			hResult = E_PKT_AVC_REJECTED;
			bFound = TRUE;
		}
		else if (avcSubunitEqual(&reservation->addrSubunit, &pHeader->addrSubunit) ||
				avcSubunitIsUnit(&reservation->addrSubunit))		// if reservation on unit all subunit control commands must be checked
		{
			if (avcUnitNotifyCheckAddress(packetBlock, (UNION_NOTIFY *) notify))
			{
				hResult = NO_ERROR;	
			}
			else
			{
				hResult = E_PKT_AVC_REJECTED;
			}
			bFound = TRUE;
		}

		lmReleaseElement(reservationList, idx);

		if (bFound) break;
	}	

	return hResult;
}

// determine if there is a reservation, notification on the specified subunit
static BOOL avcUnitReserveListFind(LM_CONTEXT* notifyList, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 *index, RESERVE_NOTIFY **element, BOOL bRelease)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	RESERVE_DATA*		reservation = NULL;		
	uint32				idx = 0;
	uint32				pos = 0;
	BOOL				bFound = FALSE;
	
	DO_FOREVER
	{
		hResult = lmGetNthElement(notifyList, (void **) &notify, pos++, &idx);
		if (hResult != NO_ERROR) break;

		reservation = &notify->data;

		bFound = avcSubunitEqual(&reservation->addrSubunit, addrSubunit);

		if (bFound == FALSE || bRelease)
		{
			lmReleaseElement(notifyList, idx);
		}

		if (bFound)
		{
			if (index) *index = idx;
			if (element) *element = notify;
			break;
		}
	}	

	return bFound;
}

static HRESULT avcUnitReserveGetPriority(RESERVE_DATA* reservation, BYTE *priority, BYTE *text_data, uint32 *mode)
{
	HRESULT		hResult = NO_ERROR;

	*priority =	(BYTE) reservation->priority;
	memcpy(text_data, reservation->text, AVC_RESERVE_TEXT_SIZE);
	*mode = reservation->mode;

	return hResult;
}

static HRESULT avcUnitReserveSetPriority(RESERVE_DATA* reservation, BYTE priority, BYTE *text_data, uint32 mode)
{
	HRESULT		hResult = NO_ERROR;
	uint32		stored = 0;

	hResult = avcUnitReserveGetStoredPriority(priority, &stored);
	if (hResult != NO_ERROR) return hResult;

	if (priority == 0)
	{
		// no reservation on this subunittype and ID
		reservation->priority = 0;
		memcpy(reservation->text, AVC_RESERVE_NO_TEXT_STRING, AVC_RESERVE_TEXT_SIZE);
	}
	else
	{
		// a reservation on this subunittype and ID
		reservation->priority = stored;
		memcpy(reservation->text, text_data, AVC_RESERVE_TEXT_SIZE);
	}

	reservation->mode = mode;

	return hResult;
}

HRESULT avcUnitReserveGetStoredPriority(uint32 priority, uint32 *stored)
{
	HRESULT		hResult = NO_ERROR;

	if (priority < 0x02 ||
		priority == 0x0f)
	{
		*stored = priority;
	}
	else if (priority < 0x0f)
	{
		*stored = priority & 0x0e;
	}
	else
	{
		hResult = E_BAD_INPUT_PARAMETERS;
	}

	return hResult;
}

HRESULT avcUnitReserveStatus(AVC_ADDRESS_SUBUNIT *addrSubunit, BYTE *priority, BYTE *text_data)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	RESERVE_DATA*		reservation = NULL;		
	BOOL				bFound = FALSE;
	uint32				index = 0;
	uint32				mode = RESERVE_MODE_RESERVE;
	
	// determine if there is a reservation on the specified subunit
	bFound = avcUnitReserveListFind(reservationList, addrSubunit, &index, &notify, FALSE);

	reservation = &notify->data;

	if (bFound == TRUE)
	{
		// there has been a reservation made on this subunittype and ID
		avcUnitReserveGetPriority(reservation, priority, text_data, &mode);
	}
	else
	{
		// no reservation has been made on this subunit so respond that the function was accepted
		*priority = 0;
		memcpy(text_data, AVC_RESERVE_NO_TEXT_STRING, AVC_RESERVE_TEXT_SIZE);
	}

	lmReleaseElement(reservationList, index);

	return hResult;
}

static HRESULT avcUnitReserveListAdd(LM_CONTEXT* notifyList, PB *packetBlock, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 priority, BYTE *text_data, uint32 mode, BOOL bUsePacketBlock)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY		notify;
	RESERVE_DATA*		reservation = NULL;		
				
	// make a list entry
	memset(&notify, 0, sizeof(notify));
	reservation = &notify.data;
	avcSubunitCopySubunit(&reservation->addrSubunit, addrSubunit);
	hResult = avcUnitReserveSetPriority(reservation, (BYTE) priority, text_data, mode);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcUnitNotifyAdd(notifyList, (UNION_NOTIFY *) &notify, packetBlock, bUsePacketBlock);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitReserveNotify(PB *packetBlock, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 priority, BYTE *text_data)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	RESERVE_DATA*		reservation = NULL;		
	BOOL				bFound = FALSE;
	uint32				index = 0;
	uint32				mode = RESERVE_MODE_RESERVE;

	// determine if there is a reservation on the specified subunit
	bFound = avcUnitReserveListFind(reservationList, addrSubunit, &index, &notify, FALSE);

	if (bFound == TRUE)
	{
		reservation = &notify->data;
		mode = reservation->mode;
	}

	lmReleaseElement(reservationList, index);

	if (bFound == FALSE ||				// return a REJECTED packet because notifications on non existing reservations is undefined
		mode == RESERVE_MODE_BUSRESET)	// return a REJECTED packet because notifications on reservations in bus reset is undefined
	{
		return E_PKT_AVC_REJECTED;	
	}

	// there has been a reservation made on this subunittype and ID
	hResult = avcUnitReserveListAdd(notificationList, packetBlock, addrSubunit, priority, text_data, FALSE, TRUE);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
				
static HRESULT avcUnitReserveNotifyCheckCB(UNION_NOTIFY *notify, BOOL *bChanged)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		reserveNotify = (RESERVE_NOTIFY *) notify;
	RESERVE_DATA*		reservation = NULL;		
	BYTE				priority = 0;
	BYTE				text_data[AVC_RESERVE_TEXT_SIZE];

	reservation = &reserveNotify->data;

	hResult = avcUnitReserveStatus(&reservation->addrSubunit, &priority, text_data);
	if (hResult != NO_ERROR) return hResult;

	*bChanged = (reservation->priority != priority);

	return hResult;
}

static HRESULT avcUnitReserveNotifyUpdateCB(UNION_NOTIFY *notify, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		reserveNotify = (RESERVE_NOTIFY *) notify;
	RESERVE_DATA*		reservation = NULL;		
	BYTE				priority = 0;
	BYTE				text_data[AVC_RESERVE_TEXT_SIZE];

	reservation = &reserveNotify->data;

	hResult = avcUnitReserveStatus(&reservation->addrSubunit, &priority, text_data);
	if (hResult != NO_ERROR) return hResult;
		
	hResult = dsWrite8Bits(pStream, priority);								// operand[0]
	if (hResult != NO_ERROR) return hResult;
	hResult = dsWriteChunk(pStream, AVC_RESERVE_TEXT_SIZE, (char *) text_data);		// operand[1,..,12]
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitReserveNotifyCheck(void)
{
	return avcUnitNotifyCheck(notificationList, avcUnitReserveNotifyCheckCB, avcUnitReserveNotifyUpdateCB);
}

static HRESULT avcUnitReserveReservationAdd(PB *packetBlock, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 priority, BYTE *text_data)
{
	return avcUnitReserveListAdd(reservationList, packetBlock, addrSubunit, priority, text_data, TRUE, FALSE);
}

static HRESULT avcUnitReserveReservationRemove(RESERVE_NOTIFY* notify)
{
	return avcUnitNotifyRemoveComm((UNION_NOTIFY *) notify);
}

static HRESULT avcUnitReserveNotificationRemove(RESERVE_NOTIFY* notify)
{
	return avcUnitNotifyRemoveComm((UNION_NOTIFY *) notify);
}

// control commands: add/remove a reservation
static HRESULT avcUnitReserveReservationUpdate(PB *packetBlock, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 priority, BYTE *text_data)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	RESERVE_DATA*		reservation = NULL;		
	BOOL				bFound = FALSE;
	uint32				index = 0;

	// determine if there is a reservation on the specified subunit
	bFound = avcUnitReserveListFind(reservationList, addrSubunit, &index, &notify, FALSE);
	if (bFound == FALSE)
	{
		// return a REJECTED packet because reservation update on non reserved subunits is undefined.
		return E_PKT_AVC_REJECTED;	
	}

	reservation = &notify->data;

	// check if this came from the same node that made this reservation before
	if (avcUnitNotifyCheckAddress(packetBlock, (UNION_NOTIFY *) notify))
	{
		if (priority > 0)
		{	
			// modify an existing reservation
			hResult = avcUnitReserveSetPriority(reservation, (BYTE) priority, text_data, RESERVE_MODE_RESERVE);
			if (hResult != NO_ERROR) return hResult;
			lmReleaseElement(reservationList, index);
		}
		else
		{
			// remove existing reservation made on this subunittype and ID
			avcUnitReserveReservationRemove(notify);
			lmReleaseElement(reservationList, index);
			lmRemoveElement(reservationList, index);
		}
	}
	else
	{
		if (priority > reservation->priority) // compare stored priority or command priority of reservation
		{	
			// update to a new reservation with higher priority on this subunittype and ID
			// remove existing reservation made on this subunittype and ID
			avcUnitReserveReservationRemove(notify);
			lmReleaseElement(reservationList, index);
			lmRemoveElement(reservationList, index);
			// add new reservation made on this subunittype and ID
			hResult = avcUnitReserveReservationAdd(packetBlock, addrSubunit, priority, text_data);
		}
		else
		{
			lmReleaseElement(reservationList, index);
			return E_PKT_AVC_REJECTED;
		}
	}

	// notify after adding new or removing/updating existing reservation
	avcUnitReserveNotifyCheck();

	return hResult;
}

// control commands: add/update/remove a reservation
HRESULT avcUnitReserveControl(PB *packetBlock, AVC_ADDRESS_SUBUNIT *addrSubunit, uint32 priority, BYTE *text_data)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bFound = FALSE;

	// determine if there is a reservation on the specified subunit
	bFound = avcUnitReserveListFind(reservationList, addrSubunit, NULL, NULL, TRUE);

	if (bFound == TRUE)
	{
		// update existing reservation made on this subunittype and ID
		hResult = avcUnitReserveReservationUpdate(packetBlock, addrSubunit, priority, text_data);
	}
	else
	{
		// add new reservation made on this subunittype and ID
		
		//LM??? check unit/subunit reservations
		
		hResult = avcUnitReserveReservationAdd(packetBlock, addrSubunit, priority, text_data);
	}

	return hResult;
}

void avcUnitReserveRemoveNotifications(void)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	uint32				idx = 0;
	uint32				pos = 0;
	
	DO_FOREVER
	{
		hResult = lmGetNthElement(notificationList, (void **) &notify, pos, &idx);
		if (hResult != NO_ERROR) break;

		// remove notification made on this subunittype and ID
		avcUnitReserveNotificationRemove(notify);
		lmReleaseElement(notificationList, idx);
		lmRemoveElement(notificationList, idx);
	}	
}

void avcUnitReserveMarkAsUnEstablished(void)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	RESERVE_DATA*		reservation = NULL;		
	uint32				idx = 0;
	uint32				pos = 0;
	
	DO_FOREVER
	{
		hResult = lmGetNthElement(reservationList, (void **) &notify, pos++, &idx);
		if (hResult != NO_ERROR) break;

		reservation = &notify->data;
		avcUnitReserveSetPriority(reservation, 0, NULL, RESERVE_MODE_BUSRESET);

		lmReleaseElement(reservationList, idx);		
	}	

	// notify after adding new or removing/updating existing reservation
	//LM??? avcUnitReserveNotifyCheck();
}

void avcUnitReserveRemoveNonEstablished(void)
{
	HRESULT				hResult = NO_ERROR;
	RESERVE_NOTIFY*		notify = NULL;
	RESERVE_DATA*		reservation = NULL;		
	uint32				idx = 0;
	uint32				pos = 0;
	
	DO_FOREVER
	{
		hResult = lmGetNthElement(reservationList, (void **) &notify, pos, &idx);
		if (hResult != NO_ERROR) break;

		reservation = &notify->data;

		if (reservation->mode == RESERVE_MODE_BUSRESET)
		{
			// remove non established reservation made on this subunittype and ID
			avcUnitReserveReservationRemove(notify);
			lmReleaseElement(reservationList, idx);
			lmRemoveElement(reservationList, idx);
		}
		else
		{
			lmReleaseElement(reservationList, idx);
			pos++;
		}
	}	

	// notify after adding new or removing/updating existing reservation
	//LM??? avcUnitReserveNotifyCheck();
}

/*********************************************************
	Thread waiting for and process incoming request packets
*/
void avcUnitReserveThread(void *dummy)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bResetDetected = FALSE;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		// wait for bus reset completion
		lalWaitOnResetCompletion(&bResetDetected);

		if (avcUnitReserveInitialized == FALSE) continue;
	
		// Remove all notifications because of bus reset
		avcUnitReserveRemoveNotifications();
		// Mark all Reservations as non established during bus reset
		avcUnitReserveMarkAsUnEstablished();
	
		// wait until ten seconds has elapsed since the bus reset, or another reset occurs
		hResult = lalWaitTimeSinceResetCompletion(ACR_POST_BUS_RESET_WAITTIME);
		if (hResult != NO_ERROR) continue;

		avcUnitReserveRemoveNonEstablished();

		// wait for bus reset indication
		lalWaitOnResetIndication(&bResetDetected);
	}
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT avcUnitReserveResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avcUnitReserve handler thread
	hResult = TCTaskResume(avcUnitReserveThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avcUnitReserveSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avcUnitReserve handler thread
	hResult = TCTaskSuspend(avcUnitReserveThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT avcUnitReserveInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avcUnitReserve handler thread
	hResult = TCTaskCreate(&avcUnitReserveThreadID, avcUnitReserveThreadName, avcUnitReserveThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitReserveInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lmCreateList(reservationList, LM_LISTSIZE(ACR_MAX_RESERVATIONS, sizeof(RESERVE_NOTIFY)), sizeof(RESERVE_NOTIFY), NULL);
	if (hResult != NO_ERROR) return hResult;
	hResult = lmCreateList(notificationList, LM_LISTSIZE(ACR_MAX_NOTIFICATONS, sizeof(RESERVE_NOTIFY)), sizeof(RESERVE_NOTIFY), NULL);
	if (hResult != NO_ERROR) return hResult;
	
	avcUnitReserveInitialized = TRUE;
	
	return hResult;
}

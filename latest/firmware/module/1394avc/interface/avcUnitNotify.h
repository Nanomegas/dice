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

	Implementation of avc unit notify utilities

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifndef _AVCUNITNOTIFY_H
#define _AVCUNITNOTIFY_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "listmanager.h"
#include "avcDefs.h"
#include "avcUnitPower.h"
#include "avcUnitReserve.h"
#include "avcUnitChannelUsage.h"
#include "avcUnitConnect.h"
#include "avcUnitConnectAV.h"
#include "avcUnitSignalFormat.h"				// INPUT and OUTPUT PLUG SIGNAL FORMAT
#include "avcUnitSignalSource.h"

enum
{
	NOTIFY_TYPE_POWER,
	NOTIFY_TYPE_RESERVE,
	NOTIFY_TYPE_CHANNEL_USAGE,
	NOTIFY_TYPE_CONNECT,
	NOTIFY_TYPE_CONNECT_AV,
	NOTIFY_SIGNAL_FORMAT,						// INPUT and OUTPUT PLUG SIGNAL FORMAT
	NOTIFY_SIGNAL_SOURCE
};

typedef	struct
{
	BOOL				bUsePacketBlock;
	PB*					packetBlock;
	NODEHANDLE			handle;					// uint32 nodeAddr;
} NOTIFY_COMM;									// Notify communication 

typedef	struct
{
	NOTIFY_COMM			comm;
	POWER_DATA			data;
} POWER_NOTIFY;

typedef	struct
{
	NOTIFY_COMM			comm;
	RESERVE_DATA		data;
} RESERVE_NOTIFY;

typedef	struct
{
	NOTIFY_COMM			comm;
	CHANNEL_USAGE_DATA	data;
} CHANNEL_USAGE_NOTIFY;

typedef	struct
{
	NOTIFY_COMM			comm;
	CONNECT_DATA		data;
} CONNECT_NOTIFY;

typedef	struct
{
	NOTIFY_COMM			comm;
	CONNECT_AV_DATA		data;
} CONNECT_AV_NOTIFY;

typedef	struct
{
	NOTIFY_COMM			comm;
	SIGNAL_FORMAT_DATA	data;
} SIGNAL_FORMAT_NOTIFY;							// INPUT and OUTPUT PLUG SIGNAL FORMAT

typedef	struct
{
	NOTIFY_COMM			comm;
	SIGNAL_SOURCE_DATA	data;
} SIGNAL_SOURCE_NOTIFY;

typedef union
{
	POWER_NOTIFY			powerNotify;
	RESERVE_NOTIFY			reserveNotify;
	CHANNEL_USAGE_NOTIFY	channelUsageNotify;
	CONNECT_NOTIFY			connectNotify;
	CONNECT_AV_NOTIFY		connectAVNotify;
	SIGNAL_FORMAT_NOTIFY	signalFormatNotify;	// INPUT and OUTPUT PLUG SIGNAL FORMAT
	SIGNAL_SOURCE_NOTIFY	signalSourceNotify;
	NOTIFY_COMM				notifyComm;
} UNION_NOTIFY;

typedef HRESULT (* NOTIFY_CHANGE_CALLBACK)(void);
typedef HRESULT (* NOTIFY_CHECK_CALLBACK)(UNION_NOTIFY *notify, BOOL *bChanged);
typedef HRESULT (* NOTIFY_UPDATE_CALLBACK)(UNION_NOTIFY *notify, pDataStream pStream);

HRESULT			avcUnitNotifyAdd(LM_CONTEXT* notifyList, UNION_NOTIFY *notify, PB *packetBlock, BOOL bUsePacketBlock);
HRESULT			avcUnitNotifyRemoveOld(LM_CONTEXT* notifyList, uint32 nodeAddr);
HRESULT			avcUnitNotifyCheck(LM_CONTEXT* notifyList, NOTIFY_CHECK_CALLBACK notifyCheckCB, NOTIFY_UPDATE_CALLBACK notifyUpdateCB);
BOOL			avcUnitNotifyCheckAddress(PB *packetBlock, UNION_NOTIFY *notify);
HRESULT			avcUnitNotifyRemoveComm(UNION_NOTIFY *notify);
HRESULT			avcUnitNotifyChange(NOTIFY_CHANGE_CALLBACK notifyChange);

void			avcUnitNotifyThread(void *dummy);

HRESULT			avcUnitNotifyCreateMessageQueues(void);
HRESULT			avcUnitNotifyResumeTasks(void);									// Resume tasks for this module
HRESULT			avcUnitNotifySuspendTasks(void);								// Suspend tasks for this module
HRESULT			avcUnitNotifyInitializeTasks(void);								// Create tasks for this module
HRESULT			avcUnitNotifyInitialize(void);									// Initialization of this module

#endif //_AVCUNITNOTIFY_H


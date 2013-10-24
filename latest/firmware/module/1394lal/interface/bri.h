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
 * DESCRIPTION: Bus Reset Interface

	Purpose: Bus Reset Interface functions

		This module provides high-level access to low-level bus reset events.
		Ideally, applications would be isolated from bus resets, but there are
		a few situations where there needs be a connection.

		getCurrentBusGeneration - 
			Returns the current bus generation.  Applications may want to compare
			this value with the generation in a packet block.

		waitOnResetComplete -
			If a bus reset is in progress, this function suspends the caller until
			the reset has been completely handled.  If no reset is in progress, this
			function returns immediately.

		registerBRIndicationCallback - 
			Registers a callback to be invoked when a bus reset has begun.

		registerResetCompletionCB - 
			Registers a callback to be invoked when a bus reset has completed.

  Revisions:
		created 04/24/2002 lm
****************************************************************************/

#ifndef _BRI_H
#define _BRI_H

#include "1394.h"
#include "1394defs.h"
#include "TCTypes.h"
#include "bmdefs.h"
#include "lhlbri.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

#define _BRI_PRE_COMPLETION
//#undef _BRI_PRE_COMPLETION

#define BRI_MAX_BR_CBS					16
#define BRI_BUSRESET_STORM_THRESHOLD	5

typedef void (* BRI_CALLBACK)(void);										// callbacks registered for bus reset events look like this:
																			
HRESULT			briInitialize(void);										
HRESULT			briCreateSemaphores(void);									
BOOL			briInBusReset(void);										
 
_TC_INLINE BOOL	briNewBusReset(void) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL	briNewBusReset(void)										// Returns whether bus reset has happened during BR processing
				{
					return lhlBriInBusReset();
				}

void			briBusResetStart(void);										// On bus reset, sets event flags to perform the bus reset sequence 
void			briBusResetIndication(void);								// Called when a bus reset is first noticed by the Link layer.
HRESULT			briGetCurrentBusGeneration(uint32* pCurBusGen);				// Returns current bus generation. This is the number of times the node has seen a bus reset since hardware reset.
void			briSetCurrentBusGeneration(uint32 CurBusGen);				// Update the current bus generation number

																			// This is intended as a general, informative callback
																			// callee must not depend on being called at any particular point in the reset process.
HRESULT			briRegisterBRIndicationCallback (BRI_CALLBACK callback);	// Registers a callback to be invoked when a bus reset has begun
HRESULT			briRegisterBRCompletionCallback (BRI_CALLBACK callback);	// Registers a callback to be invoked when a bus reset has completed.

HRESULT			briWaitOnResetIndication (BOOL *bResetDetected);			// Wait for a bus reset indication
HRESULT			briWaitOnResetCompletion (BOOL *bResetDetected);			// Wait for a bus reset completion
HRESULT			briElapsedTimeSinceResetIndication(uint32 *elapsedTime);	// time elapsed since last bus reset indication
HRESULT			briElapsedTimeSinceResetCompletion(uint32 *elapsedTime);	// time elapsed since last bus reset completion
HRESULT			briWaitTimeSinceResetIndication(uint32 msec);				// wait until msec has elapsed since the bus reset indication, or since another reset indications occurs
HRESULT			briWaitTimeSinceResetCompletion(uint32 msec);				// wait until msec has elapsed since the bus reset completion, or since another reset completions occurs

#ifdef _BRI_PRE_COMPLETION
HRESULT			briWaitOnResetPreCompletion (BOOL *bResetDetected);			// Wait for a bus reset pre completion
HRESULT			briElapsedTimeSinceResetPreCompletion(uint32 *elapsedTime);	// time elapsed since last bus reset pre completion
HRESULT			briWaitTimeSinceResetPreCompletion(uint32 msec);			// wait until msec has elapsed since the bus reset pre completion, or since another reset completions occurs
#else //_BRI_PRE_COMPLETION
#define			briWaitOnResetPreCompletion				briWaitOnResetCompletion 
#define			briElapsedTimeSinceResetPreCompletion	briElapsedTimeSinceResetCompletion 
#define			briWaitTimeSinceResetPreCompletion		briWaitTimeSinceResetCompletion
#endif //_BRI_PRE_COMPLETION

HRESULT			briProcessSelfId(void);
void			briBuildSpeedMap(void);
void			briBuildTopologyMap(void);
void			briCallBusResetIndicationCBs(void);
HRESULT			briScanBus(void);								// Required if using nodeHandles
HRESULT			briBuildNodeInfos(void);
HRESULT     briUpdateNodeInfos(BOOL bClear);
HRESULT			briFixupHandles(void);
void			briCallBusResetCompletionCBs(void);
void			briBusResetCompletion(void);

HRESULT			briSignalOnResetIndication (void);
HRESULT			briSignalOnResetCompletion (void);

#ifdef _BRI_PRE_COMPLETION
void			briBusResetPreCompletion(void);
HRESULT			briSignalOnResetPreCompletion (void);
#else //_BRI_PRE_COMPLETION
void			briBusResetPreCompletion(void);
#define			briSignalOnResetPreCompletion			//briSignalOnResetCompletion
#endif //_BRI_PRE_COMPLETION

HRESULT			briGetBusID(uint16 *pBusId);

_TC_INLINE BOOL	briIsBroadcastNodeAddr(uint32 nodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL	briIsBroadcastNodeAddr(uint32 nodeAddr)
				{
					return (nodeAddr == BROADCAST_NODE_ADDR);
				}

BOOL			briIsThisNodeAddr(uint32 nodeAddr);
HRESULT			briGetThisNodeAddr(uint32 *pNodeAddr);

BOOL			briIsThisNodeRoot(void);
HRESULT			briGetNodeAddrRoot(uint32* pNodeAddr);

void			briGetLocalNodeInfo(LOCAL_NODEINFO *pNodeInfo);
void			briSetLocalNodeInfo(LOCAL_NODEINFO *pNodeInfo);

BIB_DATA*		briGetBusInfoBlockCache(void);
uint32			briGetBusInfoBlockCacheData(uint32 nodeIndex, uint32 dataIndex);

BOOL			briIsSelfIDsValid(void);
SELFID_BUNDLE*	briGetSelfIDBundle(void);
QUADLET*		briGetSIDs(void);
QUADLET			briGetIndexedSID(uint32 index);
uint32			briGetNumNodes(void);
uint32			briGetNumSIDQuads(void);

#endif

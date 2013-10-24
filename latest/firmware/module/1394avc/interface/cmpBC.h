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

/** @addtogroup CMP
    @{
*/

/** @defgroup CMPBC CMP Broadcast Connection

	This module implements the connection management procedures for
	broadcast connections.  Refer to IEC 61883-1 for details.

	@{
*/

#ifndef _CMPBC_H
#define _CMPBC_H

#include "packetBlock.h"
#include "TCTypes.h"
#include "lalDefs.h"
#include "cmpDefs.h"

typedef struct
{
	CONNECTION_STATUS 		status;
} CMP_BC_CONNECTION_OWNED;

/** Determine if a broadcast-in connection is established on the specified iPCR
	@param iPCRNumber		The index of the iPCR
	@param bEstablished		The address to place the result.  TRUE is a connection is established.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCInIsConnectionEstablished(uint32 iPCRNumber, BOOL *bEstablished);

/** Create or overlay a broadcast-in connection on the specified iPCR
	@param iPCRNumber		The index of the iPCR
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCInConnectionEstablish(uint32 iPCRNumber);

/** Break a broadcast-in connection on the specified iPCR
	@param iPCRNumber		The index of the iPCR
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCInConnectionBreak(uint32 iPCRNumber);

/** Restore a broadcast-in connection on the specified iPCR
	@param iPCRNumber		The index of the iPCR
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCInConnectionRestore(uint32 iPCRNumber);

/** Mark owned connections for restoring.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpBCInConnectionResetOwned(void);

/** Restore owned connections following a bus reset (re-establish all broadcast in connections previously established by us)
	@param bRestoreAllDone	The address to use to indicate if all restores are done.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpBCInConnectionRestoreOwned(BOOL *bRestoreAllDone);

/** Clear ownership of all non-established broadcast in connections previously established by us
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpBCInConnectionClearOwnedNonEstablished(void);



/** Determine if a broadcast-out connection is established on the specified oPCR
	@param oPCRNumber		The index of the oPCR
	@param bEstablished		The address to place the result.  TRUE is a connection is established.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCOutIsConnectionEstablished(uint32 oPCRNumber, BOOL *bEstablished);

/** Create or overlay a broadcast-out connection on the specified oPCR
	@param oPCRNumber		The index of the oPCR
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCOutConnectionEstablish(uint32 oPCRNumber);

/** Break a broadcast-out connection on the specified oPCR
	@param oPCRNumber		The index of the oPCR
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCOutConnectionBreak(uint32 oPCRNumber);

/** Restore a broadcast-out connection on the specified oPCR
	@param oPCRNumber		The index of the oPCR
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpBCOutConnectionRestore(uint32 oPCRNumber);

/** Mark owned connections for restoring.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpBCOutConnectionResetOwned(void);

/** Restore owned connections following a bus reset (re-establish all broadcast in connections previously established by us)
	@param bRestoreAllDone	The address to use to indicate if all restores are done.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpBCOutConnectionRestoreOwned(BOOL *bRestoreAllDone);

/** Clear ownership of all non-established broadcast out connections previously established by us
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpBCOutConnectionClearOwnedNonEstablished(void);

//! @}
//! @}


#endif //_CMPBC_H


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

/** @defgroup CMPP2P CMP Point-to-Point Connections

	This module implements the connection management procedures for
	point-to-point connections.  Refer to IEC 61883-1 for details.

	@{
*/


#ifndef _CMPP2P_H
#define _CMPP2P_H

#include "packetBlock.h"
#include "TCTypes.h"
#include "lalDefs.h"
#include "cmpDefs.h"

#define _CMP_P2P_USE_NODE_HANDLE
#undef _CMP_P2P_USE_NODE_HANDLE

typedef struct
{
#ifdef _CMP_P2P_USE_NODE_HANDLE
	LAL_NODE_HANDLE 		oNodeHandle;					// node handle of the source plug
#else //_CMP_P2P_USE_NODE_HANDLE
	uint16			 		oNodeAddr;						// nodeID of the source plug
#endif //_CMP_P2P_USE_NODE_HANDLE
	uint32					oPCRNumber;						// oPCR of the source plug
	CONNECTION_STATUS 		status;
} CMP_P2P_CONNECTION_OWNED;

/** Determine if a p2p connection is established
	@param iPCRNumber	The index of the iPCR on this node
	@param oPCRNumber	The index of the oPCR on the remote node
	@param oNodeAddr	The node address of the remote note
	@param bEstablished	The address to place the result in. True if a connection is established.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	cmpP2PInIsConnectionEstablished(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr, BOOL *bEstablished);

/** Create or overlay a p2p connection between a local iPCR and an oPCR on a remote node
	@param iPCRNumber	The index of the iPCR on this node
	@param oPCRNumber	The index of the oPCR on the remote node
	@param oNodeAddr	The node address of the remote note
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpP2PInConnectionEstablish(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr);

/** Break a p2p connection between a local iPCR and an oPCR on a remote node
	@param iPCRNumber	The index of the iPCR on this node
	@param oPCRNumber	The index of the oPCR on the remote node
	@param oNodeAddr	The node address of the remote note
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpP2PInConnectionBreak(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr);

/** Restore a p2p connection between a local iPCR and an oPCR on a remote node
	@param iPCRNumber	The index of the iPCR on this node
	@param oPCRNumber	The index of the oPCR on the remote node
	@param oNodeAddr	The node address of the remote note
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpP2PInConnectionRestore(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr);

/** Mark owned connections for restoring
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpP2PInConnectionResetOwned(void);

/** Restore owned connections following a bus reset (re-establish all p2p in connections previously established by us)
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpP2PInConnectionRestoreOwned(BOOL *bRestoreAllDone);

/** Clear ownership of all non-established p2p in connections previously established by us
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT cmpP2PInConnectionClearOwnedNonEstablished(void);

//! @}
//! @}
#endif //_CMPP2P_H

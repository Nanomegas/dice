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
* DESCRIPTION: Utility for doing lock transactions
	
	Revisions:
		created 20/09/2002 lm

****************************************************************************/

/** @addtogroup LockHandler
	A utility module for implementing lock transactions.
	@{
*/

#ifndef _LOCK_H
#define _LOCK_H

#include "TCTypes.h"
#include "PacketBlock.h"

#define LT_MASK_SWAP		BIT0	//!< Allow MASK_SWAP
#define LT_COMPARE_SWAP		BIT1	//!< Allow COMPARE_SWAP
#define LT_FETCH_ADD		BIT2	//!< Allow FETCH_ADD
#define LT_LITTLE_ADD		BIT3	//!< Allow LITTLE_ADD
#define LT_BOUNDED_ADD		BIT4	//!< Allow BOUNDED_ADD
#define LT_WRAP_ADD			BIT5	//!< Allow WRAP_ADD
#define LT_VENDOR_DEPENDENT BIT6	//!< Allow VENDOR_DEPENDENT
#define LT_ALLOW_64			BIT7	//!< Allow 64 bit operations

/** Perform a lock transaction.
	Will do the lock transaction on the register specified.
	The lock response is not sent.

	Possible error return values:

	E_LOCK_TYPE_NOT_IMPLEMENTED
	E_LOCK_TYPE_NOT_ALLOWED
	E_LOCK_TYPE_INVALID
    E_LOCK_TRANSACTION_INVALID

	Typical usage of this function from a txl calledback function
	
		- pbGetDestinationOffset
		- determine register
		- get semaphore for register
		- doLockTransaction
		- txlSendLockResponse
		- put semaphore for register

	NB: if LT_ALLOW_64 is specified, reg, oldValue, and modifyMask must point to 64 bit 
	structures (consecutive quadlets)	

	@param packetBlock			pointer to an incoming packet block
	@param reg					pointer to the register to do the lock transaction on
	@param oldValue				pointer to a holder for the old value of reg
	@param lockTypesAllowed		a bit field of LT_ types indicating what lock types are allowed.
	@param modifyMask			A 1 in the bit position indicates that bit can be modified.
	@return						NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		doLockTransaction(PB *packetBlock, uint32 *reg, uint32 *oldValue, uint32 lockTypesAllowed, uint32 *modifyMask);

#endif

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
 * DESCRIPTION: DiceDriver

	Purpose:	DiceDriver
				

	Revisions:
		created 21/12/2004 AliA
		rewritten 21/03/2005 ml

****************************************************************************/

#ifndef _DICE_DRIVER_H_
#define	_DICE_DRIVER_H_

#ifdef _DICE_DRIVER

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "dal.h"
#include "diceDriverExtStatus.h"
#include "dicedriverInterface.h"

/** @addtogroup DiceDriver

	This module provides an interface for the device to work with the Host DICE Driver. Please
	contact TC Applied Technologies Ltd. support to obtain the DICE Driver.

	<strong>
	Note that when using this Dice Driver, the build must not include CMP or mLAN.
	</strong>

	@{
	
*/



HRESULT		diceDriverResumeTasks(void);
HRESULT		diceDriverSuspendTasks(void);

/** Initializes the Dice Driver support
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		diceDriverInitialize(void);
HRESULT		diceDriverInitializeTasks(void);
/** Sends notification to Host
	@param notification		Bitmap indicating events
*/
void		diceDriverNotifyHost(uint32 notification);

/// DiceDriver callback parameters
typedef struct
{
	DAL_NOMINAL_RATE_ENUM	rate;			//!< nominal sample rate
	DAL_CLOCK_SOURCE_ENUM	source;			//!< clock source
	BOOL					bEnabled;		//!< enabled state
	BOOL					bAttached;		//!< if attached to a driver
} DICE_DRIVER_CB_DATA;

/// DiceDriver message codes
typedef enum 
{
	eDDR_MSG_CLOCK_CHG,			//!< ... sample rate change
	eDDR_MSG_ATTACH,			//!< ... host driver attached
	eDDR_MSG_DETACH,			//!< ... host driver detached
	eDDR_MSG_ENABLE,			//!< ... enable streaming
	eDDR_MSG_DISABLE,			//!< ... disable streaming
	eDDR_MSG_AC3_CHG			//!< ... AC# state change
} DICE_DRIVER_MSG_ENUM;

//! The function type for DiceDriver callbacks 
typedef HRESULT (*DICEDRIVER_CALLBACK_FUNC)(DICE_DRIVER_MSG_ENUM msg, DICE_DRIVER_CB_DATA * pData);

//! Stream configuration parameters
typedef struct 
{
	uint8  AudioChannels;		//!< number of audio channels
	uint8  midiChannels;		//!< number of MIDI channels
	char * names;				//!< ordered list of channel names
	uint16	AC3Capabilities;	//!< AC3 caps
} STREAM_CONFIG;


//! Configure the DICE for desired streaming settings, install callback for events
HRESULT diceDriverConfigure (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
                             char * nickName,
                             DICEDRIVER_CALLBACK_FUNC func);

//! Configure the DICE for desired streaming settings, install callback for events, maintaining clock caps and names for Host driver
HRESULT diceDriverConfigureNew (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
                             char * nickName,
                             DICEDRIVER_CALLBACK_FUNC func,
							 uint32 ClockCapabilities,
							 char* ClockSourceNames);

HRESULT diceDriverSetHardwareLatencies( uint32 txHwLatency, uint32 rxHwLatency );

HRESULT diceDriverSetSortID( uint32 sortID );

HRESULT diceDriverSetCapabilites( uint32 capabilities );
uint32 diceDriverGetCapabilites(void);

//! Configure the DICE for desired streaming settings only
HRESULT diceDriverConfigureStreaming (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
									  STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams);

//! Configure the DICE callback, nickname and clock source capabilities only
HRESULT diceDriverConfigureInitial   (char * nickName,
									  DICEDRIVER_CALLBACK_FUNC func,
							          uint32 ClockCapabilities,
							          char* ClockSourceNames);

DICE_DRIVER_STRUCT*	diceDriverGetState(void);

//! Set current clock rate, lock status, extended status
HRESULT diceDriverSetStatus (BOOL bLocked, DAL_NOMINAL_RATE_ENUM nominalRate, uint32 rateHz, uint32 extStatus);

//! Get pointer to current operating DiceDriver parameters
DICE_DRIVER_STRUCT*		diceDriverGetState(void);

//! @}

#endif//_DICE_DRIVER

#endif//_DICE_DRIVER_H_

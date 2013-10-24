//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
 * DESCRIPTION: AvcDriver

	Purpose:	AvcDriver
				Adapted directly from the DiceDriver module

	Revisions:
		created 03/20/2006 bk
****************************************************************************/

#ifndef _AVC_DRIVER_H_
#define	_AVC_DRIVER_H_

#ifdef _AVC

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "dal.h"
#include "avcDriverExtStatus.h"
#include "avcDriverInterface.h"

/** @addtogroup AvcDriver

	This modules provides an interface for the device to work with the AV/C Class Compliant Driver.

	<strong>
	Note that when using this AV/C Driver,the build must not include DiceDriver or mLAN.
	</strong>

	@{
	
*/

HRESULT		avcDriverResumeTasks(void);
HRESULT		avcDriverSuspendTasks(void);

/** Initializes the AV/C Driver support
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avcDriverInitialize(void);
HRESULT		avcDriverInitializeTasks(void);
/** Sends notification to Host
	@param notification		Bitmap indicating events
*/
void		avcDriverNotifyHost(uint32 notification);

/// AvcDriver callback parameters
typedef struct
{
	DAL_NOMINAL_RATE_ENUM	rate;			//!< nominal sample rate
	DAL_CLOCK_SOURCE_ENUM	source;			//!< clock source
	BOOL					bEnabled;		//!< enabled state
	BOOL					bAttached;		//!< if attached to a driver
} AVC_DRIVER_CB_DATA;

/// AvcDriver message codes
typedef enum 
{
	eAVCDR_MSG_CLOCK_CHG,		//!< ... sample rate change
	eAVCDR_MSG_ATTACH,			//!< ... host driver attached
	eAVCDR_MSG_DETACH,			//!< ... host driver detached
	eAVCDR_MSG_ENABLE,			//!< ... enable streaming
	eAVCDR_MSG_DISABLE,			//!< ... disable streaming
	eAVCDR_MSG_AC3_CHG			//!< ... AC3 state change
} AVC_DRIVER_MSG_ENUM;			

//! The function type for AvcDriver callbacks 
typedef HRESULT (*AVCDRIVER_CALLBACK_FUNC)(AVC_DRIVER_MSG_ENUM msg, AVC_DRIVER_CB_DATA * pData);

//! Stream configuration parameters
typedef struct 
{
	uint8  AudioChannels;		//!< number of audio channels
	uint8  midiChannels;		//!< number of MIDI channels
    char * names;				//!< ordered list of channel names
	uint16	AC3Capabilities;	//!< AC3 caps
} STREAM_CONFIG;


//! Configure the DICE for desired streaming settings, install callback for events
HRESULT avcDriverConfigure (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
                             char * nickName,
                             AVCDRIVER_CALLBACK_FUNC func);

//! Configure the DICE for desired streaming settings, install callback for events, maintaining clock caps and names for Host driver
HRESULT avcDriverConfigureNew (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
                             char * nickName,
                             AVCDRIVER_CALLBACK_FUNC func,
							 uint32 ClockCapabilities,
							 char* ClockSourceNames);

//! Configure the DICE for desired streaming settings only
HRESULT avcDriverConfigureStreaming (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
									  STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams);

//! Configure the DICE callback, nickname and clock source capabilities only
HRESULT avcDriverConfigureInitial   (char * nickName,
									  AVCDRIVER_CALLBACK_FUNC func,
							          uint32 ClockCapabilities,
							          char* ClockSourceNames);

//! Set current clock rate, lock status, extended status
HRESULT avcDriverSetStatus (BOOL bLocked, DAL_NOMINAL_RATE_ENUM nominalRate, uint32 rateHz, uint32 extStatus);

//! Get pointer to current operating AvcDriver parameters
AVC_DRIVER_STRUCT*		avcDriverGetState(void);

HRESULT		extFmt2DalRate(BYTE ext_fmt_rate, DAL_NOMINAL_RATE_ENUM *dal_rate);
HRESULT		dal2ExtFmtRate(DAL_NOMINAL_RATE_ENUM dal_rate, BYTE *ext_fmt_rate);

HRESULT		avcDriverSetRate(DAL_NOMINAL_RATE_ENUM dal_rate);
HRESULT		avcDriverGetRate(DAL_NOMINAL_RATE_ENUM *dal_rate);
BOOL		avcDriverPlugIsSyncSource(BYTE dst_su, BYTE dst_plug_id, BYTE src_su, BYTE src_plug_id);
BOOL		avcDriverPlugIsSyncPath(BYTE dst_su, BYTE dst_plug_id, BYTE src_su, BYTE src_plug_id);
HRESULT		avcDriverSetClockSrc(uint8 src_su, uint8 src_plug_id);
HRESULT		avcDriverGetClockSrc(DAL_CLOCK_SOURCE_ENUM *clkSrc);
HRESULT		avcDriverSetEnable(BOOL bEnable);
HRESULT		avcDriverSetCh(uint32 plugID, BOOL bTx, uint8 channel);


//! @}

#endif//_AVC

#endif//_AVC_DRIVER_H_

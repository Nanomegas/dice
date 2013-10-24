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
 * DESCRIPTION: Define the AVSystem API (AVS)

	Exposes an Isochronous Interface configuration API for stream
	format, direction, isoc channel, packet size, etc.

	An Isoc Channel is a 1394 channel, allocated from the Resource Manager
 	using CMP, usually via AV/C CONNECT commands.

  	A Media Channel is a logical channel managed by the FPGA logic that
 	represents a stream to or from a CODEC.

	Revisions:
		created 09/19/2002 lm

****************************************************************************/

/** @addtogroup AVS

	Exposes an Isochronous Interface configuration API for stream
	format, direction, isoc channel, packet size, etc.

	This module can be used from the command prompt by the @ref cli_avs commands.

	An Isoc Channel is a 1394 channel, allocated from the Resource Manager
 	using CMP, usually via AV/C CONNECT commands.

	These functions are typically used by CMP and AV/C stream control.

	@{
*/

#ifndef _AVS_H
#define _AVS_H

#include "avsDefs.h"
#include "avsEnvDefs.h"
#include "sysCli.h"

/*****************************************************************************

	Read / Write AVS registers

*****************************************************************************/

//! @defgroup AVSInt AVS Internal
//! @{


/** Reads a value from the AVS.
	@param registerOffset	The register offset to read.
	@param value			The read value will be placed at this location.
*/
_TC_INLINE void avsRegRead(uint32 registerOffset, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE void avsRegRead(uint32 registerOffset, uint32 *value)
// Read a value from the AVS
{
	avsEnvRegRead(registerOffset, value);

#ifdef _SYS_VERBOSE
	if (sysCliVerboseIsEnabled(SYS_VERBOSE_AVS_RD))
	{
		sysDebugPrintf("avs get 0x%08x:0x%08x\n\r", mAvsPtr(registerOffset), *value);
	}
#endif //_SYS_VERBOSE
}

/** Writes a value to the AVS.
	@param registerOffset	The register offset to write.
	@param value			The value to be written.
*/
_TC_INLINE void avsRegWrite(uint32 registerOffset, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void avsRegWrite(uint32 registerOffset, uint32 value)
// Write a value to the AVS
{
#ifdef _SYS_VERBOSE
	if (sysCliVerboseIsEnabled(SYS_VERBOSE_AVS_WR))
	{
		if (sysCliVerboseIsEnabled(SYS_VERBOSE_AVS_DIFF))
		{
			uint32 old;
			avsEnvRegRead(registerOffset, &old);
			avsEnvRegWrite(registerOffset, value);
			sysDebugPrintf("avs set 0x%08x:0x%08x\t(old/new 0x%08x:0x%08x)\n\r", mAvsPtr(registerOffset), value, old, *mAvsPtr(registerOffset));
		}
		else
		{
			avsEnvRegWrite(registerOffset, value);
			sysDebugPrintf("avs set 0x%08x:0x%08x\n\r", mAvsPtr(registerOffset), value);
		}
		return;
	}
#endif //_SYS_VERBOSE

	avsEnvRegWrite(registerOffset, value);
}
//! @}

/**	@defgroup AVsInit Module Initialization
	Initialization of the AVS module.
	#include avs.h
	@{
*/
//! Early initialization of this module.
HRESULT		avsInitialize(void);									
//! Late initialization of this module.
HRESULT		avsInitializeTasks(void);
//! Resume the tasks of this module.
HRESULT		avsResumeTasks(void);
//! Suspend the tasks of this module.
HRESULT		avsSuspendTasks(void);
//! @}


// functions for use by CMP, AV/C stream control and by DAL

/**	@defgroup AVSSC Stream Control
	Provides control of AVS streaming, channels, formats, and sample rates.
	@{
*/

/** Start streaming on a plug.
	@param plugID	The ID of the plug. (Tx: 1-2, Rx: 1-4)
	@param bTx      True if a Tx plug.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsStart(uint32 plugID, BOOL bTx);

/** Start streaming on a Tx plug.
	@param plugID	The ID of the plug (1 or 2).
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsStartTx(uint32 plugID);

/** Start streaming on a plug.
	@param plugID	The ID of the plug (1 to 4).
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsStartRx(uint32 plugID);

/** Stop streaming on a plug.
	@param plugID	The ID of the plug. (Tx: 1-2, Rx: 1-4)
	@param bTx      True if a Tx plug.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsStop(uint32 plugID, BOOL bTx);

/** Stop streaming on a Tx plug.
	@param plugID	The ID of the plug (1 or 2).
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsStopTx(uint32 plugID);

/** Stop streaming on a plug.
	@param plugID	The ID of the plug (1 to 4).
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsStopRx(uint32 plugID);

/** Query if a plug is streaming.
	@param plugID	The ID of the plug. (Tx: 1-2, Rx: 1-4)
	@param bTx      True if a Tx plug.
	@return			True is the plug is streaming.
*/
BOOL		avsIsStarted(uint32 plugID, BOOL bTx);

/** Query if a Tx plug is streaming.
	@param plugID	The ID of the plug (1 or 2).
	@return			True is the plug is streaming.
*/
BOOL		avsIsTxStarted(uint32 plugID);

/** Query if a Rx plug is streaming.
	@param plugID	The ID of the plug (1 to 4).
	@return			True is the plug is streaming.
*/
BOOL		avsIsRxStarted(uint32 plugID);

/** Set the isochronous channel of a plug.
	@param plugID	The ID of the plug. (Tx: 1-2, Rx: 1-4)
	@param bTx      True if a Tx plug.
	@param channel  The isochronous channel to use.
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetCh(uint32 plugID, BOOL bTx, uint8 channel);

/** Set the isochronous channel of a Tx plug.
	@param plugID	The ID of the plug (1 or 2).
	@param channel  The isochronous channel to use.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetTxCh(uint32 plugID, uint8 channel);

/** Set the isochronous channel of a Rx plug.
	@param plugID	The ID of the plug (1 to 4).
	@param channel  The isochronous channel to use.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetRxCh(uint32 plugID, uint8 channel);

/** Obtain the current isochronous channel of a plug.
	@param plugID	The ID of the plug. (Tx: 1-2, Rx: 1-4)
	@param bTx      True if a Tx plug.
	@param channel  The address to place the current channel.
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetCh(uint32 plugID, BOOL bTx, uint8 *channel);

/** Obtain the current isochronous channel of a Tx plug.
	@param plugID	The ID of the plug (1 or 2).
	@param channel  The address to place the current channel.
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetTxCh(uint32 plugID, uint8 *channel);

/** Obtain the current isochronous channel of a Rx plug.
	@param plugID	The ID of the plug (1 to 4).
	@param channel  The address to place the current channel.
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetRxCh(uint32 plugID, uint8 *channel);

/** Adjust the presentation time and phase for a Tx plug.
	@param plugID	The ID of the plug (1 or 2).
	@param samples  The number of samples to adjust. ( 0 <= samples <= 100).
	@param phase    The fine tuning phase to adjust in %. (0 <= phase <= 100).
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT	    avsSetTxPresntOffset(uint32 plugID, uint8 samples, uint8 phase);

/** Adjust the presentation time and phase for a Rx plug.
	@param plugID	The ID of the plug (1 to 4).
	@param samples  The number of samples to adjust. ( 0 <= samples <= 100).
	@param phase    The fine tuning phase to adjust in %. (0 <= phase <= 100).
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT	    avsSetRxPresntAdjust(uint32 plugID, int8 samples, uint8 phase);


/** Set the format in the CIP1 header for a plug
	@param plugID	The ID of the plug (Tx: 1-2, Rx: 1-4).
	@param bTx      True if a Tx plug.
	@param format   The format as defined by IEC 61883.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetFormat(uint32 plugID, BOOL bTx, uint32 format);

/** Set the format in the CIP1 header for a Tx plug
	@param plugID	The ID of the plug (1 or 2).
	@param format   The format as defined by IEC 61883.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetTxFormat(uint32 plugID, uint32 format);

/** Set the format in the CIP1 header for a Rx plug
	@param plugID	The ID of the plug (1 to 4).
	@param format   The format as defined by IEC 61883.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetRxFormat(uint32 plugID, uint32 format);

/** Get the format of the CIP1 header for a plug
	@param plugID	The ID of the plug (Tx: 1-2, Rx: 1-4).
	@param bTx      True if a Tx plug.
	@param format   The address to place the format in.  Value as defined by IEC 61883.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetFormat(uint32 plugID, BOOL bTx, uint32 *format);

/** Get the format of the CIP1 header for a Tx plug
	@param plugID	The ID of the plug (1 or 2).
	@param format   The address to place the format in.  Value as defined by IEC 61883.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetTxFormat(uint32 plugID, uint32 *format);

/** Get the format of the CIP1 header for a Rx plug
	@param plugID	The ID of the plug (1 to 4).
	@param format   The address to place the format in.  Value as defined by IEC 61883.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetRxFormat(uint32 plugID, uint32 *format);


// Set mute 
HRESULT		avsMute (uint32 plugID, BOOL bTx, BOOL online);

/** Set mute for a Tx plug.
	@param plugID	The ID of the plug (1 or 2).
	@param muteon	True if the plug is to be muted.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsTxMute(uint32 plugID, BOOL muteon);

/** Set mute for a Rx plug.
	@param plugID	The ID of the plug (1 to 4).
	@param muteon	True if plug is to be muted.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsRxMute(uint32 plugID, BOOL muteon);

/** Set sample rate for a given plug. Plug must be restarted after calling this function.
	@param plugID	The ID of the plug (Tx: 1-2, Rx: 1-4).
	@param bTx		True is a Tx plug.
	@param sfc		The same frequency code. See AVS_SFC.. definitions in avsDefs.h.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetSampleRate(uint32 plugID, BOOL bTx, uint8 sfc);
HRESULT		avsSetTxSampleRate(uint32 plugID, uint8 sfc);
HRESULT		avsSetRxSampleRate(uint32 plugID, uint8 sfc);

HRESULT		avsSetDualWire(uint32 plugID, BOOL bTx, uint8 bDualWire);
HRESULT		avsSetTxDualWire(uint32 plugID, uint8 bDualWire);
HRESULT		avsSetRxDualWire(uint32 plugID, uint8 bDualWire);


/** Set the data block size (number of audio channels) for a plug.  This is generally used for Tx plugs.
	@param plugID	The ID of the plug (Tx: 1-2, Rx: 1-4).
	@param bTx		True if a Tx plug.
	@param channels The number of data blocks, or channels, to use.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetDBS(uint32 plugID, BOOL bTx, uint32 channels);
HRESULT		avsSetTxDBS(uint32 plugID, uint32 channels);
HRESULT		avsSetRxDBS(uint32 plugID, uint32 channels);


/** Get number of audio channels for a given plug
	@param plugID	The ID of the plug (Tx: 1-2, Rx: 1-4).
	@param bTx		True is a Tx plug.
	@param channels The address to place the data block size, or number of channels, into.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetDBS(uint32 plugID, BOOL bTx, uint32 *channels);
HRESULT		avsGetTxDBS(uint32 plugID, uint32 * channels);
HRESULT		avsGetRxDBS(uint32 plugID, uint32 * channels);


/** Set the transmit speed for a Tx plug
	@param plugID	The ID of the plug (1 or 2).
	@param speed    The speed to use.  Use SPEED_400, SPEED_200, or SPEED_100 defined in 1394.h
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetTxSpeed(uint32 plugID, uint32 speed);

/** Obtain the current transmit speed for a Tx plug
	@param plugID	The ID of the plug (1 or 2)
	@param speed    The address to place the current speed in. Will be SPEED_400, SPEED_200, or SPEED_100 as defined in 1394.h
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsGetTxSpeed(uint32 plugID, uint32 *speed);

/**	Set the quadlet selection field (the sequence) for a given audio channel on a Rx plug.
	@param plugID	The ID of the plug (1 to 4)
	@param slot		The receive slot (1 to 16)
	@param value	The sequence to extract.  Must be less than the DBS.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetRxQSelSlot(uint32 plugID, uint8 slot, uint8 value);

/** Set plug on-line or off-line.  An offline plug will be muted.
	@param plugID	The ID of the plug (Tx: 1-2, Rx: 1-4).
	@param bTx		True is a Tx plug.
	@param online   True if plug is online.  False to mute plug.
	@return NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		avsSetOnline(uint32 plugID, BOOL bTx, BOOL online);

// return the indicated samplerate of a stream. Used by DAL
uint32		avsAudioRxGetReceivedSFC (uint32 plugID);

HRESULT		avsSetRxSepcifyFlags (uint32 plugID, uint8 bSpecifyDBS, uint8 bSpecifyFDF);


//! @}

//! @addtogroup AVSInt
//! @{



#define		EVT_AVS_SLIP			    0x00000001
#define		EVT_AVS_RPT				    0x00000002
#define		EVT_AVS_LOCK_ACQUIRED	0x00000004
#define		EVT_AVS_LOCK_LOST		  0x00000008
#define		EVT_AVS_ENABLE			  0x00000010
#define		EVT_AVS_DISABLE			  0x00000020

//! the AVS Rx state machine. This function should be called every 20ms
void        avsRxPlugStateMachine (uint32 plugID, uint8 * pEvent); /// Does nothing in DICE JR, 
  
//! Returns TRUE if AVS rx is in lock
BOOL		avsRxIsLocked(uint32 plugID);


void		avsTxPlugStateMachine (uint32 plugID, uint8 * pEvent);

/*****************************************************************************

	Functions for debugging and internal event handling

*****************************************************************************/

//! AVS system reset
HRESULT		avsBoardReset(void);

//! Returns TRUE if AVS rx is started and the current source Id
BOOL		avsRxGetSourceID(uint32 plugID, uint32 *pSourceID);

//! updating Tx plugs sourceID if changed af bus reset
HRESULT		avsTxCheckSourceID(void);


//! Flush media fifo for all active receivers
void		avsRxFlush(void);



//! return plugID for an active channel
HRESULT		avsGetPlugIDFromChannel(BOOL bTx, uint32 channel, BYTE *plugID);

//! @}



extern AVSPORT avsRxPort[NUM_AVSPORTS];
extern AVSPORT avsTxPort[NUM_AVSPORTS];

#endif //_AVS_H

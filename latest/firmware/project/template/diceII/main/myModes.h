//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies  Ltd.(ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	myModes.h - Header with generic mode structure
	
	Revisions:
		created 02/02/2005 arg


HOW TO ADD MORE MODES
- Create a myMode<n>.c file, use one of the existing as template
- Make the changes and REMEMBER to replace modeCfg<oldn> with  modeCfg<n>
- Increment MYMODES_NBMODES below
- In myModes.c add extern declaration of modeCfg<n> and add to modeDesc
- In main makefile add new c file
- Make.

****************************************************************************/
#ifndef _MYMODES_H_
#define	_MYMODES_H_

#include "dicedriver.h"

/** @addtogroup myMode
	A <b> Mode </b> is a configuration of DICE audio interfaces, routings, and clock rates. <br>
	The current mode settings are used by DiceDriver on the Host to configure to match. <br> <br>
	HOW TO ADD MORE MODES <br> <br>
	1) Create a myMode<n>.c file, use one of the existing as template <br>
	2) Make the changes and REMEMBER to replace modeCfg<oldn> with  modeCfg<n> <br>
	3) Increment MYMODES_NBMODES below <br>
	4) In <b> myModes .c </b> add extern declaration of modeCfg<n> and add to modeDesc <br>
	5) In main makefile add new c file <br>
	6) Make <br> <br>
	@{
*/

/// Stream Config
typedef struct {
  STREAM_CONFIG * pTxStreamCfg;		//!< tx channels, names, AC3 caps
  uint32          nbTxStreams;		//!< number of tx streams
  uint32          txHwLatency;		//!< hardware (A/D + DICE routing) latency in samples
  STREAM_CONFIG * pRxStreamCfg;		//!< rx channels, names, AC3 caps
  uint32          nbRxStreams;		//!< number of rx streams
  uint32          rxHwLatency;		//!< hardware (DICE routing + D/A) latency in samples
} STREAM_CFG;

#define MODE_INIT_MIDI   0x00000001 //Board indicated MIDI should be enabled
#define MODE_INIT_EXPN   0x00000002 //Expander board detected   

typedef HRESULT (*MODE_ROUTING_FUNC)(uint32 ratemode);
typedef HRESULT (*MODE_INIT_FUNC)(uint32 initFlags);

/// Mode Config
typedef struct {
   STREAM_CFG  * low;				//!< config for low sample rate range
   STREAM_CFG  * mid;				//!< config for mid sample rate range
   STREAM_CFG  * high;				//!< config for high sample rate range
   uint32       inputDevices;		//!< input audio interfaces in the router
   uint32       outputDevices;		//!< output audio interfaces in the router
   uint32       clocksCaps;			//!< clock rates and sync sources
   char *       clockNames;			//!< names for display on the host GUI
   MODE_ROUTING_FUNC fRouting;		//!< fn that handles routing for rate ranges
   MODE_INIT_FUNC    fInit;			//!< fn that initializes the modes
   char *       modeName;			//!< for CLI to display current
} MODE_CFG;

#define MYMODES_NBMODES 16

/// init
extern MODE_CFG * myModesInitialize (uint32 initFlags);
/// set mode
extern HRESULT myModeSetMode (uint32 mode);
//! @}




#endif//_MYAPP_H_

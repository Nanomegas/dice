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
 * DESCRIPTION: DRD definitions shared by PC and Device

	LM??? Here we have 'hardcoded' things that will eventually reside in descriptors.

	Revisions:
		created 09/25/2002 lm

****************************************************************************/

#ifndef _DRDDEFS_H
#define _DRDDEFS_H

// tuner presets
#define DRD_MIN_TUNER_PRESET_ID			1
#define DRD_MAX_TUNER_PRESET_ID			40

// audio channel numbering
#define DRD_MIN_AUDIO_CHANNEL_NUMBER	1
#define DRD_MAX_AUDIO_CHANNEL_NUMBER	8

// mode setting									// must match information in subunit descriptor !!!!!				
#define DRD_MODE_MONO					01
#define DRD_MODE_STEREO					02
#define DRD_MODE_DOLBY_PROLOGIC			03
#define DRD_MODE_AC3					04
										
#define DRD_MIN_MODE_SETTING			01
#define DRD_MAX_MODE_SETTING			99

// for each mode, how many speakers?			// must match information in subunit descriptor !!!!!
#define	DRD_NUM_SPEAKERS_MONO			1
#define	DRD_NUM_SPEAKERS_STEREO			2
#define	DRD_NUM_SPEAKERS_PROLOGIC		4
#define	DRD_NUM_SPEAKERS_AC3			6

#endif

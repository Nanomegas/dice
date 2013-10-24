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
/**********************************************************************
 * DESCRIPTION: AV/C definitions from 1394ta specifications

	Defines for all the values specified in the various 1394ta specifications

	Extended to cover
		AV/C Digital Interface Command Set General Specification, Version 4.1
	
	Original ported sources covered
		AV/C Digital Interface Command Set General Specification, Version 3.0
	
	Enhancements to the AV/C General Specication 3.0, Version 1.0FC2	
		AV/C Audio Sub-unit Specification, Version 1.0rc

	Revisions:
		created 09/25/2002 lm

***********************************************************************/

#ifndef _AVCDEFS_H
#define _AVCDEFS_H

#include "1394.h"
#include "TCTypes.h"

// SubUnit definition
typedef	uint32		SUBUNIT_TYPE;
typedef	uint32		SUBUNIT_ID;

// ctype
// commands
#define AVC_CTYPE_CONTROL							0x00
#define AVC_CTYPE_STATUS							0x01
#define AVC_CTYPE_SPECIFIC_INQUIRY					0x02
#define AVC_CTYPE_NOTIFY							0x03
#define AVC_CTYPE_GENERAL_INQUIRY					0x04

// response
#define AVC_RESPONSE_MASK_BIT						BIT3
#define AVC_RESPONSE_NOT_IMPLEMENTED				0x08
#define AVC_RESPONSE_ACCEPTED						0x09
#define AVC_RESPONSE_REJECTED						0x0A
#define AVC_RESPONSE_IN_TRANSITION					0x0B
#define AVC_RESPONSE_IMPLEMENTED					0x0C
#define AVC_RESPONSE_STABLE							0x0C
#define AVC_RESPONSE_CHANGED						0x0D
#define AVC_RESPONSE_reserved						0x0E
#define AVC_RESPONSE_INTERIM						0x0F

// subunit type
#define AVC_SU_TYPE_MONITOR							0x00
#define AVC_SU_TYPE_AUDIO							0x01		
#define AVC_SU_TYPE_PRINTER							0x02
#define AVC_SU_TYPE_DISC							0x03		// recorder/player
#define AVC_SU_TYPE_TAPE							0x04		// recorder/player
#define AVC_SU_TYPE_TUNER							0x05		
#define AVC_SU_TYPE_CA								0x06
#define AVC_SU_TYPE_CAMERA							0x07		
//#define AVC_SU_TYPE_RESERVED1						0x08
#define AVC_SU_TYPE_PANEL							0x09
#define AVC_SU_TYPE_BULLETIN_BOARD					0x0A
#define AVC_SU_TYPE_CAMERA_STORAGE					0x0B
#define AVC_SU_TYPE_MUSIC							0x0C
#define AVC_SU_TYPE_VENDOR_UNIQUE					0x1C		
//#define AVC_SU_TYPE_RESERVED_FOR_SU_TYPES			0x08
#define AVC_SU_TYPE_EXTEND_FIRST					0x1E		// extension from 1st 5-bits
#define AVC_SU_TYPE_UNIT							0x1F		// the "unit" subunit
#define AVC_SU_ANY_AVAILABLE						0xFF		// any available subunit
																
#define AVC_SU_TYPE_EXTENDED_reserved				0x00		
#define AVC_SU_TYPE_MIN_TYPE_EXTENDED				0x01		// minimum value in extended subunit type
#define AVC_SU_TYPE_MAX_TYPE_EXTENDED				0xFE		// maximum value in extended subunit type
#define AVC_SU_TYPE_EXTEND_EXTENDED					0xFF		// extension from extended byte
																
// subinit id													
#define AVC_SU_ID_MAX_INSTANCE_FIRST3				0x04		// first 3-bit id instances are 0-4
#define AVC_SU_ID_EXTEND_FIRST3						0x05		// extension from 1st 3-bits
#define AVC_SU_ID_reserved							0x06		
#define AVC_SU_ID_IGNORE							0x07		
																
#define AVC_SU_ID_EXTENDED_reserved					0x00		
#define AVC_SU_ID_MIN_INSTANCE_EXTENDED				0x01		// minimum instance in extended subunit id
#define AVC_SU_ID_MAX_INSTANCE_EXTENDED				0xFE		// maximum instance in extended subunit id
#define AVC_SU_ID_EXTEND_EXTENDED					0xFF		// extension from extended byte
																
// opcodes														
																
// Unit commands												
#define AVC_CMD_DIGITAL_OUTPUT						0x10		
#define AVC_CMD_DIGITAL_INPUT						0x11
#define AVC_CMD_CHANNEL_USAGE						0x12
#define AVC_CMD_OUTPUT_PLUG_SIGNAL_FORMAT			0x18
#define AVC_CMD_INPUT_PLUG_SIGNAL_FORMAT			0x19
#define AVC_CMD_GENERAL_BUS_SETUP					0x1F
#define AVC_CMD_CONNECT_AV							0x20
#define AVC_CMD_DISCONNECT_AV						0x21
#define AVC_CMD_CONNECTIONS							0x22
#define AVC_CMD_CONNECT								0x24
#define AVC_CMD_DISCONNECT							0x25
#define AVC_CMD_SUBUNIT_INFO						0x31
#define AVC_CMD_UNIT_INFO							0x30
#define AVC_CMD_STREAM_FMT_SUPPORT					0x2F		// (Stream Format Info Spec 1.1)

// Unit commands (CCM)
#define AVC_CMD_SIGNAL_SOURCE						0x1A		// (CCM 1.0 Specification)
#define AVC_CMD_INPUT_SELECT						0x1B		// ..
#define AVC_CMD_OUTPUT_PRESET						0x1C		// ..
#define AVC_CMD_CCM_PROFILE							0x1D		// (CCM 1.1 Specification)

// Unit and subunit commands
#define AVC_CMD_VENDOR_DEPENDENT					0x00
#define AVC_CMD_RESERVE								0x01
#define AVC_CMD_PLUG_INFO							0x02
#define AVC_CMD_OPEN_INFOBLOCK						0x05
#define AVC_CMD_READ_INFOBLOCK						0x06
#define AVC_CMD_WRITE_INFOBLOCK						0x07
#define AVC_CMD_OPEN_DESCRIPTOR						0x08	
#define AVC_CMD_READ_DESCRIPTOR						0x09
#define AVC_CMD_WRITE_DESCRIPTOR					0x0A
#define AVC_CMD_SEARCH_DESCRIPTOR					0x0B
#define AVC_CMD_CREATE_DESCRIPTOR					0x0C
#define AVC_CMD_OBJECT_NUMBER_SELECT				0x0D
#define AVC_CMD_SECURITY							0x0F
#define AVC_CMD_VERSION								0xB0
#define AVC_CMD_POWER								0xB2
#define AVC_CMD_RATE								0xB3
#define AVC_CMD_FUNCTION_BLOCK						0xB8
#define AVC_CMD_CHANGE_CONFIGURATION				0xC0
#define AVC_CMD_STREAM_FMT_EXT_INFO					0xBF		// (Stream Format Info Spec 1.1)

// Panel Subunit opcodes
#define AVC_CMD_PANEL_PASS_THROUGH					0x7C
#define AVC_CMD_PANEL_GUI_UPDATE					0x7D
#define AVC_CMD_PANEL_PUSH_GUI_DATA					0x7E
#define AVC_CMD_PANEL_USER_ACTION					0x7F


#define AVC_RESERVE_TEXT_SIZE						12
#define AVC_VERSION_SUBFUNCTION_SIZE				32
#define AVC_VERSION_SU_DEP_FIELD_SIZE				31
#define AVC_PLUG_INFO_PLUGS_SIZE					4

// unit plug addresses

#define AVC_PLUG_ID_ANY_AVAILABLE_ISOCH				0x7F		// unit
#define AVC_PLUG_ID_ANY_AVAILABLE_ASYNC				0xBF		// unit
#define AVC_PLUG_ID_MULTIPLE_OUTPUT					0xFD		// unit/subunit
#define AVC_PLUG_ID_INVALID							0xFE		// unit/subunit
#define AVC_PLUG_ID_ANY_AVAILABLE_EXTERN			0xFF		// unit
#define AVC_PLUG_ID_ANY_AVAILABLE_SRC_DST			0xFF		// subunit

#define AVC_SU_UNIT									{AVC_SU_TYPE_UNIT, AVC_SU_ID_IGNORE}

#define AVC_PLUG_INVALID							{AVC_SU_UNIT, AVC_PLUG_ID_INVALID}

#define AVC_SU_ISOCH								AVC_SU_UNIT
#define AVC_PLUG_ISOCH_0							{AVC_SU_ISOCH, 0x00}
#define AVC_PLUG_ISOCH_1							{AVC_SU_ISOCH, 0x01}
#define AVC_PLUG_ISOCH_2							{AVC_SU_ISOCH, 0x02}
#define AVC_PLUG_ISOCH_3							{AVC_SU_ISOCH, 0x03}
#define AVC_PLUG_ISOCH_4							{AVC_SU_ISOCH, 0x04}
#define AVC_PLUG_ISOCH_5							{AVC_SU_ISOCH, 0x05}
#define AVC_PLUG_ISOCH_6							{AVC_SU_ISOCH, 0x06}
#define AVC_PLUG_ISOCH_7							{AVC_SU_ISOCH, 0x07}
#define AVC_PLUG_ISOCH_8							{AVC_SU_ISOCH, 0x08}
#define AVC_PLUG_ISOCH_9							{AVC_SU_ISOCH, 0x09}
#define AVC_PLUG_ISOCH_10							{AVC_SU_ISOCH, 0x0A}
#define AVC_PLUG_ISOCH_11							{AVC_SU_ISOCH, 0x0B}
#define AVC_PLUG_ISOCH_12							{AVC_SU_ISOCH, 0x0C}
#define AVC_PLUG_ISOCH_13							{AVC_SU_ISOCH, 0x0D}
#define AVC_PLUG_ISOCH_14							{AVC_SU_ISOCH, 0x0E}
#define AVC_PLUG_ISOCH_15							{AVC_SU_ISOCH, 0x0F}
#define AVC_PLUG_ISOCH_16							{AVC_SU_ISOCH, 0x10}
#define AVC_PLUG_ISOCH_17							{AVC_SU_ISOCH, 0x11}
#define AVC_PLUG_ISOCH_18							{AVC_SU_ISOCH, 0x12}
#define AVC_PLUG_ISOCH_19							{AVC_SU_ISOCH, 0x13}
#define AVC_PLUG_ISOCH_20							{AVC_SU_ISOCH, 0x14}
#define AVC_PLUG_ISOCH_21							{AVC_SU_ISOCH, 0x15}
#define AVC_PLUG_ISOCH_22							{AVC_SU_ISOCH, 0x16}
#define AVC_PLUG_ISOCH_23							{AVC_SU_ISOCH, 0x17}
#define AVC_PLUG_ISOCH_24							{AVC_SU_ISOCH, 0x18}
#define AVC_PLUG_ISOCH_25							{AVC_SU_ISOCH, 0x19}
#define AVC_PLUG_ISOCH_26							{AVC_SU_ISOCH, 0x1A}
#define AVC_PLUG_ISOCH_27							{AVC_SU_ISOCH, 0x1B}
#define AVC_PLUG_ISOCH_28							{AVC_SU_ISOCH, 0x1C}
#define AVC_PLUG_ISOCH_29							{AVC_SU_ISOCH, 0x1D}
#define AVC_PLUG_ISOCH_30							{AVC_SU_ISOCH, 0x1E}
#define AVC_PLUG_ISOCH_ANY_AVAILABLE				{AVC_SU_ISOCH, AVC_PLUG_ID_ANY_AVAILABLE_ISOCH}
													
#define AVC_SU_EXT									AVC_SU_UNIT
#define AVC_PLUG_EXT_0								{AVC_SU_EXT, 0x80 | 0x00}
#define AVC_PLUG_EXT_1								{AVC_SU_EXT, 0x80 | 0x01}
#define AVC_PLUG_EXT_2								{AVC_SU_EXT, 0x80 | 0x02}
#define AVC_PLUG_EXT_3								{AVC_SU_EXT, 0x80 | 0x03}
#define AVC_PLUG_EXT_4								{AVC_SU_EXT, 0x80 | 0x04}
#define AVC_PLUG_EXT_5								{AVC_SU_EXT, 0x80 | 0x05}
#define AVC_PLUG_EXT_6								{AVC_SU_EXT, 0x80 | 0x06}
#define AVC_PLUG_EXT_7								{AVC_SU_EXT, 0x80 | 0x07}
#define AVC_PLUG_EXT_8								{AVC_SU_EXT, 0x80 | 0x08}
#define AVC_PLUG_EXT_9								{AVC_SU_EXT, 0x80 | 0x09}
#define AVC_PLUG_EXT_10								{AVC_SU_EXT, 0x80 | 0x0A}
#define AVC_PLUG_EXT_11								{AVC_SU_EXT, 0x80 | 0x0B}
#define AVC_PLUG_EXT_12								{AVC_SU_EXT, 0x80 | 0x0C}
#define AVC_PLUG_EXT_13								{AVC_SU_EXT, 0x80 | 0x0D}
#define AVC_PLUG_EXT_14								{AVC_SU_EXT, 0x80 | 0x0E}
#define AVC_PLUG_EXT_15								{AVC_SU_EXT, 0x80 | 0x0F}
#define AVC_PLUG_EXT_16								{AVC_SU_EXT, 0x80 | 0x10}
#define AVC_PLUG_EXT_17								{AVC_SU_EXT, 0x80 | 0x11}
#define AVC_PLUG_EXT_18								{AVC_SU_EXT, 0x80 | 0x12}
#define AVC_PLUG_EXT_19								{AVC_SU_EXT, 0x80 | 0x13}
#define AVC_PLUG_EXT_20								{AVC_SU_EXT, 0x80 | 0x14}
#define AVC_PLUG_EXT_21								{AVC_SU_EXT, 0x80 | 0x15}
#define AVC_PLUG_EXT_22								{AVC_SU_EXT, 0x80 | 0x16}
#define AVC_PLUG_EXT_23								{AVC_SU_EXT, 0x80 | 0x17}
#define AVC_PLUG_EXT_24								{AVC_SU_EXT, 0x80 | 0x18}
#define AVC_PLUG_EXT_25								{AVC_SU_EXT, 0x80 | 0x19}
#define AVC_PLUG_EXT_26								{AVC_SU_EXT, 0x80 | 0x1A}
#define AVC_PLUG_EXT_27								{AVC_SU_EXT, 0x80 | 0x1B}
#define AVC_PLUG_EXT_28								{AVC_SU_EXT, 0x80 | 0x1C}
#define AVC_PLUG_EXT_29								{AVC_SU_EXT, 0x80 | 0x1D}
#define AVC_PLUG_EXT_30								{AVC_SU_EXT, 0x80 | 0x1E}
#define AVC_PLUG_EXT_ANY_AVAILABLE					{AVC_SU_EXT, AVC_PLUG_ID_ANY_AVAILABLE_EXTERN}
													
#define AVC_SU_AUDIO_0								{AVC_SU_TYPE_AUDIO, 0}
#define AVC_PLUG_AUDIO_0_0							{AVC_SU_AUDIO_0, 0}
#define AVC_PLUG_AUDIO_0_ANY_AVAILABLE				{AVC_SU_AUDIO_0, AVC_PLUG_ID_ANY_AVAILABLE_SRC_DST}
													
#define AVC_SRC_DST_TYPE_SUBUNIT					0x00
#define AVC_SRC_DST_TYPE_IGNORE						0x01
#define AVC_SRC_DST_TYPE_ISOCH_EXT_PLUG				0x02
#define AVC_SRC_DST_TYPE_RESERVED					0x03

// subunit commands

// function Block (command 0xB8)								// The audio modes available to the audio subunit.
#define AVC_AUDIO_AUDIOMODE_MONO					0x01
#define AVC_AUDIO_AUDIOMODE_STEREO					0x02
#define AVC_AUDIO_AUDIOMODE_DOLBYPROLOGIC			0x04
#define AVC_AUDIO_AUDIOMODE_HOMETHXCINEMA			0x08
#define AVC_AUDIO_AUDIOMODE_DOLBYDTS				0x10
#define AVC_AUDIO_AUDIOMODE_AC_3					0x20
#define AVC_AUDIO_AUDIOMODE_PROPRIETARY				0x40

// function block types											// audio s/u 1.0, table 10-1
#define AVC_FB_TYPE_SELECTOR						0x80
#define AVC_FB_TYPE_FEATURE							0x81
#define AVC_FB_TYPE_PROCESSING						0x82
#define AVC_FB_TYPE_CODEC							0x83
													
#define AVC_FB_TYPE_SUBUNIT_DEST_PLUG				0xF0
#define AVC_FB_TYPE_SUBUNIT_SOURCE_PLUG				0xF1
#define AVC_FB_TYPE_EXTEND							0xFF
													
// function block id 
#define AVC_FB_ID_SUBUNIT							0x00		// "used when addressing the entire subunit as a whole as in the change configuration command"
#define AVC_FB_ID_MIN_ID							0x01
#define AVC_FB_ID_MAX_ID							0xFE
#define AVC_FB_ID_EXTEND							0xFF

// function block control attributes							// I think these are general, if not they're specific to audio subuinit
#define AVC_FB_CTRL_ATTRIBUTE_RESOLUTION			0x01
#define AVC_FB_CTRL_ATTRIBUTE_MINIMUM				0x02
#define AVC_FB_CTRL_ATTRIBUTE_MAXIMUM				0x03
#define AVC_FB_CTRL_ATTRIBUTE_DEFAULT				0x04
#define AVC_FB_CTRL_ATTRIBUTE_CONFIGURATION			0x05
#define AVC_FB_CTRL_ATTRIBUTE_DURATION				0x08
#define AVC_FB_CTRL_ATTRIBUTE_CURRENT				0x10
#define AVC_FB_CTRL_ATTRIBUTE_MOVE					0x18
#define AVC_FB_CTRL_ATTRIBUTE_DELTA					0x19

// The following tokens are defined in table 9.3 of the audio subunit specification
// These are used to represent the commands	that a given audio channel can accept in a given feature function block
#define AVC_AUD_CH_ACCEPTS_MUTE						0x0001
#define AVC_AUD_CH_ACCEPTS_VOL						0x0002
#define AVC_AUD_CH_ACCEPTS_LR_BALANCE				0x0004
#define AVC_AUD_CH_ACCEPTS_FR_BALANCE				0x0008
#define AVC_AUD_CH_ACCEPTS_BASS_CNTRL				0x0010
#define AVC_AUD_CH_ACCEPTS_MID_CNTRL				0x0020
#define AVC_AUD_CH_ACCEPTS_TREBLE_CNTRL				0x0040
#define AVC_AUD_CH_ACCEPTS_GRAPHIC_EQ				0x0080
#define AVC_AUD_CH_ACCEPTS_AUTO_GAIN				0x0100
#define AVC_AUD_CH_ACCEPTS_DELAY_CNTRL				0x0200
#define AVC_AUD_CH_ACCEPTS_BASS_BOOST				0x0400
#define AVC_AUD_CH_ACCEPTS_LOUDNESS					0x0800

// The following tokens are defined in table A.4 of the audio subunit specification
// They	are used to indicate the the desired/reported command type in an av/c control packet
#define AVC_AUDIO_SELECTOR_CONTROL					0x01
#define AVC_AUDIO_MUTE_CONTROL						0x01
#define AVC_AUDIO_VOLUME_CONTROL					0x02
#define AVC_AUDIO_LR_BALANCE_CONTROL				0x03
#define AVC_AUDIO_FR_BALANCE_CONTROL				0x04
#define AVC_AUDIO_BASS_CONTROL						0x05
#define AVC_AUDIO_MID_CONTROL						0x06
#define AVC_AUDIO_TREBLE_CONTROL					0x07
#define AVC_AUDIO_GEQ_CONTROL						0x08
#define AVC_AUDIO_AGC_CONTROL						0x09
#define AVC_AUDIO_DELAY_CONTROL						0x0A
#define AVC_AUDIO_BASSBOOST_CONTROL					0x0B
#define AVC_AUDIO_LOUDNESS_CONTROL					0x0C
#define AVC_AUDIO_ENABLE_CONTROL					0x01
#define AVC_AUDIO_MODE_CONTROL						0x02
#define AVC_AUDIO_MIXER_CONTROL						0x03
#define AVC_AUDIO_SPACIOUSNESS_CONTROL				0x01
#define AVC_AUDIO_REVERBTYPE_CONTROL				0x01
#define AVC_AUDIO_REVERBLEVEL_CONTROL				0x02
#define AVC_AUDIO_REVERBTIME_CONTROL				0x03
#define AVC_AUDIO_REVERBEARLYTIME_CONTROL			0x04
#define AVC_AUDIO_REVERBDELAY_CONTROL				0x05
#define AVC_AUDIO_CHORUSRATE_CONTROL				0x01
#define AVC_AUDIO_CHORUSDEPTH_CONTROL				0x02
#define AVC_AUDIO_COMPRESSION_RATIO_CONTROL			0x01
#define AVC_AUDIO_MAXAMPL_CONTROL					0x02
#define AVC_AUDIO_THRESHOLD_CONTROL					0x03
#define AVC_AUDIO_ATTACKTIME_CONTROL				0x04
#define AVC_AUDIO_RELEASETIME_CONTROL				0x05
#define AVC_AUDIO_GUID_CONTROL						0x01
#define AVC_AUDIO_MPEG_DUAL_CHANNEL_CONTROL			0x03
#define AVC_AUDIO_MPEG_SECOND_STEREO_CONTROL		0x04
#define AVC_AUDIO_MPEG_MULTILINGUAL_CONTROL			0x05
#define AVC_AUDIO_MPEG_DYN_RANGE_CONTROL			0x06
#define AVC_AUDIO_MPEG_SCALE_CONTROL				0x07
#define AVC_AUDIO_HL_SCALING_CONTROL				0x08
#define AVC_AUDIO_AC3_DYNAMIC_RANGE_CONTROL			0x03
#define AVC_AUDIO_AC3_SCALE_CONTROL					0x04
#define AVC_AUDIO_AC3_HL_SCALING_CONTROL			0x05
#define AVC_AUDIO_AC3_DUAL_MONO_CONTROL				0x06
#define AVC_AUDIO_AC3_DOLBY_SURROUND_CONTROL		0x07
#define AVC_AUDIO_AC3_FRAME_ERROR_STATUS_CONTROL	0x08
#define AVC_AUDIO_AC3_SAMPLE_RATE_STATUS_CONTROL	0x09
#define AVC_AUDIO_AC3_DATA_RATE_STATUS_CONTROL		0x0A
#define AVC_AUDIO_AC3_LFE_ON_STATUS_CONTROL			0x0B
#define AVC_AUDIO_AC3_MOD_STATUS_CONTROL			0x0C
#define AVC_AUDIO_AC3_BSID_STATUS_CONTROL			0x0D
#define AVC_AUDIO_AC3_BSMOD_STATUS_CONTROL			0x0E
#define AVC_AUDIO_AC3_CMIXLEV_STATUS_CONTROL		0x0F
#define AVC_AUDIO_AC3_SMIXLEV_STATUS_CONTROL		0x10
#define AVC_AUDIO_AC3_DSUR_STATUS_CONTROL			0x11
#define AVC_AUDIO_AC3_CPYRT_STATUS_CONTROL			0x12
#define AVC_AUDIO_AC3_ORGNL_STATUS_CONTROL			0x13
#define AVC_AUDIO_AC3_DIALNORM_STATUS_CONTROL		0x14
#define AVC_AUDIO_AC3_DIALNORM2_STATUS_CONTROL		0x15
#define AVC_AUDIO_AC3_MIXLEV_STATUS_CONTROL			0x16
#define AVC_AUDIO_AC3_MIXLEV2_STATUS_CONTROL		0x17
#define AVC_AUDIO_AC3_ROOMTYPE_STATUS_CONTROL		0x18
#define AVC_AUDIO_AC3_ROOMTYPE2_STATUS_CONTROL		0x19
//#define AVC_AUDIO_CHANGE_CONFIGURATION			

// AVC audio subunit command parameters.
#define AVC_AUDIO_MUTE_ON							0x70
#define AVC_AUDIO_MUTE_OFF							0x60
#define AVC_AUDIO_CHANNEL_MASTER					0x00
#define AVC_AUDIO_CHANNEL_ALL						0xff

#define AVC_AUDIO_GMS_MAX_NUM_SPEAKERS				15

#define AVC_AUDIO_GMS_LEFT_FRONT					0x0001
#define AVC_AUDIO_GMS_RIGHT_FRONT					0x0002
#define AVC_AUDIO_GMS_CENTER_FRONT					0x0004
#define AVC_AUDIO_GMS_LF_ENHANCEMENT				0x0008
#define AVC_AUDIO_GMS_LEFT_SURROUND					0x0010
#define AVC_AUDIO_GMS_RIGHT_SURROUND				0x0020
#define AVC_AUDIO_GMS_LEFT_OF_CENTER				0x0040
#define AVC_AUDIO_GMS_RIGHT_OF_CENTER				0x0080
#define AVC_AUDIO_GMS_SURROUND						0x0100
#define AVC_AUDIO_GMS_SIDE_LEFT						0x0200
#define AVC_AUDIO_GMS_SIDE_RIGHT					0x0400
#define AVC_AUDIO_GMS_TOP							0x0800
#define AVC_AUDIO_GMS_BOTTOM						0x1000
#define AVC_AUDIO_GMS_LEFT_FRONT_EFFECT				0x2000
#define AVC_AUDIO_GMS_RIGHT_FRONT_EFFECT			0x4000
#define AVC_AUDIO_GMS_NONCONVENTIONAL				0x8000
													
#define AVC_AUDIO_GMS_SUPER_WOOFER					AVC_AUDIO_GMS_LF_ENHANCEMENT

// AVC music subunit commands
#define AVC_MUSIC_DEST_PLUG_CONFIGURE				0x40
#define AVC_MUSIC_SOURCE_PLUG_CONFIGURE				0x41
#define AVC_MUSIC_DEST_CONFIGURATIONS				0x42
#define AVC_MUSIC_SOURCE_CONFIGURATIONS				0x43
#define AVC_MUSIC_MUSIC_PLUG_INFO					0xC0
#define AVC_MUSIC_CURRENT_CAPABILITY				0xC1

// AVC_PLAYBACK_MODE											// ID's are the same for the CD and VCR subunits.
#define AVC_PLAYBACK_MODE_NEXTFRAME					0x30
#define AVC_PLAYBACK_MODE_SLOWESTFORWARD			0x31
#define AVC_PLAYBACK_MODE_SLOWFORWARD6				0x32
#define AVC_PLAYBACK_MODE_SLOWFORWARD5				0x33
#define AVC_PLAYBACK_MODE_SLOWFORWARD4				0x34
#define AVC_PLAYBACK_MODE_SLOWFORWARD3				0x35
#define AVC_PLAYBACK_MODE_SLOWFORWARD2				0x36
#define AVC_PLAYBACK_MODE_SLOWFORWARD1				0x37
#define AVC_PLAYBACK_MODE_NORMALPLAY				0x38
#define AVC_PLAYBACK_MODE_FASTFORWARD1				0x39
#define AVC_PLAYBACK_MODE_FASTFORWARD2				0x3A
#define AVC_PLAYBACK_MODE_FASTFORWARD3				0x3B
#define AVC_PLAYBACK_MODE_FASTFORWARD4				0x3C
#define AVC_PLAYBACK_MODE_FASTFORWARD5				0x3D
#define AVC_PLAYBACK_MODE_FASTFORWARD6				0x3E
#define AVC_PLAYBACK_MODE_FASTESTFORWARD			0x3F
#define AVC_PLAYBACK_MODE_PREVIOUSFRAME				0x40
#define AVC_PLAYBACK_MODE_SLOWESTREVERSE			0x41
#define AVC_PLAYBACK_MODE_SLOWREVERSE1				0x42
#define AVC_PLAYBACK_MODE_SLOWREVERSE2				0x43
#define AVC_PLAYBACK_MODE_SLOWREVERSE3				0x44
#define AVC_PLAYBACK_MODE_SLOWREVERSE4				0x45
#define AVC_PLAYBACK_MODE_SLOWREVERSE5				0x46
#define AVC_PLAYBACK_MODE_SLOWREVERSE6				0x47
#define AVC_PLAYBACK_MODE_NORMALREVERSE				0x48
#define AVC_PLAYBACK_MODE_FASTREVERSE1				0x49
#define AVC_PLAYBACK_MODE_FASTREVERSE2				0x4A
#define AVC_PLAYBACK_MODE_FASTREVERSE3				0x4B
#define AVC_PLAYBACK_MODE_FASTREVERSE4				0x4C
#define AVC_PLAYBACK_MODE_FASTREVERSE5				0x4D
#define AVC_PLAYBACK_MODE_FASTREVERSE6				0x4E
#define AVC_PLAYBACK_MODE_FASTESTREVERSE			0x4F
#define AVC_PLAYBACK_MODE_REVERSE					0x65
#define AVC_PLAYBACK_MODE_REVERSEPAUSE				0x6D
#define AVC_PLAYBACK_MODE_FORWARD					0x75
#define AVC_PLAYBACK_MODE_FORWARDPAUSE				0x7D
													
#define AVC_PLAYBACK_MODE_FIRST						0x30
#define AVC_PLAYBACK_MODE_LAST						0x7D
													
//	AVC_DISC										
#define AVC_DISC_COMMAND_ACCEPT_CHANGES				0xD2
#define AVC_DISC_COMMAND_REJECT_CHANGES				0xD2
#define AVC_DISC_COMMAND_ASSOCIATEWITHPLUG			0xD3
#define AVC_DISC_COMMAND_AUTOUPDATEONOFF			0xD4
#define AVC_DISC_COMMAND_COMBINE					0x41
#define AVC_DISC_COMMAND_CONFIGURE					0xD1
#define AVC_DISC_COMMAND_DISCSTATUS					0xD0
#define AVC_DISC_COMMAND_DIVIDE						0x42
#define AVC_DISC_COMMAND_ERASE						0x40
#define AVC_DISC_COMMAND_IMPORTMEDIUM				0xC1
#define AVC_DISC_COMMAND_EXPORTMEDIUM				0xC1
#define AVC_DISC_COMMAND_MONITOR					0xC6
#define AVC_DISC_COMMAND_MOVE						0x43
#define AVC_DISC_COMMAND_INCR_OBJ_POS_NUMBER		0x51
#define AVC_DISC_COMMAND_OBJ_NUMBER_SELECT			0x0D
#define AVC_DISC_COMMAND_PLAY						0xC3
#define AVC_DISC_COMMAND_RECORD						0xC2
#define AVC_DISC_COMMAND_RECORDOBJECT				0x56
#define AVC_DISC_COMMAND_REHEARSAL					0xC7
#define AVC_DISC_COMMAND_SEARCH						0x50
#define AVC_DISC_COMMAND_STOP						0xC5
#define AVC_DISC_COMMAND_UNDO						0x44
													
//	AVC_DISC_SEARCH_TYPES							
#define AVC_DISC_SEARCHTYPE_POSITION				0x00
#define AVC_DISC_SEARCHTYPE_ABSOLUTEUNIT			0x01
#define AVC_DISC_SEARCHTYPE_RELATIVEUNIT			0x10
													
// AVC_TUNER										
#define AVC_TUNER_DIRECT_SELECT_INFOTYPE			0xc8
#define AVC_TUNER_DIRECT_SELECT_DATA				0xcb
#define AVC_TUNER_CA_ENABLE							0xcc
#define AVC_TUNER_STATUS							0xcd
													
// CONNECTION STATE TYPES								
#define AVC_CONNECTION_ESTABLISH					0x70
#define AVC_CONNECTION_BREAK						0x60
#define AVC_CONNECTION_STATUS						0xFF

// POWER REQUEST TYPES								
#define AVC_POWER_ON								0x70
#define AVC_POWER_OFF								0x60
#define AVC_POWER_STATUS							0x7F
													
// AVC Security Categories							
#define AVC_SECURITY_CATEGORY_DTCP_AKE				0x0		// 0x1,..,0xD: Reserved
#define AVC_SECURITY_CATEGORY_VENDOR				0xE
#define AVC_SECURITY_CATEGORY_EXTENDED				0xF
													
// AVC Descriptor Specifier Types					
#define AVC_DS_SUBUNIT_IDENTIFIER					0x00
#define AVC_DS_LIST_ID								0x10
#define AVC_DS_LIST_TYPE							0x11
#define AVC_DS_ENTRY_POSITION						0x20
#define AVC_DS_ENTRY_OBJECT_ID						0x21
#define AVC_DS_ENTRY_TYPE							0x22
#define AVC_DS_OBJECT_ID							0x23
#define AVC_DS_INFOBLOCK_INSTANCE					0x30
#define AVC_DS_INFOBLOCK_POSITION					0x31
#define AVC_DS_SUBUNIT_TYPE_SPECIFIC_DESCRIPTOR		0x80	// 0x80 - 0xFF
#define AVC_DS_SUBUNIT_STATUS_DESCRIPTOR			0x80
//#define AVC_DS_UNUSED_DESCRIPTOR_SPECIFIER			0x99	// LM??? DH Defined, make this something which is not define in the specification.
#define AVC_DS_UNUSED_DESCRIPTOR_SPECIFIER			0xFF	// BK??? All values defined in spec, using an unlikely number for initializers
													
// AVC Descriptor Subfunction Types					
#define AVC_DESC_SUBFUNCTION_CLOSE					0x00
#define AVC_DESC_SUBFUNCTION_READOPEN				0x01
#define AVC_DESC_SUBFUNCTION_WRITEOPEN				0x03
													
#define AVC_DESC_SUBFUNCTION_CHANGE					0x10
#define AVC_DESC_SUBFUNCTION_REPLACE				0x20
#define AVC_DESC_SUBFUNCTION_INSERT					0x30
#define AVC_DESC_SUBFUNCTION_DELETE					0x40
#define AVC_DESC_SUBFUNCTION_PARTIAL_REPLACE		0x50
													
// AVC Descriptor Status Types.						
#define AVC_DESC_STATUS_READY_FOR_OPEN				0x00
#define AVC_DESC_STATUS_READ_ONLY_OPEN				0x01
#define AVC_DESC_STATUS_NON_EXISTANT				0x04
#define AVC_DESC_STATUS_LIST_ONLY					0x05
#define AVC_DESC_STATUS_AT_CAPACITY					0x11
#define AVC_DESC_STATUS_WRITE_ENABLED_OPEN			0x33
													
#define AVC_DESC_STATUS_COMPLETE_READ				0x10
#define AVC_DESC_STATUS_MORE_TO_READ				0x11
#define AVC_DESC_STATUS_DATA_LENGTH_TOO_LARGE		0x12
													
// AVC Descriptor list and entry attributes
#define AVC_DESC_ATTRIB_UP_TO_DATE					BIT3
#define AVC_DESC_ATTRIB_OBJECT_ID					BIT4
#define AVC_DESC_ATTRIB_CHILD_ID					BIT5
#define AVC_DESC_ATTRIB_NONE						0

// AVC Write Descriptor Response codes						// these are returned in the subfunction field of a Write Command.
#define AVC_DESC_WRITE_SUCCESSFUL					0x00
#define AVC_DESC_WRITE_PARTIAL						0x01
#define AVC_DESC_WRITE_INVALID						0x02
													
// AVC Write Descriptor Group Tags					
#define AVC_DESC_GROUPTAG_IMMEDIATE					0x00
#define AVC_DESC_GROUPTAG_FIRST						0x01
#define AVC_DESC_GROUPTAG_CONTINUE					0x02
#define AVC_DESC_GROUPTAG_LAST						0x03

													
// RATE command subfunctions						
#define RATE_SYNC_SELECT							0x00
#define RATE_BASE_CONFIGURE							0x01
#define RATE_FLOW_CONTROL							0x02
#define RATE_CAPABILITY_INQUIRY						0x80
	
#endif //_AVCDEFS_H
										

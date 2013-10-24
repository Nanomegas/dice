//==============================================================================
//####TCAT-SOURCE-HEADER-BEGIN####
//
// This confidential and proprietary source may be used only as authorized
// by a licensing agreement from TC Applied Technologies.
//
// Copyright (c) 2004-2013 TC Applied Technologies
//                         a division of TC Group Americas Ltd.
//                         All rights reserved.
//
// Unauthorized use, duplication or distribution of this source is strictly
// prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies.
//
//####TCAT-SOURCE-HEADER-END####
//==============================================================================
//
//	third party product specific defines
//
//==============================================================================

#ifndef _TCAT_DICE_MYPRODUCT_H_
#define _TCAT_DICE_MYPRODUCT_H_

#include "tcat_dice_version.h"


//==============================================================================
//
//	product line defines
//
//==============================================================================
//
//	Change this to customize the software for your DICE(tm) product line:
//
//	You have to change all defines to make your driver coexist with other 
//	customizations.
//
//	kTCAT_DICE_VENDOR 
//	is your natural company name as shown in any software provider information.
#define kTCAT_DICE_VENDOR					TC Applied Technologies

//	kTCAT_DICE_VENDOR_SHORT 
//	your company short form used for device matching. 
//	Textual match for firmware config rom. This cannot contain any 
//	non-alphanumeric character! Use underscores instead!
#define kTCAT_DICE_VENDOR_SHORT				TCAT

//	kTCAT_DICE_VENDOR_ID
//	is your OUI
#define kTCAT_DICE_VENDOR_ID				0x166

//	kTCAT_DICE_USR 
//	will be displayed to the end user in any audio or midi driver selection 
//	dialogs.
//	This does not necessarily reflect your device name - since more of your 
//	devices can be supported by that software. If you plan to only support one
//	device with this software give it a name related to your device.
//	Make sure this name does not exceed 32 character including eventual 
//	enumeration additions made by the OS. If you also enable additions like
//	Audio, MIDI, ASIO etc. in tcat_dice_macros.h all needs to fit.
//	This is an OS limitation.
#define kTCAT_DICE_USR						TCAT Dice EVM Platform

//	kTCAT_DICE_DRV 
//  will be used to generate filenames and internally used symbolic 
//	links.
//	This does not necessarily reflect your device name - since more of your 
//	devices can be supported by that software. If you plan to only support one
//	device with this software give it a name related to your device.
#define kTCAT_DICE_DRV						TcatDice

//	kTCAT_DICE_GUID 
//	is a GUID that you pick to be distinctive from other DICE products that 
//	might be installed in the same system. 
//	(Unlike the static firmware magic number that is just an ID to find some 
//	information in the binary file.)
#define kTCAT_DICE_GUID						{0x7ca7d1ce, 0x6cd2, 0x4e81, {0xb9, 0xf2, 0x71, 0x2d, 0x52, 0x15, 0x58, 0x50}}
#define kTCAT_DICE_GUID_PLIST				7ca7d1ce-6cd2-4e81-b9f2-712d52155850


//==============================================================================
//
//	device defines
//
//==============================================================================
//
//	The following defines the devices supported by this product line software:
//	You can remove defines you don't need.
//
//	kTCAT_DICE_DEVICE_*_DESC
//	Defines the device description for each of the products supported by this 
//	software. This will only appear at the device install process and then 
//	replaced by kTCAT_DICE_USR.
//
//	kTCAT_DICE_DEVICE_*_ID
//	Defines the device IDs for each of the products supported by this software.
//
//	kTCAT_DICE_DEVICE_*_MODEL_ID
//	Textual match for firmware config rom. This cannot contain any 
//	non-alphanumeric character! Use underscores instead!
//
#define kTCAT_DICE_DEVICE_1_DESC					kTCAT_DICE_VENDOR Dice Classic Evaluation Board
#define kTCAT_DICE_DEVICE_1_ID						0x1
#define kTCAT_DICE_DEVICE_1_MODEL_ID				DICE_II_Evaluation_Board
//#define kTCAT_DICE_DEVICE_1_MIDI_NAME				EVM
//#define kTCAT_DICE_DEVICE_1_MIDI_MANUFACTURER		TCAT
//#define kTCAT_DICE_DEVICE_1_MIDI_DEVICE_NAME		kTCAT_DICE_DEVICE_1_MIDI_MANUFACTURER kTCAT_DICE_DEVICE_1_MIDI_NAME
//#define kTCAT_DICE_DEVICE_1_MIDI_ICON_FILE		EVM.tiff
//#define kTCAT_DICE_DEVICE_1_MIDI_PORT_1_NAME		MIDI
//#define kTCAT_DICE_DEVICE_1_MIDI_PORT_2_NAME		Ctrl LoopBack
//#define kTCAT_DICE_DEVICE_1_MIDI_PORT_2_EMBEDDED	1

#define kTCAT_DICE_DEVICE_2_DESC					kTCAT_DICE_VENDOR Dice EVM002 Evaluation Board
#define kTCAT_DICE_DEVICE_2_ID						0x2
#define kTCAT_DICE_DEVICE_2_MODEL_ID				DICE_EVM002_Evaluation_Board
#define kTCAT_DICE_DEVICE_2_MIDI_NAME				EVM002
#define kTCAT_DICE_DEVICE_2_MIDI_DEVICE_NAME		kTCAT_DICE_VENDOR_SHORT kTCAT_DICE_DEVICE_2_MIDI_NAME
#define kTCAT_DICE_DEVICE_2_MIDI_ICON_FILE			EVM002.tiff
#define kTCAT_DICE_DEVICE_2_MIDI_PORT_1_NAME		MIDI
#define kTCAT_DICE_DEVICE_2_MIDI_PORT_2_NAME		LoopBack
#define kTCAT_DICE_DEVICE_2_MIDI_PORT_2_EMBEDDED	1

#define kTCAT_DICE_DEVICE_3_DESC					kTCAT_DICE_VENDOR Dice Classic Evaluation Board EAP
#define kTCAT_DICE_DEVICE_3_ID						0x3
#define kTCAT_DICE_DEVICE_3_MODEL_ID				DICE_II_Evaluation_Board_EAP

#define kTCAT_DICE_DEVICE_4_DESC					kTCAT_DICE_VENDOR Dice EVM002 Evaluation Board EAP
#define kTCAT_DICE_DEVICE_4_ID						0x4
#define kTCAT_DICE_DEVICE_4_MODEL_ID				DICE_EVM002_Evaluation_Board_EAP

#define kTCAT_DICE_DEVICE_5_DESC					kTCAT_DICE_VENDOR Dice EVM003 Evaluation Board
#define kTCAT_DICE_DEVICE_5_ID						0x5
#define kTCAT_DICE_DEVICE_5_MODEL_ID				DICE_EVM003_Evaluation_Board

#define kTCAT_DICE_DEVICE_6_DESC					kTCAT_DICE_VENDOR Dice EVM003 Evaluation Board EAP
#define kTCAT_DICE_DEVICE_6_ID						0x6
#define kTCAT_DICE_DEVICE_6_MODEL_ID				DICE_EVM003_Evaluation_Board_EAP

#define kTCAT_DICE_DEVICE_7_DESC					kTCAT_DICE_VENDOR Dice EVM004-FW843 Evaluation Board
#define kTCAT_DICE_DEVICE_7_ID						0x7
#define kTCAT_DICE_DEVICE_7_MODEL_ID				DICE_EVM004_FW843_Evaluation_Board


//==============================================================================

#endif  // _TCAT_DICE_MYPRODUCT_H_

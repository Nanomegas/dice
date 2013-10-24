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
//	Purpose:	DICE  Driver Interface. This file defines the data structures
//				used between the DiceDriver firmware and the PC or Mac driver.
//	Revisions:
//		created 01/03/2005 ml
//
//==============================================================================

#pragma once


namespace tcat
{
namespace dice
{

//\cond

/// we will use the space 1M into the private space
#define FL_DRIVER_BASE_START_HI		0xffff
#define FL_DRIVER_BASE_START		0xe0100000
#define FL_DRIVER_BASE_END_HI		0xffff
#define FL_DRIVER_BASE_END		    (FL_DRIVER_BASE_START + sizeof(FL_1394MEMORY))

#define MAX_IMAGE_NAME	16

/// opcodes
typedef enum
{
	FL_OP_GET_IMAGE_DESC,			///< parameters: imageId  return: imageDesc
	FL_OP_DELETE_IMAGE,				///< parameters: name  return: none
	FL_OP_CREATE_IMAGE,				///< parameters: execAddr, entryAddr, name  return: none
	FL_OP_UPLOAD,					///< parameters: index, length, data, return none
	FL_OP_UPLOAD_STAT,				///< parameters: length return checksum
	FL_OP_RESET_IMAGE,				///< parameters: none
	FL_OP_TEST_ACTION,				///< ...
	FL_OP_GET_FLASH_INFO,			///< ...
	FL_OP_READ_MEMORY,				///< ...
	FL_OP_NOOP,						///< ...
	FL_OP_GET_RUNNING_IMAGE_VINFO,	///< parameters: none  return: vendorInfo
	FL_OP_CREATE_IMAGE2,			///< parameters: none  return: vendorInfo
	FL_OP_GET_APP_INFO				///< parameters: none  return: new info structure
}FL_OP_ENUM;


// opcode definitions
#define FL_OP_EXECUTE	0x80000000
#define FL_OP_MASK		0x00000FFF

typedef struct
{
	uint32	imageId;
}FL_GET_IMAGE_DESC_PARAM;

typedef struct
{
	char	name[16];
}FL_DELETE_IMAGE_PARAM;

typedef struct
{
	uint32	length;
	uint32	execAddr;
	uint32	entryAddr;
	char	name[16];
}FL_CREATE_IMAGE_PARAM;

typedef struct
{
	uint32	index;
	uint32	length;
	uint32	buffer[256];
}FL_UPLOAD_PARAM;

typedef struct
{
	uint32	length;
}FL_UPLOAD_STAT_PARAM;

//Valid Test Command ID's, Parameter in FL_TEST_ACTION_PARAM, return in FL_TEST_RTN
#define FL_TEST_CMD_POKE			1		//Write value to location, lvalue0=32 bit address, lvalue1=32 bit data
#define FL_TEST_CMD_PEEK			2		//Read value from location, lvalue0=32 bit address, return data[0] = 32 bit value
#define FL_TEST_CMD_GET_AVS_CNT		3		//Return AVS counters in data[NUM_AVS_EVENTS].
#define FL_TEST_CMD_CLR_AVS_CNT		4		//Clear AVS counters
#define FL_TEST_CMD_SET_MODE		5		//Set the EVM board mode equal to lvalue0, invoked by myModeSetMode
#define FL_TEST_CMD_SET_MIDIBP		6		//Set the EVM (or customer product) to bypass MIDI if lvalue!=0, invoked by myModeBypassMidi
#define FL_TEST_CMD_GET_AVS_PHASE	7		//Return the relative phase between PLL and Avs receiver, lvalue=0/1 (for Avs1/Avs2), return
											//	data[0]=signed phase offset in steps of 0.1us.
											//	data[1]=signed phase offset in %.
#define FL_TEST_CMD_ENABLE_BNC_SYNC	8		//WCKO is programmed to pulse on AVS errors
#define FL_TEST_CMD_PULSE_BNC_SYNC	9		//Send out pulse on WCKO
#define FL_TEST_CMD_ENTER_TESTMODE  10		//Enter a firmware test mode

#define FL_TEST_CMD_EMUL_SLOW_CMD	127		//This command does nothing but busy waits for the number of ms in lvalue0 before clearing the exec. flag.




typedef struct
{
	uint32	cmdID;
	uint32	lvalue0; 
	uint32	lvalue1;
}FL_TEST_ACTION_PARAM;

typedef struct
{
	uint32 uiStartAddress;
	uint32 uiEndAddress;
	uint32 uiNumBlocks;
	uint32 uiBlockSize;
} FL_INFO_PARAM;

typedef struct
{
	uint32 uiStartAddress;
	uint32 uiLen;
	char ReadBuffer[4000];
} FL_READ_MEMORY;

typedef union
{
	FL_GET_IMAGE_DESC_PARAM	getImage;
	FL_DELETE_IMAGE_PARAM	deleteImage;
	FL_CREATE_IMAGE_PARAM	createImage;
	FL_UPLOAD_PARAM			upload;
	FL_UPLOAD_STAT_PARAM	uploadStat;
	FL_TEST_ACTION_PARAM	testParam;
	FL_READ_MEMORY ReadMemory;
}FL_PARAM;

// Configuration flags for firmware compile defines

// Target
#define	CFG_DICEII_VERIFICATION		0x0001 
#define CFG_DICEII_EVAL				0x0002
// Driver
#define CFG_GENERIC_MLAN			0x0004
#define CFG_MLAN					0x0008
#define CFG_DICE_DRIVER				0x0010
#define CFG_AVC_DRIVER				0x0020

typedef struct
{
	char	name[MAX_IMAGE_NAME];
	uint32	flashBase;
	uint32	memBase;
	uint32	size;
	uint32	entryPoint;
	uint32	length;
	uint32	chkSum;

	uint32 uiBoardSerialNumber;
	uint32 uiVersionHigh;
	uint32 uiVersionLow;
	uint32 uiConfigurationFlags;
	
	char BuildTime[64];
	char BuildDate[64];

}FL_GET_IMAGE_DESC_RETURN;

typedef struct
{
	uint32	uiBaseSDKVersion;			//The full version/revision of the SDK this build was based on
	uint32	uiApplicationVersion;		//The full version/revision of the Application
	uint32	uiVendorID;					//The Vendor ID
	uint32	uiProductID;				//The product ID
	char	BuildTime[64];				//Build time
	char	BuildDate[64];				//Build date
	uint32	uiBoardSerialNumber;		//The serial number of the board as optained from persist. storage
} FL_GET_APP_INFO_RETURN;


typedef struct
{
	uint32 uiVProductID;
	char uiVendorID[8];
	uint32 uiVMajor;
	uint32 uiVMinor;
	uint32 user1;
	uint32 user2;
} FL_GET_VENDOR_IMAGE_DESC_RETURN;

typedef struct
{
	uint32	checksum;
} FL_GET_UPLOAD_STAT_RETURN;

typedef struct {
	uint32 data[100];
} FL_TEST_RTN;

typedef union
{
	FL_GET_IMAGE_DESC_RETURN			getImage;
	FL_GET_VENDOR_IMAGE_DESC_RETURN		getImageVInfo;
	FL_GET_UPLOAD_STAT_RETURN			uploadStat;
	FL_INFO_PARAM						FlashInfo;
	FL_READ_MEMORY						ReadMemory;
	FL_GET_APP_INFO_RETURN				GetAppInfo;
	FL_TEST_RTN							testRtn;
}FL_RETURN;


/** communication protocol (computer point of view)
Check if status is not busy
write op code with exec bit set
check opcode until exec bit clear
read error for return code
if applicable read returned data

If progress is supported it will be indicated in the
progress variable. It is defined as follows:
bit0..11: current progress
bit12..23: max progress
bit24..27: timeout between progress in seconds
bit31: If 1 progress is supported for this command
*/
#define FL_PRGS_CURR_MASK 0x00000fff
#define FL_PRGS_MAX_MASK  0x00fff000
#define FL_PRGS_TOUT_MASK 0x0f000000
#define FL_PRGS_FLAG      0x80000000

#define FL_PRGS_CURR_SHFT 0
#define FL_PRGS_MAX_SHFT  12
#define FL_PRGS_TOUT_SHFT 24


#define FL_VERSION	100		//version 1.0
typedef struct
{
	uint32	version;	// RO
	uint32	opcode;		// RW opcode and execute flag
	uint32	error;		// RO error returned
	uint32	progress;	// 
	uint32	capabilities; //capability flags
	uint32	reserved[6];
	union
	{
		FL_PARAM	parameters;
		FL_RETURN	rtn;
	} data; // RW with single and block
	uint32	endOfData;
	uint32  testDelay; //number of ms to wait on access to testbuf before returning split transaction.
	uint32	testbuf[32]; //128 bytes of test data.
}FL_1394MEMORY;


#define FL_CAP_AUTOERASE 0x00000001
#define FL_CAP_PROGRESS  0x00000002


//\endcond

} // namepace dice
} // namespace tcat

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
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd. 
//
//===========================================================================

/****************************************************************************
 * DESCRIPTION: dalRemoteInterface.h

	Purpose:	Remote DAL (RDAL) for direct PC control.
				
	Revisions:
		created 05/02/2005 zo
		
****************************************************************************/

/***	
Actual DAL APIs we are remoting
===============================

HRESULT dalCreateInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_RATE_MODE_ENUM rateMode, 
	uint32 inputDevices, uint32 outputDevices);

HRESULT dalStartInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId);

HRESULT dalSetClockSource (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_CLOCK_SOURCE_ENUM clockSource, 
	DAL_NOMINAL_RATE_ENUM nominalRate);

HRESULT dalSetRoute(DAL_INTERFACE_ID_ENUM dalInterfaceId, DEST_DESCRIPTION dst, SRC_DESCRIPTION src);

HRESULT dalGetCurrentStatus  (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_STATUS * pStatus);

BOOL dalIsLocked (DAL_INTERFACE_ID_ENUM dalInterfaceId);

HRESULT dalGetCurrentConfiguration  (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_CONFIG * pConfig);

HRESULT dalInstallCallback  (const DAL_EVENTS * pEventMask, uint32 holdoff_ms, DAL_CALLBACK_FUNC func, uint32 data);

HRESULT dalUninstallCallback(DAL_CALLBACK_FUNC func);

HRESULT dalGlobalMute   (DAL_INTERFACE_ID_ENUM dalInterfaceId, BOOL mute);
***/

#ifndef _DAL_REMOTE_INTERFACE_H_
#define	_DAL_REMOTE_INTERFACE_H_

/// we will use the space 3M into the private space

#define RDAL_BASE_START_HI		0xffff
#define RDAL_BASE_START			0xe0300000
#define RDAL_BASE_END_HI		0xffff
#define RDAL_BASE_END			(RDAL_BASE_START + sizeof(RDAL_1394MEMORY))

//
// RDAL Opcodes
//
typedef enum
{
	RDAL_OP_CREATE_INTERFACE = 1000,	
	RDAL_OP_START_INTERFACE,
	RDAL_OP_SET_CLOCK_SOURCE,
	RDAL_OP_SET_ROUTE,
	RDAL_OP_GET_CURRENT_STATUS,
	RDAL_OP_IS_LOCKED,
	RDAL_OP_GET_CURRENT_CONFIG,
	RDAL_OP_INSTALL_CALLBACK,
	RDAL_OP_UNINSTALL_CALLBACK,
	RDAL_OP_GLOBAL_MUTE,
	RDAL_OP_TOGGLE_LED,
	RDAL_OP_GET_EVM_UI,
	RDAL_OP_SET_OUT_CHANNEL_VOL,		// AXM20
	RDAL_OP_SET_IN_CHANNEL_VOL,			// AXM20
	RDAL_OP_SET_OUT_CHANNEL_MUTE,		// AXM20
	RDAL_OP_SET_IN_CHANNEL_MUTE,		// AXM20
	RDAL_OP_GET_IN_CHANNEL_VOL,			// AXM20
	RDAL_OP_GET_OUT_CHANNEL_VOL,		// AXM20
	RDAL_OP_NOOP,

} RDAL_OP_ENUM;

/// opcode definitions
#define RDAL_OP_EXECUTE	0x80000000
#define RDAL_OP_MASK	0x00000FFF

//
// RDAL input parameters
//
typedef struct
{
	uint32 uiInterfaceId;
	uint32 uiRateMode;
	uint32 uiInputDevices;
	uint32 uiOutputDevices;

} RDAL_CREATE_INTERFACE;

typedef struct
{
	uint32 uiInterfaceId;

} RDAL_START_INTERFACE;

typedef struct
{
	uint32 uiInterfaceId;
	uint32 uiClockSource;
	uint32 uiNominalRate;
	
} RDAL_SET_CLOCK_SOURCE;

typedef struct
{
	uint32 uiInterfaceId;
	// uint16 ui16Destination;
	// uint16 ui16Source;
	
	DEST_DESCRIPTION DestRoute;
	SRC_DESCRIPTION SrcRoute;
	
} RDAL_SET_ROUTE;

typedef struct
{
	uint32 uiInterfaceId;
	DAL_STATUS DalStatus;
	
} RDAL_GET_CURRENT_STATUS;

typedef struct
{
	uint32 uiInterfaceId;
	
} RDAL_IS_LOCKED;

typedef struct
{
	uint32 uiInterfaceId;
	DAL_CONFIG DalConfig;
	
} RDAL_GET_CURRENT_CONFIGURATION;

typedef struct
{
	// uint32 uiEventMask;
	uint32 uiHoldoffMS;
	
	DAL_EVENTS DalEvents;
	
} RDAL_INSTALL_CALLBACK;

typedef struct
{
	uint32 uiInterfaceId;
	uint32 uiMute;
	
} RDAL_GLOBAL_MUTE;

typedef struct
{
	uint32 uiLED_ID;
	uint32 uiOn;

} RDAL_TOGGLE_LED;

typedef struct
{
	uint32 uiDeltaOrAbs;

} RDAL_GET_EVM_UI;

// AXM20
typedef struct
{
	uint32 uiChannelId;
	uint32 uiLevel;
	
} RDAL_SET_CHANNEL_VOL;

typedef struct
{
	uint32 uiChannelId;
	uint32 uiMute;
	
} RDAL_SET_CHANNEL_MUTE;

typedef struct
{
	uint32 uiInOrOut;
	uint32 uiChannelId;
} RDAL_GET_CHANNEL_VOL;
// end AXM20

typedef union
{
	RDAL_CREATE_INTERFACE CreateIterface;
	RDAL_START_INTERFACE StartInterface;
	RDAL_SET_CLOCK_SOURCE SetClockSource;
	RDAL_SET_ROUTE SetRoute;
	RDAL_GET_CURRENT_STATUS GetCurrentStatus;
	RDAL_IS_LOCKED IsLocked;
	RDAL_GET_CURRENT_CONFIGURATION GetCurrentConfig;
	RDAL_INSTALL_CALLBACK InstallCallback;
	RDAL_GLOBAL_MUTE GlobalMute;
	RDAL_TOGGLE_LED ToggleLED;
	RDAL_GET_EVM_UI GetEVMui;
	RDAL_SET_CHANNEL_VOL SetChannelVol;			// AXM20
	RDAL_SET_CHANNEL_MUTE SetChannelMute;		// AXM20
	RDAL_GET_CHANNEL_VOL GetChannelVol;			// AXM20
} RDAL_PARAMS; 

//
// RDAL returned parameters
//

typedef struct
{
	uint32 uiState;	
	uint32 uiLockedRateMode;
	uint32 uiLockedNominalRate;
	uint32 uiLockedRateHz;
	uint32 uiBadRateHz;
	
} RDAL_GET_CURRENT_STATUS_RETURN;

typedef struct
{
	uint32 uiIsLocked;

} RDAL_IS_LOCKED_RETURN;

typedef struct
{
	uint32 uiRateMode;	
	uint32 uiClockSource;
	uint32 uiSampleRate;	
	uint32 uiInputDevices;
	uint32 uiOutputDevices;

} RDAL_GET_CURRENT_CONFIG_RETURN;

typedef struct
{
	uint32 uiKnobVal;
	uint32 uiKnobSwitch;
	uint32 uiDIPsw2;

} RDAL_GET_EVM_UI_RETURN;

// AXM20
typedef struct
{
	uint32 uiInOrOut;
	uint32 uiChId;
	uint32 uiMuted;
	uint32 uiVolume;
	
} RDAL_GET_CHANNEL_VOL_RETURN;
// end AXM20

typedef union
{
	RDAL_GET_CURRENT_STATUS_RETURN CurrentStatus;
	RDAL_IS_LOCKED_RETURN IsLocked;
	RDAL_GET_EVM_UI_RETURN EVMui;
	RDAL_GET_CHANNEL_VOL_RETURN ChannelVol;			// AXM20

} RDAL_RETURN_PARAMS;


/*** 
communication protocol (computer point of view)

Check if status is not busy
write op code with exec bit set
check opcode until exec bit clear
read error for return code
if applicable read returned data

***/

#define RDAL_VERSION	101		//version 1.01

typedef struct
{
	uint32	version;	// RO
	uint32	opcode;		// RW opcode and execute flag
	uint32	error;		// RO error returned
	uint32	reserved[8];
	
	union
	{
		RDAL_PARAMS parameters;
		RDAL_RETURN_PARAMS rtn;
	
	} data; // RW with single and block

	uint32	endOfData;

} RDAL_1394MEMORY;

#endif


//==============================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//==============================================================================
//
//	Windows platform dependencies:  not to expose to applications!
//
//==============================================================================

#pragma once

#include "tcat.h"


namespace tcat
{
namespace dice
{

///////////////////////////////////////////////////////////////////////////////
//
//	private ASIO interface
//
///////////////////////////////////////////////////////////////////////////////
///
enum
{
	kAsioSetSlaveClient	= 0x33112004	//define for special clients which work in restricted mode
};


///////////////////////////////////////////////////////////////////////////////
//
//	Minimum/Maximum ASIO buffer size defines for different sample rates 
//
///////////////////////////////////////////////////////////////////////////////
///
#define	MIN_ASIO_BUF_SIZE_AT_LOW_RATES	32
#define	MIN_ASIO_BUF_SIZE_AT_MID_RATES	64
#define	MIN_ASIO_BUF_SIZE_AT_HIGH_RATES	128

#define	MAX_ASIO_BUF_SIZE_AT_LOW_RATES	2048
#define	MAX_ASIO_BUF_SIZE_AT_MID_RATES	4096
#define	MAX_ASIO_BUF_SIZE_AT_HIGH_RATES	8192


///////////////////////////////////////////////////////////////////////////////
//
//	IOCTL 
//
///////////////////////////////////////////////////////////////////////////////
///	
///	platform dependent device I/O control
///
enum
{
	kIoctlAPIVersion = 0x1006,
};


#define WIN_CTL_CODE( code )	CTL_CODE( FILE_DEVICE_UNKNOWN, code, METHOD_BUFFERED, FILE_ANY_ACCESS )


enum IOCTL
{
#if 0
												//	Input							Output
												// --------------------------------------------------------------------

	kIoctlVersionGet				= 0x909,	//	none							current: ioctl::VERSION_GET
												//									legacy:  uint32  (kTCAT_DICE_VERSION32)

	kIoctlVendorVersionStrGet		= 0x910,	//	none							ioctl::VENDOR_VERSION_STR

	kIoctlBusNotificationsSet		= 0x923,	//	ioctl::BUS_NOTIFICATION			none
	kIoctlBusConfigGet				= 0x921,	//	none							ioctl::BUS_CONFIG
	kIoctlBusClockSet				= 0x939,	//	uint32							none
	kIoctlBusSyncSet				= 0x938,	//	uint32							none
	kIoctlBusIoSizeSet				= 0x937,	//	uint32							none
	kIoctlBusMinMaxIoSizeGet		= 0x957,	//	none							ioctl::BUS_GET_IO_SIZE_MIN_MAX
	kIoctlBusMasterSet				= 0x940,	//	uint64							none
	kIoctlBusSpeakerGet				= 0x946,	//	none							uint32
	kIoctlBusOutChannelMapSet		= 0x950,	//	ioctl::BUS_SET_OUT_CHANNEL_MAP	none
	kIoctlBusInChannelMapSet		= 0x951,	//	ioctl::BUS_SET_IN_CHANNEL_MAP	none
	kIoctlBusChannelMapGet			= 0x949,	//	none							ioctl::BUS_GET_CHANNEL_MAP
	kIoctlBusChannelsGet			= 0x948,	//	none							ioctl::BUS_CHANNELS
	kIoctlBusOpModeSet				= 0x943,	//	uint32							none
	kIoctlBusOpModeGet				= 0x944,	//	none							uint32
	kIoctlBusErrorsGet				= 0x942,	//	none							ioctl::BUS_ERROR
	kIoctlBusErrorsClear			= 0x958,	//	none							none
	kIoctlBusDpcStatsGet			= 0x960,	//	none							ioctl::BUS_DPC_STATS
	kIoctlBusDpcStatsClear			= 0x961,	//	none							none
	kIoctlBusAc3ChannelsSet			= 0x954,	//	ioctl::BUS_AC3_MAP				none
	kIoctlBusClientsGet				= 0x841,	//	none							ioctl::BUS_CLIENT_COUNT
	kIoctlBusAddToReportedLatency	= 0x962,	//	BUS_ADD_AUDIO_LATENCY			none

	kIoctlDeviceNotificationsGet	= 0x945,	//	none							DEVICE_NOTIFICATIONS
	kIoctlDeviceClockGet			= 0x924,	//	ioctl::DEVICE_CLOCK				ioctl::DEVICE_CLOCK
	kIoctlDeviceNicknameSet			= 0x932,	//	ioctl::DEVICE_NICKNAME			none
	kIoctlDeviceOhciRead			= 0x914,	//	OHCI_HEADER						ioctl::OHCI<1>
	kIoctlDeviceOhciWrite			= 0x915,	//	ioctl::OHCI<1>					none
	kIoctlDeviceStateSet			= 0x941,	//	DEVICE_INFO						none

#else
	kIoctlVersionGet =				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x909, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_INTERFACE_VERSION

	kIoctlVendorVersionStrGet =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x910, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_VENDOR_VERSION_STR

	kIoctlBusNotificationsSet =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x923, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_REGISTER_CLIENT
	kIoctlBusConfigGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x921, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_CONFIG_INFO
	kIoctlBusClockSet =				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x939, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_SET_SAMPLE_RATE
	kIoctlBusSyncSet =				CTL_CODE(FILE_DEVICE_UNKNOWN, 0x938, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_SET_CLK_SRC
	kIoctlBusIoSizeSet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x937, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_SET_BUFFER_SIZE
	kIoctlBusMinMaxIoSizeGet =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x957, METHOD_BUFFERED, FILE_ANY_ACCESS), // IOCTL_DICE_GET_MIN_ASIO_SAMPLES
	kIoctlBusMasterSet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x940, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_SET_MASTER_DEVICE
	kIoctlBusSpeakerGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x946, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_SPEAKER_CONFIG
	kIoctlBusOutChannelMapSet =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x950, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_OUT_SET_CHANNEL_MAP
	kIoctlBusInChannelMapSet =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x951, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_IN_SET_CHANNEL_MAP
	kIoctlBusChannelMapGet =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x949, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_CHANNEL_MAPPING
	kIoctlBusChannelsGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x948, METHOD_BUFFERED, FILE_ANY_ACCESS), // IOCTL_DICE_GET_CHANNEL_INFO
	kIoctlBusOpModeSet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x943, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_SET_OP_MODE
	kIoctlBusOpModeGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x944, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_OP_MODE
	kIoctlBusErrorsGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x942, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_AUDIO_ERROR_COUNT
	kIoctlBusErrorsClear =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x958, METHOD_BUFFERED, FILE_ANY_ACCESS), // IOCTL_DICE_CLEAR_AUDIO_ERROR_COUNT
	kIoctlBusDpcStatsGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x960, METHOD_BUFFERED, FILE_ANY_ACCESS),
	kIoctlBusDpcStatsClear =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x961, METHOD_BUFFERED, FILE_ANY_ACCESS),
	kIoctlBusAc3ChannelsSet	=		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x954, METHOD_BUFFERED, FILE_ANY_ACCESS), // IOCTL_DICE_SET_AC3_CHANNEL_MAPPING
	kIoctlBusClientsGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x841, METHOD_BUFFERED, FILE_ANY_ACCESS),
	kIoctlBusAddToReportedLatency =	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x962, METHOD_BUFFERED, FILE_ANY_ACCESS),

	kIoctlDeviceNotificationsGet =	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x945, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_READ_NOTIFICATION_QUAD
	kIoctlDeviceClockGet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x924, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_GET_CURRENT_CLOCK_PARAM
	kIoctlDeviceNicknameSet =		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x932, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_SET_NICKNAME
	kIoctlDeviceOhciRead =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x914, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_ASYNC_READ
	kIoctlDeviceOhciWrite =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x915, METHOD_BUFFERED, FILE_ANY_ACCESS),	// IOCTL_DICE_ASYNC_WRITE
	kIoctlDeviceStateSet =			CTL_CODE(FILE_DEVICE_UNKNOWN, 0x941, METHOD_BUFFERED, FILE_ANY_ACCESS), // IOCTL_DICE_SET_DEVICE_STATE 

#endif
};



//\cond IOCTL

namespace ioctl
{

///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::VERSION_GET
//
///////////////////////////////////////////////////////////////////////////////
///
///	version compatibility IOCTL
///
struct VERSION_GET
{
	uint32		driverVersion32;	///< returns kTCAT_DICE_VERSION32
	uint32		ioctlAPIVersion;	///< returns kIoctlAPIVersion
};


///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_NOTIFICATION_32 
//
///////////////////////////////////////////////////////////////////////////////
///
///	bus notification events IOCTL (used by kIoctlBusNotificationsSet on 64 bit platform by 32 bit applications)
///
struct BUS_NOTIFICATION_32
{
	void* POINTER_32 hNotification[kBusNotifications];	///< The notification events
};


///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_GET_IO_SIZE_MIN_MAX 
//
///////////////////////////////////////////////////////////////////////////////
struct BUS_GET_IO_SIZE_MIN_MAX
{
	uint32		minimum;		///< minimum buffer size
	uint32		maximum;		///< maximum buffer size
};


///////////////////////////////////////////////////////////////////////////////
///
/// channel - speaker index routing IOCTL (used by kIoctlBusInChannelMapSet, kIoctlBusOutChannelMapSet)
///
///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_GET_CHANNEL_MAP
//
struct BUS_GET_CHANNEL_MAP
{
	int inChannel[kTCAT_DICE_MAX_WDM_IN_CHANNELS];		///< Audio channel numbers 1- 8. 
	int outChannel[kTCAT_DICE_MAX_WDM_OUT_CHANNELS];	///< Audio channel numbers 1- 8
};


///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_SET_OUT_CHANNEL_MAP
//
struct BUS_SET_OUT_CHANNEL_MAP
{
	int wdmSeqIndex;	 ///< WDM sequence index to map i.e. speaker index
	int outChannelIndex; ///< out channel index to which WDM sequence should be mapped.
};

///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_SET_IN_CHANNEL_MAP
//
struct BUS_SET_IN_CHANNEL_MAP
{
	int wdmSeqIndex;	 ///< WDM sequence index to map 
	int inChannelIndex;  ///< in channel index to which WDM sequence should be mapped.
};


#define CLIENT_CHANNEL_NAME_LENGTH	kTCAT_DICE_NICKNAME + kTCAT_DICE_CHANNELNAME + 1

///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_CHANNELS
//
///////////////////////////////////////////////////////////////////////////////
///
///	audio channel information IOCTL (used by kIoctlBusChannelsGet)
///
struct BUS_CHANNELS
{
	uint32 channels;														///< Number of audio channels
	uint32 out;																///< in=false; out=true
	struct 
	{
		char name[CLIENT_CHANNEL_NAME_LENGTH];								///< Channel name
		uint32 ac3_capable;													///< AC3 capabilty 
	}	channel[kTCAT_DICE_DEVICES * kTCAT_DICE_RX * kTCAT_DICE_MAX_CHANNELS_PER_STREAM];	///< Channel struct
	uint32 ac3_map[kTCAT_DICE_AC3_CHANNELS];								///< AC3 channel map
};


///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_AC3_MAP
//
///////////////////////////////////////////////////////////////////////////////
///
struct BUS_AC3_MAP
{
	uint32 channel; ///< ???
	uint32 ac3_channel; ///< ???
};


///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::CLIENT_COUNT
//
///////////////////////////////////////////////////////////////////////////////
///
///	client count IOCTL (used by kIoctlClientCountGet)
///

struct BUS_CLIENT_COUNT
{
	uint32 asioClientCount; ///< Number of of ASIO clients open; 0 means no ASIO client open
	uint32 wdmClientCount; ///< Number of WDM clients open; 0 means no WDM client open
	uint32 diceDriverClientCount; ///< Number of any other clients open (like the control panel)
};



} // namespace ioctl

//\endcond IOCTL

} // namespace dice
} // namespace tcat

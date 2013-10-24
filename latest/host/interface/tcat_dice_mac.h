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
//	Mac OS X platform dependencies:  not to expose to applications!
//
//==============================================================================

#pragma once


namespace tcat
{
namespace dice
{

//\cond IOCTL

#define kTCAT_DICE_MIDI_BUFFER_SIZE				1024


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::IOCTL
//
///////////////////////////////////////////////////////////////////////////////
//!
//!	platform dependent device I/O control
//!

enum
{
	kIoctlAPIVersion = 0x1003,
};


enum IOCTL
{
										//	Input						Output
										// -----------------------------------------------------------------
	kIoctlVersionGet,					//	none						current: 2 scalers (kTCAT_DICE_VERSION32, kIoctlAPIVersion)
										//								legacy : 1 scaler  (kTCAT_DICE_VERSION32)

	kIoctlVendorVersionStrGet,			//	none						VENDOR_VERSION_STR

	kIoctlBusNotificationsSet,			//	ioctl::BUS_NOTIFICATION		none
	kIoctlBusConfigGet,					//	none						ioctl::BUS_CONFIG
	kIoctlBusClockSet,					//	uint32						none
	kIoctlBusSyncSet,					//	uint32						none
	kIoctlBusMasterSet,					//	uint64						none
	kIoctlBusOpModeGet,					//	none						uint32
	kIoctlBusOpModeSet,					//	uint32						none
	kIoctlBusPersistanceNicknameGet,	//	none						DEVICE_NICKNAME_TABLE
	kIoctlBusPersistanceDisabledGet,	//	none						DEVICE_DISABLED_TABLE
	kIoctlBusErrorsGet,					//	none						BUS_ERROR
	kIoctlBusErrorsClear,				//	none						none
	kIoctlBusControllerGuid64Get,		//	uint32						uint64
	kIoctlBusAddToReportedLatency,		//	BUS_ADD_AUDIO_LATENCY		none

	kIoctlDeviceClockGet,				//	uint32						DEVICE_CLOCK
	kIoctlDeviceNicknameSet,			//	DEVICE_NICKNAME				DEVICE_NICKNAME
	kIoctlDeviceNotificationsGet,		//	none						DEVICE_NOTIFICATIONS
	kIoctlDeviceStateSet,				//	DEVICE_SET_STATE			none
	kIoctlDeviceOhciRead,				//	OHCI<1>						OHCI<1>
	kIoctlDeviceOhciWrite,				//	OHCI<1>						none
#if HAS_OHCI_ASYNC_IOCTL
	kIoctlDeviceOhciReadRequest,		//	ASYNC_OHCI<1>				void* (asyncReadHandle)
	kIoctlDeviceOhciReadResponse,		//	void* (asyncReadHandle)		OHCI<1>
	kIoctlDeviceOhciWriteRequest,		//	ASYNC_OHCI<1>				void* (asyncReadHandle)
#endif

	kIoctlMIDIInfoGet,					//	none						DEVICE_MIDI_INFO
	kIoctlMIDIDataReceivedEventSet,		//	DEVICE_MIDI_NOTIFICATION	none

	kIoctlDriverSettingsFlagGetClear,	//  none						none
	kIoctlDriverSettingsRestore,		//  DRIVER_RESTORE_SETTINGS		none

	kIoctlItems
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl
//
///////////////////////////////////////////////////////////////////////////////
///
///	OS X specific IOCTL structures used to communicate with driver
///
namespace ioctl
{

// for BUS_NOTIFICATION magicVal field
enum
{
	kClientUnknown		= 0,
	kClient32Bit		= 32,
	kClient64Bit		= 64,
};


///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DEVICE_NICKNAME_TABLE
//
///////////////////////////////////////////////////////////////////////////////
///
///	nickname table entry
///
struct NICKNAME_TABLE_ENTRY
{
	uint64		guid64;					///< WWUID from Config ROM Bus Info Block
	NICKNAME	nickname;				///< device nickname
};


///
///	nickname table access IOCTL (used by kIoctlBusPersistanceNicknameGet)
///
struct DEVICE_NICKNAME_TABLE
{
	NICKNAME_TABLE_ENTRY	nicknames[kTCAT_DICE_DEVICES_IN_TABLE];	///< Device dependent information
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DEVICE_DISABLED_TABLE
//
///////////////////////////////////////////////////////////////////////////////
///
///	disabled device table access IOCTL (used by kIoctlBusPersistanceDisabledGet)
///
struct DEVICE_DISABLED_TABLE
{
	uint64		disabled_guid64[kTCAT_DICE_DEVICES_IN_TABLE];	///< list of disabled devices' GUID
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DEVICE_SET_STATE
//
///////////////////////////////////////////////////////////////////////////////
///
///	set device state IOCTL (used by kIoctlDeviceStateSet)
///
struct DEVICE_SET_STATE
{
	DEVICE_INFO::STATE	deviceState;	///< The state to be set.
	uint32				idx;			///< Index referring to a particular device.
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DEVICE_MIDI_INFO
//
///////////////////////////////////////////////////////////////////////////////
///
///	MIDI port info IOCTL (used by kIoctlMIDIInfoGet)
///
struct DEVICE_MIDI_INFO
{
	//\cond
	uint32		__pad_0;
	//\endcond
	uint32		numDevices;								///< total number of valid devices in diceDevInfo.
	DEVICE_INFO	diceDevInfo[kTCAT_DICE_DEVICES];		///< this stores nicknames, disabled state, device index, and unique ID for each device.
	uint32		numMIDIOutPorts[kTCAT_DICE_DEVICES];	///< Array to hold the number of MIDI Out ports available on each of the devices.
	uint32		numMIDIInPorts[kTCAT_DICE_DEVICES];		///< Array to hold the number of MIDI In ports on each of the devices.
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DEVICE_MIDI_NOTIFICATION
//
///////////////////////////////////////////////////////////////////////////////
//\cond
///
///	MIDI port notification events IOCTL (used by kIoctlMIDIDataReceivedEventSet)
///
struct DEVICE_MIDI_NOTIFICATION
{
	uint64		callbackPtr;
	uint64		refConPtr;
	uint64		magicVal;
};
//\endcond



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DRIVER_RESTORE_SETTINGS
//
///////////////////////////////////////////////////////////////////////////////
//\cond
///
///	set device state IOCTL (used by kIoctlDriverSettingsRestore)
///
struct DRIVER_RESTORE_SETTINGS
{
	uint32					ioctlAPIVersion;
	uint32					opMode;
	uint64					master_guid64;
	SYNC					sync;
	CLOCK					sampleRate;
	DEVICE_NICKNAME_TABLE	deviceNickNameTable;
	DEVICE_DISABLED_TABLE	deviceDisabledTable;
};
//\endcond



} // namespace ioctl



///////////////////////////////////////////////////////////////////////////////
//
//	DICE_MIDI_BUFFERS
//
///////////////////////////////////////////////////////////////////////////////
///
///	information to be shared between the MIDI driver and the kernel driver
///

#define MIDI_TX_FIFO_MAX		16

struct DEVICE_MIDI_BUFFER
{
	uint32		midiHead;										///< The location in the MIDI buffer where the next MIDI byte should be stored.
	uint32		midiTail;										///< The location in the MIDI buffer from where the next MIDI byte should be read.

	uint32		midiSentExtOffsets[MIDI_TX_FIFO_MAX];
	uint32		midiSentExtOffsetsNumFree;
	uint32		midiSentExtOffsetsMask;
	uint32		midiSentExtOffsetsHead;
	uint32		midiSentExtOffsetsTail;
	int32		midiSentExtOffsetsThreshold;
	//\cond
	uint32		__pad_0;
	//\endcond

	uint64		midiTimeStampLastSent;							///< The time when the last MIDI event was sent.

	uint64		midiTimeStamp[kTCAT_DICE_MIDI_BUFFER_SIZE];		///< this is expressed in nanosecond time units
	uint8		midiByte[kTCAT_DICE_MIDI_BUFFER_SIZE];			///< buffer that contains the actual MIDI data
};


///
/// Contains MIDI buffers for all the ports on all the devices!
///
struct DEVICE_MIDI_BUFFERS
{
	DEVICE_MIDI_BUFFER	midiInBuffers[kTCAT_DICE_DEVICES][kTCAT_DICE_MAX_MIDI_PORTS_PER_DEVICE];	///< MIDI In port buffers.
	DEVICE_MIDI_BUFFER	midiOutBuffers[kTCAT_DICE_DEVICES][kTCAT_DICE_MAX_MIDI_PORTS_PER_DEVICE];	///< MIDI Out Port buffers.
};


/// These are User/Kernel "shared" memory "types"
typedef enum
{
	kMIDIBuffers,			///< Retrieve address of the DICE_MIDI_BUFFERS structure in the Dice Interface device.
	kInterfaceReadyFlag,	///< Retrieve address of the Interface ready flag (uint32).
	kNumberOfMemoryTypes	///< Total number of User/Kernel shared memory types.
} UserClientMemoryTypes;


//\endcond IOCTL

} // namespace dice
} // namespace tcat

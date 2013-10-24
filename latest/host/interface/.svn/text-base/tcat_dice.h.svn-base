//==============================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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
//	common dice defines interfacing firmware, driver and pal.
//
#pragma once

#include "tcat.h"


namespace tcat
{
namespace dice
{

///////////////////////////////////////////////////////////////////////////////
//
//	defines
//
///////////////////////////////////////////////////////////////////////////////
//
#define kTCAT_DICE_FW_BASE_HI				0xffff
#define	kTCAT_DICE_FW_BASE				0xe0000000
//#define kTCAT_DICE_FW_OFFSET_APP		  0xa00000
#define kTCAT_DICE_FW_OFFSET_FWL		  0x100000

#define kTCAT_DICE_AC3_CHANNELS					2
#define kTCAT_DICE_TX							2
#define kTCAT_DICE_RX							4
#define kTCAT_DICE_DEVICES						5
#define kTCAT_DICE_MAX_CHANNELS_PER_STREAM		64
#define kTCAT_DICE_DEVICES_IN_TABLE				32
#define kTCAT_DICE_CHANNELNAME					32
#define kTCAT_DICE_NICKNAME						64
#define kTCAT_DICE_SYNCNAME						256
#define kTCAT_DICE_CLOCKNAMES					256
#define kTCAT_DICE_MAX_MIDI_PORTS_PER_STREAM	8
#define kTCAT_DICE_MAX_MIDI_PORTS_PER_DEVICE	(kTCAT_DICE_MAX_MIDI_PORTS_PER_STREAM * kTCAT_DICE_TX)



///////////////////////////////////////////////////////////////////////////////
//
//	SYNC
//
///////////////////////////////////////////////////////////////////////////////
///
///	sync options (replacement for clock sources - name changed to differentiate
///	from clock/sample rates)
///
enum SYNC
{
	kSyncAes0,		///< AES Receiver 0
	kSyncAes1,		///< AES Receiver 1
	kSyncAes2,		///< AES Receiver 2
	kSyncAes3,		///< AES Receiver 3
	kSyncAesAny,	///< First available AES Receiver
	kSyncAdat,		///< ADAT In
	kSyncTdif,		///< TDIF In
	kSyncWc,		///< Word CLock
	kSyncArx0,		///< AVS Receiver 0
	kSyncArx1,		///< AVS Receiver 1
	kSyncArx2,		///< AVS Receiver 2
	kSyncArx3,		///< AVS Receiver 3
	kSyncInternal,	///< Internal Crystal

	kSyncItems
};



///////////////////////////////////////////////////////////////////////////////
//
//	SYNC_CAPS
//
///////////////////////////////////////////////////////////////////////////////
///
///	bit map indicating supported sync sources
///
enum SYNC_CAPS
{
	kSyncCapsAes0 =		1 << (16 + kSyncAes0),			///< Sync support to AES0 
	kSyncCapsAes1 =		1 << (16 + kSyncAes1),			///< Sync support to AES1
	kSyncCapsAes2 =		1 << (16 + kSyncAes2),			///< Sync support to AES2
	kSyncCapsAes3 =		1 << (16 + kSyncAes3),			///< Sync support to AES3
	kSyncCapsAesAny =	1 << (16 + kSyncAesAny),		///< Sync support to the first available AES Receiver
	kSyncCapsAdat =		1 << (16 + kSyncAdat),			///< Sync support to ADAT
	kSyncCapsTdif =		1 << (16 + kSyncTdif),			///< Sync support to TDIF
	kSyncCapsWc =		1 << (16 + kSyncWc),			///< Sync support to WC
	kSyncCapsArx0 =		1 << (16 + kSyncArx0),			///< Sync support to ARX0
	kSyncCapsArx1 =		1 << (16 + kSyncArx1),			///< Sync support to ARX1
	kSyncCapsArx2 =		1 << (16 + kSyncArx2),			///< Sync support to ARX2
	kSyncCapsArx3 =		1 << (16 + kSyncArx3),			///< Sync support to ARX3
	kSyncCapsInternal =	1 << (16 + kSyncInternal)		///< Sync support to Internal
};



///////////////////////////////////////////////////////////////////////////////
//
//	CLOCK
//
///////////////////////////////////////////////////////////////////////////////
///
///	clock rate options (replacement for sample rate - name changed to 
///	differentiate from SYNC/clock source)
///
enum CLOCK
{
	kClock32,		///< 32KHz nominal
	kClock44,		///< 44.1KHz nominal
	kClock48,		///< 48KHz nominal
	kClock88,		///< 88KHz nominal
	kClock96,		///< 96KHz nominal
	kClock176,		///< 176KHz nominal
	kClock192,		///< 192KHz nominal
	kClockAnyLo,	///< 32-48KHz range
	kClockAnyMid,	///< 88-96KHz range
	kClockAnyHi,	///< 176-192KHz range
	kClockNone,		///< Unspecified, or User rate
	kClockItems
};



///////////////////////////////////////////////////////////////////////////////
//
//	CLOCK_CAPS
//
///////////////////////////////////////////////////////////////////////////////
///
///	bit map indicating supported clocks
///
enum CLOCK_CAPS
{
	kClockCaps32 =	1 << kClock32,
	kClockCaps44 =	1 << kClock44,
	kClockCaps48 =	1 << kClock48,
	kClockCaps88 =	1 << kClock88,
	kClockCaps96 =	1 << kClock96,
	kClockCaps176 =	1 << kClock176,
	kClockCaps192 =	1 << kClock192
};



///////////////////////////////////////////////////////////////////////////////
//
//	CLOCK_SYNC_CAPS
//
///////////////////////////////////////////////////////////////////////////////
///
///	combined bit map indicating supported clocks and sync sources
///
union CLOCK_SYNC_CAPS
{
	CLOCK_CAPS	clock;	///< bit map indicating supported clocks
	SYNC_CAPS	sync;	///< clock rate options
};



///////////////////////////////////////////////////////////////////////////////
//
//	OP_MODE
//
///////////////////////////////////////////////////////////////////////////////
///
///	operation modes
///
enum OP_MODE
{
	kOpModeLowestLatency,
	kOpModeNormal,
	kOpModeSafeMode1,	
	kOpModeSafeMode2,
	kOpModeItems
};



///////////////////////////////////////////////////////////////////////////////
//
//	BUS_NOTIFICATION
//
///////////////////////////////////////////////////////////////////////////////
///
///	bus event notification sources
///
enum BUS_NOTIFICATION 
{
	kBusNotificationDeviceArrived,		///< DEVICE_ADD_EVENT
	kBusNotificationDeviceRemoved,		///< DEVICE_REMOVE_EVENT
	kBusNotificationBusReset,			///< BUS_RESET_EVENT
	kBusNotificationConfigChanged,		///< CONFIG_CHANGE_EVENT
	kBusNotificationInterfaceReady,		///< INTERFACE_READY_EVENT
	kBusNotificationDeviceNotification,	///< DEVICE_WRITE_EVENT
	kBusNotifications
};



///////////////////////////////////////////////////////////////////////////////
//
//	DEVICE_NOTIFICATION
//
///////////////////////////////////////////////////////////////////////////////
///
///	notification source definitions for kBusNotificationDeviceNotification
///
enum DEVICE_NOTIFICATION				/// device notification flags
{
	kDeviceNotificationRx =				1 << 0,	///< 0x01: RX configuration has changed
	kDeviceNotificationTx =				1 << 1,	///< 0x02: TX configuration has changed
	kDeviceNotificationIsocDuplicated =	1 << 2,	///< 0x04: Duplicate Isoc. Channel reported
	kDeviceNotificationBwError =		1 << 3,	///< 0x08: Isoc. TX slot not granted (not enough bandwidth)
	kDeviceNotificationLock =			1 << 4,	///< 0x10: Clock lock status has changed
	kDeviceNotificationClockAccept =	1 << 5,	///< 0x20: Clock settings have been accepted by the firmware
	kDeviceNotificationExStatus =		1 << 6,	///< 0x40: extended status has changed

	kDeviceNotificationUser =			0xffff0000, ///< ...
	kDeviceNotificationAll =			0xffffffff, ///< ...
	kDeviceNotificationNone =			0		 ///< ...
};



///////////////////////////////////////////////////////////////////////////////
//
//	DEVICE_NOTIFICATIONS
//
///////////////////////////////////////////////////////////////////////////////
///
///	struct containing DEVICE_NOTIFICATION for all devices
///
struct DEVICE_NOTIFICATIONS
{	
	DEVICE_NOTIFICATION		notification[kTCAT_DICE_DEVICES];	///< notification source definitions for kBusNotificationDeviceNotification
};



///////////////////////////////////////////////////////////////////////////////
//
//	STATUS
//
///////////////////////////////////////////////////////////////////////////////
///
///	device lock status
///
enum STATUS
{
	kStatusLocked = 1,
	kStatusNoLock
};



///////////////////////////////////////////////////////////////////////////////
//
//	EX_STATUS
//
///////////////////////////////////////////////////////////////////////////////
///
///	device extended status bitmap
///
enum EX_STATUS
{
	kExStatusAes0,
	kExStatusAes1,
	kExStatusAes2,
	kExStatusAes3,
	kExStatusAdat,
	kExStatusTdif,
	kExStatusArx0,
	kExStatusArx1,
	kExStatusArx2,
	kExStatusArx3,
	kExStatusItems,

	kExStatusLocked = 1,
	kExStatusLockedMask = 0xffff,
	kExStatusLockedAes0 = kExStatusLocked << kExStatusAes0,
	kExStatusLockedAes1 = kExStatusLocked << kExStatusAes1,
	kExStatusLockedAes2 = kExStatusLocked << kExStatusAes2,
	kExStatusLockedAes3 = kExStatusLocked << kExStatusAes3,
	kExStatusLockedAdat = kExStatusLocked << kExStatusAdat,
	kExStatusLockedTdif = kExStatusLocked << kExStatusTdif,
	kExStatusLockedArx0 = kExStatusLocked << kExStatusArx0,
	kExStatusLockedArx1 = kExStatusLocked << kExStatusArx1,
	kExStatusLockedArx2 = kExStatusLocked << kExStatusArx2,
	kExStatusLockedArx3 = kExStatusLocked << kExStatusArx3,

	kExStatusSlip = 0x10000,
	kExStatusSlipMask = 0xffff0000,
	kExStatusSlipAes0 = kExStatusSlip << kExStatusAes0,
	kExStatusSlipAes1 = kExStatusSlip << kExStatusAes1,
	kExStatusSlipAes2 = kExStatusSlip << kExStatusAes2,
	kExStatusSlipAes3 = kExStatusSlip << kExStatusAes3,
	kExStatusSlipAdat = kExStatusSlip << kExStatusAdat,
	kExStatusSlipTdif = kExStatusSlip << kExStatusTdif,
	kExStatusSlipArx0 = kExStatusSlip << kExStatusArx0,
	kExStatusSlipArx1 = kExStatusSlip << kExStatusArx1,
	kExStatusSlipArx2 = kExStatusSlip << kExStatusArx2,
	kExStatusSlipArx3 = kExStatusSlip << kExStatusArx3
};



typedef char NICKNAME[kTCAT_DICE_NICKNAME];	///< Device nickname char array
typedef char SYNCNAME[kTCAT_DICE_SYNCNAME];	///< Device sync source names char array



///////////////////////////////////////////////////////////////////////////////
//
//	DEVICE_INFO
//
///////////////////////////////////////////////////////////////////////////////
///
///	containing device dependent information
///
struct DEVICE_INFO 
{
	uint64		guid64;					///< WWUID from Config ROM Bus Info Block
	NICKNAME	nickname;				///< Default device name
	enum STATE							///  Device state information
	{
		DEVICE_STATE_ACTIVE = 1,        ///< Device working properly
		DEVICE_STATE_LOW_RESOURCES,     ///< Device not active due to insufficient resources
		DEVICE_STATE_LOCK_FAILURE,      ///< Device not active due to lock Failure
		DEVICE_STATE_VERSION_CONFLICT,  ///< Device not active due to mismatch in firmware versions
		DEVICE_STATE_USER_DISABLED,     ///< Device not active because the user disabled it
		DEVICE_STATE_UNKNOWN_ERROR,		///< Device got in passive mode due to some unknown error
		DEVICE_STATE_REMOVED,			///< Device is removed
		DEVICE_STATE_ALREADY_LOCKED,	///< Device is already locked
	}			state;					///< Device state information
	uint32		idx;					///< Index referring to a particular device;
	uint32		capabilities;			///< Device capability flags (enum DD_CAPABILITIES)
	//\cond
	uint32		__pad_0;
	//\endcond
};



//\cond IOCTL

///////////////////////////////////////////////////////////////////////////////
//
//	MIDI_DEVICE_INFO
//
///////////////////////////////////////////////////////////////////////////////
///
///	containing device dependent MIDI port information
///
struct MIDI_DEVICE_PORT_INFO
{
	const char*				name;				///< MIDI port name
	uint32					maxBytesPerSec;		///< maximum send speed in bytes/sec
#ifdef __MACH__
	bool					embedded;			///< OS X only:  indicates MIDI port is embedded -- it has no corresponding external connector
#endif
};


///
///	containing device dependent MIDI information
///
struct MIDI_DEVICE_INFO
{
	uint32						productID;		///< the device's kTCAT_DICE_DEVICE_x_ID value
	const char* 				name;			///< MIDI device name
#ifdef __MACH__
	const char* const			manufacturer;	///< OS X only: this corresponds to the Manufacturer field in Audio MIDI Setup
	const char* const			deviceName;		///< OS X only: this corresponds to the Device Name field in Audio MIDI Setup
	const char* const			iconFileName;	///< OS X only: filename of the image file used for the icon in Audio MIDI Setup
#endif
	MIDI_DEVICE_PORT_INFO		ports[kTCAT_DICE_MAX_MIDI_PORTS_PER_DEVICE];	///< array of MIDI_DEVICE_PORT_INFO ports
};



///////////////////////////////////////////////////////////////////////////////
//
//	OHCI_ADDRESS
//
///////////////////////////////////////////////////////////////////////////////
///
///	OHCI address space definition
///
struct OHCI_ADDRESS
{
	uint32	hi;	///< Hi address
	uint32	lo;	///< Lo address
};



///////////////////////////////////////////////////////////////////////////////
//
//	OHCI_HEADER
//
///////////////////////////////////////////////////////////////////////////////
///
///	header for OHCI async transactions
///
struct OHCI_HEADER
{
	OHCI_ADDRESS	offset;		///< Address space offset
    uint32			bytes;		///< Number of bytes to be accessed
	uint32			idx;		///< Index referring to a particular device;
};

//\endcond IOCTL



//\cond IOCTL

///////////////////////////////////////////////////////////////////////////////
//
//	ioctl
//
///////////////////////////////////////////////////////////////////////////////
///
///	IOCTL structures used to communicate with driver
///
namespace ioctl
{

///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::VENDOR_VERSION_STR
//
///////////////////////////////////////////////////////////////////////////////
///
///	vendor version string IOCTL (used by kIoctlVendorVersionStrGet)
///
struct VENDOR_VERSION_STR
{
	char		version[128];	///< vendor version string
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_CONFIG
//
///////////////////////////////////////////////////////////////////////////////
///
///	bus configuration IOCTL (used by kIoctlBusConfigGet)
///
//#pragma pack(push,4)
struct BUS_CONFIG
{
 	uint64		master_guid64;						///< The GUID of the sync master device
	uint32		io_size;							///< I/O buffer size
	SYNC		sync;								///< This device is synched to
	CLOCK		clock;								///< The clock (sample rate) setting of this device
	CLOCK_CAPS	clock_caps;							///< The clock capability bit map
	SYNC_CAPS	sync_caps;							///< The sync capability bit map
	//\cond
	uint32		__pad_0;
	//\endcond
	SYNCNAME	syncname[kSyncItems];				///< The device sync source names char array
	DEVICE_INFO device_info[kTCAT_DICE_DEVICES];	///< Device dependent information
	uint32		devices;							///< Number of devices
	uint32		audio_input_channels;				///< Total number of audio input channels
	uint32		audio_output_channels;				///< Total number of audio output channels
	//\cond
	uint32		__pad_1;
	//\endcond
};
//#pragma pack(pop)


///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_NOTIFICATION
//
///////////////////////////////////////////////////////////////////////////////
///
///	bus notification events IOCTL (used by kIoctlBusNotificationsSet)
///
struct BUS_NOTIFICATION
{
#ifdef __MACH__
	uint64	hNotification[kBusNotifications];	///< notification events
	uint64	pRefCon;							///< object back pointer
	uint64	magicVal;							///< used to indicate client architecture
#else
	void*	hNotification[kBusNotifications];	///< notification events
#endif
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_ADD_AUDIO_LATENCY 
//
///////////////////////////////////////////////////////////////////////////////
///
///	structure used to dynamically increase the driver's reported audio latency
///
struct BUS_ADD_AUDIO_LATENCY
{
	uint32		inputSamples;		///< The number of samples to add to the driver's reported audio input latency
	uint32		outputSamples;		///< The number of samples to add to the driver's reported audio output latency
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_ERROR
//
///////////////////////////////////////////////////////////////////////////////
///
///	structure used to retrieve bus streaming related errors
///
struct BUS_ERROR
{
	enum
	{
//\cond
		kBusErrorWatchdogTimer,			///< The watchdog timer detected no callbacks coming in.
		kBusErrorMissedCallbacks,		///< A "missed' callback was detected. This could also mean "missing" send packets on the bus.
		kBusErrorDbcFail,				///< A bad (unexpected) dbc was received.
		kBusErrorSytFail,				///< The timestamp(SYT) to be transmitted was detected to be out of range.
		kBusErrorUsermodeDelays,
		kBusErrorItems
//\endcond
	};									///< Error counter types
	uint32	error[kBusErrorItems];		///< Error counters
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::BUS_DPC_STATS 
//
///////////////////////////////////////////////////////////////////////////////
struct BUS_DPC_STATS
{
	uint32		maxDriverDpcDelayUs;

	uint32		delayCount_0to99;
	uint32		delayCount_100to199;
	uint32		delayCount_200to299;
	uint32		delayCount_300to399;
	uint32		delayCount_400to499;
	uint32		delayCount_500to599;
	uint32		delayCount_600to699;
	uint32		delayCount_700to799;
	uint32		delayCount_800to899;
	uint32		delayCount_900to999;
	uint32		delayCount_1000to1199;
	uint32		delayCount_1200to1399;
	uint32		delayCount_1400to1599;
	uint32		delayCount_1600to1799;
	uint32		delayCount_1800to1999;
	uint32		delayCount_2000to2999;
	uint32		delayCount_3000to3999;
	uint32		delayCount_4000to4999;
	uint32		delayCount_5000to9999;
	uint32		delayCount_10000to19999;
	uint32		delayCount_20000to29999;
	uint32		delayCount_30000andGreater;

	//\cond
	uint32		__pad_0;
	//\endcond
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DEVICE_CLOCK
//
///////////////////////////////////////////////////////////////////////////////
///
///	clock information IOCTL (used by kIoctlDeviceClockGet)
///
struct DEVICE_CLOCK
{
	uint32		clock;						///< 32bit integer sampling rate in Hz
	STATUS		status;						///< Device lock status
	EX_STATUS	ex_status;					///< Device extended status bitmap
	uint32		idx;						///< Device index
	uint32		tx_locked[kTCAT_DICE_TX];	///< 1 will indicate Stream locked, 0 is for unlocked
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::DEVICE_NICKNAME
//
///////////////////////////////////////////////////////////////////////////////
///
///	nickname access IOCTL (used by kIoctlDeviceNicknameSet)
///
struct DEVICE_NICKNAME
{
	uint32		idx;						///< Device index
	uint32		devices;					///< Number of devices
	DEVICE_INFO	info[kTCAT_DICE_DEVICES];	///< Device dependent information
	NICKNAME	nickname;					///< Device nickname char array
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::OHCI
//
///////////////////////////////////////////////////////////////////////////////
///
///	OHCI async read/write IOCTL (used by kIoctlDeviceOhciRead, 
/// kIoctlDeviceOhciWrite)
///
template<int size> struct OHCI
{
	OHCI_HEADER	header;		///< header for OHCI async transactions
	uint32		data[size];	///< payload
};



///////////////////////////////////////////////////////////////////////////////
//
//	ioctl::ASYNC_OHCI
//
///////////////////////////////////////////////////////////////////////////////
///
///	(OSX:) OHCI async read/write IOCTL (used by kIoctlDeviceOhciReadRequest)
///
template<int size> struct ASYNC_OHCI 
{
	void*		callback;	///< asynchronous response callback
	void*		refcon;		///< object back pointer
	OHCI<size>	ohci;		///< header and payload for OHCI async transactions 
};



} // namespace ioctl

//\endcond IOCTL


} // namespace dice

} // namespace tcat

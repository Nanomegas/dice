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
//	see tcat_dice_pal_device.h
//

#include "tcat_dice_pal_system.h"
#include "../../interface/tcat_dice_version.h"
#include "TDL_Interface.h"

#define kDriverLog_File_ID		1001


//\cond
struct PRODUCT_TYPES
{
	unsigned int		id;
	const char* const	type;
};

static const struct PRODUCT_TYPES __product_types[] =
{
#ifdef kTCAT_DICE_DEVICE_1_ID
	{ kTCAT_DICE_DEVICE_1_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_1_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_2_ID
	{ kTCAT_DICE_DEVICE_2_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_2_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_3_ID
	{ kTCAT_DICE_DEVICE_3_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_3_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_4_ID
	{ kTCAT_DICE_DEVICE_4_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_4_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_5_ID
	{ kTCAT_DICE_DEVICE_5_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_5_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_6_ID
	{ kTCAT_DICE_DEVICE_6_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_6_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_7_ID
	{ kTCAT_DICE_DEVICE_7_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_7_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_8_ID
	{ kTCAT_DICE_DEVICE_8_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_8_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_9_ID
	{ kTCAT_DICE_DEVICE_9_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_9_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_10_ID
	{ kTCAT_DICE_DEVICE_10_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_10_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_11_ID
	{ kTCAT_DICE_DEVICE_11_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_11_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_12_ID
	{ kTCAT_DICE_DEVICE_12_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_12_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_13_ID
	{ kTCAT_DICE_DEVICE_13_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_13_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_14_ID
	{ kTCAT_DICE_DEVICE_14_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_14_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_15_ID
	{ kTCAT_DICE_DEVICE_15_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_15_DESC) },
#endif
#ifdef kTCAT_DICE_DEVICE_16_ID
	{ kTCAT_DICE_DEVICE_16_ID, TCAT_STRINGIFY(kTCAT_DICE_DEVICE_16_DESC) },
#endif
};
//\endcond



#ifdef WIN32
#include <windows.h>
#include "../interface/tcat_dice_win.h"
#endif//WIN32
#ifdef __MACH__
#include "../interface/tcat_dice_mac.h"
#include "tcat_osx.h"
#include <sys/errno.h>
#endif//__MACH__
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace tcat;
using namespace dice;



#ifdef __MACH__
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::fwlib
//
///////////////////////////////////////////////////////////////////////////////
//
//	This wrapper ensures correct balance of FWLib open/close - even in case of
//	an exception
//
//\cond
class fwlib
{
public:
	fwlib(IOFireWireLibDeviceRef interface) throw(tcat::exception::base): m_interface( interface )
	{
		IOReturn err;
		while ((err = (**m_interface).Open(m_interface))==kIOReturnExclusiveAccess)
			pal::delay(1);
		TCAT_EXCEPTION_THROW (err,xptn_device_access);
	}
#ifdef TCAT_DICE_AVC
	fwlib(IOFireWireLibDeviceRef interface, IOFireWireSessionRef session) throw(tcat::exception::base): m_interface( interface )
	{
		TCAT_EXCEPTION_THROW((**m_interface).OpenWithSessionRef(m_interface,session),xptn_device_access);
	}
#endif

	~fwlib()
	{
		if (m_interface)
		{
			(**m_interface).Close(m_interface);
			m_interface = NULL;
		}
	}

	void read (uint32 offset, void* data32, uint32 bytes) const throw(tcat::exception::base)
	{
		if (bytes > m_maxReadWriteSize)
		{
			TDL_ENTRY( kTDLFlag_Verbose | kTDLFlag_PAL, "PAL fwlib splitting into multiple reads (offset 0x%08x, data %p, bytes %d)",
					   offset, data32, bytes, 0 );
		}

		while (bytes)
		{
			uint32	bytesToRead = (bytes > m_maxReadWriteSize) ? m_maxReadWriteSize : bytes;
			uint32	bytesRead = bytesToRead;

			FWAddress addr = FWAddress(kTCAT_DICE_FW_BASE_HI, kTCAT_DICE_FW_BASE | offset);
			IOReturn err = (**m_interface).Read(m_interface, (**m_interface).GetDevice(m_interface), &addr, data32, (UInt32*)&bytesRead, kFWDontFailOnReset, 0);
			if (err)
			{
				TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL fwlib read() error 0x%08x  (offset 0x%08x, data %p, bytes %d)",
						   err, offset, data32, bytesRead );
				TCAT_EXCEPTION_THROW (err, xptn_device_timeout);
			}

			if (bytesRead != bytesToRead)
			{
				TCAT_EXCEPTION_THROW ((bytesRead != bytesToRead), xptn_device_timeout);
			}

			offset += bytesRead;
			data32 = ((uint8*)data32) + bytesRead;
			bytes -= bytesRead;
		}
	}

	void write (uint32 offset, const void* data32, uint32 bytes) const throw(tcat::exception::base)
	{
		if (bytes > m_maxReadWriteSize)
		{
			TDL_ENTRY( kTDLFlag_Verbose | kTDLFlag_PAL, "PAL fwlib splitting into multiple writes (offset 0x%08x, data %p, bytes %d)",
					   offset, data32, bytes, 0 );
		}

		while (bytes)
		{
			uint32	bytesToWrite = (bytes > m_maxReadWriteSize) ? m_maxReadWriteSize : bytes;
			uint32	bytesWritten = bytesToWrite;

			FWAddress addr = FWAddress(kTCAT_DICE_FW_BASE_HI, kTCAT_DICE_FW_BASE | offset);
			IOReturn err = (**m_interface).Write(m_interface, (**m_interface).GetDevice(m_interface), &addr, data32, (UInt32*)&bytesWritten, kFWDontFailOnReset, 0);
			if (err)
			{
				TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL fwlib write() error 0x%08x  (offset 0x%08x, data %p, bytes %d)",
						   err, offset, data32, bytesToWrite );
				TCAT_EXCEPTION_THROW (err, xptn_device_timeout);
			}

			if (bytesWritten != bytesToWrite)
			{
				TCAT_EXCEPTION_THROW ((bytesWritten != bytesToWrite), xptn_device_timeout);
			}

			offset += bytesWritten;
			data32 = ((uint8*)data32) + bytesWritten;
			bytes -= bytesWritten;
		}
	}

private:
	static const uint32			m_maxReadWriteSize = 512;
	IOFireWireLibDeviceRef		m_interface;
};



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::avclib
//
///////////////////////////////////////////////////////////////////////////////
#ifdef TCAT_DICE_AVC
class avclib
{
public:
	avclib(IOFireWireAVCLibUnitInterfaceRef interface) throw(tcat::exception::base): m_interface(NULL)
	{
		m_interface = interface;
		IOReturn err = 0;
		while ((err = (*m_interface)->open(m_interface))==kIOReturnExclusiveAccess)
			pal::delay(1);
		TCAT_EXCEPTION_THROW (err,xptn_device_access);
	}
	~avclib()
	{
		if (m_interface)
			(*m_interface)->close(m_interface);
	}
	operator IOFireWireSessionRef()
	{
		return (*m_interface)->getSessionRef(m_interface);
	}
private:
	IOFireWireAVCLibUnitInterfaceRef m_interface;
};
#endif // #ifdef TCAT_DICE_AVC
//\endcond
#endif



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::device
//
///////////////////////////////////////////////////////////////////////////////
//
device::device():
	#ifdef __MACH__
	#ifdef TCAT_DICE_AVC
	m_avc_interface(NULL),
	#endif//TCAT_DICE_AVC
	m_fw_interface(NULL),
	#endif//__MACH
	m_notification(kDeviceNotificationAll)
{
	m_bus.owner(this);
	memset (&m_spaces, 0, sizeof m_spaces);
	memset (&m_cache, 0xff, sizeof m_cache); 
	memset (&m_isocs, 0, sizeof m_isocs);
	memset (&m_squ, 0, sizeof m_squ);
	memset (&m_midi_ports, 0, sizeof m_midi_ports);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::~device
//
///////////////////////////////////////////////////////////////////////////////
//
device::~device()
{
	#ifdef __MACH__
	#ifdef TCAT_DICE_AVC
	if (m_avc_interface)
		(**(IOFireWireAVCLibUnitInterfaceRef)m_avc_interface).Release((IOFireWireAVCLibUnitInterfaceRef)m_avc_interface);
	#endif//TCAT_DICE_AVC
	if (m_fw_interface)
	{
		(**(IOFireWireLibDeviceRef)m_fw_interface).Release((IOFireWireLibDeviceRef)m_fw_interface);
		m_fw_interface = NULL;
	}
	#endif//__MACH
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::type
//
///////////////////////////////////////////////////////////////////////////////
//
std::string device::type(uint32 product_id)
{
	for (size_t idx=0; idx<sizeof(__product_types)/sizeof(PRODUCT_TYPES); idx++)
		if (product_id == (__product_types[idx].id & kTCAT_DICE_GUID64_PRODUCT_ID_MASK))
			return __product_types[idx].type;
	return "n/a.";
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::create
//
///////////////////////////////////////////////////////////////////////////////
//
void device::create (bus_impl* bus, const DEVICE_INFO& info) throw(tcat::exception::base)
{
	#ifdef __MACH__
	// find correspondenting FWLib interface for async read/write
	CFMutableDictionaryRef dictionary;
	#ifdef TCAT_DICE_AVC
	dictionary = ::IOServiceMatching("IOFireWireAVCUnit");
	::CFDictionaryAddValue (dictionary, CFSTR("GUID"), CFNumber<kCFNumberSInt64Type,uint64>(info.guid64));
	IOFireWireAVCLibUnitInterfaceRef avc_interface = PlugInInterface(kIOFireWireAVCLibUnitTypeID, dictionary);
	m_avc_interface = (void**)avc_interface;
	#endif
	dictionary = ::IOServiceMatching("IOFireWireDevice");
	::CFDictionaryAddValue (dictionary, CFSTR("GUID"), CFNumber<kCFNumberSInt64Type,uint64>(info.guid64));
	IOFireWireLibDeviceRef fw_interface = PlugInInterface(kIOFireWireLibTypeID, dictionary);
	m_fw_interface = (void**)fw_interface;
	#endif//__MACH__

	lock lock(this);
	m_bus = bus;
	m_cache[0].info = info;
		
	ohci_read (0, (uint32*)&m_spaces, sizeof(m_spaces)>>2);
	m_spaces[kSpaceGlobal].offset <<= 2;
	m_spaces[kSpaceRx].offset <<= 2;
	m_spaces[kSpaceTx].offset <<= 2;
	
	// does the firmware support clock/sync caps?
	if (TCAT_OFFSET(DD_GLOBAL,clock_sync_caps)/4 < m_spaces[kSpaceGlobal].size32)
	{
		struct
		{
			CLOCK_SYNC_CAPS	clock_sync_caps;
			SYNCNAME		sync_names;
		} sync;
		ohci_read (m_spaces[kSpaceGlobal].offset + TCAT_OFFSET(DD_GLOBAL,clock_sync_caps), (uint32*) &sync, sizeof(sync)/4);
		#ifdef __BIG_ENDIAN__
		pal::swap32 (sync.sync_names,sync.sync_names,sizeof(sync.sync_names)>>2);
		#endif//__BIG_ENDIAN__
		sync.sync_names[sizeof(sync.sync_names)-1] = '\0';	// forced null-termination
		m_clock_sync_caps = sync.clock_sync_caps;
		std::string names = sync.sync_names;
		std::string::size_type begin = 0;

		for (uint32 squ=0; squ<kSyncItems; squ++)
		{
			// separate the string and distribute it to the channel name array
			std::string::size_type end = names.find('\\',begin);
			m_sync_names.push_back(std::string(names,begin,end-begin));
			begin = ++end;
		}
	}
	else
		m_clock_sync_caps.clock = CLOCK_CAPS(0);

	// request firmware version info
	try
	{
		firmware_package (FL_OP_GET_APP_INFO, 5000);
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, data.rtn.GetAppInfo), (uint32*) &m_fw_info, sizeof(m_fw_info)>>2);			
		#ifdef __BIG_ENDIAN__
		// re-swap the strings:
		pal::swap32 (&m_fw_info.BuildTime, (sizeof(m_fw_info.BuildTime)+sizeof(m_fw_info.BuildDate))>>2);
		#endif
	}
	catch (xptn_device_firmware)
	{
		FL_GET_VENDOR_IMAGE_DESC_RETURN image;
		FL_GET_IMAGE_DESC_RETURN info;
		uint32 sector = 0;
		firmware_package (FL_OP_GET_RUNNING_IMAGE_VINFO, 5000);
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, data.rtn.getImageVInfo), (uint32*) &image, sizeof(image)>>2);			
		// send image ID
		firmware_package (FL_OP_GET_IMAGE_DESC, 5000, &sector);
		// receive image info
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, data.rtn.getImage), (uint32*) &info, sizeof(info)>>2);

		#ifdef __BIG_ENDIAN__
		pal::swap32 (&image.uiVendorID, sizeof(image.uiVendorID)>>2);
		pal::swap32 (m_fw_info.BuildDate, info.BuildDate, TCAT_MIN(sizeof(m_fw_info.BuildDate),sizeof(info.BuildDate)>>2));
		pal::swap32 (m_fw_info.BuildTime, info.BuildTime, TCAT_MIN(sizeof(m_fw_info.BuildTime),sizeof(info.BuildTime)>>2));
		#else
		strncpy (m_fw_info.BuildDate, info.BuildDate, TCAT_MIN(sizeof(m_fw_info.BuildTime),sizeof(info.BuildDate)));
		strncpy (m_fw_info.BuildTime, info.BuildTime, TCAT_MIN(sizeof(m_fw_info.BuildTime),sizeof(info.BuildTime)));
		#endif 

		m_fw_info.uiBaseSDKVersion = TCAT_VERSION32(info.uiVersionHigh,info.uiVersionLow,0,0);
		m_fw_info.uiApplicationVersion = TCAT_VERSION32(image.uiVMajor,image.uiVMinor,0,0);
		m_fw_info.uiVendorID = from_string<uint32>("0x"+std::string(image.uiVendorID));
		if (!m_fw_info.uiVendorID)
			m_fw_info.uiVendorID = kTCAT_DICE_VENDOR_ID;
		m_fw_info.uiProductID = image.uiVProductID;		
		m_fw_info.uiBoardSerialNumber = info.uiBoardSerialNumber;
	}
	ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, capabilities), &m_fwl_capabilities);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::fetch_rx
//
///////////////////////////////////////////////////////////////////////////////
//
void device::fetch_rx() throw(tcat::exception::base)
{
	lock lock(this);
	IO<kTCAT_DICE_RX> rx;
	// read rx header
	ohci_read (m_spaces[kSpaceRx].offset, (uint32*)&rx, (sizeof(rx)-sizeof(rx.isoc))>>2);

	TCAT_EXCEPTION_THROW (rx.isoc_channels > kTCAT_DICE_RX, xptn_internal);

	m_channels[false].clear();
	m_midi_ports[false] = 0;
	for (uint32 isoc=0; isoc<rx.isoc_channels; isoc++)
	{
		// read rx isoc channel
		ohci_read (
			m_spaces[kSpaceRx].offset + (uint32)sizeof(rx)-(uint32)sizeof(rx.isoc) + isoc*rx.size32*4, 
			(uint32*)&rx.isoc[isoc],
			std::min<uint32>(rx.size32,(uint32)sizeof(rx.isoc[isoc])>>2));

		TCAT_EXCEPTION_THROW (rx.isoc[isoc].rx.sequences.audio > kTCAT_DICE_MAX_CHANNELS_PER_STREAM, xptn_internal);

		// extract channel names (endian swapped on ppc)
		#ifdef __BIG_ENDIAN__
		pal::swap32 (rx.isoc[isoc].names, sizeof(rx.isoc[isoc].names) >> 2);	// the ppc needs to swap the string...
		#endif
		rx.isoc[isoc].names[sizeof(rx.isoc[isoc].names)-1] = '\0';	// forced null-termination
		std::string names = rx.isoc[isoc].names;
		std::string::size_type begin = 0;
		for (uint32 squ=0; squ<rx.isoc[isoc].rx.sequences.audio; squ++)
		{
			// separate the string and distribute it to the channel name array
			std::string::size_type end = names.find('\\',begin);
			m_channels[false].push_back(channel(std::string(names,begin,end-begin),rx.isoc[isoc].isoc_channel,isoc));
			begin = ++end;
		}
		m_squ[false][isoc] = rx.isoc[isoc].rx.sequences;
		m_midi_ports[false] += rx.isoc[isoc].rx.sequences.midi;
	}
	m_isocs[false] = rx.isoc_channels;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::fetch_tx
//
///////////////////////////////////////////////////////////////////////////////
//
//	extract isoc TX channel information
//
void device::fetch_tx() throw(tcat::exception::base)
{
	lock lock(this);
	IO<kTCAT_DICE_TX> tx;
	// read tx header
	ohci_read (m_spaces[kSpaceTx].offset, (uint32*)&tx, (sizeof(tx)-sizeof(tx.isoc))>>2);

	TCAT_EXCEPTION_THROW (tx.isoc_channels > kTCAT_DICE_TX, xptn_internal);

	m_channels[true].clear();
	m_midi_ports[true] = 0;
	for (uint32 isoc=0; isoc<tx.isoc_channels; isoc++)
	{
		// read tx isoc channel
		ohci_read (
			m_spaces[kSpaceTx].offset + (uint32)sizeof(tx)-(uint32)sizeof(tx.isoc) + isoc*tx.size32*4, 
			(uint32*)&tx.isoc[isoc],
			std::min<uint32>(tx.size32,(uint32)sizeof(tx.isoc[isoc])>>2));

		TCAT_EXCEPTION_THROW (tx.isoc[isoc].tx.sequences.audio > kTCAT_DICE_MAX_CHANNELS_PER_STREAM, xptn_internal);

		// extract channel names (endian swapped on ppc)
		#ifdef __BIG_ENDIAN__
		pal::swap32 (tx.isoc[isoc].names, sizeof(tx.isoc[isoc].names) >> 2);	// the ppc needs to swap the string...
		#endif
		tx.isoc[isoc].names[sizeof(tx.isoc[isoc].names)-1] = '\0';	// forced null-termination
		std::string names = tx.isoc[isoc].names;
		std::string::size_type begin = 0;
		for (uint32 squ=0; squ<tx.isoc[isoc].tx.sequences.audio; squ++)
		{
			// separate the string and distribute it to the channel name array
			std::string::size_type end = names.find('\\',begin);
			m_channels[true].push_back(channel(std::string(names,begin,end-begin),tx.isoc[isoc].isoc_channel,isoc));
			begin = ++end;
		}
		m_squ[true][isoc] = tx.isoc[isoc].tx.sequences;
		m_midi_ports[true] += tx.isoc[isoc].tx.sequences.midi;
	}
	m_isocs[true] = tx.isoc_channels;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::fetch
//
///////////////////////////////////////////////////////////////////////////////
//
//	update local fetch of device parameters
//
void device::fetch (const DEVICE_INFO& info, DEVICE_NOTIFICATION notification) throw(tcat::exception::base)
{
	lock lock(this);
	TCAT_EXCEPTION_THROW (m_cache[0].info.guid64!=info.guid64, xptn_internal);
	m_cache[0].info = info;
	if (notification & (kDeviceNotificationClockAccept | kDeviceNotificationLock | kDeviceNotificationExStatus))
	{
		TCAT_EXCEPTION_THROW (m_cache[0].info.idx>kTCAT_DICE_DEVICES, xptn_internal);
		#ifndef TCAT_DICE_AVC
		#ifdef WIN32
		m_cache[0].clock.idx = m_cache[0].info.idx;
		m_bus->ioctl (kIoctlDeviceClockGet, &m_cache[0].clock, sizeof m_cache[0].clock, &m_cache[0].clock, sizeof m_cache[0].clock);
		#elif __MACH__
		m_bus->ioctl (kIoctlDeviceClockGet, &m_cache[0].info.idx, sizeof m_cache[0].info.idx, &m_cache[0].clock, sizeof m_cache[0].clock);
		#endif//__MACH__
		#endif//TCAT_DICE_AVC
	}
	if (notification & kDeviceNotificationRx)	fetch_rx();
	if (notification & kDeviceNotificationTx)	fetch_tx();
	*(uint32*)&m_notification |= notification;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::update
//
///////////////////////////////////////////////////////////////////////////////
//
//	notify about changes
//
bool device::update()
{
	bool changed = false;

#ifdef WIN32
	if (TDL_ENABLED( kTDLFlag_Verbose ))
	{
		LONGLONG	perfTicks, perfTicksPerSec;
		QueryPerformanceCounter( (PLARGE_INTEGER)&perfTicks );
		QueryPerformanceFrequency( (PLARGE_INTEGER)&perfTicksPerSec );
		TDL_ENTRY( kTDLFlag_Verbose, "PAL [device 0x%x] update()  (notification: 0x%08x, timestamp: %d)",
				   this, m_notification, ((perfTicks * 1000000) / perfTicksPerSec), 0 );
	}
#endif

	if (strncmp (m_cache[0].info.nickname, m_cache[1].info.nickname, sizeof m_cache[0].info.nickname))
	{
		update_nickname();
		changed = true;
	}
	if (m_cache[0].info.state != m_cache[1].info.state)
	{
		update_state();
		changed = true;
	}
	if (m_cache[0].info.idx != m_cache[1].info.idx)
	{
		update_idx();
		changed = true;
	}
	if (m_cache[0].clock.clock != m_cache[1].clock.clock)
	{
		update_clock();
		changed = true;
	}
	if (m_cache[0].clock.status != m_cache[1].clock.status)
	{
		update_locked();
		changed = true;
	}
	if ((m_cache[0].clock.ex_status ^ m_cache[1].clock.ex_status) & kExStatusLockedMask)
	{
		update_rx_locked();
		changed = true;
	}
	if (*m_cache[0].clock.tx_locked!=*m_cache[1].clock.tx_locked)
	{
		update_tx_locked();
		changed = true;
	}
	if (m_notification & kDeviceNotificationRx)		
	{
		update_rx();
		changed = true;
	}
	if (m_notification & kDeviceNotificationTx)	
	{
		update_tx();
		changed = true;
	}
	if (m_notification & kDeviceNotificationUser)
	{
#ifdef WIN32
		if (TDL_ENABLED( kTDLFlag_Verbose ))
		{
			LONGLONG	perfTicks, perfTicksPerSec;
			QueryPerformanceCounter( (PLARGE_INTEGER)&perfTicks );
			QueryPerformanceFrequency( (PLARGE_INTEGER)&perfTicksPerSec );
			TDL_ENTRY( kTDLFlag_Verbose, "PAL [device 0x%x] user notification  (timestamp: %d)",
					   this, ((perfTicks * 1000000) / perfTicksPerSec), 0, 0 );
		}
#endif
		update_user(DEVICE_NOTIFICATION(m_notification & kDeviceNotificationUser));
		changed = true;
	}
	m_cache[1] = m_cache[0];					// backup cache
	m_notification = kDeviceNotificationNone;	// clear notification flags
	return changed;
}



#ifndef TCAT_DICE_AVC
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::nickname
//
///////////////////////////////////////////////////////////////////////////////
//
void device::nickname(const std::string name) throw(tcat::exception::base)
{
	if (name != m_cache[0].info.nickname)
	{
		ioctl::DEVICE_NICKNAME nickname;
		ioctl::DEVICE_NICKNAME dummy;
		uint32 len32=(uint32)(sizeof(nickname.nickname) >> 2);
		TCAT_EXCEPTION_THROW (m_cache[0].info.idx>kTCAT_DICE_DEVICES, xptn_internal);

		nickname.idx = m_cache[0].info.idx;
		memset (&nickname.nickname, 0, sizeof nickname.nickname);
		name.copy (nickname.nickname, sizeof(nickname.nickname)-1);
//		m_bus->ioctl (kIoctlDeviceNicknameSet, &nickname, sizeof nickname);
		m_bus->ioctl (kIoctlDeviceNicknameSet, &nickname, sizeof nickname, &dummy, sizeof dummy);

		memset (&m_cache[0].info.nickname, 0, sizeof nickname.nickname);
		name.copy (m_cache[0].info.nickname,sizeof(m_cache[0].info.nickname)-1);	// devendra doesn't ring the ready bell: so we have to copy it again. thanks.
		#ifdef __BIG_ENDIAN__
		pal::swap32(nickname.nickname, len32);
		#endif
		ohci_write (m_spaces[kSpaceGlobal].offset + TCAT_OFFSET(DD_GLOBAL, nickname), (uint32*)nickname.nickname, len32);
	}
}
#endif//TCAT_DICE_AVC



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::ohci_read
//
///////////////////////////////////////////////////////////////////////////////
//
void device::ohci_read (uint32 offset, uint32* pData32, uint32 nData32) const throw(tcat::exception::base)
{
	uint32 bytes = nData32 << 2;

#ifdef __MACH__
	lock lock(const_cast<device*>(this));	// it seems the fwlib needs to be protected from multiple clients trying to open it at the same time

	TDL_ENTRY( kTDLFlag_Verbose | kTDLFlag_PAL, "PAL [device %p] ohci_read  (offset 0x%08x, bytes %d)",
			   this, offset, bytes, 0 );

	#ifdef TCAT_DICE_AVC
	fwlib fwlib((IOFireWireLibDeviceRef)m_fw_interface, avclib((IOFireWireAVCLibUnitInterfaceRef)m_avc_interface));
	#else
	fwlib fwlib((IOFireWireLibDeviceRef)m_fw_interface);
	#endif//TCAT_DICE_AVC

	fwlib.read(offset, pData32, bytes);
	#ifdef __LITTLE_ENDIAN__ 
	pal::swap32 (pData32, nData32);
	#endif //__LITTLE_ENDIAN__
#endif //__MACH__

#ifdef WIN32
	ioctl::ASYNC_OHCI<0x1000> async;
	
	TCAT_EXCEPTION_THROW (m_cache[0].info.idx>kTCAT_DICE_DEVICES || nData32>0x1000, xptn_internal);

	async.ohci.header.idx = m_cache[0].info.idx;
	async.ohci.header.offset.hi = kTCAT_DICE_FW_BASE_HI;
	async.ohci.header.offset.lo = kTCAT_DICE_FW_BASE | offset;
	async.ohci.header.bytes = bytes;

	TCAT_EXCEPTION_TRY(m_bus->ioctl (kIoctlDeviceOhciRead, &async.ohci.header, sizeof async.ohci.header, &async.ohci, bytes + sizeof(OHCI_HEADER)), xptn_device, xptn_device);
	pal::swap32 (pData32, async.ohci.data, nData32);
#endif	
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::ohci_write
//
///////////////////////////////////////////////////////////////////////////////
//
void device::ohci_write (uint32 offset, const uint32* pData32, uint32 nData32) const throw(tcat::exception::base)
{
	uint32 bytes = nData32 << 2;

#ifdef __MACH__
	lock lock(const_cast<device*>(this));	// it seems the fwlib needs to be protected from multiple clients trying to open it at the same time

	TDL_ENTRY( kTDLFlag_Verbose | kTDLFlag_PAL, "PAL [device %p] ohci_write  (offset 0x%08x, bytes %d)",
			   this, offset, bytes, 0 );

	#ifdef __LITTLE_ENDIAN__
	TCAT_EXCEPTION_THROW (nData32>0x1000, xptn_internal);
	uint32 data32[0x1000];	// use a swap buffer on i386...
	pal::swap32 (&data32, pData32, nData32);
	pData32 = data32;
	#endif //__LITTLE_ENDIAN__

	#ifdef TCAT_DICE_AVC
	fwlib fwlib((IOFireWireLibDeviceRef)m_fw_interface, avclib((IOFireWireAVCLibUnitInterfaceRef)m_avc_interface));
	#else
	fwlib fwlib((IOFireWireLibDeviceRef)m_fw_interface);
	#endif//TCAT_DICE_AVC
	fwlib.write(offset, pData32, bytes);
#endif //__MACH__

#ifdef WIN32
	ioctl::OHCI<0x1000> ohci;
	
	TCAT_EXCEPTION_THROW (m_cache[0].info.idx>kTCAT_DICE_DEVICES || nData32>0x1000, xptn_internal);

	ohci.header.idx = m_cache[0].info.idx;
	ohci.header.offset.hi = kTCAT_DICE_FW_BASE_HI;
	ohci.header.offset.lo =	kTCAT_DICE_FW_BASE | offset;
	ohci.header.bytes = bytes;

	pal::swap32 (ohci.data, pData32, nData32);
	TCAT_EXCEPTION_TRY(m_bus->ioctl (kIoctlDeviceOhciWrite, &ohci, sizeof (OHCI_HEADER) + bytes), xptn_device, xptn_device);
#endif //WIN32
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::rx_slipped
//
///////////////////////////////////////////////////////////////////////////////
//
//	Returns the extended status information	
//
const EX_STATUS device::rx_slipped() throw(tcat::exception::base)
{
	lock lock(this);
	EX_STATUS ex_status;
	ohci_read (m_spaces[kSpaceGlobal].offset + TCAT_OFFSET(DD_GLOBAL,ex_status), (uint32*) &ex_status, sizeof(ex_status)/4);
	ex_status = EX_STATUS((m_cache[0].clock.ex_status | ex_status) & kExStatusSlipMask);
	*(uint32*)&m_cache[0].clock.ex_status &= kExStatusLockedMask;	// clear slip information in cached ex_status value
	return ex_status;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_info
//
///////////////////////////////////////////////////////////////////////////////
//
bool device::firmware_info (uint32 sector, FL_GET_IMAGE_DESC_RETURN& info) const
{
	try
	{
		// send image ID
		firmware_package (FL_OP_GET_IMAGE_DESC, 5000, &sector);

		// receive image info
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, data.rtn.getImage), (uint32*) &info, sizeof(info)>>2);
			
		#ifdef __BIG_ENDIAN__
		pal::swap32 (&info.name, sizeof(info.name)>>2);
		pal::swap32 (&info.BuildDate, sizeof(info.BuildDate)>>2);
		pal::swap32 (&info.BuildTime, sizeof(info.BuildTime)>>2);
		#endif 
	}
	catch(...)
	{
		return false;
	}
	return true;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_info
//
///////////////////////////////////////////////////////////////////////////////
//
FL_GET_APP_INFO_RETURN device::firmware_info (const std::string filename) throw(tcat::exception::base)
{
	// open firmware binary
	FL_GET_APP_INFO_RETURN info = {0};
	std::ifstream fws(filename.c_str(), std::ios::in | std::ios::binary);
	TCAT_EXCEPTION_THROW (!fws, xptn_resource_file);

	// search for kTCAT_DICE_VERSION_MAGIC_STRING to identify binary
	while (!fws.eof())
	{
		std::string s;
		std::streampos seek = fws.tellg(); 
		fws >> s;
		std::string::size_type pos=s.find(kTCAT_DICE_VERSION_NEW_MAGIC_STRING);
		if (pos!=std::string::npos)
		{
			seek += static_cast<int32>(pos + sizeof(kTCAT_DICE_VERSION_NEW_MAGIC_STRING));
			fws.seekg (seek);
			fws.read((char*)&info, sizeof(info)-sizeof(info.uiBoardSerialNumber));
			#ifdef __BIG_ENDIAN__
			pal::swap32 (&info, sizeof(FL_GET_APP_INFO_RETURN)>>2);
			#endif
			return info;
		}
		pos=s.find(kTCAT_DICE_VERSION_MAGIC_STRING);
		if (pos!=std::string::npos)
		{
			VERSION tmp;
			seek += static_cast<int32>(pos);
			fws.seekg (seek);
			fws.read((char*)&tmp, sizeof tmp);
			#ifdef __BIG_ENDIAN__
			pal::swap32 (&tmp, sizeof(tmp)>>2);
			#endif 
			info.uiBaseSDKVersion = TCAT_VERSION32 (from_string<uint32>("0x"+std::string(tmp.Major)), from_string<uint32>("0x"+std::string(tmp.Minor)), 0,0);
			info.uiVendorID = from_string<uint32>("0x"+std::string(tmp.VendorID));
			info.uiProductID = from_string<uint32>("0x"+std::string(tmp.VProductID));
			info.uiApplicationVersion = TCAT_VERSION32 (from_string<uint32>("0x"+std::string(tmp.VMajor)), from_string<uint32>("0x"+std::string(tmp.VMinor)), 0,0);
			return info;
		}
	}
	TCAT_EXCEPTION_THROW (true, xptn_resource_firmware);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_package
//
///////////////////////////////////////////////////////////////////////////////
//
void device::firmware_package (
	FL_OP_ENUM operation, 
	uint32 timeout, 
	const void* package, 
	uint32 size32,
	uint32 percent,
	uint32 percent_last) const throw(tcat::exception::base)
{
	uint32 progress_timeout = 0;
	uint32 progress = 0;
	struct
	{
		uint32 opcode;
		uint32 error;
		uint32 progress;
	} cmd = {0};
	
	// send package
	if (package)
		ohci_write (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY,data.parameters), (uint32*) package, size32);

	// send opcode to start package processing
	cmd.opcode = operation | FL_OP_EXECUTE;
	ohci_write (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, opcode), &cmd.opcode);

	// device reset can't check results...
	if (operation == FL_OP_RESET_IMAGE) return;
	
	// check for package processed
	uint32 start_time = pal::time();
	while (cmd.opcode & FL_OP_EXECUTE)
	{
		uint32 time = pal::time()-start_time;
		try
		{
			ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, opcode), (uint32*)&cmd, sizeof(cmd)/sizeof(uint32));
		}
		catch (xptn_device_timeout)
		{
			pal::delay(timeout/5);	// The operation failed, the device is very busy, let's only bother it 5 times in the full timeout period.
		}
		if (cmd.progress & FL_PRGS_FLAG)
		{
			// progress indication in firmware loader protocol
			if (cmd.progress != progress)
			{
				start_time = pal::time();
				progress = cmd.progress;
				progress_timeout = ((progress & FL_PRGS_TOUT_MASK) >> FL_PRGS_TOUT_SHFT) * 1000;
				if (percent+percent_last>0)
					firmware_progress(percent_last + (uint32)((percent-percent_last) * 
						float(((progress & FL_PRGS_CURR_MASK)>>FL_PRGS_CURR_SHFT) / float((progress & FL_PRGS_MAX_MASK)>>FL_PRGS_MAX_SHFT))), operation);
			}
			else
            {
				TCAT_EXCEPTION_THROW (time>progress_timeout, xptn_device_firmware_timeout);
            }
		}
		else
		{
			// legacy firmware loader protocol without progress indication
			TCAT_EXCEPTION_THROW (time>timeout, xptn_device_firmware_timeout);
			if (percent+percent_last>0)
				firmware_progress(percent_last + (percent-percent_last) * time/timeout, operation);
		}
		if (cmd.opcode & FL_OP_EXECUTE)
			pal::delay(kTCAT_DICE_FRMW_EXECUTE_RETRY_DELAY);	// a courtesy to the device - give it time and it will be faster...
	}

	// check error processing package
	TCAT_EXCEPTION_THROW (cmd.error, xptn_device_firmware);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_upload
//
///////////////////////////////////////////////////////////////////////////////
//
uint32 device::firmware_upload (const std::string filename, const std::string sector, bool check32) throw(tcat::exception::base)
{
	FL_1394MEMORY fl;
	unsigned char checksum = 0;
	unsigned int checksum32 = 0;
	uint32 filesize;
	void* pNext;

	// retrieve firmware upload structs, states and flags
	ohci_read (kTCAT_DICE_FW_OFFSET_FWL, (uint32*) &fl, (sizeof(fl) - sizeof(fl.data) - sizeof(fl.endOfData)) >> 2);
	TCAT_EXCEPTION_THROW (fl.opcode & FL_OP_EXECUTE, xptn_device_firmware);

	// open firmware binary; & portably get file length
	std::ifstream fws(filename.c_str(), std::ios::in | std::ios::binary);
	TCAT_EXCEPTION_THROW (!fws, xptn_resource_file);
	fws.seekg (0, std::ios_base::end);
	filesize = (uint32)fws.tellg();
	fws.seekg(0, std::ios_base::beg);

	// upload binary
	fl.data.parameters.upload.index = 0;
	do
	{
		uint32 percent = (100*fl.data.parameters.upload.index)/filesize;
		pNext = fws.read((char*)fl.data.parameters.upload.buffer, sizeof fl.data.parameters.upload.buffer);
		std::streamsize size=fws.gcount();
		fl.data.parameters.upload.length = (uint32)size;
		// creating checksum (before endian swapping the buffer!!!)
		if (check32)
		{
			uint32* pChecksum32 = fl.data.parameters.upload.buffer;
			size>>=2;
			while (size--)
				checksum32 += *pChecksum32++;
		}
		else
		{
			unsigned char* pChecksum = (unsigned char*) fl.data.parameters.upload.buffer;
			while (size--)
				checksum = (0xff & (checksum + *pChecksum++)); // we mask the result due to bug in MS compiler, otherwise we could use: checksum += *pChecksum++;
		}
		#ifdef __BIG_ENDIAN__
		pal::swap32 (fl.data.parameters.upload.buffer, sizeof(fl.data.parameters.upload.buffer)>>2);
		#endif 
		firmware_package (
			FL_OP_UPLOAD, 1000,
			&fl.data.parameters.upload, 
			((uint32)sizeof(fl.data.parameters.upload) - (uint32)sizeof(fl.data.parameters.upload.buffer) + fl.data.parameters.upload.length+3) >> 2,
			percent, percent);
		fl.data.parameters.upload.index += fl.data.parameters.upload.length;
	} while (pNext);

	// check upload status
	firmware_package (FL_OP_UPLOAD_STAT, 4000, &filesize, 1, 100, 0);
	// check checksum
	ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY,data.parameters), fl.data.parameters.upload.buffer);
	if (check32)
		TCAT_EXCEPTION_THROW (checksum32 != fl.data.parameters.upload.buffer[0], xptn_device_firmware);
	else
		TCAT_EXCEPTION_THROW (checksum != (fl.data.parameters.upload.buffer[0] & 0xff), xptn_device_firmware);
	firmware_progress(100, FL_OP_UPLOAD_STAT);	// inform operation success
	return filesize;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_upload
//
///////////////////////////////////////////////////////////////////////////////
//
uint32 device::firmware_upload (const unsigned char *pData, uint32 filesize, const std::string sector, bool check32) throw(tcat::exception::base)
{
	FL_1394MEMORY fl;
	unsigned char checksum = 0;
	unsigned int checksum32 = 0;

	// retrieve firmware upload structs, states and flags
	ohci_read (kTCAT_DICE_FW_OFFSET_FWL, (uint32*) &fl, (sizeof(fl) - sizeof(fl.data) - sizeof(fl.endOfData)) >> 2);
	TCAT_EXCEPTION_THROW (fl.opcode & FL_OP_EXECUTE, xptn_device_firmware);

	// upload binary
	fl.data.parameters.upload.index = 0;

	uint32 amountRead = 0;
	while (amountRead < filesize)
	{
		uint32 percent = (100*fl.data.parameters.upload.index)/filesize;
		uint32 readSize = sizeof(fl.data.parameters.upload.buffer);
		if((amountRead + readSize) > filesize)
			readSize = filesize - amountRead;

		amountRead += readSize;
		memmove((char*)fl.data.parameters.upload.buffer, pData + amountRead, readSize);

		fl.data.parameters.upload.length = (uint32)readSize;
		// creating checksum (before endian swapping the buffer!!!)
		if (check32)
		{
			uint32* pChecksum32 = fl.data.parameters.upload.buffer;
			readSize>>=2;
			while (readSize--)
				checksum32 += *pChecksum32++;
		}
		else
		{
			unsigned char* pChecksum = (unsigned char*) fl.data.parameters.upload.buffer;
			while (readSize--)
				checksum = (0xff & (checksum + *pChecksum++)); // we mask the result due to bug in MS compiler, otherwise we could use: checksum += *pChecksum++;
		}
		#ifdef __BIG_ENDIAN__
		pal::swap32 (fl.data.parameters.upload.buffer, sizeof(fl.data.parameters.upload.buffer)>>2);
		#endif 
		firmware_package (
			FL_OP_UPLOAD, 1000,
			&fl.data.parameters.upload, 
			((uint32)sizeof(fl.data.parameters.upload) - (uint32)sizeof(fl.data.parameters.upload.buffer) + fl.data.parameters.upload.length+3) >> 2,
			percent, percent);
		fl.data.parameters.upload.index += fl.data.parameters.upload.length;
	} 

	// check upload status
	firmware_package (FL_OP_UPLOAD_STAT, 4000, &filesize, 1, 100, 0);
	// check checksum
	ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY,data.parameters), fl.data.parameters.upload.buffer);
	if (check32)
		TCAT_EXCEPTION_THROW (checksum32 != fl.data.parameters.upload.buffer[0], xptn_device_firmware);
	else
		TCAT_EXCEPTION_THROW (checksum != (fl.data.parameters.upload.buffer[0] & 0xff), xptn_device_firmware);
	firmware_progress(100, FL_OP_UPLOAD_STAT);	// inform operation success
	return filesize;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_download
//
///////////////////////////////////////////////////////////////////////////////
//
uint32 device::firmware_download (const std::string filename) throw(tcat::exception::base)
{
	FL_1394MEMORY fl;
	memset (&fl,0,sizeof fl);
	firmware_package (FL_OP_GET_FLASH_INFO, 400);	
	// receive flash info
	ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY, data.rtn.FlashInfo), (uint32*) &fl.data.rtn.FlashInfo, sizeof(fl.data.rtn.FlashInfo)>>2);
	
	// open binary output file
	std::ofstream fws(filename.c_str(), std::ios::out | std::ios::binary);
	TCAT_EXCEPTION_THROW (!fws, xptn_resource_file);

	// obtain data
	unsigned int size = fl.data.rtn.FlashInfo.uiNumBlocks * fl.data.rtn.FlashInfo.uiBlockSize;
	unsigned int start = fl.data.rtn.FlashInfo.uiStartAddress;
	unsigned int end = fl.data.rtn.FlashInfo.uiEndAddress;
	int percent = 0;
	memset (&fl,0,sizeof fl);
	while (start<end)
	{
		fl.data.parameters.ReadMemory.uiLen = std::min<uint32>(end-start, sizeof(fl.data.parameters.ReadMemory.ReadBuffer));
		fl.data.parameters.ReadMemory.uiStartAddress = start;
		percent = (size-end+start)*100/size;
		firmware_package (
			FL_OP_READ_MEMORY, 400,
			&fl.data.parameters.ReadMemory, 
			(sizeof(fl.data.parameters.ReadMemory) - sizeof(fl.data.parameters.ReadMemory.ReadBuffer)) >> 2,
			percent, percent);
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY,data.parameters.ReadMemory.ReadBuffer), (uint32*)fl.data.parameters.ReadMemory.ReadBuffer, sizeof(fl.data.parameters.ReadMemory.ReadBuffer) >> 2);
		#ifdef __BIG_ENDIAN__
		pal::swap32 (fl.data.parameters.ReadMemory.ReadBuffer, sizeof(fl.data.parameters.ReadMemory.ReadBuffer)>>2);
		#endif
		fws.write(fl.data.parameters.ReadMemory.ReadBuffer, fl.data.parameters.ReadMemory.uiLen);
		start += fl.data.parameters.ReadMemory.uiLen;
//		std::cout << std::endl << std::hex << fl.data.parameters.ReadMemory.uiStartAddress << ":" << (uint32) fl.data.parameters.ReadMemory.ReadBuffer[0] << std::endl;
	}
	firmware_progress(100, FL_OP_READ_MEMORY);	// inform operation success
	return size;
}

///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_download
//
///////////////////////////////////////////////////////////////////////////////
//
uint32 device::firmware_download (std::string * buf, const std::string sector) throw(tcat::exception::base)
{
	int sector_id = 0;
	unsigned int size = 0;
	tcat::dice::FL_GET_IMAGE_DESC_RETURN info;
	while (firmware_info((tcat::uint32)sector_id, info))
	{
		std::string name = info.name;
		if (0 == name.compare(sector))
		{
			FL_1394MEMORY fl;

			// obtain data
			size = info.length;
			unsigned int start = info.flashBase;
			unsigned int end = start+size;
			int percent = 0;
			memset (&fl,0,sizeof fl);
			while (start<end)
			{
				fl.data.parameters.ReadMemory.uiLen = std::min<uint32>(end-start, sizeof(fl.data.parameters.ReadMemory.ReadBuffer));
				fl.data.parameters.ReadMemory.uiStartAddress = start;
				percent = (size-end+start)*100/size;
				firmware_package (
					FL_OP_READ_MEMORY, 400,
					&fl.data.parameters.ReadMemory, 
					(sizeof(fl.data.parameters.ReadMemory) - sizeof(fl.data.parameters.ReadMemory.ReadBuffer)) >> 2,
					percent, percent);
				ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(FL_1394MEMORY,data.parameters.ReadMemory.ReadBuffer), (uint32*)fl.data.parameters.ReadMemory.ReadBuffer, sizeof(fl.data.parameters.ReadMemory.ReadBuffer) >> 2);
				#ifdef __BIG_ENDIAN__
				pal::swap32 (fl.data.parameters.ReadMemory.ReadBuffer, sizeof(fl.data.parameters.ReadMemory.ReadBuffer)>>2);
				#endif
				// copy to buffer
				buf->append(fl.data.parameters.ReadMemory.ReadBuffer, fl.data.parameters.ReadMemory.uiLen);
				start += fl.data.parameters.ReadMemory.uiLen;
//				std::cout << std::endl << std::hex << fl.data.parameters.ReadMemory.uiStartAddress << ":" << (uint32) fl.data.parameters.ReadMemory.ReadBuffer[0] << std::endl;
			}
			break;
		}
		sector_id++;
	}
	return size;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_delete
//
///////////////////////////////////////////////////////////////////////////////
//
void device::firmware_delete (const std::string sector) throw(tcat::exception::base)
{
	FL_1394MEMORY fl;
	memset (&fl,0,sizeof fl);
	sector.copy (fl.data.parameters.deleteImage.name, sizeof fl.data.parameters.deleteImage.name);
	#ifdef __BIG_ENDIAN__
	pal::swap32 (&fl.data.parameters.deleteImage.name, sizeof(fl.data.parameters.deleteImage.name)>>2);
	#endif
	firmware_package (FL_OP_DELETE_IMAGE, 20000, &fl.data.parameters.deleteImage, sizeof(fl.data.parameters.deleteImage.name)>>2, 100, 0);
	firmware_progress(100, FL_OP_DELETE_IMAGE);	// inform operation success
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_create
//
///////////////////////////////////////////////////////////////////////////////
//
//	caller sets execAddr and entryAddr to NULL to use the default
//
void device::firmware_create (const std::string sector, uint32 filesize, int32 exec_addr, int32 entry_addr) throw(tcat::exception::base)
{	
	FL_1394MEMORY fl;

	// create image; creating the image without deleting it requires at least firmware build 278
	memset (&fl,0,sizeof fl);
	fl.data.parameters.createImage.length = filesize;
	fl.data.parameters.createImage.entryAddr = entry_addr;
	fl.data.parameters.createImage.execAddr = exec_addr;
	sector.copy (fl.data.parameters.createImage.name, sizeof fl.data.parameters.createImage.name);
	#ifdef __BIG_ENDIAN__
	pal::swap32 (&fl.data.parameters.createImage.name, sizeof(fl.data.parameters.createImage.name)>>2);
	#endif
	firmware_package (FL_OP_CREATE_IMAGE, 20000, &fl.data.parameters.createImage, sizeof(fl.data.parameters.createImage), 100, 0);
	firmware_progress(100, FL_OP_CREATE_IMAGE);	// inform operation success
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_reset
//
///////////////////////////////////////////////////////////////////////////////
//
void device::firmware_reset() throw(tcat::exception::base)
{
	TDL_ENTRY( kTDLFlag_Info, "PAL [device 0x%x] firmware_reset()", this, 0, 0, 0 );

	firmware_package(FL_OP_RESET_IMAGE, 5000);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::device::firmware_test
//
///////////////////////////////////////////////////////////////////////////////
//
void device::firmware_test(uint32 test_mode) throw(tcat::exception::base)
{
	dice::FL_TEST_ACTION_PARAM fl;
	fl.cmdID=FL_TEST_CMD_ENTER_TESTMODE;
	fl.lvalue0 = test_mode;
	fl.lvalue1 = 0;
	firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
}



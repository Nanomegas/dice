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
//	see tcat_dice_pal_bus.h
//
#include <IOKit/IOKitLib.h>
#include <IOKit/avc/IOFireWireAVCLib.h>
#include <IOKit/avc/IOFireWireAVCConsts.h>
#include <sys/errno.h>

#include "tcat_dice_pal_system.h"
#include "../interface/tcat_dice_mac.h"
#include "../../interface/tcat_dice_version.h"
#include "tcat_osx.h"
//#include <iostream>
#include <iomanip>
#include <fstream>

//#define LOG_ROOT_CALLBACKS



using namespace tcat;
using namespace dice;



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_arrival_removal
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_callback_notification_arrival_removal (bus_impl* bus, IOReturn)
{
	#ifdef LOG_ROOT_CALLBACKS
	std::cout << "bus_callback_notification_arrival_removal" << std::endl;
	#endif//LOG_ROOT_CALLBACKS
	try
	{
		TCAT_EXCEPTION_THROW (!bus, xptn_internal);
		try
		{
			bus->open();
			bus->m_update = true;
			bus->m_update_devicelist = true;
		}
		catch (tcat::exception::base exception)
		{
			bus->exception (exception);
		}
		catch (...)
		{
			bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
		}
	}
	catch (...)
	{
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_bus_reset
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_callback_notification_bus_reset (bus_impl* bus, IOReturn)
{
	#ifdef LOG_ROOT_CALLBACKS
	std::cout << "bus_callback_notification_bus_reset" << std::endl;
	#endif//LOG_ROOT_CALLBACKS
	try
	{
		TCAT_EXCEPTION_THROW (!bus, xptn_internal);
		try
		{
			bus->ready_reset();
		}
		catch (tcat::exception::base exception)
		{
			bus->exception (exception);
		}
		catch (...)
		{
			bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
		}
	}
	catch (...)
	{
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_interface_ready
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_callback_notification_interface_ready (bus_impl* bus, IOReturn)
{
	#ifdef LOG_ROOT_CALLBACKS
	std::cout << "bus_callback_notification_interface_ready" << std::endl;
	#endif//LOG_ROOT_CALLBACKS
	try
	{
		TCAT_EXCEPTION_THROW (!bus, xptn_internal);
		try
		{
			if (bus->m_update)
				bus->fetch();
			if (bus->m_update_devicelist)
				bus->fetch_devicelist();
			bus->ready_set();
			if (bus->m_update)
			{
				bus->update();
				bus->m_update=false;
			}
			if (bus->m_update_devicelist)
			{
				bus->update_devicelist();
				bus->update_devices();
				bus->m_update_devicelist=false;
			}
		}
		catch (tcat::exception::base exception)
		{
			bus->exception (exception);
		}
		catch (...)
		{
			bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
		}
	}
	catch (...)
	{
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_config
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_callback_notification_config (bus_impl* bus, IOReturn)
{
	#ifdef LOG_ROOT_CALLBACKS
	std::cout << "bus_callback_notification_config" << std::endl;
	#endif//LOG_ROOT_CALLBACKS
	try
	{
		TCAT_EXCEPTION_THROW (!bus, xptn_internal);
		try
		{
			bus->m_update=true;	// mark the bus to be updated - next time the interface is signalled ready
		}
		catch (tcat::exception::base exception)
		{
			bus->exception (exception);
		}
		catch (...)
		{
			bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
		}
	}
	catch (...)
	{
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_device
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_callback_notification_device (bus_impl* bus, IOReturn)
{
	#ifdef LOG_ROOT_CALLBACKS
	std::cout << "bus_callback_notification_device" << std::endl;
	#endif//LOG_ROOT_CALLBACKS
	try
	{
		TCAT_EXCEPTION_THROW (!bus, xptn_internal);
		try
		{
			bus->fetch_devices();
			bus->update_devices();
		}
		catch (tcat::exception::base exception)
		{
			bus->exception (exception);
		}
		catch (...)
		{
			bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
		}
	}
	catch (...)
	{
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::bus
//
///////////////////////////////////////////////////////////////////////////////
//
bus_impl::bus_impl() throw(tcat::exception::base):
	m_timeout(kTCAT_DICE_DRIVER_BUSY_TIMEOUT),
	m_driver_version(0),
	m_hBus(0),
	m_id(0),
	m_ready(true),
	m_update(false),
	m_update_devicelist(false)
{
	m_system.owner(this);
	TCAT_EXCEPTION_THROW (::pthread_mutex_init(&m_ready_mutex, NULL), xptn_resource);
	TCAT_EXCEPTION_THROW (::pthread_cond_init(&m_ready_cond, NULL), xptn_resource);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::~bus
//
///////////////////////////////////////////////////////////////////////////////
//
bus_impl::~bus_impl()
{
	dice::lock lock(this);
	close();
	if (m_id)
	{
		::IOObjectRelease (m_id);
		m_id = 0;
	}
} 



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::fetch
//
///////////////////////////////////////////////////////////////////////////////
//
bool bus_impl::available(uint64 guid64) throw(tcat::exception::base)
{
	bool result=false;
	CFMutableDictionaryRef dict = ::IOServiceMatching("IOFireWireAVCUnit");
	::CFDictionaryAddValue (dict, CFSTR("Unit_Spec_ID"), CFNumber<>(kTCAT_DICE_VENDOR_ID));	// todo: use bus_id
	::CFDictionaryAddValue (dict, CFSTR("Unit_SW_Vers"), CFNumber<>(0x010001));
	::CFDictionaryAddValue (dict, CFSTR("GUID"), CFNumber<kCFNumberSInt64Type,uint64>(guid64));
	io_iterator_t io_iterator;

	// check if this STL bus list entry is an existing bus interface
	if (!::IOServiceGetMatchingServices (kIOMasterPortDefault, dict, &io_iterator))
	{
	    if (::IOIteratorNext(io_iterator))
			result = true;
		::IOObjectRelease(io_iterator);
	}
	return result;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::fetch_devicelist
//
///////////////////////////////////////////////////////////////////////////////
//
bool bus_impl::fetch_devicelist() throw(tcat::exception::base)
{
	bool result = false;
	CFMutableDictionaryRef dict = ::IOServiceMatching("IOFireWireAVCUnit");
	::CFDictionaryAddValue (dict, CFSTR("Unit_Spec_ID"), CFNumber<>(kTCAT_DICE_VENDOR_ID));
	::CFDictionaryAddValue (dict, CFSTR("Unit_SW_Vers"), CFNumber<>(0x010001));

	// add new devices
	io_iterator_t io_iterator;
	// check if this STL bus list entry is an existing bus interface
	if (!::IOServiceGetMatchingServices (kIOMasterPortDefault, dict, &io_iterator))
	{
	    while (io_service_t io_service=::IOIteratorNext(io_iterator))
		{
			DEVICE_INFO device_info = {0};
			unsigned int size = sizeof device_info.guid64;
			::IORegistryEntryGetProperty(io_service,"GUID",(char*)&device_info.guid64,&size);
			::IOObjectRelease(io_service);			
			result |= add(device_info);
		}
		::IOObjectRelease(io_iterator);
	}
	return result;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::fetch
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::fetch() throw(tcat::exception::base)
{
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::update
//
///////////////////////////////////////////////////////////////////////////////
//
bool bus_impl::update()
{
	bool bReturn = false;
#ifndef TCAT_DICE_AVC
	
	if (m_cache[0].config.sync != m_cache[1].config.sync)
	{ 
		update_sync(); 
		bReturn = true; 
	}
	for (int idx=0; idx<kSyncItems; idx++)
		if (std::string(m_cache[0].config.syncname[idx])!=m_cache[1].config.syncname[idx])
		{ 
			update_sync_names(); 
			bReturn = true;
			break;
		}
	if (m_cache[0].config.sync_caps != m_cache[1].config.sync_caps)
	{ 
		update_sync_caps(); 
		bReturn = true; 
	}
	if (m_cache[0].config.clock != m_cache[1].config.clock)
	{ 
		update_clock(); 
		bReturn = true; 
	}
	if (m_cache[0].config.master_guid64 != m_cache[1].config.master_guid64)
	{ 
		update_master(); 
		bReturn = true; 
	}
	if (m_cache[0].op_mode != m_cache[1].op_mode)
	{ 
		update_op_mode(); 
		bReturn = true; 
	}
	if (m_cache[0].clients != m_cache[1].clients)
	{ 
		update_clients(); 
		bReturn = true; 
	}
#ifdef WIN32
	if (m_cache[0].config.io_size != m_cache[1].config.io_size)
	{ 
		update_io_size(); 
		bReturn = true; 
	}
	if (m_cache[0].wdm != m_cache[1].wdm)
	{
		update_wdm();
		bReturn = true; 
	}
	for (int io=0; io<=1; io++)
	{
		// notify abour speaker configuration change
		if (m_cache[0].speaker[io].cfg != m_cache[1].speaker[io].cfg)
		{
			update_speaker(io);
			bReturn = true;
		}
		// notify about channel mapping change for each of the speakers
		uint32 speaker=0;
		for (int chn=0; chn<32; chn++)
		{
			if (((1<<chn) & m_cache[0].speaker[io].cfg) && m_cache[0].speaker[io].map[speaker] != m_cache[1].speaker[io].map[speaker])
			{
				update_speaker_map(io, speaker);
				speaker++;
				bReturn = true;
			}
		}
	}
#endif//WIN32
	m_cache[1] = m_cache[0];
#endif//TCAT_DICE_AVC
	return bReturn;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::open
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::open() throw(tcat::exception::base) 
{
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::close
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::close() throw(tcat::exception::base) 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::create
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::create (system_impl* system, const ID id) throw(tcat::exception::base)
{
	m_system = system;
	m_id = id;
}



//////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::ready_set
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ready_set() throw(tcat::exception::base)
{
	mutex mutex(&m_ready_mutex);
	if (!m_ready)
	{
		m_ready = true;
		TCAT_EXCEPTION_THROW (::pthread_cond_broadcast(&m_ready_cond), xptn_resource);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::ready_reset
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ready_reset() throw(tcat::exception::base)
{
	mutex mutex(&m_ready_mutex);
	m_ready = false;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::ready_wait
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ready_wait() throw(tcat::exception::base)
{	
	timespec timeout;
	timeout.tv_sec = m_timeout/1000;
	timeout.tv_nsec = (long)(m_timeout*1000000 - (uint64)timeout.tv_sec*1000000000);

	mutex mutex(&m_ready_mutex);
	while (!m_ready)
		TCAT_EXCEPTION_THROW (ETIMEDOUT==::pthread_cond_timedwait_relative_np(&m_ready_cond,&m_ready_mutex,&timeout), xptn_device_timeout);
}

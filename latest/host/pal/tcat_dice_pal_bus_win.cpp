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
#include <iostream>
#include <windows.h>

#include "tcat_dice_pal_system.h"
#include "../interface/tcat_dice_win.h"

#define kDriverLog_File_ID		1005


using namespace tcat;
using namespace dice;


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::bus_impl
//
///////////////////////////////////////////////////////////////////////////////
//
bus_impl::bus_impl() throw(tcat::exception::base):
	m_hBus(INVALID_HANDLE_VALUE),
	m_hReady(NULL),
	m_hThread(NULL),
	m_thread_notification_running(true),
	m_timeout(kTCAT_DICE_DRIVER_BUSY_TIMEOUT),
	m_driver_version(0)
{
	m_system.owner(this);
	memset (m_hEvents, 0, sizeof m_hEvents);
//	memset (&m_cache, 0xff, sizeof m_cache);
//	memset (&m_speaker, 0xff, sizeof m_speaker);
} 



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::~bus_impl
//
///////////////////////////////////////////////////////////////////////////////
//
bus_impl::~bus_impl()
{
	close();
	lock lock(this);
	// First stop the thread so that no deleted observer will be called:
	if (m_hThread != NULL)
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] terminating notification thread", this, 0, 0, 0 );
		// terminate notification thread
		// signal any event to get this thread terminated
		m_thread_notification_running = false;
		for (int event=0; event<kBusNotifications; event++)
		{
			if (m_hEvents[event])
				::SetEvent(m_hEvents[event]);
		}
		// wait until observer thread terminated
		::WaitForSingleObject (m_hThread, INFINITE);
		m_hThread = NULL;
	}
	for (int event=0; event<kBusNotifications; event++)
	{
		if (m_hEvents[event])
		{
			// disable and close driver notification
			::CloseHandle(m_hEvents[event]);
			m_hEvents[event] = NULL;
		}
	}
	::CloseHandle(m_hReady);
	m_hReady = NULL;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::open
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::open() throw(tcat::exception::base)
{
	lock lock(this);
	ioctl::VERSION_GET		driverVersion;
	ioctl::BUS_NOTIFICATION notification;
	uint32 bytes;
	BOOL bResult;

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] opening driver connection", this, 0, 0, 0 );

	close();

	// open bus driver handle
	::SetLastError(0);
	uint32 time = pal::time();
	while (INVALID_HANDLE_VALUE==m_hBus)
	{
		m_hBus = ::CreateFile ("\\\\.\\" kTCAT_DICE_DRV_STR, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);

		switch (DWORD error = ::GetLastError())
		{
		case ERROR_SUCCESS:
			TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] CreateFile successful", this, 0, 0, 0 );
			break; 
		case ERROR_BUSY:
		case ERROR_GEN_FAILURE:
			TCAT_EXCEPTION_THROW (pal::time()-time > kTCAT_DICE_DRIVER_OPEN_TIMEOUT, xptn_device);
			TDL_ENTRY( kTDLFlag_Warning | kTDLFlag_PAL, "PAL [bus_impl 0x%x] CreateFile failed (error %d), waiting and retrying...",
					   this, error, 0, 0 );
			progress (100 * (pal::time()-time) / kTCAT_DICE_DRIVER_OPEN_TIMEOUT, kProgressOpen);
			pal::delay(kTCAT_DICE_DRIVER_BUSY_RETRY_DELAY);
			break;
		default:
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] CreateFile failed (error %d)", this, error, 0, 0 );
			TCAT_EXCEPTION_THROW (true, xptn_driver_access);
		}
	}

	// check bus interface compatibility
	memset( &driverVersion, 0, sizeof(driverVersion) );
	TCAT_EXCEPTION_THROW (!::DeviceIoControl(m_hBus, kIoctlVersionGet, NULL, 0, &driverVersion, sizeof(driverVersion), &bytes, NULL), xptn_driver_version);
	m_driver_version = driverVersion.driverVersion32;
	if (driverVersion.ioctlAPIVersion != kIoctlAPIVersion)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] driver API mismatch (PAL 0x%x != driver 0x%x)",
				   this, kIoctlAPIVersion, driverVersion.ioctlAPIVersion, 0 );
		TCAT_EXCEPTION_THROW ((driverVersion.ioctlAPIVersion != kIoctlAPIVersion), xptn_driver_version);
	}

	// connect notification observer events
	for (int event=0; event<kBusNotifications; event++)
		notification.hNotification[event] =	m_hEvents[event];

	::SetLastError(0);
	bResult = ::DeviceIoControl(m_hBus, kIoctlBusNotificationsSet, &notification, sizeof notification, 0, NULL, &bytes, NULL);
	if (!bResult)
	{
		DWORD error = ::GetLastError();
		TCAT_EXCEPTION_THROW (!bResult, xptn_driver_access);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::close
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::close() throw(tcat::exception::base)
{
	if (m_hBus != INVALID_HANDLE_VALUE)
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] closing driver connection", this, 0, 0, 0 );

		// close bus driver
		::CloseHandle (m_hBus);
		m_hBus = INVALID_HANDLE_VALUE;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::pal::ioctl
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ioctl (uint32 ctrl, const void* in, uint32 in_size, void* out, uint32 out_size) throw(tcat::exception::base)
{
	TCAT_EXCEPTION_THROW (m_hBus==INVALID_HANDLE_VALUE, xptn_device);

	TDL_ENTRY( kTDLFlag_Verbose | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl  (ctl_code 0x%x, ioctl 0x%x)", this, ctrl, ((ctrl >> 12) & 0xfff), 0 );

	switch (ctrl)
	{
	case kIoctlBusOutChannelMapSet:
	case kIoctlBusInChannelMapSet:
	case kIoctlBusClockSet:
	case kIoctlBusIoSizeSet:
	case kIoctlBusMasterSet:
	case kIoctlBusSyncSet:
	case kIoctlBusNotificationsSet:
	case kIoctlBusOpModeSet:
	case kIoctlDeviceNicknameSet:
		// reset the ready event for changing driver state IOCTL
		ready_reset();
		break;
	case kIoctlDeviceOhciRead:
	case kIoctlDeviceOhciWrite:
		// OHCI access is independent from interface.
		break;
	default:
		// wait for the driver interface to become ready
		TCAT_EXCEPTION_THROW (::WaitForSingleObject(m_hEvents[kBusNotificationInterfaceReady], m_timeout)==WAIT_TIMEOUT, xptn_device);
		TCAT_EXCEPTION_THROW (m_hBus==INVALID_HANDLE_VALUE, xptn_device);
		break;
	}

	uint32 time=pal::time();
	::SetLastError(0);
	uint32 out_size_returned;
	while (!::DeviceIoControl(m_hBus, ctrl, (void*)in, in_size, out, out_size, &out_size_returned, NULL))
	{
		switch (DWORD error=::GetLastError())
		{
		case ERROR_INVALID_PARAMETER:
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl returned ERROR_INVALID_PARAMETER", this, 0, 0, 0 );
			TCAT_EXCEPTION_THROW (true, xptn_internal);
			break;
		case ERROR_CRC: // ohci address error
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl returned ERROR_CRC", this, 0, 0, 0 );
			TCAT_EXCEPTION_THROW (true, xptn_device_access);
			break;
		case ERROR_GEN_FAILURE: // streaming parameter locked
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl returned ERROR_GEN_FAILURE", this, 0, 0, 0 );
			TCAT_EXCEPTION_THROW (true, xptn_driver_locked);
			break;
		case ERROR_BUSY:
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl returned ERROR_BUSY", this, 0, 0, 0 );
			TCAT_EXCEPTION_THROW (pal::time()-time>m_timeout, xptn_device_timeout);
			progress (100 * (pal::time()-time) / m_timeout, ctrl);
			pal::delay(kTCAT_DICE_DRIVER_BUSY_RETRY_DELAY);
			break;
		default:
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl returned error %d", this, error, 0, 0 );
			switch (ctrl)
			{
			case kIoctlDeviceOhciRead:
			case kIoctlDeviceOhciWrite:
				TCAT_EXCEPTION_THROW (pal::time()-time>kTCAT_DICE_OHCI_RETRY_TIMEOUT, xptn_device_timeout);
				progress (100 * (::pal::time() - time) / kTCAT_DICE_OHCI_RETRY_TIMEOUT, ctrl);
				pal::delay(kTCAT_DICE_OHCI_RETRY_DELAY);
				break;
			default:
				TCAT_EXCEPTION_THROW (true, xptn_driver);
			}
			break;
		}
		TCAT_EXCEPTION_THROW (m_hBus==INVALID_HANDLE_VALUE, xptn_device);
		::SetLastError(0);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::create
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::create (system_impl* system, const ID id) throw(tcat::exception::base)
{
	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] create()", this, 0, 0, 0 );

	m_system = system;
	m_id = id;
	// create driver notification events
	for (int event=0; event<kBusNotifications; event++)
	{
		m_hEvents[event] = ::CreateEvent (NULL, event==kBusNotificationInterfaceReady, event==kBusNotificationInterfaceReady, NULL);
		TCAT_EXCEPTION_THROW (!m_hEvents[event], xptn_resource);
	}

	// create driver ready event
	m_hReady = ::CreateEvent (NULL, TRUE, FALSE, NULL);
	TCAT_EXCEPTION_THROW (!m_hReady, xptn_resource);

	// create notification observer thread
	m_hThread = ::CreateThread (NULL, 0, &static_thread_notification, this, 0, NULL);
	TCAT_EXCEPTION_THROW (!m_hThread, xptn_resource);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::static_thread_notification
//
///////////////////////////////////////////////////////////////////////////////
//
//	this thread waits for notifications from driver and informs the
//	platform independent bus handlers
//
//\cond
DWORD WINAPI bus_impl::static_thread_notification(void* prm)
{
	bus_impl* bus = (dice::bus_impl*)prm;
	try
	{
		TCAT_EXCEPTION_THROW (!bus, xptn_internal);
		HANDLE hEvents[kBusNotifications];
		// remove the kBusNotificationInterfaceReady event from the event list
		for (int event=0; event<kBusNotifications; event++)
			if (event==kBusNotificationInterfaceReady)
				hEvents[event]=bus->m_hEvents[0];
			else
				hEvents[event]=bus->m_hEvents[event];
		for(;;)
		{
			DWORD dwEvent = ::WaitForMultipleObjects (kBusNotifications, hEvents, FALSE, INFINITE);
			// this try/catch blocks ensures that any inner exception will not lead to terminate this thread:
			// only bus removal - indicated by m_thread_notification_running can terminate this thread

			TDL_ENTRY( kTDLFlag_Verbose | kTDLFlag_PAL, "PAL [bus_impl 0x%x] notification  (dwEvent %d)", bus, dwEvent, 0, 0 );

			try
			{
 				lock test(NULL);
				do
				{
					if (!bus->m_thread_notification_running)
					{
						TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] static_thread_notification is terminated", bus, 0, 0, 0 );
						return 0;
					}
					pal::delay(1);
				} while (!bus->test());
				test = bus;
				if (!bus->m_thread_notification_running)
				{
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] static_thread_notification is terminated", bus, 0, 0, 0 );
					return 0;
				}

				switch (dwEvent) 
				{ 
				case WAIT_OBJECT_0 + kBusNotificationConfigChanged:
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] kBusNotificationConfigChanged", bus, 0, 0, 0 );
					bus->fetch();
					bus->ready_set();		// indicate that the bus has been updated
					bus->update();
					break;
				case WAIT_OBJECT_0 + kBusNotificationDeviceNotification:	// notify devices
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] kBusNotificationDeviceNotification", bus, 0, 0, 0 );
					bus->fetch_devices();
					bus->update_devices();
					break;
				case WAIT_OBJECT_0 + kBusNotificationDeviceArrived:
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] kBusNotificationDeviceArrived", bus, 0, 0, 0 );
					bus->open();
					bus->fetch();
					bus->fetch_devicelist();
//					bus->ready_set();		// indicate that the bus has been updated
					bus->update();
					bus->update_devicelist();
					bus->update_devices();
					break;
				case WAIT_OBJECT_0 + kBusNotificationDeviceRemoved:
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] kBusNotificationDeviceRemoved", bus, 0, 0, 0 );
					// open may fail regularily - this try/catch block will ensure the bus objects gets updated even open fails
					try
					{
						bus->open();
						bus->fetch();
						bus->fetch_devicelist();
//						bus->ready_set();		// indicate that the bus has been updated
						bus->update();
						bus->update_devicelist();
					}
					catch(tcat::exception::base exception)
					{
						TDL_ENTRY( kTDLFlag_Warning | kTDLFlag_PAL, "PAL [bus_impl 0x%x] base exception processing kBusNotificationDeviceRemoved", bus, 0, 0, 0 );
						// don't notify it might have been the last device of this bus
					}
					catch(...)
					{
						TDL_ENTRY( kTDLFlag_Warning | kTDLFlag_PAL, "PAL [bus_impl 0x%x] exception processing kBusNotificationDeviceRemoved", bus, 0, 0, 0 );
						bus->exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
					}
					break;
				case WAIT_OBJECT_0 + kBusNotificationBusReset:
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] kBusNotificationBusReset", bus, 0, 0, 0 );
					bus->ready_reset();
					break;
				case WAIT_ABANDONED_0:
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] notification WAIT_ABANDONED_0", bus, 0, 0, 0 );
					bus->exception (xptn_internal(__FILE__,__LINE__,TCAT_FUNCTION));
					break;
				}
			}
			catch(tcat::exception::base exception)
			{
				TDL_ENTRY( kTDLFlag_Warning | kTDLFlag_PAL, "PAL [bus_impl 0x%x] base exception processing notification", bus, 0, 0, 0 );
				bus->exception (exception);
			}
			catch(...)
			{
				TDL_ENTRY( kTDLFlag_Warning | kTDLFlag_PAL, "PAL [bus_impl 0x%x] exception processing notification", bus, 0, 0, 0 );
				bus->exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
			}
		}
	}
	catch(...)
	{
		TDL_ENTRY( kTDLFlag_Warning | kTDLFlag_PAL, "PAL [bus_impl 0x%x] static_thread_notification exception", bus, 0, 0, 0 );

		if (bus)
			bus->exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
	}
	return 0;
}
//\endcond



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::ready_set
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ready_set() throw(tcat::exception::base)
{
	::SetEvent(m_hReady);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::ready_reset
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ready_reset() throw(tcat::exception::base)
{
	::ResetEvent(m_hReady);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::ready_wait
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ready_wait() throw(tcat::exception::base)
{
	TCAT_EXCEPTION_THROW (::WaitForSingleObject(m_hReady, m_timeout)==WAIT_TIMEOUT, xptn_device);
}


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::io_size
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::io_size(uint32 io_size) throw(tcat::exception::base)
{
	if (io_size != m_cache[0].config.io_size)
		ioctl(kIoctlBusIoSizeSet, &io_size, sizeof io_size);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::speaker_map
//
///////////////////////////////////////////////////////////////////////////////
void bus_impl::speaker_map(bool out, tcat::uint32 speaker, tcat::uint32 channel) throw(tcat::exception::base)
{
	if (channel != m_cache[0].speaker[out].map[speaker])
	{
		if (out)
		{
			ioctl::BUS_SET_OUT_CHANNEL_MAP map = {0};
			map.wdmSeqIndex = speaker;
			map.outChannelIndex = channel;
			ioctl(kIoctlBusOutChannelMapSet, &map, sizeof map, &map, sizeof map);
		}
		else
		{
			ioctl::BUS_SET_IN_CHANNEL_MAP map = {0};
			map.wdmSeqIndex = speaker;
			map.inChannelIndex = channel;
			ioctl(kIoctlBusInChannelMapSet, &map, sizeof map, &map, sizeof map);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::dpc_stats
//
///////////////////////////////////////////////////////////////////////////////
void bus_impl::dpc_stats( ioctl::BUS_DPC_STATS& stats ) const throw(tcat::exception::base)
{
	const_cast<bus_impl*>(this)->ioctl( kIoctlBusDpcStatsGet, NULL, 0, &stats, sizeof(ioctl::BUS_DPC_STATS) );
}


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::dpc_stats_clear
//
///////////////////////////////////////////////////////////////////////////////
void bus_impl::dpc_stats_clear() const throw(tcat::exception::base)
{
	const_cast<bus_impl*>(this)->ioctl( kIoctlBusDpcStatsClear, NULL, 0, NULL, 0 );
}

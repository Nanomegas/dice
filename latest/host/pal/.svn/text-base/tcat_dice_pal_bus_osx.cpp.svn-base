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
#include <sys/errno.h>

#include "tcat_dice_pal_system.h"
#include "../interface/tcat_dice_mac.h"
#include "../../interface/tcat_dice_version.h"
#include "TDL_Interface.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sys/stat.h>


#define kDriverLog_File_ID		1007


using namespace tcat;
using namespace dice;

//\cond

bus_impl::notification_lock		bus_impl::s_NotificationRefConsLock;
bus_impl::notification_refcon	bus_impl::s_NotificationRefCons[kMaxBusNoticationRefCons];
uint16							bus_impl::s_NotificationRefConsGenerationCount = 2000;	// start at a non-zero value


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_arrival_removal
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::bus_callback_notification_arrival_removal( void *refcon, IOReturn /*result*/, void **/*args*/, uint32_t /*numArgs*/ )
{
	uint16		refConIndex = ((uint32)((uintptr_t)refcon)) & 0xffff;
	uint16		generationCount = (((uint32)((uintptr_t)refcon)) >> 16) & 0xffff;

	dice::lock notificationLock( &s_NotificationRefConsLock );

	if (    (refConIndex >= kMaxBusNoticationRefCons)
	     || (s_NotificationRefCons[refConIndex].generationCount != generationCount)
	     || (s_NotificationRefCons[refConIndex].busImplPtr == NULL)
	   )
	{
		return;
	}

	bus_impl	*bus = s_NotificationRefCons[refConIndex].busImplPtr;

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] device arrival/removal notification", bus, 0, 0, 0 );

	dice::lock lock( bus );

	try
	{
		bus->open();
		bus->m_update = true;
		bus->m_update_devicelist = true;
	}
	catch (tcat::exception::base exception)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] tcat::exception::base exception", bus, 0, 0, 0 );
		bus->exception (exception);
	}
	catch (...)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] exception", bus, 0, 0, 0 );
		bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_bus_reset
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::bus_callback_notification_bus_reset( void *refcon, IOReturn /*result*/, void **/*args*/, uint32_t /*numArgs*/ )
{
	uint16		refConIndex = ((uint32)((uintptr_t)refcon)) & 0xffff;
	uint16		generationCount = (((uint32)((uintptr_t)refcon)) >> 16) & 0xffff;

	dice::lock notificationLock( &s_NotificationRefConsLock );

	if (    (refConIndex >= kMaxBusNoticationRefCons)
	     || (s_NotificationRefCons[refConIndex].generationCount != generationCount)
	     || (s_NotificationRefCons[refConIndex].busImplPtr == NULL)
	   )
	{
		return;
	}

	bus_impl	*bus = s_NotificationRefCons[refConIndex].busImplPtr;

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] bus reset notification", bus, 0, 0, 0 );

	dice::lock lock( bus );

	try
	{
		bus->ready_reset();
	}
	catch (tcat::exception::base exception)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] tcat::exception::base exception", bus, 0, 0, 0 );
		bus->exception (exception);
	}
	catch (...)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] exception", bus, 0, 0, 0 );
		bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_interface_ready
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::bus_callback_notification_interface_ready( void *refcon, IOReturn /*result*/, void **/*args*/, uint32_t /*numArgs*/ )
{
	uint16		refConIndex = ((uint32)((uintptr_t)refcon)) & 0xffff;
	uint16		generationCount = (((uint32)((uintptr_t)refcon)) >> 16) & 0xffff;

	dice::lock notificationLock( &s_NotificationRefConsLock );

	if (    (refConIndex >= kMaxBusNoticationRefCons)
	     || (s_NotificationRefCons[refConIndex].generationCount != generationCount)
	     || (s_NotificationRefCons[refConIndex].busImplPtr == NULL)
	   )
	{
		return;
	}

	bus_impl	*bus = s_NotificationRefCons[refConIndex].busImplPtr;

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] interface ready notification", bus, 0, 0, 0 );

	dice::lock lock( bus );

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
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] tcat::exception::base exception", bus, 0, 0, 0 );
		bus->exception (exception);
	}
	catch (...)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] exception", bus, 0, 0, 0 );
		bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_config
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::bus_callback_notification_config( void *refcon, IOReturn /*result*/, void **/*args*/, uint32_t /*numArgs*/ )
{
	uint16		refConIndex = ((uint32)((uintptr_t)refcon)) & 0xffff;
	uint16		generationCount = (((uint32)((uintptr_t)refcon)) >> 16) & 0xffff;

	dice::lock notificationLock( &s_NotificationRefConsLock );

	if (    (refConIndex >= kMaxBusNoticationRefCons)
	     || (s_NotificationRefCons[refConIndex].generationCount != generationCount)
	     || (s_NotificationRefCons[refConIndex].busImplPtr == NULL)
	   )
	{
		return;
	}

	bus_impl	*bus = s_NotificationRefCons[refConIndex].busImplPtr;

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] config notification", bus, 0, 0, 0 );

	dice::lock lock( bus );

	try
	{
		bus->m_update=true;	// mark the bus to be updated - next time the interface is signalled ready
	}
	catch (tcat::exception::base exception)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] tcat::exception::base exception", bus, 0, 0, 0 );
		bus->exception (exception);
	}
	catch (...)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] exception", bus, 0, 0, 0 );
		bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_callback_notification_device
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::bus_callback_notification_device( void *refcon, IOReturn /*result*/, void **/*args*/, uint32_t /*numArgs*/ )
{
	uint16		refConIndex = ((uint32)((uintptr_t)refcon)) & 0xffff;
	uint16		generationCount = (((uint32)((uintptr_t)refcon)) >> 16) & 0xffff;

	dice::lock notificationLock( &s_NotificationRefConsLock );

	if (    (refConIndex >= kMaxBusNoticationRefCons)
	     || (s_NotificationRefCons[refConIndex].generationCount != generationCount)
	     || (s_NotificationRefCons[refConIndex].busImplPtr == NULL)
	   )
	{
		return;
	}

	bus_impl	*bus = s_NotificationRefCons[refConIndex].busImplPtr;

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] device notification", bus, 0, 0, 0 );

	dice::lock lock( bus );

	try
	{
		bus->fetch_devices();
		bus->update_devices();
	}
	catch (tcat::exception::base exception)
	{
		TDL_ENTRY( kTDLFlag_Warning | kTDLFlag_PAL, "PAL [bus_impl 0x%x] base exception", bus, 0, 0, 0 );
		bus->exception (exception);
	}
	catch (...)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] exception", bus, 0, 0, 0 );
		bus->exception (xptn_internal(__FILE__, __LINE__, TCAT_FUNCTION));
	}
}

//\endcond


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
	m_NotificationRefConIndex(kInvalidRefConIndex),
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
	dice::lock notificationLock( &s_NotificationRefConsLock );
	dice::lock lock(this);

	close();

	if (m_id)
	{
		::IOObjectRelease (m_id);
		m_id = 0;
	}

	::pthread_cond_destroy(&m_ready_cond);
	::pthread_mutex_destroy(&m_ready_mutex);
} 



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::open
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::open() throw(tcat::exception::base)
{
	dice::lock notificationLock( &s_NotificationRefConsLock );
	dice::lock lock(this);

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] opening driver connection", this, 0, 0, 0 );

	close();

	TCAT_EXCEPTION_THROW (::IOServiceOpen( m_id, mach_task_self(), 0, &m_hBus ), xptn_device);			
	TCAT_EXCEPTION_THROW (::IOConnectSetNotificationPort( m_hBus, 0, ::IONotificationPortGetMachPort((IONotificationPortRef)m_system->m_io_notification ), 0), xptn_resource);

	// retrieve driver version & check bus interface compatibility
	uint64		scalerValues[2] = { 0, 0 };
	uint32		scalerCount = 2;

	m_driver_version = 0;

	kern_return_t	kernReturn;

	kernReturn = ::IOConnectCallScalarMethod( m_hBus, kIoctlVersionGet, NULL, 0, scalerValues, &scalerCount );
	// an older driver (pre 3.5.0) will return kIOReturnBadArgument
	TCAT_EXCEPTION_THROW ((kernReturn == kIOReturnBadArgument), xptn_driver_version);
	TCAT_EXCEPTION_THROW (kernReturn, xptn_driver);
	m_driver_version = static_cast<uint32>(scalerValues[0]);
	if (scalerValues[1] != (uint64)kIoctlAPIVersion)
	{
		TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] driver API mismatch (PAL 0x%x != driver 0x%x)",
				   this, kIoctlAPIVersion, scalerValues[1], 0 );
		TCAT_EXCEPTION_THROW ((scalerValues[1] != (uint64)kIoctlAPIVersion), xptn_driver_version);
	}

	uint16		refConIndex = kInvalidRefConIndex;

	// find the next free slot in the NoficationRefCons table
	for (uint16 i = 0; i < kMaxBusNoticationRefCons; i++)
	{
		if (s_NotificationRefCons[i].busImplPtr == NULL)
		{
			refConIndex = i;
			break;
		}
	}

	if (refConIndex < kMaxBusNoticationRefCons)
	{
		ioctl::BUS_NOTIFICATION notification;
		uint32					refCon;

		m_NotificationRefConIndex = refConIndex;

		refCon = ((s_NotificationRefConsGenerationCount << 16) | m_NotificationRefConIndex);
		s_NotificationRefCons[m_NotificationRefConIndex].busImplPtr = this;
		s_NotificationRefCons[m_NotificationRefConIndex].generationCount = s_NotificationRefConsGenerationCount;
		++s_NotificationRefConsGenerationCount;

		memset( &notification, 0, sizeof(notification) );	// clear notification structure so future additions will be handled gracefully

		notification.pRefCon = (uint64)refCon;
		notification.hNotification[kBusNotificationDeviceArrived] =			(uint64) &bus_callback_notification_arrival_removal;
		notification.hNotification[kBusNotificationDeviceRemoved] =			(uint64) &bus_callback_notification_arrival_removal;
		notification.hNotification[kBusNotificationBusReset] =				(uint64) &bus_callback_notification_bus_reset;
		notification.hNotification[kBusNotificationConfigChanged] =			(uint64) &bus_callback_notification_config;
		notification.hNotification[kBusNotificationInterfaceReady] =		(uint64) &bus_callback_notification_interface_ready;
		notification.hNotification[kBusNotificationDeviceNotification] =	(uint64) &bus_callback_notification_device;
#if __LP64__
		notification.magicVal = (uint64)ioctl::kClient64Bit;
#else
		notification.magicVal = (uint64)ioctl::kClient32Bit;
#endif
		try
		{
			TCAT_EXCEPTION_THROW (::IOConnectCallStructMethod( m_hBus, kIoctlBusNotificationsSet, &notification, sizeof(notification), NULL, NULL ), xptn_driver);
		}
		catch (...)
		{
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] kIoctlBusNotificationsSet exception", this, 0, 0, 0 );
			s_NotificationRefCons[m_NotificationRefConIndex].busImplPtr = NULL;
			m_NotificationRefConIndex = kInvalidRefConIndex;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::close
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::close() throw(tcat::exception::base)
{
	dice::lock notificationLock( &s_NotificationRefConsLock );
	dice::lock lock(this);

	if (m_hBus)
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] closing driver connection", this, 0, 0, 0 );

		if (m_NotificationRefConIndex < kMaxBusNoticationRefCons)
		{
			ioctl::BUS_NOTIFICATION notification;

			s_NotificationRefCons[m_NotificationRefConIndex].busImplPtr = NULL;
			m_NotificationRefConIndex = kInvalidRefConIndex;

			memset( &notification, 0, sizeof notification );	// send a cleared notification structure to unregister
			::IOConnectCallStructMethod( m_hBus, kIoctlBusNotificationsSet, &notification, sizeof(notification), NULL, NULL );
		}

		::IOConnectSetNotificationPort( m_hBus, 0, NULL, 0 );
		::IOServiceClose( m_hBus );
		m_hBus = 0;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::create
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::create (system_impl* system, const ID id) throw(tcat::exception::base)
{
	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] create()", this, 0, 0, 0 );

	m_system = system;
	m_id = id;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::ioctl
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::ioctl (uint32 ctrl, const void* pIn, uint32 nInSize, void* pOut, uint32 nOutSize) throw(tcat::exception::base)
{
	TDL_ENTRY( kTDLFlag_Verbose | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl  (ctrl 0x%x, inSize %d, outSize %d)", this, ctrl, nInSize, nOutSize );

	kern_return_t kern_return;
	switch (ctrl)
	{
		case kIoctlBusClockSet:
		case kIoctlBusSyncSet:
		case kIoctlBusMasterSet:
		case kIoctlBusOpModeSet:
//		case kIoctlDeviceNicknameSet:
//		case kIoctlBusNotificationsSet:
			// reset the ready event for changing driver state IOCTL
			ready_reset();
			break;
		default:
//			TCAT_EXCEPTION_TRY(ready_wait(),xptn_device_timeout,xptn_device_timeout);
			break;
	}

//	uint32 time = pal::time();
	for (;;)
	{
		size_t		outputStructCnt = (size_t)nOutSize;

		kern_return = ::IOConnectCallStructMethod( m_hBus, ctrl, pIn, nInSize, pOut, &outputStructCnt );

		switch (kern_return)
		{
			case kIOReturnSuccess:
			case kIOReturnNotOpen:	// device removed
				return;
			case kIOReturnBadArgument:
				TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl (%d) returned kIOReturnBadArgument", this, ctrl, 0, 0 );
				TCAT_EXCEPTION_THROW (true, xptn_internal_interface);
				return;
			default:
				TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus_impl 0x%x] ioctl (%d) returned error %d", this, ctrl, kern_return, 0 );
/*
				switch (ctrl)
				{
					case kIoctlDeviceOhciRead:
					case kIoctlDeviceOhciWrite:
						TCAT_EXCEPTION_THROW (pal::time()-time>kTCAT_DICE_OHCI_RETRY_TIMEOUT, xptn_device_timeout);
						progress (100 * (::pal::time() - time) / kTCAT_DICE_OHCI_RETRY_TIMEOUT, ctrl);
						pal::delay(kTCAT_DICE_OHCI_RETRY_DELAY);
						break;
					default:
*/
						TCAT_EXCEPTION_THROW (true, xptn_driver);
//				}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
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
	timeout.tv_nsec = (clock_res_t)(m_timeout*1000000 - (uint64)timeout.tv_sec*1000000000);

	mutex mutex(&m_ready_mutex);
	while (!m_ready)
		TCAT_EXCEPTION_THROW (ETIMEDOUT==::pthread_cond_timedwait_relative_np(&m_ready_cond,&m_ready_mutex,&timeout), xptn_device_timeout);
}


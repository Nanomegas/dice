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
//	see dice_pal_system.h
//
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

#include "tcat_dice_pal_system.h"
#include "tcat_osx.h"
#include "../../interface/tcat_dice_defaults.h"
#include <iostream>

using namespace tcat;
using namespace dice;


CFMutableDictionaryRef match()
{
	CFMutableDictionaryRef dict = ::IOServiceMatching("IOFireWireAVCUnit");
	::CFDictionaryAddValue (dict, CFSTR("Unit_Spec_ID"), CFNumber<>(kTCAT_DICE_VENDOR_ID));	// todo: use bus_id
	::CFDictionaryAddValue (dict, CFSTR("Unit_SW_Vers"), CFNumber<>(0x010001));
	return dict; 
}

///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::system_impl
//
///////////////////////////////////////////////////////////////////////////////
//
system_impl::system_impl(): 
	m_io_notification(NULL),
	m_io_runloop(NULL), 
	m_io_runloop_source(NULL),
	m_thread(NULL),
//	m_cond(NULL),
	m_state(kStateStarting)
{
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::system_impl
//
///////////////////////////////////////////////////////////////////////////////
//
system_impl::~system_impl()
{
	try
	{
		// remove device arrival/removal notification
		if (m_state!=kStateTerminating && m_io_runloop && ::CFRunLoopCopyCurrentMode((CFRunLoopRef)m_io_runloop))
		{
			// there is still a running RunLoop: do house keeping
			if (m_io_runloop_source)
				::CFRunLoopRemoveSource((CFRunLoopRef)m_io_runloop, (CFRunLoopSourceRef)m_io_runloop_source, kCFRunLoopDefaultMode);
			if (m_io_notification)
				::IONotificationPortDestroy((IONotificationPortRef)m_io_notification);
		}
		m_io_runloop_source = 0;
		m_io_notification = 0;
		// stop notification & RunLoop/thread
		if (m_thread && m_io_runloop)
		{
			mutex mutex(&m_mutex);
			while (m_state!=kStateTerminating)
			{
				::CFRunLoopStop((CFRunLoopRef)m_io_runloop);
				::pthread_cond_wait(&m_cond,&m_mutex);
			}
		}
		if (m_thread)
		{
			::pthread_mutex_destroy(&m_mutex);
			::pthread_cond_destroy(&m_cond);
			::pthread_detach(m_thread);
			m_thread=NULL;
		}
	}
	catch(...)
	{
		// nothing goes wrong!
	}
}



//\cond

///////////////////////////////////////////////////////////////////////////////
//
//	system_callback_bus_arrival_removal
//
///////////////////////////////////////////////////////////////////////////////
//
void system_callback_bus_arrival_removal (void* refcon, io_iterator_t io_iterator)
{
//	std::cout << "system_callback_bus_arrival_removal" << std::endl;
//	system_impl* system = (system_impl*)refcon;

	dice::reference<system_impl> system = (system_impl*)refcon;
	try
	{
		TCAT_EXCEPTION_THROW (!system, xptn_internal);
		while (::IOIteratorNext(io_iterator));
		if (system->fetch_buslist())
			system->update_buslist();
	}
	catch (tcat::exception::base exception)
	{
		if (system)
			system->exception (exception);
	}
	catch (...)
	{
		if (system)
			system->exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::static_callback_runloop_observer
//
///////////////////////////////////////////////////////////////////////////////
//
void system_impl::static_callback_runloop_observer(void*, int, system_impl*system)
{
	system->callback_runloop_observer();
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::static_thread
//
///////////////////////////////////////////////////////////////////////////////
//
void* system_impl::static_thread(system_impl*system)
{
	return system->thread();
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::callback_runloop_observer
//
///////////////////////////////////////////////////////////////////////////////
//
void system_impl::callback_runloop_observer()
{
	try
	{
		if (fetch_buslist())
			update_buslist();
		m_state = kStateRunning;
	}
	catch (tcat::exception::base xptn)
	{
		m_state = kStateTerminating;
		exception (xptn);
	}
	catch(...)
	{
		m_state = kStateTerminating;
		exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
	}
	try
	{
		mutex mutex(&m_mutex);
		::pthread_cond_broadcast(&m_cond);
	}
	catch (tcat::exception::base base_exception)
	{
		exception (base_exception);
	}
	catch(...)
	{
		exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
	}
}					



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::thread
//
///////////////////////////////////////////////////////////////////////////////
//
void* system_impl::thread()
{
	try
	{
		CFRunLoopObserverContext context ={0};
		CFRunLoopObserverRef observer;
		context.info = this;
		TCAT_EXCEPTION_THROW (!(m_io_runloop = ::CFRunLoopGetCurrent()), xptn_resource);
		TCAT_EXCEPTION_THROW (!(observer = ::CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopEntry, true, 0, (CFRunLoopObserverCallBack)static_callback_runloop_observer, &context)), xptn_resource);
		::CFRunLoopAddObserver ((CFRunLoopRef)m_io_runloop, observer, kCFRunLoopDefaultMode);
		::CFRunLoopAddSource ((CFRunLoopRef)m_io_runloop, (CFRunLoopSourceRef)m_io_runloop_source, kCFRunLoopDefaultMode);
		::CFRunLoopRun();
		::CFRunLoopRemoveObserver((CFRunLoopRef)m_io_runloop, observer, kCFRunLoopDefaultMode);
	}
	catch (tcat::exception::base base_exception)
	{
		exception (base_exception);
	}
	catch(...)
	{
		exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
	}
	try
	{
		mutex mutex(&m_mutex);
		m_state = kStateTerminating;
		::pthread_cond_broadcast(&m_cond);
	}
	catch (tcat::exception::base base_exception)
	{
		exception (base_exception);
	}
	catch(...)
	{
		exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
	}
	return NULL;
}

//\endcond



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::create
//
///////////////////////////////////////////////////////////////////////////////
//
void system_impl::create() throw(tcat::exception::base)
{
	// create and attach bus arival & removal notification
	TCAT_EXCEPTION_THROW (!(m_io_notification = ::IONotificationPortCreate (kIOMasterPortDefault)), xptn_resource);

	io_iterator_t io_iterator;	
	TCAT_EXCEPTION_THROW (::IOServiceAddMatchingNotification(
		(IONotificationPortRef)m_io_notification, 
		kIOFirstPublishNotification,
		match(),
		system_callback_bus_arrival_removal, 
		this, 
		&io_iterator), xptn_resource);
	while (::IOIteratorNext(io_iterator));
	
	TCAT_EXCEPTION_THROW (::IOServiceAddMatchingNotification(
		(IONotificationPortRef)m_io_notification, 
		kIOTerminatedNotification,
		match(),
		system_callback_bus_arrival_removal, 
		this, 
		&io_iterator), xptn_resource);
	while (::IOIteratorNext(io_iterator));

	TCAT_EXCEPTION_THROW (!(m_io_runloop_source = ::IONotificationPortGetRunLoopSource ((IONotificationPortRef)m_io_notification)), xptn_resource);
		
	// create a thread if the current runloop is not running
	CFRunLoopRef runloop = ::CFRunLoopGetCurrent();
	if (::CFRunLoopCopyCurrentMode(runloop))
	{
		m_io_runloop = runloop;
		::CFRunLoopAddSource ((CFRunLoopRef)m_io_runloop, (CFRunLoopSourceRef)m_io_runloop_source, kCFRunLoopDefaultMode);
	}
	else
	{
		TCAT_EXCEPTION_THROW (::pthread_mutex_init (&m_mutex,NULL), xptn_resource);
		TCAT_EXCEPTION_THROW (::pthread_cond_init (&m_cond,NULL), xptn_resource);
		mutex mutex(&m_mutex);
		TCAT_EXCEPTION_THROW (::pthread_create (&m_thread, NULL, (void*(*)(void*))static_thread, this), xptn_resource);
		while (!m_state)
			TCAT_EXCEPTION_THROW (::pthread_cond_wait(&m_cond,&m_mutex), xptn_resource);
		if (m_state!=kStateRunning) 
			throw m_exception;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::available
//
///////////////////////////////////////////////////////////////////////////////
//
bool system_impl::available(const bus_impl::ID bus_id) throw(tcat::exception::base)
{
	bool result=false;
	io_iterator_t io_iterator;

	// check if this STL bus list entry is an existing bus interface
	if (!::IOServiceGetMatchingServices (kIOMasterPortDefault, match(), &io_iterator))
	{
	    if (io_service_t io_service=::IOIteratorNext(io_iterator))
		{
			::IOObjectRelease(io_service);			
			result = true;
		}
		::IOObjectRelease(io_iterator);
	}
	return result;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::update_buslist
//
///////////////////////////////////////////////////////////////////////////////
//
bool system_impl::fetch_buslist() throw(tcat::exception::base)
{
	bool result = false;
	io_iterator_t	io_iterator;
	io_service_t	io_service;

	dice::lock lock(this);
	// check if this STL bus list entry is an existing bus interface
	if (!::IOServiceGetMatchingServices (kIOMasterPortDefault, match(), &io_iterator))
	{
	    if ((io_service=::IOIteratorNext(io_iterator)))
		{
			::IOObjectRelease(io_service);			
			result = create_bus(io_service); // todo: use bus_id
		}
		::IOObjectRelease(io_iterator);
	}
	return result;
}

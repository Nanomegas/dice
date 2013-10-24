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
//	see tcat_dice_pal_system.h
//

#include <windows.h>
#include <setupapi.h>
#include <dbt.h>

#include "tcat_dice_pal_system.h"
#include "TDL_Interface.h"


#define kDriverLog_File_ID		1000


using namespace tcat;
using namespace dice;


static volatile HANDLE static_event_device_arrival_removal = NULL;
static GUID sGUID = kTCAT_DICE_GUID;


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::callback_notification
//
///////////////////////////////////////////////////////////////////////////////
//
//	this callback receives bus arrival & removal messages
//
//\cond
LRESULT CALLBACK callback_notification (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT ret = 0;

	switch (message)
	{
	case WM_DEVICECHANGE:
		switch (wParam)
		{
		case DBT_DEVICEARRIVAL:
		case DBT_DEVICEREMOVECOMPLETE:
			TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [hWnd 0x%x] device arrival/removal notification", hWnd, 0, 0, 0 );
			try
			{
				if (static_event_device_arrival_removal)
					::SetEvent(static_event_device_arrival_removal);
			}
			catch(...)
			{
			}
		break;
		}
	default:
		ret = ::DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return ret;
}
//\endcond



///////////////////////////////////////////////////////////////////////////////
//
//	thread_device_arrival_removal
//
///////////////////////////////////////////////////////////////////////////////
//
//	this thread waits for notifications from driver and informs the
//	platform independent bus handlers
//
//\cond
DWORD WINAPI thread_device_arrival_removal(void* prm)
{
	system_impl* system = (system_impl*)prm;
	try
	{
		TCAT_EXCEPTION_THROW (!system, xptn_internal);
		system->m_thread_device_arrival_removal_running = true;
		for(;;)
		{
			try
			{
				::WaitForSingleObject (static_event_device_arrival_removal, INFINITE);
				if (!system->m_thread_device_arrival_removal_running)
				{
					TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system_impl 0x%x] device_arrival_removal notification thread is terminated",
							   system, 0, 0, 0 );
					return 0;
				}

				TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system_impl 0x%x] static_event_device_arrival_removal event",
						   system, 0, 0, 0 );

				if (system->fetch_buslist())
					system->update_buslist();
			}
			catch(tcat::exception::base exception)
			{
				TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [system_impl 0x%x] tcat::exception::base exception", system, 0, 0, 0 );
				system->exception (exception);
			}
			catch(...)
			{
				TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [system_impl 0x%x] exception", system, 0, 0, 0 );
				system->exception (xptn_internal(__FILE__, __LINE__, __FUNCTION__));
			}
		}
	}
	catch(...)
	{
	}
	return 0;
}
//\endcond



//////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::system_impl
//
///////////////////////////////////////////////////////////////////////////////
//
system_impl::system_impl() : 
	m_hWnd(NULL),
	m_hDevNotify(NULL),
	m_hThread(NULL),
	m_thread_device_arrival_removal_running(false)
{
	TDL_OpenConnection();
	TDL_ENTRY( kTDLFlag_Info, "TDL_OpenConnection  (PAL, version 0x%08X)", kTCAT_DICE_VERSION32, 0, 0, 0 );
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::~system_impl
//
///////////////////////////////////////////////////////////////////////////////
//
system_impl::~system_impl()
{
	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system_impl 0x%x] ~system_impl()", this, 0, 0, 0 );

	if (m_hDevNotify)
	{
		// destroy WM_DEVICECHANGE listener
		::UnregisterDeviceNotification(m_hDevNotify);
		m_hDevNotify = NULL;
	}
	if(m_hWnd)
	{
		// destroy notification window
		::DestroyWindow((HWND)m_hWnd);
		m_hWnd = NULL;
	}
	::UnregisterClass(kTCAT_DICE_DRV_STR,::GetModuleHandle(NULL));

	// First stop the thread so that no deleted observer will be called:
	if (m_hThread)
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system_impl 0x%x] terminating device_arrival_removal notification thread", this, 0, 0, 0 );
		// terminate notification thread
		// signal any event to get this thread terminated
		m_thread_device_arrival_removal_running = false;
		if (static_event_device_arrival_removal)
		{
			::SetEvent(static_event_device_arrival_removal);
			// wait until observer thread terminated
			::WaitForSingleObject (m_hThread, INFINITE);
			m_hThread = NULL;
		}
	}
	::CloseHandle(static_event_device_arrival_removal);
	static_event_device_arrival_removal = NULL;

	TDL_ENTRY( kTDLFlag_Info, "TDL_CloseConnection  (PAL)", 0, 0, 0, 0 );
	TDL_CloseConnection();
}



//////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::create
//
///////////////////////////////////////////////////////////////////////////////
//
void system_impl::create() throw(tcat::exception::base)
{
	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system_impl 0x%x] create()", this, 0, 0, 0 );

	// create a notification event to defer message handling
	static_event_device_arrival_removal = ::CreateEvent (NULL, FALSE, FALSE, NULL);
	TCAT_EXCEPTION_THROW (!static_event_device_arrival_removal, xptn_resource);

	// create notification observer thread
	m_hThread = ::CreateThread (NULL, 0, &thread_device_arrival_removal, this, 0, NULL);
	TCAT_EXCEPTION_THROW (!m_hThread, xptn_resource);

	// create window class to receive WM_DEVICECHANGE notifications
	HINSTANCE hInstance = ::GetModuleHandle(NULL);
	TCAT_EXCEPTION_THROW (!hInstance, xptn_resource);

	_snprintf( m_UniqueClassName, sizeof(m_UniqueClassName), kTCAT_DICE_DRV_STR " %d", GetTickCount() + rand() );

	WNDCLASSEX wndcls;
	memset (&wndcls, 0, sizeof wndcls);
	wndcls.cbSize =			sizeof wndcls; 
	wndcls.lpfnWndProc =	callback_notification;
	wndcls.hInstance =		hInstance;
	wndcls.lpszClassName =	m_UniqueClassName;
	TCAT_EXCEPTION_THROW (!::RegisterClassEx(&wndcls), xptn_resource);

	m_hWnd = ::CreateWindowEx (
		0,
		m_UniqueClassName,
		NULL,
		0,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		HWND_MESSAGE, // message-only window
		NULL,
		hInstance,
		NULL);
	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system_impl 0x%x] m_hWnd = 0x%x", this, m_hWnd, 0, 0 );
	TCAT_EXCEPTION_THROW (!m_hWnd, xptn_resource);
	::SetWindowLongPtr((HWND)m_hWnd, GWLP_USERDATA, (uint32)(uint64)this);		// store this pointer as user data with the window handle

	DEV_BROADCAST_DEVICEINTERFACE db;
	memset (&db, 0, sizeof db);
	db.dbcc_size = sizeof db;
	db.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	db.dbcc_classguid = sGUID;
	m_hDevNotify = ::RegisterDeviceNotification (m_hWnd, &db, DEVICE_NOTIFY_WINDOW_HANDLE);
	TCAT_EXCEPTION_THROW (!m_hDevNotify, xptn_resource);

	if (fetch_buslist())	// trigger bus creation
		update_buslist();
}



///////////////////////////////////////////////////////////////////////////////
//
//	driver
//
///////////////////////////////////////////////////////////////////////////////
//
//	driver stack object class handles creation and destruction of the driver 
//	interfaces
//
//\cond
class driver
{
public:
	driver():
		m_hClass(INVALID_HANDLE_VALUE)
	{
		// create device info handle
		m_hClass = ::SetupDiGetClassDevs (&sGUID, NULL, NULL, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
	}
	~driver()
	{
		if (m_hClass != INVALID_HANDLE_VALUE)
			::SetupDiDestroyDeviceInfoList (m_hClass);
	}
	std::string	path(int enumeration)
	{
		uint32	bytes;
		char detail[0x100];
		SP_DEVICE_INTERFACE_DATA data;
		PSP_INTERFACE_DEVICE_DETAIL_DATA pDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA) &detail;
		
		data.cbSize = sizeof SP_DEVICE_INTERFACE_DATA;
		pDetail->cbSize = sizeof SP_INTERFACE_DEVICE_DETAIL_DATA;
		// check if there are at least one existing bus interface
		if (::SetupDiEnumDeviceInterfaces (m_hClass, NULL, &sGUID, enumeration, &data) &&
			::SetupDiGetInterfaceDeviceDetail (m_hClass, &data, pDetail, sizeof detail, &bytes, NULL))
			return pDetail->DevicePath;
		return "";
	}
//	operator HDEVINFO()	{ return m_hClass; }
	operator bool()		{ return m_hClass!=INVALID_HANDLE_VALUE; }
//	bool operator !()	{ return m_hClass!=INVALID_HANDLE_VALUE; }
protected:
	HDEVINFO m_hClass;
};
//\endcond



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::available
//
///////////////////////////////////////////////////////////////////////////////
//
//	Returns the the bus availabilty in the OS of the given ID
//
bool system_impl::available (const bus_impl::ID bus_id) throw(tcat::exception::base)
{
	std::string path;
	int	enumeration = 0;
	
	driver drv;	// get a driver interface
	if (!drv) return false;
	
	lock lock(this);

	// check if there are at least one existing bus interface
	while ((path=drv.path(enumeration++))!="")
#ifdef MULTI_BUS_SUPPORT
		if (path==bus_id)
#endif
			return true;
	return false;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::system_impl::fetch_buslist
//
///////////////////////////////////////////////////////////////////////////////
//
bool system_impl::fetch_buslist() throw(tcat::exception::base)
{
	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system_impl 0x%x] fetch_buslist()", this, 0, 0, 0 );

	bool bReturn = false;
	std::string path;
	int	enumeration = 0;
	
	// get a driver interface
	driver drv;
	if (!drv) return false;	// TODO: Think of throwing an exception here
	
	lock lock(this);

	// create a bus object for each driver interface found
#ifdef MULTI_BUS_SUPPORT
	while ((path=drv.path(enumeration++))!="")
		bReturn &= create_bus(path);
#else
	if ((path=drv.path(enumeration++))!="")
		bReturn = create_bus(kTCAT_DICE_DRV_STR);
#endif
	return bReturn;
}

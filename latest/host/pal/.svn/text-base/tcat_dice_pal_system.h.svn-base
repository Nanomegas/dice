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

#pragma once

#include "tcat_dice_pal_bus.h"
#include "../TDL/interface/public/TDL_Interface.h"

#define kDriverLog_File_ID		1002


namespace tcat
{
namespace dice
{


/** @mainpage
The TCAT DICE™ platform abstraction layer (tcat::pal) is a cross platform
DICE™ control application library and provides an interface to the DICE™ 
functionality. It hides platform differences from the user and automatically 
deals with OS driver communications and notifications. It instantiates and 
removes bus and device representation objects as a reaction to OS device 
arrival/removal notifications.

The application basically needs to install a (derived) system class. The 
system class will build a tree of busses, devices and audio channels and 
maintains an up-to-date state.
All common DICE™ functionality is addressed by the class tree and the user 
can overwrite the tcat::pal classes to add specific functionality and to react 
to bus and device notification.

<b>The tcat::pal is an STL tree and consists of four parts.</b>
- an application wide system singleton acts as the STL tree root,
- automatically generated bus objects, the STL tree branches,
- automatically generated device objects, the STL branch limbs and
- automatically generated channel objects, the STL limb leaves.

\code
system --> bus +-> device +-> channel
               |          +-> channel
               |          +-> channel
               |					 
               +-> device +-> channel
               |          +-> channel
               |					
               +-> device			
\endcode

<i>The current DICE™ driver implementation is limited to only one bus - 
representing one firewire bus with attached DICE™ devices. 
Anyway the tcat::pal concept is to provide a stable interface definition, 
so it already covers the bus handling.</i>

The system and bus classes are implemented as templates. Their STL list object 
types are defined by these template parameters. 
To host a derived bus class the system class template parameter has to be set
to the derived bus class and to host a derived device class the bus class 
template parameter has to be set to the derived device class.

<b>Instantiating a derived device tree:</b>
\code
typedef system<bus<my_device> >	my_system;
typedef reference<my_system>	my_system_ref;

my_system_ref sys = my_system::static_create<my_system>();
\endcode

<b>Instantiating a derived bus:</b>
\code
typedef system<my_bus<device> >	my_system;
typedef reference<my_system>	my_system_ref;

my_system_ref sys = my_system::static_create<my_system>();
\endcode

<b>Instantiating a derived bus and device tree:</b>
\code
typedef system<my_bus<my_device> >	my_system;
typedef reference<my_system>		my_system_ref;

my_system_ref sys = my_system::static_create<my_system>();
\endcode

<b>Following is a list of all (over-writable) notification handlers:</b>
- general for all classes:
	- <b>create()</b>:				Called before object creation							
	- <b>exception()</b>:			Called on fatal exceptions if the 
									exception cannot be delivered by try/catch 
									(i.e. happened from within another thread)											
	
- specific to system:
    - <b>update_buslist()</b>:		Called before the list of busses needs to be 
									updated	
    - <b>close()</b>:				This must be called before the system's destructor is called
									to guarantee an orderly shutdown of any existing bus objects
									and their notification callbacks.

- specific to bus:
	- <b>mount()</b>:				Notifies about the readily created bus 
									(Windows: first device arrival; OSX: immediately & always) 							
	- <b>unmount()</b>:				Notifies about the removal of the bus
									(Windows: last device removal; OSX: when system is destructed.)
    - <b>update_clock()</b>:		Informs about clock() (sample rate) changes
    - <b>update_clock_caps()</b>:	Informs about clock_caps() (sample rate capabilities) changes
    - <b>update_sync()</b>:			Informs about sync() (clock source) changes
    - <b>update_sync_names()</b>:	Informs about sync_name() changes
    - <b>update_sync_caps()</b>:	Informs about sync_caps() (clock source) changes
    - <b>update_devicelist()</b>:	called before the list of devices needs to 
									be updated	
    - <b>update_devices()</b>:		called before all devices will be notified 
									about device state change
    - <b>update_clock()</b>:		The clock (sampling rate) has changed
    - <b>update_sync()</b>:			The sync source has changed
    - <b>update_io_size()</b>:		The I/O size has changed (Windows only)
    - <b>update_min_io_size()</b>:	The minimum allowed I/O size has changed (Windows only)
    - <b>update_max_io_size()</b>:	The maximum allowed I/O size has changed (Windows only)
    - <b>update_master()</b>:		The clock master has changed
    - <b>update_op_mode()</b>:		The operation mode has changed
    - <b>update_speaker()</b>:		Informs about Out speaker configuration changes (Windows only)
    - <b>update_speaker_map(bool out, uint32 speaker)</b>:		 
									Informs about speaker_map In(false) or Out(true) changes (Windows only)
    - <b>update_clients()</b>:		The number of driver clients has changed. Use this
									notifier to find out if a change of driver clients
									will affect bus property accessibility
    - <b>update_rate_readonly()</b>:		Restrictions on changing the sample rate parameter has changed
    - <b>update_sync_readonly()</b>:		Restrictions on changing the sync source parameter has changed
    - <b>update_master_readonly()</b>:		Restrictions on changing the master device parameter has changed
    - <b>update_io_size_readonly()</b>:		Restrictions on changing the buffer size parameter has changed
    - <b>update_op_mode_readonly()</b>:		Restrictions on changing the operation mode parameter has changed
    - <b>update_nickname_readonly()</b>:	Restrictions on changing the nickname parameter has changed
    - <b>progress()</b>:			Called whenever a time consuming process is
									making any progress

- specific to device:
	- <b>mount()</b>:				Notifies about the arrival of a valid 
									(completely built and updated) device 							
	- <b>unmount()</b>:				Notifies about the removal of the device
									(physically disconnected)						
	- <b>update()</b>:				Called before the device state will be 
									updated
	- <b>update_nickname()</b>:		Informs about a nickname change
	- <b>update_idx()</b>:			Informs about a device index changes 
									(actually another device with a lower 
									GUID has arrived or has been removed)
	- <b>update_state()</b>:		The device state has changed
	- <b>update_clock()</b>:		Notifies about device clock (sample rate) 
									change at a locked device - 
									obtain new sample rate with clock()
	- <b>update_tx_locked()</b>:	Notifies about an receiver lock status change - 
									obtain new state with rx_locked()
	- <b>update_locked()</b>:		Notifies about the locked state of the 
									device has changed - 
									obtain new state with locked()
    - <b>firmware_progess()</b>:	Called during firmware operation and 
									informes about operation progress										
<br><br><br>

<b>Use the reference objects to create and access the device tree:</b>
\code
{
	tcat::dice::system_ref my_system;

	try
	{
		// create a system object - a single instance usually held over the entire application life time
		my_system = tcat::dice::system<bus<device> >::static_create<tcat::dice::system<bus<device> >();
	}
	catch (tcat::exception::base xptn)
	{
		// the system object will only fail to construct if it can't be bound to any notification 
		// that would allow establishing a device tree at a time when driver/device access is available
		// do error handling here, possibly
		return driver_not_installed_error;
	}

	// if no exception was thrown the system object has been successfully constructed

	// get a bus: 
	{ 
		// the enclosing brackets define the lifetime of the bus object - it is to released as soon as possible
		tcat::dice::bus_ref my_bus;
		{
			// the enclosing brackets define the lifetime of the lock and therefor how long the lock is held.
			tcat::dice::lock lock(my_system);			// lock the system
			my_bus = my_system->at(0);					// it is ok to assume that there is only one bus in tcat::pal version 3.x
			if (!my_bus)
				return no_bus_and_no_devices_present;	// leaving the focus at any place in code is ok - objects will be released
		}
		// access the bus
		tcat::dice::CLOCK bus_clock = my_bus->clock();
		
		// the enclosing brackets define the lifetime of the device object - it is to released as soon as possible
		// the bus and device object lifetime does not need to coincide (it does though in this example)
		tcat::dice::device_ref my_device;
		// get a device
		{
			// the enclosing brackets define the lifetime of the lock and therefor how long the lock is held.
			tcat::dice::lock lock(my_bus);		// lock the bus
			my_device = my_bus->at(device_idx);	// please refer to the STL documentation of the various ways of accessing an STL container
			if (!my_device)
				return no_devices_present;		// leaving the focus at any place in code is ok - objects will be released
		}

		// access the device
		std::string device_nickname = my_device->nickname();
		
		// at this point device and bus object are going out of focus - the referenced objects are released
	}

	if (my_system)
	{
		my_system->close();
	}
}
\endcode

<b>Due to the driver design there are platform differences:</b>
The OSX bus object is bound to the availability of a 1394 bus driver. 
The tcat::pal will always construct a valid bus object 
(as long as a version conforming DICE interface  can be found). 
The properties of the bus object are fully accessible although 
it may not contain any devices. 
(The bus object will only be removed if the 1394 driver unloads: 
i.e. the physical 1394 interface is removed.)<br><br>

The windows pal::bus object is constructed when the first device appears 
and is destructed when the last device of that bus disappears. 
No device - no bus - no driver interface.<br><br><br>



<b>Note:</b><br> Link to <b>setupapi.lib</b> and <b>winmm.lib</b> 
to create a windows application.
<br><br><br>


<b>Latest changes / migration tips:</b><br><br>

<b>4.1.2</b><br>

Case 1193 Allowing WDM applications to change the sample rate is now handled automatically in the 
driver. Therefore allow_wdm_samplerate_changes(), allow_wdm_samplerate_changes_readonly(),
update_allow_wdm_samplerate_changes(), and update_allow_wdm_samplerate_changes_readonly() have 
been removed.
<br><br><br>

<b>4.1.0</b><br>

Case 1112 For Windows Vista and later, WDM applications can be allowed or disallowed to change the sample 
rate. Using allow_wdm_samplerate_changes() an application can get/set this preference. Using 
allow_wdm_samplerate_changes_readonly() an application can determine if the setting can currently
be changed (i.e. when no other WDM clients are not active). Using update_allow_wdm_samplerate_changes() 
and update_allow_wdm_samplerate_changes_readonly() the application can receive notifications that these
settings have changed.
<br><br><br>



<b>4.0.0</b><br>

Case 981 Eliminated a race condition in the PAL that could occur on Mac OS X when sequentially 
setting multiple driver parameters (sample rate, clock source, clock master, operation mode) that 
would result in only the first parameter actually changing.

Case 1061 The 4.0.0 driver is no longer limited to power-of-two ASIO buffer sizes. The driver now allows 
setting a buffer size that is a multiple of either 32 or 48 (and must also between the min and 
max) using the PAL API. ASIO hosts are still told that only power-of-two buffer sizes are supported, 
since it is not possible to describe the above limitation in the ASIO protocol.
<br><br><br>



<b>3.5.6</b><br>

Case 553 It is now possible to have multiple instantiations of the PAL library in a Windows application.
<br><br><br>



<b>3.5.3</b><br>

Case 769 PAL based applications could crash when quit.
<br>
- A new system::close() method has been added that unmounts and closes any bus objects to
avoid race conditions during shutdown.  This must now be called before the system destructor
is called.  Since a single instance of the system object is usually held over the entire
application life time, close() should be called just before the application releases its
system object reference and quits.<br>
- The debug build will now assert when run from the debugger to indicate that a system
object has not been closed properly or that a bus object has not been unmounted properly.
<br><br><br>



<b>3.4.1</b><br>

More detailed parameter locking info is available. For applications that
would like to restrict changing parameters based on a separate heuristic
for each parameter. The restriction depends on the number of open ASIO and
or WDM clients depending on the particular parameter.

- bus::update_rate_readonly() : sample rate change restriction notification. <br>
- bus::update_sync_readonly() : sync source change restriction notification. <br>
- bus::update_master_readonly() : master change restriction notification. <br>
- bus::update_io_size_readonly() : buffer size change restriction notification. <br>
- bus::update_op_mode_readonly() : operation mode change restriction notification. <br>
- bus::update_nickname_readonly() : nickname change restriction notification. <br>

- bool bus::rate_readonly() : determine if the sample rate parameter can be changed <br>
- bool bus::sync_readonly() : determine if the sync source parameter can be changed <br>
- bool bus::master_readonly() : determine if the master parameter can be changed <br>
- bool bus::io_size_readonly() : determine if the buffer size parameter can be changed <br>
- bool bus::op_mode_readonly() : determine if the operation mode parameter can be changed <br>
- bool bus::nickname_readonly() : determine if the nickname parameter can be changed <br>
<br><br><br>



<b>3.4.0</b><br>
- (WDM:) bus::update_wdm() : WDM enabled state notification. This method has 
been removed. WDM can no longer be disabled in the driver so this notification 
is not needed.<br>
- (WDM:) bus::wdm(bool enable) : Set WDM enabled state. This method has been 
removed. WDM can no longer be disabled in the driver.<br>
- (WDM:) bus::wdm() : Get WDM enabled state. This method has been removed. 
WDM is always enabled in the driver.<br>
- (WDM:) bus::speaker(bool out, uint32 configuration) : Set speaker configuration. 
This method has been removed. Speaker setup should always be changed from the 
Windows Sounds and Audio Devices control panel.<br>
- (WDM:) bus::speaker(bool out) : Set speaker configuration. 
This method has been removed. Speaker setup should always be changed from the 
Windows Sounds and Audio Devices control panel.<br>
- (WDM:) uint32 bus::speaker(bool out) : Get speaker configuration. 
This method has changed. Speaker setup always applies to out, so the parameter
has been removed.
- (WDM:) bus::update_speaker(bool out) : speaker configuration notification. 
This method has changed to bus::update_speaker(). The speaker setup only applies 
to out, so the parameter has been removed.<br>
<br><br><br>



<b>3.3.0</b><br>
- bus::sync_name(): case 263: fixed incorrectly endian swapped sync names 
on PPC.
- (WDM:) bus::update_clock_change_permit() and bus::clock_change_permit():
case 258: The driver detects now by itself, in which situation a streaming 
property change request can be honored without disturbing improper OS or 
DAW implementations. 
Therefor the sample rate change permission feature has been removed from 
driver and pal.
- case 258: bus::clients() and bus::update_clients() have been added to find 
out if a change of driver clients will affect bus property accessibility.
- (OSX:) The permission of the persistent storage file was user dependent 
and access could therefor fail when generated by another user.
- exception handling: case 255: version mismatch exceptions will now be 
thrown by system::static_create() instead of notifiying by the 
system::exception().
<br><br><br>



<b>3.2.0</b><br>
- The value type of bus_impl::opmode() has been changed from int to OP_MODE.
<i>Please use the enum OP_MODE now. The meaning of values itself have been changed
on the OSX version to have the same meaning as under windows.</i>
- Build version flags have been added to the 32bit build number. The flags indicate
official release vs. intermediate preliminary builds, builds made by the build server 
vs. local developer builds and instrumented/debug vs. deployment builds.
<i>Please make sure you always use TCAT_VERSION_xxx() macros to exctract the correct 
version information from the 32bit value.</i>
- OSX: workaround for reading an invalid master device GUID after device removal
has been implemented.
<br><br><br>




<b>build 128</b><br>
- pal: windows: occasional deadlock on system removal fixed
<i>Please make sure that the notifiers of the super class are always called when
handling notifications.</i>
<br><br><br>



<b>build 122</b><br>
- pal:osx: notification thread hang on initialization failure fix
<br><br><br>



<b>build 118</b><br>
- new binary and firmware versioning support
- new harmonized device::firmware_info() methods
- obsolete device::firmware_vendor() removed
<br><br><br>



<b>build 97</b><br>
- device ordering fixed
- device notifications for stacked devices not correctly distributed fixed
- device update handling (would not always notify about changes) fixed
- bus notification handling fixed
- support for slow firmware loader responses added
- integrated channel handling
- WDM: open fails with more specific exception
- OSX: device arrival update notification fixed
<br><br><br>



<b>build 82</b><br>
<b>split of device::ex_status() in device::rx_locked() and 
device::rx_slipped()</b><br>
The extended status retrieval method had to be split up into rx_locked() and
rx_slipped() due to an inconsistent slip state report. The device only 
notifies about lock status changes. Slip indications are accumulated and 
cleared when the device register is read. 

The change notifier update_ex_status() is replaced by update_tx_locked() 
and now only informs about receiver lock state changes.<br><br>

<b>removed device::notification()</b><br>
device::notification() has been removed. Overwrite device::update_user() to
react on user device notifications.<br><br><br>



<b>build 78</b><br>
<b>1. Data Fetch & Notification Separation</b><br>
The whole device tree will be internally updated before the notifiers are
called. The consequence is that it is now guaranteed that any state of tree
will be valid and can be requested in any of the notifiers.
While the underlying changes are quite drastic the interface has been kept
untouched where possible.<br><br>

Anyway some minor adaptations had to be made:<br>
void system::update_buslist() is only called if there was an actual change to
the bus list. The return value therefor has been eliminated and is now void.

void bus::update_devicelist() is only called if there was an actual change to
the device list. The return value therefor has been eliminated and is now void.

Since no actual data fetching is done in the update notifiers anymore it does
not make sense to throw any kind of exceptions to be handled by the pal. 
Therefor the exception declaration of the notifiers has been removed.<br><br>

<b>2. OSX: Utilizes Apples FWLib for asynchronous 
device::ohci_read() / device::ohci_write()</b><br>
This changes only the underlying FW communication method. No changes to the
pal interface are made.<br><br><br>



//\cond
<b>build 66</b><br>
system::static_create<class system_t>() takes template parameter system_t.
There is no need to overwrite static_create anymore in order to instantiate
a derived system class.<br><br><br>



<b>build 59</b><br>
<b>1. Templatized tcat::pal</b><br>
The tcat::pal implements template parameters to control the item types of its
underlying STL lists.
The bus type of the bus object STL list in the system class, as well as the 
device type of the device object STL list in the bus class can now be defined.
The consequence is that the container classes don't have to be overwritten 
anymore, to create specialized list items and the specialized list items can 
now be accessed cast-free in a type-safe manner.

To achieve this all container are user-transparently separated in interface 
and implementation.
While the interface is the template container all it's functions had to be 
implemented in the header, the non-template platform dependent implementation 
is still hidden in the .cpp files.

<b>What you have to do:</b>
- instead of inheriting from bus you will now inherit form 
<b>bus<device_type></b> and instead from system from <b>system<bus_type></b>. 
<i>(But most likely you don't need to overwrite the system class anymore)</i>.

<b>What you can do now:</b>
- get rid of specialized container classes that are only there to create 
specialized list items,
- remove casts to the list items and use the compilers type check instead of 
using dynamic_cast that are resolved at runtime, but may fail,

<b>A Quick fix:</b><br>
In order to just build with the new tcat::pal - without taking advantage of 
the new functionality you will have to:
- inherit from <b>bus<device></b> instead of bus and
- inherit from <b>system<bus<device> ></b> instead of system.

<b>What else has changed:</b>
- system::new_bus() returns the specialized bus and probably don't need 
to be overwritten (anymore),
- <b>bus::new_device()</b> returns the specialized device and only needs to be 
overwritten in case you want to take advantage of the factory functionality.
- <b>bus::create()</b> takes system_impl instead of system as back pointer 
parameter
- <b>device::create()</b> takes bus_impl instead of bus as back pointer 
parameter
- the protected <b>m_bus</b> pointer of the device class has changed its type 
from <b>bus*</b> to the <b>reference<bus_impl></b>,
- the protected <b>m_system</b> pointer of the bus class has changed its type 
from <b>system*</b> to the <b>reference<system_impl></b>.

following members are defined private now and cannot be accessed anymore.
- all internal handles and threads of system_impl
- all internal handles and threads of bus_impl
<br><br><br>



<b>2. Recursive referencing</b><br>
The tcat::pal has now implemented full recursive referencing ensuring that all 
objects are available as long as they are referenced.<br>
following members are defined private now and cannot be accessed anymore.
- <b>base::addref()</b>, <b>base::release()</b> - this was mistakenly possible, 
now the factory will ensure that objects that don't create are released.
- <b>base::lock()</b>, <b>base::unlock()</b> - use the lock stack class
<br><br><br>



<b>3. Notifications</b><br>
The tcat::pal has a slightly different device creation process.
It will now finish the virtual creation process of the derived busses and 
devices before calling the virtual update() function.
This ensures that the first update() is called when all derived bus/device 
functionality is created.

Two new notifier functions do support the device arrival/removal process:
- bus / device::mount() - is called after the bus / device is created, updated and 
mounted into the device tree - ready to use.
- bus / device::unmount() - is called when the bus / device is physically 
disconnected and it's representation is removed from the device tree.<br>
<i>It is now also ensured that the device list order of the bus will always be 
the same for a certain device collection.</i>

There are new specialized notification handlers: (See the above list of all 
(over-writable) notification handlers).
<br><br><br>
//\endcond



To watch all tcat::pal notifications handlers you may run the <b>dice cmd line 
tool</b> (you can run it in parallel to your application - to debug 
notification handling) with the tracing  (+t) and/or logging (+l) enabled:
\code
dice +t +l sg
\endcode
*/
///////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
/// 
///	\class tcat::dice::system_impl
///
///////////////////////////////////////////////////////////////////////////////
///
///	\brief Platform dependent system implementation
///
/// \throws
///	Besides internal exceptions a dice::xptn_driver_version will be thrown at
/// construction time if the installed driver version is not compatible.
///
/// The application wide singleton tcat::dice::system instanciates 
///	tcat::dice::busses as reactions to OS device arrival/removal notifications.
///	As far as a driver is installed the tcat::dice::system will create.
///
///////////////////////////////////////////////////////////////////////////////
///
class system_impl: public base
{
	typedef base	super;
	friend	class	bus_impl;
	friend	class	device;
	friend	class	reference<system_impl>;
public:
	virtual	bool				fetch_buslist()									throw(tcat::exception::base);	///< Refreshes the the system object state fetch
	virtual	void				update_buslist()											{}					///< Notifies the system object of bus(driver) arrival/removal
	virtual void				exception(const tcat::exception::base& exception)			{}					///< Exception handler to overwrite 
protected:
								system_impl();																	///< First stage system constructor
	virtual						~system_impl();																	///< System destructor
	virtual	void				create			()								throw(tcat::exception::base);	///< Second stage critical object creation
	virtual bool				create_bus		(const bus_impl::ID bus_id) = 0;								///< Creates a bus object
	virtual bool				available		(const bus_impl::ID bus_id)		throw(tcat::exception::base);	///< Return the availabilty in the OS of the the bus of the given ID

private:
#ifdef WIN32
			handle				m_hWnd;					///< (WDM:) Window handle for receiving OS messages WM_DEVICECHANGE
			handle				m_hDevNotify;			///< (WDM:) Notification handle for receiving OS messages WM_DEVICECHANGE
			handle				m_hThread;				///< (WDM:) Notification thread for receiving OS messages WM_DEVICECHANGE
			char				m_UniqueClassName[64];
#endif

public:
#ifdef WIN32
//\cond
			bool				m_thread_device_arrival_removal_running;
//\endcond
#endif

#ifdef __MACH__
//\cond
	virtual	void*				thread();				///< (OSX:) notification thread
	virtual	void				callback_runloop_observer();
	static	void*				static_thread(system_impl*system);
	static	void				static_callback_runloop_observer(void*, int, system_impl*system);
			void*				m_io_notification;		///< (OSX:) Notification port for device removal/arrival (IONotificationPortRef)
			void*				m_io_runloop;			///< (OSX:) The run loop (CFRunLoopRef)
			void*				m_io_runloop_source;	///< (OSX:) Run loop source for connecting notification port & callback (CFRunLoopSourceRef)
			pthread_t			m_thread;				///< (OSX:) Notification handler thread
			pthread_mutex_t		m_mutex;				///< (OSX:) Notification handler signal mechanism
			pthread_cond_t		m_cond;					///< (OSX:) Notification handler signal mechanism
			enum
			{
				kStateStarting,
				kStateRunning,
				kStateTerminating
			}					m_state;
	tcat::exception::base		m_exception;			///< (OSX:) forward exceptions from RunLoop thread to system::static_create
//\endcond
#endif
};




///////////////////////////////////////////////////////////////////////////////
/// 
///	\class tcat::dice::system
///
///////////////////////////////////////////////////////////////////////////////
///
///	\brief Application wide singleton acts as the STL device tree root
///
/// The application wide singleton tcat::dice::system instanciates 
///	tcat::dice::busses as reactions to OS device arrival/removal notifications.
///	As far as a driver is installed the tcat::dice::system will create.
///
template <class bus_t=bus<device> > class system: public system_impl, public std::deque<reference<bus_t> >
{
	typedef typename dice::reference<bus_t>	bus_ref;
	typedef typename std::deque<bus_ref>	super;
	typedef	system<bus_t> self;
public:
	typedef	typename super::iterator		iterator;															///< bus object iterator
	typedef	typename super::const_iterator	const_iterator;														///< bus object const iterator
	
	template <class system_t>
	static	dice::reference<system_t>	static_create()	throw(tcat::exception::base)							///< Static object creation
	{
		dice::reference<system_t> system = new system_t;
		try
		{
			system->create();
		}
		catch (...)
		{
			if (system)
			{
				system->close();
			}
			throw;
		}
		return system;
	}

	virtual			~system()
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system 0x%x] ~system()", this, 0, 0, 0 );

		if (m_open)
		{
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [system 0x%x] close() should have been called before ~system()", this, 0, 0, 0 );
			PAL_DEBUG_BREAK		// the close() method should have been called before trying to destroy the object
		}
		super::clear();
	}						/// system destructor

	virtual void close()																						///< This must be called before the destructor is called.  It unmounts and closes any bus objects to avoid race conditions during shutdown.
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system 0x%x] close()", this, 0, 0, 0 );

		// unmount and close the bus objects
		for (iterator i=super::begin(); i!=super::end(); i++)
		{
			(*i)->unmount();
			(*i)->close();
		}
		m_open = false;
	}

	virtual	bus_t*	new_bus()								const		{ return new bus_t(); }					///< This creates a bus object. This is for future use only

	virtual bool	create_bus	(const bus_impl::ID bus_id)				
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system 0x%x] create_bus()", this, 0, 0, 0 );

		if (!find(bus_id))	// check if bus interface is already in STL bus list
		{
			bus_ref bus(new_bus(),this);
			if (bus)
			{
				lock lock(bus);
				bus->create(this,bus_id);
				bus->open();
				bus->fetch();
				bus->fetch_devicelist();
				this->push_front(bus);
				bus->ready_set();
				// bus is created: now notify all subscribers!
				bus->mount();
				bus->update();
				bus->update_devicelist();
				return true;
			}
		}
		return false;
	}

	virtual bool	fetch_buslist()	throw(tcat::exception::base)												///< Fetches the bus list into the cached copy of the hardware representation
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system 0x%x] fetch_buslist()", this, 0, 0, 0 );

		bool bReturn = false;
		lock lock(this);
		// remove not existing busses
		for (iterator i=super::begin(); i!=super::end();)
			if (available((*i)->id()))	// check if this STL bus list entry is an existing bus interface
				i++; // only increment if item is not to be removed 
			else
			{
				(*i)->unmount();
				i = this->erase(i);
				bReturn = true;
			}
		return system_impl::fetch_buslist() || bReturn;
	}

	virtual	void	update_buslist()																			///< Notifies the system object of bus(driver) arrival/removal
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system 0x%x] update_buslist()", this, 0, 0, 0 );

		lock lock(this);
		// devices are created; but not notified yet
		for (iterator i=super::begin(); i!=super::end(); i++)
			(*i)->update_devices();		
	}

	bus_ref	find(const bus_impl::ID bus_id)	const																///< Finds a bus identified by the bus_id in the STL bus list
	{
		for (const_iterator bus=self::begin(); bus!=self::end(); bus++)
		{
			if ((*bus)->id()==bus_id)
				return *bus;
		}
		return NULL;
	}

protected:
	system():
		m_open(true)
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [system 0x%x] system()", this, 0, 0, 0 );
	}																											/// system constructor -- should not be called directly, instead use static_create()

private:
	bool	m_open;
};


typedef dice::reference<system<bus<device> > > system_ref;	///< Standard system reference



} // namespace dice
} // namespace tcat


#undef kDriverLog_File_ID

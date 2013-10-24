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
//	dice pal bus
//
#pragma once

#include "tcat_dice_pal_device.h"
#include "../TDL/interface/public/TDL_Interface.h"

#define kDriverLog_File_ID		1003


namespace tcat
{
namespace dice
{


#define kTCAT_DICE_DRIVER_BUSY_TIMEOUT			 5000
#define kTCAT_DICE_DRIVER_OPEN_TIMEOUT			10000
#define kTCAT_DICE_DRIVER_BUSY_RETRY_DELAY		  100
#define kTCAT_DICE_FRMW_EXECUTE_RETRY_DELAY		   10

#define kTCAT_DICE_SPEAKER_MAPS					16



class system_impl;



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl
//
///////////////////////////////////////////////////////////////////////////////
///
/// platform dependent bus implementation
///
class bus_impl: public base
{
	friend class device;
	friend class reference<bus_impl>;
	typedef base super;
	
public:
	class channel: public std::string	/// bus channel containing name and ac3 information for one audio channel
	{
	public:
				channel(std::string name, bool ac3_enabled, bool ac3_capable): std::string(name), m_ac3_enabled(ac3_enabled), m_ac3_capable(ac3_capable) {}	///< constructor
		bool	ac3_enabled()	const	{ return m_ac3_enabled; }	///< Return the isoc channel
		bool	ac3_capable()	const	{ return m_ac3_capable; }	///< Return AVS sequence number in the isoc channel
	private:
		//\cond
		bool	m_ac3_enabled;
		bool	m_ac3_capable;
		//\endcond
	};
	typedef	std::deque<channel>	channel_list;		///< Internal channel representation class
#ifdef WIN32
	typedef std::string ID;																												///< (WDM:) bus ID
	#define kTCAT_DICE_BUS_ID_NONE "" 
#endif//WIN32
#ifdef __MACH__
	typedef unsigned int ID;																											///< (OSX:) bus ID
	#define kTCAT_DICE_BUS_ID_NONE 0 
#endif//__MACH__
	
	virtual			void				create		(system_impl* system, const ID id)		throw(tcat::exception::base);				///< Second stage critical bus creation 
	virtual			void				open()												throw(tcat::exception::base);				///< Opens the DICE(tm) driver
					void				close()												throw(tcat::exception::base);				///< Closes the DICE(tm) driver
	
	virtual			uint32				driver_version()							const		{ return m_driver_version; }			///< Returns the 32bit integer driver version 
//	virtual	const	std::string			name()										const		{ return std::string(""); } 
	virtual			bool				add			(DEVICE_INFO& device_info)				throw(tcat::exception::base)=0;				///< Sorts the device into the STL list
	virtual			bool				available	(uint64 guid64)							throw(tcat::exception::base);				///< Return true if the guid is a connected DICE(tm) device

	virtual			void				ready_wait()										throw(tcat::exception::base);				///< Waits for the bus-is-ready semaphore
	virtual			void				ready_set()											throw(tcat::exception::base);				///< Signals the bus-is-ready semaphore
	virtual			void				ready_reset()										throw(tcat::exception::base);				///< Resets the bus-is-ready semaphore
	virtual	const	ID&					id()										const		{ return m_id; }						///< Returns the bus ID
	virtual			uint32				clients()									const		{ return m_cache[0].clients; }			///< Returns the total number of audio clients - used to know about streaming parameter change permission 
	virtual			bool				rate_readonly()								const		{ return m_cache[0].rate_readonly; }		///< is the sample rate parameter currently changeable
	virtual			bool				sync_readonly()								const		{ return m_cache[0].sync_readonly; }		///< is the sync source parameter currently changeable
	virtual			bool				master_readonly()							const		{ return m_cache[0].master_readonly; }		///< is the master parameter currently changeable
	virtual			bool				op_mode_readonly()							const		{ return m_cache[0].op_mode_readonly; }		///< is the operation mode parameter currently changeable
	virtual			bool				io_size_readonly()							const		{ return m_cache[0].io_size_readonly; }		///< (Windows only) is the buffer size parameter currently changeable
	virtual			bool				nickname_readonly()							const		{ return m_cache[0].nickname_readonly; }	///< is the nickname parameter currently changeable
#ifndef TCAT_DICE_AVC
	virtual	const	CLOCK&				clock()										const		{ return m_cache[0].config.clock; }		///< Returns the current CLOCK enum (sample rate)
	virtual			void				clock		(CLOCK clock)							throw(tcat::exception::base);				///< Sets the clock (sample rate)
	virtual	const	std::string			clock_name	(CLOCK clock)					const;
																																		///< Transforms the CLOCK enum into a human readable std::string
	virtual const	CLOCK_CAPS&			clock_caps()								const		{ return m_cache[0].config.clock_caps; }///< Returns the clock capabilities of the master device
	virtual	const	SYNC&				sync()										const		{ return m_cache[0].config.sync; }		///< Returns the sync source of the master device
	virtual			void				sync		(SYNC sync)								throw(tcat::exception::base);				///< Sets sync source of the master device
	virtual	const	std::string			sync_name	(SYNC sync)						const		{ return std::string(m_cache[0].config.syncname[sync]); }
																																		///< Transforms the SYNC source enum into a human readable std::string provided by the master device
	virtual const	SYNC_CAPS&			sync_caps()									const		{ return m_cache[0].config.sync_caps; }	///< Returns the possible sync sources of the master device
	virtual			void				ioctl		(uint32 ctrl, const void* in, uint32 in_size = sizeof(uint32), void* out = NULL, uint32 out_size = 0)	
																							throw(tcat::exception::base);				///< IOCTL Driver communication interface
#ifdef WIN32
	virtual	const	uint32				io_size()									const		{ return m_cache[0].config.io_size; }	///< (Windows only) Returns the I/O buffer size
	virtual			void				io_size		(uint32 io_size)						throw(tcat::exception::base);				///< (Windows only) Sets the I/O buffer size
	virtual	const	uint32				min_io_size()								const		{ return m_cache[0].min_io_size; }		///< (Windows only) Returns the minimum I/O buffer size
	virtual	const	uint32				max_io_size()								const		{ return m_cache[0].max_io_size; }		///< (Windows only) Returns the maximum I/O buffer size
	virtual			uint32				speaker()									const		{ return m_cache[0].speaker[1].cfg; }			///< (Windows only) Returns In(false) or Out(true) speaker config
	virtual			int					speaker_map	(bool out, uint32 speaker)		const		{ return m_cache[0].speaker[out].map[speaker];}///< (Windows only) Returns In(false) or Out(true) channel for speaker
	virtual			void				speaker_map	(bool out, uint32 speaker, uint32 channel)	throw(tcat::exception::base);			///< (Windows only) Sets the In(false) or Out(true) channel for speaker
					void				dpc_stats( ioctl::BUS_DPC_STATS& stats )	const	throw(tcat::exception::base);				///< Returns driver DPC latency statistics
					void				dpc_stats_clear()							const	throw(tcat::exception::base);				///< Clears driver DPC latency statistics
#endif//WIN32
	virtual	const	OP_MODE				op_mode()									const		{ return m_cache[0].op_mode; }			///< Returns the driver op mode (additional kernel buffer safe mode)
	virtual			void				op_mode		(OP_MODE op_mode)						throw(tcat::exception::base);				///< Sets the driver op mode (additional kernel buffer safe mode)
	virtual	const	uint64&				master()									const		{ return m_cache[0].config.master_guid64; }	///< Returns the clock master device
	virtual			void				master		(uint64 guid64)							throw(tcat::exception::base);				///< Sets the clock master device
	virtual			void				add_reported_audio_latency(uint32 inputSamples, uint32 outputSamples)	throw(tcat::exception::base);	///< Allows for increasing the driver's reported audio latency
					ioctl::BUS_ERROR	errors()									const	throw(tcat::exception::base);				///< Returns driver streaming error counters
					void				error_clear()								const	throw(tcat::exception::base);				///< Clears driver streaming error counters
#endif//TCAT_DICE_AVC

	virtual			void				exception(const tcat::exception::base& exception)		{}										///< Exception occurred at subscribed item
	
	virtual			bool				update();																						///< Notifies about bus configuration changes
	virtual			void				update_devices()										{}										///< Sends device notifications to the list of devices  
	virtual			void				update_devicelist()										{}										///< Notifies about device arrival/removal and updates the internal STL list
	virtual			void				fetch()												throw(tcat::exception::base);				///< Caches the bus configuration
	virtual			void				fetch_devices()										throw(tcat::exception::base) = 0;			///< Sends device notifications to the list of devices  
	virtual			bool				fetch_devicelist()									throw(tcat::exception::base);				///< Sends device notifications to the list of devices  
	virtual			void				timeout	(uint32 timeout=kTCAT_DICE_DRIVER_BUSY_TIMEOUT)	{ m_timeout=timeout; }					///< Sets the timeout in milliseconds for driver operation
	virtual			channel_list&		channels	(bool out)									{ return m_cache[0].channels[out]; }	///< Returns In(false) or Out(true) channels
	virtual			void				progress	(uint32 percent, uint32 ctrl)	const		{}										///< Called whenever a time consuming process is making any progress

protected:
	virtual			void				update_clients()										{}										///< Informs about clients() (number of driver clients) changes - used to know about streaming parameter change permission 
	virtual			void				update_rate_readonly()									{}										///< Notification that the sample rate restriction has changed
	virtual			void				update_sync_readonly()									{}										///< Notification that the sync source restriction has changed
	virtual			void				update_master_readonly()								{}										///< Notification that the master restriction has changed
	virtual			void				update_op_mode_readonly()								{}										///< Notification that the operation mode restriction has changed
	virtual			void				update_io_size_readonly()								{}										///< (Windows only) Notification that the buffer size restriction has changed
	virtual			void				update_nickname_readonly()								{}										///< Notification that the nickname restriction has changed
#ifndef TCAT_DICE_AVC
	virtual	DEVICE_NOTIFICATIONS		device_notifications()								throw(tcat::exception::base);				///< Obtains the notification flags for all devices  
	virtual			void				update_clock()											{}										///< Informs about clock() (sample rate) changes
	virtual			void				update_clock_caps()										{}										///< Informs about clock_caps() (sample rate capabilities) changes
	virtual			void				update_sync()											{}										///< Informs about sync() (clock source) changes
	virtual			void				update_sync_names()										{}										///< Informs about sync_name() changes
	virtual			void				update_sync_caps()										{}										///< Informs about sync_caps() (clock source) changes
#ifdef WIN32
	virtual			void				update_io_size()										{}										///< (Windows only) Informs about io_size() I/O buffer size changes
	virtual			void				update_min_io_size()									{}										///< (Windows only) Informs about min_io_size() minimum allowed I/O buffer size changes
	virtual			void				update_max_io_size()									{}										///< (Windows only) Informs about max_io_size() maximum allowed I/O buffer size changes
	virtual			void				update_speaker()										{}										///< (Windows only) Informs about speaker In(false) or Out(true) setup changes
	virtual 		void				update_speaker_map(bool out, uint32 speaker)			{}										///< (Windows only) Informs about speaker_map In(false) or Out(true) changes
#endif//WIN32
	virtual			void				update_master()											{}										///< Informs about a new clock master() device
	virtual			void				update_op_mode()										{}										///< Informs about op_mode() changes
#endif//TCAT_DICE_AVC
										bus_impl()											throw(tcat::exception::base);				///< First stage bus constructor
	virtual								~bus_impl();																					///< Destructor

private:

#ifdef __MACH__
//\cond
	static			void				bus_callback_notification_arrival_removal( void *refcon, int result, void **args, uint32_t numArgs );
	static			void				bus_callback_notification_bus_reset( void *refcon, int result, void **args, uint32_t numArgs );
	static			void				bus_callback_notification_interface_ready( void *refcon, int result, void **args, uint32_t numArgs );
	static			void				bus_callback_notification_config( void *refcon, int result, void **args, uint32_t numArgs );
	static			void				bus_callback_notification_device( void *refcon, int result, void **args, uint32_t numArgs );

					enum 
					{
						kMaxBusNoticationRefCons = 8,
						kInvalidRefConIndex = kMaxBusNoticationRefCons,
					};
					typedef struct
					{
						bus_impl*	busImplPtr;
						uint16		generationCount;
					} notification_refcon;
					class notification_lock: public base
					{
					};
	static			notification_lock		s_NotificationRefConsLock;
	static			notification_refcon		s_NotificationRefCons[kMaxBusNoticationRefCons];
	static			uint16					s_NotificationRefConsGenerationCount;
//\endcond
#endif//__MACH__

protected:
//\cond
					enum 
					{
						kProgressOpen = 0x8000000,
					};
					uint32				m_timeout;																						///< Internal driver access timeout
					uint32				m_driver_version;																				///< Internal 32bit driver version
					class cache 
					{
					public:
						cache(): 
							clients(0), 
							rate_readonly(false), 
							sync_readonly(false), 
							master_readonly(false),
							op_mode_readonly(false),
							io_size_readonly(false),
							nickname_readonly(false)
						{
							#ifndef TCAT_DICE_AVC
							memset (&config, 0xff, sizeof config);
							memset (&op_mode, 0xff, sizeof op_mode);
							#ifdef WIN32
							memset (&speaker, 0xff, sizeof speaker);
							#endif//WIN32
							#endif//TCAT_DICE_AVC
						}

						channel_list		channels[2];
						uint32				clients;
						bool				rate_readonly;
						bool				sync_readonly;
						bool				master_readonly;
						bool				op_mode_readonly;
						bool				io_size_readonly;
						bool				nickname_readonly;
						#ifndef TCAT_DICE_AVC
						ioctl::BUS_CONFIG	config;																						///< Internal bus config info
						OP_MODE				op_mode;
						#ifdef WIN32																						///< (WDM) WDM audio enabled or disabled
						struct SPEAKER_CONFIG
						{
							uint32	cfg;
							int	map[kTCAT_DICE_SPEAKER_MAPS];
						} speaker[2];
						uint32				min_io_size;
						uint32				max_io_size;
						#endif//WIN32
						#else//TCAT_DICE_AVC
						struct
						{
							uint32 devices;
						} config;
						#endif//TCAT_DICE_AVC
					} m_cache[2];
					reference<system_impl>	m_system;
private:	
					handle				m_hBus;																							///< Internal platform handle to bus driver
					ID					m_id;																							///< Internal bus ID
					#ifdef WIN32
	static			uint32	__stdcall	static_thread_notification	(void* lpParameter);												///< (WDM) Internal notification thread
	volatile		bool				m_thread_notification_running;																	///< (WDM) Internal bool weather notification thread is to terminate
					handle				m_hEvents[kBusNotifications];																	///< (WDM) Internal bus notification events for triggering notification thread
					handle				m_hThread;																						///< (WDM) Internal notification observer thread handle 
					handle				m_hReady;																						///< (WDM) Internal driver is ready signal
					#endif//WIN32
					#ifdef __MACH__
					uint16				m_NotificationRefConIndex;																			///< (OSX) Internal notification callback reference
//					#ifndef TCAT_DICE_AVC
					bool				m_ready;																						///< (OSX) Internal driver is ready signal
					pthread_cond_t		m_ready_cond;																					///< (OSX) Internal driver is ready signal
					pthread_mutex_t		m_ready_mutex;																					///< (OSX) Internal driver is ready signal
//					#endif//TCAT_DICE_AVC
public:
					bool				m_update;																						///< (OSX) Internal update flag
					bool				m_update_devicelist;																			///< (OSX) Internal update flag
					#endif//__MACH__
//\endcond
};



template <class bus_t> class system; 
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus
//
///////////////////////////////////////////////////////////////////////////////
///
/// platform independent bus - a generic device container
///
template<class device_t> class bus: public bus_impl, public std::deque<reference<device_t> >
{
	typedef typename dice::reference<device_t>	device_ref;
	typedef typename std::deque<device_ref>		super;
	
public:
	typedef	typename super::iterator			iterator;																///< device object iterator
	typedef	typename super::const_iterator		const_iterator;															///< device object const iterator

	bus():
		m_mount( false )
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus 0x%x] bus()", this, 0, 0, 0 );
		// empty
	}

	virtual						~bus()																					///< bus destructor
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus 0x%x] ~bus()", this, 0, 0, 0 );
		if (this->m_mount)
		{
			TDL_ENTRY( kTDLFlag_Error | kTDLFlag_PAL, "PAL [bus 0x%x] unmount() should have been called before ~bus()", this, 0, 0, 0 );
			PAL_DEBUG_BREAK		// the virtual method unmount() should have been called before trying to destroy the object
			this->m_mount = this->m_mount;
		}
	}
	virtual	inline	void		mount()																					/// Notifies about the readily created bus (first device arrival) 
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus 0x%x] mount()", this, 0, 0, 0 );
		lock lock(this);
		m_mount=true;
	}
	virtual	inline	void		unmount()																				/// Notifies about the removal of the bus (last device removal)
	{
		TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus 0x%x] unmount()", this, 0, 0, 0 );
		lock lock(this);
		if (m_mount)
		{
			m_mount = false;
			for (iterator device=super::begin(); device!=super::end(); device++)
			{
				(*device)->unmount();
			}
		}
	}
	virtual			device_ref	find		(uint64 guid64)					const								/// Finds a device on this bus by its unique ID
	{
		// search all STL device list entries for a given GUID
//		lock lock(this);
		for (const_iterator device=super::begin(); device!=super::end(); device++)
			if ((*device)->guid64() == guid64)
				return *device;
		return NULL;
	}
	virtual			device_ref	find		(const std::string nickname)	const								/// Finds a device on this bus by its nickname
	{
		// search all STL device list entries for a given nickname
//		lock lock(this);
		for (const_iterator device=super::begin(); device!=super::end(); device++)
			if ((*device)->nickname()==nickname)
				return *device;
		return NULL;
	}
	virtual			bool		add			(DEVICE_INFO& device_info)	throw(tcat::exception::base)
	{
		iterator device_list = super::begin();
		while (device_list!=super::end() && (*device_list)->guid64()<device_info.guid64) 
			device_list++;
		if (device_list==super::end() || (*device_list)->guid64()!=device_info.guid64)
		{
			// the guid of the container device is higher than the guid of the device to check:
			// create new device and insert it before this container device
			device_ref device(new_device (device_info.guid64),this);
			if (device)
			{
				device->create (this, device_info); 
				device->fetch (device_info, kDeviceNotificationAll);
				this->insert (device_list, device);
				device->mount();
				return true;
			}
		}
		return false;
	}
	virtual			void		fetch_devices()							throw(tcat::exception::base)			/// fetch device properties of the list of devices  
	{
		#ifndef TCAT_DICE_AVC
		DEVICE_NOTIFICATIONS notifications = device_notifications();
		lock lock(this);
		// the device_info will always be filled from the beginning of the array -
		// anyway the device_info.index will point into the driver structure of the
		// device array and therefor might not reflect the event_info index.
		for (uint32 idx=0; idx<m_cache[0].config.devices; idx++)
		{
			DEVICE_INFO& device_info = m_cache[0].config.device_info[idx];
			if (device_ref device=find(device_info.guid64))
				device->fetch(device_info, notifications.notification[device_info.idx]);
		}
		#endif//TCAT_DICE_AVC
	}
	virtual			bool		fetch_devicelist()						throw(tcat::exception::base)			/// Checks for device arrival/removal and updates the internal STL list
	{
		lock lock(this);
		// remove not existing devices
		for (iterator i=super::begin(); i!=super::end();)
		{
			// check if this STL bus list entry is an existing bus interface
			if (available((*i)->guid64()))
				i++;
			else
			{
				(*i)->unmount();
				i = this->erase(i);
			}
		}
		return bus_impl::fetch_devicelist();
	}
	virtual			void		update_devices()						throw(tcat::exception::base)					/// Sends device notifications to the list of devices  
	{
		lock lock(this);
		for (iterator device=super::begin(); device!=super::end(); device++)
			(*device)->update();	// notify subscriber
	}
protected:
	virtual			device_t*	new_device	(uint64 guid64)					const	{ return new device_t(); }			///< Override this to instantiate a derived device
private:
					bool		m_mount;
};
typedef dice::reference<bus<device> > bus_ref;	///< standard device reference



} // namespace dice
} // namespace tcat


#undef kDriverLog_File_ID

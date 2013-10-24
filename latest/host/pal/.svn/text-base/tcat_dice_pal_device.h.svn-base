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
//	dice pal device
//
#pragma once
#include <deque>
#include "tcat_dice_pal.h"
#include "../../interface/tcat_dice_firmware.h"
#include "../../interface/tcat_dice_firmware_loader.h"



namespace tcat
{
namespace dice
{



class bus_impl;
///////////////////////////////////////////////////////////////////////////////
//
//	device
//
///////////////////////////////////////////////////////////////////////////////
///
///	device representation
///
class device: public base
{
	typedef base super;
	typedef std::deque<std::string> SYNC_NAMES;

public:
	class channel: public std::string	/// device channel containing name and isoc stream related information for one audio channel
	{
	public:
					channel(std::string name, uint32 isoc, uint32 avs): std::string(name), m_isoc(isoc), m_avs(avs)	{}	///< constructore
		uint32		isoc()	const	{ return m_isoc; }	///< Return the isoc channel
		uint32		avs()	const	{ return m_avs; }	///< Return AVS sequence number in the isoc channel
	private:
	//\cond
		uint32		m_isoc;
		uint32		m_avs;
	//\endcond
	};
	typedef	std::deque<channel>	channel_list;		///< Internal channel representation class

								device();																						///< First stage device class construction
	virtual						~device();																						///< Device class desctruction
	static	std::string			type				(uint32 product_id);														///< Returns the product type description as specified in the customization for the given product ID
			std::string			type()														{ return type(uint32((guid64()>>kTCAT_DICE_GUID64_PRODUCT_ID_SHIFT)&kTCAT_DICE_GUID64_PRODUCT_ID_MASK)); }	///< Returns the product type description as specified in the customization for the given product ID
	virtual	void				create				(bus_impl* bus, const DEVICE_INFO& info)	throw(tcat::exception::base);	///< Second stage device creation
	virtual	void				fetch				(const DEVICE_INFO& info, DEVICE_NOTIFICATION notification=kDeviceNotificationNone)
																								throw(tcat::exception::base);	///< Updates device state
	virtual	bool				update();																						///< Notifies about a general device state change. Overwrite this to handle device state change notifications
	virtual	void				mount()														{}									///< Notifies about the arrival of a valid (completly built and updated) device 
	virtual	void				unmount()													{}									///< Notifies about the removal of the device (physically disconnected)
	virtual reference<bus_impl>	bus()														{ return m_bus; }					///< Returns the bus_impl reference into this device is mounted to
	const	uint64&				guid64()										const		{ return m_cache[0].info.guid64; }	///< Returns the device GUID. Format: | 24bit OUI (0x166) | 8bit Cat || 10bit product ID | 22bit serial# |
	const	uint32				idx()											const		{ return m_cache[0].info.idx; }		///< Returns the device index identifying the device in the driver internal device structure. The idx() is not related to the index into the bus owned device STL list.
	const	std::string			nickname()										const		{ return *(uint32*)m_cache[0].info.nickname==0xffffffff?"":m_cache[0].info.nickname; }///< Returns the device nickname
			void				nickname			(const std::string name)			throw(tcat::exception::base);			///< Sets device nickname
	const	DEVICE_INFO::STATE	state()											const		{ return m_cache[0].info.state; }	///< Returns the device state
	const	bool				supports_eap()									const		{ return (m_cache[0].info.capabilities & 0x00000004) != 0; }  ///< Returns whether the device supports the EAP protocol
	const	channel_list&		channels			(bool input)				const		{ return m_channels[input];}		///< Returns the in(true) or out(false) channel container
	const	SEQUENCES&			sequence			(bool input, uint32 isoc)	const		{ return m_squ[input][isoc];}		///< Returns the number of the in(true) or out(false) AUDIO or MIDI channels (sequences) in this isoc					
	const	uint32				clock()											const		{ return m_cache[0].clock.clock; }	///< Returns the meassured sample rate
	const	bool				locked()										const		{ return m_cache[0].clock.status & kStatusLocked; }						///< Returns wether the device is locked or not
	const	bool				tx_locked			(uint32 stream)				const		{ return m_cache[0].clock.tx_locked[stream] & 1;}///< Returns the transmitter locked state	
	const	EX_STATUS			rx_locked()										const		{ return EX_STATUS(m_cache[0].clock.ex_status & kExStatusLockedMask); }	///< Returns the extended status locked information	
	const	EX_STATUS			rx_slipped()											throw(tcat::exception::base);			///< Returns the extended status slip information	
	const	CLOCK_SYNC_CAPS		clock_sync_caps()								const		{ return m_clock_sync_caps; }		///< Returns the static combination of device clock and sync capabilty bit maps	
	const	SYNC_NAMES			sync_names()									const		{ return m_sync_names; }			///< Returns sync name container
			void				ohci_read			(uint32 offset, uint32* pData32, uint32 nData32=1)			
																				const	throw(tcat::exception::base);			///< Issues an OHCI asynchronous read command
			void				ohci_write			(uint32 offset, const uint32* pData32, uint32 nData32=1)	
																				const	throw(tcat::exception::base);			///< Issues an OHCI asynchronous write command
	const	uint32				midi_ports			(bool out)					const		{ return m_midi_ports[out]; }		///< Returns the number of MIDI (IN=false; OUT=true) ports
	const	uint32				isoc_streams		(bool out)					const		{ return m_isocs[out]; }			///< Returns the number isochronous streams (IN=false; OUT=true)
	const	uint32				firmware_capabilities()							const		{ return m_fwl_capabilities; }		///< Firmware loader protocol capabilities
			bool				firmware_info		(uint32 sector, FL_GET_IMAGE_DESC_RETURN& info)	const;						///< Returns the firmware binary info description by reading from the device
	static	FL_GET_APP_INFO_RETURN	firmware_info	(const std::string filename)		throw(tcat::exception::base);			///< Returns the firmware binary info description by parsing the binary file
	const	FL_GET_APP_INFO_RETURN&	firmware_info()								const		{ return m_fw_info; }				///< Return firmware info
			uint32				firmware_upload		(const std::string filename, const std::string sector, bool check32=false)
																						throw(tcat::exception::base);			///< Uploads the firmware to the device, does not burn it yet and returns the file size. Set check32 to true only if your firmware uses 32-bit checksums.
			uint32				firmware_upload		(const unsigned char *pData, uint32 filesize, const std::string sector, bool check32)
																						throw(tcat::exception::base);			///< Uploads the firmware to the device, does not burn it yet and returns the file size. Set check32 to true only if your firmware uses 32-bit checksums.
			uint32				firmware_download	(const std::string filename)		throw(tcat::exception::base);			///< Downloads the complete flash image from the device to a file
			uint32				firmware_download	(std::string * buf, const std::string sector)		throw(tcat::exception::base);			///< Downloads a flash sector (file) from the device to a file
			void				firmware_delete		(const std::string sector)			throw(tcat::exception::base);			///< Deletes the specified image from the device flash
			void				firmware_create		(const std::string sector, uint32 filesize, int32 exec_addr=0x30000, int32 entry_addr=0x30040) throw(tcat::exception::base);	///< Creates a flash image
			void				firmware_reset()										throw(tcat::exception::base);			///< Resets the device
			void				firmware_test(uint32 test_mode)							throw(tcat::exception::base);			///< Set device test mode
#ifdef TCAT_DEBUG
			DD_GLOBAL			_dbg_global()									const	throw(tcat::exception::base)	{ DD_GLOBAL global; ohci_read(m_spaces[kSpaceGlobal].offset, (uint32*)&global, std::min<uint32>(m_spaces[kSpaceGlobal].size32,sizeof(global)>>2)); return global; }
			IO<kTCAT_DICE_TX>	_dbg_tx()										const	throw(tcat::exception::base)	{ IO<kTCAT_DICE_TX> tx; ohci_read(m_spaces[kSpaceTx].offset, (uint32*)&tx, std::min<uint32>(m_spaces[kSpaceTx].size32,sizeof(tx)>>2)); return tx; }
			IO<kTCAT_DICE_RX>	_dbg_rx()										const	throw(tcat::exception::base)	{ IO<kTCAT_DICE_RX> rx; ohci_read(m_spaces[kSpaceRx].offset, (uint32*)&rx, std::min<uint32>(m_spaces[kSpaceRx].size32,sizeof(rx)>>2)); return rx; }
	const	SPACE&				_dbg_space(SPACES space)						const		{ return m_spaces[space]; }
#endif

protected:
	virtual	void				fetch_rx()												throw(tcat::exception::base);			///< Notifies about an RX configuration change and extracts the isoc RX channel information
	virtual	void				fetch_tx()												throw(tcat::exception::base);			///< Notifies about an TX configuration change and extracts the isoc TX channel information

	virtual	void				update_clock()												{}									///< Notifies about device clock (sample rate) change at a locked device - obtain new sample rate with clock()
	virtual	void				update_rx_locked()											{}									///< Notifies about an extended lock status change - obtain new state with rx_locked()
	virtual	void				update_locked()												{}									///< Notifies about the locked state of the device has changed - obtain new state with locked()
	virtual	void				update_tx_locked()											{}									///< Notifies about an TX status change - obtain new state with tx_locked()
	virtual	void				update_nickname()											{}									///< Notifies about a nickname change - obtain new nickname with nickname()
	virtual	void				update_idx()												{}									///< Notifies about a device index change (actually another device with a lower GUID has arrived or has been removed)
	virtual	void				update_state()												{}									///< Notifies about a device state change - obtain new state with state()
	virtual	void				update_rx()													{}									///< Notifies about an RX configuration change and extracts the isoc RX channel information
	virtual	void				update_tx()													{}									///< Notifies about an TX configuration change and extracts the isoc TX channel information
	virtual	void				update_user(DEVICE_NOTIFICATION notification)				{}									///< Notifies about a device user event
	virtual	void				firmware_progress	(uint32 percent, FL_OP_ENUM operation)	const	{}							///< Firmware operation progress callback
			void				firmware_package	(FL_OP_ENUM operation, uint32 timeout, const void* package=NULL, uint32 size32=1, uint32 percent=0, uint32 percent_last=0)
																						const	throw(tcat::exception::base);	///< Internal firmware package communication protocol helper			
//\cond
			struct
			{
				DEVICE_INFO			info;				///< Contains device identifiers
				ioctl::DEVICE_CLOCK	clock;				///< Contains dice clock param
			} m_cache[2];								///< Current and last cache state (used to diff to find out to what to notify about)
private:
			reference<bus_impl>	m_bus;					///< The bus reference
			#ifdef __MACH__
			#ifdef TCAT_DICE_AVC
			void**				m_avc_interface;		///< OSX: fwlib device interface
			#endif//TCAT_DICE_AVC
			void**				m_fw_interface;			///< OSX: fwlib device interface
			#endif//__MACH__
			SPACE				m_spaces[kSpaces];		///< Device firewire space orginization
			DEVICE_NOTIFICATION	m_notification;			///< Driver notification flags
			channel_list		m_channels[2];			///< Array of I/O channels
			SEQUENCES			m_squ[2][4];			///< Contains information about audio and midi channel configuration of the used isoc streams
			CLOCK_SYNC_CAPS		m_clock_sync_caps;		///< Static combination of device clock and sync capabilty bit maps
			SYNC_NAMES			m_sync_names;			///< Sync name container
			uint32				m_isocs[2];				///< Number of Isoc streams
			uint32				m_midi_ports[2];		///< Number of MIDI ports
			FL_GET_APP_INFO_RETURN	m_fw_info;			///< Firmware info
			uint32				m_fwl_capabilities;		///< Firmware loader protocol capabilities
//\endcond
};
typedef dice::reference<device> device_ref;						///< Device reference type



} // namespace dice
} // namespace tcat

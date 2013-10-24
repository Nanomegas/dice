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
#include "tcat_dice_pal_bus.h"

#ifdef WIN32
#include <windows.h>
#include "../interface/tcat_dice_win.h"
#endif//WIN32
#ifdef __MACH__
#include "../interface/tcat_dice_mac.h"
#include "tcat_dice_pal_system.h"
#include <IOKit/IOKitLib.h>
#endif//__MACH__

#define kDriverLog_File_ID		1004


using namespace tcat;
using namespace dice;



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::available
//
///////////////////////////////////////////////////////////////////////////////
//
bool bus_impl::available(uint64 guid64) throw(tcat::exception::base)
{
	for (unsigned int idx=0; idx<m_cache[0].config.devices; idx++)
	{
		DEVICE_INFO& device_info = m_cache[0].config.device_info[idx];
		if (guid64 == device_info.guid64)
			return true;
	}
	return false;
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
	// add new devices
	for (unsigned int idx = 0; idx<m_cache[0].config.devices; idx++)
		result |= add(m_cache[0].config.device_info[idx]);
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
	lock lock(this);

	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] fetch()", this, 0, 0, 0 );

	ioctl (kIoctlBusConfigGet, NULL, 0, &m_cache[0].config, sizeof m_cache[0].config);
	ioctl (kIoctlBusOpModeGet, NULL, 0, &m_cache[0].op_mode, sizeof m_cache[0].op_mode);

#ifdef WIN32
	ioctl::BUS_CHANNELS channels;
	for (int io=0; io<=1; io++)
	{
		channels.out = io;
		ioctl (kIoctlBusChannelsGet, &channels, sizeof channels, &channels, sizeof channels);
		m_cache[0].channels[io].clear();
		for (unsigned int chnl=0; chnl<channels.channels; chnl++)
			m_cache[0].channels[io].push_back(
				channel(
					channels.channel[chnl].name,
					(channels.ac3_map[0]==chnl)||(channels.ac3_map[1]==chnl),
					(channels.channel[chnl].ac3_capable != 0)));
	}
	// retrieve the speaker setup
	ULONG spk = 0;
	const_cast<bus_impl*>(this)->ioctl(kIoctlBusSpeakerGet, &spk, sizeof spk, &spk, sizeof spk);
	m_cache[0].speaker[1].cfg = spk;

	// retreive the channel mapping for each of the speakers
	ioctl::BUS_GET_CHANNEL_MAP map = {0};
	const_cast<bus_impl*>(this)->ioctl(kIoctlBusChannelMapGet, &map, sizeof map, &map, sizeof map);
	for (int chn=0; chn<kTCAT_DICE_MAX_WDM_IN_CHANNELS; chn++)
	{
		m_cache[0].speaker[0].map[chn] = map.inChannel[chn];		
	}
	for (int chn=0; chn<kTCAT_DICE_MAX_WDM_OUT_CHANNELS; chn++)
	{
		m_cache[0].speaker[1].map[chn] = map.outChannel[chn];		
	}

	// get the audio client count
	ioctl::BUS_CLIENT_COUNT clients;
	ioctl (kIoctlBusClientsGet, NULL, 0, &clients, sizeof clients);
	m_cache[0].clients = clients.asioClientCount + clients.wdmClientCount;

	// parameter locking rules

	// hold sample rate if any clients
	m_cache[0].rate_readonly = (m_cache[0].clients > 0);

	// hold buffer size if any WDM clients, or if more than one ASIO client and no WDM clients
	m_cache[0].io_size_readonly = ( (clients.wdmClientCount > 0)
							|| !((clients.asioClientCount < 2) && (0 == clients.wdmClientCount)) );

	// hold operation mode if io_size is holding
	m_cache[0].op_mode_readonly = m_cache[0].io_size_readonly;

	// hold sync source if any clients
	m_cache[0].sync_readonly = (m_cache[0].clients > 0);
	// hold master device if any clients
	m_cache[0].master_readonly = (m_cache[0].clients > 0);
	// hold nickname if any clients
	m_cache[0].nickname_readonly = (m_cache[0].clients > 0);

	// get the minimum buffer size
	ioctl::BUS_GET_IO_SIZE_MIN_MAX	ioRange = {0};
	ioctl( kIoctlBusMinMaxIoSizeGet, NULL, 0, &ioRange, sizeof(ioRange) );
	m_cache[0].min_io_size = ioRange.minimum;
	m_cache[0].max_io_size = ioRange.maximum;

#endif//WIN32
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
	
	TDL_ENTRY( kTDLFlag_Info | kTDLFlag_PAL, "PAL [bus_impl 0x%x] update()", this, 0, 0, 0 );

	if (m_cache[0].config.sync != m_cache[1].config.sync)
	{ 
		update_sync(); 
		bReturn = true; 
	}
	for (int idx=0; idx<kSyncItems; idx++)
	{
		if (std::string(m_cache[0].config.syncname[idx]) != m_cache[1].config.syncname[idx])
		{ 
			update_sync_names(); 
			bReturn = true;
			break;
		}
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
	if (m_cache[0].config.clock_caps != m_cache[1].config.clock_caps)
	{ 
		update_clock_caps(); 
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
	if (m_cache[0].rate_readonly != m_cache[1].rate_readonly)
	{ 
		update_rate_readonly(); 
		bReturn = true; 
	}
	if (m_cache[0].sync_readonly != m_cache[1].sync_readonly)
	{ 
		update_sync_readonly(); 
		bReturn = true; 
	}
	if (m_cache[0].master_readonly != m_cache[1].master_readonly)
	{ 
		update_master_readonly(); 
		bReturn = true; 
	}
	if (m_cache[0].op_mode_readonly != m_cache[1].op_mode_readonly)
	{ 
		update_op_mode_readonly(); 
		bReturn = true; 
	}
	if (m_cache[0].io_size_readonly != m_cache[1].io_size_readonly)
	{ 
		update_io_size_readonly(); 
		bReturn = true; 
	}
	if (m_cache[0].nickname_readonly != m_cache[1].nickname_readonly)
	{ 
		update_nickname_readonly(); 
		bReturn = true; 
	}

#ifdef WIN32
	if (m_cache[0].config.io_size != m_cache[1].config.io_size)
	{
		update_io_size();
		bReturn = true;
	}
	if (m_cache[0].min_io_size != m_cache[1].min_io_size)
	{
		update_min_io_size();
		bReturn = true;
	}
	if (m_cache[0].max_io_size != m_cache[1].max_io_size)
	{
		update_max_io_size();
		bReturn = true;
	}

	// notify about speaker setup change
	if (m_cache[0].speaker[1].cfg != m_cache[1].speaker[1].cfg)
	{
		update_speaker();
		bReturn = true;
	}

	for (int io=0; io<=1; io++)
	{
		// notify about channel mapping change for each of the speakers
		uint32 speaker=0;
		for (int chn=0; chn<32; chn++)
		{
			if (((1<<chn) & m_cache[0].speaker[io].cfg) && m_cache[0].speaker[io].map[speaker] != m_cache[1].speaker[io].map[speaker])
			{
				update_speaker_map( (io != 0), speaker );
				speaker++;
				bReturn = true;
			}
		}
	}
#endif//WIN32

	m_cache[1] = m_cache[0];

	return bReturn;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::device_notifications
//
///////////////////////////////////////////////////////////////////////////////
//
DEVICE_NOTIFICATIONS bus_impl::device_notifications() throw(tcat::exception::base)
{
	DEVICE_NOTIFICATIONS notifications;
	ioctl (kIoctlDeviceNotificationsGet, NULL, 0, &notifications, sizeof notifications);
	return notifications;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::clock
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::clock(CLOCK clk) throw(tcat::exception::base)
{
	if (clk != m_cache[0].config.clock)
	{
		uint32		newClk = (uint32)clk;

		ioctl( kIoctlBusClockSet, &newClk, sizeof(newClk) );
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::clock_name
//
///////////////////////////////////////////////////////////////////////////////
//
const std::string bus_impl::clock_name(CLOCK clock) const
{
	const std::string clock_name[] =
	{
		"32kHz",
		"44.1kHz",
		"48kHz",
		"88.2kHz",
		"96kHz",
		"176.4kHz",
		"192kHz",
		"Any Lo(32-48kHz)",
		"Any Mid(88.1-96kHz)",
		"Any Hi(176-192kHz)"
	};

	return ((unsigned int)clock < (sizeof(clock_name)/sizeof(std::string))-1) ? clock_name[clock] : "Invalid (None)";
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::sync
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::sync(SYNC sync) throw(tcat::exception::base)
{
	if (sync != m_cache[0].config.sync)
	{
		uint32		newSync = (uint32)sync;

		ioctl( kIoctlBusSyncSet, &newSync, sizeof(newSync) );
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::master
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::master(uint64 guid64) throw(tcat::exception::base)
{
	if (guid64 != m_cache[0].config.master_guid64)
	{
		ioctl( kIoctlBusMasterSet, &guid64, sizeof(guid64) );
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus::master
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::add_reported_audio_latency(uint32 inputSamples, uint32 outputSamples) throw(tcat::exception::base)
{
	ioctl::BUS_ADD_AUDIO_LATENCY	addAudioLatency;

	addAudioLatency.inputSamples = inputSamples;
	addAudioLatency.outputSamples = outputSamples;

	ioctl( kIoctlBusAddToReportedLatency, &addAudioLatency, sizeof(addAudioLatency) );
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::errors
//
///////////////////////////////////////////////////////////////////////////////
//
//	returns number of streaming related errors
//
ioctl::BUS_ERROR bus_impl::errors() const throw(tcat::exception::base)
{
	ioctl::BUS_ERROR errors;
	const_cast<bus_impl*>(this)->ioctl (kIoctlBusErrorsGet, NULL, 0, &errors, sizeof errors);
	return errors;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::error_clear
//
///////////////////////////////////////////////////////////////////////////////
//
//	returns number of sample slips.
//
void bus_impl::error_clear() const throw(tcat::exception::base)
{
	const_cast<bus_impl*>(this)->ioctl (kIoctlBusErrorsClear, NULL, 0);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::dice::bus_impl::op_mode
//
///////////////////////////////////////////////////////////////////////////////
//
void bus_impl::op_mode(OP_MODE op_mode) throw(tcat::exception::base)
{
	if (op_mode != m_cache[0].op_mode)
		ioctl(kIoctlBusOpModeSet, &op_mode, sizeof op_mode);
}

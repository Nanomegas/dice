///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////
//
#pragma once
#include "tcat_dice_pal_system.h"
#include "dice_eap_device.h"

///////////////////////////////////////////////////////////////////////////////
//
//	event_device
//
///////////////////////////////////////////////////////////////////////////////
//
class device_listener;

class event_device: public tcat::dice::eap_device
{
	typedef tcat::dice::device super;
	typedef std::deque<device_listener*> listeners;
	listeners m_listeners;
public:
	virtual void attach(device_listener* listener);
	virtual void detach(device_listener* listener);
	virtual void mount();						
	virtual void unmount();
	virtual void update_nickname();
	virtual void update_idx();
	virtual void update_state();
	virtual void update_clock();
	virtual void update_rx();
	virtual void update_tx();
	virtual void update_rx_locked();
	virtual void update_tx_locked();
	virtual void update_locked();
	virtual void firmware_progress(tcat::uint32 percent, tcat::dice::FL_OP_ENUM operation) const;
	virtual void update_user(tcat::dice::DEVICE_NOTIFICATION notification);
};
typedef tcat::dice::reference<event_device> event_device_ref;



///////////////////////////////////////////////////////////////////////////////
//
//	event_bus
//
///////////////////////////////////////////////////////////////////////////////
//
class bus_listener;
class event_bus: public tcat::dice::bus<event_device>
{
	typedef tcat::dice::bus<event_device> super;
	typedef std::deque<bus_listener*> listeners;
	listeners m_listeners;
public:
	virtual void attach(bus_listener* listener);
	virtual void detach(bus_listener* listener);
	virtual void mount();
	virtual void unmount();
	virtual void update_clients();
	virtual void update_rate_readonly();
	virtual void update_sync_readonly();
	virtual void update_master_readonly();
	virtual void update_op_mode_readonly();
	virtual void update_io_size_readonly();
	virtual void update_nickname_readonly();
	virtual void update_clock();
	virtual void update_clock_caps();
	virtual void update_sync_names();
	virtual void update_sync_caps();
	virtual void update_devicelist();
	virtual void update_devices() throw(tcat::exception::base);
	virtual void update_sync();
	virtual void update_master();
	virtual void update_op_mode();
#ifdef WIN32
	virtual void update_speaker();
	virtual void update_speaker_map(bool out, tcat::uint32 speaker);
	virtual void update_io_size();
	virtual void update_min_io_size();
#endif
	virtual		~event_bus();
//	virtual void progress();
};
typedef tcat::dice::reference<event_bus>	event_bus_ref;

typedef tcat::dice::system<event_bus>		event_system;
typedef tcat::dice::reference<event_system>	event_system_ref;	

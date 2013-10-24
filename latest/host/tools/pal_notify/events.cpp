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
#include "events.h"
#include "listener.h"



///////////////////////////////////////////////////////////////////////////////
//
//	event_device
//
///////////////////////////////////////////////////////////////////////////////
//
void event_device::attach(device_listener* listener)	
{
	tcat::dice::lock lock(this);
	m_listeners.push_back(listener); 
	listener->mount(this);
}

void event_device::detach(device_listener* listener)	
{ 
	tcat::dice::lock lock(this);
	listener->unmount(); 
	m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), listener)); 
}

void event_device::mount()						
{
	super::mount(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->mount(this); 
}

void event_device::unmount()						
{ 
	super::unmount(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->unmount(); 
}

void event_device::update_nickname()				
{ 
	super::update_nickname(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_nickname(); 
}

void event_device::update_idx()					
{	
	super::update_idx(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_idx(); 
}

void event_device::update_state()					
{ 
	super::update_state(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_state(); 
}

void event_device::update_clock()					
{ 
	super::update_clock(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_clock(); 
}

void event_device::update_rx()				
{ 
	super::update_rx(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_rx(); 
}

void event_device::update_tx()				
{ 
	super::update_tx(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_tx(); 
}

void event_device::update_rx_locked()				
{ 
	super::update_rx_locked(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_rx_locked(); 
}

void event_device::update_tx_locked()				
{ 
	super::update_tx_locked(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_tx_locked(); 
}

void event_device::update_locked()				
{ 
	super::update_locked(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_locked(); 
}

void event_device::firmware_progress(tcat::uint32 percent, tcat::dice::FL_OP_ENUM operation) const
{
	super::firmware_progress(percent, operation);
	for (listeners::const_iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++)
		(*listener)->firmware_progress(percent, operation); 
}

void event_device::update_user(tcat::dice::DEVICE_NOTIFICATION notification)
{
	super::update_user(notification);
	for (listeners::const_iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++)
		(*listener)->update_user(notification); 
}


///////////////////////////////////////////////////////////////////////////////
//
//	event_bus
//
///////////////////////////////////////////////////////////////////////////////
//
event_bus::~event_bus()	
{
}

void event_bus::attach(bus_listener* listener)	
{ 
	tcat::dice::lock lock(this);
	m_listeners.push_back(listener); 
	listener->mount(this); 
}

void event_bus::detach(bus_listener* listener)	
{ 
	tcat::dice::lock lock(this);
	listener->unmount(); 
	m_listeners.erase(std::find(m_listeners.begin(), m_listeners.end(), listener)); 
}

void event_bus::mount()						
{
	super::mount(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->mount(this);
}

void event_bus::unmount()						
{ 
	super::unmount(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->unmount(); 
}

void event_bus::update_clients()					
{ 
	super::update_clients(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_clients(); 
}

void event_bus::update_rate_readonly()
{ 
	super::update_rate_readonly(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_rate_readonly(); 
}

void event_bus::update_sync_readonly()
{ 
	super::update_sync_readonly(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_sync_readonly(); 
}

void event_bus::update_master_readonly()
{ 
	super::update_master_readonly(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_master_readonly(); 
}

void event_bus::update_op_mode_readonly()
{ 
	super::update_op_mode_readonly(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_op_mode_readonly(); 
}

void event_bus::update_io_size_readonly()
{ 
	super::update_io_size_readonly(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_io_size_readonly(); 
}

void event_bus::update_nickname_readonly()
{ 
	super::update_nickname_readonly(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_nickname_readonly(); 
}

void event_bus::update_clock()					
{ 
	super::update_clock(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_clock(); 
}

void event_bus::update_clock_caps()				
{ 
	super::update_clock_caps(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_clock_caps(); 
}

void event_bus::update_sync_names()			
{ 
	super::update_sync_names(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_sync_names(); 
}

void event_bus::update_sync_caps()				
{ 
	super::update_sync_caps(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_sync_caps(); 
}

void event_bus::update_devicelist()			
{ 
	super::update_devicelist(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_devicelist(); 
}

void event_bus::update_devices() throw(tcat::exception::base)				
{ 
	super::update_devices(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_devices(); 
}

void event_bus::update_sync()					
{ 
	super::update_sync(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_sync();
}

void event_bus::update_master()				
{ 
	super::update_master(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_master(); 
}

void event_bus::update_op_mode()				
{ 
	super::update_op_mode(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_op_mode(); 
}

#ifdef WIN32
void event_bus::update_io_size()				
{ 
	super::update_io_size(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_io_size(); 
}

void event_bus::update_min_io_size()				
{ 
	super::update_min_io_size(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_min_io_size(); 
}

void event_bus::update_speaker()		
{ 
	super::update_speaker(); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_speaker(); 
}

void event_bus::update_speaker_map(bool out, tcat::uint32 speaker) 
{	
	super::update_speaker_map(out, speaker); 
	for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) 
		(*listener)->update_speaker_map(out, speaker); 
}

#endif // WIN32

//void progress() { super::progress(); for (listeners::iterator listener=m_listeners.begin(); listener!=m_listeners.end(); listener++) (*listener)->progress(); }


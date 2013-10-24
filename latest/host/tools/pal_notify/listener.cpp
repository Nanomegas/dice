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
//	device_listener
//
///////////////////////////////////////////////////////////////////////////////
//
device_listener::~device_listener()	
{
	if (m_device)
		m_device->detach(this); 
}

void device_listener::mount(tcat::dice::reference<event_device> device)			
{ 
	if (device)
		m_device = device; 
}

void device_listener::unmount()			
{ 
	m_device = NULL;
}



///////////////////////////////////////////////////////////////////////////////
//
//	bus_listener
//
///////////////////////////////////////////////////////////////////////////////
//
bus_listener::~bus_listener()	
{
	if (m_bus)
		m_bus->detach(this); 
}

void bus_listener::mount(tcat::dice::reference<event_bus> bus)			
{ 
	if (bus)
		m_bus = bus; 
}

void bus_listener::unmount()			
{ 
	m_bus = NULL;
}

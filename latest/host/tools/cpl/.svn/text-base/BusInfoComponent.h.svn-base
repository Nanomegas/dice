///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "JuceHeader.h"
#include "tcat.h"
#include "events.h"
#include "listener.h"
#include <map>

class BusInfoComponent;


//==============================================================================
// this component contains the subcomponents that display global information
//
// 
class BusInfoComponent : 
	public Component,
	public AsyncUpdater,
	public bus_listener
{
public:
	BusInfoComponent();
    ~BusInfoComponent();

	// gui callbacks
	void paint (Graphics& g);
    void resized();
	void broughtToFront();
    void lookAndFeelChanged();
	void handleAsyncUpdate();

	// gui updates
	void updateContent();
	void updateInfo();

	// bus listener callbacks, used in the PAL thread
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();

private:
    Label		* m_driver_info_l;				// shows driver version
    Label		* m_pal_info_l;					// shows pal version
    Label		* m_vendor_info_l1;				// shows vendor info
    Label		* m_vendor_info_l2;				// shows vendor info
    Label		* m_vendor_info_l3;				// shows vendor info
	Drawable	* m_logo;						// vendor logo
};

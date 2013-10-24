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

#ifndef __DEVICE_GENERAL_COMPONENT__
#define __DEVICE_GENERAL_COMPONENT__

#include "JuceHeader.h"
#include "DeviceWindow.h"
#include "events.h"
#include "listener.h"

class DeviceGeneralComponent : 
		public Component,
		public LabelListener,
		public ActionBroadcaster,
		public ActionListener,
		public device_listener,
		private AsyncUpdater
{
public:
	DeviceGeneralComponent();
    ~DeviceGeneralComponent();

	void setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus);

	void handleAsyncUpdate();
	
	void resized();
	void broughtToFront();
	void lookAndFeelChanged();

	void updateContent ();
	void setNewNickname();

	// Label editor mode callback
	void labelTextChanged (Label* labelThatHasChanged);

	// device_listener callbacks
	void mount(tcat::dice::reference<event_device> device);
	void unmount();
	void update_nickname();
	void update_state();
	void update_clock();
	void update_locked();

	void actionListenerCallback(const String &message);

	juce_UseDebuggingNewOperator

private:
	Label					* m_type_l;
	TextEditor				* m_type;
	Label					* m_dev_nickname;
    Label					* m_dev_nickname_l;
    Label					* m_status_l;
    Label					* m_status;
    Label					* m_status_ex;
    Label					* m_rate_l;
    Label					* m_rate;
    
	tcat::uint64			m_guid;
	String					m_dev_type;
	String					m_nickname;
	tcat::dice::DEVICE_INFO::STATE	m_state;
	tcat::uint32			m_clock;
	bool					m_Tx0Locked;
	bool					m_Tx1Locked;
	bool					m_lock_state;
	bool					m_show_measured_sr;
};

#endif   // __DEVICE_GENERAL_COMPONENT__

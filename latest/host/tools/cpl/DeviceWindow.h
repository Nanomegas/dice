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
#include "events.h"
#include "listener.h"

class DeviceListComponent;
class DeviceGeneralComponent;
class DeviceSimpleFirmwareComponent;
class DeviceFirmwareComponent;
class DeviceTestComponent;
class DeviceMixerComponent;
class DeviceWindow;
class Blender;

//==============================================================================

class DeviceWindowTabs : public TabbedComponent
{
public:
    //==============================================================================
    DeviceWindowTabs (DeviceWindow * devWindow);
    ~DeviceWindowTabs();
	
	void currentTabChanged(int newCurrentTabIndex, const String & tabName);

	DeviceWindow		* m_dev_window;
	int					m_first_mixer_index;

    juce_UseDebuggingNewOperator
};

//==============================================================================

class DeviceWindow  : 
				public Component,
				public ActionListener,
				public ChangeListener,
				public bus_listener,
				private AsyncUpdater
{
public:
    //==============================================================================
    DeviceWindow ();
    ~DeviceWindow();
	
	void setSizeFromTabIndex(int index);
    void resized();
	void lookAndFeelChanged();

	void updateShownDevice();
	void actionListenerCallback (const String &message);
	void changeListenerCallback (ChangeBroadcaster* source);
	void handleAsyncUpdate();
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();
	void update_devicelist();
	void update_sync();
	void update_clock();

	// EAP support
	int								m_num_mixers;
	DeviceMixerComponent			* m_devmixer[18];
	Blender							* m_blender;


    juce_UseDebuggingNewOperator

private:
	friend class ContentComp;

	enum DEV_UPDATE_NOTIFICATION
	{
		updt_device_change =    (1 << 0),
		updt_mixers_created =	(1 << 1),
		updt_sync_clock =		(1 << 2)
	};

	uint32							m_update;
	tcat::dice::SYNC				m_sync;
	tcat::uint32					m_clock;
	DeviceListComponent				* m_devlist;
	DeviceWindowTabs				* m_tabs;
	DeviceGeneralComponent			* m_devgeneral;
	DeviceSimpleFirmwareComponent	* m_dev_simplefirmware;
	DeviceFirmwareComponent			* m_devfirmware;
	DeviceTestComponent				* m_devtest;
};


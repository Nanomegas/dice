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

#include "DeviceWindow.h"
#include "MainCplWindow.h"
#include "DeviceList.h"
#include "DeviceGeneralComponent.h"
#include "DeviceFirmwareComponent.h"
#include "DeviceSimpleFirmwareComponent.h"
#include "DeviceTestComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"
#include "DeviceMixerComponent.h"
#include "Blender.h"
#include "tcat_dice.h"
#include "EventLogger.h"

DeviceWindowTabs::DeviceWindowTabs (DeviceWindow * devWindow) :
	TabbedComponent (TabbedButtonBar::TabsAtTop),
	m_dev_window(devWindow),
	m_first_mixer_index (0)
{
}

DeviceWindowTabs::~DeviceWindowTabs ()
{
}

void DeviceWindowTabs::currentTabChanged(const int newCurrentTabIndex, const String & tabName)
{
	if (JUCEApplication::getInstance()->isInitialising())
	{
		return;
	}

	int currentTab = getCurrentTabIndex();
	if (currentTab < 0)
	{
		return;
	}

	for (int i=0; i<getNumTabs(); i++)
	{
		if (i >= m_first_mixer_index)
		{
			// enable only the visible mixer
			if (i == currentTab)
			{
				getTabContentComponent(i)->setEnabled(true);
			}
			else
			{
				getTabContentComponent(i)->setEnabled(false);
			}
		}
	}

	// set app window size
	m_dev_window->setSizeFromTabIndex(currentTab);
}

#ifdef WIN32
// suppress warning about default initialize of array in constructor
//  default init is correct but recently supported by MS, so MS warns that we're using it as new behavior
#pragma warning( disable : 4351 )
#endif

// the device window consists of a header, tabbed control and a device list
DeviceWindow::DeviceWindow ()
: m_num_mixers (0),
	m_devmixer (),
	m_blender (0),
	m_update (0),
	m_sync ((tcat::dice::SYNC) 0),
	m_clock (0)
{

	setName ("Device Window");
	Colour tabColour(findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId));

	// create and place controls. 
	//	the cotrols will be "live" after our listener is attached. 
	//	see mount() below
    m_tabs = new DeviceWindowTabs(this);
    addAndMakeVisible (m_tabs);
	m_tabs->setBoundsInset(BorderSize<int> (0));
	m_tabs->setTabBarDepth(35);
	
	m_devlist = new DeviceListComponent ();
    m_devlist->setRowHeight (60);
	m_devlist->setBounds (m_tabs->getRight(), 100, getWidth()-m_tabs->getWidth(), 300);	// height should be row height * 5
    addAndMakeVisible (m_devlist);
    // the device window wants action messages from the device list
    m_devlist->addActionListener (this);
	
	m_devgeneral = new DeviceGeneralComponent();
	m_tabs->addTab ("General", tabColour, m_devgeneral, true);
    // the device list wants action messages from the device general component
	m_devgeneral->addActionListener (m_devlist);
	m_tabs->m_first_mixer_index++;

	if (appDefaults->getAppSettings()->getBoolValue("showSimpleTab"))
	{
		m_dev_simplefirmware = new DeviceSimpleFirmwareComponent();
		m_tabs->addTab ("Firmware Loader", tabColour, m_dev_simplefirmware, true);
		// the device simple firmware view wants action messages from the device general component
		m_devgeneral->addActionListener (m_dev_simplefirmware);
		m_tabs->m_first_mixer_index++;
	}

	if (appDefaults->getAppSettings()->getBoolValue("showAdvancedTab"))
	{
		m_devfirmware = new DeviceFirmwareComponent();
		m_tabs->addTab ("Firmware Utils", tabColour, m_devfirmware, true);
		// the device firmware view wants action messages from the device general component
		m_devgeneral->addActionListener (m_devfirmware);
		m_tabs->m_first_mixer_index++;
	}

	if (appDefaults->getAppSettings()->getBoolValue("showTestTab"))
	{
		m_devtest = new DeviceTestComponent();
		m_tabs->addTab ("Test", tabColour, m_devtest, true);
		// the device test view wants action messages from the device general component
		m_devgeneral->addActionListener (m_devtest);
		m_tabs->m_first_mixer_index++;
	}

	// blender creates mixer tabs dynamically for EAP-enabled devices
	m_blender = new Blender();
	// the device window wants change messages from the blender object
    m_blender->addChangeListener (this);
	m_blender->setInitialized();

	lookAndFeelChanged();
}

DeviceWindow::~DeviceWindow()
{
	if (m_blender)
	{
		m_blender->removeAllChangeListeners();
		deleteAndZero(m_blender);
	}

    deleteAllChildren();
}

void DeviceWindow::setSizeFromTabIndex(int index)
{
	if ((m_num_mixers > 0) && (index >= m_tabs->m_first_mixer_index))
	{
		int mixer_id = index - m_tabs->m_first_mixer_index;
		int width = m_devmixer[mixer_id]->getMixerWidth();
		AppWindow::getInstance()->setMainWindowSize(width, 700);
	}
	else
	{
		AppWindow::getInstance()->setMainWindowSize(700, 640);
	}
}

void DeviceWindow::resized()
{
	m_tabs->setBounds (2, 61, getWidth()-202, getHeight()-94);
	m_devlist->setBounds (m_tabs->getRight()+7, 100, getWidth()-m_tabs->getWidth()-8, getHeight()-130 /* 300 */);
}

void DeviceWindow::lookAndFeelChanged()
{
	Colour tabColour(findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId));

	repaint();

	for (int i=0; i<m_tabs->getNumTabs(); i++)
	{
		m_tabs->setTabBackgroundColour(i, tabColour);
	}
	m_tabs->setColour(TabbedComponent::outlineColourId, tabColour);

	repaint();
}

void DeviceWindow::updateShownDevice()
{
	tcat::uint64 guid = m_devlist->getSelectedGuid();

	if (guid != 0)
	{
		m_devgeneral->setShownDeviceGuid(guid, m_bus);

		if (appDefaults->getAppSettings()->getBoolValue("showSimpleTab"))
		{
			m_dev_simplefirmware->setShownDeviceGuid(guid, m_bus);
		}

		if (appDefaults->getAppSettings()->getBoolValue("showAdvancedTab"))
		{
			m_devfirmware->setShownDeviceGuid(guid, m_bus);
		}

		if (appDefaults->getAppSettings()->getBoolValue("showTestTab"))
		{
			m_devtest->setShownDeviceGuid(guid, m_bus);
		}

		if (m_bus)
		{
			event_device_ref devref = m_bus->find(guid);
			m_blender->setShownDeviceGuid(guid, m_bus);
		}
	}
}

void DeviceWindow::handleAsyncUpdate()
{
	if (m_update & updt_device_change)
	{
		if (m_bus)
		{
			tcat::uint64 guid = m_devlist->getSelectedGuid();

			if (guid != 0)
			{
				updateShownDevice();
			}
			else
			{
				if (m_devlist->getNumRows() > 0)
				{
					m_devlist->selectRow(0);
				}
			}
		}
		m_update &= ~updt_device_change;
	}
	if (m_update & updt_mixers_created)
	{
		m_update &= ~updt_mixers_created;
		update_sync();
	}
	if (m_update & updt_sync_clock)
	{
		// ADAT compatible settings?
		if ((tcat::dice::kSyncAdat == m_sync) && (2 == m_clock))
		{
			// unmute and enable any ADAT channels
		}
		else
		{
			// mute and disable any ADAT channels
		}

		// in high rate modes only two stereo mixers are supported
		if (m_num_mixers > 2)
		{
			if (m_bus)
			{
				uint32 clock = m_bus->clock();
				bool bIsHighRate = (clock >= tcat::dice::kClock176);
				for (int i=2; i<m_num_mixers; i++)
				{
					m_devmixer[i]->forceDirectOut(bIsHighRate);
				}
			}
		}

		m_update &= ~updt_sync_clock;
	}
}

void DeviceWindow::changeListenerCallback (ChangeBroadcaster* source)
{
	Blender * bl = dynamic_cast<Blender *>(source);
	change_msg msg;

	bl->getChangeMessage(&msg);

	int mixer_id = msg.mixer_id;
	String nameStr(msg.str);
	int ch_id = msg.ch_id;
	Colour tabColour(Colours::grey);
	tcat::uint64 guid = 0;
	event_device_ref devref = 0;

	switch (msg.type)
	{
	case kAddMixer:
		// instantiate a new mixer, and attach it to the event device
		m_devmixer[mixer_id] = new DeviceMixerComponent(this, nameStr, mixer_id);
		// mixer component wants change messages from the blender
		bl->addChangeListener(this);

		tabColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);
		m_tabs->addTab (nameStr, tabColour, m_devmixer[mixer_id], true);
		guid = m_devlist->getSelectedGuid();
		if (m_bus)
		{
			devref = m_bus->find(guid);
			if (devref)
			{
				devref->attach(m_devmixer[mixer_id]);
			}
		}
		m_num_mixers++;
		break;

	case kRemoveMixers:
		for (int i=0; i<m_num_mixers; i++)
		{
			if (m_tabs->getNumTabs() >= m_tabs->m_first_mixer_index)
			{
				m_tabs->removeTab(m_tabs->m_first_mixer_index);
			}
		}
		for (int id=0; id<MIX_MAX_NUM_MIXERS; id++)
		{
			m_devmixer[id] = 0;
		}
		m_num_mixers = 0;
		m_tabs->setCurrentTabIndex(0);
		break;

	case kAddChannel:
		// instantiate the new channel
		m_devmixer[mixer_id]->addChannel(ch_id, nameStr);
		break;

	case kChannelPos:
		// set the x position for the channel
		m_devmixer[mixer_id]->setChannelPos(ch_id, msg.int_val1);
		break;

	case kMasterPos:
		// set the x position for the master out channel
		m_devmixer[mixer_id]->setMasterPos(msg.int_val1);
		break;

	case kAddGroupLabel:
		// add a new chanel grouping label with x position
		m_devmixer[mixer_id]->addGroupLabel(msg.int_val1, nameStr);
		break;

	case kMixersCreated:
		m_update |= updt_mixers_created;
		triggerAsyncUpdate();
		break;

	default:
		break;
	}
}

void DeviceWindow::actionListenerCallback (const String &message)
{
	//   display control settings for the currently selected device
	if (0 == message.compare("selected device changed"))
	{
		m_update |= updt_device_change;
		triggerAsyncUpdate();
	}
}

void DeviceWindow::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	if (m_bus)
	{
		m_bus->attach(m_devlist);
	}
}

void DeviceWindow::unmount()
{
	bus_listener::unmount();
}

void DeviceWindow::update_devicelist()
{
	m_update |= updt_device_change;
	triggerAsyncUpdate();		// notify the gui thread
}

void DeviceWindow::update_sync()
{
	if (m_bus)
	{
		m_sync = m_bus->sync();		// cache the sync source
		m_clock = m_bus->clock();	// cache the clock source
		m_update |= updt_sync_clock;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void DeviceWindow::update_clock()
{
	if (m_bus)
	{
		m_sync = m_bus->sync();		// cache the sync source
		m_clock = m_bus->clock();	// cache the clock source
		m_update |= updt_sync_clock;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

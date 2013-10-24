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

#include "Listener.h"
#include "BusWindow.h"
#include "BusGeneralComponent.h"
#include "BinaryData.h"
#include "tcat_dice_pal_device.h"
#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"
#include "EventLogger.h"


//==============================================================================
// this component listens for device locked notifications from the bus clock master
LockStatusComponent::LockStatusComponent() : 
	m_locked (false)
{
	m_warning_icon = (Drawable::createFromImageData (BinaryData::warning_png, BinaryData::warning_pngSize));
	m_unlock_icon = (Drawable::createFromImageData (BinaryData::not_locked_svg, BinaryData::not_locked_svgSize));
	
	addActionListener(this);
}

LockStatusComponent::~LockStatusComponent()
{
	if (m_warning_icon) delete(m_warning_icon);
	if (m_unlock_icon) delete(m_unlock_icon);
}

void LockStatusComponent::lookAndFeelChanged()
{
	repaint();
}

void LockStatusComponent::paint (Graphics& g)
{
	Path p;

	if (m_device)
	{
		if (!m_locked)
		{
			p.addRoundedRectangle(2.0f, 6.0f, 80.0f, 26, 3);
			g.setColour (findColour(TCATLookAndFeel::tcatDeviceBackgroundColourId));
			g.fillPath(p);
			g.setColour (findColour(TCATLookAndFeel::tcatDeviceOutlineColourId));
			g.strokePath(p, PathStrokeType (2.0f));
			m_warning_icon->drawWithin (g, Rectangle<float>(13, 9, 20, 20), RectanglePlacement::centred, 1.0f);
			g.setOpacity (0.8f);
			m_unlock_icon->drawWithin (g, Rectangle<float>(50, 11, 16, 16), RectanglePlacement::centred, 1.0f);

			g.setColour(findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId).contrasting());
			g.drawFittedText ("Unlocked", 90, 7, 75, 24, Justification::left, 1);
		}
		else
		{
			g.setColour(findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId).contrasting());
			g.drawFittedText ("System clock is Locked", 10, 7, 130, 24, Justification::left, 1);
		}
	}
}

void LockStatusComponent::resized()
{
}

void LockStatusComponent::attachToMaster(event_bus_ref& busref)
{
	// no longer want events from the old device
	if (m_device)
	{
		m_device->detach(this);
	}

	// find the new bus clock master and attach
	event_device_ref devref = busref->find(busref->master());
	if (devref)
	{
		devref->attach(this);
	}
	
	// set initial value
	if (m_device)
	{
		update_locked();
	}
}

void LockStatusComponent::mount(tcat::dice::reference<event_device> device)
{
	// call super's mount() first
	device_listener::mount(device);
	// which gives us an m_device reference
	if (m_device)
	{
		update_locked();
	}
}

void LockStatusComponent::unmount()
{
	// call super's unmount first
	device_listener::unmount();
}

void LockStatusComponent::update_locked()
{
	if (m_device)
	{
		m_locked = m_device->locked();
		setTooltip(m_locked ? String::empty : "The system is not locked because the master device is not locked. Verify that the Sync Source is valid.");
		sendActionMessage ("repaint");
	}
}

void LockStatusComponent::actionListenerCallback(const String &message)
{
	if (0 == message.compare("repaint"))
	{
		repaint();
	}
}


//==============================================================================
// this component contains the subcomponents that manage general bus settings
BusGeneralComponent::BusGeneralComponent () :
	m_rate ((tcat::dice::CLOCK)0),
	m_clockcaps ((tcat::dice::CLOCK_CAPS)0),
	m_sync ((tcat::dice::SYNC) 0),
	m_synccaps ((tcat::dice::SYNC_CAPS)0),
	m_iosize (0),
	m_min_iosize (0),
	m_max_iosize (0),
	m_master (0),
	m_locked (0),
	m_master_nickname (String::empty),
	m_num_clients (0),
	m_mode (0),
	m_update (0),
	m_rate_hide_if_single (false),
	m_sync_hide_if_single (false)
{
	setName ("Bus General");

	m_rate_cb = new ComboBox ("rate cb");
	addAndMakeVisible (m_rate_cb);
	m_rate_cb->setEditableText (false);
	m_rate_cb->setJustificationType (Justification::centred);
	m_rate_cb->addListener (this);
	m_rate_l = new Label (String::empty, "Sample Rate:");
	m_rate_l->attachToComponent (m_rate_cb, true);
    m_rate_hide_if_single = appDefaults->getAppSettings()->getBoolValue("hideRatesIfSingle");

	m_sync_cb = new ComboBox ("sync cb");
	addAndMakeVisible (m_sync_cb);
	m_sync_cb->setEditableText (false);
	m_sync_cb->setJustificationType (Justification::centred);
	m_sync_cb->addListener (this);
	m_sync_l = new Label (String::empty, "Sync Source:");
	m_sync_l->attachToComponent (m_sync_cb, true);
	m_sync_hide_if_single = appDefaults->getAppSettings()->getBoolValue("hideSyncsIfSingle");
    
#ifdef WIN32
	m_buffer_cb = new ComboBox ("buffer");
	addAndMakeVisible (m_buffer_cb);
	m_buffer_cb->setEditableText (false);
	m_buffer_cb->setJustificationType (Justification::centred);
	m_buffer_cb->addListener (this);
	m_buffer_l = new Label (String::empty, "Buffer Size:");
	m_buffer_l->attachToComponent (m_buffer_cb, true);

#endif // WIN32

    addAndMakeVisible (m_opmode_cb = new ComboBox ("OpModeComboBox"));
    m_opmode_cb->setEditableText (false);
    m_opmode_cb->setJustificationType (Justification::centred);
    m_opmode_cb->addItem ("Lowest Latency", 1);
    m_opmode_cb->addItem ("Normal Mode", 2);
    m_opmode_cb->addItem ("Safe Mode Level 1", 3);
    m_opmode_cb->addItem ("Safe Mode Level 2", 4);
    m_opmode_cb->addListener (this);
	m_opmode_l = new Label (String::empty, "Operation Mode:");
	m_opmode_l->attachToComponent (m_opmode_cb, true);
	
	m_master_cb = new ComboBox("selected master device");
	addAndMakeVisible (m_master_cb);
	
	m_master_cb->setEditableText (false);
	m_master_cb->setJustificationType (Justification::centred);
	m_master_cb->addListener (this);
	m_master_l = new Label (String::empty, "Master:");
	m_master_l->attachToComponent (m_master_cb, true);
	
	m_locked_l = new LockStatusComponent ();
	addAndMakeVisible (m_locked_l);

	m_clock_icon = (Drawable::createFromImageData (BinaryData::clock_mstr_svg, BinaryData::clock_mstr_svgSize));

	lookAndFeelChanged();
}

BusGeneralComponent::~BusGeneralComponent() 
{
	if (m_clock_icon) delete(m_clock_icon);
    deleteAllChildren();
}


void BusGeneralComponent::paint (Graphics& g)
{
	if (m_clock_icon)
	{
	    m_clock_icon->drawWithin (g, Rectangle<float>(14, 8, 25, 25), RectanglePlacement::centred, 1.0f);
	}
}

void BusGeneralComponent::resized()
{
	m_master_cb->setBounds (100, 8, 280, 24);
	m_rate_cb->setBounds (100, 36, 120, 24);
	m_sync_cb->setBounds (100, 64, 120, 24);

#ifdef WIN32
	m_buffer_cb->setBounds (515, 8, 145, 24);
#endif
	m_opmode_cb->setBounds (515, 36, 145, 24);

	m_locked_l->setBounds (230, 45, 150, 40);
}

void BusGeneralComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void BusGeneralComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId);

	repaint();

	m_rate_l->setColour (Label::backgroundColourId, backgroundColour);
	m_rate_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_sync_l->setColour (Label::backgroundColourId, backgroundColour);
	m_sync_l->setColour (Label::textColourId, backgroundColour.contrasting());
#ifdef WIN32
	m_buffer_l->setColour (Label::backgroundColourId, backgroundColour);
	m_buffer_l->setColour (Label::textColourId, backgroundColour.contrasting());
#endif
	m_opmode_l->setColour (Label::backgroundColourId, backgroundColour);
	m_opmode_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_master_l->setColour (Label::backgroundColourId, backgroundColour);
	m_master_l->setColour (Label::textColourId, backgroundColour.contrasting());
}

void BusGeneralComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
	if (m_bus)
	{
		if (comboBoxThatHasChanged==m_rate_cb)
		{
			const int rateIndex = m_rate_cb->getSelectedId();
            if (rateIndex > 0)
            {
                if (m_rate != (rateIndex-1))
                {
                    m_bus->clock(tcat::dice::CLOCK(rateIndex-1));
                }
            }
		}
		else if (comboBoxThatHasChanged==m_sync_cb)
		{
			const int syncIndex = m_sync_cb->getSelectedId();
            if(syncIndex > 0)
            {
                if (m_sync != (syncIndex-1))
                {
                    m_bus->sync(tcat::dice::SYNC(syncIndex-1));
                }
			}
		}
#ifdef WIN32
		else if (comboBoxThatHasChanged==m_buffer_cb)
		{
			uint32 bufSize = (uint32)m_buffer_cb->getText().getIntValue();
			if (m_iosize != bufSize)
			{
				try
				{
					m_bus->io_size(bufSize);
				}
				catch(...)
				{
					EventLogger::getInstance()->logMessage("  Attempt to set buffer size below minimum allowed value.");
					m_bus->update_io_size();
				}
			}
		}
#endif
		else if (comboBoxThatHasChanged==m_master_cb)
		{
			int nSelected = m_master_cb->getSelectedId();
			if (nSelected > 0)
			{
				if ((int)m_wwuids.size() >= (nSelected-1))
				{
					tcat::uint64 selected_guid = m_wwuids.at(nSelected-1);
					tcat::uint64 master_guid = m_bus->master();
					if (selected_guid != master_guid)
					{
						event_device_ref devref = m_bus->find (selected_guid);
						if (devref)
						{
							m_bus->master (selected_guid);
						}
					}
				}
			}

		}
		else if (comboBoxThatHasChanged == m_opmode_cb)
		{
            int nSelected = m_opmode_cb->getSelectedId();
            if (nSelected > 0)
            {
                if (m_bus->op_mode() != (nSelected-1))
                    m_bus->op_mode(tcat::dice::OP_MODE(nSelected-1));
            }
		}
	}
}

void BusGeneralComponent::actionListenerCallback(const String &message)
{
	if (0 == message.compare("nickname changed"))
	{
		// refresh names in master device combo
		if (m_bus)
		{
			update_devicelist();
		}
	}
}

// Async callback used for GUI updates
// Since the PAL uses locks and the message thread (this GUI thread) uses locks,
//  we use this asynchronous mechanism to avoid deadlocks.
//  The PAL callbacks have cached everything needed before triggering updates here
const int BusGeneralComponent::buff_sizes[num_buff_sizes] = {32, 48, 64, 96, 128, 192, 256, 384, 512, 768, 1024, 2048};
void BusGeneralComponent::handleAsyncUpdate()
{
	if (m_update & updt_unmount)
	{
		// bus is no longer available, clear and disable gui components
		m_update = 0;
		clearContent();
		setEnabled(false);
	}
	else if (m_bus)
	{
		// this object has just been attached, enable and populate component values
		if (m_update & updt_mount)
		{
			m_update &= ~updt_mount;
			setEnabled(true);
			updateContent();
		}
		
		// the list of named sync sources
		if (m_update & updt_syncnames)
		{
			m_sync_cb->clear();
			StringArray keys(m_syncnames_a.getAllKeys());
			
			for (int i=0; i<keys.size(); i++)
			{
				int itemId = (m_syncnames_a[keys[i]]).getIntValue() + 1;
				m_sync_cb->addItem(keys[i], itemId);
			}
			m_sync_cb->setSelectedId (m_sync+1,true);

			bool bShow = true;
			if (m_sync_cb->getNumItems() <= 1)
			{
				bShow = !m_sync_hide_if_single;
			}
			m_sync_cb->setVisible(bShow);
			m_sync_l->setVisible(bShow);

			m_update &= ~updt_syncnames;
		}
		
		// the current selected sync source
		if (m_update & updt_sync)
		{
			m_sync_cb->setSelectedId(m_sync+1, true);

			m_update &= ~updt_sync;
		}
		
		// the list of available sync sources
		if (m_update & updt_synccaps)
		{
			bool bShow = true;
			if (m_sync_cb->getNumItems() <= 1)
			{
				bShow = !m_sync_hide_if_single;
			}
			m_sync_cb->setVisible(bShow);
			m_sync_l->setVisible(bShow);

			m_update &= ~updt_synccaps;
		}
		
		// the list of available sample rates
		if (m_update & updt_clock_caps)
		{
			m_rate_cb->clear();				
			StringArray keys(m_clocknames_a.getAllKeys());
			for (int i=0; i<keys.size(); i++)
			{
				int itemId = (m_clocknames_a[keys[i]]).getIntValue() + 1;
				m_rate_cb->addItem(keys[i], itemId);
			}
			m_rate_cb->setSelectedId (m_rate+1,true);

			bool bShow = true;
			if (m_rate_cb->getNumItems() <= 1)
			{
				bShow = !m_rate_hide_if_single;
			}
			m_rate_cb->setVisible(bShow);
			m_rate_l->setVisible(bShow);
//			m_locked_l->setVisible(bShow);

			m_update &= ~updt_clock_caps;
		}
		
		// the nominal sample rate
		if (m_update & updt_clock)
		{
			m_rate_cb->setSelectedId(m_rate+1, true);

			m_update &= ~updt_clock;
		}
		
		// the list of bus devices
		if (m_update & updt_devicelist)
		{
			if (0 == m_bus->size())
			{
				m_rate_cb->setText(String::empty);
				m_sync_cb->setText(String::empty);
				m_master_cb->clear();
				m_opmode_cb->setText(String::empty);
#ifdef WIN32
				m_buffer_cb->setText(String::empty);
#endif
			}
			else
			{
				// update the master combobox with names found in the WWUID+nickname map
				m_master_cb->clear();
				for (int i=0; i<(int)m_names.size();i++)
				{
					m_master_cb->addItem (m_names.at(i).c_str(),i+1);
				}

				update_clock();
				update_clock_caps();
				update_sync_names();
				update_sync_caps();
				update_sync();
				update_master();
				update_op_mode();
#ifdef WIN32
				update_io_size();
				update_min_io_size();
#endif
			}

			m_update &= ~updt_devicelist;
		}
		
#ifdef WIN32
		// the buffer size
		if (m_update & updt_iosize)
		{
			String ioStr;
			ioStr << String::formatted ("%i", m_iosize);
			m_buffer_cb->setText (ioStr, true);	

			m_update &= ~updt_iosize;
		}

		// the maximum/minimum buffer size
		if ((m_update & updt_min_iosize) || (m_update & updt_max_iosize))
		{
			if ((m_min_iosize>0) && (m_max_iosize>0))
			{
				m_buffer_cb->clear();

				int size_mult = 0;
				if (m_rate <= tcat::dice::kClock48)
				{
					size_mult = 1;
				}
				else if (m_rate <= tcat::dice::kClock96)
				{
					size_mult = 2;
				}
				else
				{
					size_mult = 4;
				}

				for (int i=0; i<num_buff_sizes; i++)
				{
					tcat::uint32 size = buff_sizes[i]*size_mult;
					if ((size >= m_min_iosize) && (size <= m_max_iosize))
					{
						m_buffer_cb->addItem (String::formatted ("%i", size), size);
					}
				}
				m_buffer_cb->setSelectedId(m_iosize);
				String ioStr;
				ioStr << String::formatted ("%i", m_iosize);
				m_buffer_cb->setText (ioStr, true);	
			}

			if (m_update & updt_min_iosize)
			{
				m_update &= ~updt_min_iosize;
			}
			else if (m_update & updt_max_iosize)
			{
				m_update &= ~updt_max_iosize;
			}
		}
#endif // WIN32

		// the bus' clock master device
		if (m_update & updt_master)
		{
			m_master_cb->setSelectedItemIndex(-1,false);
			m_master_cb->setTooltip("Unknown.");

			for (int i=0; i<(int)m_wwuids.size();i++)
			{
				if (m_master == m_wwuids[i])
				{
					m_master_cb->setSelectedId(i+1,true);
					tcat::uint32 guidHi = (tcat::uint32)(m_master>>32) & 0xffffffff;
					tcat::uint32 guidLo = (tcat::uint32)m_master & 0xffffffff;
					m_master_cb->setTooltip(String::formatted("Unique Id: %08X% 08X", guidHi, guidLo));
				}
			}
			
			m_update &= ~updt_master;
		}

		if (m_update & updt_opmode)
		{
			if (0 < m_bus->size())
			{
				m_opmode_cb->setSelectedId(m_mode+1, true);
			}
			m_update &= ~updt_opmode;
		}

		String lockStr = String::empty;
		lockStr << "This setting is disabled while there are open Audio Applications using the device.";
		bool bLock = false;
		
		// the restriction on rate changes has changed
		if (m_update & updt_rate_readonly)
		{
			bLock = m_bus->rate_readonly();
			m_rate_cb->setEnabled(!bLock);
			m_rate_cb->setTooltip(bLock ? lockStr : String::empty);
			
			String logStr = "sample rate changes are ";
			if (bLock) logStr << "not ";
			logStr << "allowed.";
			EventLogger::getInstance()->logMessage(logStr);

			m_update &= ~updt_rate_readonly;
		}

		if (m_update & updt_sync_readonly)
		{
			bLock = m_bus->sync_readonly();
			m_sync_cb->setEnabled(!bLock);
			m_sync_cb->setTooltip(bLock ? lockStr : String::empty);

			String logStr = "sync source changes are ";
			if (bLock) logStr << "not ";
			logStr << "allowed.";
			EventLogger::getInstance()->logMessage(logStr);

			m_update &= ~updt_sync_readonly;
		}

		if (m_update & updt_master_readonly)
		{
			bLock = m_bus->master_readonly();
			m_master_cb->setEnabled(!bLock);
			m_master_cb->setTooltip(bLock ? lockStr : String::empty);

			String logStr = "master device changes are ";
			if (bLock) logStr << "not ";
			logStr << "allowed.";
			EventLogger::getInstance()->logMessage(logStr);

			m_update &= ~updt_master_readonly;
		}

#ifdef WIN32
		if (m_update & updt_io_size_readonly)
		{
			bLock = m_bus->io_size_readonly();
			m_buffer_cb->setEnabled(!bLock);
			m_buffer_cb->setTooltip(bLock ? lockStr : String::empty);

			String logStr = "buffer size changes are ";
			if (bLock) logStr << "not ";
			logStr << "allowed.";
			EventLogger::getInstance()->logMessage(logStr);

			m_update &= ~updt_io_size_readonly;
		}
#endif

		if (m_update & updt_op_mode_readonly)
		{
			bLock = m_bus->op_mode_readonly();
			m_opmode_cb->setEnabled(!bLock);
			m_opmode_cb->setTooltip(bLock ? lockStr : String::empty);

			String logStr = "operation mode changes are ";
			if (bLock) logStr << "not ";
			logStr << "allowed.";
			EventLogger::getInstance()->logMessage(logStr);

			m_update &= ~updt_op_mode_readonly;
		}

		// the restriction on nickname changes has changed
		if (m_update & updt_nickname_readonly)
		{
			bLock = m_bus->nickname_readonly();

			String logStr = "nickname changes are ";
			if (bLock) logStr << "not ";
			logStr << "allowed.";
			EventLogger::getInstance()->logMessage(logStr);

			// tell other interested objects (nickname)
			sendActionMessage (bLock ? "clients open" : "clients closed");

			m_update &= ~updt_nickname_readonly;
		}
	}
}

void BusGeneralComponent::updateContent()
{
	update_clock();
	update_clock_caps();
	update_sync_names();
	update_sync_caps();
	update_sync();
	update_devicelist();
	update_master();
	update_op_mode();
	update_rate_readonly();
	update_sync_readonly();
	update_master_readonly();
	update_io_size_readonly();
	update_op_mode_readonly();
	update_nickname_readonly();
#ifdef WIN32
	update_io_size();
	update_min_io_size();
#endif
}

void BusGeneralComponent::clearContent()
{
	m_rate_cb->clear();
	m_rate_cb->setText(String::empty);
	m_sync_cb->clear();
	m_sync_cb->setText(String::empty);
	m_master_cb->clear();
	m_master_cb->setText(String::empty);
	m_master_cb->setTooltip("When a Master is selected, hover here to see its bus id.");
	m_opmode_cb->setText(String::empty);
#ifdef WIN32
	m_buffer_cb->setText(String::empty);
#endif
}

void BusGeneralComponent::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	if (m_bus)
	{
		m_update |= updt_mount;	// notification for gui thread
		update_devicelist();	// this will trigger the gui thread notification
	}
}

void BusGeneralComponent::unmount()
{
	// call super's umnount first
	bus_listener::unmount();
	m_update |= updt_unmount;
	triggerAsyncUpdate();
}

void BusGeneralComponent::update_clock()
{
	if (m_bus)
	{
		m_rate = m_bus->clock();	// cache the current rate
		m_update |= updt_clock;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_clock_caps()
{
	if (m_bus)
	{
		m_rate = m_bus->clock();			// cache the current clock rate
		m_clockcaps = m_bus->clock_caps();	// cache the current clock rate caps
		// cache the list of nominal sample rates
		m_clocknames_a.clear();
		for (int clock=tcat::dice::kClock32; clock<=tcat::dice::kClock192; clock++)
		{
			if (m_clockcaps & (1<<clock))
			{
				String clockStr = m_bus->clock_name((tcat::dice::CLOCK)clock).c_str();
				if (0 != clockStr.compare(String::empty))
				{
					m_clocknames_a.set(clockStr, String::formatted("%i", clock) );
				}
			}
		}
		m_update |= updt_clock_caps;
		triggerAsyncUpdate();				// notify the gui thread
	}
}

void BusGeneralComponent::update_sync()
{
	if (m_bus)
	{
		m_sync = m_bus->sync();		// cache the sync source
		m_update |= updt_sync;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_sync_names()
{
	if (m_bus)
	{
		// cache the list of available sync sources
		// also get current sync source setting here to guarantee that the 
		//  sync value is current for the GUI notification handler
		m_sync = m_bus->sync();

		tcat::dice::SYNC_CAPS sync_caps = m_bus->sync_caps();
		m_syncnames_a.clear();
		for (int sync=tcat::dice::kSyncAes0; sync<tcat::dice::kSyncItems; sync++)
		{
			if (sync_caps & (1<<sync))
			{
				String syncStr = m_bus->sync_name((tcat::dice::SYNC)sync).c_str();
				if (0 != syncStr.compare(String::empty))
				{
					m_syncnames_a.set(syncStr, String::formatted("%i",sync) );
				}
			}
		}
		m_update |= (updt_syncnames | updt_sync);
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_sync_caps()
{
	if (m_bus)
	{
		m_sync = m_bus->sync();
	
		m_update |= (updt_synccaps);
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_devicelist()
{
	if (m_bus)
	{
		// cache a list of nicknames for the devices on this bus
		m_wwuids.clear();
		m_names.clear();
		for (int i=0; i<(int)m_bus->size();i++)
		{
			m_wwuids.push_back(m_bus->at(i)->guid64());
			m_names.push_back(m_bus->at(i)->nickname().c_str());
		}
		
		// update master as well
		m_master = m_bus->master();

		if (m_master)
		{
			event_device_ref devref = m_bus->find(m_master);
			if (devref)
			{
				for (int i=0; i<(int)m_wwuids.size(); i++)
				{
					if (m_wwuids[i] == m_master)
					{
						m_master_nickname = m_names[i].c_str();
					}
				}
			}
			else
			{
				m_master_nickname = String::empty;
			}
		}	
		m_update |= (updt_devicelist | updt_master);
		triggerAsyncUpdate();		// notify the gui thread
	}
}

#ifdef WIN32
void BusGeneralComponent::update_io_size()
{
	if (m_bus)
	{
		// cache the current buffer size
		m_iosize = m_bus->io_size();
		m_min_iosize = m_bus->min_io_size();
		m_max_iosize = m_bus->max_io_size();
		m_update |= updt_iosize;
		triggerAsyncUpdate();		// notify the gui thread
	}	
}

void BusGeneralComponent::update_min_io_size()
{
	if (m_bus)
	{
		// cache the current buffer size
		m_iosize = m_bus->io_size();
		m_min_iosize = m_bus->min_io_size();
		m_max_iosize = m_bus->max_io_size();
		m_update |= updt_min_iosize;
		m_update |= updt_iosize;
		triggerAsyncUpdate();		// notify the gui thread
	}	
}

void BusGeneralComponent::update_max_io_size()
{
	if (m_bus)
	{
		// cache the current buffer size
		m_iosize = m_bus->io_size();
		m_min_iosize = m_bus->min_io_size();
		m_max_iosize = m_bus->max_io_size();
		m_update |= updt_max_iosize;
		m_update |= updt_iosize;
		triggerAsyncUpdate();		// notify the gui thread
	}	
}
#endif // WIN32

void BusGeneralComponent::update_master()
{
	if (m_bus)
	{
		// associate the lock state label with the new master device
		m_locked_l->attachToMaster(m_bus);

		// cache the master
		m_master = m_bus->master();
		if (m_master)
		{
			event_device_ref devref = m_bus->find(m_master);
			if (devref)
			{
				for (int i=0; i<(int)m_wwuids.size(); i++)
				{
					if (m_wwuids[i] == m_master)
					{
						m_master_nickname = m_names[i].c_str();
					}
				}
			}
			else
			{
				m_master_nickname = String::empty;
			}
		}
		m_update |= updt_master;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_op_mode()
{
	if (m_bus)
	{
		// cache the operation mode
		m_mode = m_bus->op_mode();
		m_update |= updt_opmode;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_rate_readonly()
{
	if (m_bus)
	{
		m_update |= updt_rate_readonly;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_sync_readonly()
{
	if (m_bus)
	{
		m_update |= updt_sync_readonly;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_master_readonly()
{
	if (m_bus)
	{
		m_update |= updt_master_readonly;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_io_size_readonly()
{
	if (m_bus)
	{
		m_update |= updt_io_size_readonly;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_op_mode_readonly()
{
	if (m_bus)
	{
		m_update |= updt_op_mode_readonly;
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusGeneralComponent::update_nickname_readonly()
{
	if (m_bus)
	{
		m_update |= updt_nickname_readonly;
		triggerAsyncUpdate();		// notify the gui thread
	}
}


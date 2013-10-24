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

#include "DeviceList.h"
#include "BinaryData.h"
#include "juce_TCATLookAndFeel.h"
#include "EventLogger.h"

// ListBox item
class DeviceRowComponent : 
	public Component,
	public SettableTooltipClient,
	public device_listener,
	public ActionBroadcaster,
	private AsyncUpdater,
	private Timer
{
public:
    DeviceRowComponent (DeviceListComponent& owner_, int rowNumber_) :
		m_owner (owner_),
		m_row (rowNumber_),
		m_nickname (String::empty),
		m_guid (0),
		m_master (0),
		m_selected (false),
		m_locked (false),
		m_state (tcat::dice::DEVICE_INFO::DEVICE_STATE_REMOVED),
		m_countdown (0),
		m_dev_image (0),
		m_lock_icon (0),	
		m_unlock_icon (0),
		m_master_icon (0),
		m_warning_icon (0),
		m_info_icon (0)
    {
		loadIcons();
		addMouseListener(this, true);
    }


	void mouseUp(const MouseEvent &e)
	{
		if (e.mouseWasClicked() && e.mods.isShiftDown())
		{
			EventLogWindow::getInstance()->showWindow(true);
		}
	}

	void paint(Graphics &g)
	{
		int h = getHeight();
		int w = getWidth();
		Path p;

		// fill list element with underlying color
		g.setColour (findColour(TCATLookAndFeel::tcatDeviceAreaBackgroundColourId));
		g.fillRoundedRectangle (3.0f,4.0f,(float)w-7.0f,(float)h-9.0f, 3.0f);

		// listbox background
		g.setColour(findColour(TCATLookAndFeel::tcatDeviceBackgroundColourId));
		if (!m_selected)
		{
			g.setOpacity(0.4f);
		}
		g.fillRoundedRectangle (3.0f,4.0f,(float)w-7.0f,(float)h-9.0f, 3.0f);
		g.setOpacity(1.0f);

		// listbox outline
		g.setColour (findColour(TCATLookAndFeel::tcatDeviceOutlineColourId));
		if (!m_selected)
		{
			g.setOpacity (0.4f);
		}
		p.clear();
		p.addRoundedRectangle(2.0f, 3.0f, (float)(w-5), (float)(h-7), 3.0f);
		g.strokePath (p, PathStrokeType (4.0f));
		g.setOpacity (1.0f);
		g.setColour (Colours::black);
		
		// images
		if (m_device)
		{
			// device icon
			if (m_dev_image)
			{
				if (tcat::dice::DEVICE_INFO::DEVICE_STATE_USER_DISABLED == m_state)
				{
					g.setOpacity(0.10f);
				}
				else if (!m_selected)
				{
 					g.setOpacity (0.5f);
				}
				m_dev_image->drawWithin (g, Rectangle<float>(10, 10, 40, 40), RectanglePlacement::centred, 1.0f);
				if (tcat::dice::DEVICE_INFO::DEVICE_STATE_USER_DISABLED == m_state)
				{
					g.setColour(Colours::red);
					g.drawFittedText ("disabled",
								10, 10, 40, 40,
								Justification::centred, 1, 0.7f);
					g.setOpacity(1.0);
					g.setColour(Colours::black);
				}
			}

			// warning indicator badge
			if (m_countdown > 0)
			{
				if (m_countdown > 30)	// show the warning indicator
				{
					if (!m_selected)
					{
 						g.setOpacity (0.5f);
					}
					m_warning_icon->drawWithin (g, Rectangle<float>(15, 25, 20, 20), RectanglePlacement::centred, 1.0f);
				}
				else					// show a fading info indicator
				{
					g.setOpacity(0.5f - ((30-m_countdown)/60.0f));
					m_info_icon->drawWithin (g, Rectangle<float>(15, 25, 20, 20), RectanglePlacement::centred, 1.0f);
				}
				g.setOpacity(1.0f);
			}

			// nickname area
			//   nickname background
			p.clear();
			p.addRoundedRectangle(56.0f, 9.0f, (float)(w-96), (float)(h-18), 3.0f);
			g.setColour(Colours::ivory);
			if (!m_selected)
			{
				g.setOpacity (0.50f);
			}
			g.fillPath(p);

			//   nickname outline
			g.setColour (Colours::lightgrey);
			g.strokePath (p, PathStrokeType (1.0f));

			//   nickname text
			if (m_selected)
			{
				g.setColour (Colours::black);
			}
			else
			{
				g.setColour (Colours::grey);
			}
			g.drawFittedText (m_nickname,
						59, 10, w-102, h-20,
						Justification::centred, 3, 0.7f);

            //   master indicator						
			if (!m_selected)
			{
				g.setOpacity (0.75f);
			}
			if (m_guid == m_owner.m_master)
			{
				m_master_icon->drawWithin (g, Rectangle<float>((float)w-33, 8, 23, 23), RectanglePlacement::centred, 1.0f);
			}
			
			//   lock indicator
			if (!m_selected)
			{
				g.setOpacity (0.50f);
			}
			if (m_locked)
			{
				m_lock_icon->drawWithin (g, Rectangle<float>((float)w-31, 33, 18, 18), RectanglePlacement::centred, 1.0f);
			}
			else
			{
				m_unlock_icon->drawWithin (g, Rectangle<float>((float)w-31, 33, 18, 18), RectanglePlacement::centred, 1.0f);
			}
		}
		else
		{
			g.drawFittedText ("Unable to retrieve device info.",
						50, 10, w-100, h-20,
						Justification::centred, 3, 0.7f);
		}
	}
	
    ~DeviceRowComponent()
    {
		if (m_dev_image) delete m_dev_image;
		if (m_lock_icon) delete m_lock_icon;
		if (m_unlock_icon) delete m_unlock_icon;
		if (m_master_icon) delete m_master_icon;
		if (m_warning_icon) delete m_warning_icon;
		if (m_info_icon) delete m_info_icon;
	}
	
	void loadIcons ()
	{
		m_dev_image = (Drawable::createFromImageData (BinaryData::dev_1394_svg, BinaryData::dev_1394_svgSize));
		m_lock_icon = (Drawable::createFromImageData (BinaryData::locked_svg, BinaryData::locked_svgSize));
		m_unlock_icon = (Drawable::createFromImageData (BinaryData::not_locked_svg, BinaryData::not_locked_svgSize));
		m_master_icon = (Drawable::createFromImageData (BinaryData::clock_mstr_svg, BinaryData::clock_mstr_svgSize));
		m_warning_icon = (Drawable::createFromImageData (BinaryData::warning_png, BinaryData::warning_pngSize));
		m_info_icon = (Drawable::createFromImageData (BinaryData::info_png, BinaryData::info_pngSize));
	}
	
	void mouseDown(const MouseEvent &e)
	{
		m_owner.selectRowsBasedOnModifierKeys(m_row, e.mods, false);
	}
	
	void setSelected(bool isSelected)
	{
		m_selected = isSelected;
		setTooltip(m_selected ? "Shift+Click to see event log." : String::empty);
	}
	
	void handleAsyncUpdate()
	{
		repaint();
	}
	
	void setStatus(const String& ttStr, bool startWarning)
	{
		EventLogger::getInstance()->logMessage(m_nickname + " : " + ttStr);
		setTooltip(getTooltip() + "\n" + Time::getCurrentTime().toString (false, true) + " : " + ttStr);
		if (startWarning)
		{
			m_countdown = 40;
			startTimer(200);
		}
	}

	void timerCallback ()
	{
		// use delay on the warning indicator
		if (m_countdown-- <= 0)
		{
			setTooltip(m_selected ? "Shift+Click to see event log." : String::empty);
			stopTimer();
		};
		repaint();
	}

	void mount(tcat::dice::reference<event_device> device)
	{
		// call super's mount() first
		device_listener::mount(device);
		// which gives us an m_device reference
		if (m_device)
		{
			m_guid = m_device->guid64();
			m_nickname = m_device->nickname().c_str();
 			m_locked = m_device->locked();
		}
	}
		
	void unmount()
	{
		// call super's unmount() first
		device_listener::unmount();
	}

	void update_nickname()
	{
		if (m_device)
		{
			m_nickname = m_device->nickname().c_str();
			try
			{
				triggerAsyncUpdate();
			}
			catch (...)
			{
				DBG("AsyncUpdater exception in update_nickname()");
			}
		}
	}

	void update_locked()
	{
		if (m_device)
		{
 			m_locked = m_device->locked();
			try
			{
				if (!m_locked)
				{
					setStatus("Device unlocked.", true);
				}
				else
				{
					setStatus("Device locked.", false);
				}
				triggerAsyncUpdate();
			}
			catch (...)
			{
				DBG("AsyncUpdater exception in update_nickname()");
			}
		}
	}

	void update_rx()
	{
		if (m_device)
		{
			sendActionMessage("update rx");
		}
	}
	
	void update_tx()
	{
		if (m_device)
		{
			sendActionMessage("update tx");
		}
	}
 
	void update_rx_locked()
	{
		if (m_device)
		{
			tcat::dice::EX_STATUS rx_status = m_device->rx_locked();
			setStatus("device rx lock changed:", false);
			setStatus("arx4 arx3 arx2 arx1 tdif adat aes4 aes3 aes2 aes1", false);
			String statusStr(String::empty);

			for (int i=9; i>=0; i--)
			{
				if (rx_status & (1<<i))
					statusStr << "  L  ";
				else
					statusStr << "  U  ";
			}
			setStatus(statusStr, true);
			triggerAsyncUpdate();
		}
	}
	
	void update_state()
	{
		if (m_device)
		{
			tcat::dice::DEVICE_INFO::STATE state = m_device->state();

			if (tcat::dice::DEVICE_INFO::DEVICE_STATE_ACTIVE == state)
			{
				setStatus("Device state normal.", false);
			}
			else
			{
				String statusStr;
				switch (state)
				{
					case tcat::dice::DEVICE_INFO::DEVICE_STATE_USER_DISABLED:
						statusStr << "disabled";
						break;

					case tcat::dice::DEVICE_INFO::DEVICE_STATE_LOCK_FAILURE:
					case tcat::dice::DEVICE_INFO::DEVICE_STATE_ALREADY_LOCKED:
						statusStr << "locked by another host computer";
						break;

					case tcat::dice::DEVICE_INFO::DEVICE_STATE_LOW_RESOURCES:
						statusStr << "could not allocate bus resources";
						break;

					case tcat::dice::DEVICE_INFO::DEVICE_STATE_REMOVED:
						statusStr << "removed";
						break;

					default:
						statusStr << "Unknown error.";
						break;
				}
				setStatus("Device state changed: " + statusStr, true);
			}

			if (state != m_state)
			{
				m_state = state;
			}
		}
	}

	void update_user(tcat::dice::DEVICE_NOTIFICATION notification)
	{
		// uncomment this to view user notifications in the EventLog window.
//		String notificationHex = String::formatted("%08x", notification);
//		EventLogger::getInstance()->logMessage(m_nickname + " : device user notification: 0x" + notificationHex);
	}

	juce_UseDebuggingNewOperator

private:
    DeviceListComponent& m_owner;
	int m_row;
	String m_nickname;
	tcat::uint64 m_guid;
	tcat::uint64 m_master;
	bool m_selected;
	bool m_locked;
	tcat::dice::DEVICE_INFO::STATE m_state;
	int	m_countdown;
    Drawable * m_dev_image;
    Drawable * m_lock_icon;	
    Drawable * m_unlock_icon;
    Drawable * m_master_icon;
    Drawable * m_warning_icon;
    Drawable * m_info_icon;
};



DeviceListComponent::DeviceListComponent ()
	:	ListBox ("dev list", 0), m_num_rows (0), m_master (0)
{
	setModel (this);
    setMultipleSelectionEnabled (false);
	setOpaque(false);

	lookAndFeelChanged();
}

DeviceListComponent::~DeviceListComponent()
{
}

void DeviceListComponent::mouseUp(const MouseEvent &e)
{
	if (e.mouseWasClicked() && e.mods.isShiftDown())
	{
		EventLogWindow::getInstance()->showWindow(true);
	}
}

void DeviceListComponent::lookAndFeelChanged()
{
	repaint();

	setColour(ListBox::backgroundColourId, Colours::transparentBlack);
}

void DeviceListComponent::actionListenerCallback(const String &message)
{
	if (0 == message.compare("nickname changed"))
	{
		updateContent();
	}
//	else if ( (0 == message.compare("update rx")) || (0 == message.compare("update tx")) )
	else if (0 == message.compare("update rx"))	// firmware always sends rx and tx notifications together
	{
		sendActionMessage("update channels");
	}
}

void DeviceListComponent::handleAsyncUpdate()
{
	try
	{
		sendActionMessage ("selected device changed");
		updateContent();
	}
	catch (...)
	{
		DBG("device list exception: updateContent");
	}
}

// updating custom ListBox component
Component * DeviceListComponent::refreshComponentForRow(int rowNumber, bool isRowSelected, Component * existingComponentToUpdate)
{
	DeviceRowComponent* devRowBox = (DeviceRowComponent*) existingComponentToUpdate;

	// If an existing component is being passed-in for updating, we'll delete it and create a new one
	if (devRowBox)
	{
		devRowBox->removeActionListener(this);
		deleteAndZero(devRowBox);
	}

	if (m_bus)
	{
		if (rowNumber < (int)m_bus->size())
		{
			event_device_ref devref = m_bus->at(rowNumber);
			devRowBox = new DeviceRowComponent (*this, rowNumber);
			devRowBox->setSelected(isRowSelected);
			devref->attach(devRowBox);
			// the device list wants action messages from this device row item
			devRowBox->addActionListener(this);
		}
	}

	return devRowBox;
}

int DeviceListComponent::getNumRows()
{
	return m_num_rows;
}

// satisfy pure virtual member here
//   since we're using custom components for list items, 
//   refreshComponentForRow() is called by mouseDown event in the list Component
void DeviceListComponent::paintListBoxItem (int rowNumber,
                        Graphics& g,
                        int width, int height,
                        bool rowIsSelected)
{
}

// selection has changed, so tell the device view tab(s) about it
void DeviceListComponent::selectedRowsChanged(int lastRowSelected)
{
	sendActionMessage ("selected device changed");
}

tcat::uint64 DeviceListComponent::getSelectedGuid (void)
{
	tcat::uint64 guid = 0;
	int sel = getSelectedRow ();
	
	if (sel<0) 
	{
		return 0;
	}

	if (m_bus)
	{
		if (sel < (int)m_bus->size())
		{
			if (m_bus->at(sel))
				guid = m_bus->at(sel)->guid64();
		}
	}
	return guid;
}


void DeviceListComponent::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	if (m_bus)
	{
		// bus has changed, update device list
		update_devicelist();
		update_master();
	}
}

void DeviceListComponent::unmount()
{
	bus_listener::unmount();
	triggerAsyncUpdate();
}

void DeviceListComponent::update_master()
{
	if (m_bus)
	{
		m_master = m_bus->master();
		triggerAsyncUpdate();
	}
}

void DeviceListComponent::update_devicelist ()
{
	if (m_bus)
	{
		m_num_rows = (int)m_bus->size();
		triggerAsyncUpdate();
	}
}

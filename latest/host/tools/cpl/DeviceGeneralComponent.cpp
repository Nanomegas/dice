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

#include "DeviceGeneralComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "tcat_dice.h"
#include "EventLogger.h"
#include "appDefaults.h"

// this component contains the subcomponents that display device-related items
DeviceGeneralComponent::DeviceGeneralComponent() :
	m_guid (0),
	m_dev_type (String::empty),
	m_nickname (String::empty),
	m_state(tcat::dice::DEVICE_INFO::DEVICE_STATE_REMOVED),
	m_clock (0),
	m_Tx0Locked (false),
	m_Tx1Locked (false),
	m_lock_state (false)
{
    setName ("Device General");
    
	// type
    m_type = new TextEditor (String::empty);
	m_type->setMultiLine(true, true);
	m_type->setReadOnly(true);
    addAndMakeVisible (m_type);
	m_type_l = new Label(String::empty, "Device\ndescription:");        
	m_type_l->setJustificationType(Justification::centred);
    addAndMakeVisible (m_type_l);

	// nickname
	m_dev_nickname = new Label("Dev nickname", "(unknown)");        
    addAndMakeVisible (m_dev_nickname);
	m_dev_nickname->setJustificationType(Justification::centred);
    m_dev_nickname->setEditable(true, true, true);
	m_dev_nickname->addListener(this);
    m_dev_nickname_l = new Label ("Nickname label", "Nickname:");
	m_dev_nickname_l->setJustificationType(Justification::right);
    addAndMakeVisible (m_dev_nickname_l);
	
	// device status
    m_status = new Label(String::empty, String::empty);
	m_status->setJustificationType(Justification::centred);
    addAndMakeVisible (m_status);
    m_status_l = new Label (String::empty, "Status:");
	m_status_l->setJustificationType(Justification::right);
    addAndMakeVisible (m_status_l);
	m_status_ex = new Label(String::empty, String::empty);
    addAndMakeVisible (m_status_ex);

	m_show_measured_sr = appDefaults->getAppSettings()->getBoolValue("showMeasuredSampleRate");
	
	m_rate = new Label(String::empty, String::empty);
	m_rate->setJustificationType(Justification::centred);
    addAndMakeVisible (m_rate);
	m_rate_l = new Label (String::empty, String::empty);
    if (m_show_measured_sr)
	{
		m_rate_l->setText("Measured Sampling Rate:", false);
	}
	else
	{
		m_rate_l->setText("Device clock:", false);
	}
	m_rate_l->setJustificationType(Justification::right);
    addAndMakeVisible (m_rate_l);

	lookAndFeelChanged();
}

DeviceGeneralComponent::~DeviceGeneralComponent()
{
	deleteAllChildren();
}

void DeviceGeneralComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void DeviceGeneralComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);

	repaint();

	m_type->setColour (TextEditor::backgroundColourId, backgroundColour);
	m_type->setColour (TextEditor::textColourId, backgroundColour.contrasting());
	m_type->setColour (TextEditor::outlineColourId, findColour(TextEditor::backgroundColourId));
	m_type_l->setColour (Label::backgroundColourId, backgroundColour);
	m_type_l->setColour (Label::textColourId, backgroundColour.contrasting());

	m_dev_nickname->setColour(Label::backgroundColourId, findColour(TCATLookAndFeel::tcatTextboxBackgroundColourId));
	m_dev_nickname->setColour(Label::outlineColourId, findColour(TextEditor::backgroundColourId));
	m_dev_nickname->setColour(Label::textColourId, findColour(TCATLookAndFeel::tcatTextboxBackgroundColourId).contrasting());
	m_dev_nickname_l->setColour (Label::backgroundColourId, backgroundColour);
	m_dev_nickname_l->setColour (Label::textColourId, backgroundColour.contrasting());

	m_rate->setColour (Label::backgroundColourId, backgroundColour);
	m_rate->setColour (Label::textColourId, backgroundColour.contrasting());
	m_rate->setColour (Label::outlineColourId, findColour(TextEditor::backgroundColourId));
	m_rate_l->setColour (Label::backgroundColourId, backgroundColour);
	m_rate_l->setColour (Label::textColourId, backgroundColour.contrasting());

	m_status->setColour (Label::backgroundColourId, backgroundColour);
	m_status->setColour (Label::textColourId, backgroundColour.contrasting());
	m_status->setColour (Label::outlineColourId, findColour(TextEditor::backgroundColourId));
 	m_status_l->setColour (Label::backgroundColourId, backgroundColour);
	m_status_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_status_ex->setColour (Label::backgroundColourId, backgroundColour);
	m_status_ex->setColour (Label::textColourId, backgroundColour.contrasting());
}

void DeviceGeneralComponent::handleAsyncUpdate()
{
	updateContent();
}

void DeviceGeneralComponent::setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus)
{
	m_guid = guid;

	if (m_device)
	{
		m_device->detach(this);
	}

	event_device_ref devref = bus->find(guid);
	if (devref) devref->attach(this);
}

 void DeviceGeneralComponent::resized()
 {
    m_type->setBounds (120,25,300,48);
    m_type_l->setBounds (20,25,100,48);

    m_dev_nickname->setBounds (120,90,300,24);
    m_dev_nickname_l->setBounds (10,90,100,24);

    m_rate->setBounds (225,130,80,24);
    m_rate_l->setBounds (20,130,195,24);

    m_status->setBounds (225,160,80,24);
    m_status_l->setBounds (20,160,195,24);

    m_status_ex->setBounds (10,190,getWidth()-20,getHeight()-200);
}

//==============================================================================
    
void DeviceGeneralComponent::updateContent (void)
{
	if (m_device)
	{
		setEnabled(true);

		// device type
		m_type->setText(m_dev_type, false);
		
		// nickname
		m_dev_nickname->setText(m_nickname, false);

		// measured sample rate or device clock state
		String rateStr = String::empty;

		if (m_show_measured_sr)
		{
			if (m_clock)
			{
				rateStr = String::formatted("%i Hz", m_clock);
			}
		}
		else
		{
			rateStr = m_lock_state ? "Locked" : "Not locked";
		}
		m_rate->setText(rateStr, false);

		// device state
		String statusStr;
		switch (m_state)
		{
			case tcat::dice::DEVICE_INFO::DEVICE_STATE_ACTIVE:
				statusStr << "normal";
				break;

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
		m_status->setText(statusStr, false);

		if (tcat::dice::DEVICE_INFO::DEVICE_STATE_LOW_RESOURCES == m_state)
		{
			m_status_ex->setFont(Font(12));
			String ErrStr = String::empty;
			ErrStr << "Warning: The device is reporting low resources.\n\n";
			ErrStr << "This can occur when video devices, hard disks, ";
			ErrStr << "etc. have already allocated the bus resources.\n\n";
	#ifdef WIN32
			ErrStr << "This may also occur when a necessary 1394 update ";
			ErrStr << "is not installed. Search the Microsoft website for ";
			ErrStr << "'1394 hotfix' to locate any relevant updates for ";
			ErrStr << "your version of the Windows operating system.\n";
	#endif
			m_status_ex->setText(ErrStr, false);
			m_status_ex->setVisible(true);
			EventLogger::getInstance()->logMessage(m_nickname + "  Device reported low resources.");
		}
		else
		{
			m_status_ex->setText(String::empty, false);
			m_status_ex->setVisible(false);
		}
	}
	else
	{
		m_dev_nickname->setText(String::empty, false);
		m_type->setText(String::empty, false);
		m_rate->setText(String::empty, false);
		m_status->setText(String::empty, false);
		m_status_ex->setText(String::empty, false);
		m_status_ex->setColour(Label::outlineColourId, findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId));
		setEnabled(false);
	}
}

void DeviceGeneralComponent::setNewNickname()
{
	if (m_device)
	{			
		String nickname = m_dev_nickname->getText().trim();

		if (nickname.length()>(kTCAT_DICE_NICKNAME-1))
		{
			nickname = nickname.substring(0, (kTCAT_DICE_NICKNAME-1));
			m_dev_nickname->setText(nickname, false);
		}

		if ( (nickname.compare(String::empty))
		  && (nickname.compare(m_nickname)) )
		{
			m_nickname = nickname;
			m_device->nickname(std::string(m_nickname.toUTF8()));
			sendActionMessage("nickname changed"); // tell interested gui controls
		}
		else
		{
			m_dev_nickname->setText(m_nickname, false);
		}
	}
}

void DeviceGeneralComponent::labelTextChanged (Label* labelTatHasChaged)
{
	setNewNickname();
}

void DeviceGeneralComponent::mount(tcat::dice::reference<event_device> device)
{
	// call super's mount() first
	device_listener::mount(device);
	// which gives us an m_device reference
	if (m_device)
	{
		update_nickname();
		update_state();
		update_clock();
		update_rx_locked();
		update_tx_locked();
		update_locked();
		m_dev_type = m_device->type().c_str();
	}
}

void DeviceGeneralComponent::unmount()
{
	device_listener::unmount();
	triggerAsyncUpdate();
}

void DeviceGeneralComponent::update_nickname()
{
	if (m_device)
	{
		m_nickname = m_device->nickname().c_str();
	}
	else
	{
		m_nickname = String::empty;
	}
	triggerAsyncUpdate();
	sendActionMessage("nickname changed"); // tell interested gui controls
}

void DeviceGeneralComponent::update_state()
{
	if (m_device)
	{
		m_state = m_device->state();

	}
	else
	{	
		m_state = tcat::dice::DEVICE_INFO::DEVICE_STATE_REMOVED;
	}
	triggerAsyncUpdate();
}

void DeviceGeneralComponent::update_clock()
{
	if (m_device)
	{
		m_clock = m_device->clock();
	}
	else
	{
		m_clock = 0;
	}
	triggerAsyncUpdate();
}

void DeviceGeneralComponent::update_locked()
{
	if (m_device)
	{
		m_lock_state = m_device->locked();
	}
	else
	{
		m_lock_state = false;
	}
	triggerAsyncUpdate();
}

void DeviceGeneralComponent::actionListenerCallback(const String &message)
{
	if (0 == message.compare("clients open"))
	{
		m_dev_nickname->setEnabled(false);
		m_dev_nickname->setTooltip("This setting is disabled while there are open Audio Applications using the device.");
	}
	else if (0 == message.compare("clients closed"))
	{
		m_dev_nickname->setEnabled(true);
		m_dev_nickname->setTooltip(String::empty);
	}
}

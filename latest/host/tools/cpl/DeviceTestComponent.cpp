///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "DeviceTestComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "tcat_dice_pal_device.h"
#include "tcat_dice_firmware_loader.h"
#include "FirmwareProgress.h"
#include "appDefaults.h"
//==============================================================================
// this component contains the subcomponents for device test mode
DeviceTestComponent::DeviceTestComponent() : 
	m_guid (0),
	m_nickname(String::empty),
	m_running_vendor_id (0),
	m_running_product_id (0),
	m_running_vmajor (0),
	m_running_vminor (0),
	m_running_vsub (0),
	m_running_vbuild (0),
	m_testmode_cb (0),
	m_send_cmd (0),
	m_info_l (0),
	m_runningversioninfo_label (0)
{
	setName("Device Test");

    addAndMakeVisible (m_info_l = new Label ("infoLabel", ""));
    m_info_l->setFont (Font (15.0000f, Font::plain));
    m_info_l->setJustificationType (Justification::centred);

    addAndMakeVisible (m_runningversioninfo_label = new Label ("runningVersionInfoLabel",
                                                            "Current running firmware version: n/a"));
    m_runningversioninfo_label->setFont (Font (15.0000f, Font::plain));
    m_runningversioninfo_label->setJustificationType (Justification::topLeft);

	m_testmode_cb = new ComboBox ("testmode cb");
	addAndMakeVisible (m_testmode_cb);
	m_testmode_cb->setEditableText (false);
	m_testmode_cb->setJustificationType (Justification::centred);
	m_testmode_cb->addListener (this);
	m_testmode_cb->addItem ("test0", 1);
	m_testmode_cb->addItem ("test1", 2);
	m_testmode_cb->addItem ("test2", 3);
	m_testmode_cb->addItem ("test3", 4);
	m_testmode_cb->addItem ("test4", 5);
	m_testmode_cb->addItem ("test5", 6);
	m_testmode_cb->addItem ("test6", 7);
	m_testmode_cb->addItem ("test7", 8);
	m_testmode_cb->addItem ("test8", 9);
	m_testmode_cb->addItem ("test9", 10);
	m_testmode_cb->addItem ("test10", 11);
	m_testmode_cb->addItem ("test11", 12);
	m_testmode_cb->addItem ("test12", 13);
	m_testmode_cb->addItem ("test13", 14);
	m_testmode_cb->addItem ("test14", 15);
	m_testmode_cb->addItem ("test15", 16);
	m_testmode_cb->setSelectedId(1);

	addAndMakeVisible (m_send_cmd = new TextButton ("sendCmdButton"));
    m_send_cmd->setButtonText ("Set Test Mode ");
    m_send_cmd->addListener (this);

	lookAndFeelChanged();
}

DeviceTestComponent::~DeviceTestComponent()
{
	deleteAllChildren();
}

//==============================================================================
void DeviceTestComponent::paint (Graphics& g)
{
}

void DeviceTestComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void DeviceTestComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);

	repaint();

	m_info_l->setColour (Label::backgroundColourId, backgroundColour);
	m_info_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_info_l->setColour(Label::outlineColourId, findColour(TextEditor::backgroundColourId));
	m_runningversioninfo_label->setColour (Label::backgroundColourId, backgroundColour);
	m_runningversioninfo_label->setColour (Label::textColourId, backgroundColour.contrasting());
}

void DeviceTestComponent::updateContent()
{
	String info;
	if (m_guid)
	{
		setEnabled(true);
		tcat::uint32 guidHi = (tcat::uint32)(m_guid>>32) & 0xffffffff;
		tcat::uint32 guidLo = (tcat::uint32)m_guid & 0xffffffff;

		info << "Nickname: ";
		info << m_nickname;
		info << "\nUnique Id: ";
		info << String::formatted("%08X", guidHi);
		info << String::formatted("%08X", guidLo);
	}
	else
	{
		info << "Device not selected.";
		m_runningversioninfo_label->setText("Running info: n/a", false);
		setEnabled(false);
	}
	m_info_l->setText(info, false);
}

//==============================================================================
void DeviceTestComponent::resized()
{
	int w = getWidth();
    m_info_l->setBounds(30, 40, w-60, 40);	// nickname, WWUID
    m_runningversioninfo_label->setBounds (30, 100, w-60, 40);
	m_testmode_cb->setBounds((int)(w/2-115), 160, 100, 24);
	m_send_cmd->setBounds(m_testmode_cb->getRight()+20, m_testmode_cb->getY(), 110, 24);
}

void DeviceTestComponent::buttonClicked (Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_send_cmd)
	{
		if (m_device)
		{
			try
			{
				m_device->firmware_test(m_testmode_cb->getSelectedId()-1);
			}
			catch (tcat::exception::base Exception)
			{
				AlertWindow::showMessageBox (AlertWindow::WarningIcon,
											 "Set Test Mode",
											 "This command is not supportd by the device.\n\n");
			}
		}
    }
}

//==============================================================================
void DeviceTestComponent::textEditorTextChanged (TextEditor& editor)
{
}
void DeviceTestComponent::textEditorReturnKeyPressed (TextEditor& editor)
{
}
void DeviceTestComponent::textEditorEscapeKeyPressed (TextEditor& editor)
{

}
void DeviceTestComponent::textEditorFocusLost (TextEditor& editor)
{
}

void DeviceTestComponent::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
}

void DeviceTestComponent::setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus)
{
	m_guid = guid;
	if (m_device)
	{
		m_device->detach(this);
	}

	event_device_ref devref = bus->find(guid);
	if (devref) devref->attach(this);
	
	readDevice();
}

void DeviceTestComponent::readDevice()
{
	#define SPLIT_VERSION(v) TCAT_VERSION32_MAJOR(v),TCAT_VERSION32_MINOR(v),TCAT_VERSION32_SUB(v),TCAT_VERSION32_BUILD(v)
	if (0 == m_guid) return;

	if (m_device)
	{
		// get running vendor_id and product_id from WWUID
		tcat::uint64 guid = m_device->guid64();
		// bits 39-63 bits of guid
		m_running_vendor_id = (tcat::uint32)(guid>>40) & 0xffffff;
		// bits 21-31 of guid
		m_running_product_id = (tcat::uint32)(guid>>kTCAT_DICE_GUID64_PRODUCT_ID_SHIFT) & kTCAT_DICE_GUID64_PRODUCT_ID_MASK;

		// running firmware info
		try
		{
			// get compatibility and version
			tcat::dice::FL_GET_APP_INFO_RETURN runningInfo = m_device->firmware_info();
			m_running_vmajor = TCAT_VERSION32_MAJOR(runningInfo.uiApplicationVersion);
			m_running_vminor = TCAT_VERSION32_MINOR(runningInfo.uiApplicationVersion);
			m_running_vsub = TCAT_VERSION32_SUB(runningInfo.uiApplicationVersion);
			m_running_vbuild = TCAT_VERSION32_BUILD(runningInfo.uiApplicationVersion);

			m_runningversioninfo_label->setText(String::formatted(
				"Running info: Vendor:%x, Product:%x, SDK: %d.%d.%d.%d, app: %d.%d.%d.%d",
				m_running_vendor_id, m_running_product_id,
				SPLIT_VERSION(runningInfo.uiBaseSDKVersion),
				m_running_vmajor, m_running_vminor, m_running_vsub, m_running_vbuild), false);
		}
		catch (tcat::exception::base Exception)
		{
			m_runningversioninfo_label->setText("Running info: unknown", false);
		}
	}
}

void DeviceTestComponent::actionListenerCallback(const String &message)
{
	if (0 == message.compare("nickname changed"))
	{
		if (m_device)
			m_nickname = m_device->nickname().c_str();
		else
			m_nickname = "Unknown";
			
		updateContent();
	}
}

void DeviceTestComponent::handleAsyncUpdate()
{
	updateContent();
}

void DeviceTestComponent::mount(tcat::dice::reference<event_device> device)
{
	// call super's mount() first
	device_listener::mount(device);
	// which gives us an m_device reference
	if (m_device)
	{
		m_guid = m_device->guid64();
		m_nickname = m_device->nickname().c_str();
		triggerAsyncUpdate();
	}
}

void DeviceTestComponent::unmount()
{
	device_listener::unmount();
	m_guid = 0;
	m_nickname = String::empty;
	triggerAsyncUpdate();
}

void DeviceTestComponent::update_nickname()
{
	if (m_device)
	{
		m_nickname = m_device->nickname().c_str();
		triggerAsyncUpdate();
	}
}

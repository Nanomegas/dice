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

#include "DeviceSimpleFirmwareComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "tcat_dice_pal_device.h"
#include "tcat_dice_firmware_loader.h"
#include "FirmwareProgress.h"
#include "appDefaults.h"

//==============================================================================
// this component contains the subcomponents that manage device firmware
DeviceSimpleFirmwareComponent::DeviceSimpleFirmwareComponent() : 
	m_guid (0),
	m_nickname (String::empty),
	m_font (0),
	m_file_vendor_id (0),
	m_file_product_id (0),
	m_file_vmajor (0),
	m_file_vminor (0),
	m_file_vsub (0),
	m_file_vbuild (0),
	m_running_vendor_id (0),
	m_running_product_id (0),
	m_running_vmajor (0),
	m_running_vminor (0),
	m_running_vsub (0),
	m_running_vbuild (0),
	m_allow_continue (false),
	m_cur_working_file (String::empty),
	m_cur_working_filesize (0),
	m_instr_l (0),
	m_info_l (0),	
	m_ulfile_label (0),
	m_filechooser (0),
	m_current_file (File::nonexistent),
	m_upload (0),
	m_show_details (0),
	m_details_group (0),
	m_runningversioninfo_label (0),
	m_fileversioninfo_label (0),
	m_sectorname_label (0),
	m_sectorname (String::empty)
{
	setName("Device Simple Firmware");

    addAndMakeVisible (m_instr_l = new Label ("instrLabel", "Choose a local firmware application file, then click Upload."));
    m_instr_l->setFont (Font (15.0000f, Font::plain));
    m_instr_l->setJustificationType (Justification::centredLeft);

    addAndMakeVisible (m_info_l = new Label ("infoLabel", ""));
    m_info_l->setFont (Font (15.0000f, Font::plain));
    m_info_l->setJustificationType (Justification::centred);

    addAndMakeVisible (m_ulfile_label = new Label ("uploadFileLabel",
                                                   "Local upload file:"));
    m_ulfile_label->setFont (Font (15.0000f, Font::plain));
    m_ulfile_label->setJustificationType (Justification::centredLeft);
    m_ulfile_label->setEditable (false, false, false);

    addAndMakeVisible (m_filechooser = new FilenameComponent ("uploadfile",
                                                            File::nonexistent,
                                                            true, false, false,
                                                            "*.bin",
                                                            String::empty,
                                                            "(choose a file for upload to flash)"));
    m_filechooser->addListener (this);
    m_filechooser->setBrowseButtonText ("browse");
    
    addAndMakeVisible (m_upload = new TextButton ("uploadButton"));
    m_upload->setButtonText ("upload");
    m_upload->addListener (this);

	addAndMakeVisible (m_show_details = new ToggleButton ("Show details button"));
	m_show_details->setButtonText("Show details");
	m_show_details->addListener (this);

	addChildComponent (m_details_group = new GroupComponent ("detailsGroup", "Details"));
	m_sectorname = appDefaults->getAppSettings()->getValue("defaultFrmwSector");
    addChildComponent (m_sectorname_label = new Label ("sectorNameLabel",
														"Replacing firmware application: " + m_sectorname) );

	m_allow_continue = appDefaults->getAppSettings()->getBoolValue("allowLoadAfterWarning");

    m_sectorname_label->setFont (Font (15.0000f, Font::plain));
    m_sectorname_label->setJustificationType (Justification::centredLeft);

    addChildComponent (m_runningversioninfo_label = new Label ("runningVersionInfoLabel",
                                                            "Current running firmware version: n/a"));
    m_runningversioninfo_label->setFont (Font (15.0000f, Font::plain));
    m_runningversioninfo_label->setJustificationType (Justification::topLeft);

    addChildComponent (m_fileversioninfo_label = new Label ("fileVersionInfoLabel",
                                                            "File info: file not selected."));
    m_fileversioninfo_label->setFont (Font (15.0000f, Font::plain));
    m_fileversioninfo_label->setJustificationType (Justification::topLeft);

	lookAndFeelChanged();
}

DeviceSimpleFirmwareComponent::~DeviceSimpleFirmwareComponent()
{
	deleteAllChildren();
}

//==============================================================================
void DeviceSimpleFirmwareComponent::paint (Graphics& g)
{
}

void DeviceSimpleFirmwareComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void DeviceSimpleFirmwareComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);

	repaint();

	m_instr_l->setColour (Label::backgroundColourId, backgroundColour);
	m_instr_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_info_l->setColour (Label::backgroundColourId, backgroundColour);
	m_info_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_info_l->setColour(Label::outlineColourId, findColour(TextEditor::backgroundColourId));
	m_ulfile_label->setColour (Label::backgroundColourId, backgroundColour);
	m_ulfile_label->setColour (Label::textColourId, backgroundColour.contrasting());
	m_show_details->setColour(ToggleButton::textColourId, findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId).contrasting());
	m_details_group->setColour(GroupComponent::textColourId, findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId).contrasting());
	m_sectorname_label->setColour (Label::backgroundColourId, backgroundColour);
	m_sectorname_label->setColour (Label::textColourId, backgroundColour.contrasting());
	m_runningversioninfo_label->setColour (Label::backgroundColourId, backgroundColour);
	m_runningversioninfo_label->setColour (Label::textColourId, backgroundColour.contrasting());
	m_fileversioninfo_label->setColour (Label::backgroundColourId, backgroundColour);
	m_fileversioninfo_label->setColour (Label::textColourId, backgroundColour.contrasting());
}

void DeviceSimpleFirmwareComponent::updateContent()
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
void DeviceSimpleFirmwareComponent::resized()
{
	m_instr_l->setBounds(40, 10, getWidth()-20, 24);
    m_info_l->setBounds(30, 40, getWidth()-60, 40);	// nickname, WWUID

	m_ulfile_label->setBounds (10, 83, 112, 24);
	
	m_filechooser->setBounds(10, 110, getWidth()-20, 24);

	m_upload->setBounds (280, 146, 90, 24);
	
	m_show_details->setBounds(80, 146, 100, 24);

	m_details_group->setBounds(3, 173, getWidth()-6, 128);
    m_sectorname_label->setBounds (20, 192, getWidth()-40, 22);
    m_runningversioninfo_label->setBounds (20, 217, getWidth()-40, 44);
    m_fileversioninfo_label->setBounds (20, 252, getWidth()-40, 44);   
}

void DeviceSimpleFirmwareComponent::buttonClicked (Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_show_details)
	{
		bool bChecked = m_show_details->getToggleState();
		m_details_group->setVisible(bChecked);
		m_sectorname_label->setVisible(bChecked);
		m_runningversioninfo_label->setVisible(bChecked);
		m_fileversioninfo_label->setVisible(bChecked); 
	}
	else if (buttonThatWasClicked == m_upload)
    {
		String ulFile = m_current_file.getFullPathName();
		bool bUpload = false;
		bool bSectorFound = false;
		bool bError = false;

		if (m_current_file.exists())						// check file selected
		{
			if (m_device)
			{
				// determine sector name to write from deafaults file and product id
				
				
				// make sure sector exists
				for (std::vector<sector_rec>::const_iterator i = m_sector_list.begin(); i != m_sector_list.end(); ++i)
				{
					String str(i->name.c_str());
					if (m_sectorname.compare(str))
					{
						bSectorFound = true;
						bUpload = true;
					}
				}

				if (!bSectorFound)
				{
					AlertWindow::showMessageBox (AlertWindow::WarningIcon,
											 "File Upload",
											 String::formatted("Sector not found: " + m_sectorname + "\n\nThe utility cannot update your firmware application.") );
				}
			}						

			if (bUpload)
			{
				// vendor and product id checks
				if (m_file_vendor_id == 0)
				{
					bError = true;
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Cannot determine manufacturer information from selected file.\n\nCompatibility with this firmware file cannot be guaranteed.\n\nContinue?"))
					{
						bUpload = false;
					}
				}
				else if (m_file_vendor_id != m_running_vendor_id)
				{
					bError = true;
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Manufacturer information from selected file does not match the device.\n\nCompatibility with this firmware file cannot be guaranteed.\n\nContinue?"))
					{
						bUpload = false;
					}
				}

				if (bUpload && (m_file_product_id != m_running_product_id))
				{
					bError = true;
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Product ID information from selected file does not match the device.\n\nCompatibility with this firmware file cannot be guaranteed.\n\nContinue?"))
					{
						bUpload = false;
					}
				}
			}

			if (bUpload)
			{
				// version checks
				if (m_file_vmajor < m_running_vmajor)
				{
					bError = true;
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Firmware is older than the current running version.\n\nContinue?"))
					{
						bUpload = false;
					}
				}
				else if ((m_file_vmajor == m_running_vmajor) && (m_file_vminor < m_running_vminor))
				{
					bError = true;
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Firmware is older than the current running version.\n\nContinue?"))
					{
						bUpload = false;
					}
				}
			}

			if (bUpload)
			{
				if (bError && !m_allow_continue)
				{
					AlertWindow::showMessageBox (AlertWindow::WarningIcon,
											 "File Upload",
											 String::formatted("This utility does not allow upload after the previous warnings.\n\nUpload cancelled.") );
				}
				else if (m_device)
				{
					FWProgressBackgroundThread * fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_UPLOAD, ulFile, m_sectorname, 0,  m_device);
					fwThread->addActionListener(this); // thread will message us if upload was successful and the write-to-flash step will happen in the ActionListener callback
					m_device->attach(fwThread);

					fwThread->runThread();
					delete fwThread;
				}
			}
		}
		else
		{
            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         "File Upload",
                                         "File not found.\n\nPlease choose a file to upload to flash.\n\n");
		}
    }
}

void DeviceSimpleFirmwareComponent::filenameComponentChanged (FilenameComponent*)
{
	#define SPLIT_VERSION(v) TCAT_VERSION32_MAJOR(v),TCAT_VERSION32_MINOR(v),TCAT_VERSION32_SUB(v),TCAT_VERSION32_BUILD(v)
    // this is called when the user changes the filename in the file chooser box
	File uploadFile = m_filechooser->getCurrentFile();
	m_current_file = uploadFile;
	
	if (m_current_file.exists())
	{
		if (m_device)
		{
			try
			{
				String cur_file(m_current_file.getFullPathName());
				tcat::dice::FL_GET_APP_INFO_RETURN fileInfo = m_device->firmware_info(std::string(cur_file.toUTF8()));
				m_file_vendor_id = fileInfo.uiVendorID;
				m_file_product_id = fileInfo.uiProductID;
				m_file_vmajor = TCAT_VERSION32_MAJOR(fileInfo.uiApplicationVersion);
				m_file_vminor = TCAT_VERSION32_MINOR(fileInfo.uiApplicationVersion);
				m_file_vsub = TCAT_VERSION32_SUB(fileInfo.uiApplicationVersion);
				m_file_vbuild = TCAT_VERSION32_BUILD(fileInfo.uiApplicationVersion);

				m_fileversioninfo_label->setText(String::formatted(
					"File info: Vendor:%x, Product:%x, SDK: %d.%d.%d.%d\n app: %d.%d.%d.%d",
					m_file_vendor_id, m_file_product_id,
					SPLIT_VERSION(fileInfo.uiBaseSDKVersion),
					m_file_vmajor, m_file_vminor, m_file_vsub, m_file_vbuild), false);
			}
			catch (tcat::exception::base Exception)
			{
				m_fileversioninfo_label->setText("File info: unknown image version info", false);
			}
		}
	}
	else
	{
		m_fileversioninfo_label->setText("File info: file not selected", false);
	}
}

//==============================================================================
void DeviceSimpleFirmwareComponent::textEditorTextChanged (TextEditor& editor)
{
}
void DeviceSimpleFirmwareComponent::textEditorReturnKeyPressed (TextEditor& editor)
{
}
void DeviceSimpleFirmwareComponent::textEditorEscapeKeyPressed (TextEditor& editor)
{

}
void DeviceSimpleFirmwareComponent::textEditorFocusLost (TextEditor& editor)
{
}

void DeviceSimpleFirmwareComponent::setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus)
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

void DeviceSimpleFirmwareComponent::readDevice()
{
	m_sector_list.clear();

	if (0 == m_guid) return;

	if (m_device)
	{
		// get running vendor_id and product_id from WWUID
		tcat::uint64 guid = m_device->guid64();
		// bits 39-63 bits of guid
		m_running_vendor_id = (tcat::uint32)(guid>>40) & 0xffffff;
		// bits 21-31 of guid
		m_running_product_id = (tcat::uint32)(guid>>kTCAT_DICE_GUID64_PRODUCT_ID_SHIFT) & kTCAT_DICE_GUID64_PRODUCT_ID_MASK;

		// do sector (flash file) name checks
		m_sectorname = appDefaults->getAppSettings()->getValue(String::formatted("frmwSector%04d", m_running_product_id));
		if (String::empty == m_sectorname)
		{
			m_sectorname = appDefaults->getAppSettings()->getValue("defaultFrmwSector");
		}

		// must set a default for sector name in defaults file or in initialise()
		jassert(String::empty != m_sectorname);

		m_sectorname_label->setText("Replacing firmware application: " + m_sectorname, false);

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
				"Running info: Vendor:%x, Product:%x, SDK: %d.%d.%d.%d\n app: %d.%d.%d.%d",
				m_running_vendor_id, m_running_product_id,
				SPLIT_VERSION(runningInfo.uiBaseSDKVersion),
				m_running_vmajor, m_running_vminor, m_running_vsub, m_running_vbuild), false);
		}
		catch (tcat::exception::base Exception)
		{
			m_runningversioninfo_label->setText("Running info: unknown", false);
		}

		tcat::uint32 i = 0;
		tcat::dice::FL_GET_IMAGE_DESC_RETURN info;
		while (m_device->firmware_info(i, info))
		{
			sector_rec srec;
			srec.name = info.name;
			srec.flashBase = info.flashBase;
			srec.memBase = info.memBase;
			srec.length = info.length;
			srec.entryPoint = info.entryPoint;
			m_sector_list.push_back(srec);

			i++;
		}
	}
}

void DeviceSimpleFirmwareComponent::actionListenerCallback(const String &message)
{
	if (0 == message.compare("nickname changed"))
	{
		if (m_device)
			m_nickname = m_device->nickname().c_str();
		else
			m_nickname = "Unknown";
			
		updateContent();
	}
	else if (0 == message.compare("fw upload success"))
	{
		// a successful upload happened, commit it to flash
		if (m_device && m_current_file.exists())
		{
			FWProgressBackgroundThread * fwThread = NULL;

			// if device supports atomic delete+program, then skip this unnecessary delete step
			if (0 == (m_device->firmware_capabilities() & FL_CAP_AUTOERASE))
			{
				fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_DELETE_IMAGE, String::empty, m_sectorname, 0,  m_device);
				m_device->attach(fwThread);
				fwThread->runThread();
				delete fwThread;
			}

			fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_CREATE_IMAGE, String::empty, m_sectorname, (tcat::uint32)m_current_file.getSize(), m_device, true);
			m_device->attach(fwThread);
			fwThread->runThread();
			delete fwThread;

			m_device->firmware_reset();
		}
	}
}

void DeviceSimpleFirmwareComponent::handleAsyncUpdate()
{
	updateContent();
}

void DeviceSimpleFirmwareComponent::mount(tcat::dice::reference<event_device> device)
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

void DeviceSimpleFirmwareComponent::unmount()
{
	device_listener::unmount();
	m_guid = 0;
	m_nickname = String::empty;
	triggerAsyncUpdate();
}

void DeviceSimpleFirmwareComponent::update_nickname()
{
	if (m_device)
	{
		m_nickname = m_device->nickname().c_str();
		triggerAsyncUpdate();
	}
}

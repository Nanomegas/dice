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

#include "DeviceFirmwareComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "tcat_dice_pal_device.h"
#include "tcat_dice_firmware_loader.h"
#include "FirmwareProgress.h"

//==============================================================================
// this component contains the subcomponents that manage device firmware
DeviceFirmwareComponent::DeviceFirmwareComponent() : 
	m_guid (0),
	m_nickname (String::empty),
	m_font (0),
	m_numRows (0),
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
	m_duplicate_found (false),
	m_cur_working_file (String::empty),
	m_cur_working_filesize (0),
	m_current_file (File::nonexistent),
	m_runningversioninfo_label (0),
	m_ulfile_label (0),
	m_filechooser (0),
	m_fileversioninfo_label (0),
	m_sectorname_label (0),
	m_ul_sectorname (0),
	m_upload (0),
	m_read_device (0),
	m_reset_device (0),
	m_del_sector (0),
	m_download (0),
	m_info_l (0),
	m_table (0)
{
    setName ("Device Firmware Utils");
    
    addAndMakeVisible (m_filechooser = new FilenameComponent ("uploadfile",
                                                            File::nonexistent,
                                                            true, false, false,
                                                            "*.bin;*.xml",
                                                            String::empty,
                                                            "(choose a file for upload to flash)"));
    m_filechooser->addListener (this);
    m_filechooser->setBrowseButtonText ("browse");

    addAndMakeVisible (m_table = new TableListBox ("firmware files", this));

	// give it a border
	m_table->setOutlineThickness (1);
	// set up the column labels
	m_table->setHeaderHeight(20);
	m_table->setRowHeight(20);
    m_table->getHeader().addColumn ("Name", 1, 138, 120, -1, TableHeaderComponent::notSortable);
    m_table->getHeader().addColumn ("Flash Addr", 2, 80, 60, -1, TableHeaderComponent::notResizableOrSortable);
    m_table->getHeader().addColumn ("Memory Addr", 3, 80, 60, -1, TableHeaderComponent::notResizableOrSortable);
    m_table->getHeader().addColumn ("Length", 4, 80, 60, -1, TableHeaderComponent::notResizableOrSortable);
    m_table->getHeader().addColumn ("Entry Point", 5, 80, 60, -1, TableHeaderComponent::notResizableOrSortable);
    
    addAndMakeVisible (m_read_device = new TextButton ("readDeviceButton"));
    m_read_device->setButtonText ("read device");
    m_read_device->addListener (this);

    addAndMakeVisible (m_reset_device = new TextButton ("resetDeviceButton"));
    m_reset_device->setButtonText ("reset device");
    m_reset_device->addListener (this);

    addAndMakeVisible (m_ulfile_label = new Label ("uploadFileLabel",
                                                   "Local upload file:"));
    m_ulfile_label->setFont (Font (15.0000f, Font::plain));
    m_ulfile_label->setJustificationType (Justification::centredLeft);

    addAndMakeVisible (m_upload = new TextButton ("uploadButton"));
    m_upload->setButtonText ("upload");
    m_upload->addListener (this);

    addAndMakeVisible (m_ul_sectorname = new TextEditor ("sectorName"));
    m_ul_sectorname->setMultiLine (false);
    m_ul_sectorname->setReturnKeyStartsNewLine (false);
    m_ul_sectorname->setReadOnly (false);
    m_ul_sectorname->setScrollbarsShown (false);
    m_ul_sectorname->setCaretVisible (true);
    m_ul_sectorname->setPopupMenuEnabled (true);
    m_ul_sectorname->setInputRestrictions(16, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789_.");

    addAndMakeVisible (m_sectorname_label = new Label ("sectorNameLabel",
                                                       "Create/Replace Sector name:"));
    m_sectorname_label->setFont (Font (15.0000f, Font::plain));
    m_sectorname_label->setJustificationType (Justification::centredLeft);

    addAndMakeVisible (m_fileversioninfo_label = new Label ("fileVersionInfoLabel",
                                                            "File info: n/a"));
    m_fileversioninfo_label->setFont (Font (15.0000f, Font::plain));
    m_fileversioninfo_label->setJustificationType (Justification::topLeft);

    addAndMakeVisible (m_runningversioninfo_label = new Label ("runningVersionInfoLabel",
                                                            "Current running firmware version: n/a"));
    m_runningversioninfo_label->setFont (Font (15.0000f, Font::plain));
    m_runningversioninfo_label->setJustificationType (Justification::topLeft);

    addAndMakeVisible (m_download = new TextButton ("downloadButton"));
    m_download->setButtonText ("download all");
    m_download->addListener (this);

    addAndMakeVisible (m_del_sector = new TextButton ("deleteSectorButton"));
    m_del_sector->setButtonText ("delete selected sector");
    m_del_sector->addListener (this);

    addAndMakeVisible (m_info_l = new Label ("infoLabel", ""));
    m_info_l->setFont (Font (15.0000f, Font::plain));
    m_info_l->setJustificationType (Justification::centredLeft);
}

DeviceFirmwareComponent::~DeviceFirmwareComponent()
{
	deleteAllChildren();
}

//==============================================================================
void DeviceFirmwareComponent::paint (Graphics& g)
{
}

void DeviceFirmwareComponent::updateContent()
{
	String info;
	if (m_guid)
	{
		setEnabled(true);
		tcat::uint32 guidHi = (tcat::uint32)(m_guid>>32) & 0xffffffff;
		tcat::uint32 guidLo = (tcat::uint32)m_guid & 0xffffffff;

		info << "Nickname: ";
		info << m_nickname;
		info << ",  Unique Id: ";
		info << String::formatted("%08X", guidHi);
		info << String::formatted("%08X", guidLo);
	}
	else
	{
		info << "Device not selected.";
		m_runningversioninfo_label->setText("Running info: n/a", false);
		m_ul_sectorname->setText(String::empty, false);
		m_numRows = 0;
		m_table->updateContent();
		setEnabled(false);
	}
	m_info_l->setText(info, false);
	m_ul_sectorname->repaint();
}

//==============================================================================
void DeviceFirmwareComponent::resized()
{
    m_runningversioninfo_label->setBounds (10, 10, getWidth()-20, 24);

    m_ulfile_label->setBounds (10, 35, 112, 24);
	m_filechooser->setBounds(123, 35, getWidth()-153, 24);
	
    m_fileversioninfo_label->setBounds (10, 63, getWidth()-20, 24);
	
    m_sectorname_label->setBounds (10, 88, 190, 24);
    m_ul_sectorname->setBounds (200, 88, 165, 24);
    m_upload->setBounds (376, 88, 90, 24);
    
    m_read_device->setBounds (10, 123, 90, 24);
    m_reset_device->setBounds (110, 123, 90, 24);
    m_del_sector->setBounds(212, 123, 152, 24);
    m_download->setBounds (376, 123, 90, 24);

    m_info_l->setBounds(10, 149, getWidth()-20, 24);
	m_table->setBoundsInset (BorderSize<int> (173, 8, 8, 8));

	lookAndFeelChanged();
}

void DeviceFirmwareComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void DeviceFirmwareComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);

	repaint();

	m_ulfile_label->setColour (Label::backgroundColourId, backgroundColour);
	m_ulfile_label->setColour (Label::textColourId, backgroundColour.contrasting());
	m_sectorname_label->setColour (Label::backgroundColourId, backgroundColour);
	m_sectorname_label->setColour (Label::textColourId, backgroundColour.contrasting());
	m_fileversioninfo_label->setColour (Label::backgroundColourId, backgroundColour);
	m_fileversioninfo_label->setColour (Label::textColourId, backgroundColour.contrasting());
	m_runningversioninfo_label->setColour (Label::backgroundColourId, backgroundColour);
	m_runningversioninfo_label->setColour (Label::textColourId, backgroundColour.contrasting());
	m_info_l->setColour (Label::backgroundColourId, backgroundColour);
	m_info_l->setColour (Label::textColourId, backgroundColour.contrasting());
    m_ul_sectorname->setTextToShowWhenEmpty ("(enter a sector name)", findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId));
}

void DeviceFirmwareComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == m_read_device)
    {
		readDevice();
    }
    else if (buttonThatWasClicked == m_reset_device)
    {
		if (m_device)
		{
			m_device->firmware_reset();
		}
    }
    else if (buttonThatWasClicked == m_upload)
    {
		String ulFile = m_current_file.getFullPathName();
		bool bUpload = false;
		m_duplicate_found = false;

		if (m_current_file.exists())						// check file selected
		{
			// do sector (flash file) name checks
			String sector = m_ul_sectorname->getText();
			if (sector.compare(String::empty) == 0)
			{
				AlertWindow::showMessageBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Please select a sector or enter a sector name.\n\n");
			}
			else if (sector.compareIgnoreCase("redboot") == 0)
			{
				// using RedBoot sector name
				if (AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
										 "File Upload",
										 "Confirm overwriting the boot sector with a new (ROM-able) RedBoot image.\n\nUsing an incorrect file here will make your device inoperable."))
				{
					bUpload = true;
				}
			}
			else if (sector.compareIgnoreCase("redboot config") == 0)
			{
				// disallow overwriting config area
				AlertWindow::showMessageBox (AlertWindow::WarningIcon,
										 "File Upload",
										 "Overwriting of Redboot config area not allowed.");
			}
			else if (sector.compareIgnoreCase("fis directory") == 0)
			{
				// disallow overwriting file system table
				AlertWindow::showMessageBox (AlertWindow::WarningIcon,
										 "File Upload",
										 "Overwriting of FIS directory not allowed.");
			}
			else
			{
				if (m_device)
				{
					for (std::vector<sector_rec>::const_iterator i = m_sector_list.begin(); i != m_sector_list.end(); ++i)
					{
						String str(i->name.c_str());
						if (sector.compare(str) == 0)
							m_duplicate_found = true;
					}

					if (m_duplicate_found)
					{
						if (AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
												 "File Upload",
												 String::formatted("Confirm overwriting sector: " + sector)) )
						{
							bUpload = true;
						}
					}
					else
					{
						bUpload = true;
					}
				}						
			}

			if (bUpload && (m_current_file.getSize() > 0x20000))
			{
				// vendor and product id checks
				if (m_file_vendor_id == 0)
				{
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Cannot determine manufacturer information from selected file.\n\nCompatibility with this firmware file cannot be guaranteed.\n\nContinue?"))
					{
						bUpload = false;
					}
				}
				else if (m_file_vendor_id != m_running_vendor_id)
				{
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Manufacturer information from selected file does not match the device.\n\nCompatibility with this firmware file cannot be guaranteed.\n\nContinue?"))
					{
						bUpload = false;
					}
				}
				
				if (bUpload && (m_file_product_id != m_running_product_id))
				{
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Product ID information from selected file does not match the device.\n\nCompatibility with this firmware file cannot be guaranteed.\n\nContinue?"))
					{
						bUpload = false;
					}
				}

			}

			if (bUpload && (m_current_file.getSize() > 0x20000))
			{
				// version checks
				if (m_file_vmajor < m_running_vmajor)
				{
					if (!AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											 "File Upload",
											 "Firmware is older than the current running version.\n\nContinue?"))
					{
						bUpload = false;
					}
				}
				else if ((m_file_vmajor == m_running_vmajor) && (m_file_vminor < m_running_vminor))
				{
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
				if (m_device)
				{
					FWProgressBackgroundThread * fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_UPLOAD, ulFile, m_ul_sectorname->getText(), 0,  m_device);
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
	else if (buttonThatWasClicked == m_download)
	{
		FileChooser fc ("Save Entire flash to local file...", 
						File::getCurrentWorkingDirectory(), 
						"*.bin", 
						true);
		if (fc.browseForFileToSave(true))
		{
			File chosenFile = fc.getResult();
			m_cur_working_file = chosenFile.getFullPathName();
			m_cur_working_filesize = chosenFile.getSize();
			chosenFile.getParentDirectory().setAsCurrentWorkingDirectory();
		
			if (m_cur_working_file.isNotEmpty())
			{
				if (m_device)
				{
					FWProgressBackgroundThread * fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_READ_MEMORY, m_cur_working_file, String::empty, 0,  m_device);
					m_device->attach(fwThread);
					fwThread->runThread();
					delete fwThread;
				}
			}		
		}
	}    
	else if (buttonThatWasClicked == m_del_sector)
    {
		if (m_ul_sectorname->getText().compare(String::empty) == 0)
		{
			AlertWindow::showMessageBox (AlertWindow::WarningIcon,
											"File Upload",
											"Please select a sector or enter a sector name.\n\n");
		}
		else if (m_ul_sectorname->getText().compareIgnoreCase("redboot") == 0)
		{
			AlertWindow::showMessageBox (AlertWindow::WarningIcon,
											"File Upload",
											"Deletion of this sector is prohibited.\n\nIf you want to replace the RedBoot flash sector file, use the\nupload feature and provide a (ROM-able) RedBoot image.");
		}
		else if ( (m_ul_sectorname->getText().compareIgnoreCase("redboot config") == 0)
				|| (m_ul_sectorname->getText().compareIgnoreCase("fis directory") == 0) )
		{
			AlertWindow::showMessageBox (AlertWindow::WarningIcon,
											"Delete Sector",
											"Deletion of this sector is prohibited.");
		}
		else
		{
			if (AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
											"Delete Sector",
											"Confirm deleting sector: " + m_ul_sectorname->getText() + "\n\n"))
			{
				if (m_device)
				{
					FWProgressBackgroundThread * fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_DELETE_IMAGE, String::empty, m_ul_sectorname->getText(), 0,  m_device);
					m_device->attach(fwThread);
					fwThread->runThread();
					delete fwThread;
					readDevice();
				}						
			}
		}
	}
}

void DeviceFirmwareComponent::filenameComponentChanged (FilenameComponent*)
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
					"File info: Vendor:%x, Product:%x, SDK: %d.%d.%d.%d, app: %d.%d.%d.%d",
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
		m_fileversioninfo_label->setText("File info: n/a", false);
	}
}

//==============================================================================
// This is overloaded from TableListBoxModel, and must return the total number of rows in our table
int DeviceFirmwareComponent::getNumRows()
{
	return m_numRows;
}


// This is overloaded from TableListBoxModel, and should fill in the background of the whole row
void DeviceFirmwareComponent::paintRowBackground (Graphics& g, int rowNumber, int width, int height, bool rowIsSelected)
{
	if (rowNumber % 2)
		g.fillAll (Colours::gainsboro);
	else
		g.fillAll (Colours::silver);
	
    if (rowIsSelected)
		g.fillAll (Colours::white);
}

// This is overloaded from TableListBoxModel, and must paint any cells that aren't using custom
// components.
void DeviceFirmwareComponent::paintCell (Graphics& g,
                int rowNumber,
                int columnId,
                int width, int height,
                bool rowIsSelected)
{
    g.setColour (Colours::black);

	if (m_device)
	{
		if (rowNumber < (int)m_sector_list.size())
		{
			switch (columnId)
			{
			case 1:
				{
					String str = m_sector_list[rowNumber].name.c_str();

					g.drawFittedText (str, 2, 0, width - 4, height, Justification::centredLeft, 1);
					if (rowIsSelected)
					{
						if ( ( 0 == str.compareIgnoreCase("redboot config") ) || ( 0 == str.compareIgnoreCase("fis directory") ) )
						{
							m_ul_sectorname->setText(String::empty, false);
						}
						else
						{
							m_ul_sectorname->setText(str, false);
						}
						triggerAsyncUpdate();
					}
				}
				break;
			case 2:
				g.drawFittedText (String::formatted("0x%08X", m_sector_list[rowNumber].flashBase), 2, 0, width - 4, height, Justification::centredLeft, 1);
				break;
			case 3:
				g.drawFittedText (String::formatted("0x%08X", m_sector_list[rowNumber].memBase), 2, 0, width - 4, height, Justification::centredLeft, 1);
				break;
			case 4:
				g.drawFittedText (String::formatted("0x%08X", m_sector_list[rowNumber].length), 2, 0, width - 4, height, Justification::centredLeft, 1);
				break;
			case 5:
				g.drawFittedText (String::formatted("0x%08X", m_sector_list[rowNumber].entryPoint), 2, 0, width - 4, height, Justification::centredLeft, 1);
				break;
			default:
				break;
			};
		}
	}

    g.setColour (Colours::black.withAlpha (0.2f));
    g.fillRect (width - 1, 0, 1, height);
}

void DeviceFirmwareComponent::textEditorTextChanged (TextEditor& editor)
{
}
void DeviceFirmwareComponent::textEditorReturnKeyPressed (TextEditor& editor)
{
}
void DeviceFirmwareComponent::textEditorEscapeKeyPressed (TextEditor& editor)
{

}
void DeviceFirmwareComponent::textEditorFocusLost (TextEditor& editor)
{
}

void DeviceFirmwareComponent::setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus)
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

void DeviceFirmwareComponent::readDevice()
{
	m_sector_list.clear();

	m_numRows = 0;
	
	m_table->updateContent();

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

		tcat::dice::FL_GET_IMAGE_DESC_RETURN info;
		while (m_device->firmware_info((tcat::uint32)m_numRows, info))
		{
			sector_rec srec;
			srec.name = info.name;
			srec.flashBase = info.flashBase;
			srec.memBase = info.memBase;
			srec.length = info.length;
			srec.entryPoint = info.entryPoint;
			m_sector_list.push_back(srec);

			m_numRows++;
		}
	}
			
	if (m_numRows < 6)
	{
		m_numRows = 6;
	}

	if (m_numRows > 6)
	{
		m_table->getHeader().setColumnWidth(1, 120);
	}

	m_table->updateContent();
				
	const MessageManagerLock jlock;
	m_ul_sectorname->setText(String::empty, false);
}

void DeviceFirmwareComponent::actionListenerCallback(const String &message)
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
			if (m_duplicate_found && (0 == (m_device->firmware_capabilities() &FL_CAP_AUTOERASE)))
			{
				fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_DELETE_IMAGE, String::empty, m_ul_sectorname->getText(), 0,  m_device);
				m_device->attach(fwThread);
				fwThread->runThread();
				delete fwThread;
			}

			fwThread = new FWProgressBackgroundThread(tcat::dice::FL_OP_CREATE_IMAGE, String::empty, m_ul_sectorname->getText(), (tcat::uint32)m_current_file.getSize(), m_device, true);
			m_device->attach(fwThread);
			fwThread->runThread();
			delete fwThread;

			readDevice();
		}
	}
}

void DeviceFirmwareComponent::handleAsyncUpdate()
{
	updateContent();
}

void DeviceFirmwareComponent::mount(tcat::dice::reference<event_device> device)
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

void DeviceFirmwareComponent::unmount()
{
	device_listener::unmount();
	m_guid = 0;
	m_nickname = String::empty;
	triggerAsyncUpdate();
}

void DeviceFirmwareComponent::update_nickname()
{
	if (m_device)
	{
		m_nickname = m_device->nickname().c_str();
		triggerAsyncUpdate();
	}
}

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

#include "BusOhciCheckComponent.h"
#include "BusWindow.h"
#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"
#include "fwi.h"

//==============================================================================
// this component contains the subcomponents that display global info

BusOhciCheckComponent::BusOhciCheckComponent () :
    m_fwi(0)
{
	setName ("Bus OHCI Check");
    
    m_fwi = new fwi();
    
	m_rescan = new TextButton ("Scan now");
    addAndMakeVisible (m_rescan);
    m_rescan->changeWidthToFitText (24);
    m_rescan->addListener (this);
    m_rescan->setTopLeftPosition (25, 35);
	m_rescan->setTooltip("Examines the 1394 OHCI controllers in your system and shows driver compatibility information.");

	m_look_online = new ToggleButton ("Online");
    addAndMakeVisible (m_look_online);
    m_look_online->addListener (this);
#if defined (WIN32)
    m_look_online->setBounds (135, 10, 40, 24);
#else
    m_look_online->setBounds (135, 18, 40, 24);
#endif
	m_look_online->setButtonText ("Look online for latest info.");
	m_look_online->setTooltip("Search the online compatibility database. The built-in database will be used if your computer is not connected to the Internet.");
	m_look_online->changeWidthToFitText();

	m_verbose = new ToggleButton ("Verbose");
    addAndMakeVisible (m_verbose);
    m_verbose->addListener (this);
#if defined (WIN32)
    m_verbose->setBounds (135, 35, 50, 24);
#else
    m_verbose->setBounds (135, 55, 50, 24);
#endif
	m_verbose->setButtonText ("Show additional details.");
	m_verbose->setTooltip("Show additional information that may be useful for reports during compatibility testing.");
	m_verbose->changeWidthToFitText();

#if defined (WIN32)  
	m_show_inactive = new ToggleButton ("ShowAll");
    addAndMakeVisible (m_show_inactive);
    m_show_inactive->addListener (this);
    m_show_inactive->setBounds (135, 60, 50, 24);
	m_show_inactive->setButtonText ("Show inactive devices.");
	m_show_inactive->setTooltip("List devices that are no longer present, or are present but disabled.");
	m_show_inactive->changeWidthToFitText();
#endif
    
	lookAndFeelChanged();
}
	
BusOhciCheckComponent::~BusOhciCheckComponent() 
{
    if (m_fwi)
    {
        delete m_fwi;
        m_fwi = 0;
    }
    deleteAllChildren();
}

void BusOhciCheckComponent::paint (Graphics& g)
{
}

void BusOhciCheckComponent::resized()
{
}

// rescan
void BusOhciCheckComponent::buttonClicked (Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_rescan)
	{
		updateInfo();
	}
}

void BusOhciCheckComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void BusOhciCheckComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId);
	m_look_online->setColour(ToggleButton::textColourId, backgroundColour.contrasting());
#if defined (WIN32)
	m_show_inactive->setColour(ToggleButton::textColourId,backgroundColour.contrasting());
#endif
	m_verbose->setColour(ToggleButton::textColourId, backgroundColour.contrasting());

	repaint();
}

void BusOhciCheckComponent::updateInfo()
{
#if defined (WIN32)
    int nOSMajor = -1;
    int nOSMinor = -1;
    int nOSServicePack = -1;
    TCHAR szOS[256] = { 0 };
    TCHAR szHW[8192] = { 0 };
#else
    char szOS[256] = { 0 };
    char szHW[8192] = { 0 };
#endif
    
    String info(String::empty);
    
    fwi::OHCI1394_DEV_INFO devInfo = { false, false, false,  { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, { 0 }, 0, 0, 0, { 0 }, { 0 }, 0, 0};
    fwi::FWI_INFO_METHOD find_method = m_look_online->getToggleState() ? fwi::kLookOnline : fwi::kUseBuiltIn;
    
    memset((void *)szOS, 0, sizeof(szOS));
    memset((void *)szHW, 0, sizeof(szHW));
    
    // platform version string
    if ( m_fwi->getOSDisplayString( szOS, sizeof(szOS) ) )
    {
        if (m_verbose->getToggleState())
        {
            info << "OS: " << szOS << "\n";
            
            if ( 0 == m_fwi->getHardwareInfo(szHW, sizeof(szHW)) )
            {
                info << szHW << "\n";
            }
        }
        
#if defined (WIN32)
        if ( (0 == m_fwi->getOSInfo(&nOSMajor, &nOSMinor, &nOSServicePack)) && m_verbose->getToggleState() )
        {
            // driver file versions
            TCHAR szVer[32];
            bool bIsWin7 = ( (6 == nOSMajor) && (1 == nOSMinor) );
            
            if (bIsWin7)
            { 
                info << _T("  Microsoft 1394ohci.sys ");
                if ( 0 != m_fwi->getDriverFileVersion( _T("1394ohci.sys"), szVer, sizeof(szVer)) )
                {
                    info << String::formatted("[%s]\n", szVer);
                }
                else
                {
                    info << "[N/A]\n";
                }
            }
            
            if ( 0 != m_fwi->getDriverFileVersion( _T("ohci1394.sys"), szVer, sizeof(szVer)) )
            {
                info << String::formatted("  Microsoft ohci1394.sys [%s] %s", szVer, bIsWin7 ? "(legacy)\n" : "\n");
            }
            
            if ( 0 != m_fwi->getDriverFileVersion( _T("1394bus.sys"), szVer, sizeof(szVer)) )
            {
                info << String::formatted("  Microsoft 1394bus.sys  [%s] %s", szVer, bIsWin7 ? "(legacy)\n" : "\n");
            }
            
            // check for required WindowsXP hotfixes
            if ( (nOSMajor == 5) && (nOSMinor == 1) )
            {
                if ( 2 == nOSServicePack )
                {
                    info << "\n  Microsoft Windows XP Service Pack 3 or later recommended.\n";
                    info << String::formatted("  Microsoft 1394 Hotfix KB885222 is %s installed.\n", m_fwi->isXP1394SP2HotFixInstalled() ? "correctly"  : "NOT" );
                }
                else if ( (3 == nOSServicePack) && (devInfo.maxspeed == FW_INTERFACES_MAX_SPEED_S800) )
                {
                    if ( (devInfo.sidSpeed < 0) || (devInfo.sidSpeed > 2) )
                    {
                        info << String::formatted("\n  Microsoft 1394 Hotfix KB955408 is %s installed.\n", m_fwi->isXP1394SP3HotFixInstalled() ? "correctly" : "NOT" );
                    }
                }
            }
        }
#endif // WIN32
    }
    
    info << "\nLooking for OHCI 1394 Host Controllers...\n";
    
#if defined (WIN32)
    if ( (5 == nOSMajor) && (1 == nOSMinor) &&  (2 == nOSServicePack) )
    {
        info << "\n*** Online data access requires Windows XP SP3 or later. ***\n";
    }
#endif // WIN32
    
    
    // look for any OHCI 1394 host controllers
    long lResult = 0;
    int index = 0; 
    int numFound = 0;
    
    while ((lResult==FWI_NO_ERROR) || (lResult==FWI_FILTERED))
    {
        fwi::FWI_INFO_FILTER filter = fwi::kAllAdapters;
#if defined (WIN32)
        filter = m_show_inactive->getToggleState() ? fwi::kAllAdapters :  fwi::kActiveAdaptersOnly,
#endif
        lResult = m_fwi->get1394OHCIControllerInfo(index++, 
                                                   find_method, 
                                                   filter,
                                                   &devInfo );
        
        switch (lResult)
        {
			case FWI_ERROR_FILE_NOT_FOUND:
            {
                if (fwi::kLookOnline == find_method)
                {
                    info << "\n\n*** Could not get online data.\nThe online source may not be available, or your computer may not be connected to the Internet.\n";
                }
                break;
            }
			case FWI_ERROR_BAD_FILE_FORMAT:
            {
                if (fwi::kLookOnline == find_method)
                {
                    info << "\n\n*** The online data source is not available.\n";
                }
                break;
            }
			case FWI_ACCESS_ERROR:
            {
                info << "\n\n*** Could not access the Registry. Make sure to run this program with Administrator privileges.\n";
                break;
            }
			case FWI_FILTERED:
				break;
                
			case FWI_NO_ERROR:
            {
                if (devInfo.bValid)
                {
                    ++numFound;
                    
                    info << "\n" << numFound << ":";
                    
                    info << "\n  Vendor : (";
                    info << devInfo.vendorId;
                    info << ") ";
                    info << devInfo.vendorName;
                    
                    info << "\n  Chipset: (";
                    info << devInfo.deviceId;
                    info << ") ";
                    info << devInfo.chipset;
                    
                    info << "\n  Revision: ";
                    info << devInfo.revision;
                    
#if defined (WIN32)
                    if (m_show_inactive->getToggleState())
                    {
                        info << "\n  Status :  ";
                        if (devInfo.bActive)
                        {
                            info << "Active";
                        }
                        else
                        {
                            info << "No longer present, or disabled.";
                        }
                    }
#endif // WIN32
                    
                    if (m_verbose->getToggleState())
                    {
                        info << "\n  Details:";
                        
                        info << "\n    Subsysten VendorId: ";
                        info << String::formatted("%04x", devInfo.subsysVendorId);
                        info << "\n    Subsystem DeviceId: ";
                        info << String::formatted("%04x", devInfo.subsysDeviceId);
                        
                        if (devInfo.bFound)
                        {
                            info << "\n    Max # isoch Rx contexts: ";
                            info << devInfo.maxRx;
                            info << "\n    Max # isoch Tx contexts: ";
                            info << devInfo.maxTx;
                        }
                    }
                    
                    info << "\n  Max 1394 Speed Capability: ";
                    switch (devInfo.maxspeed)
                    {
						case FW_INTERFACES_MAX_SPEED_S100:
							info << "S100";
							break;
						case FW_INTERFACES_MAX_SPEED_S200:
							info << "S200";
							break;
						case FW_INTERFACES_MAX_SPEED_S400:
							info << "S400";
							break;
						case FW_INTERFACES_MAX_SPEED_S800:
							info << "S800";
							break;
                            
						default:
							info << "Unknown.";
							break;
                    }
                    
                    info << "\n  Support:  ";
                    if (devInfo.bFound)
                    {
                        switch (devInfo.support)
                        {
							case FW_INTERFACES_SUPPORT_LEVEL_NONE:
								info << "Not compatible.";
								break;
							case FW_INTERFACES_SUPPORT_LEVEL_FULL:
								info << "Compatible, no known issues.";
								break;
							case FW_INTERFACES_SUPPORT_LEVEL_LIMITED:
								info << "Limited driver support.";
								break;
							case FW_INTERFACES_SUPPORT_LEVEL_UNKNOWN:	// case fall-through
							default:
								info << "This controller has not been qualified yet.";
								break;
                        }
                        
                        if (String::empty != devInfo.notes)
                        {
                            info << String::formatted("\n  Notes  :  %s", devInfo.notes);
                        }
                    }
                    else
                    {
                        info << "No support data available.";
                    }
                    
#if defined (WIN32)
                    if (nOSMajor == 5)
                    {
                        switch (devInfo.sidSpeed)
                        {
							case 0: info << "SidSpeed:  S100  ** Warning **  1394 devices will have limited channel count.\n"; break;
							case 1: info << "SidSpeed:  S200  ** Warning **  1394 devices will have limited channel count.\n"; break;
							case 2: info << "SidSpeed:  S400\n"; break;
							case 3: info << "SidSpeed:  S400/S800\n"; break;
							default:
								if (FW_INTERFACES_MAX_SPEED_S800 == devInfo.maxspeed)
								{
									info << "SidSpeed:  ** Warning **  SidSpeed registry entry is missing or illegal.\n    1394 devices may not work, or may have limited channel count.\n";
								}
								break;
                        }
                    }
#endif // WIN32
                }
                break;
            }
			default:
				break;
        }
        
    }
    info << "\n\nDone.";

	// show the info in a popup box
	TextEditor * alertText = new TextEditor();
	alertText->setMultiLine(true);
	alertText->setBounds(10, 0, 480, 400);
	alertText->setText(info);

	AlertWindow *alert = new AlertWindow("OHCI Controller Info", "", AlertWindow::NoIcon);
	alert->addButton("OK", 0);
	alert->addButton("Save to file", 1);
	alert->centreWithSize(520, 520);
	alert->addCustomComponent(alertText); 
	int retVal = alert->runModalLoop();

	delete alertText;
	delete alert;

	// user wants to save results to file
	if (1 == retVal)
	{
		FileChooser fc ("Save OHCI Info to local file...", 
				File::getSpecialLocation(File::userDocumentsDirectory), 
				"*.txt", 
				true);

		if (fc.browseForFileToSave(false))
		{
			File chosenFile = fc.getResult();
			String cur_working_file = chosenFile.getFullPathName();

			// force extension
			if (cur_working_file.endsWith(".txt") == false)
			{
				cur_working_file.append(".txt", 4);
			}
			File workingFile = File(cur_working_file);

			// check exists
			if (workingFile.exists())
			{
				if (AlertWindow::showOkCancelBox (AlertWindow::WarningIcon,
										 "Save OHCI Info to local file...",
										 cur_working_file + " already exists.\rDo you want to replace it?"))
				{
					workingFile.replaceWithText(info);
				}
			}
			else
			{
				workingFile.replaceWithText(info);
			}
		}
	}
}

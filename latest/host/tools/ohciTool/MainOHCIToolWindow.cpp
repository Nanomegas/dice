///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "MainOHCIToolWindow.h"
#include "juce_TCATLookAndFeel.h"
#include "tcat_dice_defaults.h"
#include "fwi.h"

#ifdef _DEBUG	                               
// debug header for leak detection
// http://www.codeproject.com/KB/applications/visualleakdetector.aspx
// #include "vld.h"
#endif // _DEBUG

//==============================================================================
class OhciTab  : public Component,
                public ButtonListener
{
    //==============================================================================
    MainOHCIToolWindow* m_mainWindow;
	TextButton* m_rescan;
	ToggleButton* m_look_online;
#if defined (WIN32)
	ToggleButton* m_show_inactive;
#endif
	ToggleButton* m_verbose;
	Label* m_info_l;
	TextEditor* m_info;
	fwi * m_fwi;

public:
    //==============================================================================
    OhciTab (MainOHCIToolWindow* mainWindow) : 
        m_mainWindow (mainWindow), 
		m_rescan (0),
		m_look_online (0),
#if defined (WIN32)
		m_show_inactive (0),
#endif
		m_verbose (0),
		m_info_l (0),
		m_fwi (0)
    {
        m_fwi = new fwi();
        
		m_rescan = new TextButton ("Scan now");
        addAndMakeVisible (m_rescan);
        m_rescan->changeWidthToFitText (24);
        m_rescan->addListener (this);
        m_rescan->setTopLeftPosition (10, 30);
        m_rescan->setExplicitFocusOrder(1);
        m_rescan->setWantsKeyboardFocus(true);

		m_look_online = new ToggleButton ("Online");
        addAndMakeVisible (m_look_online);
        m_look_online->addListener (this);
        m_look_online->setBounds (120, 13, 100, 24);
		m_look_online->setButtonText ("Look online for the latest compatibility info");
		m_look_online->changeWidthToFitText();
        m_look_online->setExplicitFocusOrder(2);
        m_look_online->setWantsKeyboardFocus(true);

#if defined (WIN32)
		m_show_inactive = new ToggleButton ("ShowAll");
        addAndMakeVisible (m_show_inactive);
        m_show_inactive->addListener (this);
        m_show_inactive->setBounds (120, 65, 100, 24);
		m_show_inactive->setButtonText ("Show inactive devices");
		m_show_inactive->changeWidthToFitText();
        m_show_inactive->setExplicitFocusOrder(3);
        m_show_inactive->setWantsKeyboardFocus(true);
#endif // WIN32
		
		m_verbose = new ToggleButton ("Verbose");
        addAndMakeVisible (m_verbose);
        m_verbose->addListener (this);
        m_verbose->setBounds (120, 39, 100, 24);
		m_verbose->setButtonText ("Show additional details");
		m_verbose->changeWidthToFitText();
        m_verbose->setExplicitFocusOrder(4);
        m_verbose->setWantsKeyboardFocus(true);

		m_info = new TextEditor ("Results editor");
		addAndMakeVisible (m_info);
		m_info->setText ("No results yet.",false);
		m_info->setMultiLine(true);
		m_info->setFont(Font("Lucida Console", 10.0f, Font::plain));
        m_info->setExplicitFocusOrder(4);
        m_info->setWantsKeyboardFocus(true);

		m_info_l = new Label (String::empty, "Results label");
		addAndMakeVisible (m_info_l);
		m_info_l->setBounds (10, 87, 100, 24);
		m_info_l->setText("Results:",false);
        m_info_l->setWantsKeyboardFocus(false);
#if defined (WIN32)
        m_info->setFont(Font(13.0f, Font::plain));
#else
        m_info->setFont(Font("Monaco", 13.0f, Font::plain));
#endif // WIN32
		
	}

    ~OhciTab()
    {
        if (m_fwi)
        {
            delete m_fwi;
            m_fwi = 0;
        }
        
        // (need to do this because the default look-and-feel object is one of our members,
        // so will be deleted with us, and would leave a dangling pointer)
        deleteAllChildren();

    }

    
    //==============================================================================
    void resized()
    {
		m_info->setBounds (10, 110, getWidth()-20, getHeight()-120 );
    }

    void paint (Graphics& g)
    {
		float w = (float)getWidth();
		float h = (float)getHeight();
		float x1 = (float)(getX());
		float y1 = (float)(getY());
        float x2 = x1 + w;
        float y2 = y1 + h;
        Path p;
        Colour colour1(0xfffbfbfb);
        Colour colour2(0xffe3e3e3);
        
		ColourGradient cg (colour1, 0, y1, colour2, 0, 100, false);
        
        p.addRoundedRectangle(x1, y1, x2, y2, 3.0f);
        g.setGradientFill(cg);
        g.fillPath(p);
        
		g.setColour(colour2);
		g.fillRect(x1, y1+100, w, h);
    }

	// rescan
	void buttonClicked (Button* buttonThatWasClicked)
    {
		if (buttonThatWasClicked == m_rescan)
		{
			updateInfo();
		}
    }

	void updateInfo()
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
		m_info->setText(info,false);
	}

	juce_UseDebuggingNewOperator
};




//==============================================================================
class AboutTab  : public Component
{
    //==============================================================================
    MainOHCIToolWindow* m_mainWindow;
	TextEditor * m_info;

public:
    //==============================================================================
    AboutTab (MainOHCIToolWindow* mainWindow)
        : m_mainWindow (mainWindow)
    {        

		m_info = new TextEditor ();
		addAndMakeVisible (m_info);
		m_info->setText ("",false);
        m_info->setMultiLine(true);

		String str;
		str = "TCAT OHCI1394 Tool\n";
		str +=kTCAT_DICE_USR_COPYRIGHT_STR;
		str += "\n";
		str +="Version: ";
		str +=kTCAT_DICE_VERSION_EXTENDED_STR;
		m_info->setText (str,false);
    }

    ~AboutTab()
    {
        // (need to do this because the default look-and-feel object is one of our members,
        // so will be deleted with us, and would leave a dangling pointer)
        deleteAllChildren();

    }

	void resized()
	{
		m_info->setBounds (10, 10, getWidth()-20, getHeight()-20 );
	}

    void paint (Graphics& g)
    {
		float w = (float)getWidth();
		float h = (float)getHeight();
		float x1 = (float)(getX());
		float y1 = (float)(getY());
        float x2 = x1 + w;
        float y2 = y1 + h;
        Path p;
        Colour colour1(0xfffbfbfb);
        Colour colour2(0xffe3e3e3);
        
        ColourGradient cg (colour1, 0, y1, colour2, 0, 100, false);
        
        p.addRoundedRectangle(x1, y1, x2, y2, 3.0f);
        g.setGradientFill(cg);
        g.fillPath(p);
        
        g.setColour(colour2);
		g.fillRect(x1, y1+100, w, h);
    }

    juce_UseDebuggingNewOperator
};


//==============================================================================
class ContentComp  : public Component
{
    //==============================================================================
    MainOHCIToolWindow *		m_mainWindow;
	TabbedComponent *	m_tabs;
	OhciTab *			m_ohciTab;
	AboutTab *			m_aboutTab;

    TCATLookAndFeel tcatLookAndFeel;

 
    TooltipWindow tooltipWindow; // to add tooltips to an application, you
                                 // just need to create one of these and leave it
                                 // there to do its work..

    enum CommandIDs
    {
        useNativeTitleBar          = 0x200d
    };

public:
    //==============================================================================
    ContentComp (MainOHCIToolWindow* mainWindow)
        : m_mainWindow (mainWindow)
    {
        LookAndFeel::setDefaultLookAndFeel (&tcatLookAndFeel);
 
        tcatLookAndFeel.setColour (TextButton::buttonColourId, Colours::lightgrey.brighter());
        tcatLookAndFeel.setColour (TextEditor::focusedOutlineColourId, Colours::white);

	    m_tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
		m_tabs->setColour(TabbedComponent::backgroundColourId, Colours::lightgrey);

	    addAndMakeVisible (m_tabs);
		m_tabs->setBounds (0, 0, getWidth(), getHeight());
	
		//Now add the sub components
		m_ohciTab = new OhciTab(m_mainWindow);
		m_tabs->addTab ("OHCI Info", Colours::white, m_ohciTab, true);
    
		m_aboutTab = new AboutTab(m_mainWindow);
		m_tabs->addTab ("About", Colours::white, m_aboutTab, true);

	}

    ~ContentComp()
    {
        // (need to do this because the default look-and-feel object is one of our members,
        // so will be deleted with us, and would leave a dangling pointer)
        LookAndFeel::setDefaultLookAndFeel (0);

        deleteAllChildren();

    }

    //==============================================================================
    void resized()
    {
		m_tabs->setBounds (0, 0, getWidth(), getHeight()); 
    }
    
	void paint (Graphics &g)
	{
		return; // bkbk
		float w = (float)getWidth();
		float h = (float)getHeight();
		float x1 = (float)(getX());
		float y1 = (float)(getY());
		float x2 = x1 + w;
		float y2 = y1 + h;
		Path p;
		Colour colour1(0xfffbfbfb);
		Colour colour2(0xffe3e3e3);

		ColourGradient cg (colour1, 0, y1, colour2, 0, 100, false);

		p.addRoundedRectangle(x1, y1, x2, y2, 3.0f);
		g.setGradientFill(cg);
		g.fillPath(p);

		g.setColour(colour2);
		g.fillRect(x1, y1+100, w, h);

		g.setColour(Colours::grey);
		g.drawRoundedRectangle(x1, y1, x2, y2, 3.0f, 1.0f);
	}

    juce_UseDebuggingNewOperator
};


//==============================================================================
MainOHCIToolWindow::MainOHCIToolWindow()
    : DocumentWindow ("TCAT OHCI 1394 Tool",
					  Colours::white,
                      DocumentWindow::allButtons,
                      true)
{
    commandManager = new ApplicationCommandManager();

    setResizable( true, true ); // resizability is a property of ResizableWindow

    ContentComp* contentComp = new ContentComp( this );

    // sets the main content component for the window to be this tabbed
    // panel. This will be deleted when the window is deleted.
    setContentOwned( contentComp, false );

	setUsingNativeTitleBar( true );

    setVisible( true );
}


MainOHCIToolWindow::~MainOHCIToolWindow()
{
    // setting our content component to 0 will delete the current one, and
    // that will in turn delete all its child components. You don't always
    // have to do this explicitly, because the base class's destructor will
    // also delete the content component, but in this case we need to
    // make sure our content comp has gone away before deleting our command
    // manager.
    setContentOwned( 0, false );
    delete commandManager;
}


void MainOHCIToolWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}


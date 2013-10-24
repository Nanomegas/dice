///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 - 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "MainCPLWindow.h"
#include "Events.h"
#include "Listener.h"
#include "juce_TCATLookAndFeel.h"
#include "BusWindow.h"
#include "DeviceGeneralComponent.h"
#include "DeviceWindow.h"
#include "DeviceList.h"
#include "appDefaults.h"
#include "EventLogger.h"

// The System Class
//==============================================================================
// this is the one and only derived system class:
// it is not an event class since we have only one listener.
// so it is the listener by itself.

class ContentComp;
class TheSystem: 
	public tcat::dice::system<event_bus>,
	public ActionBroadcaster
{
	typedef tcat::dice::system<event_bus> super;
	friend class ContentComp;
	
public:
	bool m_startup_file_error;

	TheSystem ()
	{
		m_startup_file_error = false;
	}
	virtual	void	update_buslist() ///< Notifies the system object of bus(driver) arrival/removal
	{
		super::update_buslist(); 
		// by calling this all notifiers of all busses and devices will be called to check 
		// for changes and to call their listeners...
		// the only thing for sure here is: there was a device arrival/removal event
		// we don't know if a bus was created or removed or if nothing happened to the busses.
		// the only way to find out is to cache the bus list.
		
		tcat::dice::lock lock(this); // protect against changes to the bus list.

		bool bFound = false;
		for (iterator bus=begin(); bus!=end(); bus++)
		{
			if ((*bus)->size() > 0)
			{
				m_bus_id = (*bus)->id();
				bFound = true;
				sendActionMessage("bus changed");
				break;	// for now, just use the first found bus
			}
		}
		if (!bFound)
		{
			sendActionMessage("bus changed");
		}
	}

	void exception (const tcat::exception::base& exception)	
	{
		m_startup_file_error = true;
	}

private:
	tcat::dice::bus_impl::ID m_bus_id;
};
typedef tcat::dice::reference<TheSystem>	TheSystem_ref;	


// The Main Content Component
//==============================================================================
class ContentComp : 
		public Component,
		public ActionListener
{
	TheSystem_ref				m_system;
	BusWindow*					busWindow;
	DeviceWindow*				devWindow;
	TooltipWindow				tooltipWindow;
	tcat::dice::bus_impl::ID	busID;
	
public:
	ContentComp () : 
		m_system(0), 
		busWindow(0), 
		devWindow(0)
	{
		// create the system object
		try
		{
			m_system = TheSystem::static_create<TheSystem>();
		}		
		catch(tcat::dice::xptn_driver_version xptn)
		{
			tcat::exception::base * exception = dynamic_cast<tcat::exception::base *> (&xptn);
			String xptnStr = exception->usr_msg().c_str();
			AlertWindow::showMessageBox (AlertWindow::NoIcon,
				kTCAT_DICE_USR_STR,
				"Error: "  + xptnStr 
									+ "\n\nThis application requires driver version " 
									+ TCAT_STRINGIFY(kTCAT_DICE_VERSION_MAJOR) 
									+ "." + TCAT_STRINGIFY(kTCAT_DICE_VERSION_MINOR) 
									+ "." + TCAT_STRINGIFY(kTCAT_DICE_VERSION_SUB) 
									+ "\n\nPlease check the driver configuration.");
			JUCEApplication::quit();
		}
		catch (tcat::exception::base exception)
		{
			String xptnStr = exception.usr_msg().c_str();
			AlertWindow::showMessageBox (AlertWindow::NoIcon,
										 kTCAT_DICE_USR_STR,
										 "Driver error: " + xptnStr + "\n\nCheck the driver configuration or device connections.");
			JUCEApplication::quit();
		}
	
		if (m_system)
		{
			busWindow = new BusWindow ();
			addAndMakeVisible (busWindow);
			devWindow =  new DeviceWindow ();
			addAndMakeVisible (devWindow);

			// the bus general tab wants action messages from the device general component
			devWindow->m_devgeneral->addActionListener (busWindow->m_busgeneral);
#ifdef WIN32
			// the bus wdm tab wants action messages from the device general component
			devWindow->m_devgeneral->addActionListener (busWindow->m_buswdm);
			// the bus wdm tab wants action messages from the device list component
			devWindow->m_devlist->addActionListener (busWindow->m_buswdm);
#endif
			// the device general tab wants action messages from the bus general window
			busWindow->m_busgeneral->addActionListener (devWindow->m_devgeneral);

			// the main content component wants action messages from the system object
			m_system->addActionListener(this);

			if (0 == m_system->size())
			{
				AlertWindow::showMessageBox (AlertWindow::NoIcon,
					kTCAT_DICE_USR_STR,
					"No devices found.\n\nConnect device(s).");
			}
			else
			{
#ifdef __MACH__
				for (int i=0; i<(int)m_system->size(); i++)
				{
					if (0 == m_system->at(i)->size())
					{
						AlertWindow::showMessageBox (AlertWindow::NoIcon,
							kTCAT_DICE_USR_STR,
							"No devices found.\n\nConnect device(s).");
					}
				}
#endif	// __MACH__
				m_system->update_buslist();
			}
		}
		else
		{
			AlertWindow::showMessageBox (AlertWindow::NoIcon,
				kTCAT_DICE_USR_STR,
				"Driver not found.\n\nPlease check the driver configuration.");
			JUCEApplication::quit();
		}
	
		if (m_system && m_system->m_startup_file_error)
		{
			AlertWindow::showMessageBox (AlertWindow::NoIcon,
				kTCAT_DICE_USR_STR,
				"Error: Could not open driver settings file.\nChanges to settings cannot be saved.\n\nContact technical support for more information.");
			m_system->m_startup_file_error = false;
		}
	}
	
	void actionListenerCallback(const String &message)
	{
		if (0 == message.compare("bus changed"))
		{
			for (int i=0; i<(int)m_system->size(); i++)
			{
				// there is at least one bus (for WIN32 at least one device exists for which a driver is installed)
#ifdef WIN32
				if (m_system->at(i)->size() > 0)	// assume all devices are on the first bus
#endif
				{
					// attach the bus
					event_bus_ref busref = m_system->at(i);
					if (busref)
					{
						busref->attach(busWindow);
						busref->attach(devWindow);
					}
					break;	// for now, just use the first found bus
				}
			}
		}
	}
	
	void resized()
	{
        if (busWindow) busWindow->setBounds (10,10,getWidth() - 20, 200);
	    if (devWindow) devWindow->setBounds (10,160,getWidth() - 20, getHeight()-140);
	}

	bool keyPressed  (const KeyPress & key)
	{
		if (key.isKeyCode (KeyPress::F5Key))
		{
			if (appDefaults->getAppSettings()->getBoolValue("F5RefreshesColours"))
			{
				appDefaults->closeDefaultsFile();	// cause values to be reread from file
				TCATLookAndFeel::getInstance()->initColours();
				TCATLookAndFeel::getInstance()->dumpColours();
				LookAndFeel::setDefaultLookAndFeel (TCATLookAndFeel::getInstance());
			}
			return true;
		}
		return false;
	}

	void paint (Graphics& g)
	{
		int h = getHeight();
		int w = getWidth();
		Path p;

		// background
		g.fillAll(findColour(TCATLookAndFeel::tcatWindowBackgroundColourId));

		// curved corners
		g.setColour(findColour(TCATLookAndFeel::tcatTitleBarColourId));
		p.startNewSubPath(0, 0);
		p.lineTo(0, 10.0f);
		p.addArc(0, 0, 10.0f, 10.0f, float_Pi * 1.5f, float_Pi * 2.0f);
		p.closeSubPath();
		g.fillPath(p);

		p.clear();
		p.startNewSubPath((float)w, 0);
		p.lineTo((float)w-10, 0);
		p.addArc((float)w-10, 0, 10.0f, 10.0f, 0, float_Pi * 0.5f);
		p.closeSubPath();
		g.fillPath(p);

		// top area for global settings
		g.setColour(findColour(TCATLookAndFeel::tcatGlobalAreaBackgroundColourId));
		p.clear();
		p.addRoundedRectangle(5.0f, 5.0f, (float)(w-10), 172.0f, 4.0f);
		g.fillPath (p);

		// header for global settings
		g.setColour(findColour(TCATLookAndFeel::tcatGlobalHeadingColourId));
		p.clear();
		p.addRoundedRectangle(10.0f, 10.0f, (float)(w-20), 24.0f, 4.0f);
		g.fillPath (p);

		// tabs area for global settings
		p.clear();
		g.setColour(findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId));
		p.addRoundedRectangle(10.0f, 39.0f, (float)(w-20), 133.0f, 4.0f);
		g.fillPath (p);

		g.setColour(findColour(TCATLookAndFeel::tcatGlobalHeadingColourId).contrasting());
		g.setFont(15.0, Font::bold);
		g.drawFittedText ("Global Settings",
					10, 10, w-20, 24,
					Justification::centred, 1, 1.0f);

		// bottom area for device tabs
		p.clear();
		g.setColour(findColour(TCATLookAndFeel::tcatDeviceAreaBackgroundColourId));
		p.addRoundedRectangle(5.0f, 185.0f, (float)(w-10), (float)(h-190), 4.0f);
		g.fillPath (p);

		// header for device settings
		p.clear();
		g.setColour(findColour(TCATLookAndFeel::tcatDeviceHeadingColourId));
		p.addRoundedRectangle(10.0f, 190.0f, (float)(w-20), 24.0f, 4.0f);
		g.fillPath (p);

		// tabs area for device settings
		p.clear();
		g.setColour(findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId));
		p.addRoundedRectangle(10.0f, 219.0f, (float)(w-218), (float)(h-229), 4.0f);
		g.fillPath (p);

		g.setColour(findColour(TCATLookAndFeel::tcatDeviceHeadingColourId).contrasting());
		g.drawFittedText ("Device Settings",
					10, 190, w-20, 24,
					Justification::centred, 1, 1.0f);

		// header for device list area
		p.clear();
		g.setColour(findColour(TCATLookAndFeel::tcatDevicesHeadingColourId));
		p.addRoundedRectangle((float)(w-197), 228.0f, 182, 24.0f, 3.0f);
		g.fillPath (p);

		g.setColour(findColour(TCATLookAndFeel::tcatDevicesHeadingColourId).contrasting());
		g.drawFittedText ("Devices",
					w-198, 228, 182, 24,
					Justification::centred, 1, 1.0f);

		g.setColour(findColour(TCATLookAndFeel::tcatDeviceAreaBackgroundColourId).contrasting());
		g.drawFittedText ("no devices found",
				w-198, 265, 182, 34,
				Justification::centred, 1, 0.7f);
	}

	~ContentComp()
	{
        if (busWindow) delete busWindow;
        if (devWindow) delete devWindow;
		deleteAllChildren();
		if (m_system)
		{
			m_system->close();
		}
	}

	void LookAndFeelChanged()
	{
		repaint();
	}
};


// The Top Level Window
//==============================================================================
/** This is the top-level window that we'll pop up. Inside it, we'll create and
    show a content component.
*/
MainCplWindow::MainCplWindow(): 
DocumentWindow (kTCAT_DICE_USR_STR, Colours::white, DocumentWindow::allButtons, true)
{
    ContentComp * contentComp = new ContentComp();
    setContentOwned (contentComp, false);
	setBackgroundColour(findColour(TCATLookAndFeel::tcatWindowBackgroundColourId));
}

MainCplWindow::~MainCplWindow()
{
}

void MainCplWindow::closeButtonPressed()
{
    // The correct thing to do when you want the app to quit is to call the
    // JUCEApplication::systemRequestedQuit() method.

    // That means that requests to quit that come from your own UI, or from other
    // OS-specific sources (e.g. the dock menu on the mac) all get handled in the
    // same way.
	JUCEApplication::getInstance()->systemRequestedQuit();
}


//==============================================================================
juce_ImplementSingleton (AppWindow)

AppWindow::AppWindow () : mainWindow (0)
{
}

AppWindow::~AppWindow()
{
	mainWindow = 0;
	clearSingletonInstance();
}

void AppWindow::setMainWindowSize (int width, int height)
{
	if (mainWindow)
	{
		mainWindow->setSize(width, height);
	}
}

void AppWindow::setMainWindow (MainCplWindow * _mainWindow)
{
	mainWindow = _mainWindow;
}


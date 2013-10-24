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

#include "tcat_dice_pal_system.h"
#include "MainCplWindow.h"
#include "appDefaults.h"
#include "EventLogger.h"
#include "juce_TCATLookAndFeel.h"

#ifdef WIN32
#ifdef _DEBUG	                               
// debug header for leak detection
// http://www.codeproject.com/KB/applications/visualleakdetector.aspx
// #include "vld.h"
#endif // _DEBUG
#endif // WIN32

ApplicationProperties* appProperties = nullptr;
AppDefaults* appDefaults = nullptr;

//==============================================================================
/** This is the application object that is started up when Juce starts. It handles
 the initialisation and shutdown of the whole application.
*/
class DiceCplApplication: public JUCEApplication
{
    MainCplWindow* theMainWindow;
	
public:
	DiceCplApplication(): theMainWindow(0)
	{
	}
	
	void initialise (const String& commandLine)
	{
		appProperties = new ApplicationProperties();
		appDefaults = new AppDefaults();
		
		// specify the defaults file name and format
		// the installer for this application will have placed the file in the appropriate directory by now
		appDefaults->setStorageParameters("cpl", "defs");
		
        // initialise our settings file..
		
		PropertiesFile::Options options;
		options.applicationName     = kTCAT_DICE_DRV_STR;
		options.filenameSuffix      = "settings";
		options.osxLibrarySubFolder = "Preferences";
		options.storageFormat		= PropertiesFile::storeAsXML;
		options.millisecondsBeforeSaving = 1000;
		
		appProperties->setStorageParameters (options);
		
		// specify settings that will be used in case of missing defaults file
		PropertySet * defaults = appDefaults->getDefaultSettings();
		
		// visibility of tabs
		defaults->setValue("showSimpleTab", var(false));
		defaults->setValue("showAdvancedTab", var(false));
		defaults->setValue("showTestTab", var(false));
		defaults->setValue("showDPC", var(false));
		defaults->setValue("showSystem", var(false));
		
		// visiblity of controls
		// deprecated		defaults->setValue("showDPCInducer", false);
		defaults->setValue("hideSyncsIfSingle", true);
		defaults->setValue("showMeasuredSampleRate", true);

		// behaviors
		defaults->setValue("allowLoadAfterWarning", var(false));
		defaults->setValue("defaultFrmwSector", String("dice"));
		// if your devices have different application sectornames, enter them here 
		// the 4 digit number is the product_id
		//		defaults->setValue("frmwSector0002", "dice");
		
		// vendor strings
		defaults->setValue("vendorInfo1", String(""));
		defaults->setValue("vendorInfo2", String(""));
		defaults->setValue("vendorInfo3", String(""));
		
		// GUI generated colors
		defaults->setValue("F5RefreshesColours", var(false));	// using F5 key, allows quick editing/viewing of colour changes in cpl.defs
		
		defaults->setValue("UseGeneratedColourShades", true);	// if true, specified colors are ignored
		defaults->setValue("BaseColourHue", String("50")); // (0-360) for standard hues, (above 360) forces grey
		defaults->setValue("BaseColourValue", String("75")); // (0-255) when using hues or grey, specifies starting brightness value - 0 darkest, 255 brightest
		defaults->setValue("BaseColourSaturation", String("30")); // (0-255) when using hues, specifies the starting saturation
		
		defaults->setValue("UseBlendedColours", var(false));	// if true, generates colurs by blending BlendColour1 with BlendColour2
		defaults->setValue("BlendColour1", String("FFFF99")); // rgb RRGGBB
		defaults->setValue("BlendColour2", String("000033")); // rgb RRGGBB
		
		// GUI specified colors - a greyscale scheme - equivalent to H=361, V=25 above
		defaults->setValue("TitleBarButton", String("191919"));
		defaults->setValue("ToolTipText", String("191919"));
		defaults->setValue("WindowBackground", String("3F3F3F"));
		defaults->setValue("ToolTipOutline", String("3F3F3F"));
		defaults->setValue("ComboOutline", String("3F3F3F"));
		defaults->setValue("TextboxOutline", String("3F3F3F"));
		defaults->setValue("TitleBar", String("0C0C0C"));
		defaults->setValue("GlobalAreaBackground", String("0C0C0C"));
		defaults->setValue("DeviceAreaBackground", String("0C0C0C"));
		defaults->setValue("GlobalHeading", String("262626"));
		defaults->setValue("DeviceHeading", String("262626"));
		defaults->setValue("GlobalTabArea", String("262626"));
		defaults->setValue("DeviceTabArea", String("262626"));
		defaults->setValue("DevicesHeading", String("404040"));
		defaults->setValue("GlobalTabButton", String("404040"));
		defaults->setValue("DeviceTabButton", String("404040"));
		defaults->setValue("PopupTitleBar", String("404040"));
		defaults->setValue("PopupWindowBackground", String("404040"));
		defaults->setValue("PopupMenuBackground", String("5A5A5A"));
		defaults->setValue("TextButton", String("5A5A5A"));
		defaults->setValue("TextboxBackground", String("5A5A5A"));
		defaults->setValue("ComboBackground", String("5A5A5A"));
		defaults->setValue("ListboxOutline", String("5A5A5A"));
		defaults->setValue("DeviceOutline", String("747474"));
		defaults->setValue("ToggleButton", String("747474"));
		defaults->setValue("GroupOutline", String("8E8E8E"));
		defaults->setValue("ProgressBarBackground", String("8E8E8E"));
		defaults->setValue("ProgressBarForeground", String("8E8E8E"));
		defaults->setValue("ListboxBackground", String("8E8E8E"));
		defaults->setValue("ToolTipBackground", String("CECECE"));
		defaults->setValue("DeviceBackground", String("CECECE"));
		
		LookAndFeel::setDefaultLookAndFeel (TCATLookAndFeel::getInstance());
		
		theMainWindow = new MainCplWindow();
		
 	    theMainWindow->setResizable (false,false);
		
		// set initial size and position for first-run case
        theMainWindow->centreWithSize (700, 640);	
		// get the last saved position
		theMainWindow->restoreWindowStateFromString(appProperties->getUserSettings()->getValue("mainWindowPos"));;
		
		theMainWindow->lookAndFeelChanged();
		theMainWindow->setVisible (true);
		
		AppWindow::getInstance()->setMainWindow(theMainWindow);
		
		// initialise the event logger
		EventLogWindow::getInstance();
		EventLogger::getInstance()->logMessage("Event log started.");
	}
	
	void shutdown()								
	{
		EventLogWindow::deleteInstance();
		String windowDimensions = theMainWindow->getWindowStateAsString();
		// strip off width and height and force the saved size
		int i = windowDimensions.lastIndexOfChar(' ');
		if (i > 0)
		{
			windowDimensions = windowDimensions.substring(0, i);
			i = windowDimensions.lastIndexOfChar(' ');
			if (i > 0)
			{
				windowDimensions = windowDimensions.substring(0, i);
			}
		}
		windowDimensions = windowDimensions + " 700 640";
	    appDefaults->closeFiles();
		appProperties->getUserSettings()->setValue("mainWindowPos", windowDimensions);
		appProperties->closeFiles();
		deleteAndZero(appDefaults);
		deleteAndZero(appProperties);
		
		if (theMainWindow) delete theMainWindow;
	}
	const String getApplicationName()			{ return kTCAT_DICE_USR_STR; }
	const String getApplicationVersion()		{ return kTCAT_DICE_VERSION_EXTENDED_STR;	}
	
#ifdef _DEBUG
	bool moreThanOneInstanceAllowed()			{ return true; }
#else
	bool moreThanOneInstanceAllowed()			{ return false; }
#endif
	
	void anotherInstanceStarted (const String& commandLine) { theMainWindow->toFront(true); }
};


//==============================================================================
// This macro creates the application's main() function..
START_JUCE_APPLICATION (DiceCplApplication)

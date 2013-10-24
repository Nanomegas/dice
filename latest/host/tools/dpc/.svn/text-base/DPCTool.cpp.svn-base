///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "DPCTool_headers.h"
#include "mainDPCWindow.h"
#include "..\..\..\interface\tcat_dice_defaults.h"

//==============================================================================
class DiceCplApplication : public JUCEApplication
{
    /* Important! NEVER embed objects directly inside your JUCEApplication class! Use
       ONLY pointers to objects, which you should create during the initialise() method
       (NOT in the constructor!) and delete in the shutdown() method (NOT in the
       destructor!)

       This is because the application object gets created before Juce has been properly
       initialised, so any embedded objects would also get constructed too soon.
   */
    MainDPCWindow* theMainWindow;

public:
    //==============================================================================
    DiceCplApplication()
        : theMainWindow (0)
    {
        // NEVER do anything in here that could involve any Juce function being called
        // - leave all your startup tasks until the initialise() method.
    }

    ~DiceCplApplication()
    {
        // Your shutdown() method should already have done all the things necessary to
        // clean up this app object, so you should never need to put anything in
        // the destructor.

        // Making any Juce calls in here could be very dangerous...
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
        // just create the main window...
        theMainWindow = new MainDPCWindow();
        theMainWindow->centreWithSize (340, 400);
        theMainWindow->setVisible (true);

        // this little function just demonstrates a few system info calls
        Logger::outputDebugString (collectSomeSystemInfo());

        /*  on return from this method, the app will go into its the main event
            dispatch loop, and this will run until something calls
            JUCEAppliction::quit().

            In this case, JUCEAppliction::quit() will be called by the
            demo window when the user clicks on its close button.
        */

    }

    void shutdown()
    {
        delete theMainWindow;
        theMainWindow = 0;
    }

    //==============================================================================
    const String getApplicationName()
    {
        return "TCAT DPC Tool";
    }

    const String getApplicationVersion()
    {
        return kTCAT_DICE_VERSION_EXTENDED_STR;
    }

    bool moreThanOneInstanceAllowed()
    {
        return false;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        // This will get called if the user launches another copy of the app, but
        // there's nothing that the demo app needs to do here.
    }

private:
    //==============================================================================
    // this little function just demonstrates a few system info calls
    static const String collectSomeSystemInfo()
    {
        String systemInfo;

        systemInfo
          << "Time and date: " << Time::getCurrentTime().toString (true, true)
          << "\nUser logon name: " << SystemStats::getLogonName()
          << "\nFull user name: " << SystemStats::getFullUserName()
          << "\nOperating system: " << SystemStats::getOperatingSystemName()
          << "\nCPU vendor: " << SystemStats::getCpuVendor()
          << "\nCPU speed: " << SystemStats::getCpuSpeedInMegaherz() << "MHz"
          << "\nNumber of CPUs: " << SystemStats::getNumCpus()
          << "\nCPU has MMX: " << (SystemStats::hasMMX() ? "yes" : "no")
          << "\nCPU has SSE: " << (SystemStats::hasSSE() ? "yes" : "no")
          << "\nCPU has SSE2: " << (SystemStats::hasSSE2() ? "yes" : "no")
          << "\nCPU has 3DNOW: " << (SystemStats::has3DNow() ? "yes" : "no")
          << "\nMemory size: " << SystemStats::getMemorySizeInMegabytes() << "MB"
          << "\nFound network card MAC addresses: " << getMacAddressList()
          << "\nCurrent executable file: " << File::getSpecialLocation (File::currentExecutableFile).getFullPathName()
          << "\nCurrent application file: " << File::getSpecialLocation (File::currentApplicationFile).getFullPathName()
          << "\nCurrent working directory: " << File::getCurrentWorkingDirectory().getFullPathName()
          << "\nUser home directory: " << File::getSpecialLocation (File::userHomeDirectory).getFullPathName()
          << "\nUser documents directory: " << File::getSpecialLocation (File::userDocumentsDirectory).getFullPathName()
          << "\nUser application data directory: " << File::getSpecialLocation (File::userApplicationDataDirectory).getFullPathName()
          << "\nCommon application data directory: " << File::getSpecialLocation (File::commonApplicationDataDirectory).getFullPathName()
          << "\nTemp directory: " << File::getSpecialLocation (File::tempDirectory).getFullPathName()
          << "\n\n";

        return systemInfo;
    }

    static const String getMacAddressList()
    {
        Array <MACAddress> macAddresses;
        MACAddress::findAllAddresses (macAddresses);

        StringArray addressStrings;
        for (int i = 0; i < macAddresses.size(); ++i)
            addressStrings.add (macAddresses[i].toString());

        return addressStrings.joinIntoString (", ");
    }
};


//==============================================================================
/*
    This macro creates the application's main() function..
*/
START_JUCE_APPLICATION (DiceCplApplication)


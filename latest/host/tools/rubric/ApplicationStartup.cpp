///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "rubric_headers.h"
#include "MainWindow.h"

//==============================================================================
class RubricApplication : public JUCEApplication
{
    /* Important! NEVER embed objects directly inside your JUCEApplication class! Use
       ONLY pointers to objects, which you should create during the initialise() method
       (NOT in the constructor!) and delete in the shutdown() method (NOT in the
       destructor!)

       This is because the application object gets created before Juce has been properly
       initialised, so any embedded objects would also get constructed too soon.
   */
    MainWindow* theMainWindow;

public:
    //==============================================================================
    RubricApplication()
        : theMainWindow (0)
    {
        // NEVER do anything in here that could involve any Juce function being called
        // - leave all your startup tasks until the initialise() method.
    }

    ~RubricApplication()
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
        theMainWindow = new MainWindow();
        theMainWindow->centreWithSize (1000, 800);
        theMainWindow->setVisible (true);

        /*  on return from this method, the app will go into its the main event
            dispatch loop, and this will run until something calls
            JUCEAppliction::quit().

            In this case, JUCEAppliction::quit() will be called by the
            window when the user clicks on its close button.
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
        return "Rubric (Beta)";
    }

    const String getApplicationVersion()
    {
        return "0.75";
    }

    bool moreThanOneInstanceAllowed()
    {
        return false;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        // This will get called if the user launches another copy of the app, but
        // there's nothing that the app needs to do here.
    }

private:
    //==============================================================================
};


//==============================================================================
/*
    This macro creates the application's main() function..
*/
START_JUCE_APPLICATION (RubricApplication)

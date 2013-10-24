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

#pragma once

#include "JuceHeader.h"

//==============================================================================
class MainCplWindow: public DocumentWindow
{
public:
    //==============================================================================
    MainCplWindow();
    ~MainCplWindow();

	//==============================================================================
    // called when the close button is pressed or esc is pushed
    void closeButtonPressed();

    juce_UseDebuggingNewOperator

private:
	Path windowPath;
};


// singleton to allow main window to be resized within the application
class AppWindow : public DeletedAtShutdown
{
public:
    //==============================================================================
	AppWindow();
    ~AppWindow();

    //==============================================================================
    juce_DeclareSingleton (AppWindow, true);

	//==============================================================================
    void setMainWindowSize (int width, int height);
    void setMainWindow (MainCplWindow * _mainWindow);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
	MainCplWindow * mainWindow;

	AppWindow (const AppWindow&);
	const AppWindow& operator= (const AppWindow&);

};


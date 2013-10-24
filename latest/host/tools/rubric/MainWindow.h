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


#ifndef __MAINWINDOW_JUCEHEADER__
#define __MAINWINDOW_JUCEHEADER__


//==============================================================================
class MainWindow  : public DocumentWindow
{
public:
    //==============================================================================
    MainWindow();
    ~MainWindow();

    //==============================================================================
    // called when the close button is pressed or esc is pushed
    void closeButtonPressed();
    
    // the command manager object used to dispatch command events
    ApplicationCommandManager* commandManager;
    DescriptorTreeView* GetCurrentView();
};

extern DescriptorTreeView* GetCurrentView();

#endif   // __MAINWINDOW_JUCEHEADER__

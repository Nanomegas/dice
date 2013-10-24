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
#include "DescriptorTreeView.h"

//==============================================================================
class ContentComp  : public Component,
                     public MenuBarModel,
                     public ApplicationCommandTarget
{
    //==============================================================================
    MainWindow* mainWindow;

    DescriptorTreeView* currentDescriptorView;

    TooltipWindow tooltipWindow; // to add tooltips to an application, you
                                 // just need to create one of these and leave it
                                 // there to do its work..

    String currentWorkingFile;

    
    enum CommandIDs
    {
        fileNew                 = 0x2000,
        fileOpen                = 0x2001,
        fileSave                = 0x2002,
        fileSaveAs              = 0x2003,
        fileRevert              = 0x2004,
        showTreeView            = 0x2006,
        viewRefresh             = 0x2010,
        viewCondense            = 0x2011,
        viewExpand              = 0x2012,
        showAbout               = 0x2020,
    };

    friend class DescriptorTreeViewItem;
    friend class DescriptorTreeView;
    
public:
    //==============================================================================
    ContentComp (MainWindow* mainWindow_)
        : mainWindow (mainWindow_),
          currentDescriptorView (0)
    {
        invokeDirectly (showTreeView, true);
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
        if (currentDescriptorView != 0)
            currentDescriptorView->setBounds (0, 0, getWidth(), getHeight());
    }

    //==============================================================================
    void showDescriptorView (DescriptorTreeView* descriptorView)
    {
        delete currentDescriptorView;
        currentDescriptorView = descriptorView;

        addAndMakeVisible (currentDescriptorView);
        resized();
    }

    //==============================================================================
    const StringArray getMenuBarNames ()
    {
        const char* const names[] = { "File", "View", "Help", 0 };

        return StringArray (names);
    }

    const PopupMenu getMenuForIndex (int menuIndex,
                                     const String& menuName)
    {
        ApplicationCommandManager* const commandManager = mainWindow->commandManager;

        PopupMenu menu;

        if (menuIndex == 0)
        {
            menu.addCommandItem (commandManager, fileNew);
            menu.addCommandItem (commandManager, fileOpen);
            menu.addSeparator();
            menu.addCommandItem (commandManager, fileSave);
            menu.addCommandItem (commandManager, fileSaveAs);
            menu.addCommandItem (commandManager, fileRevert);
            menu.addSeparator();
            menu.addCommandItem (commandManager, StandardApplicationCommandIDs::quit);
        }
        else if (menuIndex == 1)
        {
            menu.addCommandItem (commandManager, viewRefresh);
            menu.addCommandItem (commandManager, viewCondense);
            menu.addCommandItem (commandManager, viewExpand);
        }
        else if (menuIndex == 2)
        {
            menu.addCommandItem (commandManager, showAbout);
        }

        return menu;
    }

    void menuItemSelected (int menuItemID,
                           int topLevelMenuIndex)
    {
        // all our menu items are invoked automatically as commands, so no need to do
        // anything in this callback
    }

    //==============================================================================
    // The following methods implement the ApplicationCommandTarget interface, allowing
    // this window to publish a set of actions it can perform, and which can be mapped
    // onto menus, keypresses, etc.

    ApplicationCommandTarget* getNextCommandTarget()
    {
        // this will return the next parent component that is an ApplicationCommandTarget (in this
        // case, there probably isn't one, but it's best to use this method in your own apps).
        return findFirstTargetParentComponent();
    }

    void getAllCommands (Array <CommandID>& commands)
    {
        // this returns the set of all commands that this target can perform..
        const CommandID ids[] = { 
                                  fileNew,
                                  fileOpen,
                                  fileSave,
                                  fileSaveAs,
                                  fileRevert,
                                  showTreeView,
                                  viewRefresh,
                                  viewCondense,
                                  viewExpand,
                                  showAbout,
        };

        commands.addArray (ids, numElementsInArray (ids));
    }

    // This method is used when something needs to find out the details about one of the commands
    // that this object can perform..
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
    {
        const String generalCategory ("General");

        switch (commandID)
        {
        case fileNew:
            result.setInfo ("New", "Start a new descriptor", generalCategory, 0);
            result.addDefaultKeypress ('n', ModifierKeys::commandModifier);
            break;

        case fileOpen:
            result.setInfo ("Open...", "Open a descriptor...", generalCategory, 0);
            result.addDefaultKeypress ('o', ModifierKeys::commandModifier);
            break;

        case fileSave:
            result.setInfo ("Save", "Save descriptor", generalCategory, 0);
            result.addDefaultKeypress ('s', ModifierKeys::commandModifier);
            break;

        case fileSaveAs:
            result.setInfo ("Save As...", "Save descriptor as...", generalCategory, 0);
            break;

        case fileRevert:
            result.setInfo ("Revert", "Revert to last saved", generalCategory, 0);
            break;

        case showTreeView:
            result.setInfo ("Treeviews", "Shows the treeview", generalCategory, 0);
            break;

        case viewRefresh:
            result.setInfo ("Refresh", "Refreshes the treeview", generalCategory, 0);
            result.addDefaultKeypress ('r', ModifierKeys::commandModifier);
            break;

        case viewCondense:
            result.setInfo ("Condense", "Show a compact treeview", generalCategory, 0);
            result.addDefaultKeypress ('d', ModifierKeys::commandModifier);
            break;

        case viewExpand:
            result.setInfo ("Expand", "Show an expanded treeview", generalCategory, 0);
            result.addDefaultKeypress ('e', ModifierKeys::commandModifier);
            break;

        case showAbout:
            result.setInfo ("About", "About", generalCategory, 0);
            break;

        default:
            break;
        };
    }

    // this is the ApplicationCommandTarget method that is used to actually perform one of our commands..
    bool perform (const InvocationInfo& info)
    {
        switch (info.commandID)
        {
        case showTreeView:
        case fileNew:
            showDescriptorView (createDescriptorTreeView(0));
            getTopLevelComponent()->setName("Rubric (Beta) - Default descriptor");
            currentWorkingFile = String::empty;
            currentDescriptorView->CondenseTree();
            break;

        case fileOpen:
            {
                FileChooser fc ("Choose a XML file to open...", 
                                File::getCurrentWorkingDirectory(), 
                                "*.xml", 
                                true);
                if (fc.browseForFileToOpen())
                {
                    File chosenFile = fc.getResult();
                    if (chosenFile.existsAsFile())
                    {
                        currentWorkingFile = chosenFile.getFullPathName();
                        chosenFile.getParentDirectory().setAsCurrentWorkingDirectory();
                    }
                    showDescriptorView (createDescriptorTreeView(&chosenFile));
                    getTopLevelComponent()->setName("Rubric (Beta) - " + chosenFile.getFileName());
                }
           }
           currentDescriptorView->CondenseTree();
           break;

        case fileSave:
            //currentDescriptorView->saveXmlTree("d:\\foo.xml");
            if (File(currentWorkingFile).existsAsFile())
            {
                currentDescriptorView->saveXmlTree(currentWorkingFile);
            }
            else
            {
                invokeDirectly (fileSaveAs, true);
            }
            break;

        case fileSaveAs:
            {
                FileChooser fc ("Save Descriptor As......", 
                                File::getCurrentWorkingDirectory(), 
                                "*.xml", 
                                true);
                if (fc.browseForFileToSave(true))
                {
                    File chosenFile = fc.getResult();
                    currentWorkingFile = chosenFile.getFullPathName();
                    chosenFile.getParentDirectory().setAsCurrentWorkingDirectory();
                    currentDescriptorView->saveXmlTree(currentWorkingFile);
                    getTopLevelComponent()->setName("Rubric (Beta) - " + chosenFile.getFileName());
                }
            }
            break;

        case fileRevert:
            if (File(currentWorkingFile).existsAsFile())
            {
                File workingFile(currentWorkingFile);
                showDescriptorView (createDescriptorTreeView(&workingFile));
            }
            else
            {
                showDescriptorView (createDescriptorTreeView(0));
            }
            
            break;
            {
                showDescriptorView (createDescriptorTreeView(0));
            }

        case viewRefresh:
            currentDescriptorView->UpdateTree();
            break;

        case viewCondense:
            currentDescriptorView->CondenseTree();
            break;

        case viewExpand:
            currentDescriptorView->ExpandTree();
            break;

        case showAbout:
            AlertWindow::showMessageBox (AlertWindow::NoIcon,
                                         "About Rubric",
                                         "AV/C Music Subunit Status Descriptor Generator \nVersion 0.75, Brian Karr\nCopyright (c) 2007-2012\nTC Applied Technologies, Ltd.",
                                         "ok");
            break;
        default:
            return false;
        };

        return true;
    }
    
    DescriptorTreeView* GetCurrentView()
    {
        return (currentDescriptorView);
    }
    
    juce_UseDebuggingNewOperator
};

//==============================================================================

//==============================================================================
MainWindow::MainWindow()
    : DocumentWindow ("Rubric (Beta)",
                      Colours::palegoldenrod,
                      DocumentWindow::allButtons,
                      true)
{
    commandManager = new ApplicationCommandManager();

    setResizable (true, false); // resizability is a property of ResizableWindow

    ContentComp* contentComp = new ContentComp (this);

    commandManager->registerAllCommandsForTarget (contentComp);
    commandManager->registerAllCommandsForTarget (JUCEApplication::getInstance());

    // this lets the command manager use keypresses that arrive in our window to send
    // out commands
    addKeyListener (commandManager->getKeyMappings());

    // sets the main content component for the window to be this tabbed
    // panel. This will be deleted when the window is deleted.
    setContentOwned (contentComp, false);

    // this tells the DocumentWindow to automatically create and manage a MenuBarComponent
    // which uses our contentComp as its MenuBarModel
#if JUCE_MAC
	MenuBarModel::setMacMainMenu ((ContentComp*) getContentComponent());
	setMenuBar (0);
	setUsingNativeTitleBar(true);
#else
    setMenuBar (contentComp);
#endif
    // tells our menu bar model that it should watch this command manager for
    // changes, and send change messages accordingly.
    contentComp->setApplicationCommandManagerToWatch (commandManager);

    setVisible (true);
}

MainWindow::~MainWindow()
{
    // setting our content component to 0 will delete the current one, and
    // that will in turn delete all its child components. You don't always
    // have to do this explicitly, because the base class's destructor will
    // also delete the content component, but in this case we need to
    // make sure our content comp has gone away before deleting our command
    // manager.
    setMenuBar (0);
    setContentOwned (0, false);

    delete commandManager;
}

void MainWindow::closeButtonPressed()
{
    // The correct thing to do when you want the app to quit is to call the
    // JUCEApplication::systemRequestedQuit() method.

    // That means that requests to quit that come from your own UI, or from other
    // OS-specific sources (e.g. the dock menu on the mac) all get handled in the
    // same way.

    JUCEApplication::getInstance()->systemRequestedQuit();
}

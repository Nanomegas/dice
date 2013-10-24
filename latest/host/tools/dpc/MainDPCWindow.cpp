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
#include "MainDPCWindow.h"
#include "Meter.h"
#include "MeterLegend.h"
#include "juce_TCATLookAndFeel.h"
#include "tcat_dice_defaults.h"

#ifdef _DEBUG	                               
// debug header for leak detection
// http://www.codeproject.com/KB/applications/visualleakdetector.aspx
// #include "vld.h"
#endif // _DEBUG
                               

//==============================================================================
class DpcTab  : public Component,
                public ButtonListener,
                private Timer
{
    //==============================================================================
    MainDPCWindow* m_mainWindow;
	Meter * meter;
	MeterLegend * legend;
	TextButton* m_reset;
	Label * m_max;
	Label * m_info;
	int maxval;
	int m_curMessage;

	enum opModesMaxDpcUs
	{
		maxDpcUs_LowLatency =  300,
		maxDpcUs_Normal     = 1300,		// yellow lower threshold
		maxDpcUs_Safe1      = 2600,		// orange lower threshold
		maxDpcUs_Safe2      = 5300,		//    red lower threshold
	};

public:
    //==============================================================================
    DpcTab (MainDPCWindow* mainWindow)
        : m_mainWindow (mainWindow),
          maxval(0),
          m_curMessage(-1)
    {        
		meter = new Meter ();
		meter->setColour (Meter::backgroundColourId,  Colour(0x30,0x30,0x30)); //TCATLookAndFeel::tcatSurround);
		meter->setBarConfig (16000, maxDpcUs_Normal, maxDpcUs_Safe1, maxDpcUs_Safe2, 3);
		addAndMakeVisible (meter);
		legend = new MeterLegend ();
		legend->setColour (Meter::backgroundColourId,  TCATLookAndFeel::tcatSurround);
		legend->setLegendConfig (16000);
		addAndMakeVisible (legend);
		startTimer (100);
		
		Label* label = new Label (String::empty, "");
		addAndMakeVisible (label);
		label->setBounds (120, 50, 100, 12);
		label->setText ("Max Latency",false);
		
		
		m_max = new Label (String::empty, "");
		addAndMakeVisible (m_max);
		m_max->setBounds (120, 65, 100, 12);
		m_max->setText("--",false);
		
		m_reset = new TextButton ("Reset");
        addAndMakeVisible (m_reset);
        m_reset->changeWidthToFitText (24);
        m_reset->addListener (this);

		meter->setBounds (10,10,30,300);
        legend->setBounds (40,10,70,300);
        m_reset->setTopLeftPosition (120, 10);
		
		m_info = new Label (String::empty, "");
		addAndMakeVisible (m_info);
		m_info->setBounds (120, 80, 200, 230);
		m_info->setText ("",false);
		m_info->setColour (Label::backgroundColourId,  TCATLookAndFeel::tcatBackground);
		m_info->setJustificationType (Justification::topLeft);
    }

    ~DpcTab()
    {
        // (need to do this because the default look-and-feel object is one of our members,
        // so will be deleted with us, and would leave a dangling pointer)
        deleteAllChildren();

    }

    
    //==============================================================================
    void resized()
    {
    }

	void setInfoMessage (int msgId)
	{
		if (msgId == m_curMessage) return;
		m_curMessage = msgId;
		switch (m_curMessage)
		{
			case 0:
				m_info->setText ("Your computer is performing well and seems to be optimized for "
	                             "streaming. You will be able to run using \"Lowest Latency\".\n"
	                             "Try to run applications you are normally running on your "
	                             "computer to see if this affects the measurement.",false);
	            break;

			case 1:
				m_info->setText ("Your computer performance is not optimal for streaming. You will need to "
	                             "select \"Normal Mode\" to prevent dropouts.\n\n"
	                             "Try to perform a Windows Update or disable Network adapters.\n",false);
	            break;
	
			case 2:
				m_info->setText ("Your computer is not performing well. You will need to "
	                             "select \"Safe Mode 1\" to prevent dropouts.\n\n"
	                             "Try to perform a Windows Update or disable Network adapters.\n",false);
	            break;
	                               
			case 3:
				m_info->setText ("Your computer is far from optimal. Selecting "
	                             "\"Safe Mode 2\" might prevent dropouts.\n\n"
	                             "Try to perform a Windows Update or disable Network adapters.\n",false);
	            break;
	                             
			case 4:
				m_info->setText ("Error: This tool requires administrator permissions as it "
	                             "is peeking into the kernel.\n"
	                             "Please make sure that you have "
	                             "administrator privileges and restart the tool.",false);
	            break;
		}
	}
	
	void buttonClicked (Button*)
    {
		meter->reset();
		maxval = 0;
		m_max->setText("--",false);
    }

	void timerCallback()
	{
		long peak=0;
		String notifyStr;
		
		if (m_mainWindow->m_dpc.getDPCPeak(peak)==DPC_NO_ERROR)
		{
			meter->setValue(peak);
			if (peak > maxval)
			{
				maxval=peak;
				String str;
				str << maxval << " us";
				m_max->setText(str,false);

				if (maxval < maxDpcUs_LowLatency)
					setInfoMessage (0);
				else if (maxval < maxDpcUs_Normal)
					setInfoMessage (1);
				else if (maxval < maxDpcUs_Safe1)
					setInfoMessage (2);
				else
					setInfoMessage (3);
			}			
		}
		else
		{
			setInfoMessage (4);
		}				
	}


    juce_UseDebuggingNewOperator
};


//==============================================================================
class AdvTab  : public Component,
                public ButtonListener,
				public SliderListener
{
    //==============================================================================
    MainDPCWindow* m_mainWindow;
	Slider * m_dpcRepeat;
	Slider * m_dpcLatency;
	ToggleButton * m_induce;

public:
    //==============================================================================
    AdvTab (MainDPCWindow* mainWindow)
        : m_mainWindow (mainWindow)
    {        
		m_induce = new ToggleButton("Enable DPC Inducer");
		addAndMakeVisible (m_induce);
		m_induce->setBounds (20, 10, 250, 20);
		m_induce->addListener(this);

		Label * label = new Label("Label","How often");
		addAndMakeVisible (label);
		label->setBounds (20,40,250,20);

		m_dpcRepeat = new Slider("Slider");
		addAndMakeVisible (m_dpcRepeat);
		m_dpcRepeat->setSliderStyle(Slider::LinearHorizontal);
		m_dpcRepeat->setRange (0.1,10.0,0.1);
		m_dpcRepeat->setSkewFactor (0.5);
		m_dpcRepeat->setTextBoxStyle (Slider::TextBoxRight,true,80,20);
		m_dpcRepeat->setTextValueSuffix("s");
		m_dpcRepeat->setBounds (20, 60, 250, 20);
		m_dpcRepeat->setValue (3.0);
		m_dpcRepeat->addListener(this);

		label = new Label("Label","How much");
		addAndMakeVisible (label);
		label->setBounds (20,90,250,20);

		m_dpcLatency = new Slider("Slider");		
		addAndMakeVisible (m_dpcLatency);
		m_dpcLatency->setSliderStyle(Slider::LinearHorizontal);
		m_dpcLatency->setRange (0.1,100,0.1);
		m_dpcLatency->setSkewFactor (0.5);
		m_dpcLatency->setTextBoxStyle (Slider::TextBoxRight,true,80,20);
		m_dpcLatency->setTextValueSuffix("ms");
		m_dpcLatency->setBounds (20, 110, 250, 20);
		m_dpcLatency->setValue (10.0);
		m_dpcLatency->addListener(this);

    }

    ~AdvTab()
    {
        // (need to do this because the default look-and-feel object is one of our members,
        // so will be deleted with us, and would leave a dangling pointer)
        deleteAllChildren();

    }

	void sliderValueChanged (Slider* slider)
	{
	}
    
 	void sliderDragEnded (Slider* slider)
	{
		double duration, stall;

		duration = m_dpcRepeat->getValue();
		stall = m_dpcLatency->getValue();
		if (m_induce->getToggleState())
			m_mainWindow->m_dpc.startDPCInducer (true, (long)(duration*1000000.0), (long) (stall*1000.0));
	}

	void buttonClicked (Button*)
    {
		double duration, stall;

		duration = m_dpcRepeat->getValue();
		stall = m_dpcLatency->getValue();
		m_mainWindow->m_dpc.startDPCInducer (m_induce->getToggleState(), (long)(duration*1000000.0), (long) (stall*1000.0));
    }


    juce_UseDebuggingNewOperator
};


//==============================================================================
class AboutTab  : public Component
{
    //==============================================================================
    MainDPCWindow* m_mainWindow;

public:
    //==============================================================================
    AboutTab (MainDPCWindow* mainWindow)
        : m_mainWindow (mainWindow)
    {        

		Label * info = new Label (String::empty, "");
		addAndMakeVisible (info);
		info->setBounds (10, 20, 320, 300);
		info->setText ("",false);
		info->setColour (Label::backgroundColourId,  TCATLookAndFeel::tcatBackground);
		info->setJustificationType (Justification::topLeft);

		String str;
		str = "TCAT DPC Tool\n";
		str +=kTCAT_DICE_USR_COPYRIGHT_STR;
		str += "\n";
		str +="Version: ";
		str +=kTCAT_DICE_VERSION_EXTENDED_STR;
		info->setText (str,false);
    }

    ~AboutTab()
    {
        // (need to do this because the default look-and-feel object is one of our members,
        // so will be deleted with us, and would leave a dangling pointer)
        deleteAllChildren();

    }

    juce_UseDebuggingNewOperator
};


//==============================================================================
class ContentComp  : public Component
{
    //==============================================================================
    MainDPCWindow *		m_mainWindow;
	TabbedComponent *	m_tabs;
	DpcTab *			m_dpcTab;
	AdvTab *			m_advTab;
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
    ContentComp (MainDPCWindow* mainWindow)
        : m_mainWindow (mainWindow)
    {
        LookAndFeel::setDefaultLookAndFeel (&tcatLookAndFeel);
        
	    m_tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	    addAndMakeVisible (m_tabs);
		m_tabs->setBounds (0, 0, getWidth(), getHeight());
	
		//Now add the sub components
		m_dpcTab = new DpcTab(m_mainWindow);
		m_tabs->addTab ("DPC",     TCATLookAndFeel::tcatSurround, m_dpcTab,      true);
    
		m_advTab = new AdvTab(m_mainWindow);
		m_tabs->addTab ("Advanced",     TCATLookAndFeel::tcatSurround, m_advTab,      true);

		m_aboutTab = new AboutTab(m_mainWindow);
		m_tabs->addTab ("About",     TCATLookAndFeel::tcatSurround, m_aboutTab,      true);

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

    juce_UseDebuggingNewOperator
};


//==============================================================================
MainDPCWindow::MainDPCWindow()
    : DocumentWindow ("TCAT DPC Tool",
					  TCATLookAndFeel::tcatSurround,
                      DocumentWindow::allButtons,
                      true)
{
	int		status;

	status = m_dpc.install( NULL, 0 );
	if (status != DPC_NO_ERROR)
	{
		DBG( String::formatted( "Unable to install DPC Checker (error: %d)", status ));
	}

	status = m_dpc.open();
	if (status != DPC_NO_ERROR)
	{
		DBG( String::formatted( "Unable to open DPC Checker (error: %d)", status ));
	}

	status = m_dpc.setChkTimerDefault( true );
	if (status != DPC_NO_ERROR)
	{
		DBG( String::formatted( "Unable to start DPC Checker (error: %d)", status ));
	}

    commandManager = new ApplicationCommandManager();

    setResizable( false, false ); // resizability is a property of ResizableWindow

    ContentComp* contentComp = new ContentComp( this );

    // sets the main content component for the window to be this tabbed
    // panel. This will be deleted when the window is deleted.
    setContentOwned( contentComp, false );

    setVisible( true );
}


MainDPCWindow::~MainDPCWindow()
{
	int		status;

	status = m_dpc.close();
	if (status != DPC_NO_ERROR)
	{
		DBG( String::formatted( "Unable to close DPC Checker (error: %d)", status ));
	}

	status = m_dpc.remove();
   	if (status != DPC_NO_ERROR)
	{
		DBG( String::formatted( "Unable to uninstall DPC Checker (error: %d)", status ));
	}

    // setting our content component to 0 will delete the current one, and
    // that will in turn delete all its child components. You don't always
    // have to do this explicitly, because the base class's destructor will
    // also delete the content component, but in this case we need to
    // make sure our content comp has gone away before deleting our command
    // manager.
    setContentOwned( 0, false );
    delete commandManager;
}


void MainDPCWindow::closeButtonPressed()
{
    JUCEApplication::getInstance()->systemRequestedQuit();
}


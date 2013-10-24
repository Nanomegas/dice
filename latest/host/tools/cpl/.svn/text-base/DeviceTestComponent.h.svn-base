///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DEVICE_TEST
#define __DEVICE_TEST

#include "JuceHeader.h"
#include "DeviceWindow.h"
#include "events.h"
#include "listener.h"
#include <vector>

//==============================================================================
class DeviceTestComponent : 
		public Component,
		public TextEditorListener,
		public ButtonListener,
		public ComboBoxListener,
		public device_listener,
		public ActionListener,
		private AsyncUpdater
{
public:
	DeviceTestComponent();
    ~DeviceTestComponent();

    void paint (Graphics& g);
    void resized();
	void broughtToFront();
	void lookAndFeelChanged();

	void updateContent();
	
	// ButtonListener callback
    void buttonClicked (Button* buttonThatWasClicked);

	// ComboBoxListener callback
	void comboBoxChanged (ComboBox* comboBoxThatHasChanged);

	// TextEditorListener callbacks
	void textEditorTextChanged (TextEditor& editor);
	void textEditorReturnKeyPressed (TextEditor& editor);
	void textEditorEscapeKeyPressed (TextEditor& editor);
    void textEditorFocusLost (TextEditor& editor);
	
	void setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus);
	void readDevice();
	
	void actionListenerCallback(const String &message);
	
	void handleAsyncUpdate();
	
	void mount(tcat::dice::reference<event_device> device);
	void unmount();
	void update_nickname();
	
	juce_UseDebuggingNewOperator

private:
	tcat::uint64	m_guid;
	String			m_nickname;
	uint32			m_running_vendor_id;
	uint32			m_running_product_id;
    uint32			m_running_vmajor;
    uint32			m_running_vminor;
    uint32			m_running_vsub;
    uint32			m_running_vbuild;
	ComboBox		* m_testmode_cb;
	TextButton		* m_send_cmd;
	Label			* m_info_l;	
	Label			* m_runningversioninfo_label;

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
    DeviceTestComponent (const DeviceTestComponent&);
    const DeviceTestComponent& operator= (const DeviceTestComponent&);
};

#endif // __DEVICE_TEST

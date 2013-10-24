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

#ifndef __DEVICE_SIMPLE_FIRMWARE__
#define __DEVICE_SIMPLE_FIRMWARE__

#include "JuceHeader.h"
#include "DeviceWindow.h"
#include "events.h"
#include "listener.h"
#include <vector>

//==============================================================================
class DeviceSimpleFirmwareComponent : 
		public Component,
		public TextEditorListener,
		public ButtonListener,
		public FilenameComponentListener,
		public device_listener,
		public ActionListener,
		private AsyncUpdater
{
public:
	DeviceSimpleFirmwareComponent();
    ~DeviceSimpleFirmwareComponent();

    void paint (Graphics& g);
    void resized();
	void broughtToFront();
	void lookAndFeelChanged();

	void updateContent();
	
	// ButtonListener callback
    void buttonClicked (Button* buttonThatWasClicked);

	// FileChooserListener callback
	void filenameComponentChanged (FilenameComponent*);

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
    Font			m_font;
	uint32			m_file_vendor_id;
	uint32			m_file_product_id;
    uint32			m_file_vmajor;
    uint32			m_file_vminor;
    uint32			m_file_vsub;
    uint32			m_file_vbuild;
	uint32			m_running_vendor_id;
	uint32			m_running_product_id;
    uint32			m_running_vmajor;
    uint32			m_running_vminor;
    uint32			m_running_vsub;
    uint32			m_running_vbuild;
    bool			m_allow_continue;

	String m_cur_working_file;
	tcat::int64 m_cur_working_filesize;
	Label* m_instr_l;
	Label* m_info_l;	
	Label* m_ulfile_label;
	FilenameComponent* m_filechooser;
	File m_current_file;
	TextButton* m_upload;
	ToggleButton* m_show_details;
	GroupComponent* m_details_group;
	Label* m_runningversioninfo_label;
	Label* m_fileversioninfo_label;
	Label* m_sectorname_label;
	String m_sectorname;

	struct sector_rec
	{
		std::string name;
		uint32 flashBase;
		uint32 memBase;
		uint32 length;
		uint32 entryPoint;
	};
	typedef std::vector<sector_rec> sector_list;
	sector_list m_sector_list;

	//==============================================================================
    // (prevent copy constructor and operator= being generated..)
    DeviceSimpleFirmwareComponent (const DeviceSimpleFirmwareComponent&);
    const DeviceSimpleFirmwareComponent& operator= (const DeviceSimpleFirmwareComponent&);
};

#endif // __DEVICE_SIMPLE_FIRMWARE__

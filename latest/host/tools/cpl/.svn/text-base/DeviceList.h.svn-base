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

#ifndef __DEVICE_LIST__
#define __DEVICE_LIST__

#include "JuceHeader.h"
#include "events.h"
#include "listener.h"

class DeviceListComponent :
		public ListBox,
		public ListBoxModel,
		public ActionBroadcaster,
		public ActionListener,
		public bus_listener,
		private AsyncUpdater
{
	friend class DeviceRowComponent;
	
public:
    //==============================================================================
    DeviceListComponent ();
    ~DeviceListComponent();

	void mouseUp(const MouseEvent &e);
	void lookAndFeelChanged();
	void actionListenerCallback(const String &message);
    void handleAsyncUpdate();

    int getNumRows();
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected);
	Component * refreshComponentForRow(int rowNumber, bool isRowSelected, Component * existingComponentToUpdate);

	tcat::uint64 getSelectedGuid (void);
	
	void selectedRowsChanged (int lastRowselected);
	
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();
	void update_master();
	void update_devicelist();
	
	juce_UseDebuggingNewOperator

private:
	int				m_num_rows;
	tcat::uint64	m_master;
};

#endif   // __DEVICE_LIST__

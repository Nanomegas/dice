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

#pragma once

#include "JuceHeader.h"
#include "DeviceWindow.h"
#include "events.h"
#include "listener.h"
#include "dice_eap_device.h"
#include "Vmeter.h"
#include "ChannelStrip.h"
#include "Blender.h"

#define	MIX_MAX_CHANNELS			32
#define MIX_METER_UPD_MS			80

#define MIX_NUM_COEFFICIENTS		(18*4)
#define MIX_COEF_HP_L_OFS			0
#define MIX_COEF_HP_R_OFS			17
#define MIX_COEF_SPDIF_L_OFS		35
#define MIX_COEF_SPDIF_R_OFS		52

class DeviceMixerComponent : 
		public Component,
		public ButtonListener,
		public device_listener,
		public ChangeListener,
		public ActionBroadcaster,
		private Timer,
		private AsyncUpdater
{
public:
	DeviceMixerComponent(DeviceWindow * _owner, const String & name, int _mixer_id);
    ~DeviceMixerComponent();

	void handleAsyncUpdate();
	void resized();
    void paint (Graphics& g);
	void buttonClicked (Button* button);
	void enablementChanged();
	void changeListenerCallback(ChangeBroadcaster* source);

	void updateContent (void);
	void addChannel(int ch_id, const String & name);
	void setChannelPos(int ch_id, int x_offset);
	void setMasterPos(int x_offset);
	void setDescription(String & description);
	int getMixerWidth();
	void addGroupLabel(int x_offset, const String & text);
	void forceDirectOut(bool bIsDirectOut);

	// device_listener callbacks
	void mount(tcat::dice::reference<event_device> device);
	void unmount();
		
	juce_UseDebuggingNewOperator

private:
	DeviceWindow		* m_owner;
	tcat::uint64		m_guid;
	int					m_mixer_id;
	MixChannelStrip		* m_channel[MIX_MAX_CHANNELS];
	int					m_num_channels;
	int					m_ch_peak_id[MIX_MAX_CHANNELS];
	MasterChannelStrip	* m_master;
	int					m_master_peak_ids[2];
	float				m_ch_peak_mult[MIX_MAX_CHANNELS];		// used to scale meter to show 'post-fader' result based on gain setting
    ToggleButton		* m_direct_out;
	bool				m_user_dirout;
	bool				m_forced_dirout;
    TextButton			* m_reset;
	StringArray			m_mix_ch_name;
	int					m_channel_spacing[MIX_MAX_CHANNELS];
	int					m_channel_offset[MIX_MAX_CHANNELS];
	StringArray			m_group_label_name;
	int					m_group_label_x_offset[8];
	String				m_action_msg;
	String				m_description;
	int					m_panel_width;
	void timerCallback();
};

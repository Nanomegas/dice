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
#include "tcat_dice.h"
#include "dice_eap_device.h"
#include "DeviceMixerComponent.h"
#include "eapProperties.h"

#define BL_SAVEFILE_VERSION				1
#define BL_MIN_SAVEFILE_VERSION			2

#define MIX_MAX_NUM_CHANNELS			32
#define MIX_MAX_NUM_MIXERS				18

#define MIX_MAX_NUM_ROUTES				256
#define MIX_MAX_NUM_OUTPUTS				16
#define MIX_MAX_NUM_INPUTS				18
#define MIX_NUM_ROWS_PER_MIXER			2
#define MIX_MAX_PAN_THROW				50		// number of ticks from zero, plus or minus

enum CHANGE_TYPE
{
	kAddMixer,
	kRemoveMixers,
	kAddChannel,
	kChannelPos,
	kMasterPos,
	kAddGroupLabel,
	kMixersCreated,
	kGain,
	kPan,
	kMutes,
	kSolos,
	kPeakId,
	kPeakMult,
	kChName,
	kMasterGain,
	kMasterMute,
	kMasterPeakIds,
	kMasterName,
	kDirectOut,
	kMixerText
};

typedef struct
{
	CHANGE_TYPE	type;
	int			mixer_id;
	int			ch_id;
	uint32		int_val1;
	uint32		int_val2;
	float		float_val;
	String		str;
} change_msg;

// channel strip
typedef struct
{
	int							gain;			// slider value
	float						gain_dbv;		// gain mapped to dBv
	int							pan;			// slider value
	int							peak_id;		// the peak that is shown by the meter
	float						peak_mult;		// multiplier to use for post-fader vu
	char						name[16];		// name directly above the channel
	int							x_offset;		// x position
} BL_CHANNEL;

// master out channel
typedef struct
{
	int							gain;			// slider value
	float						gain_dbv;		// gain mapped to dBv
	bool						muting;
	int							peak_ids[2];	// the peaks that are shown by the meters
	char						name[16];		// name directly above the channel
	int							x_offset;		// x position
} BL_MASTER;

// mixer
typedef struct
{
	char						name[16];					// name on the mixer tab
	char						description[64];			// description above the mixer channels
	uint16						num_channels;
	BL_CHANNEL					channel[MIX_MAX_NUM_CHANNELS];
	BL_MASTER					master;
	int							num_group_labels;			// number of channel grouping labels
	char						group_label_name[8][32];	// channel grouping label names
	int							group_label_x_offset[8];	// x position
	uint32						mutes;
	uint32						was_muted;
	uint32						solos;
	int							direct_out;
	int							dirout_route_ids[3][2];		// for each rate mode, indexes of routes that are changed when mixer is enabled/disabled
} BL_MIXER;

// streaming modes, mixers and global mixer coefficients
typedef struct
{
	bool						isValid;
	int							version;					// version of saved description
	unsigned					modes;						// bitmap of supported rate modes
	int							num_mixers;					// number of mixer tabs
	tcat::dice::EAP_CURR_CFG	curr_cfg;					// the current rate mode
	BL_MIXER					mixer[MIX_MAX_NUM_MIXERS];
	tcat::uint32				coeff[MIX_MAX_NUM_OUTPUTS][MIX_MAX_NUM_INPUTS];
	change_msg					msg;
} BL_DESC;


class Blender : 
		public Component,
		public device_listener,
		public ActionListener,
		public ActionBroadcaster,
		public ChangeBroadcaster,
		private AsyncUpdater
{
public:
	Blender();
    ~Blender();

	void setInitialized();									// mixers are created and ready to be set to saved state
	void clearSettings();									// zero and invalidate the description
	void setNumMixers(int numMixers);						// blender object creator must set this
	void setNumChannels(int mixerId, int numChanels);		// mixer object creator must set this

	// the component receives changes with these action messages:
	//   <type>:<mixer_id>+<channel_id>=<value>
	//
	//   <type> pan, gain, mute, master_gain, master_mute, solo, dirout (channel_id is ignored for dirout)
	//   <mixer_id> uint16
	//   <channel_id> uint16
	//   <value> uint32
	//
	// the component sends changes with these action messages:
	//   <type>:<mixer_id>+<channel_id>=<value>
	//
	//   <type> mute, solo
	//   <mixer_id> is 2-bytes unsigned int
	//   <channel_id> ignored
	//   <value> is 32-bit bitmap of channel mute/solo states

	// handle async messages
	void actionListenerCallback(const String &message);		// mixer/channel objects use action messages to communicate with blender
	void handleAsyncUpdate();								// updater for routing/streaming/mixer coefficients
	void getChangeMessage(change_msg *msg);
	
	void loadSettings();
	bool parseSettings(XmlElement *description);
	void saveSettings();

	bool getDefaultSettingsFromBinaryData();
	bool getDefaultSettingsFromDefaultsFile();
	bool getDefaultSettingsFromSaveFile();
	bool getDefaultSettingsFromDeviceFlash();

	// set current device
	void setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus);	// set the current bus device that is referenced

	// device_listener callbacks
	void mount(tcat::dice::reference<event_device> device);	// notification of bus device arrival
	void unmount();											// notification of bus device removal
	void update_clock();									// notification of sample rate change
		
	juce_UseDebuggingNewOperator

private:
	// bus listener events, for notifying the message thread
	bool								m_initialized;		// should be set true when all mixers are instantiated
	bool								m_created;			// should be set true when all mixer gui objects are created
	tcat::uint64						m_guid;				// WWUID of currently references bus device
	BL_DESC								m_bl_desc;			// the main blender descriptor

    ScopedPointer<XmlElement> xml;                          // holds an xml documentElement

	float m_dBvMap		[MIXSLIDER_NUM_TICKS+1];			// dBuMap values converted to dbV: 10^(Lu/20)

	PropertySet			* m_eap_defaults;					// settings to use in case of missing settings file
	EapPropertiesFile	* m_eap_props;						// persistenet storage of GUI settings

	enum DEV_UPDATE_NOTIFICATION
	{
		updt_mount =		(1 << 0),
		updt_unmount =		(1 << 1),
		updt_clock =		(1 << 2)
	};

	uint32							m_update;

	void load_cfg();										// initialize m_bl_desc from file
	void save_cfg();										// save m_bl_desc to file

	void stream_cfg();										// set up dice streaming
	void router_cfg();										// set up dice routing
	void mixer_cfg(int mixer_id);							// update dice mixer with new coefficients

	void createControls();									// create mixer controls from m_bl_desc
	void initControls();									// sync all mixer controls with m_bl_desc
	void setDirectOut(int mixer_id);						// mixer is disabled if non-zero

	void sendChGain(int mixer_id, int ch_id, int value);				// send gain value to mix channel
	void sendChPan(int mixer_id, int ch_id, int value);					// send pan value to mix channel
	void sendChMutes(int mixer_id, uint32 value);						// send mute states to mix channels
	void sendChSolos(int mixer_id, uint32 value);						// send solo states to mix channels
	void sendChPeakId(int mixer_id, int ch_id, int peak_id);			// send peak index to mix channel
	void sendChPeakMult(int mixer_id, int ch_id, float value);			// send peak multiplier to channel strips
	void sendChName(int mixer_id, int ch_id, const char * name);		// send name to channel strip
	void sendMasterGain(int mixer_id, int value);						// send gain value to master output channel
	void sendMasterMute(int mixer_id, bool value);						// send mute state to output channel
	void sendMasterPeakIds(int mixer_id, int id_l, int id_r);			// send peak id's to output channel
	void sendMasterName(int mixer_id, const char * name);				// send name to output channel
	void sendDirout(int mixer_id, int value);							// send direct out state to mixer
	void sendAddMixer(int mixer_id, const char * name);					// send new mixer message
	void sendRemoveMixers();											// send remove mixers message
	void sendAddChannel(int mixer_id, int ch_id, const char * name);	// send new channel message
	void sendChannelPos(int mixer_id, int ch_id, int x_offset);			// send channel position message
	void sendMasterPos(int mixer_id, int x_offset);						// send output channel position message
	void sendAddGroupLabel(int mixer_id, int x_offset, const char *);	// send new croup label message
	void sendMixersCreated();											// send mixer objects created message
	void sendMixerDescription(int mixer_id, const char * text);			// send description text to mixer
};


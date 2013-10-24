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

#include "dice_eap_device.h"
#include "eapProperties.h"
#include "Blender.h"
#include "EventLogger.h"

// supplied by the mixer GUI component
extern float dBuMap[];

Blender::Blender() :
	m_initialized (false),
	m_created (false),
	m_guid (0),
	m_bl_desc (),
    m_eap_props (0),
	m_update (0)
{
	// initialize dBV (0 to 2.0 range) map from dBu map (-100 to 6.0 range)
	for (int i=0; i<MIXSLIDER_NUM_TICKS+1; i++)
	{
		m_dBvMap[i] = pow(10, (dBuMap[i]/20));
	}
	clearSettings();
};

Blender::~Blender()
{
	if (m_device)
	{
		if (m_device->isEapSupported())
		{
			saveSettings();
		}
	}
	MessageManagerLock mmLock;
	removeAllChangeListeners();
};

void Blender::setInitialized()
{
	m_initialized = true;
	handleAsyncUpdate();
}

void Blender::clearSettings()
{
	memset (&m_bl_desc.coeff, 0, sizeof (m_bl_desc.coeff));
	memset (&m_bl_desc.curr_cfg, 0, sizeof (m_bl_desc.curr_cfg));
	m_bl_desc.num_mixers = 0;
	m_bl_desc.modes = 0;
	m_bl_desc.isValid = false;
}

void Blender::setNumMixers(int numMixers)
{
	m_bl_desc.num_mixers = numMixers;
}

void Blender::setNumChannels(int mixerId, int numChannels)
{
	m_bl_desc.mixer[mixerId].num_channels = (uint16)numChannels; 
}

void Blender::setShownDeviceGuid(tcat::uint64 guid, event_bus_ref bus)
{
	if (guid == m_guid) return;

	if (m_device)
	{
		if (m_device->isEapSupported())
		{
			saveSettings();
			clearSettings();
			sendRemoveMixers();
		}
		m_device->detach(this);
	}

	m_guid = guid;

	event_device_ref devref = bus->find(guid);
	if (devref)
	{
		devref->attach(this);
	}
}

void Blender::handleAsyncUpdate()
{
	if (m_initialized)
	{
		if (m_update & updt_unmount)
		{
			sendRemoveMixers();
			m_update &= ~updt_unmount;
		}

		if (m_update & updt_mount)
		{
			if (m_device && m_device->isEapSupported())
			{
				m_created = false;
				loadSettings();
				router_cfg();
				stream_cfg();
				createControls();
				initControls();
				for (int i=0; i<m_bl_desc.num_mixers; i++)
				{
					mixer_cfg(i);
				}
			}
			m_update &= ~updt_mount;
		}


		if (m_update & updt_clock)
		{
			m_update &= ~updt_clock;
		}
	}
}

// device_listener callbacks
void Blender::mount(tcat::dice::reference<event_device> device)
{
	// call super's mount() first
	device_listener::mount(device);
	// which gives us an m_device reference
	if (m_device)
	{
		// handle mixer config in GUI thread
		m_update |= updt_mount;
		triggerAsyncUpdate();
	}
}

void Blender::unmount()
{
	if (m_device)
	{
		if (m_device && m_device->isEapSupported())
		{
			if (m_bl_desc.isValid)
			{
				saveSettings();
			}
		}
		m_guid = 0;

		// handle mixer config in GUI thread
		m_update |= updt_unmount;
		triggerAsyncUpdate();
	}
	device_listener::unmount();
}

void Blender::update_clock()
{
	if (m_device)
	{
		// handle clock config in GUI thread
		m_update |= updt_clock;
		triggerAsyncUpdate();
	}
}

void Blender::stream_cfg()
{
	if (!m_bl_desc.isValid) return;

	if (m_device && m_device->isEapSupported())
	{
		if (m_device->isStrmCfgEnabled())
		{
			m_device->setStrmCfg(&m_bl_desc.curr_cfg.modes[0].strmCfg, tcat::dice::kLdLow);
			m_device->setStrmCfg(&m_bl_desc.curr_cfg.modes[1].strmCfg, tcat::dice::kLdMid);
			m_device->setStrmCfg(&m_bl_desc.curr_cfg.modes[2].strmCfg, tcat::dice::kLdHigh);
		}
	}
}

void Blender::router_cfg()
{
	if (!m_bl_desc.isValid) return;

	if (m_device && m_device->isEapSupported())
	{
		if (m_device->isRouterExposed() && (!m_device->isRouterReadOnly()))
		{
			m_device->setRouting(&m_bl_desc.curr_cfg.modes[0].router, tcat::dice::kLdLow);
			m_device->setRouting(&m_bl_desc.curr_cfg.modes[1].router, tcat::dice::kLdMid);
			m_device->setRouting(&m_bl_desc.curr_cfg.modes[2].router, tcat::dice::kLdHigh);
		}
	}
}

void Blender::setDirectOut(int mixer_id)
{
	int route_index = 0;
	uint32 new_route = 0;

	// change routing on output channels
	if (m_bl_desc.mixer[mixer_id].direct_out)
	{
		for (int mode=0; mode<3; mode++)
		{
			for (int output=0; output<2; output++)
			{
				route_index = m_bl_desc.mixer[mixer_id].dirout_route_ids[mode][output];
				new_route = m_bl_desc.curr_cfg.modes[mode].direct_out_routes.route[route_index];
				m_bl_desc.curr_cfg.modes[mode].router.route[route_index] = new_route;
			}
		}
	}
	else
	{
		for (int mode=0; mode<3; mode++)
		{
			for (int output=0; output<2; output++)
			{
				route_index = m_bl_desc.mixer[mixer_id].dirout_route_ids[mode][output];
				new_route = m_bl_desc.curr_cfg.modes[mode].mixer_enabled_routes.route[route_index];
				m_bl_desc.curr_cfg.modes[mode].router.route[route_index] = new_route;
			}
		}
	}
	router_cfg();
}

void Blender::mixer_cfg(int mixer_id)
{
	if (!m_bl_desc.isValid) return;

	// assumes two-column mixers

	if (m_device && m_device->isEapSupported())
	{
		if ( (m_device->isMixerExposed()) && (!m_device->isMixerReadOnly()) )
		{
			float g = 0;
			float pan = 0;
			float gL = 0;
			float gR = 0;
			uint32 cL = 0;
			uint32 cR = 0;
			int outputL = mixer_id*2;
			int outputR = mixer_id*2+1;

			// calculate coefficients
			for (int ch_id=0; ch_id<m_bl_desc.mixer[mixer_id].num_channels; ch_id++)
			{
				// check master and channel mute state
				if ( (m_bl_desc.mixer[mixer_id].master.muting ) || (m_bl_desc.mixer[mixer_id].mutes & (1<<ch_id)) )		
				{
					cR = 0;
					cL = 0;
				}
				else
				{
					g = (float)( m_bl_desc.mixer[mixer_id].channel[ch_id].gain_dbv * m_bl_desc.mixer[mixer_id].master.gain_dbv );

					// scale to L and R using pan setting
					pan = ( (float)m_bl_desc.mixer[mixer_id].channel[ch_id].pan / (float)MIX_MAX_PAN_THROW );
					if (pan > 0.0f)
					{
						gL = g * (1.0f - pan);
						gR = g;
					}
					else if (pan < 0.0f)
					{
						gL = g;
						gR = g * (1.0f + pan);
					}
					else
					{
						gL = gR = g;
					}

					// convert to dice mixer coefficients, 0-2dBV -> 0-65535

					gL*=16384.0f;
					if (gL > 65535)
						cL = 65535;
					else if (gL < 0.0)
						cL = 0;
					else
						cL = (uint32)gL;

					gR*=16384.0f;
					if (gR > 65535)
						cR = 65535;
					else if (gR < 0.0)
						cR = 0;
					else
						cR = (uint32)gR;
				}

				m_bl_desc.coeff[outputL][ch_id] = cL;
				m_bl_desc.coeff[outputR][ch_id] = cR;

				// update channel meter peak multiplier peak_mult = (1/2.0dbv)*(gain_dbv)
				float peak_mult = 0;
				if (0 == (m_bl_desc.mixer[mixer_id].mutes & (1<<ch_id)))
				{
					peak_mult = (float)m_bl_desc.mixer[mixer_id].channel[ch_id].gain_dbv * 0.5f;
				}
				if (m_bl_desc.mixer[mixer_id].channel[ch_id].peak_mult != peak_mult)
				{
					m_bl_desc.mixer[mixer_id].channel[ch_id].peak_mult = peak_mult;
					sendChPeakMult(mixer_id, ch_id, peak_mult);
				}
			}

			// write to dice mixer
			m_device->setMixerCoeffs (0, outputL, &m_bl_desc.coeff[outputL][0], 18);
			m_device->setMixerCoeffs (0, outputR, &m_bl_desc.coeff[outputR][0], 18);
		}
	}
}

void Blender::actionListenerCallback(const String &message)
{
	int mixer_id = 0;
	int channel_id = 0;
	int mixer_id_index = message.indexOfChar(':') + 1;
	int channel_id_index = message.indexOfChar('+') + 1;
	String idStr(String::empty);
	String type(message.upToFirstOccurrenceOf(":", false, false));
	String valStr(message.fromLastOccurrenceOf("=", false, false));
	uint32 value = valStr.getIntValue();
	bool bUpdateGui = false;

	if (mixer_id_index >= 0)
	{
		idStr = message.substring(mixer_id_index, mixer_id_index+2);
		mixer_id = idStr.getIntValue();

		if (channel_id_index >= 0)
		{
			idStr = message.substring(channel_id_index, channel_id_index+2);
			channel_id = idStr.getIntValue();

			if (0 == type.compare("gain"))
			{
				int gain = jmin<int>(130, (int)value);
				m_bl_desc.mixer[mixer_id].channel[channel_id].gain = gain;
				m_bl_desc.mixer[mixer_id].channel[channel_id].gain_dbv = m_dBvMap[gain];
			}
			else if (0 == type.compare("master_gain"))
			{
				// master gain
				int gain = jmin<int>(130, (int)value);
				m_bl_desc.mixer[mixer_id].master.gain = gain;
				m_bl_desc.mixer[mixer_id].master.gain_dbv = m_dBvMap[gain];
			}
			else if (0 == type.compare("pan"))
			{
				// channel strip pan
				m_bl_desc.mixer[mixer_id].channel[channel_id].pan = (int)value;
			}
			else if (0 == type.compare("master_mute"))
			{
				// master out mute
				m_bl_desc.mixer[mixer_id].master.muting = (value != 0);
			}
			else if (0 == type.compare("mute"))
			{
				uint32 muting = (uint32)value;
				uint32 channel_bit = 1<<channel_id;

				// channel strip mute
				if (muting)
				{
					// mute
					m_bl_desc.mixer[mixer_id].mutes |= channel_bit;
					m_bl_desc.mixer[mixer_id].was_muted |= channel_bit;

					// handle solos
					if (0 != m_bl_desc.mixer[mixer_id].solos)
					{
						if (m_bl_desc.mixer[mixer_id].solos == channel_bit)
						{
							// a solo was active only on that channel, unsolo all and restore mutes
							m_bl_desc.mixer[mixer_id].solos = 0;
							m_bl_desc.mixer[mixer_id].mutes = m_bl_desc.mixer[mixer_id].was_muted;
							bUpdateGui = true;
						}
						// other solos are active, unsolo only this one if soloed
						else if (m_bl_desc.mixer[mixer_id].solos & channel_bit)
						{
							m_bl_desc.mixer[mixer_id].solos ^= channel_bit;
							m_bl_desc.mixer[mixer_id].mutes |= (m_bl_desc.mixer[mixer_id].was_muted & channel_id);
							bUpdateGui = true;
						}
					}
				}
				else
				{
					// unmute
					m_bl_desc.mixer[mixer_id].mutes ^= channel_bit;
					m_bl_desc.mixer[mixer_id].was_muted ^= channel_bit;

					// handle solos
					if (0 != m_bl_desc.mixer[mixer_id].solos)
					{
						// a solo was active on another channel, solo this one too
						m_bl_desc.mixer[mixer_id].solos |= channel_bit;
						bUpdateGui = true;
					}
				}
			}
			else if (0 == type.compare("solo"))
			{
				uint32 soloing = (uint32)value;
				uint32 channel_bit = 1<<channel_id;

				if (soloing)
				{
					// is this the first soloed channel?
					if (0 == m_bl_desc.mixer[mixer_id].solos)
					{
						// solo
						m_bl_desc.mixer[mixer_id].solos |= channel_bit;

						// mute all others
						m_bl_desc.mixer[mixer_id].mutes = ~channel_bit;
						bUpdateGui = true;
					}
					else
					{
						// solo this one also
						m_bl_desc.mixer[mixer_id].solos |= channel_bit;
						m_bl_desc.mixer[mixer_id].mutes ^= channel_bit;
						bUpdateGui = true;
					}
				}
				else
				{
					// channel was soloed

					// was this the only soloed channel?
					if (m_bl_desc.mixer[mixer_id].solos == channel_bit)
					{
						// unsolo all
						m_bl_desc.mixer[mixer_id].solos = 0;

						// restore mute states
						m_bl_desc.mixer[mixer_id].mutes = m_bl_desc.mixer[mixer_id].was_muted;
						bUpdateGui = true;
					}
					else
					{
						// other channels are soloed, so unsolo this one only
						m_bl_desc.mixer[mixer_id].solos ^= channel_bit;

						// restore mute state
						if (m_bl_desc.mixer[mixer_id].was_muted & channel_bit)
						{
							m_bl_desc.mixer[mixer_id].mutes |= channel_bit;
						}
						bUpdateGui = true;
					}
				}

			}
			else if (0 == type.compare("dirout"))
			{
				m_bl_desc.mixer[mixer_id].direct_out = (0 != value);

				setDirectOut(mixer_id);
				sendDirout(mixer_id, m_bl_desc.mixer[mixer_id].direct_out);
				String msg(String::formatted("dirout: mixer id:%d, %d", mixer_id, m_bl_desc.mixer[mixer_id].direct_out));
				EventLogger::getInstance()->logMessage(msg);
			}
			else if (0 == type.compare("reset"))
			{
				for (int i=0; i<m_bl_desc.mixer[mixer_id].num_channels; i++)
				{
					m_bl_desc.mixer[mixer_id].channel[i].gain = 101;
					m_bl_desc.mixer[mixer_id].channel[i].gain_dbv = m_dBvMap[101];
					sendChGain(mixer_id, i, 101);
					m_bl_desc.mixer[mixer_id].channel[i].pan = 0;
					sendChPan(mixer_id, i, 0);
				}
				m_bl_desc.mixer[mixer_id].mutes = 0;
				m_bl_desc.mixer[mixer_id].was_muted = 0;
				sendChMutes(mixer_id, m_bl_desc.mixer[mixer_id].mutes);
				m_bl_desc.mixer[mixer_id].solos = 0;
				sendChSolos(mixer_id, m_bl_desc.mixer[mixer_id].solos);

				m_bl_desc.mixer[mixer_id].master.gain = 101;
				m_bl_desc.mixer[mixer_id].master.gain_dbv = m_dBvMap[101];
				sendMasterGain(mixer_id, 101);
				m_bl_desc.mixer[mixer_id].master.muting = false;
				sendMasterMute(mixer_id, false);

				m_bl_desc.mixer[mixer_id].direct_out = 0;
				setDirectOut(mixer_id);
				sendDirout(mixer_id, 0);

				bUpdateGui = true;
			}
			// update mixer coeff's
			mixer_cfg(mixer_id);
		}

		if (bUpdateGui)
		{
			sendChMutes(mixer_id, m_bl_desc.mixer[mixer_id].mutes);
			sendChSolos(mixer_id, m_bl_desc.mixer[mixer_id].solos);
		}
	}
}

void Blender::createControls()
{
	jassert (m_initialized);

	if (m_created) return;

	for (int mixer_id=0; mixer_id<m_bl_desc.num_mixers; mixer_id++)
	{
		sendAddMixer(mixer_id, m_bl_desc.mixer[mixer_id].name);
		for (int ch_id=0; ch_id<m_bl_desc.mixer[mixer_id].num_channels; ch_id++)
		{
			sendAddChannel(mixer_id, ch_id, m_bl_desc.mixer[mixer_id].channel[ch_id].name);
			sendChannelPos(mixer_id, ch_id, m_bl_desc.mixer[mixer_id].channel[ch_id].x_offset);
		}
		sendMasterPos(mixer_id, m_bl_desc.mixer[mixer_id].master.x_offset);
		for (int label_id=0; label_id<m_bl_desc.mixer[mixer_id].num_group_labels; label_id++)
		{
			sendAddGroupLabel(mixer_id, m_bl_desc.mixer[mixer_id].group_label_x_offset[label_id], 
				m_bl_desc.mixer[mixer_id].group_label_name[label_id]);
		}
	}
	m_created = true;
	sendMixersCreated();
}

// update the mixer controls to stored settings
void Blender::initControls()
{
	jassert (m_initialized);	// settings should be loaded first

	for (int mixer_id=0; mixer_id<m_bl_desc.num_mixers; mixer_id++)
	{
		sendMixerDescription(mixer_id, m_bl_desc.mixer[mixer_id].description);

		sendChMutes(mixer_id, m_bl_desc.mixer[mixer_id].mutes);
		sendChSolos(mixer_id, m_bl_desc.mixer[mixer_id].solos);
		sendDirout(mixer_id, m_bl_desc.mixer[mixer_id].direct_out);
		sendMasterName(mixer_id, m_bl_desc.mixer[mixer_id].master.name);
		sendMasterMute(mixer_id, m_bl_desc.mixer[mixer_id].master.muting);
		sendMasterGain(mixer_id, m_bl_desc.mixer[mixer_id].master.gain);
		sendMasterPeakIds(mixer_id, m_bl_desc.mixer[mixer_id].master.peak_ids[0], m_bl_desc.mixer[mixer_id].master.peak_ids[1]);

		for (int ch=0; ch<m_bl_desc.mixer[mixer_id].num_channels; ch++)
		{
			sendChName(mixer_id, ch, m_bl_desc.mixer[mixer_id].channel[ch].name);
			sendChGain(mixer_id, ch, m_bl_desc.mixer[mixer_id].channel[ch].gain);
			sendChPan(mixer_id, ch, m_bl_desc.mixer[mixer_id].channel[ch].pan);
			sendChPeakId(mixer_id, ch, m_bl_desc.mixer[mixer_id].channel[ch].peak_id);
		}
	}
}

void Blender::getChangeMessage(change_msg * msg)
{
	*msg = m_bl_desc.msg;
}

// send gain value to mix channel
void Blender::sendChGain(int mixer_id, int ch_id, int value)
{
	m_bl_desc.msg.type = kGain;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.ch_id = ch_id;
	m_bl_desc.msg.int_val1 = value;
	sendSynchronousChangeMessage();
}

// send pan value to mix channel
void Blender::sendChPan(int mixer_id, int ch_id, int value)
{
	m_bl_desc.msg.type = kPan;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.ch_id = ch_id;
	m_bl_desc.msg.int_val1 = value;
	sendSynchronousChangeMessage();
}

// send mute states to mix channels
void Blender::sendChMutes(int mixer_id, uint32 value)
{
	m_bl_desc.msg.type = kMutes;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = value;
	sendSynchronousChangeMessage();
}

// send solo states to mix channels
void Blender::sendChSolos(int mixer_id, uint32 value)
{
	m_bl_desc.msg.type = kSolos;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = value;
	sendSynchronousChangeMessage();
}

// send peak index to mix channel
void Blender::sendChPeakId(int mixer_id, int ch_id, int value)
{
	m_bl_desc.msg.type = kPeakId;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.ch_id = ch_id;
	m_bl_desc.msg.int_val1 = value;
	sendSynchronousChangeMessage();
}

// send peak multiplier to channel strips
void Blender::sendChPeakMult(int mixer_id, int ch_id, float value)
{
	m_bl_desc.msg.type = kPeakMult;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.ch_id = ch_id;
	m_bl_desc.msg.float_val = value;
	sendSynchronousChangeMessage();
}

// send name to channel strip
void Blender::sendChName(int mixer_id, int ch_id, const char * name)
{
	m_bl_desc.msg.type = kChName;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.ch_id = ch_id;
	m_bl_desc.msg.str = name;
	sendSynchronousChangeMessage();
}

// send gain value to master output channel
void Blender::sendMasterGain(int mixer_id, int value)
{
	m_bl_desc.msg.type = kMasterGain;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = value;
	sendSynchronousChangeMessage();
}

// send mute state to output channel
void Blender::sendMasterMute(int mixer_id, bool value)
{
	m_bl_desc.msg.type = kMasterMute;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = value;
	sendSynchronousChangeMessage();
}

// send peak id's to output channel
void Blender::sendMasterPeakIds(int mixer_id, int id_l, int id_r)
{
	m_bl_desc.msg.type = kMasterPeakIds;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = id_l;
	m_bl_desc.msg.int_val2 = id_r;
	sendSynchronousChangeMessage();
}

// send name to output channel
void Blender::sendMasterName(int mixer_id, const char * name)
{
	m_bl_desc.msg.type = kMasterName;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.str = name;
	sendSynchronousChangeMessage();
}

// send direct out state to mixer
void Blender::sendDirout(int mixer_id, int value)
{
	m_bl_desc.msg.type = kDirectOut;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = (uint32)value;
	sendSynchronousChangeMessage();
}

// send new mixer
void Blender::sendAddMixer(int mixer_id, const char * name)
{
	m_bl_desc.msg.type = kAddMixer;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.str = name;
	sendSynchronousChangeMessage();
}

// send remove mixers
void Blender::sendRemoveMixers()
{
	m_bl_desc.msg.type = kRemoveMixers;
	m_bl_desc.msg.mixer_id = 0xffff;
	sendSynchronousChangeMessage();
}

// send new channel
void Blender::sendAddChannel(int mixer_id, int ch_id, const char * name)
{
	m_bl_desc.msg.type = kAddChannel;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.ch_id = ch_id;
	m_bl_desc.msg.str = name;
	sendSynchronousChangeMessage();
}

// send channel position
void Blender::sendChannelPos(int mixer_id, int ch_id, int x_offset)
{
	m_bl_desc.msg.type = kChannelPos;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.ch_id = ch_id;
	m_bl_desc.msg.int_val1 = (uint32)x_offset;
	sendSynchronousChangeMessage();
}

// send output channel position
void Blender::sendMasterPos(int mixer_id, int x_offset)
{
	m_bl_desc.msg.type = kMasterPos;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = (uint32)x_offset;
	sendSynchronousChangeMessage();
}

void Blender::sendAddGroupLabel(int mixer_id, int x_offset, const char * text)
{
	m_bl_desc.msg.type = kAddGroupLabel;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.int_val1 = x_offset;
	m_bl_desc.msg.str = text;
	sendSynchronousChangeMessage();
}

// send mixer gui created
void Blender::sendMixersCreated()
{
	m_bl_desc.msg.type = kMixersCreated;
	m_bl_desc.msg.mixer_id = 0xffff;
	sendSynchronousChangeMessage();
}

// send description to mixer
void Blender::sendMixerDescription(int mixer_id, const char * text)
{
	m_bl_desc.msg.type = kMixerText;
	m_bl_desc.msg.mixer_id = mixer_id;
	m_bl_desc.msg.str = text;
	sendSynchronousChangeMessage();
}

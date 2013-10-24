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

#include "Blender.h"
#include "BinaryData.h"
#include "EventLogger.h"

extern float dBuMap[];

bool Blender::getDefaultSettingsFromBinaryData()
{
	String xmlText(String::empty);

	tcat::dice::FL_GET_APP_INFO_RETURN runningInfo = m_device->firmware_info();

	if (0x166 == runningInfo.uiVendorID) // TCAT EVM's
	{
		if (4 == runningInfo.uiProductID)
		{
			xmlText = BinaryData::eap_evm002_defaults_xml;
		}
		else if (6 == runningInfo.uiProductID)
		{
			xmlText = BinaryData::eap_evm003_defaults_xml;
		}
	}

	if (!xmlText.isEmpty())
	{
		XmlDocument xmlDoc (xmlText);
        xml = xmlDoc.getDocumentElement();
		if (!xml) return false;

		// get the savefile version
		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			if (child->compareAttribute("NAME", "version", true))
			{
				m_bl_desc.version = child->getIntAttribute("val", -1);
			}
		}

		if (m_bl_desc.version < BL_MIN_SAVEFILE_VERSION)
		{
			return false;
		}

		// check for a match with serial number masked off
		uint32 wwuid_high = (uint32)((m_guid>>32) & 0xffffff00);
		uint32 wwuid_low = (uint32)(m_guid & 0xffc00000);
		String descriptionId = String::formatted("%08x%08x", wwuid_high, (wwuid_low & 0xffc00000));
		
		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			if (child->compareAttribute("NAME", descriptionId, true))
			{
				XmlElement * description = child->getChildByName("EAP_DESC");
				if (description)
				{
					if (parseSettings(description))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Blender::getDefaultSettingsFromDefaultsFile()
{
#if JUCE_MAC
    File dir (File::getSpecialLocation (File::currentApplicationFile));
    
    dir = dir.getChildFile ("Contents/Resources");
    
	File defaultsFile (dir.getChildFile ("eap_mixer")
                       .withFileExtension ("xml"));
#endif
    
#if JUCE_WIN32
    File defaultsFile (File::getSpecialLocation (File::currentExecutableFile).getParentDirectory().getChildFile("eap_mixer.xml"));
#endif
    
	if (defaultsFile.exists())
	{
		XmlDocument eapXmlDoc (defaultsFile);
		xml = eapXmlDoc.getDocumentElement();
		if (!xml) return false;

		// get the savefile version
		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			// get the savefile version
			if (child->compareAttribute("NAME", "version", true))
			{
				m_bl_desc.version = child->getIntAttribute("val", -1);
			}
		}
		
		if (m_bl_desc.version < BL_MIN_SAVEFILE_VERSION)
		{
			return false;
		}

		// check for a match with category id and serial number masked off
		uint32 wwuid_high = (uint32)((m_guid>>32) & 0xffffff00);
		uint32 wwuid_low = (uint32)(m_guid & 0xffc00000);
		String descriptionId = String::formatted("%08x%08x", wwuid_high, (wwuid_low & 0xffc00000));

		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			if (child->compareAttribute("NAME", descriptionId, true))
			{
				XmlElement * description = child->getChildByName("EAP_DESC");
				if (description)
				{
					if (parseSettings(description))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Blender::getDefaultSettingsFromSaveFile()
{
	File defaultsFile(m_eap_props->getFile());
	if (defaultsFile.exists())
	{
		XmlDocument eapXmlDoc (defaultsFile);
		xml = eapXmlDoc.getDocumentElement();
		if (!xml) return false;

		// get the savefile version
		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			// get the savefile version
			if (child->compareAttribute("NAME", "version", true))
			{
				m_bl_desc.version = child->getIntAttribute("val", -1);
			}
		}

		if (m_bl_desc.version < BL_MIN_SAVEFILE_VERSION)
		{
			return false;
		}

		// look for a device description matching the device wwuid
		uint32 wwuid_high = (uint32)((m_guid>>32) & 0xffffffff);
		uint32 wwuid_low = (uint32)(m_guid & 0xffffffff);
		String descriptionId = String::formatted("%08x%08x", wwuid_high, wwuid_low);

		// if a matching device WWUID is found in the save file, parse it for all settings
		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			if (child->compareAttribute("NAME", descriptionId, true))
			{
				XmlElement * description = child->getChildByName("EAP_DESC");
				if (description)
				{
					if (parseSettings(description))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool Blender::getDefaultSettingsFromDeviceFlash()
{
	std::string buf;

	if (m_device->firmware_download(&buf, "eap_defaults"))
	{
		XmlDocument xmlDoc (String (buf.c_str()));
		xml = xmlDoc.getDocumentElement();
		if (!xml) return false;

		// get the savefile version
		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			// get the savefile version
			if (child->compareAttribute("NAME", "version", true))
			{
				m_bl_desc.version = child->getIntAttribute("val", -1);
			}
		}

		if (m_bl_desc.version < BL_MIN_SAVEFILE_VERSION)
		{
			return false;
		}

		// check for a match with serial number masked off
		uint32 wwuid_high = (uint32)((m_guid>>32) & 0xffffff00);
		uint32 wwuid_low = (uint32)(m_guid & 0xffc00000);
		String descriptionId = String::formatted("%08x%08x", wwuid_high, (wwuid_low & 0xffc00000));

		forEachXmlChildElementWithTagName (*xml, child, "VALUE")
		{
			if (child->compareAttribute("NAME", descriptionId, true))
			{
				XmlElement * description = child->getChildByName("EAP_DESC");
				if (description)
				{
					if (parseSettings(description))
					{
						return true;
					}
					else
					{
						AlertWindow::showMessageBox (AlertWindow::WarningIcon,
							"Mixer Panel",
							"The Mixer Settings defaults file in the device is corrupted or incompatible.\nContact your product manufacturer for support.");
					}
				}
			}
		}
	}

	return false;
}

void Blender::loadSettings()
{
	// specify the settings file name and format
	EapProperties::getInstance()->setStorageParameters(kTCAT_DICE_DRV_STR, ".eap.settings", -1, EapPropertiesFile::storeAsXML);

	// make a pointer to the properties for convenience
	m_eap_props = EapProperties::getInstance()->getEapSettings();

	// check that changes can be saved
	if (!EapProperties::getInstance()->testWriteAccess(true))
	{
		AlertWindow::showMessageBox (AlertWindow::WarningIcon,
			"Mixer Panel",
			"Mixer Settings will not be saved due to write permissions.\nReinstall your software or contact your product manufacturer for support.");
	}

	// look for existing settings save file and load
	if (getDefaultSettingsFromSaveFile())
	{
		m_bl_desc.isValid = true;
		return;
	}

	// settings save file not found, use settings from defaults file
	if (getDefaultSettingsFromDefaultsFile())
	{
		m_bl_desc.isValid = true;
		return;
	}

	// missing or invalid savefile and defaults file, look for the description in device flash
	if (getDefaultSettingsFromDeviceFlash())
	{
		m_bl_desc.isValid = true;
		return;
	}

	// not found in the device, try to use default settings from built-in binary data (TCAT EVM's only)
	if (getDefaultSettingsFromBinaryData())
	{
		m_bl_desc.isValid = true;
		return;
	}

	AlertWindow::showMessageBox (AlertWindow::WarningIcon,
		"Mixer Panel",
		"A Mixer Description was not found,\nor is not compatible.\n\nReinstall your software or contact your\nproduct manufacturer for support.");
}

bool Blender::parseSettings(XmlElement *description)
{
	memset (&m_bl_desc.coeff, 0, sizeof (m_bl_desc.coeff));
	memset (&m_bl_desc.curr_cfg, 0, sizeof (m_bl_desc.curr_cfg));
	m_bl_desc.modes = 0;

	if (!description) return false;

	// get the mode (streaming and routing) configurations
	XmlElement * const curr_cfg = description->getChildByName("CURR_CFG");
	if (!curr_cfg) return false;

	// modes
	int numModesFound = 0;
	forEachXmlChildElementWithTagName (*curr_cfg, mode, "MODE")
	{
		int mode_id = mode->getIntAttribute("ID", -1);
		if (mode_id < 0) return false;

		numModesFound++;

		// add this mode
		m_bl_desc.modes |= ((1 << (16 + mode_id)) | tcat::dice::kExecute);

		// routes
		XmlElement * const router = mode->getChildByName("ROUTER");
		if (!router) return false;

		int num_routes = router->getIntAttribute("NUM_ROUTES", -1);
		if ((num_routes < 0) || (num_routes > MIX_MAX_NUM_ROUTES)) return false;
		// get the mixer enabled routings for the mode
		m_bl_desc.curr_cfg.modes[mode_id].router.nbEntries = num_routes;
		m_bl_desc.curr_cfg.modes[mode_id].mixer_enabled_routes.nbEntries = num_routes;
		String routes_str = router->getStringAttribute("ROUTES");
		if (0 == routes_str.compare(String::empty)) return false;
		for (int i=0; i<num_routes; i++)
		{
			uint32 route = routes_str.upToFirstOccurrenceOf(" ", false, false).getHexValue32();
			m_bl_desc.curr_cfg.modes[mode_id].router.route[i] = route;
			m_bl_desc.curr_cfg.modes[mode_id].mixer_enabled_routes.route[i] = route;
			routes_str = routes_str.fromFirstOccurrenceOf(" ", false, false);
		}

		// get the direct out routings for the mode
		m_bl_desc.curr_cfg.modes[mode_id].direct_out_routes.nbEntries = num_routes;
		routes_str = router->getStringAttribute("DIROUT_ROUTES");
		if (0 == routes_str.compare(String::empty)) return false;
		for (int i=0; i<num_routes; i++)
		{
			uint32 route = routes_str.upToFirstOccurrenceOf(" ", false, false).getHexValue32();
			m_bl_desc.curr_cfg.modes[mode_id].direct_out_routes.route[i] = route;
			routes_str = routes_str.fromFirstOccurrenceOf(" ", false, false);
		}

		// streaming
		XmlElement * const strm_cfg = mode->getChildByName("STRM_CFG");
		if (!strm_cfg) return false;

		int num_tx_isoc_ch = strm_cfg->getIntAttribute("NUM_TX_ISOC_CH", -1);
		if (num_tx_isoc_ch < 0) return false;
		m_bl_desc.curr_cfg.modes[mode_id].strmCfg.txIsocChannels = num_tx_isoc_ch;

		int num_rx_isoc_ch = strm_cfg->getIntAttribute("NUM_RX_ISOC_CH", -1);
		if (num_rx_isoc_ch < 0) return false;
		m_bl_desc.curr_cfg.modes[mode_id].strmCfg.rxIsocChannels = num_rx_isoc_ch;

		// isoch channels are defined tx first then rx
		forEachXmlChildElementWithTagName (*strm_cfg, channel, "CHANNEL")
		{
			int index = channel->getIntAttribute("ID", -1);
			if (index < 0) return false;
			int num_audio_seq = channel->getIntAttribute("NUM_AUDIO_SEQ", -1);
			if (num_audio_seq < 0) return false;
			m_bl_desc.curr_cfg.modes[mode_id].strmCfg.channels[index].sequences.audio = num_audio_seq;
			int num_midi_seq = channel->getIntAttribute("NUM_MIDI_SEQ", -1);
			if (num_midi_seq < 0) return false;
			m_bl_desc.curr_cfg.modes[mode_id].strmCfg.channels[index].sequences.midi = num_midi_seq;
			int ac3_caps = channel->getIntAttribute("AC3_CAPS", -1);
			if (ac3_caps < 0) return false;
			m_bl_desc.curr_cfg.modes[mode_id].strmCfg.channels[index].ac3_caps = ac3_caps;

			String names_from_file = channel->getStringAttribute("NAMES");
			if (0 == names_from_file.compare(String::empty)) return false;
			String names = names_from_file.replace("\\\\", ("\\"));
			memset(m_bl_desc.curr_cfg.modes[mode_id].strmCfg.channels[index].names, '\0', 
				sizeof(m_bl_desc.curr_cfg.modes[mode_id].strmCfg.channels[index].names));
			int len = names.length();
			strncpy (m_bl_desc.curr_cfg.modes[mode_id].strmCfg.channels[index].names,
					names.toUTF8(), len);
		}
	} // foreach mode

	if (numModesFound < 3) return false;

	// get the mixer configurations
	XmlElement * const mixers = description->getChildByName("MIXERS");
	if (!mixers) return false;

	m_bl_desc.num_mixers = mixers->getIntAttribute("NUM_MIXERS", -1);
	if (m_bl_desc.num_mixers <= 0) return false;
	forEachXmlChildElementWithTagName (*mixers, mixer, "MIXER")
	{
		int mixer_id = mixer->getIntAttribute("ID", -1);
		if (mixer_id < 0) return false;

		String name = mixer->getStringAttribute("NAME");
		if (0 == name.compare(String::empty)) return false;
		String descr = mixer->getStringAttribute("DESCRIPTION");
		if (0 == descr.compare(String::empty)) return false;
		memset(m_bl_desc.mixer[mixer_id].name, '\0', 
			sizeof (m_bl_desc.mixer[mixer_id].name));
		strncpy (m_bl_desc.mixer[mixer_id].name,
			name.toUTF8(), name.length());
		memset(m_bl_desc.mixer[mixer_id].description, '\0', 
			sizeof (m_bl_desc.mixer[mixer_id].description));
		strncpy (m_bl_desc.mixer[mixer_id].description,
			descr.toUTF8(), descr.length());

		int num_channels = mixer->getIntAttribute("NUM_CHANNELS", -1);
		if (num_channels < 0) return false;
		m_bl_desc.mixer[mixer_id].num_channels = (uint16)num_channels;
		uint32 mutes = mixer->getStringAttribute("MUTES").getHexValue32();
		m_bl_desc.mixer[mixer_id].mutes = mutes;
		int was_muted = mixer->getStringAttribute("WAS_MUTED").getHexValue32();
		m_bl_desc.mixer[mixer_id].was_muted = was_muted;
		int solos = mixer->getStringAttribute("SOLOS").getHexValue32();
		m_bl_desc.mixer[mixer_id].solos = solos;
		int direct_out = mixer->getIntAttribute("DIRECT_OUT");
		m_bl_desc.mixer[mixer_id].direct_out = direct_out;

		// direct out
		String dirout_routes_str(String::empty);
		dirout_routes_str = mixer->getStringAttribute("DIROUT_ROUTES_LOW");
		if (0 == dirout_routes_str.compare(String::empty)) return false;
		uint32 route_index = dirout_routes_str.upToFirstOccurrenceOf(" ", false, false).getIntValue();
		m_bl_desc.mixer[mixer_id].dirout_route_ids[0][0] = route_index;
		dirout_routes_str = dirout_routes_str.fromFirstOccurrenceOf(" ", false, false);
		route_index = dirout_routes_str.getIntValue();
		m_bl_desc.mixer[mixer_id].dirout_route_ids[0][1] = route_index;
		
		dirout_routes_str = mixer->getStringAttribute("DIROUT_ROUTES_MID");
		if (0 == dirout_routes_str.compare(String::empty)) return false;
		uint32 route_id = dirout_routes_str.upToFirstOccurrenceOf(" ", false, false).getIntValue();
		m_bl_desc.mixer[mixer_id].dirout_route_ids[1][0] = route_id;
		dirout_routes_str = dirout_routes_str.fromFirstOccurrenceOf(" ", false, false);
		route_index = dirout_routes_str.getIntValue();
		m_bl_desc.mixer[mixer_id].dirout_route_ids[1][1] = route_index;

		dirout_routes_str = mixer->getStringAttribute("DIROUT_ROUTES_HIGH");
		if (0 == dirout_routes_str.compare(String::empty)) return false;
		uint32 dirout_route_id = dirout_routes_str.upToFirstOccurrenceOf(" ", false, false).getIntValue();
		m_bl_desc.mixer[mixer_id].dirout_route_ids[2][0] = dirout_route_id;
		dirout_routes_str = dirout_routes_str.fromFirstOccurrenceOf(" ", false, false);
		dirout_route_id = dirout_routes_str.getIntValue();
		m_bl_desc.mixer[mixer_id].dirout_route_ids[2][1] = dirout_route_id;

		// if the mixer is set to direct out, initialize to direct out routes
		if (m_bl_desc.mixer[mixer_id].direct_out)
		{
			for (int mode=0; mode<3; mode++)
			{
				for (int output=0; output<2; output++)
				{
					int route_index = m_bl_desc.mixer[mixer_id].dirout_route_ids[mode][output];
					uint32 new_route = m_bl_desc.curr_cfg.modes[mode].direct_out_routes.route[route_index];
					m_bl_desc.curr_cfg.modes[mode].router.route[route_index] = new_route;
				}
			}
		}

		// channel strips
		forEachXmlChildElementWithTagName (*mixer, channel, "CHANNEL")
		{
			int ch = channel->getIntAttribute("ID", -1);
			if (ch < 0) return false;
			String name = channel->getStringAttribute("NAME");
			memset(m_bl_desc.mixer[mixer_id].channel[ch].name, '\0', 
				sizeof (m_bl_desc.mixer[mixer_id].channel[ch].name));
			strncpy (m_bl_desc.mixer[mixer_id].channel[ch].name,
					name.toUTF8(), name.length());

			int gain = channel->getIntAttribute("GAIN");
			m_bl_desc.mixer[mixer_id].channel[ch].gain = gain;
			m_bl_desc.mixer[mixer_id].channel[ch].gain_dbv = m_dBvMap[gain];
			int pan = channel->getIntAttribute("PAN");
			m_bl_desc.mixer[mixer_id].channel[ch].pan = pan;
			int peak_id = channel->getIntAttribute("PEAK_ID");
			m_bl_desc.mixer[mixer_id].channel[ch].peak_id = peak_id;
			int x_offset = channel->getIntAttribute("X_OFFSET");
			m_bl_desc.mixer[mixer_id].channel[ch].x_offset = x_offset;
		}

		// master output
		XmlElement * const master = mixer->getChildByName("MASTER");
		if (!master) return false;

		String mstr_name = master->getStringAttribute("NAME");
		memset (m_bl_desc.mixer[mixer_id].master.name, '\0',
			sizeof(m_bl_desc.mixer[mixer_id].master.name));
		strncpy (m_bl_desc.mixer[mixer_id].master.name,
			mstr_name.toUTF8(), mstr_name.length());
		int gain = master->getIntAttribute("GAIN");
		m_bl_desc.mixer[mixer_id].master.gain = gain;
		m_bl_desc.mixer[mixer_id].master.gain_dbv = m_dBvMap[gain];
		int muting = master->getIntAttribute("MUTING");
		m_bl_desc.mixer[mixer_id].master.muting = (0 == muting) ? false : true;
		int peak_id0 = master->getIntAttribute("PEAK_ID0");
		m_bl_desc.mixer[mixer_id].master.peak_ids[0] = peak_id0;
		int peak_id1 = master->getIntAttribute("PEAK_ID1");
		m_bl_desc.mixer[mixer_id].master.peak_ids[1] = peak_id1;
		int x_offset = master->getIntAttribute("X_OFFSET");
		m_bl_desc.mixer[mixer_id].master.x_offset = x_offset;

		// channel grouping labels
		m_bl_desc.mixer[mixer_id].num_group_labels=0;
		forEachXmlChildElementWithTagName (*mixer, label, "LABEL")
		{
			int id = label->getIntAttribute("ID");
			if (id < 0) return false;
			String name = label->getStringAttribute("NAME");
			memset(m_bl_desc.mixer[mixer_id].group_label_name[id], '\0', 
				sizeof (m_bl_desc.mixer[mixer_id].group_label_name[id]));
			strncpy (m_bl_desc.mixer[mixer_id].group_label_name[id],
					name.toUTF8(), name.length());

			int x_offset = label->getIntAttribute("X_OFFSET");
			m_bl_desc.mixer[mixer_id].group_label_x_offset[id] = x_offset;
			m_bl_desc.mixer[mixer_id].num_group_labels++;
		}
	} // foreach mixer

	return true;
}

void Blender::saveSettings()
{
	if (!m_bl_desc.isValid) return;

	if (!EapProperties::getInstance()->testWriteAccess(false))
	{
		// by this time, we have already warned the user if the file can't be written
		return;
	}

	// check that we're saving a valid description
	if ( (0 == m_guid)
		|| (m_bl_desc.num_mixers < 1)
		|| (m_bl_desc.curr_cfg.modes[0].strmCfg.rxIsocChannels < 1)
		|| (m_bl_desc.curr_cfg.modes[0].router.nbEntries < 4) )
	{
		return;
	}

	XmlElement eap_desc ("EAP_DESC");

	XmlElement * version = new XmlElement ("VER");
	version->setAttribute("VERSION", BL_SAVEFILE_VERSION);
	eap_desc.addChildElement(version);

	XmlElement * curr_cfg = new XmlElement ("CURR_CFG");

	// current configurations (modes)
	XmlElement * mode[3] = { 0 };
	XmlElement * routes[3] = { 0 };
	uint32 supported_modes = m_bl_desc.modes>>16;
	for (int mode_index=0; mode_index<3; mode_index++)
	{
		if (supported_modes & (1<<mode_index))
		{
			mode[mode_index] = new XmlElement ("MODE");
			mode[mode_index]->setAttribute("ID", mode_index);

			routes[mode_index] = new XmlElement ("ROUTER");
			if (routes != 0)
			{
				int num_routes = m_bl_desc.curr_cfg.modes[mode_index].mixer_enabled_routes.nbEntries;
				routes[mode_index]->setAttribute("NUM_ROUTES", num_routes);

				// mixer enabled routes
				String routes_str(String::empty);
				for (int i=0; i<num_routes; i++)
				{
					routes_str << String::formatted("%04x ", m_bl_desc.curr_cfg.modes[mode_index].mixer_enabled_routes.route[i]);
				}
				routes[mode_index]->setAttribute("ROUTES", routes_str);

				// mixer disabled routes
				routes_str = String::empty;
				for (int i=0; i<num_routes; i++)
				{
					routes_str << String::formatted("%04x ", m_bl_desc.curr_cfg.modes[mode_index].direct_out_routes.route[i]);
				}
				routes[mode_index]->setAttribute("DIROUT_ROUTES", routes_str);

				mode[mode_index]->addChildElement(routes[mode_index]);

				// streaming
				XmlElement * strm_cfg = new XmlElement ("STRM_CFG");
				int num_tx_ch = m_bl_desc.curr_cfg.modes[mode_index].strmCfg.txIsocChannels;
				int num_rx_ch = m_bl_desc.curr_cfg.modes[mode_index].strmCfg.rxIsocChannels;
				strm_cfg->setAttribute("NUM_TX_ISOC_CH", num_tx_ch);
				strm_cfg->setAttribute("NUM_RX_ISOC_CH", num_rx_ch);

				if ((num_tx_ch+num_rx_ch) > (kTCAT_EAP_MAX_TX+kTCAT_EAP_MAX_RX))
				{
					EventLogger::getInstance()->logMessage("Invalid channel count found in mixer description:");
					String errStr(String::formatted("  tx: %d, rx: %d", num_tx_ch, num_rx_ch));
				}
				XmlElement * isoc_channel[4];
				String name_str(String::empty);
				String name_str_to_file(String::empty);
				for (int i=0; i<(num_tx_ch+num_rx_ch); i++)
				{
					isoc_channel[i] = new XmlElement("CHANNEL");
					isoc_channel[i]->setAttribute("ID", i);
					isoc_channel[i]->setAttribute("NUM_AUDIO_SEQ", (int)m_bl_desc.curr_cfg.modes[mode_index].strmCfg.channels[i].sequences.audio);
					isoc_channel[i]->setAttribute("NUM_MIDI_SEQ", (int)m_bl_desc.curr_cfg.modes[mode_index].strmCfg.channels[i].sequences.midi);
					name_str = m_bl_desc.curr_cfg.modes[mode_index].strmCfg.channels[i].names;
					name_str_to_file = name_str.replace("\\", "\\\\");
					isoc_channel[i]->setAttribute("NAMES", name_str_to_file);
					isoc_channel[i]->setAttribute("AC3_CAPS", (int)m_bl_desc.curr_cfg.modes[mode_index].strmCfg.channels[i].ac3_caps);
					strm_cfg->addChildElement(isoc_channel[i]);
				}
				mode[mode_index]->addChildElement(strm_cfg);
			}
			curr_cfg->addChildElement(mode[mode_index]);
		}
	}
	eap_desc.addChildElement(curr_cfg);

	// mixer settings
	XmlElement * mixers = new XmlElement ("MIXERS");
	mixers->setAttribute("NUM_MIXERS", m_bl_desc.num_mixers);

	XmlElement * mixer[MIX_MAX_NUM_MIXERS];
	for (int mixer_index=0; mixer_index<m_bl_desc.num_mixers; mixer_index++)
	{
		mixer[mixer_index] = new XmlElement ("MIXER");
		mixer[mixer_index]->setAttribute("ID", mixer_index);
		mixer[mixer_index]->setAttribute("NAME", m_bl_desc.mixer[mixer_index].name);
		mixer[mixer_index]->setAttribute("DESCRIPTION", m_bl_desc.mixer[mixer_index].description);
		mixer[mixer_index]->setAttribute("NUM_CHANNELS", m_bl_desc.mixer[mixer_index].num_channels);
		mixer[mixer_index]->setAttribute("MUTES", String::formatted("%04x", m_bl_desc.mixer[mixer_index].mutes));
		mixer[mixer_index]->setAttribute("WAS_MUTED", String::formatted("%04x", m_bl_desc.mixer[mixer_index].was_muted));
		mixer[mixer_index]->setAttribute("SOLOS", String::formatted("%04x", m_bl_desc.mixer[mixer_index].solos));
		mixer[mixer_index]->setAttribute("DIRECT_OUT", m_bl_desc.mixer[mixer_index].direct_out);

		// channel strips
		for (int ch=0; ch<m_bl_desc.mixer[mixer_index].num_channels; ch++)
		{
			XmlElement * channel = new XmlElement("CHANNEL");
			channel->setAttribute("ID", ch);
			channel->setAttribute("NAME", m_bl_desc.mixer[mixer_index].channel[ch].name);
			channel->setAttribute("GAIN", m_bl_desc.mixer[mixer_index].channel[ch].gain);
			channel->setAttribute("PAN", m_bl_desc.mixer[mixer_index].channel[ch].pan);
			channel->setAttribute("PEAK_ID", m_bl_desc.mixer[mixer_index].channel[ch].peak_id);
			channel->setAttribute("X_OFFSET", m_bl_desc.mixer[mixer_index].channel[ch].x_offset);
			mixer[mixer_index]->addChildElement(channel);
		}

		// master
		XmlElement * master = new XmlElement("MASTER");
		master->setAttribute("NAME", m_bl_desc.mixer[mixer_index].master.name);
		master->setAttribute("GAIN", m_bl_desc.mixer[mixer_index].master.gain);
		master->setAttribute("MUTING", m_bl_desc.mixer[mixer_index].master.muting);
		master->setAttribute("PEAK_ID0", m_bl_desc.mixer[mixer_index].master.peak_ids[0]);
		master->setAttribute("PEAK_ID1", m_bl_desc.mixer[mixer_index].master.peak_ids[1]);
		master->setAttribute("X_OFFSET", m_bl_desc.mixer[mixer_index].master.x_offset);
		mixer[mixer_index]->addChildElement(master);

		// channel grouping labels
		for (int id=0; id<m_bl_desc.mixer[mixer_index].num_group_labels; id++)
		{
			XmlElement * label = new XmlElement("LABEL");
			label->setAttribute("ID", id);
			label->setAttribute("NAME", m_bl_desc.mixer[mixer_index].group_label_name[id]);
			label->setAttribute("X_OFFSET", m_bl_desc.mixer[mixer_index].group_label_x_offset[id]);
			mixer[mixer_index]->addChildElement(label);
		}

		// direct out routing
		String route_str(String::empty);
		route_str = String::formatted("%d ", m_bl_desc.mixer[mixer_index].dirout_route_ids[0][0])
					+ String::formatted("%d ", m_bl_desc.mixer[mixer_index].dirout_route_ids[0][1]);
		mixer[mixer_index]->setAttribute("DIROUT_ROUTES_LOW", route_str);
		route_str = String::formatted("%d ", m_bl_desc.mixer[mixer_index].dirout_route_ids[1][0])
					+ String::formatted("%d ", m_bl_desc.mixer[mixer_index].dirout_route_ids[1][1]);
		mixer[mixer_index]->setAttribute("DIROUT_ROUTES_MID", route_str);
		route_str = String::formatted("%d ", m_bl_desc.mixer[mixer_index].dirout_route_ids[2][0])
					+ String::formatted("%d ", m_bl_desc.mixer[mixer_index].dirout_route_ids[2][1]);
		mixer[mixer_index]->setAttribute("DIROUT_ROUTES_HIGH", route_str);

		mixers->addChildElement(mixer[mixer_index]);
	}
	eap_desc.addChildElement(mixers);

	m_eap_props->setValue("version", m_bl_desc.version);

	// save the description with the WWUID as its identifier
	uint32 wwuid_low = (uint32)((m_guid>>32) & 0xffffffff);
	uint32 wwuid_high = (uint32)(m_guid & 0xffffffff);
	String descriptionId = String::formatted("%08x%08x", wwuid_low, wwuid_high);
	m_eap_props->setValue(descriptionId, &eap_desc);
	m_eap_props->save();
}


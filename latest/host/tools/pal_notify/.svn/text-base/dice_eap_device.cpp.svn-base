//===========================================================================
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
//===========================================================================
// $Id: dice_notification.cpp,v 1.2 2006/11/08 17:06:07 hagen Exp $
//===========================================================================
//
//	see dice_notification.h
//

#include "dice_eap_device.h"
#include <math.h>


using namespace tcat;
using namespace dice;



///////////////////////////////////////////////////////////////////////////////
//
//	eap_device::create
//
///////////////////////////////////////////////////////////////////////////////
//

void eap_device::create (dice::bus_impl* pBus, const dice::DEVICE_INFO& info) throw(tcat::exception::base)
{
	super::create(pBus, info);

	m_appSpace = NULL;
	m_eapSupported = supports_eap();

	if (!m_eapSupported) return;

	try {		
		//verify there is an app space
		uint32 eos; //end of struct is 0
		ohci_read (0x200000+kAppSpace*sizeof(EAPSPACE), &eos, 1);
		if (eos)
		{ //app space exists
			ohci_read (0x200000, (uint32*)&m_spaces, sizeof(m_spaces)>>2);
		}
		else
		{
			ohci_read (0x200000, (uint32*)&m_spaces, (sizeof(m_spaces)>>2)-2);
			m_spaces[kAppSpace].offset = 0;
			m_spaces[kAppSpace].size32 = 0;
		}		
		for (int i=0; i < kEapSpaces; i++)
		{
			m_spaces[i].offset <<= 2;
		}

		//read the capabilities
		if ((m_spaces[kCapabilities].size32 << 2) < sizeof(EAP_CAPABILITIES)) return;			
		ohci_read (0x200000 + m_spaces[kCapabilities].offset, (uint32*) &m_capabilities, sizeof(m_capabilities) >> 2);
		
		//read the current state
		updateCurrCfg();
		updateMixer();
	}
	catch (tcat::exception::base)
	{
		m_eapSupported = false;
	}

	return;
}


void eap_device::updateCurrCfg() throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	try {
		for (int i=0; i<3; i++)
		{
			ohci_read (0x200000 + m_spaces[kCurrCfg].offset+i*0x2000, (uint32*) &m_currCfg.modes[i].router, 1+routerMaxEntries());
			ohci_read (0x200000 + m_spaces[kCurrCfg].offset+0x1000+i*0x2000, (uint32*) &m_currCfg.modes[i].strmCfg, 2);
			if ((m_currCfg.modes[i].strmCfg.txIsocChannels > kTCAT_EAP_MAX_TX) || (m_currCfg.modes[i].strmCfg.rxIsocChannels > kTCAT_EAP_MAX_RX))
			{
				//error should throw exception
				return;
			}
			int nbIsocChannels = m_currCfg.modes[i].strmCfg.txIsocChannels + m_currCfg.modes[i].strmCfg.rxIsocChannels;
			ohci_read (0x200000 + m_spaces[kCurrCfg].offset+0x1008+i*0x2000, (uint32*) m_currCfg.modes[i].strmCfg.channels, (sizeof(EAP_CHANNEL)>>2)*nbIsocChannels);
		}
		ohci_read (0x200000 + m_spaces[kStndAlone].offset, (uint32*) &m_stdAlone, m_spaces[kStndAlone].size32);
		if (m_spaces[kAppSpace].size32)
		{
			m_appSpace = (uint32 *)malloc(m_spaces[kAppSpace].size32*4);
			ohci_read (0x200000 + m_spaces[kAppSpace].offset, m_appSpace, m_spaces[kAppSpace].size32);
		}
		else
		{
			m_appSpace = NULL;
		}
	}
	catch (tcat::exception::base)
	{
	}
	
}

void eap_device::updateMixer() throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if (mixerIns() * mixerOuts() > kTCAT_EAP_MAX_MIX_COEFF)
	{
		//error should throw exception
		return;
	}
	try
	{
		for (uint32 op = 0; op < mixerOuts(); op++)
		{
			//read one channel at a time
			uint32 *p = &m_mixer.coeff[(op * mixerIns())];
			ohci_read (0x200000 + m_spaces[kMixer].offset+4+ (op * mixerIns() * 4), p, mixerIns());
		}

		// get saturation bits
		uint32 *p = &m_mixer.satBits;
		ohci_read (0x200000 + m_spaces[kMixer].offset, p, 1);
	}
	catch (tcat::exception::base)
	{
	}
}

// inIdx: input channel row
// outIdx: output channel column
// coeff 0 to 65535
void eap_device::setMixerCoeffs (unsigned int inIdx, unsigned int outIdx, uint32 * coeff, unsigned int nbCoeff) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;
	
	if (((inIdx >= mixerIns()) || (outIdx >= mixerOuts())) ||
		((inIdx + nbCoeff) > mixerIns()))
	{
		//error should throw exception
		return;	
	}

	try
	{
		uint32 offset = inIdx + outIdx*mixerIns();

		if (memcmp(&m_mixer.coeff[offset], coeff, nbCoeff*sizeof(uint32)) != 0)
		{
			memcpy(&m_mixer.coeff[offset], coeff, nbCoeff*sizeof(uint32));
			ohci_write (0x200000 + m_spaces[kMixer].offset+4+(offset)*4, &m_mixer.coeff[offset], nbCoeff);
		}
	}
	catch (tcat::exception::base)
	{
	}
}

void eap_device::setMixerCoeff (unsigned int inIdx, unsigned int outIdx, uint32 coeff) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	setMixerCoeffs (inIdx, outIdx, &coeff, 1);
}

void eap_device::getPeak (unsigned int idx, tcat::uint32 * pPeaks, unsigned int nbPeaks) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if ((idx + nbPeaks) > routerMaxEntries())
	{
		return;
	}
	try
	{
		ohci_read (0x200000 + m_spaces[kPeak].offset+idx*4, (uint32*) pPeaks, nbPeaks);
		while (nbPeaks--)
		{
			uint32 peak = *pPeaks;
			*pPeaks = (peak >> 16) & 0x0fff;
			pPeaks++;
		}
	}
	catch (tcat::exception::base)
	{
	}
}

uint32 eap_device::getSatBits() throw(tcat::exception::base)
{
	if (!m_eapSupported) return 0;

	// update saturation bits
	try
	{
		ohci_read (0x200000 + m_spaces[kMixer].offset, &m_mixer.satBits, 1);
	}
	catch (tcat::exception::base)
	{
	}

	return m_mixer.satBits;
}

void eap_device::setRouting(EAP_NEW_ROUTER const * pRouter, unsigned modes) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	int size;
	uint32 i;
	
	if (pRouter->nbEntries > routerMaxEntries())
	{
		//?? throw exception
		return;
	}
	m_newRouter = *pRouter;
	//check if there are changes
	size = (1+pRouter->nbEntries)*sizeof(uint32);
	for (i=0; i<3; i++)
	{
		if (modes & (kLdLow<<i))
		{
			if (memcmp(&m_newRouter, &m_currCfg.modes[i].router,size) == 0)
				modes &= ~(kLdLow<<i);		
		}
	}
	if (modes & EAP_CMD_MODES_MASK)
	{
		try
		{
			ohci_write (0x200000 + m_spaces[kNewRouting].offset, (uint32*) &m_newRouter,1+pRouter->nbEntries);
			executeOpcode(kLdRouter | modes , true);
			//should we read back or just assume? For now assume success
			for (i=0; i<3; i++)
			{
				if (modes & (kLdLow<<i))
				{
					memcpy(&m_currCfg.modes[i].router,&m_newRouter,size);
				}
			}
		}
		catch (tcat::exception::base)
		{
		}
	}	
}


void eap_device::setStrmCfg (EAP_NEW_STRM_CFG const * pStrmCfg, unsigned modes) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	//This one would result in a device changed notification of epic proportions
	int i,size;
	
	if ((pStrmCfg->txIsocChannels > maxTxStreams()) || (pStrmCfg->rxIsocChannels > maxRxStreams()))
	{
		//?? throw exception
		return;
	}
	//check if there are changes
	size = 2*sizeof(uint32) + (pStrmCfg->txIsocChannels + pStrmCfg->rxIsocChannels)*sizeof(EAP_CHANNEL);
	for (i=0; i<3; i++)
	{
		if (modes & (kLdLow<<i))
		{
			if (memcmp(&m_currCfg.modes[i].strmCfg,pStrmCfg,size) == 0)
				modes &= ~(kLdLow<<i);		
		}
	}
	if (modes & EAP_CMD_MODES_MASK)
	{
		try
		{
			ohci_write (0x200000 + m_spaces[kNewStrmCfg].offset, (uint32*) pStrmCfg,size/4);
			executeOpcode(kLdRtrStrmCfg | modes , true);
		}
		catch (tcat::exception::base)
		{
		}
	}
}

void eap_device::storeToFlash (bool bNonBlocking) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	executeOpcode(kStFlashcfg , bNonBlocking);
}

void eap_device::storeFlashIfDirty (bool bNonBlocking) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	//later
}

void eap_device::executeOpcode(unsigned int opcode, bool bWaitComplete) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	uint32 cmd = uint32(kExecute);
	uint32 nStartTime = pal::time();
	
	//wait for previous command to complete
	while (cmd & kExecute)
	{
		uint32 nTime = pal::time();
		
		TCAT_EXCEPTION_THROW ((nTime-nStartTime) > kEAP_CMD_TIMEOUT, xptn_eap_timeout);
		try
		{
			ohci_read (0x200000 + m_spaces[kCmd].offset, &cmd, 1);
		}
		catch (tcat::exception::base)
		{
		}
		//Yield(); look at PAL for how to wait
	}
	//??? we should check overlapped return value
	cmd = opcode | kExecute;
	try
	{
		ohci_write (0x200000 + m_spaces[kCmd].offset, &cmd, 1);
	}
	catch (tcat::exception::base)
	{
	}
	
	if (bWaitComplete)
	{
		nStartTime = pal::time();
		while (cmd & kExecute)
		{
			uint32 nTime = pal::time();
			
			TCAT_EXCEPTION_THROW ((nTime-nStartTime) > kEAP_CMD_TIMEOUT, xptn_eap_timeout);
			try
			{
				ohci_read (0x200000 + m_spaces[kCmd].offset, &cmd, 1);
			}
			catch (tcat::exception::base)
			{
			}
			//Yield(); look at PAL for how to wait
		}
		//??? we should check return value
	}
}


void eap_device::setStdaClkSrc(unsigned int  clkSrc) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if (m_stdAlone.clkSrc == clkSrc) return;
	m_stdAlone.clkSrc = clkSrc;
	try
	{
		ohci_write (0x200000 + m_spaces[kStndAlone].offset, (uint32 *)&m_stdAlone, m_spaces[kStndAlone].size32);
		storeToFlash (true);
	}
	catch (tcat::exception::base)
	{
	}
}

void eap_device::setStdaExtAes(unsigned int  extAES) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if (m_stdAlone.extAES == extAES) return;
	m_stdAlone.extAES = extAES;
	try
	{
		ohci_write (0x200000 + m_spaces[kStndAlone].offset, (uint32 *)&m_stdAlone, m_spaces[kStndAlone].size32);	
		storeToFlash (true);
	}
	catch (tcat::exception::base)
	{
	}
}

void eap_device::setStdaExtADAT(unsigned int  extADAT) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if (m_stdAlone.extADAT == extADAT) return;
	m_stdAlone.extADAT = extADAT;
	try
	{
		ohci_write (0x200000 + m_spaces[kStndAlone].offset, (uint32 *)&m_stdAlone, m_spaces[kStndAlone].size32);
		storeToFlash (true);
	}
	catch (tcat::exception::base)
	{
	}
}

void eap_device::setStdaExtWC(unsigned int  extWC) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if (m_stdAlone.extWC == extWC) return;
	m_stdAlone.extWC = extWC;
	try
	{
		ohci_write (0x200000 + m_spaces[kStndAlone].offset, (uint32 *)&m_stdAlone, m_spaces[kStndAlone].size32);
		storeToFlash (true);
	}
	catch (tcat::exception::base)
	{
	}
}

void eap_device::setStdaExtInt(unsigned int  extInt) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if (m_stdAlone.extInt == extInt) return;
	m_stdAlone.extInt = extInt;
	try
	{
		ohci_write (0x200000 + m_spaces[kStndAlone].offset, (uint32 *)&m_stdAlone, m_spaces[kStndAlone].size32);
		storeToFlash (true);
	}
	catch (tcat::exception::base)
	{
	}
}
	
void eap_device::setAppSpaceItem(unsigned int  i, unsigned int  v) throw(tcat::exception::base)
{
	if (!m_eapSupported) return;

	if (i < m_spaces[kAppSpace].size32)
	{
		if (m_appSpace[i] == v) return;
		m_appSpace[i] = v;
		try
		{
			ohci_write (0x200000 + m_spaces[kAppSpace].offset, m_appSpace, m_spaces[kAppSpace].size32);
			storeToFlash (true);
		}
		catch (tcat::exception::base)
		{
		}
	}
}


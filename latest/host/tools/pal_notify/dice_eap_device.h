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
//	dice pal notification classes for EAP using the dice pal classes
//
#pragma once
#include "tcat_dice_pal_system.h"
#include "tcat_dice_firmware.h"

#include <iostream>



namespace tcat
{
namespace dice
{

TCAT_EXCEPTION_DECLARE (xptn_eap_timeout,	tcat::exception::base,	"EAP command timeout");


#define kTCAT_EAP_MAX_TX					2
#define kTCAT_EAP_MAX_RX					4
#define kTCAT_EAP_MAX_ROUTES				256
#define kTCAT_EAP_MAX_MIX_COEFF				288
#define kTCAT_EAP_MAX_MIX_OUT				16

enum EAPSPACES
{
	kCapabilities,
	kCmd,
	kMixer,
	kPeak,
	kNewRouting,
	kNewStrmCfg,
	kCurrCfg,
	kStndAlone,
	kAppSpace,
	kEapSpaces
};

struct EAPSPACE
{
	uint32 offset;
	uint32 size32;
};


enum EAP_ROUTER_CAP_FLAGS
{
	kRouterExposed = 0x00000001,
	kRouterReadOnly = 0x0000002,
	kRouterMaxEntriesMask = 0xffff0000,
	kRouterMaxEntriesShft = 16,
};

enum EAP_MIXER_CAP_FLAGS
{
	kMixerExposed = 0x00000001,
	kMixerReadOnly = 0x0000002,
	kMixerInDevMask = 0x000000f0,
	kMixerInDevShft = 4,
	kMixerOutDevMask = 0x00000f00,
	kMixerOutDevShft = 8,
	kMixerInsMask = 0x00ff0000,
	kMixerInsShft = 16,
	kMixerOutsMask = 0xff000000,
	kMixerOutsShft = 24
};
	
enum EAP_GENERAL_CAP_FLAGS
{
	kStrmCfgEnabled = 0x00000001,
	kFlashEnabled = 0x00000002,
	kMaxTxStreamsMask = 0x000000f0,
	kMaxTxStreamsShft = 4,
	kMaxRxStreamsMask = 0x00000f00,
	kMaxRxStreamsShft = 8,
	kChipTypeMask = 0x00ff0000,
	kChipTypeShft = 16
};



struct EAP_CAPABILITIES 
{
	uint32	routerCaps;
	uint32	mixerCaps;
	uint32	generalCaps;
	uint32	reserved;
};

enum EAP_OPCODE
{
	kNoOp = 0,
	kLdRouter,
	kLdStrmCfg,
	kLdRtrStrmCfg,
	kLdFlashCfg,
	kStFlashcfg
};

enum EAP_CMD_MODIFIERS
{
	kLdLow   = 0x00010000,
	kLdMid   = 0x00020000,
	kLdHigh  = 0x00040000,
	kExecute = 0x80000000
};

#define EAP_CMD_MODES_MASK (kLdLow | kLdMid | kLdHigh)

struct EAP_COMMAND 
{
	uint32	opCode;
	uint32	retVal;
};

struct EAP_MIXER
{
	uint32	satBits;
	uint32  coeff[kTCAT_EAP_MAX_MIX_COEFF];
};

struct EAP_NEW_ROUTER
{
	uint32	nbEntries;
	uint32	route[kTCAT_EAP_MAX_ROUTES];
};

struct EAP_CHANNEL
{
	dice::SEQUENCES	sequences;
	char    names[256];					// big endian channel names separated by /
	uint32	ac3_caps;
};

struct EAP_NEW_STRM_CFG
{
	uint32	txIsocChannels;
	uint32	rxIsocChannels;
	EAP_CHANNEL channels[kTCAT_EAP_MAX_TX+kTCAT_EAP_MAX_RX];
};

struct EAP_CURR_MODE_CFG
{
	EAP_NEW_ROUTER		router;
	EAP_NEW_ROUTER		mixer_enabled_routes;
	EAP_NEW_ROUTER		direct_out_routes;
	EAP_NEW_STRM_CFG	strmCfg;
};

struct EAP_CURR_CFG
{
	EAP_CURR_MODE_CFG	modes[3];
};

typedef struct
{
	uint32					clkSrc;
	uint32					extAES;
	uint32					extADAT;
	uint32					extWC;
	uint32					extInt;
	uint32					reserved[11];
} EAP_STND_ALONE_CFG;


///////////////////////////////////////////////////////////////////////////////
//
//	notification_device
//
///////////////////////////////////////////////////////////////////////////////
//
class eap_device: public dice::device
{
	typedef			dice::device super;

public:
    virtual void	create( dice::bus_impl* pBus, const dice::DEVICE_INFO& info )	throw(tcat::exception::base);	///< Updates device state. Overwrite this to handle device state change notifications

	// --- functions to access capabilities //
	
	inline bool	  isEapSupported() const {return m_eapSupported;}
	
	inline bool   isRouterExposed() const {return (m_capabilities.routerCaps & kRouterExposed);}
	inline bool   isRouterReadOnly() const {return (m_capabilities.routerCaps & kRouterReadOnly)? true : false;}
	inline uint32 routerMaxEntries() const {return ((m_capabilities.routerCaps & kRouterMaxEntriesMask) >> kRouterMaxEntriesShft);}

	inline bool   isMixerExposed() const {return (m_capabilities.mixerCaps & kMixerExposed);}
	inline bool   isMixerReadOnly() const {return (m_capabilities.mixerCaps & kMixerReadOnly)? true : false;}
	inline uint32 mixerFirstIndev() const {return ((m_capabilities.mixerCaps & kMixerInDevMask) >> kMixerInDevShft);}
	inline uint32 mixerFirstOutdev() const {return ((m_capabilities.mixerCaps & kMixerOutDevMask) >> kMixerOutDevShft);}
	inline uint32 mixerIns() const {return ((m_capabilities.mixerCaps & kMixerInsMask) >> kMixerInsShft);}
	inline uint32 mixerOuts() const {return ((m_capabilities.mixerCaps & kMixerOutsMask) >> kMixerOutsShft);}

	inline bool   isStrmCfgEnabled() const {return (m_capabilities.generalCaps & kStrmCfgEnabled);}
	inline bool   isFlashEnabled() const {return (m_capabilities.generalCaps & kFlashEnabled)? true : false;}
	inline uint32 maxTxStreams() const {return ((m_capabilities.generalCaps & kMaxTxStreamsMask) >> kMaxTxStreamsShft);}
	inline uint32 maxRxStreams() const {return ((m_capabilities.generalCaps & kMaxRxStreamsMask) >> kMaxRxStreamsShft);}
	inline uint32 chipType() const {return ((m_capabilities.generalCaps & kChipTypeMask) >> kChipTypeShft);}
	
	// --- stand-alone, persistent settings //
	inline uint32 stdaClkSrc() const {return (m_stdAlone.clkSrc);}
	inline uint32 stdaExtAes() const {return (m_stdAlone.extAES);}
	inline uint32 stdaExtADAT() const {return (m_stdAlone.extADAT);}
	inline uint32 stdaExtWC() const {return (m_stdAlone.extWC);}
	inline uint32 stdaExtInt() const {return (m_stdAlone.extInt);}
	
	inline uint32 appSpaceSize() const {return (m_spaces[kAppSpace].size32);}
	inline uint32 appSpaceItem(uint32 i) const {return (m_appSpace[i]);}
	
	void setStdaClkSrc(unsigned int  clkSrc) throw(tcat::exception::base);
	void setStdaExtAes(unsigned int  extAES) throw(tcat::exception::base);
	void setStdaExtADAT(unsigned int  extADAT) throw(tcat::exception::base);
	void setStdaExtWC(unsigned int  extWC) throw(tcat::exception::base);
	void setStdaExtInt(unsigned int  extInt) throw(tcat::exception::base);
	
	void setAppSpaceItem(unsigned int  i, unsigned int  v) throw(tcat::exception::base);
	
	// --- mixer/peak detector //
	virtual void	setMixerCoeffs (unsigned int inIdx, unsigned int outIdx, uint32 * coeff, unsigned int nbCoeff) throw(tcat::exception::base);
	virtual void	setMixerCoeff (unsigned int inIdx, unsigned int outIdx, uint32 coeff) throw(tcat::exception::base);
	virtual void	getPeak (unsigned int idx, uint32 * pPeaks, unsigned int nbPeaks = 1) throw(tcat::exception::base);
	virtual uint32	getSatBits() throw(tcat::exception::base);


	virtual void	setRouting(EAP_NEW_ROUTER const * pRouter, unsigned modes) throw(tcat::exception::base);
	virtual void	setStrmCfg (EAP_NEW_STRM_CFG const * pStrmCfg, unsigned modes) throw(tcat::exception::base);
	void			storeToFlash (bool bNonBlocking=true) throw(tcat::exception::base);
	void			storeFlashIfDirty (bool bNonBlocking=true) throw(tcat::exception::base);
	
	inline uint32 getCurrentRouteCount (uint32 mode) const {return m_currCfg.modes[mode].router.nbEntries;};
	
	
	
private:
	EAPSPACE			m_spaces[kEapSpaces];
	bool				m_eapSupported;
	EAP_CAPABILITIES	m_capabilities;
	EAP_CURR_CFG		m_currCfg;
	EAP_MIXER			m_mixer;
	EAP_NEW_ROUTER		m_newRouter; //should have critical section
	EAP_STND_ALONE_CFG  m_stdAlone;
	uint32	*			m_appSpace;
	
	
	enum consts
	{
		kEAP_CMD_TIMEOUT = 400
	};
	
		
	void updateCurrCfg() throw(tcat::exception::base);
	void updateMixer() throw(tcat::exception::base);
	void executeOpcode(unsigned int opcode, bool bWaitComplete) throw(tcat::exception::base);
};



typedef dice::reference<eap_device> eap_device_ref;



} // namespace dice
} // namespace tcat

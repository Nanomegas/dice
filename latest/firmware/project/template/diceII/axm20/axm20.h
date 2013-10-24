//===========================================================================
//
//===========================================================================

/****************************************************************************
	
	Purpose:	AXM20.h - Support for AXM20 analog expander board for DICEII EVM
	
	Revisions:
		created 05/21/2005 brian

****************************************************************************/
#ifndef _AXM20_H_
#define	_AXM20_H_

#include "TCTypes.h"

#define	CODEC_UPPER_CH_I2C_ADDR		0x49
#define	CODEC_LOWER_CH_I2C_ADDR		0x4a
#define	CODEC_IF_FMTS_I2C_OFFSET	0x04
#define	CODEC_ADC_CTL_I2C_OFFSET	0x05
#define	CODEC_TX_CTL_I2C_OFFSET		0x06
#define	CODEC_MUTE_I2C_OFFSET		0x07
#define	CODEC_OUTVOL_I2C_OFFSET		0x08
#define	CODEC_INVOL_I2C_OFFSET		0x11
#define	CODEC_STATUSCTL_I2C_OFFSET	0x18
#define	CODEC_STATUSMSK_I2C_OFFSET	0x1a

HRESULT AXM20Initialize(void);
HRESULT AXM20InitGrayIF(uint32 encId);
HRESULT AXM20Update(void);
HRESULT AXM20SetInChVol(int ch, int vol);
HRESULT AXM20SetOutChVol(int ch, int vol);
HRESULT AXM20SetOutChMute(int ch, int mute);
HRESULT AXM20GetInChVol(int ch, int *muted, int *vol);
HRESULT AXM20GetOutChVol(int ch, int *muted, int *vol);

#endif // _AXM20_H_

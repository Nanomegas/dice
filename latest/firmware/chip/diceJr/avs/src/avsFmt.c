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
/****************************************************************************
	
	Purpose:	AVS Format - Receiver and Transmitter.
				Used for setting format handling of the AVS system.
	
	Revisions:
		created 12/09/2003 kbj


****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "avsDefs.h"
#include "avs.h"
#include "avsFmt.h"
#include "avsFmtDefs.h"

static char 	moduleName[] = __FILE__ ;

/********************* AVS format receiver operations *****************/

static uint32 avsFmtRxReg(uint32 plugID, uint32 reg)
{
	switch (plugID)
	{
		case 0:
			return FMT_ARXDO1_BASE+reg;
		case 1:
			return FMT_ARXDO2_BASE+reg;
		case 2:
			return FMT_ARXDO3_BASE+reg;
		case 3:
			return FMT_ARXDO4_BASE+reg;
	}
	return 0;
}

static uint32 avsFmtTxReg(uint32 plugID, uint32 reg)
{
	switch (plugID)
	{
		case 0:
			return FMT_ATXDI1_BASE+reg;
		case 1:
			return FMT_ATXDI2_BASE+reg;
	}
	return 0;
}

// Set receive channel to create block sync output
HRESULT avsFmtRxSetBlockSyncCh(uint32 plugID, uint8 channel)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_RX && channel < AVS_CHANNELS)
	{
		uint32 setup;
		uint32 setupAddr = avsFmtRxReg(plugID, FMT_RXDO_SETUP);
		avsRegRead(setupAddr, &setup);
		// Clear channel for block sync
		setup &= ~RXDO_BLKSYNC_MASK;
		// Set channel for block sync
		setup |= (channel << RXDO_BLKSYNC_SHIFT);
		avsRegWrite(setupAddr, setup);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set receive channel to use as block sync for channel status bit collection
HRESULT avsFmtRxSetBlockSyncCSCh(uint32 plugID, uint8 channel)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_RX && channel < AVS_CHANNELS)
	{
		uint32 setup;
		uint32 setupAddr = avsFmtRxReg(plugID, FMT_RXDO_SETUP);
		avsRegRead(setupAddr, &setup);
		// Clear channel for block sync channel status source
		setup &= ~RXDO_BLKCS_MASK;
		// Set channel for block sync channel status source
		setup |= (channel << RXDO_BLKCS_SHIFT);
		avsRegWrite(setupAddr, setup);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set receive channel to sink channel status bits from
HRESULT avsFmtRxSetChannelStatusCh(uint32 plugID, uint8 channel)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_RX && channel < AVS_CHANNELS)
	{
		uint32 setup;
		uint32 setupAddr = avsFmtRxReg(plugID, FMT_RXDO_SETUP);
		avsRegRead(setupAddr, &setup);
		// Clear channel for sink of channel status bits
		setup &= ~RXDO_CSCH_MASK;
		// Set channel for sink of channel status bits
		setup |= channel;
		avsRegWrite(setupAddr, setup);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set receive channel to take and output user data from
HRESULT avsFmtRxSetUserDataCh(uint32 plugID, uint8 channel)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_RX && channel < AVS_CHANNELS)
	{
		uint32 setup;
		uint32 setupAddr = avsFmtRxReg(plugID, FMT_RXDO_SETUP);
		avsRegRead(setupAddr, &setup);
		// Clear channel  to take and output user data from
		setup &= ~RXDO_UDCH_MASK;
		// Set channel  to take and output user data from
		setup |= (channel << RXDO_UDCH_SHIFT);
		avsRegWrite(setupAddr, setup);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


// Returns the channel label for receiver plug
HRESULT avsFmtRxGetLabel(uint32 plugID, uint8 channel, uint8 *label)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_RX && channel < AVS_CHANNELS)
	{
		uint32 l;
		uint32 shift = (channel%4)*RXDO_LABEL_SHIFT;
		uint32 labelMask = RXDO_LABEL_MASK << shift;
		uint8 offset = (uint8)(channel/4);
		uint32 labelAddr = avsFmtRxReg(plugID, FMT_RXDO_LABEL) + (offset*4);
		avsRegRead(labelAddr, &l);
		*label = (uint8)((l & labelMask) >> shift);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Returns a pointer to the full channel status block for receiver plug
HRESULT avsFmtRxGetChannelStatus(uint32 plugID, uint8 *channel, uint32 **CSSTBlock)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_RX)
	{
		uint32 setup;
		uint32 setupAddr = avsFmtRxReg(plugID, FMT_RXDO_SETUP);
		avsRegRead(setupAddr, &setup);
		// Return the channel to sink channel status from
		*channel = (uint8)(setup & RXDO_CSCH_MASK);
		// Return a pointer to the channel status bits
		*CSSTBlock = (uint32 *)(avsFmtRxReg(plugID, FMT_RXDO_CS_ST) + AVSBASE);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

/********************* AVS format transmitter operations *****************/

// Set label configuration mode for channel x,
// channel = ALL_CH sets all channels to labelMode
HRESULT avsFmtTxSetLabelCfg(uint32 plugID, uint8 channel, uint8 labelMode)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 i, cfg0;
		uint32 cfg0Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG0);
		avsRegRead(cfg0Addr, &cfg0);

		if (channel < AVS_CHANNELS)
		{
			// Set single channels with labelMode
			uint32 shift = TXDI_LABEL_SHIFT*channel;
			// Clear labelMode for channel
			cfg0 &= ~(TXDI_LABEL_MASK << shift);
			// Set labelMode for channel
			cfg0 |= (labelMode&TXDI_LABEL_MASK) << shift;

		}
		else
		{
			// Set all channels with labelMode
			cfg0 = 0;
			for (i = 0; i < AVS_CHANNELS; i++)
			{
				cfg0 |= (labelMode&TXDI_LABEL_MASK) << (i*TXDI_LABEL_SHIFT);
			}
		}
		avsRegWrite(cfg0Addr, cfg0);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set AVS transmitter in Block Sync master mode, Block Sync to input or free running
HRESULT avsFmtTxSetMasterMode(uint32 plugID, BOOL syncToInput)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 cfg1;
		uint32 cfg1Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG1);
		avsRegRead(cfg1Addr, &cfg1);
		// Set master mode
		cfg1 &= ~TXDI_BLOCK_SYNC_SLAVE; 
		cfg1 &= ~TXDI_CS_ON_SLAVE_CH;
		// Set sync to input bit
		cfg1 &= ~TXDI_MASTER_SYNC_INPUT;
		if (syncToInput)
			cfg1 |= TXDI_MASTER_SYNC_INPUT;
		avsRegWrite(cfg1Addr, cfg1);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set AVS transmitter in BLock Sync slave mode, sync to channel selected
HRESULT avsFmtTxSetSlaveMode(uint32 plugID, uint8 channel)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_RX && channel < AVS_CHANNELS)
	{
		uint32 cfg1;
		uint32 cfg1Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG1);
		avsRegRead(cfg1Addr, &cfg1);
		// Set slave mode
		cfg1 |= TXDI_BLOCK_SYNC_SLAVE; 
		// Set collect channel status on block sync channel
		cfg1 |= TXDI_CS_ON_SLAVE_CH; //KBJ???
		// Clear and set block sync channel
		cfg1 &= ~TXDI_BS_SLAVE_CH_SELECT_MASK;
		cfg1 |= channel << TXDI_BS_SLAVE_CH_SELECT_SHIFT;
		avsRegWrite(cfg1Addr, cfg1);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Enable or disable auto CRC for AVS transmitters channel 0-15, 
// channel >= ALL_CH enable/disable for all channels
HRESULT avsFmtTxSetAutoCRC(uint32 plugID, uint8 channel, BOOL enable)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 cfg1;
		uint32 cfg1Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG1);
		avsRegRead(cfg1Addr, &cfg1);

		if (channel < AVS_CHANNELS)
		{
			if (enable)
			{
				// Enable auto CRC for channel
				cfg1 &= ~(0x1 << channel);
			}
			else
			{
				// Disable auto CRC for channel
				cfg1 |= (0x1 << channel);
			}
		}
		else
		{
			if (enable)
			{
				// Enable auto CRC for all channels
				cfg1 &= ~TXDI_AUTO_CH_STATUS_CRC_MASK;
			}
			else
			{
				// Disable auto CRC for all channels
				cfg1 |= TXDI_AUTO_CH_STATUS_CRC_MASK;
			}
		}

		avsRegWrite(cfg1Addr, cfg1);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Enable PAC SB bit for channels, channel >= ALL_CH sets all channels
// When enabled, the SB bit of the PAC bits will indicate start of block
HRESULT avsFmtTxSetSB(uint32 plugID, uint8 channel, BOOL enable)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 cfg3;
		uint32 cfg3Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG3);
		avsRegRead(cfg3Addr, &cfg3);

		if (channel < AVS_CHANNELS)
		{
			if (enable)
			{
				// Enable SB for channel
				cfg3 |= (0x1 << (channel + TXDI_PAC_SB_CH_SHIFT));
			}
			else
			{
				// Disable SB for channel
				cfg3 &= ~(0x1 << (channel + TXDI_PAC_SB_CH_SHIFT));
			}
		}
		else
		{
			if (enable)
			{
				// Enable SB for all channels
				cfg3 |= TXDI_PAC_SB_CH_MASK;
			}
			else
			{
				// Disable SB for all channels
				cfg3 &= ~TXDI_PAC_SB_CH_MASK;
			}
		}

		avsRegWrite(cfg3Addr, cfg3);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Enable PAC SF bit for channels, channel >= ALL_CH sets all channels 
// When enabled, the SF bit ot the PAC bits will be set indicating the second sub-frame of data
HRESULT avsFmtTxSetSF(uint32 plugID, uint8 channel, BOOL enable)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 cfg3;
		uint32 cfg3Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG3);
		avsRegRead(cfg3Addr, &cfg3);

		if (channel < AVS_CHANNELS)
		{
			if (enable)
			{
				// Enable SF for channel
				cfg3 |= (0x1 << channel);
			}
			else
			{
				// Disable SF for channel
				cfg3 &= ~(0x1 << channel);
			}
		}
		else
		{
			if (enable)
			{
				// Enable SF for all channels
				cfg3 |= TXDI_PAC_SF_CH_MASK;
			}
			else
			{
				// Disable SF for all channels
				cfg3 &= ~TXDI_PAC_SF_CH_MASK;
			}
		}

		avsRegWrite(cfg3Addr, cfg3);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Setting the validity mode, channel >= ALL_CH sets all channels to Vmode 
HRESULT avsFmtTxSetValidityMode(uint32 plugID, uint8 channel, uint8 Vmode)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 i, cfg4;
		uint32 cfg4Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG4);
		uint32 Venable = Vmode & 0x1; // BIT0
		uint32 Vbit = (Vmode >> 1) & 0x1; // BIT1

		avsRegRead(cfg4Addr, &cfg4);

		if (channel < AVS_CHANNELS)
		{
			// Clear validity mode for channel
			cfg4 &= ~(0x1 << (channel+TXDI_V_REPLACE_CH_SHIFT));
			// Clear validity bit for channel
			cfg4 &= ~(0x1 << channel);
			// Set validity mode for channel
			cfg4 |= (Venable << (channel+TXDI_V_REPLACE_CH_SHIFT));
			// Set validity bit for channel
			cfg4 |= (Vbit << channel);
		}
		else
		{
			// Set all channels with Vmode
			cfg4 = 0;
			for (i = 0; i < AVS_CHANNELS; i++)
			{
				// Set validity mode for channel
				cfg4 |= (Venable << (i+TXDI_V_REPLACE_CH_SHIFT));
				// Set validity bit for channel
				cfg4 |= (Vbit << i);
			}
		}

		avsRegWrite(cfg4Addr, cfg4);
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set AVS transmitter user status configuration, (master mode only)
// channel >= ALL_CH sets all channels with same userStatus
HRESULT avsFmtTxSetUserStatus(uint32 plugID, uint8 channel, uint8 userStatus)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 cfg, i;
		uint32 cfgAddr;

		if (channel < (AVS_CHANNELS/2))
		{
			cfgAddr = avsFmtTxReg(plugID, FMT_TXDI_CFG5);
		}
		else
		{
			cfgAddr = avsFmtTxReg(plugID, FMT_TXDI_CFG6);
		}
		if (channel < AVS_CHANNELS)
		{
			uint32 shift = (channel%8)*TXDI_USER_BIT_SHIFT;

			avsRegRead(cfgAddr, &cfg);
			// Clear channel user bit selection
			cfg &= ~(TXDI_USER_BIT_MASK << shift);
			// Set channel user bit selection
			cfg |= (TXDI_USER_BIT_MASK & userStatus) << shift;
			avsRegWrite(cfgAddr, cfg);
		}
		else
		{
			cfg = 0;
			for (i = 0; i < AVS_CHANNELS/2; i++)
			{
				// Set channel user bit selection
				cfg |= (TXDI_USER_BIT_MASK & userStatus) << (TXDI_USER_BIT_SHIFT*i);
			}
			cfgAddr = avsFmtTxReg(plugID, FMT_TXDI_CFG5);
			avsRegWrite(cfgAddr, cfg);
			cfgAddr = avsFmtTxReg(plugID, FMT_TXDI_CFG6);
			avsRegWrite(cfgAddr, cfg);
		}

		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set AVS transmitter Label to be used in TXDI_LABEL_MASK mode, 
// channel >= ALL_CH sets all channels with same label
HRESULT avsFmtTxSetLabel(uint32 plugID, uint8 channel, uint8 label)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 i, chCs;
		uint32 chCsAddr = avsFmtTxReg(plugID, FMT_TXDI_CH_CS);

		// Set label for channel
		chCs = (label << TXDI_CH_LABEL_SHIFT) & TXDI_CH_LABEL_MASK;

		if (channel < AVS_CHANNELS)
		{
			// Calculate address offset for channel
			chCsAddr += channel*4;
			avsRegWrite(chCsAddr, chCs);
		}
		else
		{
			for (i = 0; i < AVS_CHANNELS; i++)
			{
				avsRegWrite(chCsAddr, chCs);
				chCsAddr += 4;
			}
		}
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

/************** Tx Channel status configuration **************/

// Set AVS transmitter channel status configuration mode, 
// channel >= ALL_CH sets all channels with same CSMode
HRESULT avsFmtTxSetModeCS(uint32 plugID, uint8 channel, uint8 CSMode)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 i, cfg2;
		uint32 cfg2Addr = avsFmtTxReg(plugID, FMT_TXDI_CFG2);

		avsRegRead(cfg2Addr, &cfg2);

		if (channel < AVS_CHANNELS)
		{
			// Clear channel status configuration
			cfg2 &= ~(TXDI_CS_MASK << (TXDI_CS_SHIFT*channel));
			// Set channel status configuration
			cfg2 |= (CSMode&TXDI_CS_MASK) << (TXDI_CS_SHIFT*channel);
			avsRegWrite(cfg2Addr, cfg2);
		}
		else
		{
			cfg2 = 0;
			for (i = 0; i < AVS_CHANNELS; i++)
			{
				cfg2 |= (CSMode&TXDI_CS_MASK) << (TXDI_CS_SHIFT*i);
				avsRegWrite(cfg2Addr, cfg2);
			}
		}
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set AVS transmitter channel status common bits, used in TXDI_CS_COMMON mode
HRESULT avsFmtTxSetCommonCS(uint32 plugID, uint32 *pChannelStatus)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 i;
		uint32 chStAddr = avsFmtTxReg(plugID, FMT_TXDI_CH_ST);

		for (i = 0; i < TXDI_CS_ST_QUADS; i++)
		{
			avsRegWrite(chStAddr, pChannelStatus[i]);
			chStAddr += 4;
		}

		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set AVS transmitter channel-specific channel status bit, used in TXDI_CS_CHANNEL mode
// channel >= ALL_CH sets all channels with same specificCS
HRESULT avsFmtTxSetSpecificCS(uint32 plugID, uint8 channel, uint32 specificCS)
{
	HRESULT hResult = NO_ERROR;

	if (plugID < NUM_AVS_TX)
	{
		uint32 i;
		uint32 chCsAddr = avsFmtTxReg(plugID, FMT_TXDI_CH_CS);

		if (channel < AVS_CHANNELS)
		{
			chCsAddr += channel*4;
			avsRegWrite(chCsAddr, specificCS);
		}
		else
		{
			for (i = 0; i < AVS_CHANNELS; i++)
			{
				avsRegWrite(chCsAddr, specificCS);
				chCsAddr += 4;
			}
		}
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Common channel status bits used in AES master mode
static uint32 ChannelStatusBits[TXDI_CS_ST_QUADS] =
{
	0x11111111,		// Channel status 0-31
	0x22222222,		// Channel status 32-63
	0x33333333,		// Channel status 64-95
	0x44444444,		// Channel status 96-127
	0x55555555,		// Channel status 128-128
	0x66666666		// Channel status 160-191
};

/********************* AVS format global format operation ***********************/
// Set transmitter or receiver format for plug based on above listed formats
// Calling all needed formatting and avs procedure to set a default format
// channel specifies for receiver or transmitter to use for block sync
HRESULT avsFmtConfig(uint32 plugID, BOOL bTx, uint8 channel, AVS_FORMATS avsFormat)
{
	HRESULT hResult = NO_ERROR;

	if (bTx)
	{
		if (plugID < NUM_AVS_TX)
		{
			switch (avsFormat)
			{
				case AVS_FMT_TRANSPARENT: // Set transparent mode all data from router to out
					avsFmtTxSetLabelCfg(plugID, AVS_CHANNELS, TXDI_LABEL_TRANSPARENT);
					break;
				case AVS_FMT_AES_MASTER_BSYNC:  // IEC60958 mode and set transmitter in master mode
				case AVS_FMT_AES_MASTER:  // IEC60958 mode and set transmitter in master mode
					if (avsFormat ==AVS_FMT_AES_MASTER)
						avsFmtTxSetMasterMode(plugID, FALSE); // Free running block sync
					else
						avsFmtTxSetMasterMode(plugID, TRUE); // Block sync to input from clock controller
					avsFmtTxSetAutoCRC(plugID, AVS_CHANNELS, FALSE); // Disable CRC
					avsFmtTxSetSB(plugID, AVS_CHANNELS, TRUE); // Enable generating SB
					avsFmtTxSetSF(plugID, AVS_CHANNELS, TRUE); // Enable generating SF
					avsFmtTxSetValidityMode(plugID, AVS_CHANNELS, TXDI_V_AUDIO); // Validity = Audio
					avsFmtTxSetUserStatus(plugID, AVS_CHANNELS, TXDI_U_ZERO); // User bit to zero
					avsFmtTxSetCommonCS(plugID, ChannelStatusBits); // Common channel status
					avsFmtTxSetModeCS(plugID, AVS_CHANNELS, TXDI_CS_COMMON); // Select common CS mode
					avsFmtTxSetLabelCfg(plugID, AVS_CHANNELS, TXDI_LABEL_60958_MODE); // AES mode
					break;
				case AVS_FMT_AES_SLAVE:	  // IEC60958 mode and set transmitter in slave mode
					avsFmtTxSetSlaveMode(plugID, channel); // Slave mode sync from channel
					avsFmtTxSetAutoCRC(plugID, AVS_CHANNELS, FALSE); // Disable CRC
					avsFmtTxSetSB(plugID, AVS_CHANNELS, TRUE); // Enable generating SB
					avsFmtTxSetSF(plugID, AVS_CHANNELS, TRUE); // Enable generating SF
					avsFmtTxSetValidityMode(plugID, AVS_CHANNELS, TXDI_V_AUDIO); // Validity = Audio
					avsFmtTxSetUserStatus(plugID, AVS_CHANNELS, TXDI_U_UNTOUCHED); // User bits from router
					avsFmtTxSetModeCS(plugID, AVS_CHANNELS, TXDI_CS_UNTOUCHED); // Channel status bits from router
					avsFmtTxSetLabelCfg(plugID, AVS_CHANNELS, TXDI_LABEL_60958_MODE);
					break;
				case AVS_FMT_DVD_24BITS:  // Multi-bit linear audio 24 bits data of 6 channels
					hResult = E_NOT_IMPLEMENTED;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				case AVS_FMT_LABEL:
					{
						uint8 ch;
						for (ch = 0; ch < AVS_CHANNELS; ch++)
						{
							avsFmtTxSetLabel(plugID, ch, (uint8) 0x40);
						}
						avsFmtTxSetLabelCfg(plugID, AVS_CHANNELS, TXDI_LABEL_MASK_MODE);
					}
					break;
		        default:
					hResult = E_BAD_INPUT_PARAMETERS;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
			}
			return hResult;
		}
	}
	else
	{
		if (plugID < NUM_AVS_RX)
		{
			switch (avsFormat)
			{
				case AVS_FMT_TRANSPARENT: // Set transparent mode all data from router to out
				case AVS_FMT_AES_MASTER:  // IEC60958 mode 
				case AVS_FMT_AES_SLAVE:	  // IEC60958 mode
					avsFmtRxSetBlockSyncCh(plugID, channel);
					avsFmtRxSetBlockSyncCSCh(plugID, channel);
					avsFmtRxSetChannelStatusCh(plugID, channel);
					avsFmtRxSetUserDataCh(plugID, channel);
					break;
				case AVS_FMT_DVD_24BITS:  // Multi-bit linear audio 24 bits data of 6 channels
					hResult = E_NOT_IMPLEMENTED;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				default:
					hResult = E_BAD_INPUT_PARAMETERS;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
			}
			return hResult;
		}
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

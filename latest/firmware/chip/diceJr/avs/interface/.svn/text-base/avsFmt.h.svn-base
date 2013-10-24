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

/** @addtogroup AVS
	@{
*/


/** @defgroup AVSFormat AVS Format
	@{
*/

#ifndef __avsFmt_h
#define __avsFmt_h

/********************* AVS format global format operation *************/

//! The maximum number of channels per plug
#define AVS_CHANNELS			16

//! AVS Formats
typedef enum
{
	AVS_FMT_TRANSPARENT,		//!< Set transparent mode all data from router to out
	AVS_FMT_AES_MASTER,			//!< IEC60958 mode and set transmitter in master mode
	AVS_FMT_AES_MASTER_BSYNC,	//!< IEC60958 mode and set tx in master mode, block sync from clock ctrl.
	AVS_FMT_AES_SLAVE,			//!< IEC60958 mode and set transmitter in slave mode
	AVS_FMT_DVD_24BITS,			//!< Multi-bit linear audio 24 bits data of 6 channels
    AVS_FMT_LABEL,				//!< Using labels defined in memory to set for transmitter
	AVS_NUM_FORMATS
} AVS_FORMATS;


/** Set transmitter or receiver format for plug.
	@sa AVS_FORMATS

	Set transmitter or receiver block sync format. Calls all needed formatting 
	and avs procedures to set the default format channel specified for block sync.
	@param plugID	The ID of the plug (Tx:1-2, Rx:1-4).
	@param bTx		True if a Tx plug.
	@param channel  The channel @sa AVS_CHANNELS
	@param avsFormat The format to use.
	@return NO_ERROR(0) or an @ref ErrorCodes "error code".
*/
HRESULT avsFmtConfig(uint32 plugID, BOOL bTx, uint8 channel, AVS_FORMATS avsFormat);

//! @}
//! @}

/********************* AVS format receiver operations *****************/


/** @addtogroup AVSInt
	@{
*/

/**	@defgroup AVSIntFormat AVS Format Internal
	@{
*/

//! Set receive channel to create block sync output
HRESULT avsFmtRxSetBlockSyncCh(uint32 plugID, uint8 channel);

//! Set receive channel to use as block sync for channel status bit collection
HRESULT avsFmtRxSetBlockSyncCSCh(uint32 plugID, uint8 channel);

//! Set receive channel to sink channel status bits from
HRESULT avsFmtRxSetChannelStatusCh(uint32 plugID, uint8 channel);

//! Set receive channel to take and output user data from
HRESULT avsFmtRxSetUserDataCh(uint32 plugID, uint8 channel);


//! Returns the channel label for receiver plug
HRESULT avsFmtRxGetLabel(uint32 plugID, uint8 channel, uint8 *label);

//! Returns a pointer to the full channel status block for receiver plug
HRESULT avsFmtRxGetChannelStatus(uint32 plugID, uint8 *channel, uint32 **CSSTBlock);

// CSSTBlock structure - 6 quadlets of each 4 channel status bytes 
#define RXDO_CS_ST_MASK			0xff	// Channel status full block receiver x
#define RXDO_CS_ST_SHIFT		8
#define RXDO_CS_ST_QUADS		6		

/********************* AVS format transmitter operations *****************/

//! Set label configuration mode for channel x,
//! channel >= ALL_CH sets all channels to labelMode
HRESULT avsFmtTxSetLabelCfg(uint32 plugID, uint8 channel, uint8 labelMode);

#define TXDI_LABEL_TRANSPARENT  0		// Transparent mode - label byte is allowed through untouched
#define TXDI_LABEL_MASK_MODE	1		// Mask mode - label byte is replaced by constant config. value
#define TXDI_LABEL_60958_MODE	2		// IEC 60958 Conformant mode - label byte shall be 60958 conformant

//! Set AVS transmitter in Block Sync master mode, Block Sync to input or free running
HRESULT avsFmtTxSetMasterMode(uint32 plugID, BOOL syncToInput);

//! Set AVS transmitter in Block Sync slave mode, sync to channel selected
HRESULT avsFmtTxSetSlaveMode(uint32 plugID, uint8 channel);

//! Enable or disable auto CRC for AVS transmitters channel 0-15, 
//! channel >= ALL_CH enable/disable for all channels
HRESULT avsFmtTxSetAutoCRC(uint32 plugID, uint8 channel, BOOL enable);

//! Enable PAC SB bit for channels, channel >= ALL_CH sets all channels. 
//! When enabled, the SB bit of the PAC bits will indicate start of block
HRESULT avsFmtTxSetSB(uint32 plugID, uint8 channel, BOOL enable);

//! Enable PAC SF bit for channels, channel >= ALL_CH sets all channels 
//! When enabled, the SF bit ot the PAC bits will be set indicating the second sub-frame of data
HRESULT avsFmtTxSetSF(uint32 plugID, uint8 channel, BOOL enable);

//! Setting the validity mode, channel = ALL_CH sets all channels to Vmode 
HRESULT avsFmtTxSetValidityMode(uint32 plugID, uint8 channel, uint8 Vmode);

// Vmode values
#define TXDI_V_UNTOUCHED		0x0		// V bit already present in label will be untouched
#define TXDI_V_NON_AUDIO		0x1		// V bit will indicate non audio data (IEC 60958 label)
#define TXDI_V_AUDIO			0x3		// V bit will indicate valid audio data (IEC 60958 label)

//! Set AVS transmitter user status configuration, (master mode only)
//! channel >= ALL_CH sets all channels with same userStatus
HRESULT avsFmtTxSetUserStatus(uint32 plugID, uint8 channel, uint8 userStatus);

// userStatus values
#define TXDI_U_UNTOUCHED		0x00	// Allow User bit already present in label byte from router as is
#define TXDI_U_AVSRX1			0x01	// Take User bit input from AVS RX 1
#define TXDI_U_AVSRX2			0x02	// Take User bit input from AVS RX 2
#define TXDI_U_AVSRX3			0x03	// Take User bit input from AVS RX 3
#define TXDI_U_AVSRX4			0x04	// Take User bit input from AVS RX 4
#define TXDI_U_ZERO				0x05	// Set User bit to be zero
#define TXDI_U_AESRX0			0x80	// Take User bit input from AES RX 0
#define TXDI_U_AESRX1			0x81	// Take User bit input from AES RX 1
#define TXDI_U_AESRX2			0x82	// Take User bit input from AES RX 2
#define TXDI_U_AESRX3			0x83	// Take User bit input from AES RX 3

//! Set AVS transmitter Label to be used in TXDI_LABEL_MASK mode, 
//! channel >= ALL_CH sets all channels with same label
HRESULT avsFmtTxSetLabel(uint32 plugID, uint8 channel, uint8 label);

/************** Tx Channel status configuration **************/

//! Set AVS transmitter channel status configuration mode, 
//! channel >= ALL_CH sets all channels with same CSMode
HRESULT avsFmtTxSetModeCS(uint32 plugID, uint8 channel, uint8 CSMode);

// CSMode values
#define TXDI_CS_UNTOUCHED		0x0		// Channel status bit allowed through from Router untouched
#define TXDI_CS_COMMON			0x2		// Channel status bit taken from common APB Channel Status data
#define TXDI_CS_CHANNEL			0x3		// Channel status bit taken from channel-specific APB Channel Status data

//! Set AVS transmitter channel status common bits, used in TXDI_CS_COMMON mode
HRESULT avsFmtTxSetCommonCS(uint32 plugID, uint32 *pChannelStatus);

//! Length of channel status block, used for length of pChannelStatus block
#define TXDI_CS_ST_QUADS		6		

//! Set AVS transmitter channel-specific channel status bit, used in TXDI_CS_CHANNEL mode
//! channel >= ALL_CH sets all channels with same specificCS
HRESULT avsFmtTxSetSpecificCS(uint32 plugID, uint8 channel, uint32 specificCS);

//! @}
//! @}
#endif

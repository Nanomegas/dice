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
	
	Purpose:	DICE clock controller register definition
	
	Revisions:
		created 12/06/2003 kbj

****************************************************************************/

#ifndef _DICECLOCKREG_H
#define _DICECLOCKREG_H

//*******************************************************************************
// Clock synchronizer 1,2 input selection for 512fs baserate and 1fs baserate
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned sync1_in_sel	: 3;	// Clock syncronizer 1 input selection
		unsigned sync2_in_sel	: 3;	// Clock syncronizer 2 input selection
		unsigned				: 26;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 26;	// Not used
		unsigned sync2_in_sel	: 3;	// Clock syncronizer 2 input selection
		unsigned sync1_in_sel	: 3;	// Clock syncronizer 1 input selection
	} bit;
#endif

} uSYNC_CLK_SEL;

//*******************************************************************************
// Router 1,2 domain clock selection for 256fs and 1fs clocks
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned fs256_router1_sel	: 2;	// Router 1 fs256 input selection 
		unsigned fs256_router2_sel	: 2;	// Router 2 fs256 input selection
		unsigned fs1_router1_sel	: 2;	// Router 1 fs1 input selection
		unsigned fs1_router2_sel	: 2;	// Router 2 fs1 input selection
		unsigned					: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 24;	// Not used
		unsigned fs1_router2_sel	: 2;	// Router 2 fs1 input selection
		unsigned fs1_router1_sel	: 2;	// Router 1 fs1 input selection
		unsigned fs256_router2_sel	: 2;	// Router 2 fs256 input selection
		unsigned fs256_router1_sel	: 2;	// Router 1 fs256 input selection 
	} bit;
#endif

} uROUTER_CLK_SEL;

//*******************************************************************************
// Word Clock and external clock output selects
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned wclk_out			: 1;	// Word clock selection router 1(0) or router 2(1)
		unsigned ext_fbr_out		: 1;	// Extern clock fbr out from syncronizer 1(0) or syncronizer 2(1)
		unsigned ext_f512_out		: 1;	// Extern clock f512br out from syncronizer 1(0) or syncronizer 2(1)
		unsigned					: 29;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 29;	// Not used
		unsigned ext_f512_out		: 1;	// Extern clock f512br out from syncronizer 1(0) or syncronizer 2(1)
		unsigned ext_fbr_out		: 1;	// Extern clock fbr out from syncronizer 1(0) or syncronizer 2(1)
		unsigned wclk_out			: 1;	// Word clock selection router 1(0) or router 2(1)
	} bit;
#endif

} uWC_EC_SEL;

//*******************************************************************************
// AES/EBU Channel status block sync control
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ext_bsync_out_sel	: 3;	// Selects source of the block sync to other devices
		unsigned aes_tx_bsync_sel	: 3;	// Selects source of the block sync to AES tx
		unsigned txdi1_bsync_out_sel: 3;	// Selects source of the block sync to AVS ATX1
		unsigned txdi2_bsync_out_sel: 3;	// Selects source of the block sync to AVS ATX2
		unsigned					: 20;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 20;	// Not used
		unsigned txdi2_bsync_out_sel: 3;	// Selects source of the block sync to AVS ATX2
		unsigned txdi1_bsync_out_sel: 3;	// Selects source of the block sync to AVS ATX1
		unsigned aes_tx_bsync_sel	: 3;	// Selects source of the block sync to AES tx
		unsigned ext_bsync_out_sel	: 3;	// Selects source of the block sync to other devices
	} bit;
#endif

} uBLOCK_SYNC;

//*******************************************************************************
// Hybrid PLL reference event selection
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ref_event1_sel		: 5;	// Hybrid PLL 1 input selection
		unsigned ref_event2_sel		: 5;	// Hybrid PLL 2 input selection
		unsigned					: 22;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 22;	// Not used
		unsigned ref_event2_sel		: 5;	// Hybrid PLL 2 input selection
		unsigned ref_event1_sel		: 5;	// Hybrid PLL 1 input selection
	} bit;
#endif

} uHPLL_CLK_SEL;

//*******************************************************************************
// Sample rate counter and phase detector input selection
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned sr_cnt1_in1_sel	: 5;	// Sample rate counter 1 input 1
		unsigned sr_cnt1_in2_sel	: 5;	// Sample rate counter 1 input 2
		unsigned sr_cnt2_in1_sel	: 5;	// Sample rate counter 2 input 1
		unsigned sr_cnt2_in2_sel	: 5;	// Sample rate counter 2 input 2
		unsigned					: 12;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 12;	// Not used
		unsigned sr_cnt2_in2_sel	: 5;	// Sample rate counter 2 input 2
		unsigned sr_cnt2_in1_sel	: 5;	// Sample rate counter 2 input 1
		unsigned sr_cnt1_in2_sel	: 5;	// Sample rate counter 1 input 2
		unsigned sr_cnt1_in1_sel	: 5;	// Sample rate counter 1 input 1
	} bit;
#endif

} uSR_CNT_SEL;

//*******************************************************************************
// Sample rate counter and phase detector mode selection
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned sr_mode1			: 1;	// Phase Detector(0) or Sample Rate Counter(1)
		unsigned sr_mode2			: 1;	// Phase Detector(0) or Sample Rate Counter(1)
		unsigned					: 30;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 30;	// Not used
		unsigned sr_mode2			: 1;	// Phase Detector(0) or Sample Rate Counter(1)
		unsigned sr_mode1			: 1;	// Phase Detector(0) or Sample Rate Counter(1)
	} bit;
#endif

} uSR_MODE_SEL;
				
//*******************************************************************************
// Receive clock domain selections
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned aes_rx_router_sel		: 1;	// Select clock domain from router to AES
		unsigned tdif_rx_router_sel		: 1;	// Select clock domain from router to TDIF
		unsigned adat_rx_router_sel		: 1;	// Select clock domain from router to ADAT
		unsigned dsai_0_rx_router_sel	: 1;	// Select clock domain from router to DSAI1
		unsigned dsai_1_rx_router_sel	: 1;	// Select clock domain from router to DSAI2
		unsigned dsai_2_rx_router_sel	: 1;	// Select clock domain from router to DSAI3
		unsigned dsai_3_rx_router_sel	: 1;	// Select clock domain from router to DSAI4
		unsigned i2s_0_rx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned i2s_1_rx_router_sel	: 1;	// Select clock domain from router to I2S 2
		unsigned i2s_2_rx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned avs_ado1_router_sel	: 1;	// Select clock domain from router to AVS ADO1
		unsigned avs_ado2_router_sel	: 1;	// Select clock domain from router to AVS ADO2
		unsigned avs_ado3_router_sel	: 1;	// Select clock domain from router to AVS ADO3
		unsigned avs_ado4_router_sel	: 1;	// Select clock domain from router to AVS ADO4
		unsigned apb_audio_router_sel	: 1;	// Select clock domain from router to APB AUDIO
		unsigned						: 17;	// Not used
	} bit;
#else
	struct
	{
		unsigned						: 17;	// Not used
		unsigned apb_audio_router_sel	: 1;	// Select clock domain from router to APB AUDIO
		unsigned avs_ado4_router_sel	: 1;	// Select clock domain from router to AVS ADO4
		unsigned avs_ado3_router_sel	: 1;	// Select clock domain from router to AVS ADO3
		unsigned avs_ado2_router_sel	: 1;	// Select clock domain from router to AVS ADO2
		unsigned avs_ado1_router_sel	: 1;	// Select clock domain from router to AVS ADO1
		unsigned i2s_2_rx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned i2s_1_rx_router_sel	: 1;	// Select clock domain from router to I2S 2
		unsigned i2s_0_rx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned dsai_3_rx_router_sel	: 1;	// Select clock domain from router to DSAI4
		unsigned dsai_2_rx_router_sel	: 1;	// Select clock domain from router to DSAI3
		unsigned dsai_1_rx_router_sel	: 1;	// Select clock domain from router to DSAI2
		unsigned dsai_0_rx_router_sel	: 1;	// Select clock domain from router to DSAI1
		unsigned adat_rx_router_sel		: 1;	// Select clock domain from router to ADAT
		unsigned tdif_rx_router_sel		: 1;	// Select clock domain from router to TDIF
		unsigned aes_rx_router_sel		: 1;	// Select clock domain from router to AES
	} bit;
#endif

} uRX_CLK_DMN_SEL;

//*******************************************************************************
// Transmit clock domain selections
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned aes_tx_router_sel		: 1;	// Select clock domain from router to AES
		unsigned tdif_tx_router_sel		: 1;	// Select clock domain from router to TDIF
		unsigned adat_tx_router_sel		: 1;	// Select clock domain from router to ADAT
		unsigned dsai_0_tx_router_sel	: 1;	// Select clock domain from router to DSAI1
		unsigned dsai_1_tx_router_sel	: 1;	// Select clock domain from router to DSAI2
		unsigned dsai_2_tx_router_sel	: 1;	// Select clock domain from router to DSAI3
		unsigned dsai_3_tx_router_sel	: 1;	// Select clock domain from router to DSAI4
		unsigned i2s_0_tx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned i2s_1_tx_router_sel	: 1;	// Select clock domain from router to I2S 2
		unsigned i2s_2_tx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned avs_adi1_router_sel	: 1;	// Select clock domain from router to AVS ADI1
		unsigned avs_adi2_router_sel	: 1;	// Select clock domain from router to AVS ADI2
		unsigned pwm_router_sel			: 1;	// Select clock domain from router to PWM ROUTER
		unsigned						: 19;	// Not used
	} bit;
#else
	struct
	{
		unsigned						: 19;	// Not used
		unsigned pwm_router_sel			: 1;	// Select clock domain from router to PWM ROUTER
		unsigned avs_adi2_router_sel	: 1;	// Select clock domain from router to AVS ADI2
		unsigned avs_adi1_router_sel	: 1;	// Select clock domain from router to AVS ADI1
		unsigned i2s_2_tx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned i2s_1_tx_router_sel	: 1;	// Select clock domain from router to I2S 2
		unsigned i2s_0_tx_router_sel	: 1;	// Select clock domain from router to I2S 1
		unsigned dsai_3_tx_router_sel	: 1;	// Select clock domain from router to DSAI4
		unsigned dsai_2_tx_router_sel	: 1;	// Select clock domain from router to DSAI3
		unsigned dsai_1_tx_router_sel	: 1;	// Select clock domain from router to DSAI2
		unsigned dsai_0_tx_router_sel	: 1;	// Select clock domain from router to DSAI1
		unsigned adat_tx_router_sel		: 1;	// Select clock domain from router to ADAT
		unsigned tdif_tx_router_sel		: 1;	// Select clock domain from router to TDIF
		unsigned aes_tx_router_sel		: 1;	// Select clock domain from router to AES
	} bit;
#endif

} uTX_CLK_DMN_SEL;

#define CLK_ROUTER_1				1	// Select clock from router 1	
#define CLK_ROUTER_2				0	// Select clock from router 2	

//*******************************************************************************
// Receive clock regeneration select signals
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned regen_vco_ext_clk_sel	: 1;	// From internal VCO(0) or external(0)
		unsigned regen_vco_pwrdn		: 1;	// Enable(0) or disable(1) receiver VCO
		unsigned regen_vco_pre_scaler   : 6;	// Divider for pre scaler
		unsigned regen_vco_main_divider : 8;	// Divider for main
		unsigned regen_vco_post_scaler	: 2;	// Divider for post scaler
		unsigned regen_vco_up_pol		: 1;	// polarity of UP signal to onchip AES VCO
		unsigned regen_vco_down_pol		: 1;	// polarity of DOWN signal to onchip AES VCO
		unsigned						: 12;	// Not used
	} bit;
#else
	struct
	{
		unsigned						: 12;	// Not used
		unsigned regen_vco_down_pol		: 1;	// polarity of DOWN signal to onchip AES VCO
		unsigned regen_vco_up_pol		: 1;	// polarity of UP signal to onchip AES VCO
		unsigned regen_vco_post_scaler	: 2;	// Divider for post scaler
		unsigned regen_vco_main_divider : 8;	// Divider for main
		unsigned regen_vco_pre_scaler   : 6;	// Divider for pre scaler
		unsigned regen_vco_pwrdn		: 1;	// Enable(0) or disable(1) receiver VCO
		unsigned regen_vco_ext_clk_sel	: 1;	// From internal VCO(0) or external(0)
	} bit;
#endif

} uRX_MCK_SEL;

//*******************************************************************************
// ADAT S-Mux selection
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned sync_to_adata_rx		: 1;	// Selects to sync ADAT tx ADAT stream
		unsigned						: 31;	// Not used
	} bit;
#else
	struct
	{
		unsigned						: 31;	// Not used
		unsigned sync_to_adata_rx		: 1;	// Selects to sync ADAT tx ADAT stream
	} bit;
#endif

} uADAT_SMUX_SEL;

//*******************************************************************************
// Dice clock controller register structure
typedef struct
{
	uSYNC_CLK_SEL		syncClkSel;		// Synchronizer 1,2 input selects
	uROUTER_CLK_SEL		routerClkSel;	// Router domain clock selects
	uWC_EC_SEL			wcEcSel;		// Word Clock and External clock output selects
	uBLOCK_SYNC			blockSync;		// AES/EBU Channel status block sync control
	uHPLL_CLK_SEL		hpllClkSel;		// Hybrid PLL reference event selects
	uSR_CNT_SEL			srCntSel;		// Sample rate counter and phase detector input 
	uSR_MODE_SEL		srModeSel;		// Sample rate counter and phase detector mode
	uRX_CLK_DMN_SEL		rxClkDmnSel;	// Receiver clock domain selects
	uTX_CLK_DMN_SEL		txClkDmnSel;	// Transmitter clock domain selects
	uRX_MCK_SEL			aesRxMckSel;	// AES receiver clock regeneration select signal
	uRX_MCK_SEL			adatRxMckSel;	// ADAT receiver clock regeneration select signal
	uRX_MCK_SEL			tdifRxMckSel;	// TDIF receiver clock regeneration select signal
	uADAT_SMUX_SEL		adatSMuxSel;	// ADAT S-Mux Selects
	uint32				prescaler1Sel;	// Prescaler select 1
	uint32				prescaler2Sel;	// Prescaler select 2
	uint32				hybridPllSetup;	// Hybrid PLL Clock (Clock doubler settings //LM???)
	uint32				srCount1;		// SR counter 1 outputs
	uint32				srCount2;		// SR counter 2 outputs
	uint32				srCount1Max;	// SR counter 1 Max
	uint32				srCount2Max;	// SR counter 2 Max
} DICE_CLOCK;

extern DICE_CLOCK *pDiceClock;

#endif

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
		unsigned sync1_in_sel	: 2; // Clock syncronizer 1 input selection
		unsigned reserved	  	: 4; // Clock syncronizer 2 input selection
		unsigned				: 26;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 26;	// Not used
		unsigned reserved		: 4;	
		unsigned sync1_in_sel	: 2;	// Clock syncronizer 1 input selection
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
		unsigned reserved0			: 2;	// Router 1 fs256 input selection 
		unsigned reserved1			: 2;	// Router 2 fs256 input selection
		unsigned fs1_router1_sel	: 2;	// Router 1 fs1 input selection
		unsigned reserved2			: 2;	// Router 2 fs1 input selection
		unsigned					: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 24;	// Not used
		unsigned reserved2			: 2;	// Router 2 fs1 input selection
		unsigned fs1_router1_sel	: 2;	// Router 1 fs1 input selection
		unsigned reserved1			: 2;	// Router 2 fs256 input selection
		unsigned reserved0			: 2;	// Router 1 fs256 input selection 
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
		unsigned extbr_wclk0		: 3;	// The soruce for the external master mode output for base rate
		unsigned wclk1				: 3;	// The source for the word clock output
		unsigned					: 26;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 26;	// Not used
		unsigned wclk1				: 3;	// The source for the word clock output
		unsigned extbr_wclk0		: 3;	// The soruce for the external master mode output for base rate
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
		unsigned reserved			: 5;	// Hybrid PLL 2 input selection
		unsigned					: 22;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 22;	// Not used
		unsigned reserved			: 5;	// Hybrid PLL 2 input selection
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
		unsigned sr_cnt1_in1_sel	: 4;	// Sample rate counter 1 input 1
		unsigned sr_cnt1_in2_sel	: 4;	// Sample rate counter 1 input 2
		unsigned sr_cnt2_in1_sel	: 4;	// Sample rate counter 2 input 1
		unsigned sr_cnt2_in2_sel	: 4;	// Sample rate counter 2 input 2
		unsigned					: 16;	// Not used
	} bit;
#else
	struct
	{
		unsigned					: 16;	// Not used
		unsigned sr_cnt2_in2_sel	: 4;	// Sample rate counter 2 input 2
		unsigned sr_cnt2_in1_sel	: 4;	// Sample rate counter 2 input 1
		unsigned sr_cnt1_in2_sel	: 4;	// Sample rate counter 1 input 2
		unsigned sr_cnt1_in1_sel	: 4;	// Sample rate counter 1 input 1
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
	uint32				reserved0;
	uint32				reserved1;
	uRX_MCK_SEL			aesRxMckSel;	// AES receiver clock regeneration select signal
	uint32				reserved2;	
	uint32				reserved3;	
	uint32				reserved4;								
	uint32				prescaler1Sel;	// Prescaler select 1
	uint32				reserved5;		
	uint32				hybridPllSetup;	// Hybrid PLL Clock (Clock doubler settings //LM???)
	uint32				srCount1;		// SR counter 1 outputs
	uint32				srCount2;		// SR counter 2 outputs
	uint32				srCount1Max;	// SR counter 1 Max
	uint32				srCount2Max;	// SR counter 2 Max
} DICE_CLOCK;

extern DICE_CLOCK *pDiceClock;

#endif

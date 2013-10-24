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
	
	Purpose:	DICE  clock module API
	
	Revisions:
		created 12/06/2003 kbj

****************************************************************************/

#ifndef _DICECLOCK_H
#define _DICECLOCK_H

#include "diceDefs.h"
#include "diceClockReg.h"

// Identification for clock synchronizer 1,2
#define CLK_SYNC_ID1	0
#define CLK_SYNC_ID2	1

// Identification for hybrid PLL 1,2
#define HYBRID_PLL_ID1	0
#define HYBRID_PLL_ID2	1

// Idendification for sample rate counter 1,2
#define SR_CNTR_ID1		0
#define SR_CNTR_ID2		1

// Idendification for prescaler 1,2
#define PRESCALER_ID1	0
#define PRESCALER_ID2	1

// Clear everything 
HRESULT clkInitialize(void);

// Clock routing for synchronizer 1,2
HRESULT clkSetInSync(uint8 clkSyncId, uint8 syncClk);
HRESULT clkGetInSync(uint8 clkSyncId, uint8 *syncClk);

// Values for syncClk
#define SYNC_CLK_EXT_44KHZ		0	// XTal 22.5792 Mhz input 
#define SYNC_CLK_EXT_48KHZ		1	// XTal 24.576 Mhz input
#define SYNC_CLK_EXT_OTHER		2	// External input (from DICE or other src)
#define SYNC_CLK_HYBRID_PLL		3	// Hybrid PLL (1 or 2)
#define SYNC_CLK_AES_RX			4	// AES rx input
#define SYNC_CLK_ADAT_RX		5	// ADAT rx input
#define SYNC_CLK_TDIF_RX		6	// TDIF rx input

// Clock routing for hybrid pll 1,2
HRESULT clkSetInHybridPll(uint8 hpllId, uint8 hpllClk);
HRESULT clkGetInHybridPll(uint8 hpllId, uint8 *hpllClk);

// Values for hpllClk
#define HPLL_CLK_AESRX			0	// 1fs clock from AES receiver
#define HPLL_CLK_ADATRX			1	// 1fs clock from ADAT receiver
#define HPLL_CLK_TDIFRX			2	// 1fs clock from TDIF receiver
#define HPLL_CLK_WORD			3	// Word clock input
#define HPLL_CLK_AVS_SYT1		4	// 1394 syt clock 1
#define HPLL_CLK_AVS_SYT2		5	// 1394 syt clock 2
#define HPLL_CLK_AVS_SYT3		6	// 1394 syt clock 3
#define HPLL_CLK_AVS_SYT4		7	// 1394 syt clock 4
#define HPLL_CLK_PRESCALER		8   // Output from prescaler 1 or 2
#define HPLL_CLK_EXTERNAL		9	// External baserate input
#define HPLL_CLK_DSAI_RX0		10	// DSAI rx0 sync input
#define HPLL_CLK_DSAI_RX1		11	// DSAI rx1 sync input
#define HPLL_CLK_DSAI_RX2		12	// DSAI rx2 sync input
#define HPLL_CLK_DSAI_RX3		13	// DSAI rx3 sync input
#define HPLL_CLK_DSAI_TX0		14	// DSAI tx0 sync input
#define HPLL_CLK_DSAI_TX1		15	// DSAI tx1 sync input
#define HPLL_CLK_DSAI_TX2		16	// DSAI tx2 sync input
#define HPLL_CLK_DSAI_TX3		17	// DSAI tx3 sync input

// Clock routing for router 1,2
HRESULT clkSetInRouter(uint8 routerId, uint8 routerClk);
HRESULT clkGetInRouter(uint8 routerId, uint8 *routerClk);

// Values for routerClk
#define ROUTER_CLK_48K_SAME		0	// *1 Clock from same syncronizer (1->1 and 2->2)								
#define ROUTER_CLK_96K_SAME		1	// *2 Clock from same syncronizer 
#define ROUTER_CLK_48K_OTHER	2	// *1 Clock from other syncronizer (1->2 and 2->1)
#define ROUTER_CLK_96K_OTHER	3	// *2 Clock from ohter syncronizer

// Set clock routing for receive device 
HRESULT clkSetInRxDev(DEV_RX_TYPES rxDevType, uint8 routerId);

// Set clock routing for transmit device 
HRESULT clkSetInTxDev(DEV_TX_TYPES txDevType, uint8 routerId);

// Set clock divider for device only AES, ADAT and TDIF
HRESULT clkSetPostDivider(DEV_RX_TYPES rxDevType, uint8 postDivider);
HRESULT clkSetMainDivider(DEV_RX_TYPES rxDevType, uint8 mainDivider);
HRESULT clkSetPreDivider(DEV_RX_TYPES rxDevType, uint8 preDivider);

// Values for post divider
#define POST_DIV_BY_1					0	// Set division value 1 for post scaler
#define POST_DIV_BY_2					1	// Set division value 2 for post scaler
#define POST_DIV_BY_4					2	// Set division value 4 for post scaler
#define POST_DIV_BY_8					3	// Set division value 8 for post scaler

// Values for main divider range 1 - 248
#define MIN_MAIN_DIV					1
#define MAX_MAIN_DIV					248

// Values for pre divider range 1 - 62
#define MIN_PRE_DIV						1
#define MAX_PRE_DIV						62

// Set polarity of UP/Down signal to onchip VCO
HRESULT clkSetSignalPolarity(DEV_RX_TYPES rxDevType, uint8 mode);

#define SIGNAL_ACTIVE_LOW				0	// Up/Down signal to VCO is active low
#define SIGNAL_ACTIVE_HIGH				1	// Up/Down signal to VCO is active high
#define INVERT_VCO_UP_SIGNAL			0	// Mode for setting invert value of Up signal to VCO
#define NORMAL_VCO_UP_SIGNAL			1	// Mode for setting normal value of Up signal to VCO

// Selects external or internal VCO for rx device only AES, ADAT and TDIF
// Enable internal receiver VCO for rx device only AES, ADAT and TDIF
HRESULT clkSetVCO(DEV_RX_TYPES rxDevType, uint8 enableRx, uint8 external);

// Values for enable
#define ENABLE_RX_VCO				0	// Enable receiver input VCO
#define DISABLE_RX_VCO				1	// Disable receiver input VCO

// Values for external
#define INTERNAL_VCO				0   // Selects internal VCO
#define EXTERNAL_VCO				1   // Selects external VCO

// Setting ADAT S-Muz
// Selects whether to sync ADAT Transmitter to a received ADAT stream
HRESULT clkSetADATSync(BOOL bSynToStream);

// Hybrid PLL clock selection
HRESULT clkSetHybridPLL(uint32 hpllClk);

// External clock input selection from synchronizer 1,2
HRESULT clkSetInExtern(uint8 syncId);
HRESULT clkGetInExtern(uint8 *syncId);

// Word clock input selection from router 1,2
HRESULT clkSetOutWord(uint8 routerId);
HRESULT clkGetOutWord(uint8 *routerId);

// Selects source of the block sync to other devices
HRESULT clkSetExternBlocksync(uint8 bsSrc); 

// Selects source of the block sync to AES tx
HRESULT clkSetAESTxBlocksync(uint8 bsSrc); 

// Selects source of the block sync to AVS ATX1
HRESULT clkSetAVSTxBlocksync(uint32 plugID, uint8 bsSrc); 

// Values for bsSrc
#define BLOCK_SYNC_AESRX			0
#define BLOCK_SYNC_AVSRX1			1
#define BLOCK_SYNC_AVSRX2			2
#define BLOCK_SYNC_AVSRX3			3
#define BLOCK_SYNC_AVSRX4			4
#define BLOCK_SYNC_AESTX			5	// Not used for AEXTx
#define BLOCK_SYNC_EXTERN			5	// Bit values 5,6,7, not for Extern
#define BLOCK_SYNC_AVSTX1			6	// Not used for plugID = 0
#define BLOCK_SYNC_AVSTX2			7	// Not used for plugID = 1

#if 0 //KBJ??? Old version of dice to be removed

// AES Tx blocksync input selection (extern or intern from AES rx)
HRESULT clkSetAESTxBlocksync(uint8 bsSrc); 

// Values for bsSrc
#define AESTX_BLOCKSYNC_EXTERN		0
#define AESTX_BLOCKSYNC_INTERN		1

// Extern blocksync input selection (AES rx or tx)
HRESULT clkSetExternBlocksync(uint8 bsSrc); 

// Values for bsSrc
#define EXTERN_BLOCKSYNC_AESTX		0
#define EXTERN_BLOCKSYNC_AESRX		1

#endif

// Setting prescale devisor for prescaler 1,2
HRESULT clkSetPrescaler(uint8 prescalerId, uint32 devisor);

// Configuration af sample rate counter
HRESULT clkSetSampleRateCntr(uint8 srCntrId, uint8 srCntInput1, uint8 srCntInput2);
HRESULT clkGetSampleRateCntr(uint8 srCntrId, uint8 *srCntInput1, uint8 *srCntInput2);

// Values for srCntInput1 and srCntInput2
#define SR_CNT_AESRX			0	// 1fs clock from AES receiver
#define SR_CNT_ADATRX			1	// 1fs clock from ADAT receiver
#define SR_CNT_TDIFRX			2	// 1fs clock from TDIF receiver
#define SR_CNT_WORD				3	// Word clock input
#define SR_CNT_AVS_SYT1			4	// 1394 syt clock 1
#define SR_CNT_AVS_SYT2			5	// 1394 syt clock 2
#define SR_CNT_AVS_SYT3			6	// 1394 syt clock 3
#define SR_CNT_AVS_SYT4			7	// 1394 syt clock 4
#define SR_CNT_ROUTER_1			8   // Output from router 1
#define SR_CNT_ROUTER_2			9   // Output from router 2
#define SR_CNT_EXTERNAL			10	// External baserate input
#define SR_CNT_DSAI_RX1			11	// DSAI rx0 sync input
#define SR_CNT_DSAI_RX2			12	// DSAI rx1 sync input
#define SR_CNT_DSAI_RX3			13	// DSAI rx2 sync input
#define SR_CNT_DSAI_RX4			14	// DSAI rx3 sync input
#define SR_CNT_DSAI_TX1			15	// DSAI tx0 sync input
#define SR_CNT_DSAI_TX2			16	// DSAI tx1 sync input
#define SR_CNT_DSAI_TX3			17	// DSAI tx2 sync input
#define SR_CNT_DSAI_TX4			18	// DSAI tx3 sync input
#define SR_CNT_AESRX1			19	// 1fs clock from AES1 receiver
#define SR_CNT_AESRX2			20	// 1fs clock from AES2 receiver
#define SR_CNT_AESRX3			21	// 1fs clock from AES3 receiver
#define SR_CNT_AESRX4			22	// 1fs clock from AES4 receiver

// Seting SR Cntr as phase detector or sample rate counter
HRESULT clkSetSampleRateMode(uint8 srCntrId, uint8 srMode);
// Get SR Cntr value and mode
HRESULT clkGetSampleRateValue(uint8 srCntrId, uint8 *srMode, uint32 *srCount);

// Values for srMode
#define SR_PHASE_DETECTOR			0 // Phase detector
#define SR_SAMPLE_RATE_CNT			1 // Sample rate counter

#endif

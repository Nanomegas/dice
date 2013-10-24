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
	
	Purpose:	DICE - Hybrid PLL API register definitions
	
	Revisions:
		created 28/07/2003 kbj

****************************************************************************/

#ifndef _DICEHPLLREG_H
#define _DICEHPLLREG_H

//***************************************************************************
// HPLL register structure
typedef struct
{							//	Addr	Field Reset Description
							//			width state
	uint32	caf_enable;		//	0x00	1	  0		Cleanly unmutes/mutes clock H and frame H 
	uint32	caf_select;		//	0x01	2	  0		Clock and frame select. Controls Caf Mux.
	uint32	coast;			//	0x02	1	  0		Holds the NCO at uts current frequency
	uint32	r03[3];			//  
	uint32	ref_select;		//	0x06	5	  1		Select the timing reference to lock to
	uint32	ref_edges;		//	0x07	2	  0		Select the edges to lock to (pos/neg/both)
	uint32	r08[2];
	uint32	rdiv;			//	0x0a	(16)  0		Controls the reference divider (NL Prescaler)
	uint32	throttle_r;		//	0x0b	2	  0		Engages additional ref division
	uint32	r0c[5];
	uint32	gravity;		//	0x11	1	  0		Output goes to Min frequency when aquisition
	uint32	r12[4];
	uint32	u_treshold;		//	0x16	8	  100	Sets threshold of the "unlocked" sensor
	uint32	r17;
	uint32	bandwidth_f;	//	0x18	4	  4		Loop bandwidth floor
	uint32	bandwidth_c;	//	0x19	4	  8		Loop bandwidth ceiling
	uint32	shape_f;		//	0x1a	2	  2		Loop response to use when locked
	uint32	shape_v;		//	0x1b	2	  3		Loop response to use while varying
	uint32	max_slew_f;		//	0x1c	4	  15	Slew limit to use when locked
	uint32	max_slew_v;		//	0x1d	4	  15	Slew limit to use while varying
	uint32	descent_lin;	//	0x1e	3	  4		Controls the linear part of the bandwidth descent
	uint32	descent_exp;	//	0x1f	3	  4		Controls the exponential part of the bandwidth descent
	uint32	r20[2];						
	uint32	loos_thr;		//	0x22	8	  10	Sets threshold on the "loose" sensor
	uint32	r23[3];						
	uint32	min_period;		//	0x26	8	  58	Lower limit on NCO period. Sets max freq.
	uint32	max_period;		//	0x27	8	  111	Upper limit on NCO period. Sets min freq.
	uint32	r28[4];						
	uint32	ndiv_f;			//	0x2c	(11)  255	Controls NL divider stage F ('frame_b')
	uint32	ndiv_e;			//	0x2d	(12)  0		Controls NL divider stage E ('fbk_event')
	uint32	ndiv_b;			//	0x2e	(7)	  1		Controls NL divider stage B ('burst')
	uint32	bypass_f;		//	0x2f	1	  0		Connects NL divider stage E directly to NCO
	uint32	phase_lag;		//	0x30	(11)  0		Sets 'frame_h' to reference phase offset
	uint32	r31;							
	uint32	fract_res;		//	0x32	2	  1		Sets resolution of justification grid
	uint32	r33;							
	uint32	burst_len;		//	0x34	(6)	  3		Sets 1-number of strobe Js in each burst
	uint32	r35;						
	uint32	gpo_grant;		//	0x36	0	  3		Configures the X3,X2,X1 as general-purpose outputs
	uint32	x1_gpo;			//	0x37	0	  2		If pin X1 is GPO, sets it to Z/H/L
	uint32	x2_gpo;			//	0x38	0	  2		If pin X2 is GPO, sets it to Z/H/L
	uint32	x3_gpo;			//	0x39	0	  2		If pin X3 is GPO, sets it to Z/H/L
	uint32	r3a[2];						
	uint32	x1x2_mode;		//	0x3c	3	  1		Sets mode of operation of pins X1 and X2
	uint32	r3d[20];					
	uint32	sink_e;			//	0x51	1	  0		Drives clock E to its minimum frequency
	uint32	anchor_e;		//	0x52	1	  0		Anchors clock E to a fixed frequency
	uint32	e_anc_val;		//	0x53	(5)	  0		Sets preriod of the clock-E anchor
	uint32	r54[2];						
	uint32	edet_x1;		//	0x56	4	  0		Configures first phase-detection output
	uint32	edet_x2;		//	0x57	4	  0		Configures second phase-detection output
	uint32	ediv_c;			//	0x58	(3)	  1		Controls BE divider stage C (clock DE)
	uint32	ediv_f;			//	0x59	(13)  511	Controls BE divider stage F (frame DE)
	uint32	ediv_s;			//	0x5a	(4)	  3		Controls BE divider stage S (strobe DE)
	uint32	r5b;						
	uint32	invert_cde;		//	0x5c	1	  0		Tailors clock DE for negedge destinations
	uint32	hobble_cde;		//	0x5d	1	  0		Makes clock DE use just posedged clock E
	uint32	r5e[2];						
	uint32	divide_cj;		//	0x60	1	  0		Causes clock J edge spacing to be doubled
	uint32	invert_cj;		//	0x61	1	  0		Tailors clock J for negedge destinations
	uint32  r62[62];					
	uint32	family_id;		//	0xa0	16	  "SB"	Identifies HybridPLL family SB
	uint32	form_id;		//	0xa1	16    "A "  Identify form A
	uint32	revision_id;	//	0xa2	16	  " 0"	Identify revision 1.xx
	uint32	instance_id;	//	0xa3	16	  0xd201 
	
  // Fill in 
  uint32  ra4[10];      // 0xa4-0xad						
  uint32  mtr_select;   // 0xae  5      0 Selects which timing reference to monitor
  uint32  mtr_edges;    // 0xaf  2      0 Selects what edges to monitor (pos/neg/both)    
  uint32  res_ex;       // 0xb0  4      7 Result exponent. Sets scaling of 'mtr_period' etc.
  uint32  punc_mp;      // 0xb1  1      - Punctuates the MTR period results
  uint32  ra4_;         // 0xb2  
  uint32  mtr_period;   // 0xb3  16     The current period of the monitored timing reference
  uint32  greatest_mp;  // 0xb4  16     The greatest MTR period since last cleared
  uint32  greatest_mp_; // 0xb5  16     The greatest MTR period since last cleared - reading from here clears
  uint32  smallest_mp;  // 0xb6  16     The smallest MTR period since last cleared
  uint32  smallest_mp_; // 0xb7  16     The smallest MTR period since last cleared - reading from here clears
  uint32  ra5[8];      // 0xb8-0cbf						
  
  uint32	tick_rate;		//	0xc0	5	  6		Sets amount of pulse strectching in status bits
	uint32	turn_rate;		//	0xc1	1	  0		Sets relative rate of the 'turn' status bit
	uint32	main_status;	//	0xc2	16			Status bits for HPLL see below
	uint32	rc3[57];		//
	uint32	shutdown_m;		//	0xfc	1	  1		For holding M-domain parts in a lower-power state
	uint32	shutdown_i;		//	0xfd	1	  1		For holding I-domain parts in a lower-power state
	uint32	shutdown_e;		//	0xfe	1	  1		For holding E-domain parts in a lower-power state

} DICE_HPLL;

// Values for main_status bits (Status for HPLL)
#define HPLL_ST_FLAWED			BIT0	// High when the numeric loop is coasting or on discontinuities in the reference
#define HPLL_ST_UNLOCKED		BIT1	// High when the numeric loop is not locked
#define HPLL_ST_VARYING			BIT2	// No explanation available
#define HPLL_ST_LOOSE			BIT3	// Related to the 'co_loose_thr' control field
#define HPLL_ST_I_SLIPPING		BIT4	// Phase error shows the I loop to be unlocked
#define HPLL_ST_I_SHAKY			BIT5	// The Block-I detector hits its linearity limit
#define HPLL_ST_E_SLIPPING		BIT6	// High when the external oscillator is not locked
#define HPLL_ST_E_SHAKY			BIT7	// The Block-E detector hits its linearity limit

#define HPLL_ST_PERD_IS_MIN		BIT8	// Related to the 'co_min_period' control field
#define HPLL_ST_PERD_IS_MAX		BIT9	// Related to the 'co_max_period' control field
#define HPLL_ST_SLEW_IS_MAX		BIT10	// Related to the two 'co_max_slew_' control fields

#define HPLL_ST_MP_FLUSHED		BIT12	// No explanation available
#define HPLL_ST_GETHERED		BIT13	// No explanation available
#define HPLL_ST_GMP_OVER		BIT14	// No explanation available
#define HPLL_TURN				BIT15	// Toggles at a rate related to the pulse stretch time


#define HPLL_REF_IN_CLKCTRL		1       //This one is used in normal operation, source is selected by Clock Controller
#define HPLL_REF_IN_AES			2
#define HPLL_REF_IN_ADAT		3
#define HPLL_REF_IN_TDIF		4
#define HPLL_REF_IN_WC			5
#define HPLL_REF_IN_AVS1		6
#define HPLL_REF_IN_AVS2		7
#define HPLL_REF_IN_AVS3		8
#define HPLL_REF_IN_AVS4		9
#define HPLL_REF_IN_PRESCL1		10
#define HPLL_REF_IN_PRESCL2		11
#define HPLL_REF_IN_EXTFBR		12
#define HPLL_REF_IN_DSAIRX0		13
#define HPLL_REF_IN_DSAIRX1		14
#define HPLL_REF_IN_DSAIRX2		15
#define HPLL_REF_IN_DSAIRX3		16
#define HPLL_REF_IN_DSAITX0		17
#define HPLL_REF_IN_DSAITX1		18
#define HPLL_REF_IN_DSAITX2		19
#define HPLL_REF_IN_DSAITX3		20
#define HPLL_REF_IN_CT300		21
#define HPLL_REF_IN_CT24K		22
#define HPLL_REF_IN_HPX3		23


extern DICE_HPLL *pDiceHPLL1;
extern DICE_HPLL *pDiceHPLL2;

#endif

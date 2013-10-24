;Copyright (c) 1999 TC Applied Technologies Ltd.
;-------------------------------------------------------------------
;$Header: /Firewire/1394Dice2/DspTest/dsptest.asm
;===================================================================
; date:         Leon Mortensen 2004-11-10
;---------------------------------------------------------------

  PAGE 132,255
		include 'dsp56300.inc'
		include	'reg56367.inc'	; 367 processor register definitions

;===========================================================

	define DSAI_MASTER	'0'	;DSAI in clock slave mode
;	define DSAI_MASTER	'1' ;DSAI in clock master mode

;	define DSAI_32BIT		'0'	;DSAI frame sync clock 1 bit
	define DSAI_32BIT		'1' ;DSAI frame sync clock 32 bit
													;need to be identical in diceSetupDSAI
	
	define USEI2S				'0' ;Use DSAI protocol
;	define USEI2S				'1' ;Use I2S protocol

;-----------------------------------------------------------------------------
;PORT C (Shared with ESAI)
;-----------------------------------------------------------------------------
  define dHw_PORTC_CTL_INIT      '$00085b'
  define dHw_PORTC_DIR_INIT      '$00085b'
  define dHw_PORTC_DAT_INIT      '$000000'

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;------------------------------------
; PCRC  0000 0000 0000 1000 0101 1011
; PRRC  0000 0000 0000 1000 0101 1011   NET                 DIR      PULL
;----------------------------------------------------------------------------
; P*C0  .... .... .... .... .... ...I   tx0_clk(SCKR),      In,			 NoPull,ESAI
; P*C1  .... .... .... .... .... ..I.   tx0_sync(FSR),      In,			 NoPull,ESAI
; P*C2  .... .... .... .... .... .n..   not used(HCKR),     In,      NoPull,ESAI
; P*C3  .... .... .... .... .... I...   rx0_clk(SCKT),      In,      NoPull,ESAI
; P*C4  .... .... .... .... ...I ....   rx0_sync(FST),      In,      NoPull,ESAI
; P*C5  .... .... .... .... ..n. ....   not used(HCKT),     In,      NoPull,ESAI
; ------------------------------------
; P*C6  .... .... .... .... .I.. ....   tx0_data(SDI0),		  In,		   NoPull,ESAI
; P*C7  .... .... .... .... n... ....   not used(SDI1),     In,			 NoPull,ESAI
; P*C8  .... .... .... ...n .... ....   not used(SDI2),			In,      NoPull,ESAI
; P*C9  .... .... .... ..n. .... ....   not used(SDO2),     Out,     NoPull,ESAI
; P*C10 .... .... .... .n.. .... ....   not used(SDO1),			Out,     NoPull,ESAI
; P*C11 .... .... .... 0... .... ....   rx0_data(SDO0),			Out,     NoPull,ESAI
; rsvd  XXXX XXXX XXXX .... .... ....   
;-----------------------------------------------------------------------------
; Legend:
;   PCRC bits named PCn
;   PRRC bits named PDCn
;   n - No connection PDCn=0, PCn=0
;   i - GPIO in       PDCn=0, PCn=1
;   o - GPIO out      PDCn=1, PCn=0
;   I - ESAI in       PDCn=1, PCn=1     I or O controlled by ESAI config
;   O - ESAI out      PDCn=1, PCn=1     I or O controlled by ESAI config
;-----------------------------------------------------------------------------
  
;#############################################################################
;## ESAI Config values
;#############################################################################

;=Transmit=Clock=Control======================================================
 define dHw_ESAI_TCCR_DSAI_Slave      '$804f01'  ;DSAI Mode, clock slave
 define dHw_ESAI_TCCR_DSAI_Master     '$e04f01'  ;DSAI Mode, clock master   ;(1+7 slots per frame)

 define dHw_ESAI_TCCR_I2S_Slave       '$884301'  ;I2S Mode,  clock slave		;(1+1 slots per frame)
 define dHw_ESAI_TCCR_LJ_Slave				'$804301'  ;Left Justified, clock slave
 define dHw_ESAI_TCCR_RJ_Slave				'$804301'  ;Right Justified, clock slave

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;------------------------------------
; TCCR  1000 0000 0100 1111 0000 0001   
;-----------------------------------------------------------------------------
; TPM   .... .... .... .... 0000 0001 _ 1(div2) => HCKT = fosc/4 (25Mhz)
; TPSR  .... .... .... ...1 .... ....   1(div1) divide by 1
; TDC   .... .... ..00 111. .... .... _ 7(8 slots per frame)
; TFP   .... ..00 01.. .... .... ....   1(div2) => SCKT = HCKT/2 (12.5Mhz)
; TCKP  .... .0.. .... .... .... .... _ Change on rise, Sample on fall
; TFSP  .... 0... .... .... .... ....   FrmSync hi at start
; THCKP ...0 .... .... .... .... .... _ Change on rise, Sample on fall
; TCKD  ..0. .... .... .... .... ....   External clock source
; TFSD  .0.. .... .... .... .... .... _ FrameSync = Input
; THCKD 1... .... .... .... .... ....   HFClk = Output
;-----------------------------------------------------------------------------

;=Receive=Clock=Control======================================================
 define dHw_ESAI_RCCR_DSAI_Slave		 '$804f01'  ;DSAI Mode, clock slave
 define dHw_ESAI_RCCR_DSAI_Master		 '$e04f01'  ;DSAI Mode, clock master

 define dHw_ESAI_RCCR_I2S_Slave			 '$884301'  ;I2S Mode,  clock slave		;(1+1 slots per frame)
 define dHw_ESAI_RCCR_LJ_Slave		   '$804301'  ;Left Justified, clock slave
 define dHw_ESAI_RCCR_RJ_Slave			 '$804301'  ;Right Justified, clock slave

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;------------------------------------
; RCCR  0000 0000 0000 1111 0000 0001   
;-----------------------------------------------------------------------------
; RPM   .... .... .... .... 0000 0001 _ 1(div2) => HCKR = fosc/4 (25Mhz)
; RPSR  .... .... .... ...1 .... ....   1(div1) divide by 1
; RDC   .... .... ..00 111. .... .... _ 7(8 slots per frame)
; RFP   .... ..00 01.. .... .... ....   1(div2) => SCKT = HCKT/2 (12.5Mhz)
; RCKP  .... .0.. .... .... .... .... _ Change on rise, Sample on fall
; RFSP  .... 0... .... .... .... ....   FrmSync hi at start
; RHCKP ...0 .... .... .... .... .... _ Change on rise, Sample on fall
; RCKD  ..0. .... .... .... .... ....   External clock source
; RFSD  .0.. .... .... .... .... .... _ FrameSync = Input
; RHCKD 1... .... .... .... .... ....   HFClk = Output
;-----------------------------------------------------------------------------

;=Transmit=Control============================================================
 define dHw_ESAI_TCR_DSAI_1bit    '$02fD80'  ;DSAI, frame sync 1-bit clock
 define dHw_ESAI_TCR_DSAI_32bit   '$027D80'  ;DSAI, frame sync 32-bit clock

 define dHw_ESAI_TCR_I2S_32bit	  '$037D00'  ;I2S, frame sync 32-bit clock
 define dHw_ESAI_TCR_LJ_Slave			'$027D00'  ;Left Justified, clock slave
 define dHw_ESAI_TCR_RJ_Slave			'$027D80'  ;Right Justified, clock slave

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;------------------------------------
; TCR   0000 0010 1111 1101 1000 0000   
;-----------------------------------------------------------------------------
; TE    .... .... .... .... ..00 0001   Enable Tx0
; TSHFD .... .... .... .... .0.. ....   MSB first,  MUST BE 1 FOR LSB
; TWA   .... .... .... .... 1... ....   Right align
; TMOD  .... .... .... ..01 .... ....   Network mode
; TSWS  .... .... .111 11.. .... ....   Slot=32 bits, Word=24
; TFSL  .... .... 1... .... .... ....   Frame sync is 1-bit clock long
; TFSR  .... ...0 .... .... .... ....   Frame sync same time as data bit
; PADC  .... ..1. .... .... .... ....   Zero padding
; rsvd  .... .X.. .... .... .... ....   
; TPR   .... 0... .... .... .... ....   Normal operation
; TEIE  ...0 .... .... .... .... ....   Transfer exception interrupt = Disable
; TEDIE ..0. .... .... .... .... ....   Transfer even interrupt      = Disable
; TIE   .0.. .... .... .... .... ....   Transfer interrupt           = Disable
; TLIE  0... .... .... .... .... ....   Transfer last slot interrupt = Disable
;-----------------------------------------------------------------------------


;=Receive=Control=============================================================
 define dHw_ESAI_RCR_DSAI_1bit    '$00fD80'  ;DSAI, frame sync 1-bit clock
 define dHw_ESAI_RCR_DSAI_32bit   '$007D80'  ;DSAI, frame sync 32-bit clock

 define dHw_ESAI_RCR_I2S_32bit    '$017D00'  ;I2S, frame sync 32-bit clock
 define dHw_ESAI_RCR_LJ_Slave			'$007D00'  ;Left Justified, clock slave
 define dHw_ESAI_RCR_RJ_Slave			'$007D80'  ;Right Justified, clock slave

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;------------------------------------
; RCR   0000 0000 1111 1101 0000 0000   
;-----------------------------------------------------------------------------
; RE    .... .... .... .... ..XX 0001   Enable Rx0
; RSHFD .... .... .... .... .0.. ....   MSB first
; RWA   .... .... .... .... 1... ....   Right align
; RMOD  .... .... .... ..01 .... ....   Network mode
; RSWS  .... .... .111 11.. .... ....   Slot=32 bits, Word=24
; RFSL  .... .... 1... .... .... ....   Frame sync is 1-bit clock long
; RFSR  .... ...0 .... .... .... ....   Frame sync same time as data bit
; rsvd  .... .XX. .... .... .... ....   
; RPR   .... 0... .... .... .... ....   Normal operation
; REIE  ...0 .... .... .... .... ....   Transfer exception interrupt = Disable
; REDIE ..0. .... .... .... .... ....   Transfer even interrupt      = Disable
; RIE   .0.. .... .... .... .... ....   Transfer interrupt           = Disable
; RLIE  0... .... .... .... .... ....   Transfer last slot interrupt = Disable
;-----------------------------------------------------------------------------

;=Common=ESAI=Control=========================================================
 define dHw_ESAI_SAICR_ASYNC '$000000'   ;Asynchroneous transmitter / receiver

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;------------------------------------
; SAICR 0000 0000 0000 0000 0000 0000   
;-----------------------------------------------------------------------------
; OF*   .... .... .... .... .... .000   Output flags
; rsvd  .... .... .... .... ..XX X...   
; SYN   .... .... .... .... .0.. ....   Mode = Asynchroneous transmit / receive
; TEBE  .... .... .... .... 0... ....   FSR pin = Serial flag 1
; ALC   .... .... .... ...0 .... ....   Data align left to bit 23
; rsvd  XXXX XXXX XXXX XXX. .... ....   
;-----------------------------------------------------------------------------

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;------------------------------------
; BCR   0000 0001 0010 0100 1000 0100
;       .... .... .... .... ...0 0100   BA0W=4  (ASIC   50nSec) CAUTION!!
;       .... .... .... ..00 100. ....   BA1W=4  (ASIC   50nSec)
;       .... .... ...0 01.. .... ....   BA2W=1  (Ext. SRAM 20nSec)
;       .... .... 001. .... .... ....   BA3W=1  (Ext. SRAM 20nSec)
;       ...0 0001 .... .... .... ....   BDFW=1  (20nSec for uninstalled SRAM)
;       ..r. .... .... .... .... ....   BusState read back
;       00.. .... .... .... .... ....   BusReq,BusLock = Normal operation
;-----------------------------------------------------------------------------
 define dHw_BCR_100 '$012484'							;Bus Control Register (100MHz)

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;-----------------------------------------------------------------------------
; AAR*  0000 0000 0100 1010 0001 1001
; BAT   .... .... .... .... .... ..01  Access Type        = SRAM
; BAAP  .... .... .... .... .... .0..  AA pin polarity    = Asserted low
; B*EN  .... .... .... .... ..01 1...  Spaces enabled     = P,X
; BAM   .... .... .... .... .0.. ....  Address muxing     = Disabled
; BPAC  .... .... .... .... 0... ....  DMA Packing logic  = Disabled
; BNC   .... .... .... 1010 .... ....  Significant AdrBits= 10
; BAC  <  0h   0h   4h> .... .... .... Compare address    = 0000 0000 01xx xxxx xxxx xxxx
;-----------------------------------------------------------------------------
 define dHw_AAR0 '$004a19'									;Addresse Attribute Register 0, ASIC RAM P,X, 0x004000 - 0x007FFF (P,X)

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;-----------------------------------------------------------------------------
; AAR*  0000 0000 0100 1010 0010 0101
; BAT   .... .... .... .... .... ..01  Access Type        = SRAM
; BAAP  .... .... .... .... .... .1..  AA pin polarity    = Asserted high
; B*EN  .... .... .... .... ..10 0...  Spaces enabled     = Y
; BAM   .... .... .... .... .0.. ....  Address muxing     = Disabled
; BPAC  .... .... .... .... 0... ....  DMA Packing logic  = Disabled
; BNC   .... .... .... 1010 .... ....  Significant AdrBits= 10
; BAC  <  0h   0h   4h> .... .... .... Compare address    = 0000 0000 01xx xxxx xxxx xxxx
;-----------------------------------------------------------------------------
 define dHw_AAR1 '$4a25'										;Addresse Attribute Register 1, ASIC RAM Y 0x004000 - 0x007FFF (Y)

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;-----------------------------------------------------------------------------
; AAR*  0001 1100 0000 0110 0011 1001
; BAT   .... .... .... .... .... ..01  Access Type        = SRAM
; BAAP  .... .... .... .... .... .0..  AA pin polarity    = Asserted high
; B*EN  .... .... .... .... ..11 1...  Spaces enabled     = X,Y,P
; BAM   .... .... .... .... .0.. ....  Address muxing     = Disabled
; BPAC  .... .... .... .... 0... ....  DMA Packing logic  = Disabled
; BNC   .... .... .... 0110 .... ....  Significant AdrBits= 6
; BAC  <  1h   ch   0h> .... .... .... Compare address    = 0001 11xx xxxx xxxx xxxx xxxx
;-----------------------------------------------------------------------------
 define dHw_AAR2 '$1c0635'									;A18 180000-1bffff=>0,1c0000-1fffff=>1. (A18 controls MSB on SRAM addr.bus), 0x1c0000 - 0x1FFFFF (Y) 

;-----------------------------------------------------------------------------
;          2            1           0  
;Bit No 3210 9876 5432 1098 7654 3210
;-----------------------------------------------------------------------------
; AAR*  0001 1000 0000 0101 0011 0001
; BAT   .... .... .... .... .... ..01  Access Type        = SRAM
; BAAP  .... .... .... .... .... .0..  AA pin polarity    = Asserted low
; B*EN  .... .... .... .... ..11 0...  Spaces enabled     = X,Y
; BAM   .... .... .... .... .0.. ....  Address muxing     = Disabled
; BPAC  .... .... .... .... 0... ....  DMA Packing logic  = Disabled
; BNC   .... .... .... 0101 .... ....  Significant AdrBits= 5
; BAC  <  1h   8h   0h> .... .... .... Compare address    = 0001 1xxx xxxx xxxx xxxx xxxx
;-----------------------------------------------------------------------------
 define dHw_AAR3 '$180531'									;SRAM chip Select ( 512k Word), 0x180000 - 0x1FFFFF (X,Y)

AAR3_128							equ   $180731   ; SRAM chip Select ( 128k Word)
AAR2_128							equ   $190835   ; SRAM Bank Select ( 128k Word),Revers polarity ( 1 block of RAM ) 
AAR3_512							equ   $180531   ; SRAM chip Select ( 512k Word)
AAR2_512							equ   $1c0635   ; SRAM Bank Select ( 512k Word),Revers polarity ( 1 block of RAM ) 

	IF USEI2S==1

nHw_ESAI_TCR_Init			equ   dHw_ESAI_TCR_I2S_32bit
nHw_ESAI_RCR_Init			equ		dHw_ESAI_RCR_I2S_32bit

nHw_ESAI_TCCR_Init		equ		dHw_ESAI_TCCR_I2S_Slave
nHw_ESAI_RCCR_Init		equ		dHw_ESAI_RCCR_I2S_Slave

	ELSE

		IF DSAI_32BIT==1																		; 1 or 32 bits frame sync

nHw_ESAI_TCR_Init			equ   dHw_ESAI_TCR_DSAI_32bit
nHw_ESAI_RCR_Init			equ   dHw_ESAI_RCR_DSAI_32bit

		ELSE

nHw_ESAI_TCR_Init			equ   dHw_ESAI_TCR_DSAI_1bit
nHw_ESAI_RCR_Init			equ   dHw_ESAI_RCR_DSAI_1bit

		ENDIF
 
		IF DSAI_MASTER==1		; Master or slave clock and fram sync

nHw_ESAI_TCCR_Init		equ dHw_ESAI_TCCR_DSAI_Master
nHw_ESAI_RCCR_Init		equ dHw_ESAI_RCCR_DSAI_Master

		ELSE

nHw_ESAI_TCCR_Init		equ dHw_ESAI_TCCR_DSAI_Slave
nHw_ESAI_RCCR_Init		equ dHw_ESAI_RCCR_DSAI_Slave

		ENDIF

	ENDIF

nHw_ESAI_SAICR_Init		equ   dHw_ESAI_SAICR_ASYNC

nHw_PortC_Dir_Init		equ   dHw_PORTC_DIR_INIT
nHw_PortC_Ctl_Init		equ   dHw_PORTC_CTL_INIT
nHw_PortC_Dat_Init		equ   dHw_PORTC_DAT_INIT

;------------------------------------------------------------------------------------------
mDelay macro mpTime_ms
		do #(mpTime_ms*50),_L2
		nop
    do #1000,_L1
		nop
		nop
_L1
    nop
_L2
		endm

;------------------------------------------------------------------------------------------
		org           xli:

gainX							ds		 1			;gainX applied to sample and to be controlled from host

RX1_SAVE					ds		 1
RX2_SAVE					ds		 1
RX3_SAVE					ds		 1
RX4_SAVE					ds		 1
RX5_SAVE					ds		 1
RX6_SAVE					ds		 1
RX7_SAVE					ds		 1

TX1_SAVE					ds		 1
TX2_SAVE					ds		 1
TX3_SAVE					ds		 1
TX4_SAVE					ds		 1
TX5_SAVE					ds		 1
TX6_SAVE					ds		 1
TX7_SAVE					ds		 1

REX1_SAVE					ds		 1
TEX1_SAVE					ds		 1

HDI08_SAVE				ds		 1
HDI08_LATC	      ds     1

TX_CH							ds		 1

ESAI_STATUS				ds		 1

IN_PTR						ds		 1			;Input block pointer
IN_START					ds		 1			;Input block start address

OUT_PTR						ds		 1			;Output block pointer
OUT_START					ds		 1			;Output block start address


;------------------------------------------------------------------------------------------
		org           yli:

gainY							ds		 1			;gainY applied to sample and to be controlled from host


;------------------------------------------------------------------------------------------
		org			xhi:
								  
smpl_block				dsm    8			;input block of sample data
																;slot located as: 0,1,2,3,4,5,6,7

;------------------------------------------------------------------------------------------
; exception table:

		org     p:$0000																	; reset vector
		jmp     F_START																	

    org     p:$0030																	; ESAI receive data
    jsr     ESAI_Rx_Data														
    org     p:$0034																	; ESAI receive exception
    jsr     ESAI_Rx_Exception												
    org     p:$0038																	; ESAI transmit data
    jsr     ESAI_Tx_Data														
    org     p:$003c																	; ESAI transmit exception
    jsr     ESAI_Tx_Exception   

		org     p:$0064																	; HDI08: adr latch
		movep   X:M_HRX,x:HDI08_LATC												
		org     p:$0066																	; HDI08: data read x mem
		jsr     HDI08_Read_X																	
		org     p:$0068																	; HDI08: data read y mem
		jsr     HDI08_Read_Y																	
		org     p:$006A																	; HDI08: data read p mem
		jsr     HDI08_Read_P																	
		org     p:$006C																	; HDI08: data write x mem
		jsr     HDI08_Write_X																	
		org     p:$006E																	; HDI08: data write y mem
		jsr     HDI08_Write_Y																	
		org     p:$0070																	; HDI08: data write p mem
		jsr     HDI08_Write_P																	
																										
;------------------------------------------------------------------------------------------
    org     p:$100																	; main program.
F_START:																						
    ori     #$03,MR																	; mask interupts.
    clr     a   #0,R0																
    rep     #256																		
    move    a,l:(R0)+																
																										
DSPRESET																						
;    btst		#12,x:M_TCSR2														
;    bcs			dsp100mhz																
    movep		#$050000,x:M_PCTL												; Set PLL on 56362 to 100,MHz @50MHz
    movep		#$002483,x:M_BCR												; Bus Control Register
;	jmp			<dspClockDone
;dsp100mhz        
;	movep		#$f781c6,x:M_PCTL
;
;    movep		#dHw_BCR_100,x:M_BCR										; Bus Control Register (100MHz)
dspClockDone        
	movep   #>dHw_AAR0,x:M_AAR0											; Addresse Attribute Register 0, ASIC RAM P,X
	movep   #>dHw_AAR1,x:M_AAR1											; Addresse Attribute Register 1, ASIC RAM Y
	movep   #>dHw_AAR2,x:M_AAR2											; Addresse Attribute Register 2, A18 180000-1bffff=>0,1c0000-1fffff=>1. (A18 controls MSB on SRAM addr.bus)
	movep   #>dHw_AAR3,x:M_AAR3											; SRAM chip Select ( 512k Word)
    movep   #$000033,x:M_IPRP												; host ESAI IPL=level 2, HDI08 IPL=level 2.
    movep   #$1048,x:M_HPCR													; host port - all gp pins
    movep   #$0,x:M_PCRC														
    movep   #$000007,x:M_PRRC												
    movep   #$0,x:M_PCRD														; just to be safe
    movep   #$0,x:M_PCRE														; just to be safe
    movep   #$000004,x:M_HCR												; enable host com intr.
		bset    #M_HF3,x:M_HCR													; set HF3 flag - for host proc. use.
    andi    #$FC,MR																	; enable interupts.
    bset    #M_TAS,OMR															; set TA-bit(11) in OMR-register for external bus sync disable 
    bset    #14,OMR																	; SET APD (Address priority disable).

		jmp			DSAI_INIT

;------------------------------------------------------------------------------------------

DSAI_INIT:

    movep   #>AAR3_128,x:AAR3												;set chipselect to support 128k RAM
    movep   #>AAR2_128,x:AAR2												;set bankselect to support 128k RAM
    movep   #>$200339,x:AAR1												;FLASH chip Select ( 512k / 2 Byte) Start addresss (0x2c0000-0x2fffff)
    movep   #>$30043d,x:AAR0												;FLASH chip A18 (512k / 2 Byte) invertet, starts midt Flash program (0x300000-0x33ffff)
																										
    movep   #>$0f256b,x:BCR													;Cs0(A18)=11ws,CS1=11ws,CS2=1WS,CS3=1WS,

		movep		#>$0,x:PDRC																;set PDC5 to 0 (reset)			
		movep		#>$0,x:PCRD																;set portD GPIO
		movep		#>$3,x:PRRD																;ADO=output , ACI=output
		bclr		#5,x:PDRC																;set PDC5 to 0 (reset)
		movep		#$000220,x:TCSR0													
		movep		#$000000,x:TLR0														
		movep		#$000000,x:TPLR														
		movep		#$000006,x:TCPR0													
		bset		#0,x:TCSR0																
		mDelay	20																			
		bset		#5,x:PDRC																;set PDC5 to 1 (unreset)
		mDelay	60

		jsr			ESAI_init																;initialization of esai
																										;initialization of sample buffers
		move		#smpl_block,r4													;Same buffer for input and output slots
		move		r4,x:IN_PTR															;Store sample buffer pointer
		move		r4,x:IN_START														;Store sample buffer start
		move		r4,a																		;Adjust output buffer offset
		add			#1,a																		
		move		a,x:OUT_PTR															;Store sample buffer pointer
		move		a,x:OUT_START														;Store sample buffer start
		move		#<7,m4																	;8 channels
		clr			a
		do			#8,ClearDataBlock
		move		a,x:(r4)+																;Clear input data buffer
ClearDataBlock																			;used for flash load.
											
		bclr		#1,x:PDRD																;Set led off
		
		move		#$7FFFFF,x0															;Initialize gainX and gainY
		move		x0,x:gainX
		move		x0,y:gainY

;------------------------------------------------------------------------------------------
MAIN
		nop
		nop
		nop
		jmp			MAIN 

;------------------------------------------------------------------------------------------
ESAI_init
		clr			a
		move		a,x:TX_CH																;Clear TX channel number
																			
    movep		#0,x:PRRC																	;ESAI Individual reset
    movep		#0,x:PCRC										

    movep		#nHw_ESAI_TCCR_Init,x:TCCR
    movep		#nHw_ESAI_TCR_Init,x:TCR
    movep		#nHw_ESAI_RCCR_Init,x:RCCR
    movep		#nHw_ESAI_RCR_Init,x:RCR
    movep		#nHw_ESAI_SAICR_Init,x:SAICR

    movep		#nHw_PortC_Ctl_Init,x:PCRC								;Select active pins
    movep		#nHw_PortC_Dir_Init,x:PRRC

    bset		#REIE,x:RCR																;ESAI enable receive interrupt/exception
    bset		#RIE,x:RCR
    bset		#TEIE,x:TCR																;ESAI enable transmitter interrupt/exception
    bset		#TIE,x:TCR

    bset		#ESL+0,x:IPRP
    bset		#ESL+1,x:IPRP

		movep		#1,x:TX0																	;ESAI tx and rx enable
		bset		#RE0,x:RCR
		bset		#TE0,x:TCR
;		bset		#RE1,x:RCR
;		bset		#TE1,x:TCR
		rts

;------------------------------------------------------------------------------------------
ESAI_Tx_Data
		move    a0,x:TX1_SAVE                           ;save a0 reg.
		move    a1,x:TX2_SAVE                           ;save a1 reg.
		move    x1,x:TX3_SAVE                           ;save x1 reg.
		move    r4,x:TX4_SAVE                           ;save r4 reg.
		move    m4,x:TX5_SAVE                           ;save m4 reg.
		move    y0,x:TX6_SAVE                           ;save y0 reg.
		move    y1,x:TX7_SAVE                           ;save y1 reg.

		move		#<7,m4																	;Set modulus 8
		move		x:OUT_PTR,r4														;Set data block pointer
		movep		x:SAISR,x1																;read status
		jclr		#TFS,x1,ESAI_Tx_Sampl_Out  							;continue if not start of frame sync
		move		x:OUT_START,r4													;point to start of data block
ESAI_Tx_Sampl_Out
		move		x:(r4)+,x1
		move		y:gainY,y1
		mpy			x1,y1,a																	;multiply sample with gainY
;		move		a,x1																		
;		move		x:gainX,y1															
;		mpy			x1,y1,a																	;multiply sample with gainX
		movep		a,x:TX0																	;write to ESAI0
		move		r4,x:OUT_PTR														;store out pointer

		move    x:TX7_SAVE,y1														;restore y1 reg.
		move    x:TX6_SAVE,y0														;restore y0 reg.
    move    x:TX5_SAVE,m4														;restore m4 reg.
    move    x:TX4_SAVE,r4														;restore r4 reg.
    move    x:TX3_SAVE,x1														;restore x1 reg.
    move    x:TX2_SAVE,a1														;restore a1 reg.
    move    x:TX1_SAVE,a0														;restore a0 reg.
		rti																							

;------------------------------------------------------------------------------------------
ESAI_Rx_Data
		move    a0,x:RX1_SAVE                           ;save a0 reg.
		move    a1,x:RX2_SAVE                           ;save a1 reg.
		move    x1,x:RX3_SAVE                           ;save x1 reg.
		move    r4,x:RX4_SAVE                           ;save r4 reg.
		move    m4,x:RX5_SAVE                           ;save m4 reg.
		move    b0,x:RX6_SAVE                           ;save b0 reg.
		move    b1,x:RX7_SAVE                           ;save b1 reg.

		move		#<7,m4																	;Set modulus 8
		move		x:IN_PTR,r4															;Set data block pointer
    movep   x:SAISR,x1																;read status
		jclr		#RFS,x1,ESAI_Rx_Sampl_In  							;continue if not start of frame sync
		move		x:IN_START,r4														;point to start of data block
ESAI_Rx_Sampl_In
		movep		x:RX0,a																	;read from ESAI0
		move		a,x:(r4)+																;store in buffer and increment
		move		r4,x:IN_PTR															;store in pointer

    move    x:RX7_SAVE,b1                           ;restore b1 reg.
    move    x:RX6_SAVE,b0                           ;restore b0 reg.
    move    x:RX5_SAVE,m4                           ;restore m4 reg.
    move    x:RX4_SAVE,r4                           ;restore r4 reg.
    move    x:RX3_SAVE,x1                           ;restore x1 reg.
    move    x:RX2_SAVE,a1                           ;restore a1 reg.
    move    x:RX1_SAVE,a0                           ;restore a0 reg.
		rti

;------------------------------------------------------------------------------------------
ESAI_Rx_Exception
		move    x1,x:REX1_SAVE													;save x1 reg.
		movep   x:SAISR,x1																;read status
		move		x1,x:ESAI_STATUS												;save status
		movep		x:RX0,x1																;read sample	
		move    x:REX1_SAVE,x1													;restore x1 reg.
		rti

;------------------------------------------------------------------------------------------
ESAI_Tx_Exception
		move    x1,x:TEX1_SAVE													;save x1 reg.
		movep   x:SAISR,x1																;read status
		move		x1,x:ESAI_STATUS												;save status
		movep		x1,x:TX0																;write to ESAI0
		move    x:TEX1_SAVE,x1													;restore x1 reg.
		rti

;------------------------------------------------------------------------------------------

HDI08_Read_X  
		move    r0,x:HDI08_SAVE													;save r0 reg
    move    x:HDI08_LATC,r0													;get adr
    movep   x:(r0),x:M_HTX													;get x data
    move    x:HDI08_SAVE,r0													;restore r0 reg
    rti																							
																										
HDI08_Read_Y																				
		move    r0,x:HDI08_SAVE													;save r0 reg
    move    x:HDI08_LATC,r0													;get adr
    movep   y:(r0),x:M_HTX													;get y data
    move    x:HDI08_SAVE,r0													;restore r0 reg
    rti																							
																										
HDI08_Read_P																				
		move    r0,x:HDI08_SAVE													;save r0 reg
    move    x:HDI08_LATC,r0													;get adr
    movep   p:(r0),x:M_HTX													;get p data
    move    x:HDI08_SAVE,r0													;restore r0 reg
    rti																							
																										
HDI08_Write_X																				
    move    r0,x:HDI08_SAVE													;save r0 reg
    move    x:HDI08_LATC,r0													;get adr
    movep   x:M_HRX,x:(r0)													;put x data
    move    x:HDI08_SAVE,r0													;restore r0 reg
    rti																							
																										
HDI08_Write_Y																				
    move    r0,x:HDI08_SAVE													;save r0 reg
    move    x:HDI08_LATC,r0													;get adr
    movep   x:M_HRX,y:(r0)													;put y data
    move    x:HDI08_SAVE,r0													;restore r0 reg
    rti																							
																										
HDI08_Write_P																				
    move    r0,x:HDI08_SAVE													;save r0 reg
    move    x:HDI08_LATC,r0													;get adr
    movep   x:M_HRX,p:(r0)													;put p data
    move    x:HDI08_SAVE,r0													;restore r0 reg
    rti

		end
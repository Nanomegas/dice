; Assembler include file for Tasking DSP56xxx tools
;
; Generated from: reg56362.h    1.4 98/07/06
; By:             sfr_h2asm.perl        1.10 98/07/03
;
; Copyright (c) 1995-98 TASKING, Inc.

; Modified 01-01-15 by LAC to include 56366 registers (56362 regs still functional)
; Name Modified by 01-10-27 to Reg56367.inc


        IF      !@DEF('SFR_DEFINED')
        define  SFR_DEFINED     '1'
        define  REG56367_INC_TCVER '3'
        define  CHGMSG ';'              ;'MSG'    ;Use ';' to disable 'MSG' to enable


; Masks for  PLL Control Register

MF              equ     0               ;  0: Multiplication Factor
MF_MASK         equ     $000fff
DF              equ     12              ; 12: Division Factor
DF_MASK         equ     $007000
XTLR            equ     15              ; 15: XTAL Range
XTLR_MASK       equ     $008000
XTLD            equ     16              ; 16: XTAL Disable
XTLD_MASK       equ     $010000
PSTP            equ     17              ; 17: STOP processing State
PSTP_MASK       equ     $020000
PEN             equ     18              ; 18: PLL Enable
PEN_MASK        equ     $040000
CODI            equ     19              ; 19: Clock Output Disable
CODI_MASK       equ     $080000
PPD             equ     20              ; 20: PLL Predivider
PPD_MASK        equ     $f00000

PCTL            equ     x:$FFFFFD               ;  PLL Control Register
OGDBR           equ     x:$FFFFFC               ;  OnCE GDB Register

; Masks for  Port A Bus Control Register

BA0W            equ     0               ;  0: Area 0 Waitstates
BA0W_MASK       equ     $00001f
BA1W            equ     5               ;  5: Area 1 Waitstates
BA1W_MASK       equ     $0003e0
BA2W            equ     10              ; 10: Area 2 Waitstates
BA2W_MASK       equ     $001c00
BA3W            equ     13              ; 13: Area 3 Waitstates
BA3W_MASK       equ     $00e000
BDFW            equ     16              ; 16: Default Area Waitstates
BDFW_MASK       equ     $1f0000
BBS             equ     21              ; 21: Bus State
BBS_MASK        equ     $200000
BLH             equ     22              ; 22: Bus Lock Hold
BLH_MASK        equ     $400000
BRH             equ     23              ; 23: Bus Request Hold
BRH_MASK        equ     $800000

BCR             equ     x:$FFFFFB               ;  Port A Bus Control Register
DCR             equ     x:$FFFFFA               ;  Port A Dram Control Register
AAR0            equ     x:$FFFFF9               ;  Port A Address Attribute Register 0
AAR1            equ     x:$FFFFF8               ;  Port A Address Attribute Register 1
AAR2            equ     x:$FFFFF7               ;  Port A Address Attribute Register 2
AAR3            equ     x:$FFFFF6               ;  Port A Address Attribute Register 3
IDR             equ     x:$FFFFF5               ;  Port A ID Register

; Masks for  DMA Status Register

DTD0            equ     0               ;  0: DMA Channel 0 Transfer Done Status
DTD0_MASK       equ     $000001
DTD1            equ     1               ;  1: DMA Channel 1 Transfer Done Status
DTD1_MASK       equ     $000002
DTD2            equ     2               ;  2: DMA Channel 2 Transfer Done Status
DTD2_MASK       equ     $000004
DTD3            equ     3               ;  3: DMA Channel 3 Transfer Done Status
DTD3_MASK       equ     $000008
DTD4            equ     4               ;  4: DMA Channel 4 Transfer Done Status
DTD4_MASK       equ     $000010
DTD5            equ     5               ;  5: DMA Channel 5 Transfer Done Status
DTD5_MASK       equ     $000020
                                        ;  6: Reserved
DACT            equ     8               ;  8: DMA Active
DACT_MASK       equ     $000100
DCH             equ     9               ;  9: DMA Active Channel
DCH_MASK        equ     $000e00
                                        ; 12: Reserved

; Masks for  DMA Control Register

DSS             equ     0               ;  0: DMA Source Space
DSS_MASK        equ     $000003
DDS             equ     2               ;  2: DMA Destination Space
DDS_MASK        equ     $00000c
DAM             equ     4               ;  4: DMA Address Mode
DAM_MASK        equ     $0003f0
D3D             equ     10              ; 10: DMA Three Dimensional Mode
D3D_MASK        equ     $000400
DRS             equ     11              ; 11: DMA Request Source
DRS_MASK        equ     $00f800
DCON            equ     16              ; 16: DMA Continuous Mode
DCON_MASK       equ     $010000
DPR             equ     17              ; 17: DMA Channel Priority
DPR_MASK        equ     $060000
DTM             equ     19              ; 19: DMA Transfer Mode
DTM_MASK        equ     $380000
DIE             equ     22              ; 22: DMA Interrupt Enable
DIE_MASK        equ     $400000
DE              equ     23              ; 23: DMA Enable
DE_MASK         equ     $800000

DSTR            equ     x:$FFFFF4               ;  DMA Status Register
DOR0            equ     x:$FFFFF3               ;  DMA Offset Register 0
DOR1            equ     x:$FFFFF2               ;  DMA Offset Register 1
DOR2            equ     x:$FFFFF1               ;  DMA Offset Register 2
DOR3            equ     x:$FFFFF0               ;  DMA Offset Register 3
DSR0            equ     x:$FFFFEF               ;  DMA Source Address Register 0
DDR0            equ     x:$FFFFEE               ;  DMA Destination Address Register 0
DCO0            equ     x:$FFFFED               ;  DMA Counter 0
DCR0            equ     x:$FFFFEC               ;  DMA Control Register 0
DSR1            equ     x:$FFFFEB               ;  DMA Source Address Register 1
DDR1            equ     x:$FFFFEA               ;  DMA Destination Address Register 1
DCO1            equ     x:$FFFFE9               ;  DMA Counter 1
DCR1            equ     x:$FFFFE8               ;  DMA Control Register 1
DSR2            equ     x:$FFFFE7               ;  DMA Source Address Register 2
DDR2            equ     x:$FFFFE6               ;  DMA Destination Address Register 2
DCO2            equ     x:$FFFFE5               ;  DMA Counter 2
DCR2            equ     x:$FFFFE4               ;  DMA Control Register 2
DSR3            equ     x:$FFFFE3               ;  DMA Source Address Register 3
DDR3            equ     x:$FFFFE2               ;  DMA Destination Address Register 3
DCO3            equ     x:$FFFFE1               ;  DMA Counter 3
DCR3            equ     x:$FFFFE0               ;  DMA Control Register 3
DSR4            equ     x:$FFFFDF               ;  DMA Source Address Register 4
DDR4            equ     x:$FFFFDE               ;  DMA Destination Address Register 4
DCO4            equ     x:$FFFFDD               ;  DMA Counter 4
DCR4            equ     x:$FFFFDC               ;  DMA Control Register 4
DSR5            equ     x:$FFFFDB               ;  DMA Source Address Register 5
DDR5            equ     x:$FFFFDA               ;  DMA Destination Address Register 5
DCO5            equ     x:$FFFFD9               ;  DMA Counter 5
DCR5            equ     x:$FFFFD8               ;  DMA Control Register 5

; Masks for  Host Control Register

HRIE            equ     0               ;  0: Host Receive Interrupt Enable
HRIE_MASK       equ     $000001
HTIE            equ     1               ;  1: Host Transmit Interrupt Enable
HTIE_MASK       equ     $000002
HCIE            equ     2               ;  2: Host Command Interrupt Enable
HCIE_MASK       equ     $000004
HF2             equ     3               ;  3: Host Flag 2
HF2_MASK        equ     $000008
HF3             equ     4               ;  4: Host Flag 3
HF3_MASK        equ     $000010
HDM             equ     5               ;  5: Host DMA Mode
HDM_MASK        equ     $0000e0
                                        ;  8: Reserved

; Masks for  Host Status Register

HRDF            equ     0               ;  0: Host Receive Data Full
HRDF_MASK       equ     $000001
HTDE            equ     1               ;  1: Host Transmit Data Empty
HTDE_MASK       equ     $000002
HCP             equ     2               ;  2: Host Command Pending
HCP_MASK        equ     $000004
HF0             equ     3               ;  3: Host Flag 0
HF0_MASK        equ     $000008
HF1             equ     4               ;  4: Host Flag 1
HF1_MASK        equ     $000010
                                        ;  5: Reserved
DMA             equ     7               ;  7: Host DMA Status
DMA_MASK        equ     $000080
                                        ;  8: Reserved

; Masks for  Host Control Register

HGEN            equ     0               ;  0: Host GPIO Port Enable
HGEN_MASK       equ     $000001
HA8EN           equ     1               ;  1: Host Address Line 8 Enable
HA8EN_MASK      equ     $000002
HA9EN           equ     2               ;  2: Host Address Line 9 Enable
HA9EN_MASK      equ     $000004
HCSEN           equ     3               ;  3: Host Chip Select Enable
HCSEN_MASK      equ     $000008
HREN            equ     4               ;  4: Host Request Enable
HREN_MASK       equ     $000010
HAEN            equ     5               ;  5: Host Acknowledge Enable
HAEN_MASK       equ     $000020
HEN             equ     6               ;  6: Host Enable
HEN_MASK        equ     $000040
                                        ;  7: Reserved
HROD            equ     8               ;  8: Host Request Open Drain
HROD_MASK       equ     $000100
HDSP            equ     9               ;  9: Host Data Strobe Polarity
HDSP_MASK       equ     $000200
HASP            equ     10              ; 10: Host Address Strobe Polarity
HASP_MASK       equ     $000400
HMUX            equ     11              ; 11: Host Multiplexed Bus
HMUX_MASK       equ     $000800
HDDS            equ     12              ; 12: Host Dual Data Strobe
HDDS_MASK       equ     $001000
HCSP            equ     13              ; 13: Host Chip Select Polarity
HCSP_MASK       equ     $002000
HRP             equ     14              ; 14: Host Request Polarity
HRP_MASK        equ     $004000
HAP             equ     15              ; 15: Host Acknowledge Polarity
HAP_MASK        equ     $008000

HCR             equ     x:$FFFFC2               ;  Host Control Register
HSR             equ     x:$FFFFC3               ;  Host Status Register
HRX             equ     x:$FFFFC6               ;  Host Data Receive Reg.
HTX             equ     x:$FFFFC7               ;  Host Data Transmit Reg.
HBAR            equ     x:$FFFFC5               ;  Host Base Address Register
HPCR            equ     x:$FFFFC4               ;  Host Port Control Register
HDDR            equ     x:$FFFFC8               ;  Host GPIO Data Direction Register
HDR             equ     x:$FFFFC9               ;  Host GPIO Data Register

; Masks for  SHI Clock Control Register

CPHA            equ     0               ;  0: SHI Clock Phase
CPHA_MASK       equ     $000001
CPOL            equ     1               ;  1: SHI Clock Polarity
CPOL_MASK       equ     $000002
HRS             equ     2               ;  2: SHI Clock Prescaler Rate Select
HRS_MASK        equ     $000004
SHDM            equ     3               ;  3: SHI Clock Divider Modulus Select
SHDM_MASK       equ     $0007f8
                                        ; 11: Reserved
HFM             equ     12              ; 12: SHI Clock Filter Mode
HFM_MASK        equ     $003000
                                        ; 14: Reserved

; Masks for  SHI Control/Status Register

SHEN            equ     0               ;  0: Host Enable
SHEN_MASK       equ     $000001
SHIIC           equ     1               ;  1: I2C/SPI Selection
SHIIC_MASK      equ     $000002
SHM             equ     2               ;  2: Serial Host Interface Mode
SHM_MASK        equ     $00000c
SHCKFR          equ     4               ;  4: I2C Clock Freeze
SHCKFR_MASK     equ     $000010
SHFIFO          equ     5               ;  5: FIFO-Enable Control
SHFIFO_MASK     equ     $000020
SHMST           equ     6               ;  6: Master Mode
SHMST_MASK      equ     $000040
SHRQE           equ     7               ;  7: Host Request Enable
SHRQE_MASK      equ     $000180
SHIDLE          equ     9               ;  9: Idle
SHIDLE_MASK     equ     $000200
SHBIE           equ     10              ; 10: Bus Error Interrupt Enable
SHBIE_MASK      equ     $000400
SHTIE           equ     11              ; 11: Transmit Interrupt Enable
SHTIE_MASK      equ     $000800
SHRIE           equ     12              ; 12: Receive Interrupt Enable
SHRIE_MASK      equ     $003000         ; 13: Receive Interrupt Enable  
SHTUE           equ     14              ; 14: Transmit Underrun Error
SHTUE_MASK      equ     $004000
SHTDE           equ     15              ; 15: Transmit Data Empty
SHTDE_MASK      equ     $008000
                                        ; 16: Reserved
SHRNE           equ     17              ; 17: Receive FIFO not empty
SHRNE_MASK      equ     $020000
                                        ; 18: Reserved
SHRFF           equ     19              ; 19: Receive FIFO full
SHRFF_MASK      equ     $080000
SHROE           equ     20              ; 20: Receive Overrun Error
SHROE_MASK      equ     $100000
SHBER           equ     21              ; 21: Host Bus Error
SHBER_MASK      equ     $200000
SHBUSY          equ     22              ; 22: Host Busy
SHBUSY_MASK     equ     $400000
;EB/1999-08-06 :: SHI register definitions corrected
;     SHI-Bit definitions adjusted to reflect reality
  CHGMSG '*** Tasking standard include fixed for errors in SHI defs. EB/1999-08-06'
                                        ; 23: Reserved

; Masks for  SHI Slave Address Register

                                        ;  0: Reserved
SHA1            equ     18              ; 18: I2C Slave Address bit 1
SHA1_MASK       equ     $040000
                                        ; 19: Reserved
SHAx            equ     20              ; 20: I2C Slave Address bit 3..6
SHAx_MASK       equ     $f00000

;NIF/1999-06-15 :: SHI register definitions corrected
;       SHRX : FFFF92->FFFF94
  CHGMSG '*** Tasking standard include fixed for errors in SHI defs. NIF/1999-06-15'

HCKR            equ     x:$FFFF90               ;  SHI Clock Control Reg.
HCSR            equ     x:$FFFF91               ;  SHI Control/Status Reg.
HSAR            equ     x:$FFFF92               ;  SHI Slave Address Reg.
SHTX            equ     x:$FFFF93               ;  SHI Transmit Data Reg.
SHRX            equ     x:$FFFF94               ;  SHI Receive Data FIFO
PCRC            equ     x:$FFFFBF               ;  Port C Control Register
PRRC            equ     x:$FFFFBE               ;  Port C Direction Register
PDRC            equ     x:$FFFFBD               ;  Port C GPIO Data Register

; Masks for  ESAI Transmit Clock Control Register

TPM             equ     0               ;  0: Prescale Modulus Select
TPM_MASK        equ     $0000ff
TPSR            equ     8               ;  8: Prescale Range
TPSR_MASK       equ     $000100
TDC             equ     9               ;  9: Frame Rate Divider Control
TDC_MASK        equ     $003e00
TFP             equ     14              ; 14: High Frequency Clock Divider Control
TFP_MASK        equ     $03c000
TCKP            equ     18              ; 18: Transmitter Clock Polarity
TCKP_MASK       equ     $040000
TFSP            equ     19              ; 19: Transmitter Frame Sync Polarity
TFSP_MASK       equ     $080000
THCKP           equ     20              ; 20: Transmitter HF Clock Polarity
THCKP_MASK      equ     $100000
TCKD            equ     21              ; 21: Transmitter Clock Source Direction
TCKD_MASK       equ     $200000
TFSD            equ     22              ; 22: Transmitter Frame Sync Signal Dir
TFSD_MASK       equ     $400000
THCKD           equ     23              ; 23: HF Clock Direction
THCKD_MASK      equ     $800000

; Masks for  ESAI Transmit Control Register

TE0             equ     0               ;  0: Transmit #0 Enable
TE0_MASK        equ     $000001
TE1             equ     1               ;  1: Transmit #1 Enable
TE1_MASK        equ     $000002
TE2             equ     2               ;  2: Transmit #2 Enable
TE2_MASK        equ     $000004
TE3             equ     3               ;  3: Transmit #3 Enable
TE3_MASK        equ     $000008
TE4             equ     4               ;  4: Transmit #4 Enable
TE4_MASK        equ     $000010
TE5             equ     5               ;  5: Transmit #5 Enable
TE5_MASK        equ     $000020
TSHFD           equ     6               ;  6: Transmitter Shift Direction
TSHFD_MASK      equ     $000040
TWA             equ     7               ;  7: Transmitter Word Alignment Control
TWA_MASK        equ     $000080
TMOD            equ     8               ;  8: Transmitter Network Mode Control
TMOD_MASK       equ     $000300
TSWS            equ     10              ; 10: Transmitter Slot and Word Length Sel.
TSWS_MASK       equ     $007c00
TFSL            equ     15              ; 15: Transmitter Frame Sync Length
TFSL_MASK       equ     $008000
TFSR            equ     16              ; 16: Transmitter Frame Sync Rel. Timing
TFSR_MASK       equ     $010000
                                        ; 17: Reserved
                                        ; 18: Reserved
TPR             equ     19              ; 19: Transmitter personal reset
TPR_MASK        equ     $080000
TEIE            equ     20              ; 20: Transmit Exception Interrupt Enable
TEIE_MASK       equ     $100000
TEDIE           equ     21              ; 21: Transmit Even Slot Data Int. Enable
TEDIE_MASK      equ     $200000
TIE             equ     22              ; 22: Transmit Interrupt Enable
TIE_MASK        equ     $400000
TLIE            equ     23              ; 23: Transmit Last Slot Interrupt Enable
TLIE_MASK       equ     $800000

;NIF/1999-10-27 :: ESAI Transmitter personal reset bit (TPR) added
  CHGMSG '*** ESAI Transmitter personal reset bit (TPR) added NIF/1999-10-27'

; Masks for  ESAI Receive Clock Control Register

RPM             equ     0               ;  0: Prescale Modulus Select
RPM_MASK        equ     $0000ff
RPSR            equ     8               ;  8: Prescale Range
RPSR_MASK       equ     $000100
RDC             equ     9               ;  9: Frame Rate Divider Control
RDC_MASK        equ     $003e00
RFP             equ     14              ; 14: High Frequency Clock Divider Control
RFP_MASK        equ     $03c000
RCKP            equ     18              ; 18: Receiver Clock Polarity
RCKP_MASK       equ     $040000
RFSP            equ     19              ; 19: Receiver Frame Sync Polarity
RFSP_MASK       equ     $080000
RHCKP           equ     20              ; 20: Receiver HF Clock Polarity
RHCKP_MASK      equ     $100000
RCKD            equ     21              ; 21: Receiver Clock Source Direction
RCKD_MASK       equ     $200000
RFSD            equ     22              ; 22: Receiver Frame Sync Signal Dir
RFSD_MASK       equ     $400000
RHCKD           equ     23              ; 23: HF Clock Direction
RHCKD_MASK      equ     $800000

; Masks for  ESAI Receive Control Register

RE0             equ     0               ;  0: Receive #0 Enable
RE0_MASK        equ     $000001
RE1             equ     1               ;  1: Receive #1 Enable
RE1_MASK        equ     $000002
RE2             equ     2               ;  2: Receive #2 Enable
RE2_MASK        equ     $000004
RE3             equ     3               ;  3: Receive #3 Enable
RE3_MASK        equ     $000008
                                        ;  4: Reserved
RSHFD           equ     6               ;  6: Receiver Shift Direction
RSHFD_MASK      equ     $000040
RWA             equ     7               ;  7: Receiver Word Alignment Control
RWA_MASK        equ     $000080
RMOD            equ     8               ;  8: Receiver Network Mode Control
RMOD_MASK       equ     $000300
RSWS            equ     10              ; 10: Receiver Slot and Word Length Sel.
RSWS_MASK       equ     $007c00
RFSL            equ     15              ; 15: Receiver Frame Sync Length
RFSL_MASK       equ     $008000
RFSR            equ     16              ; 16: Receiver Frame Sync Rel. Timing
RFSR_MASK       equ     $010000
                                        ; 17: Reserved
                                        ; 18: Reserved
RPR             equ     19              ; 19: Receiver personal reset
RPR_MASK        equ     $080000
REIE            equ     20              ; 20: Receive Exception Interrupt Enable
REIE_MASK       equ     $100000
REDIE           equ     21              ; 21: Receive Even Slot Data Int. Enable
REDIE_MASK      equ     $200000
RIE             equ     22              ; 22: Receive Interrupt Enable
RIE_MASK        equ     $400000
RLIE            equ     23              ; 23: Receive Last Slot Interrupt Enable
RLIE_MASK       equ     $800000


;NIF/1999-10-27 :: ESAI Receiver personal reset bit (RPR) added
  CHGMSG '*** ESAI Receiver personal reset bit (RPR) added NIF/1999-10-27'


; Masks for  ESAI Common Control Register

OF0             equ     0               ;  0: Serial Output Flag 0
OF0_MASK        equ     $000001
OF1             equ     1               ;  1: Serial Output Flag 1
OF1_MASK        equ     $000002
OF2             equ     2               ;  2: Serial Output Flag 2
OF2_MASK        equ     $000004
                                        ;  3: Reserved
SYN             equ     6               ;  6: Synchronous Mode Selection
SYN_MASK        equ     $000040
TEBE            equ     7               ;  7: Transmitter External Buffer Enable
TEBE_MASK       equ     $000080
ALC             equ     8               ;  8: Alignment Control
ALC_MASK        equ     $000100
                                        ;  9: Reserved

; Masks for  ESAI Status Register

IF0             equ     0               ;  0: Serial Input Flag 0
IF0_MASK        equ     $000001
IF1             equ     1               ;  1: Serial Input Flag 1
IF1_MASK        equ     $000002
IF2             equ     2               ;  2: Serial Input Flag 2
IF2_MASK        equ     $000004
                                        ;  3: Reserved
RFS             equ     6               ;  6: Receive Frame Sync Flag
RFS_MASK        equ     $000040
ROE             equ     7               ;  7: Receiver Overrun Error Flag
ROE_MASK        equ     $000080
RDF             equ     8               ;  8: Receive Data Register Full
RDF_MASK        equ     $000100
REDF            equ     9               ;  9: Receive Even-Data Register Full
REDF_MASK       equ     $000200
RODF            equ     10              ; 10: Receive Odd-Data Register Full
RODF_MASK       equ     $000400
                                        ; 11: Reserved
TFS             equ     13              ; 13: Transmit Frame Sync
TFS_MASK        equ     $002000
TUE             equ     14              ; 14: Transmit Underrun Error Flag
TUE_MASK        equ     $004000
TDE             equ     15              ; 15: Transmit Data Register Empty
TDE_MASK        equ     $008000
TEDE            equ     16              ; 16: Transmit Even-Data Register Empty
TEDE_MASK       equ     $010000
TODE            equ     17              ; 17: Transmit Odd-Data Register Empty
TODE_MASK       equ     $020000
                                        ; 18: Reserved

SAISR           equ     x:$FFFFB3               ;  ESAI Status Register
SAICR           equ     x:$FFFFB4               ;  ESAI Common Control Register
TCR             equ     x:$FFFFB5               ;  Transmit Control Register
TCCR            equ     x:$FFFFB6               ;  Transmit Clock Control Register
RCR             equ     x:$FFFFB7               ;  Receive  Control Register
RCCR            equ     x:$FFFFB8               ;  Receive  Clock Control Register
TSMA            equ     x:$FFFFB9               ;  ESAI Transmit Slot Mask Reg. A
TSMB            equ     x:$FFFFBA               ;  ESAI Transmit Slot Mask Reg. B
RSMA            equ     x:$FFFFBB               ;  ESAI Receive Slot Mask Reg. A
RSMB            equ     x:$FFFFBC               ;  ESAI Receive Slot Mask Reg. B
TX0             equ     x:$FFFFA0               ;  Transmit Data Register 0
TX1             equ     x:$FFFFA1               ;  Transmit Data Register 1
TX2             equ     x:$FFFFA2               ;  Transmit Data Register 2
TX3             equ     x:$FFFFA3               ;  Transmit Data Register 3
TX4             equ     x:$FFFFA4               ;  Transmit Data Register 4
TX5             equ     x:$FFFFA5               ;  Transmit Data Register 5
TSR             equ     x:$FFFFA6               ;  Time Slot Register
RX0             equ     x:$FFFFA8               ;  Receive Data Register 0
RX1             equ     x:$FFFFA9               ;  Receive Data Register 1
RX2             equ     x:$FFFFAA               ;  Receive Data Register 2
RX3             equ     x:$FFFFAB               ;  Receive Data Register 3

;NIF/1999-06-03 :: Port D definitions corrected
;       FFFFAF->FFFFD7, FFFFAE->FFFFD6, FFFFAD->FFFFD5
  CHGMSG '*** Tasking standard include fixed for errors in Port D defs. NIF/1999-06-03'
PCRD            equ     x:$FFFFD7               ;  Port D Control Register
PRRD            equ     x:$FFFFD6               ;  Port D Direction Register
PDRD            equ     x:$FFFFD5               ;  Port D GPIO Data Register

; Masks for  DAX Non-Audio Data Register

                                        ;  0: Reserved
XVA             equ     10              ; 10: DAX Channel A Validity
XVA_MASK        equ     $000400
XUA             equ     11              ; 11: DAX Channel A User Data
XUA_MASK        equ     $000800
XCA             equ     12              ; 12: DAX Channel A Channel Status
XCA_MASK        equ     $001000
XVB             equ     13              ; 13: DAX Channel B Validity
XVB_MASK        equ     $002000
XUB             equ     14              ; 14: DAX Channel B User Data
XUB_MASK        equ     $004000
XCB             equ     15              ; 15: DAX Channel B Channel Status
XCB_MASK        equ     $008000
                                        ; 16: Reserved

; Masks for  DAX Control Register

XDIE            equ     0               ;  0: Audio Data Reg Empty Interrupt Enable
XDIE_MASK       equ     $000001
XUIE            equ     1               ;  1: Audio Data Underrun Interrupt Enable
XUIE_MASK       equ     $000002
XBIE            equ     2               ;  2: Block Transferred Interrupt Enable
XBIE_MASK       equ     $000004
XCS             equ     3               ;  3: DAX Clock Input Select
XCS_MASK        equ     $000018
XSB             equ     5               ;  5: DAX Start Block
XSB_MASK        equ     $000020
                                        ;  6: Reserved

; Masks for  DAX Status Register

XADE            equ     0               ;  0: DAX Audio Data Register Empty
XADE_MASK       equ     $000001
XAUR            equ     1               ;  1: DAX Audio Data Underrun Error
XAUR_MASK       equ     $000002
XBLK            equ     2               ;  2: DAX Block Transfer Flag
XBLK_MASK       equ     $000004
                                        ;  3: Reserved

XSTR            equ     x:$FFFFD4               ;  DAX Status Register
XADRB           equ     x:$FFFFD3               ;  DAX Audio Data Register B
XADRA           equ     x:$FFFFD2               ;  DAX Audio Data Register A
XNADR           equ     x:$FFFFD1               ;  DAX Non-Audio Data Register
XCTR            equ     x:$FFFFD0               ;  DAX Control Register
PCRE            equ     y:$FFFF9F               ;  Port E Control Register
PRRE            equ     y:$FFFF9E               ;  Port E Direction Register
PDRE            equ     y:$FFFF9D               ;  Port E GPIO Data Register

; Masks for  Timer Control/Status Register

TEN             equ     0               ;  0: Timer Enable
TEN_MASK        equ     $000001
TOIE            equ     1               ;  1: Timer Overflow Interrupt Enable
TOIE_MASK       equ     $000002
TCIE            equ     2               ;  2: Timer Compare Interrupt Enable
TCIE_MASK       equ     $000004
                                        ;  3: Reserved
TCTRL           equ     4               ;  4: Timer Control
TCTRL_MASK      equ     $0000f0
TINV            equ     8               ;  8: Timer Inverter
TINV_MASK       equ     $000100
TRM             equ     9               ;  9: Timer Reload Mode
TRM_MASK        equ     $000200
                                        ; 10: Reserved
TDIR            equ     11              ; 11: Timer Direction
TDIR_MASK       equ     $000800
TDI             equ     12              ; 12: Timer Data Input
TDI_MASK        equ     $001000
TDO             equ     13              ; 13: Timer Data Output
TDO_MASK        equ     $002000
                                        ; 14: Reserved
TPCE            equ     15              ; 15: Timer Prescaler Clock Enable
TPCE_MASK       equ     $008000
                                        ; 16: Reserved
TOF             equ     20              ; 20: Timer Overflow Flag
TOF_MASK        equ     $100000
TCF             equ     21              ; 21: Timer Compare Flag
TCF_MASK        equ     $200000
                                        ; 22: Reserved

TCSR0           equ     x:$FFFF8F               ;  Timer 0 Control/Status
TLR0            equ     x:$FFFF8E               ;  Timer 0 Load Register
TCPR0           equ     x:$FFFF8D               ;  Timer 0 Compare Register
TCR0            equ     x:$FFFF8C               ;  Timer 0 Count Register
TCSR1           equ     x:$FFFF8B               ;  Timer 1 Control/Status
TLR1            equ     x:$FFFF8A               ;  Timer 1 Load Register
TCPR1           equ     x:$FFFF89               ;  Timer 1 Compare Register
TCR1            equ     x:$FFFF88               ;  Timer 1 Count Register
TCSR2           equ     x:$FFFF87               ;  Timer 2 Control/Status
TLR2            equ     x:$FFFF86               ;  Timer 2 Load Register
TCPR2           equ     x:$FFFF85               ;  Timer 2 Compare Register
TCR2            equ     x:$FFFF84               ;  Timer 2 Count Register
TPLR            equ     x:$FFFF83               ;  Timer Prescaler Load Reg.
TPCR            equ     x:$FFFF82               ;  Timer Prescaler Count Reg


; ESAI Mux Pin Control added by LAC 01-01-15

EMUXR           equ     y:$FFFFAF               ;  ESAI Mux Pin Control Register



; ESAI_1 Registers added by LAC 01-01-15

SAISR_1         equ     y:$FFFF93               ;  ESAI Status Register
SAICR_1         equ     y:$FFFF94               ;  ESAI Common Control Register
TCR_1           equ     y:$FFFF95               ;  Transmit Control Register
TCCR_1          equ     y:$FFFF96               ;  Transmit Clock Control Register
RCR_1           equ     y:$FFFF97               ;  Receive  Control Register
RCCR_1          equ     y:$FFFF98               ;  Receive  Clock Control Register
TSMA_1          equ     y:$FFFF99               ;  ESAI Transmit Slot Mask Reg. A
TSMB_1          equ     y:$FFFF9A               ;  ESAI Transmit Slot Mask Reg. B
RSMA_1          equ     y:$FFFF9B               ;  ESAI Receive Slot Mask Reg. A
RSMB_1          equ     y:$FFFF9C               ;  ESAI Receive Slot Mask Reg. B
TX0_1           equ     y:$FFFF80               ;  Transmit Data Register 0
TX1_1           equ     y:$FFFF81               ;  Transmit Data Register 1
TX2_1           equ     y:$FFFF82               ;  Transmit Data Register 2
TX3_1           equ     y:$FFFF83               ;  Transmit Data Register 3
TX4_1           equ     y:$FFFF84               ;  Transmit Data Register 4
TX5_1           equ     y:$FFFF85               ;  Transmit Data Register 5
TSR_1           equ     y:$FFFF86               ;  Time Slot Register
RX0_1           equ     y:$FFFF88               ;  Receive Data Register 0
RX1_1           equ     y:$FFFF89               ;  Receive Data Register 1
RX2_1           equ     y:$FFFF8A               ;  Receive Data Register 2
RX3_1           equ     y:$FFFF8B               ;  Receive Data Register 3




; Masks for  Interrupt Priority Register Core

IAL             equ     0               ;  0: IRQA Interrupt Priority Level
IAL_MASK        equ     $000003
IAT             equ     2               ;  2: IRQA Trigger mode
IAT_MASK        equ     $000004
IBL             equ     3               ;  3: IRQB Interrupt Priority Level
IBL_MASK        equ     $000018
IBT             equ     5               ;  5: IRQB Trigger mode
IBT_MASK        equ     $000020
ICL             equ     6               ;  6: IRQC Interrupt Priority Level
ICL_MASK        equ     $0000c0
ICT             equ     8               ;  8: IRQC Trigger mode
ICT_MASK        equ     $000100
IDL             equ     9               ;  9: IRQD Interrupt Priority Level
IDL_MASK        equ     $000600
IDT             equ     11              ; 11: IRQD Trigger mode
IDT_MASK        equ     $000800
D0L             equ     12              ; 12: DMA0 Interrupt Priority Level
D0L_MASK        equ     $003000
D1L             equ     14              ; 14: DMA1 Interrupt Priority Level
D1L_MASK        equ     $00c000
D2L             equ     16              ; 16: DMA2 Interrupt Priority Level
D2L_MASK        equ     $030000
D3L             equ     18              ; 18: DMA3 Interrupt Priority Level
D3L_MASK        equ     $0c0000
D4L             equ     20              ; 20: DMA4 Interrupt Priority Level
D4L_MASK        equ     $300000
D5L             equ     22              ; 22: DMA5 Interrupt Priority Level
D5L_MASK        equ     $c00000

; Masks for  Interrupt Priority Register Periph.

;NIF/1999-09-22 :: IPRP register definitions corrected
;       Bitfields did not match 56362
;LAC/2001-01-18 :: Added ESAI_1 definition
  CHGMSG 'Tasking standard include fixed for errors in IPRP defs. NIF/1999-09-22'

ESL             equ     0               ;  0: ESAI  Interrupt Priority Level
ESL_MASK        equ     $000003
SHL             equ     2               ;  2: SHI   Interrupt Priority Level
SHL_MASK        equ     $00000c
HDL             equ     4               ;  4: HDI08 Interrupt Priority Level
HDL_MASK        equ     $000030
DAL             equ     6               ;  6: DAX   Interrupt Priority Level
DAL_MASK        equ     $0000c0
TAL             equ     8               ;  8: Timer Interrupt Priority Level
TAL_MASK        equ     $000300
ESL1            equ     10              ; 10: ESAI_1 Interrupt Priority Level

IPRC            equ     x:$FFFFFF               ;  Interrupt Priority Register Core
IPRP            equ     x:$FFFFFE               ;  Interrupt Priority Register Peripheral

        ENDIF   ; !@DEF('SFR_DEFINED')


#ifndef _MIXER8_H_
#define	_MIXER8_H_

#ifdef _MIXER8

#include "TCTypes.h"
#include "PacketBlock.h"

#define	NO_OF_INPUT_CHANNELS		8
#define	NO_OF_OUTPUT_CHANNELS		8
#define	NO_OF_COEFFS				NO_OF_INPUT_CHANNELS * NO_OF_OUTPUT_CHANNELS * 4

#define MIXER8_BASE_START_HI		0xffff
#define MIXER8_BASE_START			0xe0200000
#define MIXER8_BASE_END_HI			0xffff
#define MIXER8_OFFSET(a)			((uint32)(&mixer8.a) - (uint32)&mixer8)
#define MIXER8_BASE_END				MIXER8_BASE_START + MIXER8_OFFSET(ShadowCoeffs[63]) + 4

#define	MIXER8_SRAM_DATA			__attribute__  ((section (".sram.mixer8")))
#define	MIXER8_SRAM_CODE			__attribute__  ((section (".sram.mixer8FiqHandler")))

#define	ARM_AUDIO_IN_ADDRESS  		0xCE160000
#define	ARM_AUDIO_OUT_ADDRESS  		0xCE160000
#define ARM_APB_INTERRUPT_REGISTER 	0xCE160100

#define ARM_APB_INTERRUPT_NOT_SERVED_BIT 	0x02

#define MAX_MIXER_CHANNELS_8BY8	8
#define MAX_MIXER_CHANNELS_8BY4	8
#define MAX_MIXER_CHANNELS_4BY4	4
#define MAX_MIXER_CHANNELS		8

#define MAX_AUX_SEND_8BY8		6
#define MAX_AUX_SEND_8BY4		2
#define MAX_AUX_SEND_4BY4		2
#define MAX_AUX_SEND			6

#define NORM	0x8000
#define	PAN		0x8000

#define MIXER_8BY8		0x0000001
#define MIXER_8BY4		0x0000002
#define MIXER_4BY4		0x0000004
#define MIXER_RAW_MODE	0x0000001

#define MIXER_LOW_RATE	0x01
#define MIXER_MED_RATE	0x02
#define MIXER_HIGH_RATE	0x04

//For Reading
#define MIXER_LOW_RATE_SHIFT	0
#define MIXER_MED_RATE_SHIFT	8
#define MIXER_HIGH_RATE_SHIFT	16
#define MIXER_COEFF_SHIFT		24

//For Writing
#define MIXER_CONFIG_SHIFT	0
#define MIXER_RATE_SHIFT	8
//#define MIXER_COEFF_SHIFT	16

//For Reading
#define MIXER_LOW_RATE_MASK		0x000000FF
#define MIXER_MED_RATE_MASK		0x0000FF00
#define MIXER_HIGH_RATE_MASK	0x00FF0000
#define MIXER_COEFF_MASK		0xFF000000

//For Writing
#define MIXER_CONFIG_MASK	0x000000FF
#define MIXER_RATE_MASK		0x0000FF00
#define MIXER_COEFF_MASK	0xFF000000

#define MIXER_SUPPORTED_CONFIGURATION (	((MIXER_8BY8 | MIXER_8BY4 | MIXER_4BY4)<<(MIXER_LOW_RATE_SHIFT)) |\
										((MIXER_8BY4 | MIXER_4BY4)<<(MIXER_MED_RATE_SHIFT)) |\
										((MIXER_4BY4)<<(MIXER_HIGH_RATE_SHIFT))|\
										((MIXER_RAW_MODE)<<(MIXER_COEFF_SHIFT)))

#define MIXER_CURRENT_CONFIGURATION		((MIXER_LOW_RATE <<MIXER_RATE_SHIFT) | (MIXER_8BY8))

#define ZERO_DB_GAIN_COEFF 0x4000

#define	MIXER8_VERSION_A	1
#define	MIXER8_VERSION_B	0
#define	MIXER8_VERSION_C	0
#define	MIXER8_VERSION_D	0

#define	MIXER8_VERSION	( (	MIXER8_VERSION_A << 24 ) |\
						( MIXER8_VERSION_B << 16 ) |\
						( MIXER8_VERSION_C << 8 ) |\
						( MIXER8_VERSION_A ) )

typedef struct
{
	uint32				On;
	uint32				PrePost;
	uint32				Value;
} MIXER8_AUX;

typedef struct
{
	uint32				Mute;
	uint32				Solo;
	uint32				Fader;
	int					Pan;
	uint32				Coupled;	
	MIXER8_AUX			Aux[MAX_AUX_SEND];	
} MIXER8_CH_PARAMS;

typedef struct
{
	uint32				Version;
	uint32				Enable;
	uint32				SupportedConfiguration;
	uint32				CurrentConfiguration;
	uint32				MasterGain;
	uint32				MasterMute;
	int					MasterPan;
	uint32				MaxAuxAvailable;
	uint32				MaxChannelAvailable;
	MIXER8_CH_PARAMS	ChannelParameters[MAX_MIXER_CHANNELS];
	uint32				ShadowCoeffs[64];
}MIXER8;

extern MIXER8 mixer8;
extern uint32 mixer8SemID;

HRESULT		mixer8Initialize(void);
HRESULT		mixer8InitializeTasks(void);		// create tasks for this module
HRESULT		mixer8ResumeTasks(void);			// resume tasks for this module
HRESULT		mixer8SuspendTasks(void);			// suspend tasks for this module

void		mixer_8by8(void) MIXER8_SRAM_CODE;
void		mixer_8by4(void) MIXER8_SRAM_CODE;
void		mixer_4by4(void) MIXER8_SRAM_CODE;
void		mixer_4by4_high_rate_2(void) MIXER8_SRAM_CODE;

void		mixer8Update(void);	
bool		mixer8CheckValidConfiguration(uint32 mixerDesiredConfiguration);
void		mixer8_Enable(bool flag);
int			mixer8ReadOverFlow(void);

#endif//_MIXER8

#endif//_MIXER8_H_

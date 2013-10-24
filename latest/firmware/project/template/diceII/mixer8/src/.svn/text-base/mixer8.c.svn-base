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
 * DESCRIPTION: mixer8

	Purpose:	mixer8
				

	Revisions:
		created 09/03/2005 AliA

****************************************************************************/

#include "TCTasking.h"
#include "TCInterrupt.h"
#include "ErrorCodes.h"
#include "mixer8.h"
#include "mixer8Cli.h"
#include "mixer8firewire.h"
#include "mixer8DAL.h"
#include "lal.h"
#include "dal.h"

#ifdef _MIXER8

int 	mixer8In[32] 	MIXER8_SRAM_DATA;
int		mixer8Coeff[64] MIXER8_SRAM_DATA;
int		mixer8Count 	MIXER8_SRAM_DATA;
int		mixer8Enable 	MIXER8_SRAM_DATA;
int		mixer8SP		MIXER8_SRAM_DATA;
int		mixer8OverFlow	MIXER8_SRAM_DATA;

MIXER8	mixer8;

// semaphore IDs for this module
uint32					mixer8SemID;			// mixer8 update semaphore 

// task (thread) IDs for this module
uint32					mixer8ThreadID;			// mixer8 update thread
const char*				mixer8ThreadName = "mixer8Thread";

void mixer8Thread(void *dummy)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(mixer8SemID);		
		if (hResult != NO_ERROR) 
		{
			sysDebugPrintf("Critical Error in mixer8 Thread\r\n");
			DO_FOREVER {}
		}
		mixer8Update();						
	}
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT mixer8ResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// mixer8 update thread
	hResult = TCTaskResume(mixer8ThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT mixer8SuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// mixer8 update thread
	hResult = TCTaskSuspend(mixer8ThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT mixer8InitializeTasks(void)
{
	HRESULT	hResult = NO_ERROR;
	
	// mixer8 update thread
	hResult = TCTaskCreate(&mixer8ThreadID, mixer8ThreadName, 
		mixer8Thread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT mixer8Initialize(void)
{
	uint32 i;
	HRESULT	hResult = NO_ERROR;
	
	//Initialize mixer8
	memset(&mixer8,0,sizeof(mixer8));	
	for(i=0;i<MAX_MIXER_CHANNELS;i++)
		mixer8.ChannelParameters[i].Fader = ZERO_DB_GAIN_COEFF;
	mixer8.MasterGain = ZERO_DB_GAIN_COEFF;			
	mixer8.SupportedConfiguration = MIXER_SUPPORTED_CONFIGURATION;	
	mixer8.CurrentConfiguration = MIXER_CURRENT_CONFIGURATION;
	mixer8.MaxAuxAvailable = MAX_AUX_SEND_8BY8;
	mixer8.MaxChannelAvailable = MAX_MIXER_CHANNELS_8BY8;
	mixer8.Version = MIXER8_VERSION;
	mixer8Enable = 0;
	mixer8OverFlow = 0;

	// mixer8 update semaphore
	hResult = TCSemaphoreOpen(&mixer8SemID, 0);
	if (hResult != NO_ERROR) return hResult;

	hResult = mixer8InitializeTasks();
	if (hResult != NO_ERROR) return hResult;	

	hResult = TCTaskResume(mixer8ThreadID);
	if (hResult != NO_ERROR) return hResult;	

#ifdef _CLI_TOOL_MIXER8
	mixer8CliInstallTools();
#endif //_CLI_TOOL_MIXER8

	return hResult;
}

bool mixer8CheckValidConfiguration(uint32 mixerDesiredConfiguration)
{	
	uint32	mixerSamplingRate;
	uint32	mixerMode;
	bool status = false;

	mixerSamplingRate = (((mixerDesiredConfiguration)&MIXER_RATE_MASK)>>MIXER_RATE_SHIFT);
	mixerMode = ((mixerDesiredConfiguration)&MIXER_CONFIG_MASK);	

	switch(mixerMode)
	{
	case MIXER_8BY8:
		if(mixerSamplingRate == MIXER_LOW_RATE)
		{
			mixer8.MaxAuxAvailable = MAX_AUX_SEND_8BY8;
			mixer8.MaxChannelAvailable = MAX_MIXER_CHANNELS_8BY8;
			status =  true;			
		}
		else
			status = false;
		break;
	case MIXER_8BY4:
		if(mixerSamplingRate != MIXER_HIGH_RATE)
		{
			mixer8.MaxAuxAvailable = MAX_AUX_SEND_8BY4;
			mixer8.MaxChannelAvailable = MAX_MIXER_CHANNELS_8BY4;
			status =  true;			
		}
		else
			status = false;
		break;
	case MIXER_4BY4:
		mixer8.MaxAuxAvailable = MAX_AUX_SEND_4BY4;
		mixer8.MaxChannelAvailable = MAX_MIXER_CHANNELS_4BY4;
		status =  true;		
		break;
	default:
		status = false;
		break;
	}
	return status;
}

void mixer8Update(void)
{
	int i,j;
	uint32	mixer8Solo;
	uint32	mixer8CoeffMode;

	//Mixer Coeff Mode
	mixer8CoeffMode = (mixer8.CurrentConfiguration & MIXER_COEFF_MASK)>>MIXER_COEFF_SHIFT;
	if(mixer8CoeffMode)
	{
		for(i=0;i<64;i++)
			if(mixer8.ShadowCoeffs[i] > 0x8000)
				mixer8.ShadowCoeffs[i] = 0x8000;
			mixer8Coeff[i] = mixer8.ShadowCoeffs[i];
	}
	else
	{
		//For Solo Channels
		mixer8Solo = 0;
		for(i=0;i<mixer8.MaxChannelAvailable;i++)
		{
			if(mixer8.ChannelParameters[i].Solo)
				mixer8Solo = 1;
		}
		if(!mixer8Solo)
		{
			//Left Right Channels
			for(i=0;i<mixer8.MaxChannelAvailable;i++)
			{		
				if(!mixer8.ChannelParameters[i].Mute && !mixer8.MasterMute)
				{			
					if( mixer8.ChannelParameters[i].Pan > 0 )
					{
						mixer8Coeff[i] = ((mixer8.ChannelParameters[i].Fader * mixer8.MasterGain )
							*(PAN - mixer8.ChannelParameters[i].Pan))/( PAN* NORM);
						mixer8Coeff[i+MAX_MIXER_CHANNELS] = (mixer8.ChannelParameters[i].Fader * 
							mixer8.MasterGain)/( NORM);
					}
					else if ( mixer8.ChannelParameters[i].Pan < 0 )
					{
						mixer8Coeff[i] = (mixer8.ChannelParameters[i].Fader * mixer8.MasterGain)/NORM;
						mixer8Coeff[i+MAX_MIXER_CHANNELS] = ((mixer8.ChannelParameters[i].Fader * 
							mixer8.MasterGain)*
							(PAN + mixer8.ChannelParameters[i].Pan))/(PAN*NORM);
					}
					else
					{
						mixer8Coeff[i] = (mixer8.ChannelParameters[i].Fader * mixer8.MasterGain)/NORM;
						mixer8Coeff[i+MAX_MIXER_CHANNELS] = (mixer8.ChannelParameters[i].Fader * 
							mixer8.MasterGain)/NORM;
					}
				}
				else
				{
					mixer8Coeff[i] = 0;
					mixer8Coeff[i+MAX_MIXER_CHANNELS] = 0;
				}
			}
			//Aux Channels
			for(i=0;i<mixer8.MaxAuxAvailable;i++)
			{
				for(j=0;j<mixer8.MaxChannelAvailable;j++)
				{
					if(mixer8.ChannelParameters[j].Aux[i].On)
					{
						if(mixer8.ChannelParameters[j].Aux[i].PrePost)
						{
							mixer8Coeff[i*MAX_MIXER_CHANNELS+j+MAX_MIXER_CHANNELS*2] = 
								(mixer8.ChannelParameters[j].Aux[i].Value * 
								mixer8.ChannelParameters[j].Fader)/NORM;	
						}
						else
						{
							mixer8Coeff[i*MAX_MIXER_CHANNELS+j+MAX_MIXER_CHANNELS*2] = 
								mixer8.ChannelParameters[j].Aux[i].Value;
						}
					}
					else
					{
						mixer8Coeff[i*MAX_MIXER_CHANNELS+j+MAX_MIXER_CHANNELS*2] = 0;
					}
				}
			}
		}
		else
		{
			//Left Right Channels
			for(i=0;i<mixer8.MaxChannelAvailable;i++)
			{		
				if(!mixer8.ChannelParameters[i].Mute && mixer8.ChannelParameters[i].Solo &&
					!mixer8.MasterMute)
				{			
					if( mixer8.ChannelParameters[i].Pan > 0 )
					{
						mixer8Coeff[i] = ((mixer8.ChannelParameters[i].Fader * mixer8.MasterGain)*
							(PAN - mixer8.ChannelParameters[i].Pan))/(PAN*NORM);
						mixer8Coeff[i+MAX_MIXER_CHANNELS] = (mixer8.ChannelParameters[i].Fader * 
							mixer8.MasterGain)/( NORM);
					}
					else if ( mixer8.ChannelParameters[i].Pan < 0)
					{
						mixer8Coeff[i] = (mixer8.ChannelParameters[i].Fader * mixer8.MasterGain)/NORM;
						mixer8Coeff[i+MAX_MIXER_CHANNELS] = ((mixer8.ChannelParameters[i].Fader * 
							mixer8.MasterGain)*
							(PAN + mixer8.ChannelParameters[i].Pan))/(PAN*NORM);
					}
					else
					{
						mixer8Coeff[i] = (mixer8.ChannelParameters[i].Fader * mixer8.MasterGain)/NORM;
						mixer8Coeff[i+MAX_MIXER_CHANNELS] = (mixer8.ChannelParameters[i].Fader * 
							mixer8.MasterGain)/NORM;
					}
				}
				else
				{
					mixer8Coeff[i] = 0;
					mixer8Coeff[i+MAX_MIXER_CHANNELS] = 0;
				}
			}
			//Aux Channels
			for(i=0;i<mixer8.MaxAuxAvailable;i++)
			{
				for(j=0;j<mixer8.MaxChannelAvailable;j++)
				{
					if(mixer8.ChannelParameters[j].Aux[i].On && mixer8.ChannelParameters[j].Solo)
					{
						if(mixer8.ChannelParameters[j].Aux[i].PrePost)
						{
							mixer8Coeff[i*MAX_MIXER_CHANNELS+j+MAX_MIXER_CHANNELS*2] = 
								(mixer8.ChannelParameters[j].Aux[i].Value * 
								mixer8.ChannelParameters[j].Fader)/NORM;	
						}
						else
						{
							mixer8Coeff[i*MAX_MIXER_CHANNELS+j+MAX_MIXER_CHANNELS*2] = 
								mixer8.ChannelParameters[j].Aux[i].Value;
						}								
					}
					else
					{
						mixer8Coeff[i*MAX_MIXER_CHANNELS+j+MAX_MIXER_CHANNELS*2] = 0;
					}
				}
			}
		}
		for(i=0;i<64;i++)
		{
			if(mixer8Coeff[i] > 0x8000)
				mixer8Coeff[i] = 0x8000;
			mixer8.ShadowCoeffs[i] = mixer8Coeff[i];
		}
	}	
	mixer8DALInstallFIQHandler();
}

void mixer8_Enable(bool flag)
{
	if(flag)
	{
		mixer8Enable = 1;
		mixer8OverFlow = 0;
	}
	else
		mixer8Enable = 0;
}

int mixer8_tempOverFlow;

int mixer8ReadOverFlow(void)
{
	sysDebugPrintf("Over Flow before =  %X\n\t",mixer8OverFlow);
	asm volatile ( 
		"mov r8,#0						\n\t"
		"ldr r9,=mixer8OverFlow			\n\t"
		"swp r8,r8,[r9]					\n\t"
		"ldr r9,=mixer8_tempOverFlow	\n\t"
		"str r8,[r9]					\n\t");		
	sysDebugPrintf("Over Flow after =  %X\n\t",mixer8OverFlow);
	sysDebugPrintf("temp Over Flow before =  %X\n\t",mixer8_tempOverFlow);

	return mixer8_tempOverFlow;	
}


void mixer_8by8(void)
{	
	asm volatile(
		"stmfd	sp!,{lr}			\n\t"
		"ldr	r0,=mixer8SP		\n\t"
		"str	sp,[r0]				\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#1				\n\t"
		"beq	skip_disable_8by8	\n\t"
		"ldr	r0,=0xc3000000		\n\t"		
		"ldr	r1,[r0]				\n\t"
		"orr	r1,r1,#0x80			\n\t"
		"str	r1,[r0]				\n\t"
		"mov 	r13,#8				\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t"									
      	"str 	r13,[r1]			\n\t"
		"mov	r14,#0				\n\t"
      	"ldr 	r8,=mixer8Coeff		\n\t"	 						     
	    "ldr 	r9,=mixer8In		\n\t" 								
		"ldr	r10,=0xCE160000		\n\t"		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
	    "mov 	r4,r4,lsl#8			\n\t" 								
    	"mov 	r5,r5,lsl#8			\n\t" 								
    	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov	r1,r1,lsl#8			\n\t"								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t"								 	
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"sub 	r9,r9,#128			\n\t" 								
		"sub	r10,r10,#128		\n\t"
    	"loop_8by8:					\n\t"      	
      	"ldmia 	r8!,{r0-r3}			\n\t" 						
      	"ldmia 	r9!,{r4-r7}			\n\t" 						
      	"smull 	r11,r12,r4,r0		\n\t" 								
      	"smlal 	r11,r12,r5,r1		\n\t" 								
      	"smlal 	r11,r12,r6,r2		\n\t" 								
      	"smlal 	r11,r12,r7,r3		\n\t" 								
      	"ldmia 	r8!,{r0-r3}			\n\t" 						
      	"ldmia 	r9!,{r4-r7}			\n\t" 						
      	"smlal 	r11,r12,r4,r0		\n\t" 								
      	"smlal 	r11,r12,r5,r1		\n\t" 								
      	"smlal 	r11,r12,r6,r2		\n\t" 								
      	"smlal 	r11,r12,r7,r3		\n\t"
		"sub	r9,r9,#32			\n\t"
		"mov	r14,r14,lsl#1       \n\t"
		"movs 	r4,r12,asr#12		\n\t" 								
      	"beq skip1_8by8				\n\t" 											
      	"mvns 	r5,r4				\n\t" 										
      	"beq skip1_8by8				\n\t" 											
      	"movs 	r3,r12,asr#31		\n\t" 								
      	"moveq 	r3,#0x7f			\n\t" 									
      	"movmi 	r3,#0x80			\n\t" 									
      	"mov 	r3,r3,lsl#16		\n\t"								
      	"str 	r3,[r10],#4			\n\t"		
		"orr	r14,r14,#1			\n\t"
      	"subs 	r13,r13,#1			\n\t"		
      	"bne loop_8by8				\n\t"
		"mov 	r13,#8				\n\t" 										
      	"add 	r9,r9,#32			\n\t"									
      	"sub 	r8,r8,#256			\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t" 									
      	"ldr 	r0,[r1]				\n\t" 										
      	"subs 	r0,r0,#2			\n\t" 									
      	"str 	r0,[r1]				\n\t" 
      	"bne loop_8by8				\n\t"
		"beq mixer8_exit_8by8		\n\t"
		"skip1_8by8:				\n\t" 
      	"mov 	r2,r12,lsl#11		\n\t" 								
      	"mov 	r1,r11,lsr#21		\n\t" 								
      	"orr 	r3,r2,r1			\n\t" 									
      	"str 	r3,[r10],#4			\n\t" 								
      	"subs 	r13,r13,#1			\n\t" 									
      	"bne loop_8by8				\n\t"
		"mixer8_reintialize_8by8:	\n\t"
      	"mov 	r13,#8				\n\t" 										
      	"add 	r9,r9,#32			\n\t"									
      	"sub 	r8,r8,#256			\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t" 									
      	"ldr 	r0,[r1]				\n\t" 										
      	"subs 	r0,r0,#2			\n\t" 									
      	"str 	r0,[r1]				\n\t" 
      	"bne loop_8by8				\n\t"
		"mixer8_exit_8by8:			\n\t"
		"ldr	r0,=0xCE160100		\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#2				\n\t"
		"beq	skip_disable_8by8	\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8			\n\t"		
		"str	r1,[r0]				\n\t"
		"skip_disable_8by8:			\n\t"
		"ldr	r0,=mixer8OverFlow	\n\t"
		"ldr	r1,[r0]				\n\t"
		"orr	r14,r14,r1			\n\t"
		"str	r14,[r0]			\n\t"
		"ldr	r0,=0xCE160100		\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=0xc3000000		\n\t"		
		"ldr	r1,[r0]				\n\t"
		"bic	r1,r1,#128			\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8SP		\n\t"	
		"ldr	sp,[r0]				\n\t"
		"ldmfd  sp!,{pc}			\n\t"	
	);	
}

void mixer_8by4(void)
{
	asm volatile(
		"stmfd	sp!,{lr}			\n\t"
		"ldr	r0,=mixer8SP		\n\t"
		"str	sp,[r0]				\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#1				\n\t"
		"beq	skip_disable_8by8	\n\t"
		"ldr	r0,=0xc3000000		\n\t"		
		"ldr	r1,[r0]				\n\t"
		"orr	r1,r1,#0x80			\n\t"
		"str	r1,[r0]				\n\t"
		"mov 	r13,#4				\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t"									
      	"str 	r13,[r1]			\n\t"
		"mov	r14,#0				\n\t"
      	"ldr 	r8,=mixer8Coeff		\n\t"	 						     
	    "ldr 	r9,=mixer8In		\n\t" 								
		"ldr	r10,=0xCE160000		\n\t"		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
	    "mov 	r4,r4,lsl#8			\n\t" 								
    	"mov 	r5,r5,lsl#8			\n\t" 								
    	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov	r1,r1,lsl#8			\n\t"								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t"								 	
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r9!,{r0-r7}			\n\t" 		
      	"sub 	r9,r9,#128			\n\t" 								
		"sub	r10,r10,#128		\n\t"					
      	"loop_8by4:					\n\t"
      	"ldmia 	r8!,{r0-r3}			\n\t" 						
      	"ldmia 	r9!,{r4-r7}			\n\t" 						
      	"smull 	r11,r12,r4,r0		\n\t" 								
      	"smlal 	r11,r12,r5,r1		\n\t" 								
      	"smlal 	r11,r12,r6,r2		\n\t" 								
      	"smlal 	r11,r12,r7,r3		\n\t" 								
      	"ldmia 	r8!,{r0-r3}			\n\t" 						
      	"ldmia 	r9!,{r4-r7}			\n\t" 						
      	"smlal 	r11,r12,r4,r0		\n\t" 								
      	"smlal 	r11,r12,r5,r1		\n\t" 								
      	"smlal 	r11,r12,r6,r2		\n\t" 								
      	"smlal 	r11,r12,r7,r3		\n\t"
		"sub	r9,r9,#32			\n\t"
		"mov	r14,r14,lsl#1       \n\t"
		"movs 	r4,r12,asr#12		\n\t" 								
      	"beq skip1_8by4				\n\t" 											
      	"mvns 	r5,r4				\n\t" 										
      	"beq skip1_8by4				\n\t" 											
      	"movs 	r3,r12,asr#31		\n\t" 								
      	"moveq 	r3,#0x7f			\n\t" 									
      	"movmi 	r3,#0x80			\n\t"
		"mov 	r3,r3,lsl#16		\n\t"								
      	"str 	r3,[r10],#4			\n\t"
		"orr	r14,r14,#1			\n\t"
      	"subs 	r13,r13,#1			\n\t"									
      	"bne loop_8by4				\n\t"		
		"mov 	r13,#4				\n\t" 										
      	"add 	r9,r9,#32			\n\t"									
      	"sub 	r8,r8,#128			\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t" 									
      	"ldr 	r0,[r1]				\n\t" 										
      	"subs 	r0,r0,#1			\n\t" 									
      	"str 	r0,[r1]				\n\t" 
      	"bne loop_8by4				\n\t"
		"beq mixer8_exit_8by4		\n\t"
		"skip1_8by4:				\n\t" 
      	"mov 	r2,r12,lsl#11		\n\t" 								
      	"mov 	r1,r11,lsr#21		\n\t"
      	"orr 	r3,r2,r1			\n\t"		
      	"str 	r3,[r10],#4			\n\t" 								
      	"subs 	r13,r13,#1			\n\t" 									
      	"bne loop_8by4				\n\t"
		"mixer8_reintialize_8by4:	\n\t"
		"add	r10,r10,#16			\n\t"//Cover for 4 less writes to output channels
      	"mov 	r13,#4				\n\t" 										
      	"add 	r9,r9,#32			\n\t"									
      	"sub 	r8,r8,#128			\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t" 									
      	"ldr 	r0,[r1]				\n\t" 										
      	"subs 	r0,r0,#1			\n\t" 									
      	"str 	r0,[r1]				\n\t" 
      	"bne loop_8by4				\n\t"
		"mixer8_exit_8by4:			\n\t"
		"ldr	r0,=0xCE160100		\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#2				\n\t"
		"beq	skip_disable_8by4	\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8			\n\t"
		"str	r1,[r0]				\n\t"
		"skip_disable_8by4:			\n\t"
		"ldr	r0,=mixer8OverFlow	\n\t"
		"ldr	r1,[r0]				\n\t"
		"orr	r14,r14,r1			\n\t"
		"str	r14,[r0]			\n\t"
		"ldr	r0,=0xCE160100		\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=0xc3000000		\n\t"		
		"ldr	r1,[r0]				\n\t"
		"bic	r1,r1,#128			\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8SP		\n\t"	
		"ldr	sp,[r0]				\n\t"
		"ldmfd  sp!,{pc}			\n\t"	
	);	
}

void mixer_4by4(void)
{
	asm volatile(
		"stmfd	sp!,{lr}			\n\t"
		"ldr	r0,=mixer8SP		\n\t"
		"str	sp,[r0]				\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#1				\n\t"
		"beq	skip_disable_4by4	\n\t"
		"ldr	r0,=0xc3000000		\n\t"		
		"ldr	r1,[r0]				\n\t"
		"orr	r1,r1,#0x80			\n\t"
		"str	r1,[r0]				\n\t"		
		"mov 	r13,#4				\n\t"//runs 4 times only as output channels are limited to 4
      	"ldr 	r1,=mixer8Count		\n\t"									
      	"str 	r13,[r1]			\n\t"									
      	"ldr 	r8,=mixer8Coeff		\n\t"	 						     
	    "ldr 	r14,=mixer8In		\n\t" 								
		"ldr	r10,=0xCE160000		\n\t"		
      	"ldmia 	r10!,{r0-r3}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t"
		"add	r10,r10,#16			\n\t"
		"add	r14,r14,#16			\n\t"
      	"stmia 	r14!,{r0-r3}		\n\t" 		
      	"ldmia 	r10!,{r0-r3}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov	r1,r1,lsl#8			\n\t"								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"add	r10,r10,#16			\n\t"
		"add	r14,r14,#16			\n\t"
      	"stmia 	r14!,{r0-r3}		\n\t" 		
      	"ldmia 	r10!,{r0-r3}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"add	r10,r10,#16			\n\t"
		"add	r14,r14,#16			\n\t"
      	"stmia 	r14!,{r0-r3}		\n\t" 		
      	"ldmia 	r10!,{r0-r3}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"stmia 	r14!,{r0-r3}		\n\t" 		
      	"add	r10,r10,#16			\n\t"
		"add	r14,r14,#16			\n\t"
      	"sub 	r14,r14,#128		\n\t" 								
  		"ldr	r10,=0xCE160000		\n\t"					
      	"loop_4by4:					\n\t"
      	"mov 	r9,r14				\n\t" 										
      	"ldmia 	r8!,{r0-r3}			\n\t"
		"add 	r8,r8,#16			\n\t"
      	"ldmia 	r9!,{r4-r7}			\n\t" 						
      	"smull 	r11,r12,r4,r0		\n\t" 								
      	"smlal 	r11,r12,r5,r1		\n\t" 								
      	"smlal 	r11,r12,r6,r2		\n\t" 								
      	"smlal 	r11,r12,r7,r3		\n\t" 								
		"movs 	r4,r12,asr#12		\n\t" 								
      	"beq skip1_4by4				\n\t" 											
      	"mvns 	r5,r4				\n\t" 										
      	"beq skip1_4by4				\n\t" 											
      	"movs 	r3,r12,asr#31		\n\t" 								
      	"moveq 	r3,#0x7f			\n\t" 									
      	"movmi 	r3,#0x80			\n\t" 									
      	"mov 	r3,r3,lsl#16		\n\t"								
      	"str 	r3,[r10],#4			\n\t" 								
      	"subs 	r13,r13,#1			\n\t"									
      	"bne loop_4by4				\n\t"
		"beq mixer8_reintialize_4by4\n\t"
		"skip1_4by4:				\n\t" 
      	"mov 	r2,r12,lsl#11		\n\t" 								
      	"mov 	r1,r11,lsr#21		\n\t" 								
      	"orr 	r3,r2,r1			\n\t" 									
      	"str 	r3,[r10],#4			\n\t" 								
      	"subs 	r13,r13,#1			\n\t" 									
      	"bne loop_4by4				\n\t"
		"mixer8_reintialize_4by4:	\n\t"
		"add	r10,r10,#16			\n\t"//Cover for 4 less writes to output channels
      	"mov 	r13,#4				\n\t" 										
      	"add 	r14,r14,#32			\n\t"									
      	"sub 	r8,r8,#128			\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t" 									
      	"ldr 	r0,[r1]				\n\t" 										
      	"subs 	r0,r0,#1			\n\t" 									
      	"str 	r0,[r1]				\n\t" 
      	"bne loop_4by4				\n\t"		
		"ldr	r0,=0xCE160100		\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#2				\n\t"
		"beq	skip_disable_4by4	\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8			\n\t"
		"str	r1,[r0]				\n\t"
		"skip_disable_4by4:			\n\t"
		"ldr	r0,=0xCE160100		\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=0xc3000000		\n\t"		
		"ldr	r1,[r0]				\n\t"
		"bic	r1,r1,#128			\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8SP		\n\t"	
		"ldr	sp,[r0]				\n\t"
		"ldmfd  sp!,{pc}			\n\t"
	);
}

void mixer_4by4_high_rate_2(void)
{
	asm volatile(
		"stmfd	sp!,{lr}			\n\t"
		"ldr	r0,=mixer8SP		\n\t"
		"str	sp,[r0]				\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#1				\n\t"
		"beq	skip_disable_4by4_hi\n\t"
		"mov 	r13,#4				\n\t"//runs 4 times only as output channels are limited to 4
      	"ldr 	r1,=mixer8Count		\n\t"									
      	"str 	r13,[r1]			\n\t"									
      	"ldr 	r8,=mixer8Coeff		\n\t"	 						     
	    "ldr 	r14,=mixer8In		\n\t" 								
		"ldr	r10,=0xCE160000		\n\t"		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
	    "mov 	r4,r4,lsl#8			\n\t" 								
    	"mov 	r5,r5,lsl#8			\n\t" 								
    	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r14!,{r0-r7}		\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov	r1,r1,lsl#8			\n\t"								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t"								 	
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r14!,{r0-r7}		\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r14!,{r0-r7}		\n\t" 		
      	"ldmia 	r10!,{r0-r7}		\n\t" 		
      	"mov 	r0,r0,lsl#8			\n\t" 								
      	"mov 	r1,r1,lsl#8			\n\t" 								
      	"mov 	r2,r2,lsl#8			\n\t" 								
      	"mov 	r3,r3,lsl#8			\n\t" 								
      	"mov 	r4,r4,lsl#8			\n\t" 								
      	"mov 	r5,r5,lsl#8			\n\t" 								
      	"mov 	r6,r6,lsl#8			\n\t" 								
      	"mov 	r7,r7,lsl#8			\n\t" 								
      	"stmia 	r14!,{r0-r7}		\n\t" 		
      	"sub 	r14,r14,#128		\n\t" 								
		"sub	r10,r10,#128		\n\t"		
      	"loop_4by4_hi:				\n\t"
      	"mixer_4by_4_hi_first_mul:	\n\t"
      	"mov 	r9,r14				\n\t" 										
      	"ldmia 	r8!,{r0-r3}			\n\t"
		"add	r8,r8,#16			\n\t"
      	"ldmia 	r9!,{r4-r7}			\n\t" 						
      	"smull 	r11,r12,r4,r0		\n\t" 								
      	"smlal 	r11,r12,r5,r1		\n\t" 								
      	"smlal 	r11,r12,r6,r2		\n\t" 								
      	"smlal 	r11,r12,r7,r3		\n\t" 								
		"movs 	r4,r12,asr#12		\n\t" 								
      	"beq skip1_4by4_hi			\n\t" 											
      	"mvns 	r5,r4				\n\t" 										
      	"beq skip1_4by4_hi			\n\t" 											
      	"movs 	r4,r12,asr#31		\n\t" 								
      	"moveq 	r4,#0x7f			\n\t" 									
      	"movmi 	r4,#0x80			\n\t" 									
      	"mov 	r4,r4,lsl#16		\n\t"								
      	"str 	r4,[r10],#4			\n\t" 								      	
      	"b mixer_4by_4_hi_second_mul\n\t"		
		"skip1_4by4_hi:				\n\t" 
      	"mov 	r4,r12,lsl#11		\n\t" 								
      	"mov 	r5,r11,lsr#21		\n\t" 								
      	"orr 	r6,r5,r4			\n\t" 									
      	"str 	r6,[r10],#4			\n\t"
      	"mixer_4by_4_hi_second_mul:	\n\t"
      	"ldmia 	r9!,{r4-r7}			\n\t" 						
      	"smull 	r11,r12,r4,r0		\n\t" 								
      	"smlal 	r11,r12,r5,r1		\n\t" 								
      	"smlal 	r11,r12,r6,r2		\n\t" 								
      	"smlal 	r11,r12,r7,r3		\n\t" 								
		"movs 	r4,r12,asr#12		\n\t" 								
      	"beq skip2_4by4_hi			\n\t" 											
      	"mvns 	r5,r4				\n\t" 										
      	"beq skip2_4by4_hi			\n\t" 											
      	"movs 	r4,r12,asr#31		\n\t" 								
      	"moveq 	r4,#0x7f			\n\t" 									
      	"movmi 	r4,#0x80			\n\t" 									
      	"mov 	r4,r4,lsl#16		\n\t"								
      	"str 	r4,[r10],#4			\n\t" 								
      	"subs 	r13,r13,#1			\n\t"
      	"bne loop_4by4_hi			\n\t"
		"beq mixer8_reintialize_4by4_hi\n\t"
		"skip2_4by4_hi:				\n\t" 
      	"mov 	r5,r12,lsl#11		\n\t" 								
      	"mov 	r4,r11,lsr#21		\n\t" 								
      	"orr 	r6,r5,r4			\n\t" 									
      	"str 	r6,[r10],#4			\n\t" 								
      	"subs 	r13,r13,#1			\n\t" 									
      	"bne loop_4by4_hi			\n\t"
		"mixer8_reintialize_4by4_hi:\n\t"		
      	"mov 	r13,#4				\n\t" 										
      	"add 	r14,r14,#32			\n\t"									
      	"sub 	r8,r8,#128			\n\t" 									
      	"ldr 	r1,=mixer8Count		\n\t" 									
      	"ldr 	r0,[r1]				\n\t" 										
      	"subs 	r0,r0,#1			\n\t" 									
      	"str 	r0,[r1]				\n\t" 
      	"bne loop_4by4_hi			\n\t"		
		"ldr	r0,=0xCE160100		\n\t"
		"ldr	r1,[r0]				\n\t"
		"tst	r1,#2				\n\t"
		"beq	skip_disable_4by4_hi\n\t"
		"ldr	r0,=mixer8Enable	\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8			\n\t"
		"str	r1,[r0]				\n\t"
		"skip_disable_4by4_hi:		\n\t"
		"ldr	r0,=0xCE160100		\n\t"
		"mov	r1,#0				\n\t"
		"str	r1,[r0]				\n\t"
		"ldr	r0,=mixer8SP		\n\t"	
		"ldr	sp,[r0]				\n\t"
		"ldmfd  sp!,{pc}			\n\t"
	);
}
#endif //_MIXER8


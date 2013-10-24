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
	
	Purpose:	DICE - CLI
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_DAL
#include "TCTasking.h"
#include "dal.h"
#include "dalRouter.h"
#include "dalRates.h"
#include "cli.h"


//create interface <rateMode> <inputDevices> <outputDevices>
//destroy interface
//start interface
//set clock source
//set route
//install text call back

//dump functions
//dump interface state, lock, rates etc.

#define NBNAMES 15
static char * deviceNames[NBNAMES] =
{
"aes","adat","tdif","dsai0","dsai1","dsai2","dsai3", "i2s0", "i2s1", "i2s2", "apb", "avs1","avs2","avs3","avs4"
};


static uint32 createDeviceList(char * str)
{
	uint32 devbits = 0;
	char * tok;
	uint8 i;
	
	tok = strtok (str," ,");
	while (tok)
	{
		//check tok
		for (i = 0; i < NBNAMES; i++)
		{
			if (strcmp(tok,deviceNames[i]) == 0)
			{
				devbits |= DEVICE_BIT(i);
				break;
			}
		}
		tok = strtok (0," ,");
	}
	return devbits;
}

static void dumpDeviceList (uint32 devices)
{
	uint8 i;
	for (i = 0; i < NBNAMES; i++)
	{
		if (devices & DEVICE_BIT(i))
			cliPrintf ("%s ",deviceNames[i]);
	}
}
	
static HRESULT create (uint32 domain, uint32 ratemode, char * inputDesc, char * outputDesc)
{
	uint32 inputDevices, outputDevices;
	
	inputDevices = createDeviceList(inputDesc);
	outputDevices = createDeviceList(outputDesc);
	return dalCreateInterface(domain, ratemode, inputDevices, outputDevices);
}

static  char	Create_UsageString[]=\
				"create: \n\r"\
				"use: dal:create <0|1> <rateMode> <""inputs""> <""outputs"">\n\r"\
				"------------------------------------------------------------\n\r"\
				"Create a clock domain: \n\r"\
				"<rateMode>: low, low_mid, mid, high, slave_l, slave_m, slave_h\n\r"\
				"<inputs>: ""aes adat ....""\n\r"\
				"<outputs>: ""aes adat ....""\n\r";

static	CLIDescriptor	Create_Descriptor[]=
{
	{kCLI_Name,				(uint32)"create"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&create},
	{kCLI_UsageString, 		(uint32)Create_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dkss"},
	{kCLI_ConstantsString,	(uint32)"low=0,low_mid=1,mid=2,high=3,slave_l=4,slave_m=5,slave_h=6"},
	{kCLI_ModuleName,		(uint32)"Create clock domain"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT destroy (uint32 domain)
{
	return dalDestroyInterface(domain);
}

static  char	Destroy_UsageString[]=\
				"destroy: \n\r"\
				"use: dal:destroy <0|1>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Destroy a clock domain: \n\r";
static	CLIDescriptor	Destroy_Descriptor[]=
{
	{kCLI_Name,				(uint32)"destroy"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&destroy},
	{kCLI_UsageString, 		(uint32)Destroy_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"d"},
	{kCLI_ModuleName,		(uint32)"Destroy clock domain"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT start (uint32 domain)
{
	return dalStartInterface(domain);
}

static  char	Start_UsageString[]=\
				"start: \n\r"\
				"use: dal:start <0|1>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Start a clock domain: \n\r";

static	CLIDescriptor	Start_Descriptor[]=
{
	{kCLI_Name,				(uint32)"start"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&start},
	{kCLI_UsageString, 		(uint32)Start_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"d"},
	{kCLI_ModuleName,		(uint32)"Start clock domain"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT setClock (uint32 domain, uint32 source, uint32 rate)
{
	return dalSetClockSource (domain, source, rate);
}

static  char	Clock_UsageString[]=\
				"clock: \n\r"\
				"use: dal:clock <0|1> <source> <rate>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Start a clock domain: \n\r"\
				"<source>: aesAny, aes0, aes1, aes2, aes3, adat, tdif, wc, avs1\n\r"\
				"          avs2, avs3, avs4, ext, dsai0, dsai1, dsai2, dsai3, int\r\n"\
				"<rate>:   32k, 44k1, 48k, 88k2, 96k, 176k4, 192k, any\r\n";

				
static	CLIDescriptor	Clock_Descriptor[]=
{
	{kCLI_Name,				(uint32)"clock"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&setClock},
	{kCLI_UsageString, 		(uint32)Clock_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dkk"},
	{kCLI_ConstantsString,	(uint32)"aesAny=0,aes0=1,aes1=2,aes2=3,aes3=4,\
adat=5,tdif=6,wc=7,avs1=8,avs2=9,avs3=10,avs4=11,ext=12,dsai0=13,dsai1=14,\
dsai2=15,dsai3=16,int=17,32k=0,44k1=1,48k=2,88k2=3,96k=4,176k4=5,192k=6,any=15"},
	{kCLI_ModuleName,		(uint32)"Set the source and rate"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT setRoute (uint32 domain, uint32 dstDev, uint32 dstCh, uint32 srcDev, uint32 srcCh, uint32 type)
{
	SRC_DESCRIPTION src;
	DEST_DESCRIPTION dst;
	
	src.bit_fields.moduleId = srcDev;
	src.bit_fields.channel = srcCh;
	src.bit_fields.type = type;
	
	dst.bit_fields.moduleId = dstDev;
	dst.bit_fields.channel = dstCh;
	dst.bit_fields.type = type;
	
	return dalSetRoute(domain, dst, src);
}

static  char	Route_UsageString[]=\
				"route: \n\r"\
				"use: dal:route <0|1> <dstDev> <dstCh> <srcDev> <srcCh> <type>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set a route: \n\r"\
				"<dstDev>: aes, adat, tdif, dsai0, dsai1, dsai2, dsai3, \r\n"\
				"          i2s0, i2s1, i2s2, apb, avs1, avs2\r\n"\
				"<dstCh> : 0..15\r\n"\
				"<srcDev>: aes, adat, tdif, dsai0, dsai1, dsai2, dsai3, \r\n"\
				"          i2s0, i2s1, i2s2, apb, avs1, avs2, avs3, avs4, mute\r\n"\
				"<dstCh> : 0..15\r\n"\
				"<type>  : t1, t2, t4, t8\r\n";

				
static	CLIDescriptor	Route_Descriptor[]=
{
	{kCLI_Name,				(uint32)"route"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&setRoute},
	{kCLI_UsageString, 		(uint32)Route_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dkdkdk"},
	{kCLI_ConstantsString,	(uint32) "aes=0,adat=1,tdif=2,\
dsai0=3,dsai1=4,dsai2=5,dsai3=6,i2s0=7,i2s1=8,i2s2=9,apb=10,\
avs1=11,avs2=12,avs3=13,avs4=14,mute=15,\
t1=0,t2=1,t4=2,t8=3"},
	{kCLI_ModuleName,		(uint32)"Set a routing"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT eventCB (DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data)
{
	
	if (pExtEvents->fsm_events & DAL_CB_DOM1_MASK)	
		cliPrintf ("DOMAIN1: 0x%02x\n\r",pExtEvents->fsm_events & DAL_CB_DOM1_MASK);
	if (pExtEvents->fsm_events & DAL_CB_DOM2_MASK)	
		cliPrintf ("DOMAIN2: 0x%02x\n\r",(pExtEvents->fsm_events & DAL_CB_DOM2_MASK) >> 8);
	
	if (pExtEvents->aes_events & DAL_CB_AES0_MASK)
		cliPrintf ("AES0: 0x%02x\n\r",(pExtEvents->aes_events & DAL_CB_AES0_MASK));
	if (pExtEvents->aes_events & DAL_CB_AES1_MASK)
		cliPrintf ("AES1: 0x%02x\n\r",(pExtEvents->aes_events & DAL_CB_AES1_MASK) >> 8);
	if (pExtEvents->aes_events & DAL_CB_AES2_MASK)
		cliPrintf ("AES2: 0x%02x\n\r",(pExtEvents->aes_events & DAL_CB_AES2_MASK) >> 16);
	if (pExtEvents->aes_events & DAL_CB_AES3_MASK)
		cliPrintf ("AES3: 0x%02x\n\r",(pExtEvents->aes_events & DAL_CB_AES3_MASK) >> 24);
	
	if (pExtEvents->adat_events)
		cliPrintf ("ADAT: 0x%04x\n\r",(pExtEvents->adat_events));
	
	if (pExtEvents->tdif_events)
		cliPrintf ("TDIF: 0x%04x\n\r",(pExtEvents->tdif_events));
		
	if (pExtEvents->avs_events & DAL_CB_AVS1_MASK)
		cliPrintf ("AVS1: 0x%02x\n\r",(pExtEvents->avs_events & DAL_CB_AVS1_MASK));
	if (pExtEvents->avs_events & DAL_CB_AVS2_MASK)
		cliPrintf ("AVS2: 0x%02x\n\r",(pExtEvents->avs_events & DAL_CB_AVS2_MASK) >> 8);
	if (pExtEvents->avs_events & DAL_CB_AVS3_MASK)
		cliPrintf ("AVS3: 0x%02x\n\r",(pExtEvents->avs_events & DAL_CB_AVS3_MASK) >> 16);
	if (pExtEvents->avs_events & DAL_CB_AVS4_MASK)
		cliPrintf ("AVS4: 0x%02x\n\r",(pExtEvents->avs_events & DAL_CB_AVS4_MASK) >> 24);
	
	if (pExtEvents->dsai_events & DAL_CB_DSAI0_MASK)
		cliPrintf ("DSAI0: 0x%01x\n\r",(pExtEvents->dsai_events & DAL_CB_DSAI0_MASK));
	if (pExtEvents->dsai_events & DAL_CB_DSAI1_MASK)
		cliPrintf ("DSAI1: 0x%01x\n\r",(pExtEvents->dsai_events & DAL_CB_DSAI1_MASK) >> 4);
	if (pExtEvents->dsai_events & DAL_CB_DSAI2_MASK)
		cliPrintf ("DSAI2: 0x%01x\n\r",(pExtEvents->dsai_events & DAL_CB_DSAI2_MASK) >> 8);
	if (pExtEvents->dsai_events & DAL_CB_DSAI3_MASK)
		cliPrintf ("DSAI3: 0x%01x\n\r",(pExtEvents->dsai_events & DAL_CB_DSAI3_MASK) >> 12);
	
	return NO_ERROR;
}


static HRESULT installCB (void)
{
	DAL_EVENTS events;
	//let's get notified on all events
	events.fsm_events = 0xffffffff;
	events.aes_events = 0xffffffff;
	events.adat_events = 0xffffffff;
	events.tdif_events = 0xffffffff;
	events.dsai_events = 0xffffffff;
	events.avs_events = 0xffffffff;
	
	return dalInstallCallback  (&events, 300, eventCB, 0);
}

static  char	InstallCB_UsageString[]=\
				"installCB: \n\r"\
				"use: dal:installCB\n\r"\
				"------------------------------------------------------------\n\r"\
				"Install verbose callback for DAL: \n\r";

static	CLIDescriptor	InstallCB_Descriptor[]=
{
	{kCLI_Name,				(uint32)"installCB"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&installCB},
	{kCLI_UsageString, 		(uint32)InstallCB_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"Install Call Back"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT removeCB (void)
{
	return dalUninstallCallback (eventCB);
}

static  char	RemoveCB_UsageString[]=\
				"removeCB: \n\r"\
				"use: dal:removeCB\n\r"\
				"------------------------------------------------------------\n\r"\
				"Remove verbose callback for DAL: \n\r";

static	CLIDescriptor	RemoveCB_Descriptor[]=
{
	{kCLI_Name,				(uint32)"removeCB"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&removeCB},
	{kCLI_UsageString, 		(uint32)RemoveCB_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"Remove Call Back"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static char * rateModes[] =
{
	"RATE_MODE_LOW",
	"RATE_MODE_LOW_MID",
	"RATE_MODE_MID",
	"RATE_MODE_HIGH",
	"RATE_MODE_SLAVE_LOW",
	"RATE_MODE_SLAVE_MID",
	"RATE_MODE_SLAVE_HIGH"
};

static char * nominalRates[] =
{
	"NOMINAL_RATE_32",		      
	"NOMINAL_RATE_44_1",		    
	"NOMINAL_RATE_48",
	"NOMINAL_RATE_88_2",		  
	"NOMINAL_RATE_96",		    
	"NOMINAL_RATE_176_4",	  
	"NOMINAL_RATE_192", 
	"NOMINAL_RATE_USER_1",
	"NOMINAL_RATE_USER_2",
	"NOMINAL_RATE_USER_3",			
	"NOMINAL_RATE_USER_4",			
	"NOMINAL_RATE_USER_5",			
	"NOMINAL_RATE_USER_6",			
	"NOMINAL_RATE_USER_7",
	"NOMINAL_RATE_USER_8",
	"NOMINAL_RATE_ANY",
	"NOMINAL_RATE_NONE"	
};

static char * clockSources[] =
{
	"CLK_SRC_AESRX",
	"CLK_SRC_AESRX0",
	"CLK_SRC_AESRX1",
	"CLK_SRC_AESRX2",
	"CLK_SRC_AESRX3",
	"CLK_SRC_ADATRX",
	"CLK_SRC_TDIFRX",
	"CLK_SRC_WORD",
	"CLK_SRC_AVS_SYT1",
	"CLK_SRC_AVS_SYT2",
	"CLK_SRC_AVS_SYT3",
	"CLK_SRC_AVS_SYT4",
	"CLK_SRC_EXTERNAL",
	"CLK_SRC_DSAI_RX0",
	"CLK_SRC_DSAI_RX1",
	"CLK_SRC_DSAI_RX2",
	"CLK_SRC_DSAI_RX3",
	"CLK_SRC_INTERNAL",
	"CLK_SRC_NONE"
} ;


static HRESULT dumpDomain (uint32 domain)
{
	HRESULT hResult;
	
	DAL_CONFIG config;
	DAL_STATUS status;
	
	hResult = dalGetCurrentStatus  (domain, &status);
	if (hResult != NO_ERROR) return hResult;
	hResult = dalGetCurrentConfiguration  (domain, &config);
	if (hResult != NO_ERROR) return hResult;
	
	cliPrintf("DOMAIN %i Configuration\n\r", domain);
	cliPrintf("\trateMode    : %s\n\r",rateModes[config.rateMode]);
	cliPrintf("\tclockSource : %s\n\r",clockSources[config.clockSource]);	
	cliPrintf("\tnominalRate : %s\n\r",nominalRates[config.sampleRate]);
	cliPrintf("\tinputDev.   : ");
	dumpDeviceList (config.inputDevices);
	cliPrintf("\n\r");
	cliPrintf("\toutputDev.  : ");
	dumpDeviceList (config.outputDevices);
	cliPrintf("\n\r");
	cliPrintf("DOMAIN %i Status\n\r", domain);	
	cliPrintf("\tstate       : %s\n\r",dalStateNames[status.state]);
	cliPrintf("\trateMode    : %s\n\r",rateModes[status.lockedRateMode]);
	cliPrintf("\tnominalRate : %s\n\r",nominalRates[status.lockedNominalRate]);
	cliPrintf("\trateHz      : %i\n\r",status.lockedRateHz);
	cliPrintf("\tbadRate     : %i\n\r",status.badRateHz);
	return hResult;
}

static HRESULT dumpSync (uint32 domain)
{
	HRESULT hResult;
	
	DAL_SYNC_SRC_INFO info;
	
	hResult = dalGetSyncSrcInfo (domain, &info);
	if (hResult != NO_ERROR) return hResult;
	
	cliPrintf("DOMAIN %i Sync Source Info\n\r", domain);
	cliPrintf("\tsrc         : %s\n\r",clockSources[info.src]);
	cliPrintf("\tstate       : %s\n\r",info.locked?"Locked":"Unlocked");	
	cliPrintf("\tnominalRate : %s\n\r",nominalRates[info.nomRate]);
	cliPrintf("\tAdat User   : U3=%c U2=%c U1=%c U0=%c\r\n",(info.adatUser&8)?'1':'0',
															(info.adatUser&4)?'1':'0',
															(info.adatUser&2)?'1':'0',
															(info.adatUser&1)?'1':'0');
	cliPrintf("\n\r");
	return hResult;
}




static  char	Dump_UsageString[]=\
				"dump: \n\r"\
				"use: dal:dump <0|1>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump configuration and status for a clock domain: \n\r";
				
static	CLIDescriptor	Dump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dumpDomain},
	{kCLI_UsageString, 		(uint32)Dump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"d"},
	{kCLI_ModuleName,		(uint32)"Dump domain"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	DumpSync_UsageString[]=\
				"dumpsync: \n\r"\
				"use: dal:dumpsync <0|1>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump Sync Source Status: \n\r";
				
static	CLIDescriptor	DumpSync_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dumpsync"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dumpSync},
	{kCLI_UsageString, 		(uint32)DumpSync_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"d"},
	{kCLI_ModuleName,		(uint32)"Dump Sync Source"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT setConfig (uint32 msWaitStable, uint32 msWaitLock, uint32 bPLLoffWhenNoSource)
{
	return dalSetConfig (msWaitStable, msWaitLock, bPLLoffWhenNoSource);
}

static  char	Config_UsageString[]=\
				"config: \n\r"\
				"use: dal:config <msWaitStable> <msWaitLock> <PLLoffWhenNoSource>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set JET PLL configuration: \n\r"\
				"<msWaitStable>: wait for the rate to stabelize, default 100 ms \n\r"\
				"<msWaitLock>: timeout on JET PLL lock, default 800 ms \n\r"\
				"<PLLoffWhenNoSource>: Turn clocks off when unlocked, on/off \n\r";
				
static	CLIDescriptor	Config_Descriptor[]=
{
	{kCLI_Name,				(uint32)"config"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&setConfig},
	{kCLI_UsageString, 		(uint32)Config_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ddk"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Set Configuration"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static HRESULT AVSPhase_Tool (uint32 avsId)
{
	HRESULT hResult;
	int8 phasePercent;
	int32 phase10us;
	char sign=' ';
	int32 msd,lsd;
	
	hResult = dalGetAVSPhaseOffset (avsId, &phase10us, &phasePercent);
	if (hResult == NO_ERROR)
	{
		if (phase10us < 0)
		{
			sign = '-';
			phase10us = -phase10us;
		}
		msd = phase10us/10;
		lsd = phase10us - 10*msd;
		cliPrintf("Phase offset %c%i.%ius, %i%%\n\r", sign,msd,lsd,phasePercent);	
		
	}
	return hResult;
}

static  char	AVSPhase_UsageString[]=\
				"avsphase: \n\r"\
				"use: dal:avsphase <avs1|avs2>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get the phase offset of AVS receiver in relation to main clock domain\n\r";
static	CLIDescriptor	AVSPhase_Descriptor[]=
{
	{kCLI_Name,				(uint32)"avsphase"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&AVSPhase_Tool},
	{kCLI_UsageString, 		(uint32)AVSPhase_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"avs1=0,avs2=1"},
	{kCLI_ModuleName,		(uint32)"Check the AVS Phase"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	SkewRates_UsageString[]=\
				"skew: \n\r"\
				"use: dal:skew <tenth percent>\n\r"\
				"------------------------------------------------------------\n\r"\
				"All the internal rates will be skewed by the signed percentage specified\n\r";
static	CLIDescriptor	SkewRates_Descriptor[]=
{
	{kCLI_Name,				(uint32)"skew"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dalSkewRates},
	{kCLI_UsageString, 		(uint32)SkewRates_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Skew Internal Rates"},
	{kCLI_GroupName,		(uint32)"dal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void dalCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(Create_Descriptor);
	hResult += cliInstallCLIDescriptor(Destroy_Descriptor);
	hResult += cliInstallCLIDescriptor(Start_Descriptor);
	hResult += cliInstallCLIDescriptor(Clock_Descriptor);
	hResult += cliInstallCLIDescriptor(Route_Descriptor);
	hResult += cliInstallCLIDescriptor(InstallCB_Descriptor);
	hResult += cliInstallCLIDescriptor(RemoveCB_Descriptor);
	hResult += cliInstallCLIDescriptor(Dump_Descriptor);
	hResult += cliInstallCLIDescriptor(DumpSync_Descriptor);
	hResult += cliInstallCLIDescriptor(Config_Descriptor);
	hResult += cliInstallCLIDescriptor(AVSPhase_Descriptor);
	hResult += cliInstallCLIDescriptor(SkewRates_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing dal cli tools");
	}
}

#endif //_CLI_TOOL_DAL
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

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

#ifndef _CMSAVSONLY

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_DAL
#include "TCTasking.h"
#include "cms.h"
#include "cmsAvs.h"
#include "cmsInterface.h"
#include "cli.h"
#include "irm.h"


static char 	moduleName[] = __FILE__ ;

//set commands
//stx <id> <isoc> <acfg> <ccfg>
//srx <id> <isoc> <achs> <cchs>
//srm <ratemode>
//ssy <sync>  //const or number
//sni <nickname>
//sco <id> <N81>
//sru <dstId> <srcId> <nbchnls>
//
//dump commands
//dump <rx, tx, route, general, all>

static HRESULT stx(uint8 txID, uint8 isoc, uint8 acfg, uint8 ccfg)
{
	CMS_TX_PAR cfg;
	HRESULT hRes = NO_ERROR;
	
	if (isoc < 63)
		cfg.isocCh = isoc;
	else if (isoc == CMS_ISOC_DISABLE-1)
	{	
		//choose free
		uint32 newIsoc;
		hRes = irmFindChannelAvailable(&newIsoc);
		if (hRes != NO_ERROR) return hRes;
		cfg.isocCh = (uint8)newIsoc;
	}
	else
		cfg.isocCh = CMS_ISOC_DISABLE;
			
	cfg.audioCfg = acfg;
	cfg.commCfg = ccfg;
	cfg.reserved = 0;
	
	return cmsSetTx (txID, &cfg);
}

static HRESULT srx(uint8 rxID, uint8 isoc, uint8 achs, uint8 cchs)
{
	CMS_RX_PAR cfg;
	
	if (isoc < 63)
		cfg.isocCh = isoc;
	else
		cfg.isocCh = CMS_ISOC_DISABLE;
			
	cfg.audioChs = achs;
	cfg.commChs = cchs;
	cfg.reserved = 0;
	
	return cmsSetRx (rxID, &cfg);
}

static HRESULT srm(uint32 rateMode)
{
	return cmsSetRateMode (rateMode);
}

static HRESULT ssy (uint32 syncSource)
{
	return cmsSetSync (syncSource);
}

static HRESULT sni (char * nickName)
{
	return cmsSetNickName (nickName);
}

static HRESULT sru (uint8 dstId, uint8 srcPlug, uint8 srcCh, uint8 nbChannels)
{
	uint32 route;
	if (srcPlug >= NUM_AVS_RX)
	{
		sysLogError(E_GEN_ILLEGAL_PARAM, __LINE__, moduleName);
		return E_GEN_ILLEGAL_PARAM;			
	}
	while (nbChannels--)
	{
		route = ((DEV_AVS_ADO1+srcPlug)<<4) | (srcCh & 0xf);
		setOutputRoute (&route, dstId, dstId+1);
		srcCh++;
		dstId++;
	}
	return NO_ERROR;
}

static HRESULT ind (uint32 cnt)
{
	return cmsIndicate (cnt);
}

/////// The dump function
//<rx, tx, route, general, all>
static void dumprx (void)
{
	uint8 rxID;
	CMS_RX_STAT * pStat;
	CMS_RX_PAR * pPar;
	
	cliPrintf("Rx CONFIGURATION\n\r");
	
	for (rxID=0; rxID<NUM_AVS_RX; rxID++)
	{
		pStat = cmsGetRxStat(rxID);
		pPar = cmsGetRxPar(rxID);
		cliPrintf(" RxPlug%-2i : Isoc=",rxID);
		if (pPar->isocCh == CMS_ISOC_DISABLE)
			cliPrintf("OFF ");
		else
			cliPrintf("%-4i",pPar->isocCh);
		cliPrintf("audioChs=%-3i",pPar->audioChs);
		cliPrintf("commPorts=%-3i",pPar->commChs);
		cliPrintf("stat=%s\r\n",(pStat->state&1)?"LOCKED":"UNLOCKED");
	}
	cliPrintf("\r\n");
}

static void dumptx (void)
{
	uint8 txID;
	CMS_TX_STAT * pStat;
	CMS_TX_PAR * pPar;
	
	cliPrintf("Tx CONFIGURATION\n\r");
	
	for (txID=0; txID<NUM_AVS_TX; txID++)
	{
		pStat = cmsGetTxStat(txID);
		pPar = cmsGetTxPar(txID);
		cliPrintf(" TxPlug%-2i : Isoc=",txID);
		if (pPar->isocCh == CMS_ISOC_DISABLE)
			cliPrintf("OFF ");
		else
			cliPrintf("%-4i",pPar->isocCh);
		cliPrintf("audioCfg=#%-2i ",pPar->audioCfg,pStat->audioChs);
		cliPrintf("commCfg=%-2i ",pPar->commCfg,pStat->commChs);
		switch (pStat->state)
		{
			case CMS_ISO_DISABLED:	cliPrintf("stat=DISABLED  ");break;
			case CMS_ISO_STRM:		cliPrintf("stat=STREAMING ");break;
			case CMS_ISO_NO_CH:		cliPrintf("stat=NO CHNL   ");break;
			case CMS_ISO_NO_BW:		cliPrintf("stat=NO BW     ");break;
			default:                cliPrintf("stat=ILLEGAL   ");break;
		}
		cliPrintf("%s\r\n",cmsGetAudioModeName(pPar->audioCfg));
	}
	cliPrintf("\r\n");
}

static void dumpglobal (void)
{
	CMS_GLOBAL_PAR * pPar = cmsGetGlobalPar ();
	cliPrintf("General CONFIGURATION\n\r");
	cliPrintf(" NickName: %-*s\r\n",CMS_NICKNAME_MAX,pPar->nickname);
	cliPrintf(" RateMode: ");

	switch (pPar->rateMode)
	{
		case CMS_RATE_32K_48K:  cliPrintf("32k-48k\r\n");break;
		case CMS_RATE_88K2_96K:  cliPrintf("88.2k-96k\r\n");break;
		case CMS_RATE_176K4_192K: cliPrintf("176.4k-192k\r\n");break;
		default: cliPrintf("ILLEGAL\r\n");break;
	}
	cliPrintf(" Sync:     ");
	switch (pPar->syncSource)
	{
		case CMS_SYNC_SRC_INT32K:	cliPrintf("Int 32k");break;
		case CMS_SYNC_SRC_INT44K1:	cliPrintf("Int 44k1");break;
		case CMS_SYNC_SRC_INT48K:	cliPrintf("Int 48k");break;
		case CMS_SYNC_SRC_INT88K2:	cliPrintf("Int 88k2");break;
		case CMS_SYNC_SRC_INT96K:	cliPrintf("Int 96k");break;
		case CMS_SYNC_SRC_INT176K4:	cliPrintf("Int 176k4");break;
		case CMS_SYNC_SRC_INT192K:	cliPrintf("Int 192k");break;
		case CMS_SYNC_SRC_EXT+0:	cliPrintf("AES (any)");break;
		case CMS_SYNC_SRC_EXT+1:	cliPrintf("AES12");break;
		case CMS_SYNC_SRC_EXT+2:	cliPrintf("AES34");break;
		case CMS_SYNC_SRC_EXT+3:	cliPrintf("AES56");break;
		case CMS_SYNC_SRC_EXT+4:	cliPrintf("AES78");break;
		case CMS_SYNC_SRC_EXT+5:	cliPrintf("ADAT");break;
		case CMS_SYNC_SRC_EXT+6:	cliPrintf("ADAT AUX");break;
		case CMS_SYNC_SRC_EXT+7:	cliPrintf("WORD CLOCK");break;
		
		case CMS_SYNC_SRC_SYT+0:	cliPrintf("SYT Plug0");break;
		case CMS_SYNC_SRC_SYT+1:	cliPrintf("SYT Plug1");break;
		case CMS_SYNC_SRC_SYT+2:	cliPrintf("SYT Plug2");break;
		case CMS_SYNC_SRC_SYT+3:	cliPrintf("SYT Plug3");break;
		
		default:					cliPrintf("ILLEGAL");break;
	}
	cliPrintf("\r\n\r\n");
}

static void dumproute (void)
{
	uint8 id, chs, rt, dev;
	
	cliPrintf("Output Routing\n\r");
	
	chs = cmsGetnbOutputs();
	for (id=0; id<chs; id++)
	{
		cliPrintf(" Out%-2i ",id);
		cliPrintf("%-16s from ",cmsGetAudioOutputName(id));
		rt = cmsGetOutputsRoute (id);
		dev = (rt>>4) & 0xf;
		if ((dev >= DEV_AVS_ADO1) && (dev < DEV_AVS_ADO1+NUM_AVS_RX))
		{
			cliPrintf("Plug%i ",dev-DEV_AVS_ADO1);
			cliPrintf("Ch%i\r\n",rt&0xf);
		}
		else if (dev==0xf)
		{
			cliPrintf("Muted \r\n",dev);
		}
		else
		{
			cliPrintf("Dev%02X ",dev);
			cliPrintf("Ch%i\r\n",rt&0xf);
		}
		
	}
	cliPrintf("\r\n");
}

static void dumpmodes (void)
{
	uint8 mode,modes;
	cliPrintf("Defined Tx Audio Modes\n\r");
	modes=cmsGetnbModes();
	for (mode=0; mode < modes; mode++)
	{
		cliPrintf(" %-2i %s\r\n",mode,cmsGetAudioModeName(mode));	
	}
	cliPrintf("\r\n");
}

static HRESULT dump (uint32 mode)
{
	switch (mode)
	{
		case 0:	dumprx();break;
		case 1:	dumptx();break;
		case 2:	dumproute();break;
		case 3:	dumpglobal();break;
		case 4:	
				dumprx();
				dumptx();
				dumproute();
				dumpglobal();
				break;
		case 5:	dumpmodes();break;
	}
	return NO_ERROR;
}




static  char	stx_UsageString[]=\
				"stx: \n\r"\
				"use: cms:stx <plug> <isoc> <acfg> <ccfg>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Configure transmit plug: \n\r"\
				"<plug>: 0..1\n\r"\
				"<isoc>: 0..62, auto,off\n\r"\
				"<acfg>: 0..? a valid audio configuration id\n\r"\
				"<ccfg>: 0..? a valid comm configuration id\n\r";

static	CLIDescriptor	stx_Descriptor[]=
{
	{kCLI_Name,				(uint32)"stx"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&stx},
	{kCLI_UsageString, 		(uint32)stx_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"didd"},
	{kCLI_ConstantsString,	(uint32)"off=255,auto=254"},
	{kCLI_ModuleName,		(uint32)"Configure Tx Plug"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	srx_UsageString[]=\
				"srx: \n\r"\
				"use: cms:srx <plug> <isoc> <achs> <cchs>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Configure receive plug: \n\r"\
				"<plug>: 0..3\n\r"\
				"<isoc>: 0..62, off\n\r"\
				"<acfg>: 0..16 number of audio channels expected\n\r"\
				"<ccfg>: 0..8 number of comm ports expected\n\r";

static	CLIDescriptor	srx_Descriptor[]=
{
	{kCLI_Name,				(uint32)"srx"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&srx},
	{kCLI_UsageString, 		(uint32)srx_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"didd"},
	{kCLI_ConstantsString,	(uint32)"off=255"},
	{kCLI_ModuleName,		(uint32)"Configure Rx Plug"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	srm_UsageString[]=\
				"srm: \n\r"\
				"use: cms:srm <rateMode>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set system rate mode: \n\r"\
				"<rateMode>: low,mid,high\n\r"\
				"  low  = 32k-48k\n\r"\
				"  mid  = 88.2k-96k\n\r"\
				"  high = 176.4k-192k\n\r";

static	CLIDescriptor	srm_Descriptor[]=
{
	{kCLI_Name,				(uint32)"srm"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&srm},
	{kCLI_UsageString, 		(uint32)srm_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"low=0,mid=2,high=3"},
	{kCLI_ModuleName,		(uint32)"Set Rate Mode"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	ssy_UsageString[]=\
				"ssy: \n\r"\
				"use: cms:ssy <sync>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set system sync: \n\r"\
				"<sync>: 32k,44k1,48k,88k2,96k,176k4,192k,\n\r"\
				"        aesAny,aes12,aes34,aes56,aes78,\n\r"\
				"        adat,adataux,wc,plg0,plg1,plg2,plg3\n\r";

static	CLIDescriptor	ssy_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ssy"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&ssy},
	{kCLI_UsageString, 		(uint32)ssy_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"32k=0,44k1=1,48k=2,88k2=3,96k=4,176k4=5,192k=6,aesAny=16,aes12=17,aes34=18,aes56=19,aes78=20,\
adat=21,adataux=22,wc=23,plg0=32,plg1=33,plg2=34,plg3=35"},
	{kCLI_ModuleName,		(uint32)"Set sync"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static  char	sni_UsageString[]=\
				"sni: \n\r"\
				"use: cms:sni \"<nickname>\"\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set Nickname: \n\r"\
				"<nickname>: Name enclosed in double quotes\n\r";

static	CLIDescriptor	sni_Descriptor[]=
{
	{kCLI_Name,				(uint32)"sni"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sni},
	{kCLI_UsageString, 		(uint32)sni_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"s"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set Nickname"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	sru_UsageString[]=\
				"sru: \n\r"\
				"use: cms:sru <dstId> <srcPlug> <srcCh> <nbChs>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set Output Route: \n\r"\
				"<dstId>   : 0..? id of device output\n\r"\
				"<srcPlug> : 0..3 source plug\n\r"\
				"<srcCh>   : 0..15 channel from src plug\n\r"\
				"<nbChs>   : 1..15 channels to route\n\r";

static	CLIDescriptor	sru_Descriptor[]=
{
	{kCLI_Name,				(uint32)"sru"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sru},
	{kCLI_UsageString, 		(uint32)sru_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiii"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set Output Route"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	ind_UsageString[]=\
				"ind: \n\r"\
				"use: cms:ind <count>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Indicate (blink LED etc): \n\r"\
				"<count>   : 0..15 length of indicate seq.(4=1s)\n\r";

static	CLIDescriptor	ind_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ind"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&ind},
	{kCLI_UsageString, 		(uint32)ind_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Indicate"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dump_UsageString[]=\
				"dump: \n\r"\
				"use: cms:dump <mode>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set Output Route: \n\r"\
				"<mode> : rx,tx,rou,glo,all,mod\n\r";

static	CLIDescriptor	dump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dump},
	{kCLI_UsageString, 		(uint32)dump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1,rou=2,glo=3,all=4,mod=5"},
	{kCLI_ModuleName,		(uint32)"dump"},
	{kCLI_GroupName,		(uint32)"cms"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


void cmsCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(stx_Descriptor);
	hResult += cliInstallCLIDescriptor(srx_Descriptor);
	hResult += cliInstallCLIDescriptor(srm_Descriptor);
	hResult += cliInstallCLIDescriptor(ssy_Descriptor);
	hResult += cliInstallCLIDescriptor(sni_Descriptor);
	hResult += cliInstallCLIDescriptor(sru_Descriptor);
	hResult += cliInstallCLIDescriptor(ind_Descriptor);
	hResult += cliInstallCLIDescriptor(dump_Descriptor);
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

#endif // _CMSAVSONLY

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
 * DESCRIPTION: Link Hardware Abstraction Layer Command Line Interface

		lhl CLI Tools and Variables
	
		These are used by the CLI to view lhl info.

	Revisions:
		created 05/14/2002 lm

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "lhl.h"
#include "lhlBri.h"
#include "lhlCli.h"
#include "lhlLLCCli.h"
#include "lhlPHY.h"
#include "lhlStatistics.h"
#include "lal.h"
#include "cli.h"
#include "bri.h"
#include "lhlATx.h"

static char 		moduleName[] = __FILE__ ;

/*
	print tCode
*/
HRESULT lhlCliPrintTCode(uint32 tCode)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlLLCCliPrintTCode(tCode);

	return hResult;
}

HRESULT lhlCli1394PrintTCode(uint32 tCode) // must be called from lhlLLCCliPrintTCode (for handling LLC specific cases)
{
	HRESULT		hResult = NO_ERROR;

	switch (tCode)
	{
		case TCODE_WRQUAD:
			cliPrintf("wr quad req");
			break;

		case TCODE_WRBLOCK:
			cliPrintf("wr block req");
			break;

		case TCODE_WRRESPONSE:
			cliPrintf("wr resp");
			break;

		case TCODE_RDQUAD:
			cliPrintf("rd quad req");
			break;

		case TCODE_RDBLOCK:
			cliPrintf("rd block req");
			break;

		case TCODE_RDQUADRSP:
			cliPrintf("rd quad resp");
			break;

		case TCODE_RDBLOCKRSP:
			cliPrintf("rd block resp");
			break;

		case TCODE_LOCKRQST:
			cliPrintf("lock req");
			break;

		case TCODE_ISOBLOCK:
			cliPrintf("iso block");
			break;

		case TCODE_LOCKRSP:
			cliPrintf("lock resp");
			break;

		case TCODE_RESERVED_4:						// used internally as generic type for TCODE_WRQUAD and TCODE_WRBLOCK 
			cliPrintf("wr quad/block req");			// for associating request to shared TCODE_WRRESPONSE
			break;
		
		default:
			cliPrintf("unknown tcode: %#x", tCode);
			hResult = E_FAIL;
			break;
	}

	return hResult;
}

/*
	print confirmation code
*/

HRESULT lhlCliPrintConfCode(uint32 conf)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlLLCCliPrintConfCode(conf);

	return hResult;
}

HRESULT lhlCli1394PrintConfCode(uint32 conf) // must be called from lhlLLCCliPrintConfCode (for handling LLC specific cases)
{
	HRESULT		hResult = NO_ERROR;

	switch (conf)
	{
		case ACK_1394_RESERVED_0:
			cliPrintf("reserved in 1394a");
			break;

		case ACK_1394_COMPLETE:
			cliPrintf("ack complete");
			break;

		case ACK_1394_PENDING:
			cliPrintf("ack pending");
			break;

		case ACK_1394_RESERVED_3:
			cliPrintf("reserved in 1394a");
			break;

		case ACK_1394_BUSY_X:
			cliPrintf("timeout busy_x (complete)");
			break;

		case ACK_1394_BUSY_A:
			cliPrintf("timeout busy_a (complete)");
			break;

		case ACK_1394_BUSY_B:
			cliPrintf("timeout busy_b (complete)");
			break;

		case ACK_1394_RESERVED_7:
			cliPrintf("reserved in 1394a");
			break;

		case ACK_1394_RESERVED_8:
			cliPrintf("reserved in 1394a");
			break;

		case ACK_1394_RESERVED_9:
			cliPrintf("reserved in 1394a");
			break;

		case ACK_1394_RESERVED_A:
			cliPrintf("reserved in 1394a");
			break;

		case ACK_1394_TARDY:
			cliPrintf("tardy");
			break;

		case ACK_1394_CONFLICT_ERROR:
			cliPrintf("conflict error (complete)");
			break;

		case ACK_1394_DATA_ERROR:
			cliPrintf("data error (complete)");
			break;

		case ACK_1394_TYPE_ERROR:
			cliPrintf("type error (complete)");
			break;

		case ACK_1394_ADDRESS_ERROR:
			cliPrintf("target address error");
			break;

		default:
			hResult = E_LHL_UNKNOWN_CONF;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	return hResult;
}

#ifdef _CLI_TOOL_LHL

/*
	BusReset
*/
static HRESULT lhlCliForceBusReset(void)
{
	HRESULT		hResult = NO_ERROR;

	cliOutputLine ("Force Bus Reset");
	
	hResult = lhlBriForceBusReset();

	return hResult;
}

static  char	lhlCliForceBusReset_UsageString[]=\
				"lhl:br: \n\r"\
				"use: lhl:br\n\r"\
				"------------------------------------------------------------\n\r"\
				"initiate a bus reset";
static	CLIDescriptor	lhlCliForceBusReset_Descriptor[]=
{
	{kCLI_Name,				(uint32)"br"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lhlCliForceBusReset},
	{kCLI_UsageString, 		(uint32)lhlCliForceBusReset_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"initiate a bus reset"},
	{kCLI_GroupName,		(uint32)"lhl"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Link Force Root Tool
*/
static HRESULT lhlCliForceRoot(void)
{
	HRESULT		hResult = NO_ERROR;

	cliOutputLine ("Force Root");
	
	hResult = lhlBriForceRoot();

	return hResult;
}

static  char	lhlCliForceRoot_UsageString[]=\
				"lhl:lfr: \n\r"\
				"use: lhl:lfr \n\r"\
				"------------------------------------------------------------\n\r"\
				"initiate a bus reset with force root bit set for this node";
static	CLIDescriptor	lhlCliForceRoot_Descriptor[]=
{
	{kCLI_Name,				(uint32)"lfr"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lhlCliForceRoot},
	{kCLI_UsageString, 		(uint32)lhlCliForceRoot_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"link force root"},
	{kCLI_GroupName,		(uint32)"lhl"},
	{kCLI_ConstantsString,	(uint32)""},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Phy Read Tool
*/
static HRESULT lhlCliPhyRead(uint32 address)
{
	HRESULT		hResult = NO_ERROR;
	uint8		i;
	uint32		value;
	char		tempstring[30];

	if (address == 0xff)
	{
		for (i=0; i<16; i++)
		{
			hResult = lhlReadPhy(i, &value, TRUE);

			if (hResult == NO_ERROR)
			{
				sprintf(tempstring, "Phy reg: 0x%1X = 0x%02X", i, value&0xff);
				cliOutputLine (tempstring);
			}
		}
	}
	else if (((int32)address >= 0) && (address < 16))
	{
		hResult = lhlReadPhy(address, &value, TRUE);

		if (hResult == NO_ERROR)
		{
			sprintf(tempstring, "Phy reg: 0x%1X = 0x%02X", address, value&0xff);
			cliOutputLine (tempstring);
		}
	}
	return hResult;
}




static  char	lhlCliPhyRead_UsageString[]=\
				"lhl:phy: \n\r"\
				"use: lhl:phy <addr|'all'>\n\r"\
				"------------------------------------------------------------\n\r"\
				"display one or all Phy register settings";
static	CLIDescriptor	lhlCliPhyRead_Descriptor[]=
{
	{kCLI_Name,				(uint32)"phy"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lhlCliPhyRead},
	{kCLI_UsageString, 		(uint32)lhlCliPhyRead_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"phy register read"},
	{kCLI_GroupName,		(uint32)"lhl"},
	{kCLI_ConstantsString,	(uint32)"all=255"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT lhlCliPhyWrite(uint32 address, uint32 val)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlWritePhy(address & 0x0f, val & 0xff);
	
	return hResult;
}

static  char	lhlCliPhyWrite_UsageString[]=\
				"lhl:phywr: \n\r"\
				"use: lhl:phywr <addr> <val>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Write a phy register";
static	CLIDescriptor	lhlCliPhyWrite_Descriptor[]=
{
	{kCLI_Name,				(uint32)"phywr"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lhlCliPhyWrite},
	{kCLI_UsageString, 		(uint32)lhlCliPhyWrite_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"phy register write"},
	{kCLI_GroupName,		(uint32)"lhl"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static HRESULT lhlCliPhyPkt(uint32 phypacket)
{
	
	return lhlTxSndPhyPacket((QUADLET)phypacket);
}



static  char	lhlCliPhyPkt_UsageString[]=\
				"lhl:phypkt: \n\r"\
				"use: lhl:phypkt <raw 32 bit packet>\n\r"\
				"------------------------------------------------------------\n\r"\
				"send a PHY packet";
static	CLIDescriptor	lhlCliPhyPkt_Descriptor[]=
{
	{kCLI_Name,				(uint32)"phypkt"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lhlCliPhyPkt},
	{kCLI_UsageString, 		(uint32)lhlCliPhyPkt_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"send PHY packet"},
	{kCLI_GroupName,		(uint32)"lhl"},
	{kCLI_ConstantsString,	(uint32)""},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


#ifdef _STATISTICS
/*
	Statistics Read Tool
*/

static HRESULT lhlCliStatisticsDisplay(LHL_STATISTICS *statistics)
{
	HRESULT hResult = NO_ERROR;

	if (statistics == NULL ||
		statistics->validItems <= 0 || 
		statistics->validItems > STATISTICS_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (statistics->validItems >= STATISTICS_BRI_ACC_ITEMS)
	{
		cliPrintf("    BRInt: %8i   SIInt: %8i   \n\r",
		statistics->BrBriBRInt, statistics->BrBriSIInt);
	}
	
	if (statistics->validItems >= STATISTICS_COUNT_ACC_ITEMS)
	{
		cliPrintf("    TxReq: %8i   RxRsp: %8i   RxReq: %8i   TxRsp: %8i\n\r",
		statistics->TxReqCount, statistics->RxRspCount, statistics->RxReqCount, statistics->TxRspCount);
		cliPrintf("      Int: %8i     Int: %8i     Int: %8i     Int: %8i\n\r",
		statistics->TxReqInt, statistics->RxRspInt, statistics->RxReqInt, statistics->TxRspInt);
		cliPrintf("    Start: %8i    Wake: %8i   Start: %8i    Wake: %8i\n\r",
		statistics->TxReqIntStart, statistics->TxReqIntWake, statistics->TxRspIntStart, statistics->TxRspIntWake);
		cliPrintf("  HeadErr: %8i TailErr: %8i\n\r",statistics->RxNoHeader, statistics->RxNoHeader);
		cliPrintf("SelfIDErr: %8i\n\r",statistics->SelfIDErrs);
	}

#ifdef _LOOSE_ISO
	if (statistics->validItems >= STATISTICS_ASYNC_STREAM_ACC_ITEMS)
	{
		cliPrintf("    TxStm: %8i   RxStm: %8i\n\r",
		lhlStatistics.TxStmCount, lhlStatistics.RxStmCount);
	}
#endif //_LOOSE_ISO

	if (statistics->validItems >= STATISTICS_TIMEOUT_ACC_ITEMS)
	{
		cliPrintf("  ReqTout: %8i RspTout: %8i\n\r",
		statistics->TxReqTimeout, statistics->TxRspTimeout);
	}

	if (statistics->validItems >= STATISTICS_PENDINGTX_ACC_ITEMS)
	{
		cliPrintf("  PendUse: %8i PendMax: %8i\n\r",
		statistics->PendTxInuse, statistics->PendTxInuseMax);

		cliPrintf("  TxReqME: %8i TxReqAO: %8i TxRspME: %8i TxRspAO: %8i\n\r", 
		statistics->PendTxReqMatchErr, statistics->PendTxReqAgeout, statistics->PendTxRspMatchErr, statistics->PendTxRspAgeout);
#ifdef _LOOSE_ISO
		cliPrintf("  TxStmME: %8i TxStmAO: %8i\n\r",
		lhlStatistics.PendTxStmMatchErr, lhlStatistics.PendTxStmAgeout);
#endif //_LOOSE_ISO
	}

	if (statistics->validItems >= STATISTICS_PB_ACC_ITEMS)
	{
		cliPrintf("  pbBlock: %8i pbPload: %8i\n\r",
		statistics->pbAllocBlock, statistics->pbAllocPayload);
	}

	if (statistics->validItems >= STATISTICS_TLABELS_ACC_ITEMS)
	{
		cliPrintf("  TLblReq: %8i TLblMax: %8i\n\r", 
		statistics->TLabelsReqInuse, statistics->TLabelsReqInuseMax);
#ifdef _LOOSE_ISO
		cliPrintf("  TLblStm: %8i TLblMax: %8i\n\r", 
		statistics->TLabelsStmInuse, statistics->TLabelsStmInuseMax);
#endif //_LOOSE_ISO
	}

	if (statistics->validItems >= STATISTICS_STATUSINFO_ACC_ITEMS)
	{
		cliPrintf("  sInfUse: %8i sInfMax: %8i\n\r",
		statistics->statusInfoInuse, statistics->statusInfoInuseMax);
	}

	if (statistics->validItems >= STATISTICS_MSGQUEUE_ACC_ITEMS)
	{
		cliPrintf("  msgQUse: %8i msgQMax: %8i\n\r",
		statistics->msgQueueInuse, statistics->msgQueueInuseMax);
	}

	if (statistics->validItems >= STATISTICS_HANDLE_ACC_ITEMS)
	{
		cliPrintf("  InvaHdl: %8i\n\r",
		statistics->handleInvalid);
	}

	if (statistics->validItems >= STATISTICS_SYSLOG_ACC_ITEMS)
	{
		cliPrintf("  sysLogE: %8i sysLogI: %8i\n\r",
		statistics->sysLogErr, statistics->sysLogISR);
	}

	return hResult;
}

HRESULT	lhlCliShowStatistics(void)
{
	HRESULT		hResult = NO_ERROR;

	uint32		curTime;
	curTime = TCTimeGet();
	cliOutputLine("LHL Statistics") ;
	cliPrintf("time[%5is] :\n\r", curTime / 1000);
	hResult = lhlCliStatisticsDisplay(&lhlStatistics);

	return hResult;
}

static  char	lhlCliShowStatistics_UsageString[]=\
				"lhl:stats: \n\r"\
				"use: lhl:stats\n\r"\
				"------------------------------------------------------------\n\r"\
				"display node lhl statistics";

static	CLIDescriptor	lhlCliShowStatistics_Descriptor[]=
{
	{kCLI_Name,				(uint32)"stats"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lhlCliShowStatistics},
	{kCLI_UsageString, 		(uint32)lhlCliShowStatistics_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"lhl statistics"},
	{kCLI_GroupName,		(uint32)"lhl"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

#endif //_STATISTICS

void lhlCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(lhlCliForceBusReset_Descriptor);
	hResult += cliInstallCLIDescriptor(lhlCliForceRoot_Descriptor);
	hResult += cliInstallCLIDescriptor(lhlCliPhyRead_Descriptor);
	hResult += cliInstallCLIDescriptor(lhlCliPhyWrite_Descriptor);
	hResult += cliInstallCLIDescriptor(lhlCliPhyPkt_Descriptor);
#ifdef _STATISTICS
	hResult += cliInstallCLIDescriptor(lhlCliShowStatistics_Descriptor);
#endif //_STATISTICS

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing lhl cli tools");
	}

	lhlLLCCliInstallTools();
}

#endif //_CLI_TOOL_LHL
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004-2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	DICE  Enhanced Application Protocol Interface
	
	Revisions:
		created 19/02/2007 ml
****************************************************************************/
/** @addtogroup EAP

	Exposes an Enhanced Application configuration protocol for control panel control
	of routing, mixing, peak detection, storage of settings etc.

	This module can be used from the command prompt by the @ref cli_eap commands.

	The EAP module takes advantage of DAL and DiceDriver. The main application 
	configures EAP and pass on information from DiceDriver and DAL,
	@{
*/

#ifndef _EAP_INT_H_
#define _EAP_INT_H_

#include "TCTypes.h"
#include "diceDriverInterface.h"
#include "diceRouterReg.h"

//These defines does not belong here, they should be moved to their respective
//diceDefs.h files. In this file they only serve the purpose of making sure
//the reserved areas are big enough. Host applications using this protocol
//should not assume anything about these numbers. The GENERAL_CAPS fields
//indicating MAX_TX_STREAMS and MAX_RX_STREAMS should be used and it can
//be assumed that the defines below are larger or equal to those.
//The same is the case for MAX_MIX_COEFF which is guaranteed to be larger
//or equal to the product of the MIXER_CAPS fields MIXER_INPUTS and
//MIXER_OUTPUTS.

#define	MAX_TX_STREAMS	2
#define	MAX_RX_STREAMS	2



#define	MAX_MIX_COEFF	288


/// opcodes
typedef enum
{
	EAP_OP_NOP=0,					//
	EAP_OP_LD_ROUTER,				//
	EAP_OP_LD_STRM_CFG,				//
	EAP_OP_LD_RTR_STRM_CFG,			//
	EAP_OP_LD_FLASH_CFG,			//
	EAP_OP_ST_FLASH_CFG				//
}EAP_OP_ENUM;


// opcode definitions
#define EAP_OP_EXECUTE	0x80000000
#define EAP_OP_LD_LOW	0x00010000
#define EAP_OP_LD_MID	0x00020000
#define EAP_OP_LD_HIGH	0x00040000
#define EAP_OP_MASK		0x0000FFFF



typedef struct
{
	uint32					capabilityOffSet;
	uint32					capabilitySize;
	uint32					commandOffSet;
	uint32					commandSize;
	uint32					mixerOffSet;
	uint32					mixerSize;
	uint32					peakOffSet;
	uint32					peakSize;
	uint32					newRoutingOffSet;
	uint32					newRoutingSize;
	uint32					newStrmCfgOffSet;
	uint32					newStrmCfgSize;
	uint32					currCfgOffSet;
	uint32					currCfgSize;
	uint32					stndAloneCfgOffSet;
	uint32					stndAloneCfgSize;
	uint32					appSpaceOffset;
	uint32					appSpaceSize;
	uint32					lastIsZero;
}EAP_OFFSET_STRUCT;


typedef struct
{
	uint32					routerCaps;
	uint32					mixerCaps;
	uint32					generalCaps;
	uint32					reserved;
} EAP_CAPABILITIES;

// Defines for routerCaps bit fields
#define EAP_CAP_ROUTER_EXPOSED	0x00000001
#define EAP_CAP_ROUTER_RO		0x00000002
#define EAP_CAP_ROUTER_FLS		0x00000004
#define EAP_CAP_ROUTER_MAX_MSK  0xffff0000
#define EAP_CAP_ROUTER_MAX_SHFT 16

// Defines for mixerCaps bit fields
#define EAP_CAP_MIXER_EXPOSED		0x00000001
#define EAP_CAP_MIXER_RO			0x00000002
#define EAP_CAP_MIXER_FLS			0x00000004
#define EAP_CAP_MIXER_IN_DEV_MSK	0x000000f0
#define EAP_CAP_MIXER_IN_DEV_SHFT	4
#define EAP_CAP_MIXER_OUT_DEV_MSK	0x00000f00
#define EAP_CAP_MIXER_OUT_DEV_SHFT	8
#define EAP_CAP_MIXER_INPUTS_MSK	0x00ff0000
#define EAP_CAP_MIXER_INPUTS_SHFT	16
#define EAP_CAP_MIXER_OUTPUTS_MSK	0xff000000
#define EAP_CAP_MIXER_OUTPUTS_SHFT	24

// Defines for generalCaps bit fields
#define EAP_CAP_GEN_STRCFG_EN		0x00000001
#define EAP_CAP_GEN_FLASH_EN		0x00000002
#define EAP_CAP_GEN_PEAK_EN			0x00000004
#define EAP_CAP_GEN_AUTO_STORE_EN	0x00000008
#define EAP_CAP_GEN_MAX_TX_MSK		0x000000f0
#define EAP_CAP_GEN_MAX_TX_SHFT		4
#define EAP_CAP_GEN_MAX_RX_MSK		0x00000f00
#define EAP_CAP_GEN_MAX_RX_SHFT		8
#define EAP_CAP_GEN_STRM_CFG_FLS	0x00001000
#define EAP_CAP_GEN_CHIP_MSK		0x00ff0000
#define EAP_CAP_GEN_CHIP_SHFT		16

#define EAP_CHIP_IS_DICEII			0
#define EAP_CHIP_IS_TCD2210			1
#define EAP_CHIP_IS_TCD2220			2
#define EAP_CHIP_IS_UNKNOWN			255





typedef struct
{
	uint32					opcode;
	uint32					retVal;
} EAP_COMMAND;

typedef struct
{
	uint32					satBits;
	uint32					coeff[MAX_MIX_COEFF];
} EAP_MIXER;

typedef struct
{
	uint32					peak[MAX_ROUTE_ENTRIES];
} EAP_PEAK;

typedef struct
{
	uint32					routeEntries;
	uint32					route[MAX_ROUTE_ENTRIES];
} EAP_NEW_ROUTING;

typedef struct 
{
	uint32  nbAudioChannels;	
	uint32  nbMidiChannels;		
    char    names[DD_SIZE_OF_NAMES];
	uint32	AC3Capabilities;	
} EAP_STREAM_CFG;
	

typedef struct
{
	uint32					nbTx;
	uint32					nbRx;
	EAP_STREAM_CFG			stmCfg[MAX_TX_STREAMS+MAX_RX_STREAMS];
} EAP_NEW_STRM_CFG;

typedef struct
{
	EAP_NEW_ROUTING			lowRouter;
	uint8					res1[4096-sizeof(EAP_NEW_ROUTING)];
	EAP_NEW_STRM_CFG		lowStrmCfg;
	uint8					res2[4096-sizeof(EAP_NEW_STRM_CFG)];

	EAP_NEW_ROUTING			midRouter;
	uint8					res3[4096-sizeof(EAP_NEW_ROUTING)];
	EAP_NEW_STRM_CFG		midStrmCfg;
	uint8					res4[4096-sizeof(EAP_NEW_STRM_CFG)];

	EAP_NEW_ROUTING			highRouter;
	uint8					res5[4096-sizeof(EAP_NEW_ROUTING)];
	EAP_NEW_STRM_CFG		highStrmCfg;
	uint8					res6[4096-sizeof(EAP_NEW_STRM_CFG)];
} EAP_CURR_CFG;

typedef struct
{
	uint32					clkSrc;
	uint32					extAES;
	uint32					extADAT;
	uint32					extWC;
	uint32					extInt;
	uint32					reserved[11];
} EAP_STND_ALONE_CFG;

	
	
#define EAP_CAPABILITY_SZ		(sizeof(EAP_CAPABILITIES)/4)
#define EAP_COMMAND_SZ			(sizeof(EAP_COMMAND)/4)
#define EAP_MIXER_SZ			(sizeof(EAP_MIXER)/4)
#define EAP_PEAK_SZ				(sizeof(EAP_PEAK)/4)
#define EAP_NEW_ROUTING_SZ		(sizeof(EAP_NEW_ROUTING)/4)
#define EAP_NEW_STRM_CFG_SZ		(sizeof(EAP_NEW_STRM_CFG)/4)
#define EAP_CURR_CFG_SZ			(sizeof(EAP_CURR_CFG)/4)
#define EAP_STND_ALONE_CFG_SZ	(sizeof(EAP_STND_ALONE_CFG)/4)
#define EAP_MAX_APP_SZ			(128)

#define EAP_CAPABILITY_OFF		(sizeof(EAP_OFFSET_STRUCT)/4)
#define EAP_COMMAND_OFF			(EAP_CAPABILITY_OFF+EAP_CAPABILITY_SZ)	
#define EAP_MIXER_OFF			(EAP_COMMAND_OFF+EAP_COMMAND_SZ)
#define EAP_PEAK_OFF			(EAP_MIXER_OFF+EAP_MIXER_SZ)
#define EAP_NEW_ROUTING_OFF		(EAP_PEAK_OFF+EAP_PEAK_SZ)
#define EAP_NEW_STRM_CFG_OFF	(EAP_NEW_ROUTING_OFF+EAP_NEW_ROUTING_SZ)
#define EAP_CURR_CFG_OFF		(EAP_NEW_STRM_CFG_OFF+EAP_NEW_STRM_CFG_SZ)
#define EAP_STND_ALONE_CFG_OFF	(EAP_CURR_CFG_OFF+EAP_CURR_CFG_SZ)
#define EAP_APP_SPACE_OFF		(EAP_STND_ALONE_CFG_OFF+EAP_STND_ALONE_CFG_SZ)

#define EAP_SPACE_SIZE			(EAP_APP_SPACE_OFF+EAP_MAX_APP_SZ)


#define EAP_BASE_START_HI	0xffff
#define EAP_BASE_START		0xe0200000
#define EAP_BASE_END_HI		0xffff
#define EAP_BASE_END		(EAP_BASE_START + 4*EAP_SPACE_SIZE)



//
#endif  //_EAP_INT_H_

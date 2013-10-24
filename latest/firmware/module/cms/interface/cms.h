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
	
	Purpose:	DICE  Connection Mamagement System Protocol
	
	Revisions:
		created 27/08/2007 ml
****************************************************************************/
/** @addtogroup CMS

	Exposes a  Connection Mamagement System Protocol for configuring connections
	between devices in a network.

	This module can be used from the command prompt by the @ref cli_cms commands.

	The CMS module takes advantage of DAL and AVS. The main application 
	configures CMS and pass on information from DAL,
	@{
*/

#ifndef _CMSAVSONLY

#ifndef _CMS_H_
#define _CMS_H_

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "avs.h"
#include "dal.h"
#include "cmsInterface.h"


/* Capabilities
A number of capability fields are defined in cmsInterface.h, some of those
are hardcoded depending on the capabilities of the chip.
The ones below can be passed on in the CMS_INIT_DESC for the init call to 
CMS.



// Defines for generalCaps bit fields
#define CMS_CAP_MAXRATE_MSK			0x00000003
#define CMS_CAP_MAXRATE_SHFT		0x00000000
#define CMS_CAP_MAXRATE48K			0x00000000
#define CMS_CAP_MAXRATE96K			0x00000001
#define CMS_CAP_MAXRATE192K			0x00000002


*/


//CMS event notifications
typedef enum
{
	eCMS_EVT_INDICATE = 0,			//!< ...
	eCMS_EVT_PERSIST_HOST_CHG,
	eCMS_EVT_RATEMODE_CHG,
	eCMS_EVT_COUNT
} CMS_EVT_ENUM;


//One audio configuration mode, number of sequences and their routing
typedef struct
{
	uint8	channels;
	const SRC_DESCRIPTION * srcList;
} CMS_AUDIO_TX_CFG;

//One Comm configuration mode, number of ports and source Comm device.
typedef struct
{
	uint8	ports;
	const uint8 * portList;
} CMS_COMM_TX_CFG;

//Full set of audio and comm configuration modes for one rate mode
typedef struct
{
	uint8	audioModes;
	uint8	commModes;
	const CMS_AUDIO_TX_CFG * audioMode;
	const CMS_COMM_TX_CFG * commMode;
} CMS_TX_CFGS;


//Audio destination configuration for one rate mode
typedef struct
{
	uint8	outputs;
	const uint8 dstList[];
} CMS_AUDIO_DST_CFG;

//This is only used for CLI textual information
typedef struct
{
	const char ** modeNames[3];
	const char ** dstNames[4];
} CMS_TEXT_DESC;


typedef HRESULT (*CMS_CALLBACK_FUNC)(CMS_EVT_ENUM event, uint32 data);


typedef struct
{
	uint32						capabilities;
	const char *				productName;
	const char *				defNickName;
	const CMS_TX_CFGS *			txCfg[3];  //config modes for all 3 rate modes
	const CMS_AUDIO_DST_CFG *	dstCfg[3]; //Output definition for all 3 rate modes
	uint32						inDevices[3];
	uint32						outDevices[3];
	CMS_CALLBACK_FUNC			appCB;
	uint16						nbPrivatePersistData; 
	void *						pPrivatePersistData;
	const CMS_TEXT_DESC *		pTextDesc; //can be NULL
	const CMS_XML *				pCmsInfo;
} CMS_INIT_DESC;


HRESULT cmsInitializeInterface(const CMS_INIT_DESC * pInit);


HRESULT cmsPrivatePersistChanged(void);
/*
Notify the flash system that the private persistent data has changed and needs to be updated.
*/

// Set and enquiry functions primarily used by CLI interface
HRESULT cmsSetRx (uint8 rxID, CMS_RX_PAR * pCfg);
HRESULT cmsSetTx (uint8 txID, CMS_TX_PAR * pCfg);
HRESULT cmsSetGlobal (CMS_GLOBAL_PAR * pCfg);
HRESULT cmsSetRateMode (uint32 rateMode);
HRESULT cmsSetSync (uint32 sync);
HRESULT cmsSetNickName (char * nickName);
HRESULT setOutputRoute (uint32 * pRoutes, uint8 from, uint8 to);
HRESULT cmsIndicate (uint32 indicate);

CMS_RX_STAT * cmsGetRxStat(uint8 rxID);
CMS_TX_STAT * cmsGetTxStat(uint8 txID);
CMS_RX_PAR * cmsGetRxPar(uint8 rxID);
CMS_TX_PAR * cmsGetTxPar(uint8 txID);
uint8 cmsGetnbOutputs (void);
uint8 cmsGetOutputsRoute (uint8 id);
CMS_GLOBAL_PAR * cmsGetGlobalPar (void);
uint8 cmsGetnbModes (void);
const char * cmsGetAudioModeName (uint8 mode);
const char * cmsGetAudioOutputName (uint8 id);


/* Std prototypes for thread module, called from cyg_main. */

HRESULT cmsInitialize(void);
HRESULT cmsResumeTasks(void);
HRESULT cmsSuspendTasks(void);
HRESULT cmsInitializeTasks(void);

//
#endif  //_CMS_H_
#endif // _CMSAVSONLY

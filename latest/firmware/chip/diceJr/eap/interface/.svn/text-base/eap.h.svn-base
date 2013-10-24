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
	
	Purpose:	DICE  Enhanced Application Protocol API
	
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

#ifndef _EAP_H_
#define _EAP_H_

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "diceDriver.h"
#include "dal.h"
#include "eapInterface.h"


/* Capabilities
A number of capability fields are defined in eapInterface.h, some of those
are hardcoded depending on the capabilities of the chip.
The ones below can be passed on in the EAP_INIT_DESC for the init call to 
EAP.


// Defines for routerCaps bit fields
#define EAP_CAP_ROUTER_EXPOSED	0x00000001
#define EAP_CAP_ROUTER_RO		0x00000002

// Defines for mixerCaps bit fields
#define EAP_CAP_MIXER_EXPOSED		0x00000001
#define EAP_CAP_MIXER_RO			0x00000002

// Defines for generalCaps bit fields
#define EAP_CAP_GEN_STRCFG_EN		0x00000001
#define EAP_CAP_GEN_FLASH_EN		0x00000002
#define EAP_CAP_GEN_PEAK_EN			0x00000004
#define EAP_CAP_GEN_MAX_TX_MSK		0x000000f0
#define EAP_CAP_GEN_MAX_TX_SHFT		4
#define EAP_CAP_GEN_MAX_RX_MSK		0x00000f00
#define EAP_CAP_GEN_MAX_RX_SHFT		8

*/

/*
Enter/exit Stand alone mode.

Enter:
- Boot


Exit:
- Attach
*/


/// EAP Notification events

typedef enum
{
	eEAP_EVT_CFG_CHANGE = 0,			//!< ...
	eEAP_EVT_ROUTE_CHANGE,
	eEAP_EVT_PERSIST_RECALL,
	eEAP_EVT_PERSIST_HOST_CHG,
	eEAP_EVT_COUNT
} EAP_EVT_ENUM;


typedef HRESULT (*EAP_CALLBACK_FUNC)(EAP_EVT_ENUM event, uint32 data);


/* Information about the device

Factory default, low, mid and high routing, low, mid and high channel config

*/
typedef struct
{
	uint16					nbTxStreams;
	const STREAM_CONFIG *	pTxStreams;
	uint16					nbRxStreams;
	const STREAM_CONFIG *	pRxStreams;
	uint16					nbRouterEntries;
	const uint16 *			pRouting;
} EAP_RATE_MODE_INIT;



typedef struct
{	
	EAP_RATE_MODE_INIT	lowInit;
	EAP_RATE_MODE_INIT	midInit;
	EAP_RATE_MODE_INIT	highInit;
	EAP_STND_ALONE_CFG	stdAloneCfg;
	uint16				nbMixerCoeffs;
	const uint16 *		pMixerCoeffs; //only applicable whit MIXER_EXPOSED
	uint32				routerCapabilities;
	uint32				mixerCapabilities;
	uint32				generalCapabilities;
	EAP_CALLBACK_FUNC	appCB;
	uint16				nbPrivatePersistData; 
	void *				pPrivatePersistData; //only applicable with FLASH_EN
	uint16				flashStoreHoldoffMs; //the flash minimum store wait in ms
} EAP_INIT_DESC;


HRESULT eapInitializeInterface(const EAP_INIT_DESC * pInit);

/*
This function will initialize data structures and try to obtain current setting
from flash if Flash management is enabled, if the Flash does not contain a valid
setting, the init values are used.
After initializing the internal structures the diceDriver, mixer and router structures
are initialized.
*/


HRESULT eapDriverChangeRateMode(DAL_RATE_MODE_ENUM rateMode);

/*
This function will load the new routing and stream configuration corresponding to
the new rate mode.
*/


HRESULT eapPrivatePersistChanged(void);

/*
Notify the flash system that the private persistent data has changed and needs to be updated.
*/


HRESULT eapStoreFlash (void);

/*
Notify the flash system that the persistent comfiguration structures has been changed from the
application. Normally changes will happen through the host in which case EAP will take care of the store.
*/



HRESULT eapGetMeterPeak (uint16 entry, uint16 * pValue);
/*
Get the peak value of an entry in the router
*/

EAP_STND_ALONE_CFG * eapGetStandaloneCfg(void);

/* Std prototypes for thread module, called from cyg_main. */

HRESULT eapInitialize(void);
HRESULT eapResumeTasks(void);
HRESULT eapSuspendTasks(void);
HRESULT eapInitializeTasks(void);

//
#endif  //_EAP_H_

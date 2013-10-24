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
	
	Purpose:	Dice Audio Application Layer (DAL)- DICE audio control API
  
  Description:       
  
	Revisions:
		created 10/01/2005 eb
		rewritten 21/03/2005 ml

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCDebug.h"
#include "dice.h"
#include "dalCli.h"
#include "dal.h"
#include "dalEvents.h"
#include "dalRouter.h"
#include "dalRates.h"
#include "dalAES.h"
#include "dalADAT.h"
#ifdef _AVS
#include "dalAVS.h"
#endif
#include "dalInS.h"


static char 	moduleName[] = __FILE__ ;


// task (thread) ID for this module									
uint32						dalThreadID = 0;				// clock controller thread
const char*					dalThreadName = "dalThread";


static DAL_EVENT_INTERFACE		eventInterface;

typedef struct
{
	DAL_CLOCKCTRL_INTERFACE clockCtrl;	
	uint32					inputDevices;
	uint32					outputDevices;
	uint8					bCreated;
	uint8					bGlobalMute;
	uint8					bStarted;
} DAL_INTERFACE;

static DAL_INTERFACE dalInterfaces[eDAL_NUM_INTERFACES];

static void _dalMuteTxInterfaces (uint32 outputDevices, uint8 bMute)
{	
	if (outputDevices & DEVICE_BIT(DEV_AES_TX)) dalAESTxMute (bMute);
	if (outputDevices & DEVICE_BIT(DEV_ADAT_TX)) dalADATTxMute (bMute);
	if (outputDevices & DEVICE_BIT(DEV_INS_TX0)) dalInSTxMute(INS_ID0, bMute);
	if (outputDevices & DEVICE_BIT(DEV_INS_TX1)) dalInSTxMute(INS_ID1, bMute);
	//if (outputDevices & DEVICE_BIT(DEV_APB_TX)) does not exist ML????
#ifdef _AVS
	if (outputDevices & DEVICE_BIT(DEV_AVS_ADI1)) dalAVSTxMute (AVS_PLUG_ID1, bMute);
	if (outputDevices & DEVICE_BIT(DEV_AVS_ADI2)) dalAVSTxMute (AVS_PLUG_ID2, bMute);
#endif
}

static void dalMuteTxInterfaces (DAL_INTERFACE_ID_ENUM dalInterfaceId, uint8 bMute)
{	
	if (dalInterfaces[dalInterfaceId].bGlobalMute) bMute=1;
	_dalMuteTxInterfaces (dalInterfaces[dalInterfaceId].outputDevices, bMute);
}


static void dalEnableTxInterfaces (uint32 outputDevices, uint8 bEnable)
{	
	if (outputDevices & DEVICE_BIT(DEV_AES_TX)) dalAESTxEnable (bEnable);
	if (outputDevices & DEVICE_BIT(DEV_ADAT_TX)) dalADATTxEnable (bEnable);
	if (outputDevices & DEVICE_BIT(DEV_INS_TX0)) dalInSTxEnable (INS_ID0, bEnable);
	if (outputDevices & DEVICE_BIT(DEV_INS_TX1)) dalInSTxEnable (INS_ID1, bEnable);
	//if (outputDevices & DEVICE_BIT(DEV_APB_TX)) does not exist ML????
#ifdef _AVS
	if (outputDevices & DEVICE_BIT(DEV_AVS_ADI1)) dalAVSTxEnable (AVS_PLUG_ID1, bEnable);
	if (outputDevices & DEVICE_BIT(DEV_AVS_ADI2)) dalAVSTxEnable (AVS_PLUG_ID2, bEnable);
#endif
}

static void dalNominalRateTxInterfaces (uint32 outputDevices, DAL_NOMINAL_RATE_ENUM rate)
{	
	if (outputDevices & DEVICE_BIT(DEV_AES_TX)) dalAESTxSetNominalRate (rate);
	if (outputDevices & DEVICE_BIT(DEV_ADAT_TX)) dalADATTxSetNominalRate (rate);
	if (outputDevices & DEVICE_BIT(DEV_INS_TX0)) dalInSTxSetNominalRate (INS_ID0, rate);
	if (outputDevices & DEVICE_BIT(DEV_INS_TX1)) dalInSTxSetNominalRate (INS_ID1, rate);
	//if (outputDevices & DEVICE_BIT(DEV_APB_TX)) does not exist ML????
#ifdef _AVS
	if (outputDevices & DEVICE_BIT(DEV_AVS_ADI1)) dalAVSTxSetNominalRate (AVS_PLUG_ID1, rate);
	if (outputDevices & DEVICE_BIT(DEV_AVS_ADI2)) dalAVSTxSetNominalRate (AVS_PLUG_ID2, rate);
#endif
}


static void dalEnableRxInterfaces (uint32 inputDevices, uint8 bEnable)
{	
	if (inputDevices & DEVICE_BIT(DEV_AES_RX)) dalAESRxEnable (bEnable);
	if (inputDevices & DEVICE_BIT(DEV_ADAT_RX)) dalADATRxEnable (bEnable);
	if (inputDevices & DEVICE_BIT(DEV_INS_RX0)) dalInSRxEnable (INS_ID0, bEnable);
	if (inputDevices & DEVICE_BIT(DEV_INS_RX1)) dalInSRxEnable (INS_ID1, bEnable);
	//if (inputDevices & DEVICE_BIT(DEV_APB_RX)) does not exist ML????
#ifdef _AVS
	if (inputDevices & DEVICE_BIT(DEV_AVS_ADO1)) dalAVSRxEnable (AVS_PLUG_ID1, bEnable);
	if (inputDevices & DEVICE_BIT(DEV_AVS_ADO2)) dalAVSRxEnable (AVS_PLUG_ID2, bEnable);
#endif
}

static void dalNominalRateRxInterfaces (uint32 inputDevices, DAL_NOMINAL_RATE_ENUM rate)
{	
	if (inputDevices & DEVICE_BIT(DEV_AES_RX)) dalAESRxSetNominalRate (rate);
	if (inputDevices & DEVICE_BIT(DEV_ADAT_RX)) dalADATRxSetNominalRate (rate);
	if (inputDevices & DEVICE_BIT(DEV_INS_RX0)) dalInSRxSetNominalRate (INS_ID0, rate);
	if (inputDevices & DEVICE_BIT(DEV_INS_RX1)) dalInSRxSetNominalRate(INS_ID1, rate);
	//if (inputDevices & DEVICE_BIT(DEV_APB_RX)) does not exist ML????
#ifdef _AVS
	if (inputDevices & DEVICE_BIT(DEV_AVS_ADO1)) dalAVSRxSetNominalRate (AVS_PLUG_ID1, rate);
	if (inputDevices & DEVICE_BIT(DEV_AVS_ADO2)) dalAVSRxSetNominalRate (AVS_PLUG_ID2, rate);
	//if (inputDevices & DEVICE_BIT(DEV_AVS_ADO3)) dalAVSRxSetNominalRate (AVS_PLUG_ID3, rate);
	//if (inputDevices & DEVICE_BIT(DEV_AVS_ADO4)) dalAVSRxSetNominalRate (AVS_PLUG_ID4, rate);
#endif
}

HRESULT dalGlobalMute   (DAL_INTERFACE_ID_ENUM dalInterfaceId, BOOL mute)
{
	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		sysLogError(E_DAL_INTERFACE_NOT_DEFINED, __LINE__, moduleName);
		return E_DAL_INTERFACE_NOT_DEFINED;
		
	}
	dalInterfaces[dalInterfaceId].bGlobalMute = mute;
	dalMuteTxInterfaces(dalInterfaceId,dalInterfaces[dalInterfaceId].clockCtrl.state != eDAL_STATE_LOCKED);
	
	return NO_ERROR;
}


void dalThread(void *dummy)
{
	UNUSED_ARG(dummy);
#ifdef _AVS
	uint8 event;
#endif
	DAL_NOMINAL_RATE_ENUM sysRate;
	
	DAL_EVENTS events;
	
	
	DO_FOREVER
	{
		TCTaskWait(DAL_TICK_MS);

		// Make sure State Machines for each of the active audio subsystems are run 
		aesRxStateMachine((uint8 *)&events.aes_events);
		adatRxStateMachine(&events.adat_events);
#ifdef _AVS		
		avsRxPlugStateMachine (AVS_PLUG_ID1, &event);
		events.avs_events = event;		
		avsRxPlugStateMachine (AVS_PLUG_ID2, &event);
		events.avs_events |= event << 8;
		
		avsTxPlugStateMachine (AVS_PLUG_ID1, &event);
		avsTxPlugStateMachine (AVS_PLUG_ID2, &event);
#endif
		
		// Run Clock Ctrl State Machine for each clock domain
		dalClockCtrlFSM  (&dalInterfaces[eDAL_INTERFACE_1].clockCtrl, NULL);
		events.fsm_events = dalInterfaces[eDAL_INTERFACE_1].clockCtrl.events & DAL_CB_DOM1_MASK;
								
		// check a few events by ourselves, domain 1
		if (events.fsm_events & (DAL_CB_LOCK1_AQUIRED | DAL_CB_RATE1_NOM_CHANGED))
		{
			sysRate = dalClockCtrlGetSysNominal (&dalInterfaces[eDAL_INTERFACE_1].clockCtrl);
			dalMuteTxInterfaces        (eDAL_INTERFACE_1, FALSE);
			dalNominalRateTxInterfaces (dalInterfaces[eDAL_INTERFACE_1].outputDevices, sysRate);
			dalNominalRateRxInterfaces (dalInterfaces[eDAL_INTERFACE_1].inputDevices, sysRate);			
		}
		else if (events.fsm_events & (DAL_CB_RATE1_NOM_CHANGED))
		{
			sysRate = dalClockCtrlGetSysNominal (&dalInterfaces[eDAL_INTERFACE_1].clockCtrl);
			dalNominalRateTxInterfaces (dalInterfaces[eDAL_INTERFACE_1].outputDevices, sysRate);
			dalNominalRateRxInterfaces (dalInterfaces[eDAL_INTERFACE_1].inputDevices, sysRate);			
		}
		if (events.fsm_events & (DAL_CB_LOCK1_LOST))
		{
			dalMuteTxInterfaces        (eDAL_INTERFACE_1, TRUE);
		}
		
		// if lock acquired call transmitters to notify new rate and unmute
		// if lock lost call mute
		
		//need to notify callbacks on events		
		dalEventsHandleCallback(&eventInterface, &events);		
	}
}

HRESULT dalInstallCallback  (const DAL_EVENTS * pEventMask, uint32 holdoff_ms, DAL_CALLBACK_FUNC func, uint32 data)
{
	return dalEventsInstallCallback  (&eventInterface, pEventMask, holdoff_ms, func, data);
}


HRESULT dalUninstallCallback(DAL_CALLBACK_FUNC func)
{
	return dalEventsUninstallCallback(&eventInterface, func);
}

HRESULT dalCreateInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_RATE_MODE_ENUM rateMode, uint32 inputDevices, uint32 outputDevices)
{
	DAL_INTERFACE_ID_ENUM otherInterface;
	RATE_DESC * pRate;
	HRESULT hResult = NO_ERROR;
	
	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		goto errorReturn;
	}
	if (rateMode >= eDAL_RATE_MODE_COUNT)
	{
		hResult = E_DAL_BAD_INPUT_PARAM;
		goto errorReturn;
	}
	if ((dalInterfaceId == eDAL_INTERFACE_1) && (rateMode > eDAL_RATE_MODE_ALL))
	{
		hResult = E_DAL_BAD_INPUT_PARAM;
		goto errorReturn;
	}
	if (dalInterfaces[dalInterfaceId].bCreated)
	{
		dalDestroyInterface(dalInterfaceId);
	}
	// check if other interfaces are using tx or rx devices required by this board
	for (otherInterface = eDAL_INTERFACE_1; otherInterface < eDAL_NUM_INTERFACES; otherInterface++)
	{
		if ((otherInterface != dalInterfaceId) && dalInterfaces[otherInterface].bCreated)
		{
			if ((dalInterfaces[otherInterface].inputDevices & inputDevices) || (dalInterfaces[otherInterface].outputDevices & outputDevices))
			{
				hResult = E_DAL_IODEVICE_CONFLICT;
				goto errorReturn;					
			}
		}
	}
	// setup the domain
	dalInterfaces[dalInterfaceId].inputDevices = inputDevices;
	dalInterfaces[dalInterfaceId].outputDevices = outputDevices;
	dalInterfaces[dalInterfaceId].bCreated = TRUE;
	dalInterfaces[dalInterfaceId].bGlobalMute = FALSE;
	dalInterfaces[dalInterfaceId].bStarted = FALSE;
	dalClockCtrlInputDevices (&dalInterfaces[dalInterfaceId].clockCtrl, inputDevices);
	dalClockCtrlSetRateMode (&dalInterfaces[dalInterfaceId].clockCtrl, rateMode);
	
	//find a compatible rate
	pRate = dalRateGetDefaultRate (rateMode, eDAL_NOMINAL_RATE_48);
	dalClockCtrlSetClockSource (&dalInterfaces[dalInterfaceId].clockCtrl, eDAL_CLK_SRC_INTERNAL, pRate->sysNominalID);
	
	dalRouterCreateConfiguration (dalInterfaceId, rateMode, inputDevices, outputDevices);
	
	return hResult;

errorReturn:
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;

}


HRESULT dalDestroyInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId)
{
	HRESULT hResult = NO_ERROR;

	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (dalInterfaces[dalInterfaceId].bCreated)
	{
		dalMuteTxInterfaces        (dalInterfaceId, TRUE);
		dalEnableTxInterfaces      (dalInterfaces[dalInterfaceId].outputDevices, FALSE);
		dalEnableRxInterfaces      (dalInterfaces[dalInterfaceId].inputDevices, FALSE);
		dalRouterCreateConfiguration (dalInterfaceId, eDAL_RATE_MODE_LOW, 0, 0);
		dalClockCtrlDisable			(&dalInterfaces[dalInterfaceId].clockCtrl);	
		dalInterfaces[dalInterfaceId].bCreated = FALSE;
		dalInterfaces[dalInterfaceId].bStarted = FALSE;
	}
	return hResult;
}

HRESULT dalStartInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId)
{
	HRESULT hResult = NO_ERROR;

	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (!dalInterfaces[dalInterfaceId].bCreated)
	{
		hResult = E_DAL_INTERFACE_NOT_CREATED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (!dalInterfaces[dalInterfaceId].bStarted)
	{
		dalClockCtrlEnable (&dalInterfaces[dalInterfaceId].clockCtrl);
		dalInterfaces[dalInterfaceId].bStarted = TRUE;
		//dalRouterMembershipTxInterfaces (dalInterfaces[dalInterfaceId].outputDevices, dalInterfaceId);
		//dalRouterMembershipRxInterfaces (dalInterfaces[dalInterfaceId].inputDevices, dalInterfaceId);
		dalEnableTxInterfaces      (dalInterfaces[dalInterfaceId].outputDevices, TRUE);
		dalEnableRxInterfaces      (dalInterfaces[dalInterfaceId].inputDevices, TRUE);

		//ML: Added code to make sure AES receiver is updated to HIGH mode at interface start.
		RATE_DESC * pRate;
	
		pRate = dalRateGetDefaultRate (dalInterfaces[dalInterfaceId].clockCtrl.rateMode, 
										dalInterfaces[dalInterfaceId].clockCtrl.sampleRate);
		dalNominalRateRxInterfaces (dalInterfaces[eDAL_INTERFACE_1].inputDevices, pRate->sysNominalID);	
	}
	return hResult;
}

HRESULT dalSetClockSource (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_CLOCK_SOURCE_ENUM clockSource, DAL_NOMINAL_RATE_ENUM nominalRate)
{
	HRESULT hResult = NO_ERROR;

	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (!dalInterfaces[dalInterfaceId].bCreated)
	{
		hResult = E_DAL_INTERFACE_NOT_CREATED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	hResult = dalClockCtrlSetClockSource (&dalInterfaces[dalInterfaceId].clockCtrl, clockSource, nominalRate);
	return hResult;
}

HRESULT dalSetRoute(DAL_INTERFACE_ID_ENUM dalInterfaceId, DEST_DESCRIPTION dst, SRC_DESCRIPTION src)
{
	return dalRouterAddRoute (dalInterfaceId, src, dst);
}

HRESULT dalSetRoutes(DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_ROUTING_ENTRY *pRouting, uint8 numEntries)
{
	HRESULT hResult = NO_ERROR;	
	while (numEntries--)
	{
		hResult = dalRouterAddRoute (dalInterfaceId, pRouting->src_description, pRouting->dest_description);
		if (hResult != NO_ERROR) break;
		pRouting++;
	}
	return hResult;
}

HRESULT dalGetCurrentStatus  (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_STATUS * pStatus)
{
	HRESULT hResult = NO_ERROR;

	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	pStatus->state = dalInterfaces[dalInterfaceId].clockCtrl.state;
	pStatus->lockedRateMode = dalInterfaces[dalInterfaceId].clockCtrl.actualRateMode;
	pStatus->lockedNominalRate = dalInterfaces[dalInterfaceId].clockCtrl.actualNominalRate;
	pStatus->lockedRateHz = dalInterfaces[dalInterfaceId].clockCtrl.actualRateHz;
	pStatus->badRateHz = dalInterfaces[dalInterfaceId].clockCtrl.rxPllData.rateHZ;
	return hResult;
}

BOOL dalIsLocked (DAL_INTERFACE_ID_ENUM dalInterfaceId)
{
	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		return FALSE;
	}
	return (dalInterfaces[dalInterfaceId].clockCtrl.state == eDAL_STATE_LOCKED);
}

HRESULT dalGetCurrentConfiguration  (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_CONFIG * pConfig)
{
	HRESULT hResult = NO_ERROR;

	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pConfig->rateMode	=	dalInterfaces[dalInterfaceId].clockCtrl.rateMode;
	pConfig->clockSource = 	dalInterfaces[dalInterfaceId].clockCtrl.clockSource;
	pConfig->sampleRate = dalInterfaces[dalInterfaceId].clockCtrl.sampleRate;
	pConfig->inputDevices = dalInterfaces[dalInterfaceId].inputDevices;
	pConfig->outputDevices = dalInterfaces[dalInterfaceId].outputDevices;
	return hResult;
}

HRESULT dalSetConfig (uint32 msWaitStable, uint32 msWaitLock, BOOL bPLLoffWhenNoSource)
{
	return dalClockCtrlSetConfig (msWaitStable, msWaitLock, bPLLoffWhenNoSource);
}

HRESULT dalGetSyncSrcInfo (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_SYNC_SRC_INFO * pInfo)
{
	HRESULT hResult = NO_ERROR;
	
	if (dalInterfaceId >= eDAL_NUM_INTERFACES)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	return dalClockCtrlGetSyncSrcInfo (&dalInterfaces[dalInterfaceId].clockCtrl, pInfo);
}


/*********************************************************
	Resume tasks for this module
*/	
HRESULT dalResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// Clock controller thread
	hResult = TCTaskResume(dalThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT dalSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// Clock controller thread
	hResult = TCTaskSuspend(dalThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// Initalize dice and interface and clock controller task.
HRESULT dalInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	
	hResult = dalRateInitialize ();
	if (hResult != NO_ERROR) return hResult;

	hResult = dalClockCtrlInitialize (&dalInterfaces[eDAL_INTERFACE_1].clockCtrl, eDAL_INTERFACE_1);
	if (hResult != NO_ERROR) return hResult;

	hResult = dalRouterInitialize ();
	if (hResult != NO_ERROR) return hResult;

	//make sure all interfaces come up in a known state
	_dalMuteTxInterfaces       (0xffffffff, TRUE);
	dalEnableTxInterfaces      (0xffffffff, FALSE);
	dalNominalRateTxInterfaces (0xffffffff, eDAL_NOMINAL_RATE_48);
	dalEnableRxInterfaces      (0xffffffff, FALSE);
	dalNominalRateRxInterfaces (0xffffffff, eDAL_NOMINAL_RATE_48);
	
	hResult = dalResumeTasks();
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_TOOL_DAL
	dalCliInstallTools();
#endif //_CLI_TOOL_DAL

	
	//for debugging only
	clkSetSampleRateCntr(0, SR_CNT_AVS_SYT1	, SR_CNT_ROUTER_1);
	clkSetSampleRateMode(0, SR_PHASE_DETECTOR);
	clkSetSampleRateCntr(1, SR_CNT_AVS_SYT2	, SR_CNT_ROUTER_1);
	clkSetSampleRateMode(1, SR_PHASE_DETECTOR);
	

	
	return hResult;
}

HRESULT dalInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

		// Clock controller thread
	hResult = TCTaskCreate(&dalThreadID, dalThreadName, dalThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;


	return hResult;
}




/*
Initially the system is disabled. All receiver VCO's are disabled, routers are disabled
All transmitters are muted.

When a domain is started:
	Rx Interfaces belonging to the domain are enabled including VCO's when applicable
	Tx interfaces belonging to the domain are enabled
	All Rx and Tx interfaces are notified of the current rate
	The router for the domain is enabled
	
	
When a domain is destroyed:
	All Tx interfaces belonging to the domain are muted and disabled if supported
	All Rx interfaces belonging to the domain are disabled
	The router for the domain is disabled
	
When a domain is locked:
	All Tx interfaces belonging to the domain are unmuted
	All Rx and Tx interfaces belonging to the domain are notified of the rate
	
When a domain is unlocked:
	All Tx interfaces belonging to the domain are muted
	AVS Tx interfaces are disabled
	
When a domain changes rate:
	All Rx and Tx interfaces belonging to the domain ar notified of the rate
	

Special handling for AVS:
	AVS transmitters needs to be enabled and disabled under	application control. 
	The application must use the dalAvsStart or dalAvsStartTx functions. Only if
	both the application and DAL wish to start the transmitter will it be started.
	




*/


//********* Debug function to determine the AVS phase offset **************
//This function does only operate on eDAL_INTERFACE_1
//Rx id is either 0 or 1 for AVS1 or AVS2
HRESULT dalGetAVSPhaseOffset (uint32 rxId, int32 * pPhase10us, int8 * pPhasePercent)
{
	HRESULT hResult = NO_ERROR;
	uint8 mode;
	int32 cnt,period;
	
	if (rxId >= 2)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	*pPhasePercent = 0x7f;
	*pPhase10us = 0x7fffffff;
	
	clkGetSampleRateValue(rxId, &mode, (uint32*)&cnt);
	
	if (dalIsLocked (eDAL_INTERFACE_1))
	{
		RATE_DESC * pRate;
		pRate = dalRateByID (dalInterfaces[eDAL_INTERFACE_1].clockCtrl.actualNominalRate);
		if (!pRate)
		{
			hResult = E_DAL_ILLEGAL_NOMINAL_RATE;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		period = 49152000/pRate->rate;
		if (cnt < 2*period)
		{
			if (cnt > period) cnt -= period;
			if (cnt > (period>>1)) cnt -= period;
			*pPhasePercent = 100*cnt/period;
			*pPhase10us = 10000*cnt/49152;
			return hResult;
		}
	}

	return hResult;
}

HRESULT dalSkewRates (int32 tenthPercent)
{
	dalRateSkewRates (tenthPercent);
	dalClockCtrlUpdateInternalRate (&dalInterfaces[eDAL_INTERFACE_1].clockCtrl);
	return NO_ERROR;
}


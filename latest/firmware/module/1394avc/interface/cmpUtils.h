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
 * DESCRIPTION: Connection Management Procedures (CMP) Utilties

	This module implements the connection management procedures defined in
	IEC 61883-1.

	The following registers are implemented:
		iPCR
		oPCR
		iMPR
		oMPR

	Functions for reading and performing locks on the registers

	Revisions:
		created 05/22/2003 lm

****************************************************************************/

#ifndef _CMPUTILS_H
#define _CMPUTILS_H

#include "1394.h"
#include "TCTypes.h"
#include "cmpDefs.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

typedef HRESULT (* CMP_CHANNEL_USAGE_CB)(uint32 channel);
typedef HRESULT (* CMP_SIGNAL_FORMAT_CB)(void);

typedef	struct
{
	BOOL					bIn;
	uint32					plugID;
	uint32					format;
	CMP_SIGNAL_FORMAT_CB	callback;
} CMP_SIGNAL_FORMAT_CHANGE;

#define	MAX_SIGNAL_FORMAT_ITEMS		32

HRESULT				cmpRegisterChannelUsageCB (uint32 channel, CMP_CHANNEL_USAGE_CB callback);
HRESULT				cmpUnregisterChannelUsageCB (uint32 channel);
HRESULT				cmpCheckChannelUsageCB (uint32 channel);
					
HRESULT				cmpRegisterSignalFormatCB (BOOL bIn, uint32 plugID, uint32 format, CMP_SIGNAL_FORMAT_CB callback);
HRESULT				cmpUnregisterSignalFormatCB (BOOL bIn, uint32 plugID);
HRESULT				cmpCheckSignalFormatCB (BOOL bIn, uint32 plugID, uint32 format);
					
HRESULT				cmpAllocateIRMResources(uint32 *channel, uint32 bandwidth);
HRESULT				cmpDeallocateIRMResources(uint32 channel, uint32 bandwidth);
				
BOOL				cmpIsAnyoPCRConnected(void);
BOOL				cmpIsAnyiPCRConnected(void);
				
HRESULT				cmpHandleRegister(uint32 operation, uint32 index, uint32 *value1, uint32 *value2, uint32 *value3);

														// get value of a PCR register
_TC_INLINE HRESULT	cmpGetoMPR(uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetoMPR(uint32 *value)
								{
									return cmpHandleRegister(CMP_OMPR_REGISTER_READ, 0, value, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpGetiMPR(uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetiMPR(uint32 *value)
								{
									return cmpHandleRegister(CMP_IMPR_REGISTER_READ, 0, value, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpGetoPCR(uint32 oPCRNumber, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetoPCR(uint32 oPCRNumber, uint32 *value)
								{
									return cmpHandleRegister(CMP_OPCR_REGISTER_READ, oPCRNumber, value, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpGetiPCR(uint32 iPCRNumber, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetiPCR(uint32 iPCRNumber, uint32 *value)
								{
									return cmpHandleRegister(CMP_IPCR_REGISTER_READ, iPCRNumber, value, NULL, NULL);
								}

														// set value of a PCR register (using compare-swap lock)
_TC_INLINE HRESULT	cmpSetoMPR(uint32 compareValue, uint32 newValue, uint32 *oldValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpSetoMPR(uint32 compareValue, uint32 newValue, uint32 *oldValue)
								{
									return cmpHandleRegister(CMP_OMPR_REGISTER_LOCK, 0, &compareValue, &newValue, oldValue);
								}

_TC_INLINE HRESULT	cmpSetiMPR(uint32 compareValue, uint32 newValue, uint32 *oldValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpSetiMPR(uint32 compareValue, uint32 newValue, uint32 *oldValue)
								{
									return cmpHandleRegister(CMP_IMPR_REGISTER_LOCK, 0, &compareValue, &newValue, oldValue);
								}

_TC_INLINE HRESULT	cmpSetoPCR(uint32 oPCRNumber, uint32 compareValue, uint32 newValue, uint32 *oldValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpSetoPCR(uint32 oPCRNumber, uint32 compareValue, uint32 newValue, uint32 *oldValue)
								{
									return cmpHandleRegister(CMP_OPCR_REGISTER_LOCK, oPCRNumber, &compareValue, &newValue, oldValue);
								}

_TC_INLINE HRESULT	cmpSetiPCR(uint32 iPCRNumber, uint32 compareValue, uint32 newValue, uint32 *oldValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpSetiPCR(uint32 iPCRNumber, uint32 compareValue, uint32 newValue, uint32 *oldValue)
								{
									return cmpHandleRegister(CMP_IPCR_REGISTER_LOCK, iPCRNumber, &compareValue, &newValue, oldValue);
								}


														// resets value of a plug register
_TC_INLINE HRESULT	cmpResetoMPR(uint32 resetValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpResetoMPR(uint32 resetValue)
								{
									return cmpHandleRegister(CMP_OMPR_REGISTER_RESET, 0, &resetValue, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpResetiMPR(uint32 resetValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpResetiMPR(uint32 resetValue)
								{
									return cmpHandleRegister(CMP_IMPR_REGISTER_RESET, 0, &resetValue, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpResetoPCR(uint32 oPCRNumber, uint32 resetValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpResetoPCR(uint32 oPCRNumber, uint32 resetValue)
								{
									return cmpHandleRegister(CMP_OPCR_ALL_RESET, oPCRNumber, &resetValue, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpResetiPCR(uint32 iPCRNumber, uint32 resetValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpResetiPCR(uint32 iPCRNumber, uint32 resetValue)
								{
									return cmpHandleRegister(CMP_IPCR_ALL_RESET, iPCRNumber, &resetValue, NULL, NULL);
								}

														// get/set the PCR format (update avs if required)
_TC_INLINE HRESULT	cmpGetoPCRFormat(uint32 oPCRNumber, uint32 *format) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetoPCRFormat(uint32 oPCRNumber, uint32 *format)
								{
									return cmpHandleRegister(CMP_OPCR_FORMAT_READ, oPCRNumber, format, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpGetiPCRFormat(uint32 iPCRNumber, uint32 *format) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetiPCRFormat(uint32 iPCRNumber, uint32 *format)
								{
									return cmpHandleRegister(CMP_IPCR_FORMAT_READ, iPCRNumber, format, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpSetoPCRFormat(uint32 oPCRNumber, uint32 format) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpSetoPCRFormat(uint32 oPCRNumber, uint32 format)
								{
									return cmpHandleRegister(CMP_OPCR_FORMAT_WRITE, oPCRNumber, &format, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpSetiPCRFormat(uint32 iPCRNumber, uint32 format) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpSetiPCRFormat(uint32 iPCRNumber, uint32 format)
								{
									return cmpHandleRegister(CMP_IPCR_FORMAT_WRITE, iPCRNumber, &format, NULL, NULL);
								}

														// get the state of a PCR
_TC_INLINE HRESULT	cmpGetoPCRState(uint32 oPCRNumber, CMP_PCR_STATE *state) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetoPCRState(uint32 oPCRNumber, CMP_PCR_STATE *state)
								{
									return cmpHandleRegister(CMP_OPCR_STATE_READ, oPCRNumber, (uint32 *) state, NULL, NULL);
								}

_TC_INLINE HRESULT	cmpGetiPCRState(uint32 iPCRNumber, CMP_PCR_STATE *state) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpGetiPCRState(uint32 iPCRNumber, CMP_PCR_STATE *state)
								{
									return cmpHandleRegister(CMP_IPCR_STATE_READ, iPCRNumber, (uint32 *) state, NULL, NULL);
								}

														// handling of local/remote plug registers
HRESULT				cmpHandleNodeRegister(uint32 operation, uint32 index, uint16 nodeAddr, uint32 *value1, uint32 *value2, BOOL bHighPriority);

_TC_INLINE HRESULT	cmpReadiMPR(uint16 nodeAddr, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpReadiMPR(uint16 nodeAddr, uint32 *value)
								{
									return cmpHandleNodeRegister(CMP_IMPR_REGISTER_READ, 0, nodeAddr, value, NULL, FALSE);
								}

_TC_INLINE HRESULT	cmpReadoMPR(uint16 nodeAddr, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpReadoMPR(uint16 nodeAddr, uint32 *value)
								{
									return cmpHandleNodeRegister(CMP_OMPR_REGISTER_READ, 0, nodeAddr, value, NULL, FALSE);
								}

_TC_INLINE HRESULT	cmpReadiPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpReadiPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 *value)
								{
									return cmpHandleNodeRegister(CMP_IPCR_REGISTER_READ, PCRNumber, nodeAddr, value, NULL, FALSE);
								}

_TC_INLINE HRESULT	cmpReadoPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpReadoPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 *value)
								{
									return cmpHandleNodeRegister(CMP_OPCR_REGISTER_READ, PCRNumber, nodeAddr, value, NULL, FALSE);
								}

_TC_INLINE HRESULT	cmpReadiPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 *value, BOOL bHighPriority) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpReadiPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 *value, BOOL bHighPriority)
								{
									return cmpHandleNodeRegister(CMP_IPCR_REGISTER_READ, PCRNumber, nodeAddr, value, NULL, bHighPriority);
								}

_TC_INLINE HRESULT	cmpReadoPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 *value, BOOL bHighPriority) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpReadoPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 *value, BOOL bHighPriority)
								{
									return cmpHandleNodeRegister(CMP_OPCR_REGISTER_READ, PCRNumber, nodeAddr, value, NULL, bHighPriority);
								}

_TC_INLINE HRESULT	cmpLockiMPR(uint16 nodeAddr, uint32 compareValue, uint32 newValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpLockiMPR(uint16 nodeAddr, uint32 compareValue, uint32 newValue)
								{
									return cmpHandleNodeRegister(CMP_IMPR_REGISTER_LOCK, 0, nodeAddr, &compareValue, &newValue, FALSE);
								}

_TC_INLINE HRESULT	cmpLockoMPR(uint16 nodeAddr, uint32 compareValue, uint32 newValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpLockoMPR(uint16 nodeAddr, uint32 compareValue, uint32 newValue)
								{
									return cmpHandleNodeRegister(CMP_OMPR_REGISTER_LOCK, 0, nodeAddr, &compareValue, &newValue, FALSE);
								}

_TC_INLINE HRESULT	cmpLockiPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpLockiPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue)
								{
									return cmpHandleNodeRegister(CMP_IPCR_REGISTER_LOCK, PCRNumber, nodeAddr, &compareValue, &newValue, FALSE);
								}

_TC_INLINE HRESULT	cmpLockoPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpLockoPCR(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue)
								{
									return cmpHandleNodeRegister(CMP_OPCR_REGISTER_LOCK, PCRNumber, nodeAddr, &compareValue, &newValue, FALSE);
								}

_TC_INLINE HRESULT	cmpLockiPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue, BOOL bHighPriority) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpLockiPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue, BOOL bHighPriority)
								{
									return cmpHandleNodeRegister(CMP_IPCR_REGISTER_LOCK, PCRNumber, nodeAddr, &compareValue, &newValue, bHighPriority);
								}

_TC_INLINE HRESULT	cmpLockoPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue, BOOL bHighPriority) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	cmpLockoPCRWithPriority(uint32 PCRNumber, uint16 nodeAddr, uint32 compareValue, uint32 newValue, BOOL bHighPriority)
								{
									return cmpHandleNodeRegister(CMP_OPCR_REGISTER_LOCK, PCRNumber, nodeAddr, &compareValue, &newValue, bHighPriority);
								}


														// update avs from the value of a PCR register and format
HRESULT				cmpUpdateoMPR(void);
HRESULT				cmpUpdateiMPR(void);
HRESULT				cmpUpdateoPCR(uint32 oPCRNumber);
HRESULT				cmpUpdateiPCR(uint32 iPCRNumber);
				
														// functions for getting specific parts of MPR register
_TC_INLINE uint32		cmpGetMPRRateCap(uint32 MPR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetMPRRateCap(uint32 MPR)
								{
									return ((MPR & oMPR_MASK_RATECAP) >> oMPR_SHIFT_RATECAP);				// same for iMPR and oMPR
								}

_TC_INLINE uint32		cmpGetMPRBCBase(uint32 MPR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetMPRBCBase(uint32 MPR)
								{
									return ((MPR & oMPR_MASK_BCBASE) >> oMPR_SHIFT_BCBASE);					// only valid for oMPR
								}

_TC_INLINE uint32		cmpGetMPRNumPlugs(uint32 MPR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetMPRNumPlugs(uint32 MPR)
								{
									return ((MPR & oMPR_MASK_OUTPLUGS) >> oMPR_SHIFT_OUTPLUGS);				// same for iMPR and oMPR
								}

														// functions for setting specific parts of MPR register
_TC_INLINE void		cmpSetMPRRateCap(uint32 *MPR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetMPRRateCap(uint32 *MPR, uint32 value)
								{
									*MPR = (*MPR & (~oMPR_MASK_RATECAP)) | (value << oMPR_SHIFT_RATECAP);	// same for iMPR and oMPR
								}

_TC_INLINE void		cmpSetMPRBCBase(uint32 *MPR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetMPRBCBase(uint32 *MPR, uint32 value)
								{
									*MPR = (*MPR & (~oMPR_MASK_BCBASE)) | (value << oMPR_SHIFT_BCBASE);		// only valid for oMPR
								}

_TC_INLINE void		cmpSetMPRNumPlugs(uint32 *MPR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetMPRNumPlugs(uint32 *MPR, uint32 value)
								{
									*MPR = (*MPR & (~oMPR_MASK_OUTPLUGS)) | (value << oMPR_SHIFT_OUTPLUGS);	// same for iMPR and oMPR
								}

														// functions for getting specific parts of PCR register
_TC_INLINE uint32		cmpGetPCROnline(uint32 PCR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetPCROnline(uint32 PCR)
								{
									return ((PCR & oPCR_MASK_ONLINE) >> oPCR_SHIFT_ONLINE);
								}

_TC_INLINE uint32		cmpGetPCRBCCount(uint32 PCR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetPCRBCCount(uint32 PCR)
								{
									return ((PCR & oPCR_MASK_BCCOUNT) >> oPCR_SHIFT_BCCOUNT);
								}

_TC_INLINE uint32		cmpGetPCRP2PCount(uint32 PCR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetPCRP2PCount(uint32 PCR)
								{
									return ((PCR & oPCR_MASK_P2PCOUNT) >> oPCR_SHIFT_P2PCOUNT);
								}

uint32				cmpGetPCRConnCount(uint32 PCR);
uint32				cmpGetPCRState(uint32 PCR);

_TC_INLINE uint32		cmpGetPCRChannel(uint32 PCR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetPCRChannel(uint32 PCR)
								{
									return ((PCR & oPCR_MASK_CHANNEL) >> oPCR_SHIFT_CHANNEL);
								}

_TC_INLINE uint32		cmpGetPCRRate(uint32 PCR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetPCRRate(uint32 PCR)
								{
									return ((PCR & oPCR_MASK_RATE) >> oPCR_SHIFT_RATE);						// only valid for oPCR
								}

_TC_INLINE uint32		cmpGetPCROverheadID(uint32 PCR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetPCROverheadID(uint32 PCR)
								{
									return ((PCR & oPCR_MASK_OVERHEAD) >> oPCR_SHIFT_OVERHEAD);				// only valid for oPCR
								}

_TC_INLINE uint32		cmpGetPCRPayload(uint32 PCR) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		cmpGetPCRPayload(uint32 PCR)
								{
									return ((PCR & oPCR_MASK_PAYLOAD) >> oPCR_SHIFT_PAYLOAD);				// only valid for oPCR
								}

uint32				cmpGetPCRBandwidth(uint32 PCR);															// only valid for oPCR

														// functions for setting specific parts of PCR register
_TC_INLINE void		cmpSetPCROnline(uint32 *PCR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetPCROnline(uint32 *PCR, uint32 value)
								{
									*PCR = (*PCR & (~oPCR_MASK_ONLINE)) | (value << oPCR_SHIFT_ONLINE);
								}

_TC_INLINE void		cmpSetPCRBCCount(uint32 *PCR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetPCRBCCount(uint32 *PCR, uint32 value)
								{
									*PCR = (*PCR & (~oPCR_MASK_BCCOUNT)) | (value << oPCR_SHIFT_BCCOUNT);
								}

_TC_INLINE void		cmpSetPCRP2PCount(uint32 *PCR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetPCRP2PCount(uint32 *PCR, uint32 value)
								{
									*PCR = (*PCR & (~oPCR_MASK_P2PCOUNT)) | (value << oPCR_SHIFT_P2PCOUNT);
								}

_TC_INLINE void		cmpSetPCRChannel(uint32 *PCR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetPCRChannel(uint32 *PCR, uint32 value)
								{
									*PCR = (*PCR & (~oPCR_MASK_CHANNEL)) | (value << oPCR_SHIFT_CHANNEL);
								}

_TC_INLINE void		cmpSetPCRRate(uint32 *PCR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetPCRRate(uint32 *PCR, uint32 value)
								{
									*PCR = (*PCR & (~oPCR_MASK_RATE)) | (value << oPCR_SHIFT_RATE);						// only valid for oPCR
								}

_TC_INLINE void		cmpSetPCROverheadID(uint32 *PCR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetPCROverheadID(uint32 *PCR, uint32 value)
								{
									*PCR = (*PCR & (~oPCR_MASK_OVERHEAD)) | (value << oPCR_SHIFT_OVERHEAD);				// only valid for oPCR
								}

_TC_INLINE void		cmpSetPCRPayload(uint32 *PCR, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void		cmpSetPCRPayload(uint32 *PCR, uint32 value)
								{
									*PCR = (*PCR & (~oPCR_MASK_PAYLOAD)) | (value << oPCR_SHIFT_PAYLOAD);				// only valid for oPCR
								}

#endif //_CMPUTILS_H


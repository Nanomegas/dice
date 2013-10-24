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
 * DESCRIPTION: Target Plugs

	Defines the plug characteristics of the particular target
	
	LM??? These interfaces are temporary and will be replaced by a plug manager
	module at a later stage.

	Revisions:
		created 09/19/2002 lm

******************************************************************************/

#ifndef _TARGETPLUGS_H
#define _TARGETPLUGS_H
// #ifdef _AVC //cmp needs avc

#include "1394.h"
#include "packetBlock.h"
#include "TCTypes.h"
#include "cmp.h"
#include "drd.h"
#include "avcUnitUtils.h"
#include "avcCmdStreamFormat.h"
#include "dal.h"

typedef enum
{
	CCM_INPUT_PLUG_STATE_ACTIVE,
	CCM_INPUT_PLUG_STATE_READY,
	CCM_INPUT_PLUG_STATE_NO_SELECTION,
	CCM_INPUT_PLUG_STATE_INSUFFICIENT_RESOURCE
} INPUT_PLUG_STATUS;

typedef enum
{
	CCM_OUTPUT_PLUG_STATUS_EFFECTIVE,
	CCM_OUTPUT_PLUG_STATUS_READY,
	CCM_OUTPUT_PLUG_STATUS_NOT_EFFECTIVE,
	CCM_OUTPUT_PLUG_STATUS_INSUFFICIENT_RESOURCE,
	CCM_OUTPUT_PLUG_STATUS_VIRTUAL
} OUTPUT_PLUG_STATUS;

// Following struct to store status of a connection, and a few attributes too
typedef	struct 
{
	int							active:1, locked:1, permanent:1; //LM???
} CONN_STATUS;

typedef struct _AVC_SIGNAL_PATH
{
	BYTE				dest_plug_id;		// destination_plug_id
	BYTE				dest_su;			// destination subunit and subunit_id
	BYTE				dest_sync;			// is the dest plug a sync plug?
	BYTE				source_plug_id;		// source_plug_id
	BYTE				source_su;			// source subunit and subunit_id
	BYTE				source_sync;		// is the source plug a possible sync source?
	DAL_CLOCK_SOURCE_ENUM	clk_src;		// corresponding clock source
	BYTE				status;				// path status
} AVC_SIGNAL_PATH;


typedef struct _SUBUNIT_PLUGS
{
	AVC_ADDRESS_SUBUNIT			addrSubunit;
	uint8						numSrcPlugs;
	uint8						numDstPlugs;
} SUBUNIT_PLUGS;

// Serial Bus Plugs (master plug registers) are only visable within the cmp module, 
// although they are instantiated in targetPlugs.c
extern PCR_REGISTER				iMPRRegister;	    
extern PCR_REGISTER				oMPRRegister;
extern PCR_REGISTER				*iPCRRegisters;
extern PCR_REGISTER				*oPCRRegisters; 

// extended plug format info
HRESULT		targetGetExtPlugFmtInfo(BYTE plug_addr_mode, BYTE plug_type, BYTE plug_dir, BYTE plug_ID, BYTE subunit_type, AVC_PLUG_EXT_STREAM_FORMAT **plugExtInfo);

// Plug Information Accessors
uint32		plugsGetNumIsochInDTCPPlugs(void);
uint32		plugsGetNumIsochOutDTCPPlugs(void);
uint32		plugsGetNumIsochInPlugs(void);
uint32		plugsGetNumIsochOutPlugs(void);
uint32		plugsGetNumAsyncInPlugs(void);
uint32		plugsGetNumAsyncOutPlugs(void);
uint32		plugsGetNumExtInPlugs(void);
uint32		plugsGetNumExtOutPlugs(void);
HRESULT		plugsGetBroadcastInPCRNumber(uint32 *iPCRNumber);
HRESULT		plugsGetBroadcastOutPCRNumber(uint32 *oPCRNumber);

// Note that external plugs are not exposed, except via accessors
HRESULT		targetGetSubunitPlugs(AVC_ADDRESS_SUBUNIT *address, uint8 *numSrcPlugs, uint8 *numDstPlugs);
HRESULT		targetFindPlug(AVC_CONNECTION *connection);
HRESULT		targetConnectPlugs(AVC_CONNECTION *connection);
HRESULT		targetDisconnectPlugs(AVC_CONNECTION *connection);
uint32		targetGetNumConnections(void);
HRESULT		targetGetConnections(uint8 *numConnections, uint8 maxConnections, AVC_CONNECTION *connectionArray);

HRESULT		targetChoosePlug(AVC_CONNECTION *connection, BOOL source);
HRESULT		targetPlugExists(AVC_CONNECTION *connection, BOOL source);

HRESULT		targetGetSubunitAddress(AVC_ADDRESS_SUBUNIT *subunit);
HRESULT		targetGetUnitInfoAddress(AVC_ADDRESS_SUBUNIT *subunit);
HRESULT		targetGetUnitInfoCompanyID(uint32 *company_ID);
HRESULT		targetGetSubunitInfoEntry(uint32 index, AVC_ADDRESS_SUBUNIT *subunit);
HRESULT		targetGetSubunitVersionInfo(AVC_ADDRESS_SUBUNIT *subunit, BYTE *version_information, BYTE *su_dependent_field);

HRESULT		targetSignalSourceFindPath(BYTE dest_plug_id, BYTE dest_su, BYTE *source_plug_id, BYTE *source_su, BYTE *status, AVC_SIGNAL_PATH **path);
BOOL		targetPlugIsSyncPlug(BYTE dst_su, BYTE dst_plug_id);
BOOL		targetPlugIsSyncSource(BYTE src_su, BYTE src_plug_id);
HRESULT		targetPlugGetClockSource(BYTE src_su, BYTE src_plug_id, DAL_CLOCK_SOURCE_ENUM *clockVal);

//#endif //_AVC
#endif //_TARGETPLUGS_H

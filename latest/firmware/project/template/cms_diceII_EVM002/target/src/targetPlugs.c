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
	
	Serial Bus Plugs  		- instantiation of the PCRs
	Subunit Plugs	  		- instantiation of subunit plugs	
	Internal Connections
		Fixed				- Fixed connections
		Flexi				- inter-plug connections that are possible
	External Plugs			- how many are present
	Plug info accessors

	LM??? These interfaces are temporary and will be replaced by a plug manager
	module at a later stage.

	Revisions:
		created 09/19/2002 lm

******************************************************************************/
#ifdef _AVC //this file needs AVC enabled.
static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcCmdStreamFormat.h"
#include "targetPlugs.h"
#include "avcDefs.h"
#include "drd.h"
#include "avs.h"
#include "cmp.h"
#include "targetVendorDefs.h"

/*********************************************************************************
	Serial Bus Plugs
	
	Note: The number of PCRs depends on the link layer hardware

**********************************************************************************/

/* Instantiation of the PCRs */


#ifdef _DICE2
#if 1 //def _DICE2 LM??? for DICE-2 update TCTasking

#undef  _VIDEO

#ifdef _VIDEO
#define iPCR_ITEMS		5						// 4 iPCR for Audio + 1 iPCR for Video
#define oPCR_ITEMS		3						// 2 oPCR for Audio + 1 oPCR for Video
#else //_VIDEO
// #define iPCR_ITEMS		4						// 4 iPCR for Audio
// #define oPCR_ITEMS		2						// 2 oPCR for Audio
//#define iPCR_ITEMS		2						// 2 iPCR for Music = 1 Iso + 1 Sync
//#define oPCR_ITEMS		2						// 2 oPCR for Music = 1 Iso + 1 Sync
#define iPCR_ITEMS		1						// 1 iPCR for Music
#define oPCR_ITEMS		1						// 1 oPCR for Music
#endif //_VIDEO

PCR_REGISTER icPCRRegisters[iPCR_ITEMS] = 
{	
	//	Initial plug state
	{											// Isoch Input Port 0 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x1002FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 48KHz)
		TRUE,			// bIn
		0,				// plugID
		FALSE			// bMarked
	}/* ,
	{											// Isoch Input Port 1 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x1002FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 48KHz)
		TRUE,			// bIn
		1,				// plugID
		FALSE			// bMarked
	} ,
	{											// Isoch Input Port 2 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x1002FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 48KHz)
		TRUE,			// bIn
		2,				// plugID
		FALSE			// bMarked
	},
	{											// Isoch Input Port 3 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x1002FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 48KHz)
		TRUE,			// bIn
		3,				// plugID
		FALSE			// bMarked
	} */
#ifdef _VIDEO
	,{											// Isoch Input Port for Video
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x00000000,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x00, fdf:24:0x000000]
		TRUE,			// bIn
		4,				// plugID
		FALSE			// bMarked
	}
#endif //_VIDEO
};

PCR_REGISTER ocPCRRegisters[oPCR_ITEMS] = 
{	
	//	Initial plug state
	{											// Isoch Output Port 0 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80010000,		// plugRegister
		0x1002FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 48KHz)
		FALSE,			// bIn
		0,				// plugID
		FALSE			// bMarked
	} /*,
	{											// Isoch Output Port 1 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x1002FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 48KHz)
		FALSE,			// bIn
		1,				// plugID
		FALSE			// bMarked
	} */
#ifdef _VIDEO
	,{											// Isoch Output Port for Video
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x00000000,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x00, fdf:24:0x000000]
		FALSE,			// bIn
		2,				// plugID
		FALSE			// bMarked
	}
#endif //_VIDEO
};

#else //.. //LM??? for DICE-2 update TCTasking

#define iPCR_ITEMS		1			// 1 iPCR Test
#define oPCR_ITEMS		1			// 1 oPCR Test

#if iPCR_ITEMS
PCR_REGISTER icPCRRegisters[iPCR_ITEMS] = 
{	
	//	Initial plug state
	{											// Isoch Input Port 0 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 44KHz)
		TRUE,			// bIn
		0,				// plugID
		FALSE			// bMarked
	},
};
#endif //iPCR_ITEMS

#if oPCR_ITEMS
PCR_REGISTER ocPCRRegisters[oPCR_ITEMS] = 
{	
	//	Initial plug state
	{											// Isoch Output Port 0 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80000000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 44KHz)
		FALSE,			// bIn
		0,				// plugID
		FALSE			// bMarked
	},
};
#endif //oPCR_ITEMS
#endif //.. _DICE2
#else //_DICE2

#define iPCR_ITEMS		4						// 4 iPCR for Audio + 0 iPCR for Video
#define oPCR_ITEMS		2						// 2 oPCR for Audio + 0 oPCR for Video

PCR_REGISTER icPCRRegisters[iPCR_ITEMS] = 
{	
	//	Initial plug state
	{											// Isoch Input Port 0 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80008000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x010000] (audio 44KHz)
		TRUE,			// bIn
		0,				// plugID
		FALSE			// bMarked
	},
	{											// Isoch Input Port 1 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80008000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 44KHz)
		TRUE,			// bIn
		1,				// plugID
		FALSE			// bMarked
	},
	{											// Isoch Input Port 2 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80008000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 44KHz)
		TRUE,			// bIn
		2,				// plugID
		FALSE			// bMarked
	},
	{											// Isoch Input Port 3 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80008000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 44Hz)
		TRUE,			// bIn
		3,				// plugID
		FALSE			// bMarked
	}
};

PCR_REGISTER ocPCRRegisters[oPCR_ITEMS] = 
{	
	//	Initial plug state
	{											// Isoch Output Port 0 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80008000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x010000] (audio 44KHz)
		FALSE,			// bIn
		0,				// plugID
		FALSE			// bMarked
	},
	{											// Isoch Output Port 1 for Audio
		0,				// plugMutexSemID		(initialized in cmpInitialize)
		OWNER_LOCAL,	// plugOwner
		0x80008000,		// plugRegister
		0x1001FFFF,		// plugFormat			[eoh:1=0, form:1=0, fmt:6:0x10, fdf:24:0x020000] (audio 44KHz)
		FALSE,			// bIn
		1,				// plugID
		FALSE			// bMarked
	}
};
#endif //_DICE2


#if iPCR_ITEMS
PCR_REGISTER *iPCRRegisters = icPCRRegisters;
#else //iPCR_ITEMS
PCR_REGISTER *iPCRRegisters = NULL;
#endif //iPCR_ITEMS

#if oPCR_ITEMS
PCR_REGISTER *oPCRRegisters = ocPCRRegisters;
#else //oPCR_ITEMS
PCR_REGISTER *oPCRRegisters = NULL;
#endif //oPCR_ITEMS

/* instantiation of the MPRs  */

PCR_REGISTER iMPRRegister = 
{
	0,								// plugMutexSemID (initialized in cmpInitialize)
	OWNER_LOCAL,					// plugOwner
	0x80FF0000+iPCR_ITEMS,			// plugRegister
	0,								// plugFormat
	TRUE,							// bIn
	0,								// plugID
	FALSE							// bMarked
};

PCR_REGISTER oMPRRegister = 
{	
	0,								// plugMutexSemID (initialized in cmpInitialize)
	OWNER_LOCAL,					// plugOwner
	0x80FF0000+oPCR_ITEMS,			// plugRegister
	0,								// plugFormat
	FALSE,							// bIn
	0,								// plugID
	FALSE							// bMarked
};

/********************************************************************************
	External Plugs

Note: Eventual external plug format information will go here too
Change #defines for number of external plugs on target
*********************************************************************************/

#define ASYNC_INPUT_PLUG_ITEMS		0
#define ASYNC_OUTPUT_PLUG_ITEMS		0

#define EXT_INPUT_PLUG_ITEMS		5
#define EXT_OUTPUT_PLUG_ITEMS		5

#define SIG_SU_ID_0							0
#define SIG_SU_ID_1							1
#define SIG_UNIT							0xFF
#define SIG_AUDIO_SU_0						((AVC_SU_TYPE_AUDIO<<3) | SIG_SU_ID_0)
#define SIG_MUSIC_SU_0						((AVC_SU_TYPE_MUSIC<<3) | SIG_SU_ID_0)
#define SIG_PLUG_EXTERNAL					0x80
#define SIG_STATUS_EFFECTIVE				0
#define SIG_STATUS_NOT_EFFECTIVE			(1<<5)
#define SIG_STATUS_ACTIVE					(2<<5)
#define SIG_STATUS_READY					(3<<5)
#define SIG_STATUS_NO_SELECTION				(4<<5)
#define SIG_STATUS_INSUFFICIENT_RESOURCE	(5<<5)
#define SIG_CONV_FIXED_TRANSFER_FMT			0x10
#define SIG_CONV_CHANGEABLE_TRANSFER_FMT	0x00

//  to make a signal path map, list all destination plug id's for the unit and all subunits,
//	then fill in each of their corresponding source plugs
AVC_SIGNAL_PATH signalPaths[] = 
{
//	dst_plug_id,				dst_su			source_plug_id,				src_su			status				// dest plug
	{ 0x00,						SIG_UNIT,		0x00,						SIG_MUSIC_SU_0,	SIG_STATUS_READY },	// unit oPCR0
//	{ 0x01,						SIG_UNIT,		0x06,						SIG_MUSIC_SU_0,	SIG_STATUS_READY }, // unit oPCR1
	{ 0x00 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		0x00,						SIG_AUDIO_SU_0,	SIG_STATUS_READY },	// unit external 0
	{ 0x01 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		0x01,						SIG_AUDIO_SU_0,	SIG_STATUS_READY },	// unit external 1
	{ 0x02 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		0x02,						SIG_AUDIO_SU_0,	SIG_STATUS_READY },	// unit external 2
	{ 0x03 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		0x03,						SIG_AUDIO_SU_0,	SIG_STATUS_READY },	// unit external 3
	{ 0x04 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		0x05,						SIG_MUSIC_SU_0,	SIG_STATUS_READY },	// unit external 4 midi
	{ 0x00,						SIG_AUDIO_SU_0,	0x01,						SIG_MUSIC_SU_0,	SIG_STATUS_READY },	// audio su 0
	{ 0x01,						SIG_AUDIO_SU_0,	0x02,						SIG_MUSIC_SU_0,	SIG_STATUS_READY },	// audio su 1
	{ 0x02,						SIG_AUDIO_SU_0,	0x03,						SIG_MUSIC_SU_0,	SIG_STATUS_READY },	// audio su 2
	{ 0x03,						SIG_AUDIO_SU_0,	0x04,						SIG_MUSIC_SU_0,	SIG_STATUS_READY },	// audio su 3
	{ 0x00,						SIG_MUSIC_SU_0,	0x00,						SIG_UNIT,		SIG_STATUS_READY },	// music su 0
	{ 0x01,						SIG_MUSIC_SU_0,	0x00 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		SIG_STATUS_READY },	// music su 1
	{ 0x02,						SIG_MUSIC_SU_0,	0x01 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		SIG_STATUS_READY },	// music su 2
	{ 0x03,						SIG_MUSIC_SU_0,	0x02 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		SIG_STATUS_READY },	// music su 3
	{ 0x04,						SIG_MUSIC_SU_0,	0x03 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		SIG_STATUS_READY },	// music su 4
	{ 0x05,						SIG_MUSIC_SU_0,	0x04 | SIG_PLUG_EXTERNAL,	SIG_UNIT,		SIG_STATUS_READY }	// music su 5 midi
//	{ 0x06,						SIG_MUSIC_SU_0,	0x01,						SIG_UNIT,		SIG_STATUS_READY }	// music su 6 sync: connect to iPCR0 to get Host to send non-blocking
};

// TODO: use #defined values
AVC_PLUG_EXT_STREAM_FORMAT unitInputPlugFormats[] = 
{
//	{ mode,	addr_data		status,	root,	L1,		s/r,	sync, #fmts, formats
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 6,	 {{0x02, 0x06},			// unit input 0, 9 ch compound
																		  {0x02, 0x06},
																		  {0x02, 0x06},
																		  {0x02, 0x06},
																		  {0x01, 0x0d}} }	//,
//	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	  {0x02, 0x06} },		// unit input 1, sync
};

AVC_PLUG_EXT_STREAM_FORMAT unitOutputPlugFormats[] = 
{
//	{ addr_data				status,	root,	L1,		s/r,	sync, #fmts, formats
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 6,	 {{0x02, 0x06},			// unit output 0, 9 ch compound
																		  {0x02, 0x06},
																		  {0x02, 0x06},
																		  {0x02, 0x06},
																		  {0x01, 0x0d}} }	//,
//	{{0x0, 0x06, 0xff},		0x01,	0x90,	0x00,	0x04,	0x01, 1,	  {0x02, 0x06} },		// unit output 1, sync
};

AVC_PLUG_EXT_STREAM_FORMAT audioSubunitInputPlugFormats[] = 
{
//	{ addr_data				status,	root,	L1,		s/r,	sync, #fmts, formats
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit input 0, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit input 1, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit input 2, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit input 3, 2ch audio

	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit output 0, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit output 1, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit output 2, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// audio subunit output 3, 2ch audio
};

AVC_PLUG_EXT_STREAM_FORMAT musicSubunitInputPlugFormats[] = 
{
//	{ addr_data				status,	root,	L1,		s/r,	sync, #fmts, formats
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {{0x02, 0x06},			// music subunit input 0, 9 ch compound
																		 {0x02, 0x06},
																		 {0x02, 0x06},
																		 {0x02, 0x06},
																		 {0x01, 0x0d}} },
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit input 1, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit input 2, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit input 3, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit input 4, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} } //,		// music subunit input 5, midi
//	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit input 6, sync
};

AVC_PLUG_EXT_STREAM_FORMAT musicSubunitOutputPlugFormats[] = 
{
//	{ addr_data				status,	root,	L1,		s/r,	sync, #fmts, formats
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {{0x02, 0x06},			// music subunit output 0, 9 ch compound
																		 {0x02, 0x06},
																		 {0x02, 0x06},
																		 {0x02, 0x06},
																		 {0x01, 0x0d}} },
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit output 1, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit output 2, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit output 3, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} },		// music subunit output 4, 2ch audio
	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} } //,		// music subunit output 5, midi
//	{ {0x0, 0x06, 0xff},	0x01,	0x90,	0x40,	0x04,	0x01, 1,	 {0x02, 0x06} }			// music subunit output 6, sync
};

HRESULT targetSignalSourceFindPath(BYTE dest_plug_id, BYTE dest_su, BYTE *source_plug_id, BYTE *source_su, BYTE *status)
{
	HRESULT	hResult = NO_ERROR;
	int i = 0;

	for (i=0;i<ARRAYSIZE(signalPaths);i++)
	{
		if ((signalPaths[i].dest_plug_id == dest_plug_id) && (signalPaths[i].dest_su == dest_su))
		{
			*source_plug_id = signalPaths[i].source_plug_id;
			*source_su = signalPaths[i].source_su;
			*status = signalPaths[i].status;

			return hResult;
		}
	}

	return E_FAIL;
}

/********************************************************************************
	Plug Information Accessor Functions
	Don't fiddle with these for each target - these are generic
*********************************************************************************/

uint32 plugsGetNumIsochInDTCPPlugs(void)
{	
	return iPCR_ITEMS;
} 

uint32 plugsGetNumIsochOutDTCPPlugs(void)
{
	return oPCR_ITEMS;
}

uint32 plugsGetNumIsochInPlugs(void)
{	
	return iPCR_ITEMS;
} 

uint32 plugsGetNumIsochOutPlugs(void)
{
	return oPCR_ITEMS;
}

uint32 plugsGetNumAsyncInPlugs(void)
{
	return ASYNC_INPUT_PLUG_ITEMS;
}

uint32 plugsGetNumAsyncOutPlugs(void)
{
	return ASYNC_OUTPUT_PLUG_ITEMS;
}

uint32 plugsGetNumExtInPlugs(void)
{
	return EXT_INPUT_PLUG_ITEMS;
}

uint32 plugsGetNumExtOutPlugs(void)
{
	return EXT_OUTPUT_PLUG_ITEMS;
}

HRESULT plugsGetBroadcastInPCRNumber(uint32 *iPCRNumber)
{
	HRESULT		hResult = NO_ERROR;

	*iPCRNumber = 0;	//LM??? get iPCR setup for broadcast in specified with channel etc.

	return hResult;
}

HRESULT plugsGetBroadcastOutPCRNumber(uint32 *oPCRNumber)
{
	HRESULT		hResult = NO_ERROR;

	*oPCRNumber = 0;	//LM??? get iPCR setup for broadcast out specified with (channel?), bandwidth etc.

	return hResult;
}

/**************************************************************************
	Subunit Plugs
	Change this for each target
	This information allows commands such as PLUG_INFO to be generic in 
	each subunit
**************************************************************************/

AVC_ADDRESS_SUBUNIT targetSubunitInfo[] =
{
	{AVC_SU_TYPE_AUDIO, 0},		// subunit_type, max_subunit_ID
	{AVC_SU_TYPE_MUSIC, 0}		// subunit_type, max_subunit_ID
};

SUBUNIT_PLUGS targetSubunitPlugs[] =
{
	{{AVC_SU_TYPE_AUDIO, 0}, 4, 4},
//	{{AVC_SU_TYPE_MUSIC, 0}, 7, 7}
	{{AVC_SU_TYPE_MUSIC, 0}, 6, 6}
};

/**************************************************************************
	Internal connections
	Instantiate arrays here for each target configuration
	FixedPlugs are those that cannot be changed (will REJECT a CONNECT)
	FlexiPlugs are those that be changed by INPUT SELECT or CONNECT

	Note: Multiple connections from the same source must be sequential 
	in the table - this is for efficient searching purposes.
***************************************************************************/

#define FIXED_CONNECTION_ITEMS	0

#if 0
AVC_PLUG_CONNECTION targetFixedConnections[FIXED_CONNECTION_ITEMS] = 
{
//	lock	perm	src			dst
	{0,		0,		{0,	0},		{0,	0},}
};
#endif

#define FLEXI_CONNECTION_ITEMS	7

typedef struct
{
	BOOL					bActive;						// connection is active
	AVC_CONNECTION			connection;
} AVC_FLEXI_CONNECTION;

static AVC_FLEXI_CONNECTION targetFlexiConnections[FLEXI_CONNECTION_ITEMS] =
{
//	active	lock	perm	src						dst
	{0,		{0,		0,		{AVC_PLUG_ISOCH_0,		AVC_PLUG_AUDIO_0_0}}},	// DEV_TEST,			0,
	{0,		{0,		0,		{AVC_PLUG_EXT_0,		AVC_PLUG_AUDIO_0_0}}},	// DEV_VIDEO,			0,
	{0,		{0,		0,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_EXT_0}}},		// DEV_VIDEO,			0,
	{0,		{0,		0,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_EXT_1}}},		// DEV_DIGITAL_AUDIO,	0,
	{0,		{0,		0,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_EXT_2}}},		// DEV_ANALOG_SPEAKERS,	0,
	{0,		{0,		0,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_EXT_3}}},		// DEV_ANALOG_LINE,		0,
	{0,		{0,		0,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_EXT_4}}}		// DEV_ANALOG_LINE,		1,
};

HRESULT targetGetSubunitPlugs(AVC_ADDRESS_SUBUNIT *addrSubunit, uint8 *numSrcPlugs, uint8 *numDstPlugs)
{
	HRESULT		hResult = NO_ERROR;
	uint32		searchPosition = 0;
	BOOL		bFound = FALSE;

	if (addrSubunit == NULL || 
		numSrcPlugs == NULL || 
		numDstPlugs == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	while (searchPosition < ARRAYSIZE(targetSubunitPlugs))
	{
		if (avcSubunitEqual(addrSubunit, &targetSubunitPlugs[searchPosition].addrSubunit))
		{
			*numSrcPlugs = targetSubunitPlugs[searchPosition].numSrcPlugs;
			*numDstPlugs = targetSubunitPlugs[searchPosition].numDstPlugs;
			bFound = TRUE;
			break;
		}
		searchPosition++;
	}

	if (bFound == FALSE)
	{
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

/***************************************************************************
	Target connection maintenance
	Implement these functions to track target connections
****************************************************************************/

HRESULT targetChoosePlug(AVC_CONNECTION *connection, BOOL bSource)
{
	HRESULT				hResult = NO_ERROR;
	AVC_ADDRESS_PLUG	*plug = NULL;
	uint8				numSrcPlugs = 0;
	uint8				numDstPlugs = 0;
	uint8				numPlugs = 0;
	uint8				plugID = 0;
	uint8				plugIdx = 0;
	uint32				index = 0;
	BOOL				bFound = FALSE;

	if (connection == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (bSource)
	{
		plug = &connection->plugs.src;
	}
	else
	{
		plug = &connection->plugs.dst;
	}

	targetGetSubunitPlugs(&plug->addrSubunit, &numSrcPlugs, &numDstPlugs);
	numPlugs = (uint8) ((bSource) ? numSrcPlugs : numDstPlugs);
	plugID = plug->plugID;

	// Pick the first available plug in the unit/subunit specified as the source
	for (plugIdx = 0; plugIdx < numPlugs; plugIdx++)
	{
		plug->plugID = plugIdx;

		for (index = 0; index < FLEXI_CONNECTION_ITEMS; index++)
		{
			bFound = avcSubunitConnectionPlugsEqual(&connection->plugs, &targetFlexiConnections[index].connection.plugs);
#if 1 //LM???	
			if (bFound) bFound = (targetFlexiConnections[index].bActive == FALSE);
#endif
			if (bFound) break;
		}
		if (bFound) break;
	}

#if 1 //LM???	
	if (bFound == FALSE)
	{
		for (plugIdx = 0; plugIdx < numPlugs; plugIdx++)
		{
			plug->plugID = plugIdx;

			for (index = 0; index < FLEXI_CONNECTION_ITEMS; index++)
			{
				bFound = avcSubunitConnectionPlugsEqual(&connection->plugs, &targetFlexiConnections[index].connection.plugs);
				if (bFound) bFound = (targetFlexiConnections[index].connection.bPerm == FALSE);
				if (bFound) bFound = (targetFlexiConnections[index].connection.bLock == FALSE);
				if (bFound) break;
			}
			if (bFound) break;
		}
	}
#endif
	
	if (bFound == FALSE)
	{
		plug->plugID = plugID; // set plugID back to "any available plug"
		hResult = E_FAIL;
	}

#if 0
	if (avcSubunitIsUnit(&plug->addrSubunit))
	{	
		// Unit plugs
		if (plugID == AVC_PLUG_ID_ANY_AVAILABLE_ISOCH)			// Any available Isoch I/O Plug
		{
		}
		else if (plugID == AVC_PLUG_ID_ANY_AVAILABLE_ASYNC)		// Any available Async I/O Plug
		{
		}
		else if (plugID == AVC_PLUG_ID_ANY_AVAILABLE_EXTERN)	// Any available External I/O Plug
		{
		}
	}
	else
	{
		// Subunit plugs
		if (plugID == AVC_PLUG_ID_ANY_AVAILABLE_SRC_DST)		// Any available Src/Dst I/O Plug
		{
		}
	}
#endif

	return hResult;
}

HRESULT targetConnectPlugs(AVC_CONNECTION *connection) //LM??? needs attention
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bFound = FALSE;
	uint32		index = 0;

	// This is typically called from CONNECT and SELECT INPUT to actually make the target connection between subunit plugs

	for (index = 0; index < FLEXI_CONNECTION_ITEMS; index++)
	{
		bFound = avcSubunitConnectionPlugsEqual(&connection->plugs, &targetFlexiConnections[index].connection.plugs);
		if (bFound) break;
	}
	
	if (bFound == FALSE)
	{	// Bail if we didnt find the connection
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	//LM??? overlaid connections ?

	if ((targetFlexiConnections[index].bActive) && 
		(targetFlexiConnections[index].connection.bLock))
	{
		// can't connect a connection if it's already connected and locked
		hResult = E_FAIL;
		// sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	targetFlexiConnections[index].bActive = TRUE;
	targetFlexiConnections[index].connection.bLock = connection->bLock;

	//LM??? update someone/something?
/*
	if (avcSubunitEqualToTypeID(&FlexiConnections[index].src.addrSubunit, AVC_SU_FIRST_AUDIO))
	{
//		drdSetOutputSelect(FlexiConnections[index].device, FlexiConnections[index].deviceNum);
	}

	if (avcSubunitEqualToTypeID(&FlexiConnections[index].dst.addrSubunit, AVC_SU_FIRST_AUDIO))
	{
//		drdSetOutputSelect(FlexiConnections[index].device, FlexiConnections[index].deviceNum);
	}
*/

	return hResult;
}

HRESULT targetDisconnectPlugs(AVC_CONNECTION *connection) //LM??? needs attention
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bFound = FALSE;
	uint32		index = 0;

	// This is typically called from DISCONNECT to disconnect the target connection between subunit plugs

	for (index = 0; index < FLEXI_CONNECTION_ITEMS; index++)
	{
		bFound = avcSubunitConnectionPlugsEqual(&connection->plugs, &targetFlexiConnections[index].connection.plugs);
		if (bFound) break;
	}
	
	if (bFound == FALSE)
	{	// Bail if we didnt find the connection
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	//LM??? overlaid connections ?

	if ((targetFlexiConnections[index].bActive == FALSE))
	{
		// can't disconnect a connection if it's already disconnected
		hResult = E_FAIL;
		// sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	targetFlexiConnections[index].bActive = FALSE;
	targetFlexiConnections[index].connection.bLock = FALSE;

	//LM??? update someone/something?

	return hResult;
}

HRESULT targetFindPlug(AVC_CONNECTION *connection)
{
	HRESULT				hResult = NO_ERROR;
	BOOL				bFound = FALSE;
	uint32				index = 0;
	
	// Given half a plug connection, find the missing half
	// The missing half plugs.src.plugID must be AVC_PLUG_ID_INVALID (0xFE)

	if (connection == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avcSubunitPlugIsInvalidPlug(&connection->plugs.src))		// missing dst plug
	{
		for (index = 0; index < FLEXI_CONNECTION_ITEMS; index++)
		{
			bFound = avcSubunitPlugsEqual(&connection->plugs.dst, &targetFlexiConnections[index].connection.plugs.dst);
			if (bFound) bFound = targetFlexiConnections[index].bActive;
			if (bFound) break;
		}
	}
	else														// missing src plug
	{
		for (index = 0; index < FLEXI_CONNECTION_ITEMS; index++)
		{
			bFound = avcSubunitPlugsEqual(&connection->plugs.src, &targetFlexiConnections[index].connection.plugs.src);
			if (bFound) bFound = targetFlexiConnections[index].bActive;
			if (bFound) break;
		}
	}
	
	if (bFound == FALSE)
	{	// did not find the connection
		hResult = E_FAIL;
		// sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	avcSubunitCopyConnection(connection, &targetFlexiConnections[index].connection);

	return hResult;
}

uint32 targetGetNumConnections(void)
{
	uint32	numConnections = 0;
	uint32	i = 0;

	numConnections += FIXED_CONNECTION_ITEMS;

	for (i = 0; i < FLEXI_CONNECTION_ITEMS; i++)
	{
		numConnections += (targetFlexiConnections[i].bActive) ? 1 : 0;
	}

	return numConnections;
}

HRESULT targetGetConnections(uint8 *numConnections, uint8 maxConnections, AVC_CONNECTION *connectionArray)
{
	HRESULT				hResult = NO_ERROR;
	AVC_CONNECTION*		pConn = NULL;
	uint32				i;

	if (numConnections == NULL ||
		connectionArray == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*numConnections = (uint8) targetGetNumConnections();

	if (*numConnections > maxConnections)
	{
		*numConnections = 0;
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pConn = connectionArray;

#if 0
	for (i = 0; i < FIXED_CONNECTION_ITEMS; i++)
	{
		avcSubunitCopyConnection(pConn++, targetFixedConnections[i]);
	}
#endif

	for (i = 0; i < FLEXI_CONNECTION_ITEMS; i++)
	{
		if (targetFlexiConnections[i].bActive)
		{
			avcSubunitCopyConnection(pConn++, &targetFlexiConnections[i].connection);
		}
	}

	return hResult;
}

HRESULT targetPlugExists(AVC_CONNECTION *connection, BOOL source)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bFound = FALSE;
	uint8		numSrcPlugs = 0;
	uint8		numDstPlugs = 0;

	// Simply check that a plug exists in a subunit.
	// Uses "source" to determine whether to check the source or the destination part of the connection.

	if (source)
	{
		targetGetSubunitPlugs(&connection->plugs.src.addrSubunit, &numSrcPlugs, &numDstPlugs);
		if (connection->plugs.src.plugID < numSrcPlugs) bFound = TRUE;
	}
	else
	{
		targetGetSubunitPlugs(&connection->plugs.dst.addrSubunit, &numSrcPlugs, &numDstPlugs);
		if (connection->plugs.dst.plugID < numDstPlugs) bFound = TRUE;
	}

	if (bFound == FALSE)
	{
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

#if 1 //LM??? testing
HRESULT targetGetSubunitAddress(AVC_ADDRESS_SUBUNIT *subunit)
{
	HRESULT		hResult = NO_ERROR;

	subunit->subunit_type = AVC_SU_TYPE_AUDIO;
	subunit->subunit_ID = 0;

	return hResult;
}
#endif //LM???

HRESULT targetGetUnitInfoAddress(AVC_ADDRESS_SUBUNIT *subunit)
{
	HRESULT		hResult = NO_ERROR;

#if 1 //LM???
	subunit->subunit_type = AVC_SU_TYPE_AUDIO;
	subunit->subunit_ID = AVC_SU_ID_IGNORE;
#endif

	return hResult;
}

HRESULT targetGetUnitInfoCompanyID(uint32 *company_ID)
{
	HRESULT		hResult = NO_ERROR;

	*company_ID = THIS_VENDOR_ID;

	return hResult;
}

HRESULT targetGetSubunitInfoEntry(uint32 index, AVC_ADDRESS_SUBUNIT *subunit)
{
	HRESULT		hResult = NO_ERROR;

	if (index >= ARRAYSIZE(targetSubunitInfo)) return E_BAD_INPUT_PARAMETERS;

	avcSubunitCopySubunit(subunit, &targetSubunitInfo[index]);

	return hResult;
}

HRESULT	targetGetSubunitVersionInfo(AVC_ADDRESS_SUBUNIT *subunit, BYTE *version_information, BYTE *su_dependent_field)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(subunit);
	// su_dependent_field: AVC_VERSION_SU_DEP_FIELD_SIZE bytes

#if 1 //LM??? testing
	if (*version_information == 0xFF)
	{
		// get latest version to which the unit or subunit complies
		*version_information = 0x02;
	}
	else
	{
		// get the support level of the specified version
		uint32 i;

		if (*version_information == 0x02)
		{
			for (i = 0; i < AVC_VERSION_SU_DEP_FIELD_SIZE; i++)
			{
				su_dependent_field[i] = (BYTE) i;
			}
		}
		else
		{
			hResult = E_NOT_IMPLEMENTED;
		}
	}
#else
	UNUSED_ARG(version_information);
	UNUSED_ARG(su_dependent_field);

	hResult = E_NOT_IMPLEMENTED;
#endif

	return hResult;
}
#endif // _AVC

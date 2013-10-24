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

 DESCRIPTION:

	Implementation of avc unit commands
	AV/C Digital Interface Command Set (Ver. 4.1) section 11 and 12

	Implementation of a selection of these commands (eventually)

//LM??? list not accurate/updated

		Unit general:
			UNIT INFO
			SUBUNIT INFO
			

		Unit features
			DIAGNOSTICS
			DEVICE PRESET
			POWER
			RESERVE

		Connection management:
			CHANNEL USAGE
			CONNECT
			CONNECT AV
			CONNECTIONS
			DIGITAL INPUT
			DIGITAL OUTPUT
			DISCONNECT
			DISCONNECT AV
			INPUT PLUG SIGNAL FORMAT
			OUTPUT PLUG SIGNAL FORMAT
			PLUG INFO

		Descriptors
			OPEN DESCRIPTOR
			READ DESCRIPTOR
			WRITE DESCRIPTOR
			SEARCH DESCRIPTOR
			OBJECT NUMBER SELECT

		Vendor dependent
			VENDOR DEPENDENT

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifndef _AVCUNIT_H
#define _AVCUNIT_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"

HRESULT		avcUnitCallback(AVC_HEADER *pHeader, PB *packetBlock);

HRESULT		avcUnitInitialize(void);
HRESULT		avcUnitResumeTasks(void);									// Resume tasks for this module
HRESULT		avcUnitSuspendTasks(void);									// Suspend tasks for this module
HRESULT		avcUnitInitializeTasks(void);								// Create tasks for this module
HRESULT		avcUnitInitialize(void);									// Initialization of this module

#endif //_AVCUNIT_H


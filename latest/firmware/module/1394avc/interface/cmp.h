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

/** @addtogroup CMP

	This module implements the connection management procedures defined in
	IEC 61883-1.

	This module can be used from the command prompt by the @ref cli_cmp commands.

	The following registers are implemented:
	- iPCR
	- oPCR
	- iMPR
	- oMPR

	This module instantiates these registers and respond to lock transactions
	at the register addresses.

	@{
*/



#ifndef _CMP_H
#define _CMP_H

#include "1394.h"
#include "packetBlock.h"
#include "TCTypes.h"
#include "lal.h"
#include "cmpDefs.h"
#include "cmpUtils.h"
#include "cmpP2P.h"
#include "cmpBC.h"

HRESULT	cmpGenerateQuadletResponse(PB *packetBlock, uint32 data, PB_PACKETTYPE packetType);

//! Callback for lock transactions addressed to the PCR registers (offsets F000 0900 through F000 09FC)
HRESULT	cmpCB(PB *packetBlock);



void		cmpHandlerThread(void *dummy);
HRESULT		cmpCreateSemaphores(void);

/** @defgroup CMPInit CMP Initialization
	Initialization of the CMP Module
	@{
*/

//! Resumse tasks for this module
HRESULT		cmpResumeTasks(void);
//! Suspends tasks for this module
HRESULT		cmpSuspendTasks(void);
//! Late initialization of this module (create tasks)
HRESULT		cmpInitializeTasks(void);
//! Early initialization of this module
HRESULT		cmpInitialize(void);
//! @}

/** @} */
#endif //_CMP_H


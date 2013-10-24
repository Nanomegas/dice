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
	Purpose:	Implementing Tasking Platform Generic layer
				for the windows platform

				Platform Dependend Defines
	
	Revisions:
		created 11/12/2002 lm

****************************************************************************/

#ifndef _TC_TASKINGDEFS_H_
#define _TC_TASKINGDEFS_H_

#define	TASK_PRIORITY_LOWEST			31
#define	TASK_PRIORITY_DEFAULT			16
#define TASK_PRIORITY_BELOW_NORMAL		20
										
// Priority of 1394 Core tasks			
#define TASK_PRIORITY_1394CORE_HI		7						// lhlBri, lhlATx, lhlARx (higher than normal tasks)
#define TASK_PRIORITY_1394CORE_DEFAULT	TASK_PRIORITY_DEFAULT	// lhlService (same as normal tasks)
#define TASK_PRIORITY_1394CORE_LOWEST	TASK_PRIORITY_LOWEST	// lhlWatch (lowest task priority)
										
#define	TASK_PRIORITY_BRI				TASK_PRIORITY_1394CORE_DEFAULT	//LM??? TASK_PRIORITY_1394CORE_HI
#define	TASK_PRIORITY_SERVICE			TASK_PRIORITY_1394CORE_DEFAULT
#define	TASK_PRIORITY_RX				TASK_PRIORITY_1394CORE_DEFAULT	//LM??? TASK_PRIORITY_1394CORE_HI
#define	TASK_PRIORITY_TX				TASK_PRIORITY_1394CORE_DEFAULT	//LM??? TASK_PRIORITY_1394CORE_HI
#define	TASK_PRIORITY_WATCH				TASK_PRIORITY_1394CORE_LOWEST
										
#define	TASK_STACKSIZE_DEFAULT			8192
										
#define	TC_NO_WAIT						0x00000000
#define	TC_WAIT_FOREVER					0xFFFFFFFF
#define	TC_TIMEOUT_VALUE				//0x00000001-0xFFFFFFFE msecs

#endif


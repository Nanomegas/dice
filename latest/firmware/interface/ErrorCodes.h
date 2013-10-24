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

#ifndef _ERROR_CODES_H_
#define _ERROR_CODES_H_

/*********************** error bases ***********************************/
#ifndef NO_ERROR
#define NO_ERROR	0x0
#endif

#define E_TASK			0x80000000

#define E_PCI			0x84000000
#define E_GEN			0xFF000000

#include "1394ErrorCodes.h" // [0x90000000;0xB00000000] reserved
#include "Dice2ErrorCodes.h" // [0xC0000000;0xD00000000] reserved

/************************ error codes **********************************/

/*************** Kernel user error codes **********************************/
#define E_TASK_WRONG_TASKID         (E_TASK + 1)
#define E_TASK_ALREADY_RESUMED      (E_TASK + 2)
#define E_TASL_ALREADY_SUSPENDED    (E_TASK + 3)
#define E_SEMA_ID_NOT_VALID         (E_TASK + 4)
#define E_PORT_ID_NOT_VALID			(E_TASK + 5)
#define E_TASK_OUT_OF_MESSAGES      (E_TASK + 6)

/************************* PCI ********************************************/
#define E_PCI_INIT				(E_PCI+0)	/*  InitDMA fails, because EUMBBAR register is not set */
#define E_PCI_ILLEGAL_PARAM 	(E_PCI+1)	/*  A parameter passed to for example the address translation unit is not valid */
#define E_PCI_INVALID_DEVICEID	(E_PCI+2)	/*  Invalid OHCI device */
#define E_PCI_INVALID_VENDORID	(E_PCI+3)	/*  Invalid OHCI vendor */

/************************* GENERIC ********************************************/
#define E_GEN_NOMATCH           (E_GEN+0)
#define E_GEN_ILLEGAL_PARAM     (E_GEN+1)
#define E_GEN_WRONG_CONTEXT		(E_GEN+2) //General error when a function is called from wrong context
#endif

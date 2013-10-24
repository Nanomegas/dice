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

	Implementation of avc panel subunit commands - Defines

	AV/C Panel Subunit Specification 1.21
	TA Document 2002009
	
	Panel Subunit Commands	(Direct Mode)

										ctype			target
										C,S,N			U,S
		GUI UPDATE						M,M,-			-,+
		PUSH GUI DATA					M,M,-			-,+
		USER ACTION						O,-,-			-,+
		PASS THROUGH					O,-,-			-,+


  Panel Subunit Commands	(Indirect Mode)

										ctype			target
										C,S,N			U,S
		PASS THROUGH					M,-,-			-,+

			ctype:
			C:CONTROL, S:STATUS, N:NOTIFY
			M:Mandatory, R:Recommended, O:Optional, -:Not Defined, V:Vendor dependent

			target
			U:UNIT, S:SUBUNIT

  Revisions:
		created 17/09/2003 LM

****************************************************************************/

#ifndef _AVCPANELDEFS_H
#define _AVCPANELDEFS_H

#include "TCTypes.h"

// Panel subfunctions							
												
// AVC PANEL GUI UPDATE							
#define PANEL_GUI_OPEN							0x00
#define PANEL_GUI_CLOSE							0x01
#define PANEL_GUI_RESTORE						0x02
#define PANEL_GUI_START							0x03
#define PANEL_GUI_STOP							0x04
#define PANEL_GUI_CHANGE						0x05
												
// AVC PANEL PUSH GUI DATA						
#define PANEL_PUSH_GUI_NEW						0x00
#define PANEL_PUSH_GUI_CLEAR					0x01
												
#define PANEL_PUSH_STATUS_NO_ERROR				0x00
#define PANEL_PUSH_STATUS_READY					0x01
#define PANEL_PUSH_STATUS_PREPARATION			0x81
#define PANEL_PUSH_STATUS_SOURCE_PLUG_BUSY		0x82
#define PANEL_PUSH_STATUS_NO_ELEMENT			0x83
#define PANEL_PUSH_STATUS_NOT_CONNECTED			0x84
#define PANEL_PUSH_STATUS_NOT_OWNER				0x85
#define PANEL_PUSH_STATUS_CANCELLED				0x86
#define PANEL_PUSH_STATUS_NOT_TRANSMITTING		0x87
#define PANEL_PUSH_STATUS_OTHER_ERROR			0xFE
												
#define PANEL_SLIDER_TYPE						0x14
#define PANEL_SCROLLER_TYPE						0x16
#define PANEL_LABEL_TYPE						0x17
#define PANEL_LINK_TYPE 						0x10
#define PANEL_PANEL_TYPE						0x00
												
#define PANEL_WITHOUT_DATA						0x00
#define PANEL_WITH_DATA							0x01
												
#define PANEL_LEVEL_ITSELF						0x00
#define PANEL_LEVEL_NEXT						0x01
#define PANEL_LEVEL_ALL							0x02
												
// AVC PANEL USER ACTION						
#define PANEL_USER_ACTION_SELECT				0x00
#define PANEL_USER_ACTION_SET_VALUE				0x03
#define PANEL_USER_ACTION_CHOOSE_LIST			0x05

typedef enum
{
	PANEL_OPENED,
	PANEL_STARTED,
	PANEL_STOPPED,
	PANEL_CLOSED
} AVC_PANEL_STATUS;

typedef enum
{
	PANEL_DIRECT_MODE,
	PANEL_INDIRECT_MODE
} AVC_PANEL_MODE;

typedef struct 
{
	uint16				type;
	uint16				id;
} AVC_PANEL_OWNER;

typedef struct {
	AVC_PANEL_OWNER		owner;
	AVC_PANEL_STATUS	status;
	AVC_PANEL_MODE		mode;
//LM???	AVC_PANEL_STRUCT	*ps;  // from qstructs
} AVC_PANEL_SUBUNIT;

typedef struct
{
	uint16				type;
	uint16				id;
} AVC_PANEL_ELEMENT_ID;

#endif //_AVCPANELDEFS_H


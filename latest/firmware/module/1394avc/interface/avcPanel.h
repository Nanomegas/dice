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

	Implementation of avc panel subunit commands

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

#ifndef _AVCPANEL_H
#define _AVCPANEL_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"
#include "avcPanelDefs.h"

AVC_PANEL_MODE	avcPanelGetSubunitMode(void);

HRESULT			avcPanelCallback(AVC_HEADER *pHeader, PB *packetBlock);

HRESULT			avcPanelInitialize(void);

#endif //_AVCPANEL_H


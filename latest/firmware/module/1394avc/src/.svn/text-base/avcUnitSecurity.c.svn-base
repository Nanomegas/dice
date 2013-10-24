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

	Implementation of avc security commands

	Implementation of the avc security commands
	AV/C Digital Interface Command Set Extensions (chapter 8)
	Digital Transmission Content Protection Specification Volume 1 Revision 1.2a
	
										ctype			target
										C,S,N			U,S
		SECURITY						?,?,?			?,?

			ctype:
			C:CONTROL, S:STATUS, N:NOTIFY
			M:Mandatory, R:Recommended, O:Optional, -:Not Defined, V:Vendor dependent

			target
			U:UNIT, S:SUBUNIT

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcUnitSecurity.h"
#include "avcDefs.h"
#include "avcUnitCli.h"
#include "cr.h"
#include "systemCallbacks.h"

#ifdef _AVC_SECURITY
HRESULT avcSecurity(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	CALLBACK_DESCRIPTOR*	cb = NULL;
	AVC_SECURITY_CALLBACK	callback = NULL;
	uint32					index = 0;
	uint32					category = 0;
	
	hResult = dsReadBits(pStream, 4, &category);
	if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
	{
		avcUnitCliPrintSecurity(category);
	}
#endif //_SYSDEBUG

	hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
	if (hResult != NO_ERROR) return hResultError;
	
	DO_FOREVER
	{
		hResult = crGetCallback(AVC_SECURITY_TYPE, index++, &cb);
		if (hResult != NO_ERROR) return hResultError;

		hResult = kvIsValue(cb, AVC_SECURITY_CATEGORY_KEY, category);
		if (hResult != NO_ERROR) continue;

		hResult = kvGetValue(cb, AVC_SECURITY_CALLBACK_KEY, (uint32 *) &callback);
		if (hResult != NO_ERROR) continue;
	
		hResult = (*callback) (pHeader, packetBlock, pStream);
		break;
	}
		
	return hResult;
}
#endif //_AVC_SECURITY


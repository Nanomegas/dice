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
 * DESCRIPTION: Calculates the 16 bit CRC check on a buffer per the 
				CSR Specification (ANSI/IEEE Std 1212)
	Purpose:	implement crc-16 calculations
	
	Revisions:
		created 04/23/2002 lm

****************************************************************************/

#ifndef _CRC16_H
#define _CRC16_H

#include "TCTypes.h"

/*	generateCrc16()	
    generateCrc16_1991()

	Description:
		generateCrc16 -	Compute CRC per the 1994 specification
		generateCrc16_1991 - Compute CRC per the 1991 specification
	Parameters:
		qbuf - array of quadlets over which to compute the crc
		numQuads - number of quadlets in qbuf
	Return:
		computed crc
*/

// use for all local node config rom calculations
// use to validate remote node config rom entries per 1994 spec

uint16 generateCrc16(QUADLET * qbuf, int32 numQuads);

// use to validate remote node config rom entries per 1991 spec
// in case 1994 validation fails
uint16 generateCrc16_1991(QUADLET * qbuf, int32 numQuads);

#endif

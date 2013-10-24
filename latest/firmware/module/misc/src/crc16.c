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
		created 04/03/2002 lm

****************************************************************************/

#include "TCTypes.h"
#include "crc16.h"

// use for all local node config rom calculations
// use to validate remote node config rom entries per 1994 spec

uint16 generateCrc16(QUADLET * qbuf, int32 numQuads)
{
	uint32	crc = 0;
	uint32	data = 0;
	uint32	sum = 0;
	int32	num;
	int32	shift;
		
	for(num = 0; num < numQuads; num++)
	{
		data = *(qbuf+num);

		for ( shift = 28; shift >= 0; shift -= 4)	// per 1994 spec
		{
			sum = ((crc >> 12) ^ (data >> shift)) & 0xf;
			crc = (crc << 4) ^ (sum << 12) ^ (sum << 5) ^ (sum);
		}
	}
	
	return ((uint16)(crc & 0xffff));
}

// use to validate remote node config rom entries per 1991 spec
// in case 1994 validation fails
uint16 generateCrc16_1991(QUADLET * qbuf, int32 numQuads)
{
	uint32	crc = 0;
	uint32	data = 0;
	uint32	sum = 0;
	int32	num;
	int32	shift;
		
	for(num = 0; num < numQuads; num++)
	{
		data = *(qbuf+num);
		
		for ( shift = 28; shift >  0; shift -= 4)	// per 1991 spec
		{
			sum = ((crc >> 12) ^ ((data >> shift) & 0xffff)) & 0xf;
			crc = (crc << 4) ^ (sum << 12) ^ (sum << 5) ^ (sum);
		}
	}
	
	return ((uint16)(crc & 0xffff));
}


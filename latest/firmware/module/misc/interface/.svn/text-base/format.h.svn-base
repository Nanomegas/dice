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
* DESCRIPTION: This is a set of functions to format strings
	
	primarily used to format numerical data within strings

	Revisions:
		created 21/10/2002 lm
****************************************************************************/

#ifndef _FORMAT_H
#define _FORMAT_H

#include "TCTypes.h"

#define FORMAT_INVALID			0
#define FORMAT_DECIMAL			1
#define FORMAT_HEXADECIMAL		2

uint32	hex2int (char * in_str, uint32 num_char);
int32	dec2int (char * in_str, uint32 num_char);
void	int2hex (uint32 in_int, char * out_str, uint32 num_char);
void	int2dec (uint32 in_int, char * out_str, uint32 num_char);

uint8	is_number (char * in_str, uint32 length);
BOOL	dec2Uint(char *pChar, uint32 *result);

void	formatRemoveAllInstancesOf(char * str1, char remove_me, uint32 *string_length);
void	formatCondenseWhitespace(char* cl);
void	formatInsertWhitespace(char* cl);

#ifdef _LITTLE_ENDIAN
void	formatSwapStrBytes(char *str, uint32 len);
#else //_LITTLE_ENDIAN
#define formatSwapStrBytes(str, len)		// empty macro
#endif //_LITTLE_ENDIAN

#endif //_FORMAT_H

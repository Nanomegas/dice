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
*****************************************************************************/

#include "TCTasking.h"
#include "ctype.h"
#include "stdlib.h"
#include "format.h"

#if 0 //LM???
#define NUM_ARG_BUF  15

/*
	VA_ARG
	This macro replaces the common VA_ARG, to get rid of the warnings.
	It is the same as the original with the exception that is does not have any always true/false statements.
*/

#define VA_ARG(args,type) \
( ((args)[0].va_darg += 0), ((args)[0].va_pos += (((args)[0].va_darg <= 4)) ? 0 : ((sizeof(type)+3)&~3)), ((type *)((args)[0].va_addr + ((((args)[0].va_darg <= 4)) ? (8 * ((args)[0].va_darg - 5)) : ((args)[0].va_pos - ((sizeof(type)+3)&~3)) )) )[0] )

HRESULT TC_sprintf (char * buf, const char * format, ...)
{
	HRESULT		hResult = NO_ERROR;
	char		NumericalArg[NUM_ARG_BUF];
	uint16		InBufPtr = 0;
	uint16		OutBufPtr = 0;
	uint16		x = 0;
	BOOL		bZeroPadded = FALSE;
	int32		NumDigits = 0;
	int8		ArgLength = 0;
	BOOL		bIsNegative = FALSE;
	int32		int32Arg = 0;
	uint32		uint32Arg = 0;
	char		charArg = 0;
	char		*stringArg = NULL;
	va_list		args;

	va_start(args, format);

	InBufPtr = 0;  OutBufPtr = 0;  // initialize pointers		

	do
	{
		// initialize format specifier vars each pass
		bZeroPadded = FALSE; 
		NumDigits = 0;
		// check each character in the unformatted string for special chars
		switch (format[InBufPtr])  
		{
			case '%':
				// look at the next character for more info on the nature
				// of the format specifier
				InBufPtr++;

				// first see if there's a leading 0			
				if (format[InBufPtr] == '0') 
					{
						InBufPtr++;
						bZeroPadded = TRUE;
					}

				// see if there's a numerical argument
				if ((format[InBufPtr] >= '1') && (format[InBufPtr] <= '9'))
				{
					InBufPtr++;
					// now see if there's a second digit
					if ((format[InBufPtr] >= '0') && (format[InBufPtr] <= '9'))
					{
						// convert a 2-digit numerical string to an integer
						NumDigits = (format[InBufPtr] - '0') * 10;
						NumDigits += format[InBufPtr - 1] - '0';
						InBufPtr++;
					}
					else
					{
						// convert a 1-digit string to an integer
						NumDigits = format[InBufPtr - 1] - '0';
					}
				}

				// see if we have a special hex code symbol to deal with
				if (format[InBufPtr] == '#')
				{
					InBufPtr++;
					switch (format[InBufPtr])
					{
						case 'x':
						case 'X':
							buf[OutBufPtr++] = '0';
							buf[OutBufPtr++] = 'x';
					}

				}

				// now it's time to get to the meat of the issue
				// let's handle the format specifier type
				switch (format[InBufPtr])
				{
					case 'd':
					case 'i':
						// there should be an argument in the list for us
						// read it in
						int32Arg = VA_ARG (args, long);

						// variable to set the string size for the number
						ArgLength = 0;

						// set the IsNegative flag now; we're going to stomp
						// on our original parameter passed in
						if (int32Arg < 0)
						{
							bIsNegative = TRUE;
							int32Arg *= -1;
						} 
						else
						{
							bIsNegative = FALSE;
						} 


						// create a string from the digits (LSD first)
						do
						{
							NumericalArg[ArgLength++] = '0' + (int32Arg % 10);
							int32Arg /= 10;
						} while (int32Arg > 0);

						// is a minus sign necessary?
						if (bIsNegative)
						{
							// leave room for it if this is 0-padded
							// add it to the string if not
							if (bZeroPadded) NumDigits--;  
							else NumericalArg[ArgLength++] = '-';
						}

						// pad the beginning with 0's or spaces, if specified
						// hopefully this doesn't get executed at all if 
						// ArgLength is already greater than NumDigits
						for (x = ArgLength;x < NumDigits; x++)
						{
							if (bZeroPadded) NumericalArg[ArgLength++] = '0';  
							else NumericalArg[ArgLength++] = ' ';
						}
						
						// add the minus sign at the far end of the 0-padded
						// argument
						if ((bIsNegative) && (bZeroPadded)) 
						{
							NumericalArg[ArgLength++] = '-';
						}
						
						// terminate the string for safety sake
						NumericalArg[ArgLength] = '\0';

						// now copy it to the output string (reversing the
						// order of the digits
						x = ArgLength;
						do
						{
							// pre-decrement x so we don't copy the
							// terminating character and so we do copy
							// the 0th element
							buf[OutBufPtr++] = NumericalArg[--x];
						} while (x != 0);
						break;

					case 'x':
					case 'X':
						uint32Arg = VA_ARG (args, uint32);
						// call a routine in format.c to do the conversion

						int2hex (uint32Arg, NumericalArg, NumDigits);
						// convert the digits to lower case, if necessary
						if (format[InBufPtr] == 'x')
						{
							for (x = 0; x < NumDigits;x++)
							{
								if ((NumericalArg[x] >= 'A') \
								 && (NumericalArg[x] <= 'F'))
								{
									NumericalArg[x] += ('a' - 'A');
								}					
							}
						}
						
						// terminate the string for safety sake
						NumericalArg[NumDigits] = '\0';

						// now copy the numerical argument to the output buffer
						x = 0;
						do
						{
							buf[OutBufPtr++] = NumericalArg[x++];
						} while (x < NumDigits);

						break;

					case 'u':
						uint32Arg = VA_ARG (args, uint32);

						// variable to set the string size for the number
						ArgLength = 0;

						// create a string from the digits (LSD first)
						do
						{
							NumericalArg[ArgLength++] = '0' + (uint32Arg % 10);
							uint32Arg /= 10;
						} while (uint32Arg > 0);

						// pad the beginning with 0's or spaces, if specified
						// hopefully this doesn't get executed at all if 
						// ArgLength is already greater than NumDigits
						for (x = ArgLength;x < NumDigits; x++)
						{
							if (bZeroPadded) NumericalArg[ArgLength++] = '0';  
							else NumericalArg[ArgLength++] = ' ';
						}
						
						// terminate the string for safety sake
						NumericalArg[ArgLength] = '\0';

						// now copy it to the output string (reversing the
						// order of the digits
						x = ArgLength;
						do
						{
							// pre-decrement x so we don't copy the
							// terminating character and so we do copy
							// the 0th element
							buf[OutBufPtr++] = NumericalArg[--x];
						} while (x != 0);
						break;

					case 'c':
						charArg = VA_ARG (args, char);
						buf[OutBufPtr++] = charArg;
						break;

					case 's':
						stringArg = VA_ARG (args, char *);
						while (0 != *stringArg)
						{
							buf[OutBufPtr] = *stringArg;
							++stringArg;
							++OutBufPtr;
						}
						break;

					case '%':
						buf[OutBufPtr++] = format[InBufPtr];
						break;

					default:
						buf[OutBufPtr++] = format[InBufPtr];
						break;

				}
				break;

			default:
				buf[OutBufPtr++] = format[InBufPtr];
				break;
		}

	} while (format[++InBufPtr] != '\0');	
	
	buf[OutBufPtr] = '\0';
	va_end (args);
	return NO_ERROR;

}
#endif

/* *************************************************************
	hex2int

	Arguments: A string of valid hex characters 
	(upper or lower case) of the length specified in num_char.
	return	0 if there are any invalid characters in the string.
************************************************************* */
uint32 hex2int (char * in_str, uint32 num_char)
{
	int32		x = 0;
	int32		digit_val = 0;
	uint32		output = 0;
	int32		shift = 0;

	if (num_char == 0) return 0;

	for (x = (int32) (num_char - 1);x >= 0;x--)
	{
		digit_val = -1;

		if ((in_str[x] >= '0') && (in_str[x] <= '9'))
			digit_val = (int16)in_str[x] - (int16)'0';

		if ((in_str[x] >= 'A') && (in_str[x] <= 'F'))
			digit_val = (int16)in_str[x] - (int16)'A' + 10;

		if ((in_str[x] >= 'a') && (in_str[x] <= 'f')) 
			digit_val = (int16)in_str[x] - (int16)'a' + 10;

		if (in_str[x] == 'x') return (output);
					
		if (digit_val < 0) return 0;
		output += digit_val << shift;
		shift += 4;
		if (num_char==0)
		{
			break;
		}
	}
	return output;
}

/* *************************************************************
	dec2int

	Arguments: A string of valid decimal characters
	(upper or lower case) of the length specified in num_char.

	Return value: 32-bit signed int.

	Notes: This will return 0 if there are any invalid characters in the string.
************************************************************* */
int32 dec2int (char *in_str, uint32 num_char)
{
	int32	x = 0;
	int32	digit_val = 0;
	int32	output = 0;
	uint32	multiplier = 1;

	if (num_char == 0) return 0;

	for (x = (int32) (num_char - 1);x >= 0;x--)
	{
		digit_val = -1;

		if ((in_str[x] >= '0') && (in_str[x] <= '9'))
			digit_val = (int16)in_str[x] - (int16)'0';

		if (in_str[x] == '-') return (output * -1);

		if (digit_val < 0) return 0;
		output += digit_val * multiplier;
		multiplier *= 10;
	}
	return output;
}

/* *************************************************************
	int2hex

	Arguments: an integer and the number of characters that
		should be returned in the string pointer
	
	Return value: characters in the string being pointed to.

	Notes: 	If num_chars is not large enough to hold the result,
			the result will be right-aligned.
			If num_chars is too large, zeros will be 
			the upper digits.
************************************************************* */
void int2hex (uint32 in_int, char *out_str, uint32 num_char)
{
	int32	x = 0;
	uint32	digit_val = 0;
	char	out_char = 0;
	uint32	shift = 0;

	// start at the end of the string and work my way backward
	// first terminate the string
	out_str[num_char] = '\0';

	// now process the digits
	for (x = (int32) (num_char - 1);x >= 0;x--)
	{
		digit_val = (in_int >> shift) & 0xF;
		if (digit_val >= 0xA) out_char = (char)((int16)'A' + digit_val - 0xA);
		if (digit_val <= 9) out_char = (char)((int16)'0' + digit_val);
		out_str[x] = out_char;
		shift += 4;
	}
}

/* *************************************************************
	int2dec

	Arguments: an integer and the number of characters that
		should be returned in the string pointer
	
	Return value: characters in the string being pointed to.

	Notes: 	If num_chars is not large enough to hold the result,
			the result will be right-aligned.
			If num_chars is too large, zeros will be 
			the upper digits.
************************************************************* */
void int2dec (uint32 in_int, char *out_str, uint32 num_char)
{
	int32	x = 0;
	uint32	digit_val = 0;
	char	out_char = 0;

	// start at the end of the string and work my way backward terminate the string
	out_str[num_char] = '\0';

	for (x = (int32) (num_char - 1);x >= 0;x--)
	{
		digit_val = in_int % 10;
		out_char = (char)((int16)'0' + digit_val);
		in_int /= 10;
		out_str[x] = out_char;
	}
}

/* *************************************************************
	is_number

	Arguments: 	a base string (null termination not necessary)
	Return value: 
					0 - not a number
			        1 - a valid decimal number
			        2 - a valid hex number
************************************************************* */
uint8 is_number (char * in_str, uint32 length)
{
	uint32	x = 0;
	uint8	str_type = FORMAT_INVALID;
	
	if ((in_str[0] == '0') && (in_str[1] == 'x'))
	{
		// evaluate as a hex string
		for (x = 2; x < length; x++)
		{
			// assume it's invalid first
			str_type = FORMAT_INVALID;
			// if it's a valid digit, set the str_type properly
			if ((in_str[x] >= '0') && (in_str[x] <= '9')) str_type = FORMAT_HEXADECIMAL;
			if ((in_str[x] >= 'a') && (in_str[x] <= 'f')) str_type = FORMAT_HEXADECIMAL;
			if ((in_str[x] >= 'A') && (in_str[x] <= 'F')) str_type = FORMAT_HEXADECIMAL;
			if (in_str[x] == '\0')
			{
				if (x > FORMAT_HEXADECIMAL) return FORMAT_HEXADECIMAL;
			    else return FORMAT_INVALID;
			}
			if (str_type == FORMAT_INVALID) return FORMAT_INVALID;
		}
		if (str_type == FORMAT_HEXADECIMAL) return FORMAT_HEXADECIMAL;
	}
	else
	{
		// evaluate as a decimal string
		str_type = FORMAT_DECIMAL;
		for (x = 0; x < length; x++)
		{
			// if it's not a valid digit, take action
			if ((in_str[x] == '-') && (x == 0))
			{
				; // minus valid if its the 1st character
			}
			else if ((in_str[x] < '0') || (in_str[x] > '9'))
			{
				if (in_str[x] == '\0')
			    {
			    	if (x > FORMAT_INVALID) return FORMAT_DECIMAL;
			        else return FORMAT_INVALID;
			    }
			    else return FORMAT_INVALID;
			}
		}
		return str_type;
	}

	// program error if we're here
	return FORMAT_INVALID;
}

/**************************************************************

	dec2Uint

	Similar to library function atoi, with verification

	Parameters:
		pChar - pointer to a null-terminated string
		pResult - pointer to a variable where the atoi result will be stored
				if the string is a valid UNSIGNED decimal string.
	Returns:
		TRUE if the string contains only the digits 0-9.  In this case,
			the binary value has been stored in caller's variable
		FALSE otherwise

**************************************************************/

BOOL dec2Uint (char *pChar, uint32 *pResult)
{
	uint32	i = 0;

	while (pChar[i] != 0)
	{
		if (!isdigit(pChar[i]))
		{
			return FALSE;
		}
		++i;
	}
	
	*pResult = (uint32)atoi(pChar);

	return TRUE;
}

/* *************************************************************
	formatRemoveAllInstancesOf

	Arguments: 	a base string (null termination not necessary)
				a character to remove from the string
				a pointer to the number of characters in the string
	
	Return value: the string with no 'remove_me' characters

	Notes:
************************************************************* */
void formatRemoveAllInstancesOf(char * str1, char remove_me, uint32 *string_length) 
{
	uint32	char_pos = 0;
	uint32	dest = 0;
	uint32	count = *string_length;

	//remove all unwanted characters
	while (count--)
	{
		if (str1[char_pos] == remove_me)
		{
			// in doing the copy, go one char beyond caller's string_length, as a convenience in case its an asciiz string
		   for (dest = char_pos; dest < (*string_length); ++dest)
		   {
				str1[dest] = str1[dest+1];
		   }	
		   (*string_length)--;
		}
		else
		{
			char_pos++;
		}
	}
}

/*
	formatCondenseWhitespace
	
	This function strips valid white space and turns it into single spaces. 
	If it finds a quoted string, it ignores the whitespace inside of the quoted
	string.  This makes parsing the string into tokens a far easier process.
	
*/
void formatCondenseWhitespace(char* cl)
{
	char*	p = cl;
	BOOL	openquote = FALSE;
	
	while (*p)
	{
		switch (*p)
		{
			case '\"':
				openquote = !openquote;
				break;	
			
			case ' ':
			case '\t':
				if (!openquote)
				{
					if (*(p+1))
					{
						switch (*(p+1))
						{
							case ' ':
							case '\t':
								memmove(p,(p+1),strlen((p+1))+1);
								p--;
								break;	
						}	
					}
				}
				break;
		}
		p++;
	}
	
	p = cl;
	
	while (*p)
	{
		if (*p=='\t')
		{	
			*p=' ';
		}
		
		p++;
	}	
}

void formatInsertWhitespace(char* cl)
{
	char*	p = cl;
	p = cl;
	
	while (*p)
	{
		if (*p==' ')
		{	
			*p='\t';
		}
		
		p++;
	}	
}

#ifdef _LITTLE_ENDIAN
void formatSwapStrBytes(char *str, uint32 len)
{
	uint32 i;

//	if (len % 4 != 0) return;

	for (i = 0; i < len; i += 4)
	{
		char c;
		c = str[i + 0];
		str[i + 0] = str[i + 3];
		str[i + 3] = c;
		c = str[i + 1];
		str[i + 1] = str[i + 2];
		str[i + 2] = c;
	}
}
#endif //_LITTLE_ENDIAN

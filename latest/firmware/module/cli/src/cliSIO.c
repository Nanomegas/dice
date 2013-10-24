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
 * DESCRIPTION: Command Line Interface - Serial IO 

	Purpose: 

		This file describes the Command Line Interpreter (cli) Serial IO
	
  Revisions:
		created 18/10/2002 lm
****************************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "TCTasking.h"
#include "cliSIO.h"
#include "cliSystem.h"
#include "cli.h"
#include "cliCB.h"
#include <cyg/io/devtab.h>

#ifdef _CLI

#define CLI_SIO_HISTORY_ITEMS	10
#define CLI_SIO_MAX_STR_LEN		80

static char				cliSIOCommand[CLI_SIO_HISTORY_ITEMS][CLI_SIO_MAX_STR_LEN];
static uint32			cliSIOCursorPos = 0;
static uint32			cliSIOCurrentLen = 0;
static int32			cliSioCurrentHistory = 0;
static uint32			cliSioCurrentHistoryValid = 0;
static uint32			cliSIOCurrentCommand = 0;
static BOOL				cliSIObEchoOn = TRUE;

enum
{
	CLI_SIO_STATE_NORMAL = 0,
	CLI_SIO_STATE_ESCAPE,
	CLI_SIO_STATE_KEYCODES,
	CLI_SIO_STATE_IGNORE
};

static uint32 cliSIOState = CLI_SIO_STATE_NORMAL;

/**********************************************************************
  Function:	cliSIOInitialize
 

  Returns:
	NO_ERROR - successful
	anything else indicates failure

 **********************************************************************/

static cyg_io_handle_t cli_input_handle;

HRESULT	cliSIOInitialize (void)
{
	HRESULT		hResult = NO_ERROR;
	Cyg_ErrNo	err = 0;

	err = cyg_io_lookup( "/dev/ser0", &cli_input_handle );
	if(err != ENOERR)
	{
	  //diag_printf("No serial device\n");
		hResult = E_FAIL;
	}

	return hResult;
}

static char cliGetChar(void)
{
	char		c;
	cyg_uint32	len = 1;

	cyg_io_read(cli_input_handle, &c, &len);

	return c;
}

/**********************************************************************
	cliSIOEchoOn

	Turns on/off the echoing of characters

 **********************************************************************/
void cliSIOEchoOn(BOOL bEchoOn)
{
	cliSIObEchoOn = bEchoOn;
}

/**********************************************************************
	cliSIOIsEchoOn

	Return the echoing of characters mode

 **********************************************************************/
BOOL cliSIOIsEchoOn(void)
{
	return cliSIObEchoOn;
}

void UARTprintBuf(char *data, int size)
{
	fwrite(data, 1, size, stdout);
	fflush(stdout);
}

/**********************************************************************
	cliSIOHandleNewChar

	Serial IO Interrupt Handler calls this function when new char has arrived

 **********************************************************************/
char *cliSIOHandleNewChar(BYTE value)
{
	char *cliSIONewCommand = 0;

	switch (cliSIOState)
	{
		case CLI_SIO_STATE_ESCAPE:
			if (value == KEY_KEYCODES)
			{
				cliSIOState = CLI_SIO_STATE_KEYCODES;
			}
			else
			{
				cliSIOState = CLI_SIO_STATE_IGNORE;
			}
			break;
		case CLI_SIO_STATE_KEYCODES:
			switch (value)
			{
				case KEY_LEFT_ARROW:
					if (cliSIOCursorPos > 0)
					{
						if (cliSIOIsEchoOn())
						{
							putchar(CHAR_BS);
						}
						cliSIOCursorPos--;
					}	
					break;
				case KEY_RIGHT_ARROW:
					if (cliSIOCursorPos < cliSIOCurrentLen)
					{
						if (cliSIOIsEchoOn())
						{
							putchar((BYTE) cliSIOCommand[cliSIOCurrentCommand][cliSIOCursorPos]);
						}
						cliSIOCursorPos++;
					}	
					break;
				case KEY_UP_ARROW:
				case KEY_DOWN_ARROW:
					{
						BOOL 	bChange = FALSE;
						int32	nextHistory = cliSioCurrentHistory;
						
						if (value == KEY_UP_ARROW)
						{
							nextHistory--;
							if (nextHistory == -1) 
							{
								nextHistory = CLI_SIO_HISTORY_ITEMS - 1;
							}
							if (nextHistory > cliSioCurrentHistoryValid)
							{
								nextHistory = 0;
							}
							if (nextHistory != cliSIOCurrentCommand)
							{
								bChange = TRUE;
							}
						}
						else // if (value == KEY_DOWN_ARROW)
						{
							nextHistory++;
							if (nextHistory == CLI_SIO_HISTORY_ITEMS) 
							{
								nextHistory = 0;
							}
							if (nextHistory > cliSioCurrentHistoryValid)
							{
								nextHistory = (int32) cliSioCurrentHistoryValid;
							}

							if (nextHistory != cliSIOCurrentCommand)
							{
								bChange = TRUE;
							}
						}
						
						if (bChange)
						{
							if (cliSIOIsEchoOn())
							{
								uint32	counter = (uint32) (cliSIOCurrentLen - cliSIOCursorPos);
								while (counter)
								{
									putchar(' ');
									counter--;
								}	
								while (cliSIOCurrentLen > 0)
								{
									if (cliSIOIsEchoOn())
									{
										putchar(CHAR_BS);
										putchar(' ');
										putchar(CHAR_BS);
									}
									cliSIOCurrentLen--;
								}
							}
							cliSioCurrentHistory = nextHistory;
							cliSIOCurrentLen = strlen(cliSIOCommand[cliSioCurrentHistory]);
							cliSIOCursorPos = cliSIOCurrentLen;
							
							if (cliSIOIsEchoOn())
							{
								UARTprintBuf(cliSIOCommand[cliSioCurrentHistory], cliSIOCurrentLen);
							}
							if (cliSioCurrentHistory != cliSIOCurrentCommand)
							{
								strcpy(cliSIOCommand[cliSIOCurrentCommand], cliSIOCommand[cliSioCurrentHistory]);
							}
						}
					}
					break;
			}
			cliSIOState = CLI_SIO_STATE_NORMAL;
			break;
			
		case CLI_SIO_STATE_IGNORE:
			cliSIOState = CLI_SIO_STATE_NORMAL;
			break;

		case CLI_SIO_STATE_NORMAL:
			switch (value)
			{
				case CHAR_CR:
				case CHAR_LF:
					{
						BOOL	bUpdate = FALSE;

						if (cliSIOIsEchoOn())
						{
							putchar(13);
							putchar(10);
							fflush(stdout);
						}
						cliSIOCommand[cliSIOCurrentCommand][cliSIOCurrentLen] = 0;
						cliSIOCurrentLen = 0;
						cliSIOCursorPos = 0;
						cliSIONewCommand = cliSIOCommand[cliSIOCurrentCommand];

						if (cliSIOCommand[cliSIOCurrentCommand][0] != 0)
						{
							int32 	preCommand = (int32) (cliSIOCurrentCommand - 1);
							
							bUpdate = TRUE;

							if (preCommand == -1) 
							{
								preCommand = CLI_SIO_HISTORY_ITEMS - 1;
							}
							if (preCommand < cliSioCurrentHistoryValid)
							{
								if (strcmp(cliSIOCommand[cliSIOCurrentCommand], cliSIOCommand[preCommand]) == 0)
								{
									bUpdate = FALSE;
								} 
							}
						}						
						if (bUpdate)
						{
							cliSIOCurrentCommand++;
							if (cliSIOCurrentCommand == CLI_SIO_HISTORY_ITEMS)
							{
								cliSIOCurrentCommand = 0;
							}
							memset(cliSIOCommand[cliSIOCurrentCommand], 0, CLI_SIO_MAX_STR_LEN);
						}
					}
					break;
				case CHAR_BS:
				case CHAR_DEL:
					{
						BOOL bUpdate = FALSE;
						
						if (value == CHAR_BS)
						{
							if (cliSIOCursorPos > 0)
							{
								if (cliSIOIsEchoOn())
								{
									putchar(CHAR_BS);
									putchar(' ');
									putchar(CHAR_BS);
								}
								cliSIOCursorPos--;
								cliSIOCurrentLen--;
								bUpdate = TRUE;
							}
						}
						else // if (value == CHAR_DEL)
						{
							if (cliSIOCursorPos < cliSIOCurrentLen)
							{
								cliSIOCurrentLen--;
								if (cliSIOIsEchoOn())
								{
									putchar(' ');
									putchar(CHAR_BS);
								}
								bUpdate = TRUE;
							}
						}
							
						if (bUpdate && cliSIOCursorPos != cliSIOCurrentLen)
						{
							// cursor is somewhere in the text field - update characters in front of the cursor
							uint32	i;
							if (cliSIOIsEchoOn())
							{
								UARTprintBuf(&(cliSIOCommand[cliSIOCurrentCommand][cliSIOCursorPos + 1]), cliSIOCurrentLen - cliSIOCursorPos);
								putchar(' ');
							}
					    								
							for (i = cliSIOCursorPos; i <= cliSIOCurrentLen; i++)
							{
								cliSIOCommand[cliSIOCurrentCommand][i] = cliSIOCommand[cliSIOCurrentCommand][i + 1];
								if (cliSIOIsEchoOn())
								{
									putchar(CHAR_BS);
								}
							}
						}
					}
					break;

				case CHAR_ESC:
					cliSIOState = CLI_SIO_STATE_ESCAPE;
					break;
					
				default:
					if (cliSIOCurrentLen == CLI_SIO_MAX_STR_LEN - 1)
					{
						if (cliSIOIsEchoOn())
						{
							putchar(CHAR_BELL);
						}
					}
					else
					{
						cliSIOCursorPos++;
						cliSIOCurrentLen++;
						if (cliSIOIsEchoOn())
						{
							putchar(value);
						}

						if (cliSIOCursorPos != cliSIOCurrentLen)
						{
							// cursor is somewhere in the text field - update characters in front of the cursor
							uint32	i;
							if (cliSIOIsEchoOn())
							{
								UARTprintBuf(&(cliSIOCommand[cliSIOCurrentCommand][cliSIOCursorPos - 1]), cliSIOCurrentLen - cliSIOCursorPos);
							}
					    								
							for (i = cliSIOCurrentLen; i >= cliSIOCursorPos; i--)
							{
								cliSIOCommand[cliSIOCurrentCommand][i] = cliSIOCommand[cliSIOCurrentCommand][i - 1];
								if (cliSIOIsEchoOn())
								{
									putchar(CHAR_BS);
								}
							}
							if (cliSIOIsEchoOn())
							{
								putchar(value);
							}
						}
						cliSIOCommand[cliSIOCurrentCommand][cliSIOCursorPos - 1] = (char) value;
					}
					break;
			}
			if (cliSIOState == CLI_SIO_STATE_NORMAL)
			{
				cliSioCurrentHistory = (int32) cliSIOCurrentCommand;
				if (cliSioCurrentHistoryValid < cliSIOCurrentCommand)
				{
					cliSioCurrentHistoryValid = cliSIOCurrentCommand;
				}
			}
	}
	//zluo, flush echo out
	fflush(stdout);
	return cliSIONewCommand;
}

/**********************************************************************
	cliSIOWaitForLine
	Returns pointer to a new string when sent from Interrupt Handler

 **********************************************************************/
char* cliSIOWaitForLine(void)
{
	int c;
	char *line;

	DO_FOREVER
	{
		if ((c = cliGetChar()) > 0 )
		{ //Get cmd from RS-232
			line = cliSIOHandleNewChar(c);
			if (line) return line;
		}
	}
}

/**********************************************************************
	cliSIOWriteChar
	NO_ERROR - successful, anything else indicates failure

 **********************************************************************/

HRESULT	cliSIOWriteChar (SIO_CHANNEL	channel,
						 char			txChar,
						 uint32			msecs2Wait)
{
	HRESULT		hResult = NO_ERROR;

	if (channel >= SIO_NUM_CHANNELS)
	{
		hResult = E_SIO_CHAN_INVALID;
	}

	UNUSED_ARG(msecs2Wait);

	putchar(txChar);
	fflush(stdout);
	return hResult;
}

/**********************************************************************
	cliSIOWriteString
	NO_ERROR - successful, anything else indicates failure

 **********************************************************************/

HRESULT	cliSIOWriteString (SIO_CHANNEL	channel,
						   void			*pStrBuf,
						   uint32		msecs2Wait)
{
	HRESULT		hResult = NO_ERROR;

	if (channel >= SIO_NUM_CHANNELS)
	{
		hResult = E_SIO_CHAN_INVALID;
	}

	UNUSED_ARG(msecs2Wait);

	os_printf(pStrBuf);

	return hResult;
}

/**********************************************************************
	cliSIOWriteBuf
 	NO_ERROR - successful, anything else indicates failure

 **********************************************************************/

HRESULT	cliSIOWriteBuf (SIO_CHANNEL		channel,
						void			*pTxBuf,
						int32			numTxChars,
						uint32			msecs2Wait)
{
	HRESULT		hResult = NO_ERROR;

	if (channel >= SIO_NUM_CHANNELS)
	{
		hResult = E_SIO_CHAN_INVALID;
	}

	UNUSED_ARG(msecs2Wait);

	UARTprintBuf(pTxBuf, (uint32) numTxChars);

	return hResult;
}
#endif //_CLI

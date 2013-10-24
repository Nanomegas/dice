//==========================================================================
//
//      fis.c
//
//      RedBoot - FLASH memory support
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
/****************************************************************************
 * DESCRIPTION: 

	Porting of ECOS Redboot Flash Image System (FIS) CLI
		
	Revisions:
		created 11/30/2004 zluo


*****************************************************************************/
#ifdef _FIS
#include <cyg/io/flash.h>
#include "fis.h"

#ifdef _CLI 
#include"fisCli.h"

static  char	LIST_UsageString[]= "fis.list: \n\r";

HRESULT fis_list(void) 
{
	struct fis_image_desc img;
	unsigned long index = FIS_INIT_GET_IMAGE;
	bool show_cksums = false;
	bool show_datalen = false;

	// Let sysDebugPrintf do the formatting in both cases, rather than counting
	// cols by hand....
	cliPrintf("%-16s  %-10s  %-10s  %-10s  %-s\n\r",
					  "Name","FLASH addr",
					  show_cksums ? "Checksum" : "Mem addr",
						show_datalen ? "Datalen" : "Length",
						"Entry point" );
	fis_get_next_image(&img, &index);
	while( index != FIS_INIT_GET_IMAGE)
	{
		if (img.name[0] != (unsigned char)0xFF) 
		{
			cliPrintf("%-16s  0x%08lX  0x%08lX  0x%08lX  0x%08lX\n\r", img.name, 
			img.flash_base, 
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
			show_cksums ? img.file_cksum : img.mem_base, 
			show_datalen ? img->data_length : img.size, 
#else
			img.mem_base, 
			img.size, 
#endif
			img.entry_point);
		}
		fis_get_next_image(&img, &index);
	}
	return NO_ERROR;
};

static	CLIDescriptor	List_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"list"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&fis_list},
		{kCLI_UsageString, 		(uint32)LIST_UsageString},
		{kCLI_ArgumentPatternString, (uint32)""},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Display contents of FIS "},
		{kCLI_GroupName,		(uint32)"fis"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static  char	WRITE_UsageString[]= "fis.write src_start_address src_length flash_address \n\r";

static	CLIDescriptor	Write_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"write"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&fis_write},
		{kCLI_UsageString, 		(uint32)WRITE_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"iii"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Write to FIS "},
		{kCLI_GroupName,		(uint32)"fis"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static  char	CREATE_UsageString[]= "fis.create src_start_address src_length load_addr entry_addr image_name\n\r";

static	CLIDescriptor	Create_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"create"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&fis_create},
		{kCLI_UsageString, 		(uint32)CREATE_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"iiiis"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Create an image in FIS "},
		{kCLI_GroupName,		(uint32)"fis"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static  char	DELETE_UsageString[]= "fis.delete image_name\n\r";

static	CLIDescriptor	Delete_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"delete"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&fis_delete},
		{kCLI_UsageString, 		(uint32)DELETE_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"s"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Delete an image in FIS "},
		{kCLI_GroupName,		(uint32)"fis"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static  char	ERASE_UsageString[]= "fis.erase flash_start_addr erase_length\n\r";

static	CLIDescriptor	Erase_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"erase"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&fis_erase},
		{kCLI_UsageString, 		(uint32)ERASE_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"ii"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Erase flash content"},
		{kCLI_GroupName,		(uint32)"fis"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static  char	INIT_UsageString[]= "fis.init\n\r";

static	CLIDescriptor	Init_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"init"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&fis_init},
		{kCLI_UsageString, 		(uint32)INIT_UsageString},
		{kCLI_ArgumentPatternString, (uint32)""},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Initialize FIS"},
		{kCLI_GroupName,		(uint32)"fis"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

void fisInstallCLITools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(List_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing fis list cli tools");
	}
	hResult += cliInstallCLIDescriptor(Create_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing fis create cli tools");
	}
	hResult += cliInstallCLIDescriptor(Delete_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing fis delete cli tools");
	}
	hResult += cliInstallCLIDescriptor(Init_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing fis init cli tools");
	}
	hResult += cliInstallCLIDescriptor(Write_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing fis write cli tools");
	}
	
	hResult += cliInstallCLIDescriptor(Erase_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing fis erase cli tools");
	}

}

static char verify_buffer[128];

bool
verify_action(char *fmt, ...)
{
	int16 len;
    va_list ap;
	char *line;

    va_start(ap, fmt);
	len = (int16) vsprintf(verify_buffer, fmt, ap);
	va_end(ap);

	cliOutputRaw(verify_buffer);
	cliOutputRaw(" - continue (y/n)? ");
	line = cliSIOWaitForLine();

	if((line[0] == 'y') || (line[0] == 'Y'))
		return true;
	else
		return false;
}

#endif //_CLI
#endif //_FIS

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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, tkoeller
// Date:         2000-07-28
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================
/****************************************************************************
 * DESCRIPTION: 

	Porting of ECOS Redboot Flash Image System (FIS)
		
	Revisions:
		created 11/30/2004 zluo


*****************************************************************************/

static char	moduleName[] = __FILE__;

//following definations is redboot specific only
#define CYGOPT_REDBOOT_FIS 
#define CYGOPT_REDBOOT_FIS_REDBOOT 1
#define CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK -1
#define CYGNUM_REDBOOT_FLASH_RESERVED_BASE 0
#define CYGBLD_REDBOOT_FLASH_BOOT_OFFSET 0
#define CYGSEM_REDBOOT_FLASH_CONFIG 1
#define CYGBLD_REDBOOT_MIN_IMAGE_SIZE 0x20000

#ifdef _FIS
#include <cyg/io/flash.h>
#include "fis.h"
#include <stdlib.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include "string.h"
#include "TCDebug.h"
#include "TCTasking.h"

#ifdef _CLI
#include "fisCli.h"
#endif // _CLI

// Local data used by these routines
static void *flash_start, *flash_end;
static int flash_block_size, flash_num_blocks;

//Hack to make workspace_start and workspace_end;
static unsigned char *workspace_start = 0;
static unsigned char *workspace_end = (unsigned char *)0x800000;
static unsigned int  workspace_size;
static  unsigned char *ram_start = 0, *ram_end = (unsigned char *)0x800000;

static void *fis_work_block;
static void *fis_addr;
static int fisdir_size;  // Size of FIS directory.

//Kernel is not build with flash configurations, it is an intergrated part of
//Redboot, this will work with TCAT's EVM, modify this if you have a different 
//memory map for config flash.
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
static void *cfg_base  = (void *)0x41e0000;   // Location in Flash of config data
static int   cfg_size = 0x1000;   // Length of config data - rounded to Flash block size
// Prototypes for local functions
//static unsigned char *flash_lookup_config(char *key);
#endif

//This module requires the TC nonblocking flash additions.
#ifndef _TC_FLASH_ADDITIONS
// _TC_FLASH_ADDITIONS is included from <cyg/io/flash.h>
#error "no tc flash additions"
#endif //_TC_FLASH_ADDITIONS


void fis_flash_erase_cb(void)
{
	TCTaskWait(10);
}

void fis_flash_program_cb(void)
{
	TCTaskWait(10);
}


typedef struct
{
	FIS_PROGRESS_FUNC func;
	uint32 promax;
	uint32 procur;
} FIS_PROGRESS_STRUCT;


static int fis_flash_erase_progressinfo (int len)
{
	return 10*len/flash_block_size;
}


static int fis_flash_erase_progress(void *base, int len, void **err_address, FIS_PROGRESS_STRUCT * pProgress)
{
	HRESULT		hResult = NO_ERROR;
  int				stat = FLASH_ERR_OK;

	
	//we will erase one sector at a time to be able to provide progress info
	while (len>=flash_block_size)
	{
		// Non-blocking erase flash operation
		// note: the flash may not be accessed during this flash erase function
		stat = flash_erase_with_cb(base, flash_block_size, err_address, fis_flash_erase_cb);
		if (stat != FLASH_ERR_OK)
		{
			hResult = E_FIS_FLASH_OP_FAILED;
			sysLogError(hResult, __LINE__, moduleName);
			break;
		}
		if (pProgress && pProgress->func)
		{
			pProgress->procur+=10;
			pProgress->func(pProgress->promax, pProgress->procur);
		}
		base=  (void *)(((int)base)+flash_block_size);
		len-=flash_block_size;
	}
	return stat;
}



int fis_flash_erase(void *base, int len, void **err_address)
{
	return fis_flash_erase_progress(base, len, err_address,0);
}

#define FIS_MAX_PRG_BLOCK (10*1024)

static int fis_flash_program_progressinfo(int len)
{
	return (len+FIS_MAX_PRG_BLOCK-1)/FIS_MAX_PRG_BLOCK;
}


static int fis_flash_program_progress(void *flash_base, void *ram_base, int len, void **err_address, FIS_PROGRESS_STRUCT * pProgress)
{
	HRESULT		hResult = NO_ERROR;
    int			stat=FLASH_ERR_OK;

	// Non-blocking program flash operation
	{ //ML: program smaller blocks, max 10k at a time to make sure we yield
		int remain = len;
		while (remain)
		{
			if (remain > FIS_MAX_PRG_BLOCK)
			{
				stat = flash_program_with_cb(flash_base, ram_base, FIS_MAX_PRG_BLOCK, err_address, fis_flash_program_cb);
				flash_base = (char *)flash_base + FIS_MAX_PRG_BLOCK;
				ram_base = (char *)ram_base + FIS_MAX_PRG_BLOCK;
				remain-=FIS_MAX_PRG_BLOCK;
			}
			else
			{
				stat = flash_program_with_cb(flash_base, ram_base, remain, err_address, fis_flash_program_cb);
				remain = 0;			
			}
			if (stat != FLASH_ERR_OK) break;
			if (pProgress && pProgress->func)
			{
				pProgress->procur+=1;
				pProgress->func(pProgress->promax, pProgress->procur);
			}
		}
	}
	if (stat != FLASH_ERR_OK)
	{
		hResult = E_FIS_FLASH_OP_FAILED;
		sysLogError(hResult, __LINE__, moduleName);
	}
	return stat;
}


int fis_flash_program(void *flash_base, void *ram_base, int len, void **err_address)
{
	return fis_flash_program_progress(flash_base, ram_base, len, err_address, 0);
}

static void fis_update_directory(void)
{
    int stat;
    void *err_addr;
	
#ifdef CYGSEM_REDBOOT_FLASH_COMBINED_FIS_AND_CONFIG
    memcpy((char *)fis_work_block+fisdir_size, config, cfg_size);
#endif
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Ensure [quietly] that the directory is unlocked before trying to update
    flash_unlock((void *)fis_addr, flash_block_size, (void **)&err_addr);
#endif
    if ((stat = fis_flash_erase(fis_addr, flash_block_size, (void **)&err_addr)) != 0) {
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Error erasing FIS directory at %p: %s\n\r", err_addr, flash_errmsg(stat));
    } else {
        if ((stat = fis_flash_program(fis_addr, fis_work_block, flash_block_size,
								  (void **)&err_addr)) != 0) {
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Error writing FIS directory at %p: %s\n\r", 
							   err_addr, flash_errmsg(stat));
        }
    }
#ifdef CYGSEM_REDBOOT_FLASH_LOCK_SPECIAL
    // Ensure [quietly] that the directory is locked after the update
    flash_lock((void *)fis_addr, flash_block_size, (void **)&err_addr);
#endif
}

HRESULT fis_init(void)
{
	int stat;
	struct fis_image_desc *img;
	void *err_addr;
	bool full_init = true;
	CYG_ADDRESS redboot_flash_start;
	unsigned long redboot_image_size;


	SYS_DEBUG(SYSDEBUG_TRACE_FIS, "*** Initialize FLASH Image System\n\r");

#define MIN_REDBOOT_IMAGE_SIZE CYGBLD_REDBOOT_MIN_IMAGE_SIZE
	redboot_image_size = flash_block_size > MIN_REDBOOT_IMAGE_SIZE ? 
	flash_block_size : MIN_REDBOOT_IMAGE_SIZE;

	// Create a pseudo image for RedBoot
	img = (struct fis_image_desc *)fis_work_block;
	memset(img, 0xFF, fisdir_size);  // Start with erased data
#ifdef CYGOPT_REDBOOT_FIS_RESERVED_BASE
	memset(img, 0, sizeof(*img));
	strcpy(img->name, "(reserved)");
	img->flash_base = (CYG_ADDRESS)flash_start;
	img->mem_base = (CYG_ADDRESS)flash_start;
	img->size = CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
	img++;
#endif
	redboot_flash_start = (CYG_ADDRESS)flash_start + CYGBLD_REDBOOT_FLASH_BOOT_OFFSET;
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT
	memset(img, 0, sizeof(*img));
	strcpy(img->name, "RedBoot");
	img->flash_base = redboot_flash_start;
	img->mem_base = redboot_flash_start;
	img->size = redboot_image_size;
	img++;
	redboot_flash_start += redboot_image_size;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_POST
#ifdef CYGNUM_REDBOOT_FIS_REDBOOT_POST_OFFSET
	// Take care to place the POST entry at the right offset:
	redboot_flash_start = (CYG_ADDRESS)flash_start + CYGNUM_REDBOOT_FIS_REDBOOT_POST_OFFSET;
#endif
	memset(img, 0, sizeof(*img));
	strcpy(img->name, "RedBoot[post]");
	img->flash_base = redboot_flash_start;
	img->mem_base = redboot_flash_start;
	img->size = redboot_image_size;
	img++;
	redboot_flash_start += redboot_image_size;

	SYS_DEBUG(SYSDEBUG_TRACE_FIS, "redboot_flash_start = %x.\n\r", redboot_flash_start);

#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_BACKUP
	// And a backup image
	memset(img, 0, sizeof(*img));
	strcpy(img->name, "RedBoot[backup]");
	img->flash_base = redboot_flash_start;
	img->mem_base = redboot_flash_start;
	img->size = redboot_image_size;
	img++;
	redboot_flash_start += redboot_image_size;
#endif
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
	// And a descriptor for the configuration data
	memset(img, 0, sizeof(*img));
	strcpy(img->name, "RedBoot config");
	img->flash_base = (CYG_ADDRESS)cfg_base;
	img->mem_base = (CYG_ADDRESS)cfg_base;
	img->size = cfg_size;
	img++;
#endif
	// And a descriptor for the descriptor table itself
	memset(img, 0, sizeof(*img));
	strcpy(img->name, "FIS directory");
	img->flash_base = (CYG_ADDRESS)fis_addr;
	img->mem_base = (CYG_ADDRESS)fis_addr;
	img->size = fisdir_size;
	img++;

	// Do this after creating the initialized table because that inherently
	// calculates where the high water mark of default RedBoot images is.

	if (full_init) 
	{
		unsigned long erase_size;
		CYG_ADDRESS erase_start;
		// Erase everything except default RedBoot images, fis block, 
		// and config block.
		// First deal with the possible first part, before RedBoot images:
#if (CYGBLD_REDBOOT_FLASH_BOOT_OFFSET > CYGNUM_REDBOOT_FLASH_RESERVED_BASE)
		erase_start = (CYG_ADDRESS)flash_start + CYGNUM_REDBOOT_FLASH_RESERVED_BASE;
		erase_size =  (CYG_ADDRESS)flash_start + CYGBLD_REDBOOT_FLASH_BOOT_OFFSET;
		if ( erase_size > erase_start )
		{
			erase_size -= erase_start;
      if ((stat = fis_flash_erase((void *)erase_start, erase_size, (void **)&err_addr)) != 0) 
			{
				SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   initialization failed at %p: %s\n\r", err_addr, flash_errmsg(stat));
			}
		}
#endif
		//zluo, hack 
		if(redboot_flash_start < 0x4020000)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "redboot_flash_start = %p\n\r", redboot_flash_start);
			return NO_ERROR;
		}
		// second deal with the larger part in the main:
		erase_start = redboot_flash_start; // high water of created images
		// Now the empty bits between the end of Redboot and the cfg and dir 
		// blocks. 
#if defined(CYGSEM_REDBOOT_FLASH_CONFIG) && !defined(CYGSEM_REDBOOT_FLASH_COMBINED_FIS_AND_CONFIG)
		if (fis_addr > cfg_base) 
		{
			erase_size = (CYG_ADDRESS)cfg_base - erase_start; // the gap between HWM and config data
		} else 
		{
			erase_size = (CYG_ADDRESS)fis_addr - erase_start; // the gap between HWM and fis data
		}
		if ((stat = fis_flash_erase((void *)erase_start, erase_size,
                                    (void **)&err_addr)) != 0) 
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   initialization failed %p: %s\n\r",
								 err_addr, flash_errmsg(stat));
		}
		erase_start += (erase_size + flash_block_size);
		if (fis_addr > cfg_base) 
		{
			erase_size = (CYG_ADDRESS)fis_addr - erase_start; // the gap between config and fis data
		} 
		else 
		{
			erase_size = (CYG_ADDRESS)cfg_base - erase_start; // the gap between fis and config data
		}
		if ((stat = fis_flash_erase((void *)erase_start, erase_size,
																(void **)&err_addr)) != 0) 
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   initialization failed %p: %s\n\r",
					 err_addr, flash_errmsg(stat));
		}
		erase_start += (erase_size + flash_block_size);
#else  // !CYGSEM_REDBOOT_FLASH_CONFIG        
		erase_size = (CYG_ADDRESS)fis_addr - erase_start; // the gap between HWM and fis data
		if ((stat = fis_flash_erase((void *)erase_start, erase_size, (void **)&err_addr)) != 0) 
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   initialization failed %p: %s\n\r", err_addr, flash_errmsg(stat));
		}
		erase_start += (erase_size + flash_block_size);          
#endif
		// Lastly, anything at the end
		erase_size = ((CYG_ADDRESS)flash_end - erase_start) + 1;
		if ((stat = fis_flash_erase((void *)erase_start, erase_size, (void **)&err_addr)) != 0) 
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   initialization failed at %p: %s\n\r",
								 err_addr, flash_errmsg(stat));
		}
	} 
	else 
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "    Warning: device contents not erased, some blocks may not be usable\n\r");
	}
	fis_update_directory();
	return NO_ERROR;
}

static int __flash_init = 0;

void _show_invalid_flash_address(CYG_ADDRESS flash_addr, int stat)
{
	SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Invalid FLASH address %p: %s\n\r", (void *)flash_addr, flash_errmsg(stat));
	SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   valid range is %p-%p\n\r", (void *)flash_start, (void *)flash_end);
}

void _flash_info(void)
{
	if (!__flash_init) return;
	SYS_DEBUG(SYSDEBUG_TRACE_FIS, "FLASH: %p - %p, %d blocks of %p bytes each.\n\r", 
						flash_start, (CYG_ADDRWORD)flash_end + 1, flash_num_blocks, (void *)flash_block_size);
}

void fisPrintf(const char * format, ...)
{
	//no need to do anything here, nonBlock functions yield
}


static bool do_flash_init(void)
{
	int stat;

	ram_start = (unsigned char *)CYGMEM_REGION_ram;
	ram_end = (unsigned char *)(CYGMEM_REGION_ram+CYGMEM_REGION_ram_SIZE);
#ifdef CYGMEM_SECTION_heap1
	workspace_start = (unsigned char *)CYGMEM_SECTION_heap1;
	workspace_end = (unsigned char *)(CYGMEM_SECTION_heap1+CYGMEM_SECTION_heap1_SIZE);
	workspace_size = CYGMEM_SECTION_heap1_SIZE;
#endif
	if (!__flash_init) 
	{
		if ((stat = flash_init((void *)(workspace_end-FLASH_MIN_WORKSPACE), 
														 FLASH_MIN_WORKSPACE, (_printf *)fisPrintf)) != 0) 
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "FLASH: driver init failed: %s\n\r", flash_errmsg(stat));
			return false;
		}
		flash_get_limits((void *)0, (void **)&flash_start, (void **)&flash_end);
		// Keep 'end' address as last valid location, to avoid wrap around problems
		flash_end = (void *)((CYG_ADDRESS)flash_end - 1);
		flash_get_block_info(&flash_block_size, &flash_num_blocks);

		workspace_end = (unsigned char *)(workspace_end-FLASH_MIN_WORKSPACE);
#ifdef CYGOPT_REDBOOT_FIS
#ifdef CYGOPT_REDBOOT_FIS_ZLIB_COMMON_BUFFER
		fis_work_block = fis_zlib_common_buffer;
		if(CYGNUM_REDBOOT_FIS_ZLIB_COMMON_BUFFER_SIZE < flash_block_size)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "FLASH: common buffer too small\n\r");
								workspace_end += FLASH_MIN_WORKSPACE;
			return false;
		}
#else
		workspace_end = (unsigned char *)(workspace_end-flash_block_size);
		fis_work_block = workspace_end;
#endif
		fisdir_size = flash_block_size;
		if (CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK < 0) 
		{
			fis_addr = (void *)((CYG_ADDRESS)flash_end + 1 +
													(CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK*flash_block_size));
		} 
		else 
		{
			fis_addr = (void *)((CYG_ADDRESS)flash_start + 
													(CYGNUM_REDBOOT_FIS_DIRECTORY_BLOCK*flash_block_size));
		}
#endif
		__flash_init = 1;
	}
	return true;
}

// Find the first unused area of flash which is long enough
static bool fis_find_free_checkaddr(CYG_ADDRESS *addr, unsigned long length, bool checkaddr)
{
	unsigned long *fis_ptr, *fis_end;
	unsigned long *area_start;

	// Do not search the area reserved for pre-RedBoot systems:
	fis_ptr = (unsigned long *)((CYG_ADDRESS)flash_start + CYGNUM_REDBOOT_FLASH_RESERVED_BASE);
	fis_end = (unsigned long *)(CYG_ADDRESS)flash_end;
	area_start = fis_ptr;
	while (fis_ptr < fis_end) 
	{
		if (*fis_ptr != (unsigned long)0xFFFFFFFF)
		{
			if (area_start != fis_ptr)
			{
					// Assume that this is something
					if ((fis_ptr-area_start) >= (length/sizeof(unsigned)))
					{
							if (checkaddr)
							{
								if ((fis_ptr-(unsigned long *)*addr) >= (length/sizeof(unsigned)))
								//						if (*addr == (CYG_ADDRESS)area_start)
								{
									return true;
								}
							}
							else
							{
								*addr = (CYG_ADDRESS)area_start;
								return true;
							}
					}
			}
			// Find next blank block
			area_start = fis_ptr;
			while (area_start < fis_end)
			{
				if (*area_start == (unsigned long)0xFFFFFFFF)
				{
					break;
				}
				area_start += flash_block_size / sizeof(CYG_ADDRESS);
			}
			fis_ptr = area_start;
    } 
		else 
		{
			fis_ptr += flash_block_size / sizeof(CYG_ADDRESS);
		}
	}
	if (area_start != fis_ptr) 
	{
		if ((fis_ptr-area_start) >= (length/sizeof(unsigned)))
		{
			*addr = (CYG_ADDRESS)area_start;
			return true;
		}
	}
	return false;
}

static bool fis_find_free(CYG_ADDRESS *addr, unsigned long length)
{
	return fis_find_free_checkaddr(addr, length, FALSE);
}

HRESULT fis_create_progress(CYG_ADDRESS mem_addr, uint32 length, 
		   CYG_ADDRESS exec_addr, CYG_ADDRESS entry_addr,
		   char *name, BOOL bDeleteIfNeeded, FIS_PROGRESS_FUNC progressFunc)
{
	int stat, i;
	unsigned long img_size;
	CYG_ADDRESS flash_addr;
	void *err_addr;
	struct fis_image_desc *img = NULL;
	BOOL bFlashAddrGood = FALSE;
	BOOL bDelOldImage = FALSE;
	FIS_PROGRESS_STRUCT progress;

	memset (&progress,0,sizeof(progress));
	memcpy(fis_work_block, fis_addr, fisdir_size);

	img_size = length;

	// 'length' is size of FLASH image, 'img_size' is actual data size
	// Round up length to FLASH block size
	length = ((length + flash_block_size - 1) / flash_block_size) * flash_block_size;
	if (length < img_size) 
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Internal error in image\n\r");
		return E_FIS_ILLEGAL_IMAGE;
	}

	if (!name)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Internal error in image\n\r");
		return E_FIS_ILLEGAL_IMAGE;
	}
	if (strlen(name) >= sizeof(img->name))
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Internal error in image\n\r");
		return E_FIS_ILLEGAL_IMAGE;
	}

	// Search existing files to acquire defaults for params not specified:
	img = fis_lookup(name, NULL);

	// If we have an image we need to check if it is any good
	if (img) 
	{
		if (length != img->size)
		{
			//in this case we either return an error or delete the old image
			if (!bDeleteIfNeeded) 
			{	
				SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Image already exist with different size\n\r");
				return E_FIS_ILLEGAL_IMAGE;
			}
			//We need to remember to delete the image
			bDelOldImage = TRUE;
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Delete old image\n\r");
		}
		else
		{
			bFlashAddrGood = TRUE;
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Reuse old image\n\r");
		}
	}
	else
	{
		//need to find a new free image
		img = (struct fis_image_desc *)fis_work_block;
		for (i = 0;  i < fisdir_size/sizeof(*img);  i++, img++)
		{
			if (img->name[0] == (unsigned char)0xFF) break;
		}
		if (img->name[0] != (unsigned char)0xFF)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "No more directory entries\n\r");
			return E_FIS_NO_SPACE;
		}
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Using new image\n\r");
	}
	//we definitely have an img pointer which is valid
	//Let's figure out the work we need to do for the progress info
	if (progressFunc) 
	{
		progress.func = progressFunc;
		progress.procur = 0;
		progress.promax = fis_flash_program_progressinfo(img_size);
		if (bDelOldImage || bFlashAddrGood) progress.promax += fis_flash_erase_progressinfo (img->size);
		if (!bFlashAddrGood) progress.promax += fis_flash_erase_progressinfo (length);
		progress.promax += 10; //to update the directory 		
		progressFunc(progress.promax, progress.procur);
	}

	if (bDelOldImage || bFlashAddrGood)
	{
	  if ((stat = fis_flash_erase_progress((void *)img->flash_base, img->size, &err_addr, &progress )) != 0)
		{	
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Flash Erase failed\n\r");
			return E_FIS_FLASH_OP_FAILED;
		}
	}
	if (!bFlashAddrGood)
	{
		//we need to find space
		if (!fis_find_free(&flash_addr, length))
		{	
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "No free space in flash\n\r");
			return E_FIS_NO_SPACE;
		}
	} 
	else
	{
		flash_addr = img->flash_base;
	}
	//at this point we have erased old stuff if needed and we are ready to program

	if (((stat = flash_verify_addr((void *)flash_addr)) ||
			 (stat = flash_verify_addr((void *)(flash_addr+img_size-1)))))
	{
		//this should not happen, it is an internal error
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Internal error, illegal flash address\n\r");
		return E_FIS_FLASH_OP_FAILED;
	}
	if (flash_addr & (flash_block_size-1))
	{
		//this should not happen, it is an internal error
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Internal error, illegal flash address\n\r");
		return E_FIS_FLASH_OP_FAILED;
	}

	// We need to erase the new are we found but it is likely clean so we will not progress it
	// as it will only be a blank check
	if (!bFlashAddrGood)
	{
	  if ((stat = fis_flash_erase_progress((void *)img->flash_base, img->size, &err_addr, &progress)) != 0)
		{	
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Flash erase failed\n\r");
			return E_FIS_FLASH_OP_FAILED;
		}		
	}

	if (mem_addr!=0xffffffff) //ML:Create image without programming if addr=0xffffffff
	{
		// Now program it
		if ((stat = fis_flash_program_progress((void *)flash_addr, (void *)mem_addr, img_size, &err_addr,&progress)) != 0)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Flash program failed\n\r");
			return E_FIS_FLASH_OP_FAILED;
		}
	}
	// Update directory
	memset(img, 0, sizeof(*img));
	strcpy(img->name, name);
	img->flash_base = flash_addr;
	img->mem_base = exec_addr;
	img->entry_point = entry_addr;  // Hope it's been set
	img->size = length;
	img->data_length = img_size;
#ifdef CYGSEM_REDBOOT_FIS_CRC_CHECK
	img->file_cksum = cyg_crc32((unsigned char *)flash_addr, img_size);
#endif
	fis_update_directory();
	if (progressFunc) progressFunc(progress.promax, progress.promax);
	return NO_ERROR;
}


HRESULT fis_create(CYG_ADDRESS mem_addr, uint32 length, 
		   CYG_ADDRESS exec_addr, CYG_ADDRESS entry_addr,
		   char *name)
{
	return fis_create_progress(mem_addr, length, exec_addr, entry_addr, name, FALSE, 0);
}

struct fis_image_desc * fis_lookup(char *name, int *num)
{
	int i;
	struct fis_image_desc *img;

	img = (struct fis_image_desc *)fis_work_block;
	for (i = 0;  i < fisdir_size/sizeof(*img);  i++, img++)
	{
		if ((img->name[0] != (unsigned char)0xFF) && 
				(strcasecmp(name, img->name) == 0))
		{
			if (num) *num = i;
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "find image: %s.\n\r", img->name);
			return img;
		}
	}
	return (struct fis_image_desc *)0;
}

HRESULT fis_get_next_image(struct fis_image_desc *ret, unsigned long *index)
{
	struct fis_image_desc *img;
	int i;
	img = (struct fis_image_desc *) fis_addr ;
	img += *index + 1;
	for (i = *index+1;  i < fisdir_size/sizeof(*img);  i++, img++)
	{
		if (img->name[0] != (unsigned char)0xFF) 
		{
			*ret = *img;
			*index = i;
			return NO_ERROR;
		}
	}
	*index = FIS_INIT_GET_IMAGE;
	return E_GEN_NOMATCH;
}

HRESULT fis_write(CYG_ADDRESS mem_addr, uint32 length, CYG_ADDRESS flash_addr)
{
	int stat;
	void *err_addr;
	bool prog_ok;

	SYS_DEBUG(SYSDEBUG_TRACE_FIS, "fis_write(%x, %x, %x)\n\r", mem_addr, length, flash_addr);

	// Round up length to FLASH block size
	if (((stat = flash_verify_addr((void *)flash_addr)) ||
			 (stat = flash_verify_addr((void *)(flash_addr+length-1))))) 
	{
		_show_invalid_flash_address(flash_addr, stat);
		return NO_ERROR;
	}
	if (flash_addr & (flash_block_size-1)) 
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Invalid FLASH address: %p\n\r", (void *)flash_addr);
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   must be 0x%x aligned\n\r", flash_block_size);
		return NO_ERROR;
	}
	if ((mem_addr < (CYG_ADDRESS)ram_start) ||
			((mem_addr+length) >= (CYG_ADDRESS)ram_end))
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "** WARNING: RAM address: %p may be invalid\n\r", (void *)mem_addr);
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   valid range is %p-%p\n\r", (void *)ram_start, (void *)ram_end);
	}
	// Safety check - make sure the address range is not within the code we're running
	if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+length-1)))
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Can't program this region - contains code in use!\n\r");
		return NO_ERROR;
	}
	prog_ok = true;
	if (prog_ok)
	{
		// Erase area to be programmed
		if ((stat = fis_flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Can't erase region at %p: %s\n\r", err_addr, flash_errmsg(stat));
			prog_ok = false;
		}
	}
	if (prog_ok)
	{
		// Now program it
		if ((stat = fis_flash_program((void *)flash_addr, (void *)mem_addr, length, (void **)&err_addr)) != 0)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "fis_write: can't program region at %p: %s\n\r", 
			err_addr, flash_errmsg(stat));

			//sysDebugPrintf("Can't program region at %p: %s\n\r", err_addr, stat);
			// prog_ok = false;
		}
	}
	return NO_ERROR;
}

HRESULT fis_delete_progress(char *name, FIS_PROGRESS_FUNC progressFunc)
{
	int num_reserved, i, stat;
	void *err_addr;
	struct fis_image_desc *img;
	FIS_PROGRESS_STRUCT progress;

	memset (&progress,0,sizeof(progress));

	img = (struct fis_image_desc *)fis_work_block;
	num_reserved = 0;
#ifdef CYGOPT_REDBOOT_FIS_RESERVED_BASE
	num_reserved++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT
	num_reserved++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_BACKUP
	num_reserved++;
#endif
#ifdef CYGOPT_REDBOOT_FIS_REDBOOT_POST
	num_reserved++;
#endif
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
	num_reserved++;
#endif
	num_reserved++;

	memcpy(fis_work_block, fis_addr, fisdir_size);
	img = fis_lookup(name, &i);
	if (img)
	{
		if (i < num_reserved)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Sorry, '%s' is a reserved image and cannot be deleted\n\r", img->name);
			return E_FIS_ILLEGAL_IMAGE;
		}
	} 
	else
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "No image '%s' found\n\r", name);
		return E_FIS_ILLEGAL_IMAGE;
	}
	if (progressFunc) 
	{
		progress.func = progressFunc;
		progress.procur = 0;
		progress.promax = fis_flash_erase_progressinfo (img->size);
		progress.promax += 10; //to update the directory 		
		progressFunc(progress.promax, progress.procur);
	}

	// Erase Data blocks (free space)
	if ((stat = fis_flash_erase_progress((void *)img->flash_base, img->size, (void **)&err_addr, &progress)) != 0)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Error erasing at %p: %s\n\r", err_addr, flash_errmsg(stat));
		return E_FIS_FLASH_OP_FAILED;
	}
	else
	{
		img->name[0] = (unsigned char)0xFF;    
		fis_update_directory();
	}
	progressFunc(progress.promax, progress.promax);
	return NO_ERROR;
}

HRESULT fis_delete(char *name)
{
	return fis_delete_progress(name, 0);
}


HRESULT fis_erase(CYG_ADDRESS flash_addr, unsigned long length)
{
	int stat;
	void *err_addr;

	if (((stat = flash_verify_addr((void *)flash_addr)) ||
			 (stat = flash_verify_addr((void *)(flash_addr+length-1)))))
	{
		_show_invalid_flash_address(flash_addr, stat);
		return NO_ERROR;
	}
	if (flash_addr & (flash_block_size-1))
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Invalid FLASH address: %p\n\r", (void *)flash_addr);
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "   must be 0x%x aligned\n\r", flash_block_size);
		return NO_ERROR;
	}
	// Safety check - make sure the address range is not within the code we're running
	if (flash_code_overlaps((void *)flash_addr, (void *)(flash_addr+length-1)))
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Can't erase this region - contains code in use!\n\r");
		return NO_ERROR;
	}
	if ((stat = fis_flash_erase((void *)flash_addr, length, (void **)&err_addr)) != 0)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_FIS, "Error erasing at %p: %s\n\r", err_addr, flash_errmsg(stat));
	}
	return NO_ERROR;
}



void fisInitialize(void)
{
	do_flash_init(); //hack, put the init here.
#ifdef _CLI
	fisInstallCLITools();
#endif 
}

#endif //_FIS

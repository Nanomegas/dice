#ifndef _FIS_H_
#define _FIS_H_
//==========================================================================
//
//      fis.h
//
//      RedBoot - FLASH image directory layout
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
#include "TCTypes.h"
#include <cyg/infra/cyg_type.h>
#include "ErrorCodes.h"

#define FIS_IMAGE_DESC_SIZE_UNPADDED \
  (16 + 4 * sizeof(unsigned long) + 3 * sizeof(CYG_ADDRESS))

typedef struct fis_image_desc {
    unsigned char name[16];      // Null terminated name
    CYG_ADDRESS   flash_base;    // Address within FLASH of image
    CYG_ADDRESS   mem_base;      // Address in memory where it executes
    unsigned long size;          // Length of image
    CYG_ADDRESS   entry_point;   // Execution entry point
    unsigned long data_length;   // Length of actual data
  //unsigned char _pad[CYGNUM_REDBOOT_FIS_DIRECTORY_ENTRY_SIZE-FIS_IMAGE_DESC_SIZE_UNPADDED];
    unsigned char _pad[256-FIS_IMAGE_DESC_SIZE_UNPADDED];
    unsigned long desc_cksum;    // Checksum over image descriptor
    unsigned long file_cksum;    // Checksum over image data
} FIS_IMAGE;


struct fis_image_desc *fis_lookup(char *name, int *num);

void fisInitialize(void);

//fis_init will format the flash system.
HRESULT fis_init(void);

//fis_create and fis_delete operate on images.
HRESULT fis_create(CYG_ADDRESS mem_addr, uint32 length, 
				   CYG_ADDRESS exec_addr, CYG_ADDRESS entry_addr,
				   char *name);
HRESULT fis_create_empty_image(CYG_ADDRESS flash_addr, uint32 length, char *name);
HRESULT fis_delete(char *name);

//fis_write and fis_erase are for direct flash operations.
HRESULT fis_write(CYG_ADDRESS mem_addr, uint32 length, CYG_ADDRESS flash_addr);
HRESULT fis_erase(CYG_ADDRESS flash_addr, unsigned long length);

#define FIS_INIT_GET_IMAGE -1
HRESULT fis_get_next_image(struct fis_image_desc *img, unsigned long *index);

typedef HRESULT (*FIS_PROGRESS_FUNC)(uint32 promax, uint32 procurr);

HRESULT fis_create_progress(CYG_ADDRESS mem_addr, uint32 length, 
		   CYG_ADDRESS exec_addr, CYG_ADDRESS entry_addr,
		   char *name, BOOL bDeleteIfNeeded, FIS_PROGRESS_FUNC progressFunc);

HRESULT fis_delete_progress(char *name, FIS_PROGRESS_FUNC progressFunc);



#endif  //_FIS_H_

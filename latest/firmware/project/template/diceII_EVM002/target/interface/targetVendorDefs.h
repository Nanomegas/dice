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
 /***************************************************************************
 * DESCRIPTION: 1394 Vendor and Product IDs

	Revisions:
		created 08/06/2002 lm

****************************************************************************/

#ifndef _TARGETVENDORDEFS_H
#define _TARGETVENDORDEFS_H

#include "TCTypes.h"
#include "1394defs.h"
#include "tcat_dice_myproduct.h"

/** @addtogroup myApp
@par About WWUID and Vendor ID
	A note about the device WWUID and Vendor ID
	@par
	Note that the Vendor ID is a 3-byte value stored as the most significant <br>
	3 bytes of the CSR in the case of Configuration ROM.  This quadlet is <br>
	stored in the bus info block.  Together with chip_id_hi and chip_id_low, <br>
	this constitutes the entire WWUID for the node.  Chip_id_hi and chip_id_low <br>
	must be unique for all devices shipped with the particular Vendor ID. <br>
	@par
	WWUID, as shown in Configuration ROM
	@code
	+--+--+--+--+
	|vendorID|hi|		Bus info block: offset 0xf000040c
	+--+--+--+--+
	|chip_id_low|		Bus info block: offset 0xf0000410
	+--+--+--+--+
	@endcode
	@par
	The 40 bit chip ID is allocated as follows
	@par
    Upper 32 bits of WWUID
	@code
	+------------------------+--------+
	|  24 bit OUI – 0x000166 | Cat    |
	+------------------------+--------+
	@endcode
	@par
	Category 8 bits <br>
		0x04                 All DICE II devices
	@par
	Lower 32 bits of WWUID
	@code
	+----------------------------+----------------+
	|  10 bit product identifier | 22 bit serial# |
	+----------------------------+----------------+
	@endcode
	
	<b>How To Personalize a Device: </b> <br> <br>
	In <b> targetVendorDefs .h </b> : <br>
    Change 	<b>THIS_VENDOR_ID</b>		to your OUI <br>
    Change  <b>THIS_VENDOR_NAME</b>		to the name of your company (as registered with the IEEE) <br>
    Change	<b>THIS_PRODUCT_ID</b>		to the ID you selected for your product <br>
    Change  <b>THIS_PRODUCT_NAME</b>	to the name of your product <br>

	The defines above are linked to the defines in <b> tcat_dice_myproduct.h</b>. No changes to this file are 
	needed as the information will come from the customization file.<br>
	
	If the driver supports stacking of several different devices with different ID the correct defines
	should be selected below for product ID and product name.<br>	

*/





// define masks for extracting a vendor id from a CSR WWUID HIGH quadlet
#define MASK_VENDOR_ID				0xffffff00
#define MASK_CATEGORY_ID			0x000000ff
#define SHIFT_VENDOR_ID				8

// define masks for extracting a product id from a CSR WWUID LOW quadlet
#define MASK_PRODUCT_ID				0xffc00000
#define MASK_PRODUCT_SERIAL			0x003fffff
#define SHIFT_PRODUCT_ID			22
#define MAX_PRODUCT_SERIAL			MASK_PRODUCT_SERIAL

//--------------------------------------------------------------------------------

#define VENDOR_ID_YAMAHA			0x00A0DE		// Yamaha, used for mLAN spec

#define CAT_ID_DICE 			    0x04 



									
//--------------------------------------------------------------------------------

#define THIS_VENDOR_ID				kTCAT_DICE_VENDOR_ID
#define THIS_VENDOR_NAME			TCAT_STRINGIFY(kTCAT_DICE_VENDOR_SHORT)
#define THIS_CATEGORY_ID			CAT_ID_DICE


#define THIS_PRODUCT_ID				kTCAT_DICE_DEVICE_2_ID
#define THIS_PRODUCT_NAME			TCAT_STRINGIFY(kTCAT_DICE_DEVICE_2_MODEL_ID)


//--------------------------------------------------------------------------------

#define GET_WWUID_HIGH              ((THIS_VENDOR_ID << SHIFT_VENDOR_ID)	| (THIS_CATEGORY_ID & MASK_CATEGORY_ID))
#define GET_WWUID_LOW(serial)       ((THIS_PRODUCT_ID << SHIFT_PRODUCT_ID)	| ((serial) & MASK_PRODUCT_SERIAL))

#define DEFAULT_DEVICE_ID_LOW		GET_WWUID_LOW(MAX_PRODUCT_SERIAL)       
#define DEFAULT_DEVICE_ID_HIGH		GET_WWUID_HIGH

//--------------------------------------------------------------------------------

#define DEVICE_LONG_NAME			9
#define DEVICE_SHORT_NAME			3

#ifdef _CLI

typedef struct
{
	WWUID	wwuid;
	char	name[DEVICE_LONG_NAME + 1];
} KNOWN_DEVICE_INFO;

void	targetVendorDefsGetNameFromWWUID(KNOWN_DEVICE_INFO *info);
void	targetVendorDefsPrintVendorID(uint32 vendorID);

#endif // _CLI

#endif //_TARGETVENDORDEFS_H

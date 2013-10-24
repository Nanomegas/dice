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

#include <cyg/kernel/kapi.h>
#include <stdlib.h>
#include <stdio.h>
#include "targetVendorDefs.h"
#include "tcat_dice_version.h" 



char _diceApp_build_time [] CYGBLD_ATTRIB_WEAK =
   "*                       - built on " __TIME__ ", " __DATE__ " *\n";

char _diceApp_date[] CYGBLD_ATTRIB_WEAK = __DATE__;
char _diceApp_time[] CYGBLD_ATTRIB_WEAK = __TIME__;

//It is up to the application to determine how to handle versioning of the app.
//We suggest a scheme like the one used by the SDK where
//Major upper 8 bits (0.255)
//Minor next 4 bits (0..15)
//Sub next 4 bits (0..15)
//Build last 16 bits (0..65535)

#define APP_MAJ	0
#define APP_MIN 0
#define APP_SUB 0
#define APP_BUILD 0

#define APP_VER32	((APP_MAJ<<24)|(APP_MIN<<20)|(APP_SUB<<16)|(APP_BUILD))

#ifdef _USE_OLD_VERSION_INFO


// DEPRECIATED AFTER SDK BUILD 108

// This struct embeds the version info into the compiled binary, allowing host applications
//	to find the values in the image. Host utilites can search for the GUID below, and then use 
//  the adjacent info to verify versions, target model info, etc. before updating target device 
//	application code. If your product will allow firmware updates, then make sure that the vendor 
//	ID and product ID strings match your values in targetVendorDefs.h
// ** Also, never change the MagicNumb (GUID) below if you wish to use utilities provided with the development tools.

//
//
//

typedef struct
{
	char MagicNum[36];		// initialized below, do not change this number
	char Major[5];			// 4 digits followed by a space
	char Minor[5];			// 4 digits followed by a space
	char VendorID[7];		// 6 digit 24-bit OUI (as defined in targetVendorDefs.h) followed by a space
	char VProductID[4];		// 3 digit (10 bits) Product ID (as defined in targetVendorDefs.h) followed by a space
	char VMajor[5];			// 4 digits followed by a space
	char VMinor[5];			// 4 digits followed by a space

} VERSION;

static VERSION rev = { kTCAT_DICE_VERSION_MAGIC_STRING, 
	kTCAT_DICE_VERSION_MAJOR_STRING, 
	kTCAT_DICE_VERSION_MINOR_STRING,
	"", 
	"", 
	"0000",
	"0000"
};

char * diceApp_vendor_id(void)
{
	return rev.VendorID;
}

int diceApp_vendor_major_rev(void)
{
	return atoi(rev.VMajor);
}

int diceApp_vendor_minor_rev(void)
{
	return atoi(rev.VMinor);
}

#else //Suggested version info structure

typedef struct
{
	char MagicNum[36];		// initialized below, do not change this number
	uint32	uiBaseSDKVersion;				//The full version/revision of the SDK this build was based on
	uint32	uiApplicationVersion;		//The full version/revision of the Application
	uint32	uiVendorID;							//The Vendor ID
	uint32	uiProductID;						//The product ID
	char		BuildTime[64];					//Build time
	char		BuildDate[64];					//Build date
} VERSION;

static VERSION rev = { 
	kTCAT_DICE_VERSION_NEW_MAGIC_STRING, 
	kTCAT_DICE_VERSION32,						//The full version/revision of the SDK this build was based on
	APP_VER32,											//The full version/revision of the Application
	THIS_VENDOR_ID,									//The Vendor ID
	THIS_PRODUCT_ID,								//The product ID
	__TIME__,												//Build time
	__DATE__												//Build date
};

//These functions are maintained for backward compatibility
//

static char vendorIdCache[7];
//Vendor ID as string
char * diceApp_vendor_id(void)
{
	sprintf(vendorIdCache,"%06X",rev.uiVendorID&0xFFFFFF);
	return vendorIdCache;
}

uint32 diceApp_vendor_major_rev(void)
{
	return APP_MAJ;
}

uint32 diceApp_vendor_minor_rev(void)
{
	return APP_MIN;
}

// New functions to retrieve the full 32 bit vendor application version

uint32 diceApp_vendor_sub_rev(void)
{
	return APP_SUB;
}

uint32 diceApp_vendor_build_rev(void)
{
	return APP_BUILD;
}

uint32 diceApp_vendor_full_rev(void)
{
	return APP_VER32;
}

#endif



char * diceApp_build_time(void) 
{
	return _diceApp_build_time;
}

char * diceApp_date(void) 
{
	return _diceApp_date;
}

char * diceApp_time(void) 
{
	return _diceApp_time;
}


int diceApp_product_id(void)
{
	return THIS_PRODUCT_ID;
}

uint32 diceApp_vendor_id_num(void)
{
	return THIS_VENDOR_ID;
}

uint32	diceApp_major_rev(void) 
{
	return kTCAT_DICE_VERSION_MAJOR;
}

uint32	diceApp_minor_rev(void)
{
	return kTCAT_DICE_VERSION_MINOR;
}

uint32	diceApp_sub_rev(void) 
{
	return kTCAT_DICE_VERSION_SUB;
}

uint32	diceApp_build_rev(void) 
{
	return kTCAT_DICE_VERSION_BUILD;
}

uint32	diceApp_full_rev(void) 
{
	return kTCAT_DICE_VERSION32;
}

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

#ifndef _VERSION_H_
#define _VERSION_H_

#include "TCTypes.h"

char*	diceApp_build_time(void);
char*	diceApp_date(void);
char*	diceApp_time(void);

//These functions return the SDK version this app was build on
uint32	diceApp_major_rev(void);
uint32	diceApp_minor_rev(void);
uint32	diceApp_sub_rev(void);
uint32	diceApp_build_rev(void);
uint32	diceApp_full_rev(void);

//These functions return the Vendors App. Version
uint32 diceApp_vendor_major_rev(void);
uint32 diceApp_vendor_minor_rev(void);
#ifndef _USE_OLD_VERSION_INFO
uint32 diceApp_vendor_sub_rev(void);
uint32 diceApp_vendor_build_rev(void);
uint32 diceApp_vendor_full_rev(void);
#endif

//These functions return Vendor and Product ID for this application
char * diceApp_vendor_id(void);
uint32 diceApp_vendor_id_num(void); 
int diceApp_product_id(void);

#endif  //_VERSION_H_

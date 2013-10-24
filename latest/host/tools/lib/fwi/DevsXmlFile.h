///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009-2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "fwi.h"


///////////////////////////////////////////////////////////////////////////////
// look for support information for device using online xml file
//
// Parameters
//   devInfo					must have vendorId and DeviceID fields set for 
//								a possible match
// Returns
//  FWI_ERROR_FILE_NOT_FOUND	could not find the file online, or the computer
//                              is not connected to the Internet
//  FWI_ERROR_BAD_FILE_FORMAT   the xml file is not correctly formatted
//  FWI_NO_ERROR                success, look in devInfo to see results
//
long findControllerInfoXML(fwi::OHCI1394_DEV_INFO * devInfo);

void devsXMLInitialize(void);
void devsXMLUninitialize(void);

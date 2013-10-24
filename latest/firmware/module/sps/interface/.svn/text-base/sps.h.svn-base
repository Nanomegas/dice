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
	
	Purpose:	tcatNVR.h
  
	Description:       
		Small Persistent System utility
	Revisions:
		created 02/19/2007 zluo

****************************************************************************/

#ifndef _SPS_H_
#define _SPS_H_

#define SPS_BLOCK_MAGIC				0xBABEFACE
#define SPS_SET_MAGIC			0xBABEF001

#define SPS_ERROR_NO_FLASH			0xFE000001
#define SPS_ERROR_NOT_FORMAT		0xFE000002
#define SPS_ERROR_ERASE_ERR			0xFE000003
#define SPS_ERROR_PROGRAM_ERR		0xFE000004
#define SPS_ERROR_BAD_CRC			0xFE000005
#define SPS_ERROR_BAD_BLOCK_MAGIC	0xFE000006
#define SPS_ERROR_BAD_SET_MAGIC		0xFE000007
#define SPS_ERROR_BAD_FLASH_BASE	0xFE000008
#define SPS_EVENT_SWITCH_BLOCK		0xFE000009

//Search through flash blocks, verify check sum, and read current setting to pCurrent  Setting.
//If the nbData/nbSector is different, which means the structure of NVR has changes, it needs to
//be re-initialized. 
HRESULT spsInitialize (char * name, uint8 nbSectors, uint32 nbData, void * pCurrentSetting, uint16 updateTicks);

//This is used by application to notific tcatNVR that flash write back is needed.
void spsChanged (void);

//Returns any error that may happened during flash write update.
HRESULT spsGetError (void);

//Format the sps block and program the block to current memory copy.
void spsReset(void);

#endif  //_SPS_H_

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
	
	Purpose:	DICE router register definition
	
	Revisions:
		created 12/06/2003 kbj

****************************************************************************/

#ifndef _DICEROUTERREG_H
#define _DICEROUTERREG_H

//*******************************************************************************
// Dice router limit values
#define MAX_ROUTE_ENTRIES		256
#define MAX_DEVICES				16 
#define MAX_CHANNELS			16 // Max channels per. device
#define NUM_ROUTERS				2  // Number of routers	

//*******************************************************************************
// Dice router setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned on			: 1;	// On/Off
		unsigned error		: 1;	// Error status bit
		unsigned			: 6;	// Not used
		unsigned numEntry	: 8;	// Number of route entries - 1
		unsigned			: 16;	// Not used
	} bit;
#else
	struct
	{
		unsigned			: 16;	// Not used
		unsigned numEntry	: 8;	// Number of route entries - 1
		unsigned			: 6;	// Not used
		unsigned error		: 1;	// Error status bit
		unsigned on			: 1;	// On/Off
	} bit;
#endif

} uROUTER_SETUP;

#define ROUTER_ERROR				1
#define ROUTER_ON					1
#define ROUTER_OFF					0

//*******************************************************************************
// Router table entry
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned dstCh		: 4;	// Destination channel
		unsigned dstBlkID	: 4;	// Destination block ID
		unsigned srcCh		: 4;	// Source channel
		unsigned srcBlkID	: 4;	// Source Block ID 
		unsigned			: 16;	// Not used
	} bit;
#else
	struct
	{
		unsigned			: 16;	// Not used
		unsigned srcBlkID	: 4;	// Source Block ID 
		unsigned srcCh		: 4;	// Source channel
		unsigned dstBlkID	: 4;	// Destination block ID
		unsigned dstCh		: 4;	// Destination channel
	} bit;
#endif

} uROUTER_ENTRY;

// Receiver Source Block ID's
#define AESRX_SRC_ID				0
#define ADATRX_SRC_ID				1
#define TDIFRX_SRC_ID				2
#define DSAIRX0_SRC_ID				3
#define DSAIRX1_SRC_ID				4
#define DSAIRX2_SRC_ID				5
#define DSAIRX3_SRC_ID				6
#define I2SRX0_SRC_ID				7
#define I2SRX1_SRC_ID				8
#define I2SRX2_SRC_ID				9
#define APBRX_SRC_ID				10
#define AVSRX0_SRC_ID				11
#define AVSRX1_SRC_ID				12
#define AVSRX2_SRC_ID				13
#define AVSRX3_SRC_ID				14
#define MUTEDRX_SRC_ID				15

// Transmitter Destinations Block ID's
#define AESTX_DST_ID				0
#define ADATTX_DST_ID				1
#define TDIFTX_DST_ID				2
#define DSAITX0_DST_ID				3
#define DSAITX1_DST_ID				4
#define DSAITX2_DST_ID				5
#define DSAITX3_DST_ID				6
#define I2STX0_DST_ID				7
#define I2STX1_DST_ID				8
#define I2STX2_DST_ID				9
#define APBTX_DST_ID				10
#define AVSTX0_DST_ID				11
#define AVSTX1_DST_ID				12
#define PWM_DST_ID					15

#endif

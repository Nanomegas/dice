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
#define MAX_ROUTE_ENTRIES		128
#define MAX_DEVICES				16 
#define MAX_CHANNELS			16 // Max channels per. device
#define NUM_ROUTERS				1  // Number of routers	

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
		unsigned peakon		: 1;	// Peak detector on
		unsigned			: 5;	// Not used
		unsigned numEntry	: 8;	// Number of route entries - 1
		unsigned			: 16;	// Not used
	} bit;
#else
	struct
	{
		unsigned			: 16;	// Not used
		unsigned numEntry	: 8;	// Number of route entries - 1
		unsigned			: 5;	// Not used
		unsigned peakon		: 1;	// Peak detector on
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
		unsigned peak		: 12;
		unsigned			: 4;	// Not used
	} bit;
#else
	struct
	{
		unsigned			: 4;	// Not used
		unsigned peak		: 12;
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
#define MIXER_SRC_ID				2
#define INSRX0_SRC_ID				4
#define INSRX1_SRC_ID				5
#define APBRX_SRC_ID				10
#define AVSRX0_SRC_ID				11
#define AVSRX1_SRC_ID				12
#define MUTEDRX_SRC_ID				15

// Transmitter Destinations Block ID's
#define AESTX_DST_ID				0
#define ADATTX_DST_ID				1
#define MIXERTX0_DST_ID				2
#define MIXERTX1_DST_ID				3
#define INSTX0_DST_ID				4
#define INSTX1_DST_ID				5
#define APBTX_DST_ID				10
#define AVSTX0_DST_ID				11
#define AVSTX1_DST_ID				12

#endif

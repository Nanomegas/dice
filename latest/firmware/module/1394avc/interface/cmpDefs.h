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
 * DESCRIPTION: Connection Management Procedures (CMP) Defines

	This module implements the connection management procedures defined in
	IEC 61883-1.

	The following registers are implemented:
		iPCR
		oPCR
		iMPR
		oMPR

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

#ifndef _CMPDEFS_H
#define _CMPDEFS_H

#include "1394.h"
#include "TCTypes.h"
#include "bitstring32.h"

#define CMP_POST_BUS_RESET_WAITTIME		1000				// wait 1 second af bus reset

/* Connection states */

typedef enum
{
	CONNECTION_STATUS_UNCONNECTED = 0, 						// connection is no longer used
	CONNECTION_STATUS_CONNECTED,							// connected and we own the connection
	CONNECTION_STATUS_PENDING								// status pending - goes into this state during bus resets while we remake
} CONNECTION_STATUS;

/* Plug states */

typedef enum
{
	PLUG_READY,
	PLUG_ACTIVE,
	PLUG_SUSPENDED,
	PLUG_IDLE
} CMP_PCR_STATE;

typedef enum
{
	OWNER_LOCAL,
	OWNER_REMOTE
} CMP_PCR_OWNER;

// PCR_REGISTER - for input and output plug control registers
typedef struct
{
	uint32			plugMutexSemID;
	CMP_PCR_OWNER	plugOwner;
	uint32			plugRegister;
	uint32			plugFormat;
	BOOL			bIn;
	uint32			plugID;
	BOOL			bMarked;
} PCR_REGISTER;

typedef enum
{
	CMP_RESET_MARK,
	CMP_RESET_STOP
} CMP_PCR_RESET;

/* 

 oMPR:

  msb

  2 :data rate capability
  6 :broadcast channel base
  8 :non persistent extension field [future use]
  8 :persistent extension field [future use]
  3 :reserved
  5 :number of output plugs

  lsb

 iMPR :

  msb

  2 :data rate capability
  6 :reserved
  8 :non persistent extension field [future use]
  8 :persistent extension field [future use]
  3 :reserved
  5 :number of input plugs

  lsb

 oPCR:

  msb

  1 :online
  1 :broadcast connection counter
  6 :point to point connection counter
  2 :reserved
  6 :channel number
  2 :data rate
  4 :overhead ID
  10:payload

  lsb

 iPCR:

  msb

  1 :online
  1 :broadcast connection counter
  6 :point to point connection counter
  2 :reserved
  6 :channel number
  2 :data rate
  4 :overhead ID
  10:payload

  lsb


 PCR register offset addresses:

   F000 0900  oMPR
   F000 0904  oPCR[0]
   F000   :		:
   F000 097C  oPCR[30]

   F000 0980  iMPR
   F000 0984  iPCR[0]
          :		:
   F000 09FC  iPCR[30]

*/

// write masks for incoming lock transactions
// these are defined in IEC 61883, figures 10 through 13

					   			// oMPR		  |rcp| b-ch base || n-pers ext f. || pers ext f.   || res | #outplg |
					   			// format	  | 2 |     6     ||       8       ||       8       ||  3  |    5    |
#define oMPR_RESET_MASK			BITSTRING32	(  1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 0,0,0,1,1,1,1,1)
#define oMPR_WRITE_MASK			BITSTRING32	(  0,0,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0)
								
					   			// iMPR		  |rcp| reserved  || n-pers ext f. || pers ext f.   || res | #inplg  |
					   			// format	  | 2 |     6     ||       8       ||       8       ||  3  |    5    |
#define iMPR_RESET_MASK			BITSTRING32	(  1,1,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 0,0,0,1,1,1,1,1)
#define iMPR_WRITE_MASK			BITSTRING32	(  0,0,0,0,0,0,0,0, 1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1, 0,0,0,0,0,0,0,0)
								
					   			// oPCR		  |o|b| p-p con # ||res| ch number ||drt|over.id|       payload      |
					   			// format	  |1|1|     6     || 2 |     6     || 2 |    4  |          10        |
#define oPCR_RESET_MASK			BITSTRING32	(  1,0,0,0,0,0,0,0, 0,0,1,1,1,1,1,1, 1,1,0,0,0,0,1,1, 1,1,1,1,1,1,1,1)
#define oPCR_WRITE_MASK			BITSTRING32	(  0,1,1,1,1,1,1,1, 0,0,1,1,1,1,1,1, 1,1,1,1,1,1,0,0, 0,0,0,0,0,0,0,0)
								
					   			// iPCR		  |o|b| p-p con # ||res| ch number ||            reserved            |
					   			// format	  |1|1|     6     || 2 |     6     ||               16               |
#define iPCR_RESET_MASK			BITSTRING32	(  1,0,0,0,0,0,0,0, 0,0,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0)
#define iPCR_WRITE_MASK			BITSTRING32	(  0,1,1,1,1,1,1,1, 0,0,1,1,1,1,1,1, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0)
	
#define oMPR_MASK_RATECAP		0xC0000000
#define oMPR_MASK_BCBASE		0x3F000000
#define oMPR_MASK_NP_EXT		0x00FF0000
#define oMPR_MASK_P_EXT			0x0000FF00
#define oMPR_MASK_RESERVED		0x000000E0
#define oMPR_MASK_OUTPLUGS		0x0000001F

#define iMPR_MASK_RATECAP		0xC0000000
#define iMPR_MASK_RESERVED1		0x3F000000
#define iMPR_MASK_NP_EXT		0x00FF0000
#define iMPR_MASK_P_EXT			0x0000FF00
#define iMPR_MASK_RESERVED2		0x000000E0
#define iMPR_MASK_INPLUGS		0x0000001F

#define oMPR_SHIFT_RATECAP		30
#define oMPR_SHIFT_BCBASE		24
#define oMPR_SHIFT_NP_EXT		16
#define oMPR_SHIFT_P_EXT		8
#define oMPR_SHIFT_OUTPLUGS		0

#define oPCR_MASK_ONLINE		0x80000000
#define oPCR_MASK_BCCOUNT		0x40000000
#define oPCR_MASK_P2PCOUNT		0x3F000000
#define oPCR_MASK_RES1			0x00C00000
#define oPCR_MASK_CHANNEL		0x003F0000
#define oPCR_MASK_RATE			0x0000C000
#define oPCR_MASK_OVERHEAD		0x00003C00
#define oPCR_MASK_PAYLOAD		0x000003FF

#define iPCR_MASK_ONLINE		oPCR_MASK_ONLINE
#define iPCR_MASK_BCCOUNT		oPCR_MASK_BCCOUNT
#define iPCR_MASK_P2PCOUNT		oPCR_MASK_P2PCOUNT
#define iPCR_MASK_RES1			oPCR_MASK_RES1
#define iPCR_MASK_CHANNEL		oPCR_MASK_CHANNEL
#define iPCR_MASK_RES2			0x0000FFFF

#define oPCR_SHIFT_ONLINE		31
#define oPCR_SHIFT_BCCOUNT		30
#define oPCR_SHIFT_P2PCOUNT		24
#define oPCR_SHIFT_CHANNEL		16
#define oPCR_SHIFT_RATE			14
#define oPCR_SHIFT_OVERHEAD		10
#define oPCR_SHIFT_PAYLOAD		0

#define PCR_BASE_OFFSET			0xF0000900
#define oMPR_OFFSET				0xF0000900
#define oPCR_RANGE_LOW			0xF0000904
#define oPCR_RANGE_HI			0xF000097C
#define iMPR_OFFSET				0xF0000980
#define iPCR_RANGE_LOW			0xF0000984
#define iPCR_RANGE_HI			0xF00009FC

#define iPCR_MAX_ITEMS			31			// iPCR[0],..,iPCR[30]
#define oPCR_MAX_ITEMS			31			// oPCR[0],..,oPCR[30]
								
// defines for handling cmp registers 
#define CMP_COMMAND_UNKNOWN		0x00000000
#define CMP_COMMAND_MASK		0xF0000000
#define CMP_COMMAND_LOCK		0x10000000
#define CMP_COMMAND_WRITE		0x20000000
#define CMP_COMMAND_READ		0x40000000
#define CMP_COMMAND_RESET		0x80000000

#define CMP_REGISTER_UNKNOWN	0x00000000
#define CMP_REGISTER_MASK		0x0F000000
#define CMP_REGISTER_OMPR		0x01000000
#define CMP_REGISTER_IMPR		0x02000000
#define CMP_REGISTER_OPCR		0x04000000
#define CMP_REGISTER_IPCR		0x08000000

#define CMP_TYPE_UNKNOWN		0x00000000
#define CMP_TYPE_MASK			0x000F0000
#define CMP_TYPE_REGISTER		0x00010000
#define CMP_TYPE_FORMAT			0x00020000
#define CMP_TYPE_STATE			0x00040000
#define CMP_TYPE_ALL			0x00070000

#define CMP_OMPR_REGISTER_READ	(CMP_COMMAND_READ	| CMP_REGISTER_OMPR | CMP_TYPE_REGISTER)
#define CMP_OMPR_REGISTER_LOCK	(CMP_COMMAND_LOCK	| CMP_REGISTER_OMPR | CMP_TYPE_REGISTER)
#define CMP_OMPR_REGISTER_RESET	(CMP_COMMAND_RESET	| CMP_REGISTER_OMPR | CMP_TYPE_REGISTER)
#define CMP_IMPR_REGISTER_READ	(CMP_COMMAND_READ	| CMP_REGISTER_IMPR | CMP_TYPE_REGISTER)
#define CMP_IMPR_REGISTER_LOCK	(CMP_COMMAND_LOCK	| CMP_REGISTER_IMPR | CMP_TYPE_REGISTER)
#define CMP_IMPR_REGISTER_RESET	(CMP_COMMAND_RESET	| CMP_REGISTER_IMPR | CMP_TYPE_REGISTER)

#define CMP_OPCR_REGISTER_READ	(CMP_COMMAND_READ	| CMP_REGISTER_OPCR | CMP_TYPE_REGISTER)
#define CMP_OPCR_REGISTER_LOCK	(CMP_COMMAND_LOCK	| CMP_REGISTER_OPCR | CMP_TYPE_REGISTER)
#define CMP_IPCR_REGISTER_READ	(CMP_COMMAND_READ	| CMP_REGISTER_IPCR | CMP_TYPE_REGISTER)
#define CMP_IPCR_REGISTER_LOCK	(CMP_COMMAND_LOCK	| CMP_REGISTER_IPCR | CMP_TYPE_REGISTER)

#define CMP_OPCR_FORMAT_READ	(CMP_COMMAND_READ	| CMP_REGISTER_OPCR | CMP_TYPE_FORMAT)
#define CMP_OPCR_FORMAT_WRITE	(CMP_COMMAND_WRITE	| CMP_REGISTER_OPCR | CMP_TYPE_FORMAT)
#define CMP_IPCR_FORMAT_READ	(CMP_COMMAND_READ	| CMP_REGISTER_IPCR | CMP_TYPE_FORMAT)
#define CMP_IPCR_FORMAT_WRITE	(CMP_COMMAND_WRITE	| CMP_REGISTER_IPCR | CMP_TYPE_FORMAT)

#define CMP_OPCR_STATE_READ		(CMP_COMMAND_READ	| CMP_REGISTER_OPCR | CMP_TYPE_STATE)
#define CMP_IPCR_STATE_READ		(CMP_COMMAND_READ	| CMP_REGISTER_IPCR | CMP_TYPE_STATE)

#define CMP_OPCR_ALL_RESET		(CMP_COMMAND_RESET	| CMP_REGISTER_OPCR | CMP_TYPE_ALL)
#define CMP_IPCR_ALL_RESET		(CMP_COMMAND_RESET	| CMP_REGISTER_IPCR | CMP_TYPE_ALL)

#endif //_CMPDEFS_H


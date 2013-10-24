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
//===========================================================================
// $$
//===========================================================================
/****************************************************************************
	
	Purpose:	DICE Abstraction Layer router API
	
	Revisions:
		created 24/02/2005 ml

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCDebug.h"
#include "diceRouterReg.h"
#include "diceDefs.h"
#include "dalRouter.h"
#include "mixfix.h"

static char 	moduleName[] = __FILE__ ;


#define DAL_MAX_DST_ENTRIES	114	// max number of destination channels

// Private datastructure to hold configuration
typedef struct tDAL_ROUTER_DESC
{
	//Adress of physical hw registers
	uROUTER_SETUP *		pRouterSetup;
	uROUTER_ENTRY *     pRouterentries;
	
	//Private data holding the logical configuration
	DAL_RATE_MODE_ENUM	rateMode;
	uint32				inputDevices;
	uint32				outputDevices;
	uint8				nbEntries;
	DAL_ROUTING_ENTRY	entries[DAL_MAX_DST_ENTRIES];
	uint8				entryOfs[DAL_MAX_DST_ENTRIES];
} DAL_ROUTER_DESC;
	

static DAL_ROUTER_DESC dalRouterDesc[1];


static void updateRouterEntry (DAL_ROUTER_DESC * pRouter, uint8 entry)
{
	uint8 nbch, dstch, srcch, dstmod, rtrentry;
	uROUTER_ENTRY  hwEntry;
	
	
	nbch = 1 << (pRouter->entries[entry].dest_description.bit_fields.type);
    dstch = pRouter->entries[entry].dest_description.bit_fields.channel;
    srcch = pRouter->entries[entry].src_description.bit_fields.channel;
    rtrentry = pRouter->entryOfs[entry];
    dstmod = pRouter->entries[entry].dest_description.bit_fields.moduleId;
    while (nbch--)
    {
		hwEntry.bit.srcBlkID = pRouter->entries[entry].src_description.bit_fields.moduleId;
		hwEntry.bit.srcCh = srcch++;
		hwEntry.bit.dstBlkID = pRouter->entries[entry].dest_description.bit_fields.moduleId;
		hwEntry.bit.dstCh = dstch++;
		pRouter->pRouterentries[rtrentry++] = hwEntry;
	}
}




HRESULT dalRouterCreateConfiguration (DAL_INTERFACE_ID_ENUM routerId, DAL_RATE_MODE_ENUM rateMode, uint32 inputDevices, uint32 outputDevices)
{
	DAL_ROUTER_DESC * pRouter;
	uint8 dst, cnt, bhi, nbch, rtrentry, ch;
	HRESULT hResult;
	uROUTER_SETUP hwsetup;
	
	if (routerId != eDAL_INTERFACE_1)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	pRouter = &dalRouterDesc[routerId];
	
	pRouter->rateMode = rateMode;
	pRouter->inputDevices = inputDevices | DEVICE_BIT(DEV_MUTED);
	pRouter->outputDevices = outputDevices;
	pRouter->nbEntries = 0;
	
#ifdef _MIXFIX_DAL
	if ((rateMode==eDAL_RATE_MODE_HIGH) && (inputDevices & DEVICE_BIT(DEV_MIXER_RX)))
	{
		outputDevices |= DEVICE_BIT(DEV_APB_TX); //force the APB into the system
		mixfixEnable (TRUE);
	}
	else
	{
		mixfixEnable (FALSE);
	}
#endif //_MIXFIX_DAL

	cnt = 0;
	bhi = (rateMode == eDAL_RATE_MODE_HIGH) ? 1 : 0;
	rtrentry = 0; //keeps track of the physical router entry
	
	for (dst = DEV_AES_TX; dst < NUM_DEVICES_TX	; dst++)
	{		
		if (DEVICE_BIT(dst) & outputDevices)
		{
			nbch = TX_DEV_CHANNELS[dst][bhi];
			for (ch = 0; ch < nbch; ch++)
			{
				pRouter->entries[cnt].dest_description.entry = DEV_DESCR(dst) | CH_DESCR(ch) | TYPE_DESCR(BLOCK_1_TYPE);

#ifdef _MIXFIX_DAL
				if ((rateMode==eDAL_RATE_MODE_HIGH) && (inputDevices & DEVICE_BIT(DEV_MIXER_RX)) && (dst == DEV_APB_TX))
				{
#ifdef _MIXFIX_PERFECT4  
					pRouter->entries[cnt].src_description.entry = DEV_DESCR(DEV_MIXER_RX) | CH_DESCR(((ch<4)?ch:(ch+4))) | TYPE_DESCR(BLOCK_1_TYPE);
#else //_MIXFIX_PERFECT4
					pRouter->entries[cnt].src_description.entry = DEV_DESCR(DEV_MIXER_RX) | CH_DESCR(ch) | TYPE_DESCR(BLOCK_1_TYPE);
#endif //_MIXFIX_PERFECT4
				}
				else
				{
					pRouter->entries[cnt].src_description.entry = DEV_DESCR(DEV_MUTED) | CH_DESCR(ch) | TYPE_DESCR(BLOCK_1_TYPE);
				}

#else  //_MIXFIX_DAL
				pRouter->entries[cnt].src_description.entry = DEV_DESCR(DEV_MUTED) | CH_DESCR(ch) | TYPE_DESCR(BLOCK_1_TYPE);
#endif //_MIXFIX_DAL

				pRouter->entryOfs[cnt++] = rtrentry;
				rtrentry += 1;
			}
		}
	}
	pRouter->nbEntries = cnt;
	
	// At this point we have initialized the internal router structure.
	// Make sure the router is stopped, we don't know what is in it.
	pRouter->pRouterSetup->bit.on = ROUTER_OFF;
	for (cnt = 0; cnt < pRouter->nbEntries; cnt++)
	{
		updateRouterEntry (pRouter, cnt);
	}
	//now all entries are in place and muted, update count and start router
	if (rtrentry)
	{
		hwsetup.bit.numEntry = rtrentry-1;
		hwsetup.bit.on = ROUTER_ON;
		hwsetup.bit.peakon = 1;
		pRouter->pRouterSetup->reg = hwsetup.reg;
	}
	return NO_ERROR;
}


HRESULT dalRouterAddRoute (DAL_INTERFACE_ID_ENUM routerId, SRC_DESCRIPTION src, DEST_DESCRIPTION dst)
{
	DAL_ROUTER_DESC * pRouter;
	DAL_ROUTING_ENTRY * pEntry;
	uint8 entry, nbch, channelsPlaced;
	HRESULT hResult = NO_ERROR;
	
	if (routerId != eDAL_INTERFACE_1)
	{
		hResult = E_DAL_INTERFACE_NOT_DEFINED;
		goto ErrorExit;
	}
	pRouter = &dalRouterDesc[routerId];
	
	//check basics
	//  block type must match src and dst
	//  src and dst must be member of router
	//  ch boundary must be aligned i.e. (ch & (blksize-1)) == 0
	
	if (src.bit_fields.type != dst.bit_fields.type)
	{
		hResult = E_DAL_INVALID_ROUTE;
		goto ErrorExit;
	}
	if ((!(DEVICE_BIT(src.bit_fields.moduleId) & pRouter->inputDevices)) || (!(DEVICE_BIT(dst.bit_fields.moduleId) & pRouter->outputDevices)))
	{
		hResult = E_DAL_INVALID_ROUTE;
		goto ErrorExit;
	}
   	nbch = 1 << dst.bit_fields.type;
   	if ((dst.bit_fields.channel & (nbch-1)) || (src.bit_fields.channel & (nbch-1)))
   	{
		hResult = E_DAL_INVALID_ROUTE;
		goto ErrorExit;
	}   	

#ifdef _MIXFIX_DAL
	if ((pRouter->rateMode==eDAL_RATE_MODE_HIGH) && (src.bit_fields.moduleId == DEV_MIXER_RX))
	{
		src.bit_fields.moduleId = DEV_APB_RX;
	}
#endif //_MIXFIX_DAL


	//check against dst
	//	find first dst match
	//  is specified dst block type >= config dst type
	//  if > is the match exact
	pEntry = pRouter->entries;
	for (entry = 0; entry < pRouter->nbEntries; entry++)
	{
		if (dst.bit_fields.moduleId == pEntry->dest_description.bit_fields.moduleId)
		{
			if (dst.bit_fields.channel == pEntry->dest_description.bit_fields.channel)
			{
				break;
			}
		}
		pEntry++;
	}
	if (entry >= pRouter->nbEntries)
	{
		hResult = E_DAL_INVALID_ROUTE;
		goto ErrorExit;
	}   	
	// We found an entry which starts on the same channel	
	if (dst.bit_fields.type < pEntry->dest_description.bit_fields.type)
	{
		hResult = E_DAL_INVALID_ROUTE;
		goto ErrorExit;
	}   	
	// Put in the first one
	pEntry->src_description.bit_fields.type = pEntry->dest_description.bit_fields.type;
	pEntry->src_description.bit_fields.moduleId = src.bit_fields.moduleId;
	pEntry->src_description.bit_fields.channel = src.bit_fields.channel;
	channelsPlaced = 1 << pEntry->dest_description.bit_fields.type;
	
	updateRouterEntry (pRouter, entry);
	
	if (dst.bit_fields.type > pEntry->dest_description.bit_fields.type)
	{
		//spans more than one basic destination
		while (channelsPlaced < nbch)
		{
			pEntry++;
			entry++;
			if (entry >= pRouter->nbEntries)
			{
				hResult = E_DAL_INVALID_ROUTE;
				goto ErrorExit;
			}
			if (dst.bit_fields.moduleId != pEntry->dest_description.bit_fields.moduleId)
			{
				hResult = E_DAL_INVALID_ROUTE;
				goto ErrorExit;
			}
			if (channelsPlaced + (1 << pEntry->dest_description.bit_fields.type) > nbch)
			{
				hResult = E_DAL_INVALID_ROUTE;
				goto ErrorExit;
			}
			//one more will fit
			pEntry->src_description.bit_fields.type = pEntry->dest_description.bit_fields.type;
			pEntry->src_description.bit_fields.moduleId = src.bit_fields.moduleId;
			pEntry->src_description.bit_fields.channel = src.bit_fields.channel+channelsPlaced;
			channelsPlaced += (1 << pEntry->dest_description.bit_fields.type);
			
			updateRouterEntry (pRouter, entry);
		}
	}
	return hResult;
	
ErrorExit:
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}
    
HRESULT dalRouterInitialize (void)
{
	dalRouterDesc[0].rateMode = eDAL_RATE_MODE_LOW_MID;
	dalRouterDesc[0].nbEntries = 0;
	dalRouterDesc[0].pRouterSetup = (uROUTER_SETUP *) mDicePtr(DICE_ROUTER1_SETUP);
	dalRouterDesc[0].pRouterentries = (uROUTER_ENTRY *) mDicePtr(DICE_ROUTER1_ENTRY);
	

	return NO_ERROR;
}
			
			


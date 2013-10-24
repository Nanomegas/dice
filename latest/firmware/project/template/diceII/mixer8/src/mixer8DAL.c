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
 * DESCRIPTION: mixer8

	Purpose:	mixer8
				

	Revisions:
		created 09/03/2005 AliA

****************************************************************************/

#include "TCTasking.h"
#include "TCInterrupt.h"
#include "ErrorCodes.h"
#include "mixer8.h"
#include "mixer8DAL.h"
#include "dal.h"

#ifdef _MIXER8

static const DAL_EVENTS mixer8DALSubscribedEvents = 
{
	//state machin events we want to subscribe to
	DAL_CB_LOCK1_AQUIRED | DAL_CB_LOCK1_LOST | DAL_CB_RATE1_NOM_CHANGED | DAL_CB_RATE1_CHANGED | DAL_CB_CREATED1,
	0,
	0,
	0,
	0,
	0
};

void mixer8DALInstallFIQHandler(void)
{
	DAL_STATUS		dalStatus;	
	int		mixer8DALStatus = 0;
	int		mixer8DALNominalRate = 0;
	int		mixer8Mode = 0;

	dalGetCurrentStatus(eDAL_INTERFACE_1, &dalStatus);	
	mixer8DALStatus = dalStatus.state;
	mixer8DALNominalRate = dalStatus.lockedNominalRate;
	mixer8Mode  = (mixer8.CurrentConfiguration & MIXER_CONFIG_MASK);

	if((mixer8DALStatus == eDAL_STATE_LOCKED) && (mixer8.Enable == 1))
	{
		switch(mixer8Mode)
		{
		case MIXER_8BY8:
			if(mixer8DALNominalRate < eDAL_NOMINAL_RATE_88_2)
			{
				TCInterruptCreateFIQ(&mixer_8by8);
				mixer8_Enable(true);
			}
			break;
		case MIXER_8BY4:
			if(mixer8DALNominalRate < eDAL_NOMINAL_RATE_176_4)
			{
				TCInterruptCreateFIQ(&mixer_8by4);
				mixer8_Enable(true);
			}
			break;
		case MIXER_4BY4:
			if(mixer8DALNominalRate < eDAL_NOMINAL_RATE_176_4)
			{
				TCInterruptCreateFIQ(&mixer_4by4);
				mixer8_Enable(true);
			}
			else if(mixer8DALNominalRate < eDAL_NOMINAL_RATE_USER_1)
			{
				TCInterruptCreateFIQ(&mixer_4by4_high_rate_2);
				mixer8_Enable(true);
			}
			break;
		default:
			{
				TCInterruptDestroyFIQ();
				mixer8_Enable(false);
			}
			break;
		}
	}
	else
	{
		TCInterruptDestroyFIQ();
		mixer8_Enable(false);
	}	
}

static HRESULT mixer8DALCallBack (DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data)
{
	mixer8DALInstallFIQHandler();
	return NO_ERROR;
}

HRESULT mixer8DALInitialize(void)
{
	HRESULT hResult;

	hResult = dalInstallCallback(&mixer8DALSubscribedEvents, 200, mixer8DALCallBack, 0);
	
	return hResult;
}

#endif//_MIXER8

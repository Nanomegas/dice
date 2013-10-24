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
 * DESCRIPTION: EAP

	Purpose:	Enhanced Application Protocol, Persistent Storage handling
				

	Revisions:
		created 20/02/2007 ml

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "TCDebug.h"
#include "eapInternals.h"
#include "eapPersist.h"

static char 	moduleName[] = __FILE__ ;



// Serialize functions for persistent backup/restore

typedef struct
{
	uint16					routeEntries;
	uint16					route[MAX_ROUTE_ENTRIES];
} EAP_PACKED_ROUTING;

uint16 eapGetPersistSize (void)
{
	uint16 nbPersistData;
	
	if (eapIsFlashSupported ())
	{
		nbPersistData = (eapIsMixerStored () ? sizeof(uint16)*MAX_MIX_COEFF : 0) +
						(eapIsRouterStored() ? 3*sizeof(EAP_PACKED_ROUTING) : 0) +
						(eapIsStrmCfgStored() ? 3*sizeof(EAP_NEW_STRM_CFG) : 0) +
						sizeof(EAP_STND_ALONE_CFG) +
						devInfo.nbPrivatePersistData;
	}
	else
	{
		nbPersistData = 0;
	}
	return nbPersistData;
}


static void serializeMixer(void **pStream)
{
	uint16 * pDst = (uint16 *)*pStream;
	uint32 * pCurr = (uint32 *)mDicePtr(DICE_MIXER_BASE+0x800);
	uint16   i;
	
	for (i=0; i<MAX_MIX_COEFF; i++)
	{
		*pDst++ = *pCurr++;
	}
	*pStream = pDst;
}

static void deserializeMixer(void **pStream)
{
	uint16 * pSrc = (uint16 *)*pStream;
	uint32 * pDst = (uint32 *)mDicePtr(DICE_MIXER_BASE+0x800);
	uint16   i;
	
	for (i=0; i<MAX_MIX_COEFF; i++)
	{
		*pDst++ = *pSrc++;
	}
	*pStream = pSrc;
}

static void serializeRouter(void **pStream, const EAP_NEW_ROUTING * pRouting)
{
	EAP_PACKED_ROUTING * pDst = (EAP_PACKED_ROUTING *) *pStream;
	uint16   i;
	
	pDst->routeEntries = (uint16)pRouting->routeEntries;
	for (i=0; i<MAX_ROUTE_ENTRIES; i++)
	{
		pDst->route[i] = (uint16)pRouting->route[i];
	}
	pDst++;
	*pStream = pDst;
}

static void deserializeRouter(void **pStream, EAP_NEW_ROUTING * pRouting)
{
	EAP_PACKED_ROUTING * pSrc = (EAP_PACKED_ROUTING *) *pStream;
	uint16   i;
	
	pRouting->routeEntries = (uint32)pSrc->routeEntries; 
	for (i=0; i<MAX_ROUTE_ENTRIES; i++)
	{
		pRouting->route[i] = (uint32)pSrc->route[i];
	}
	pSrc++;
	*pStream = pSrc;
}

static void serializeStrmCfg(void **pStream, const EAP_NEW_STRM_CFG * pStrmCfg)
{
	EAP_NEW_STRM_CFG * pDst = (EAP_NEW_STRM_CFG *) *pStream;
	
	*pDst = *pStrmCfg;
	pDst++;
	*pStream = pDst;
}

static void deserializeStrmCfg(void **pStream, EAP_NEW_STRM_CFG * pStrmCfg)
{
	EAP_NEW_STRM_CFG * pSrc = (EAP_NEW_STRM_CFG *) *pStream;
	
	*pStrmCfg = *pSrc;
	pSrc++;
	*pStream = pSrc;
}

static void serializeStndAlone(void **pStream, const EAP_STND_ALONE_CFG * pStndAlone)
{
	EAP_STND_ALONE_CFG * pDst = (EAP_STND_ALONE_CFG *) *pStream;
	
	*pDst = *pStndAlone;
	pDst++;
	*pStream = pDst;
}

static void deserializeStndAlone(void **pStream, EAP_STND_ALONE_CFG * pStndAlone)
{
	EAP_STND_ALONE_CFG * pSrc = (EAP_STND_ALONE_CFG *) *pStream;
	
	*pStndAlone = *pSrc;
	pSrc++;
	*pStream = pSrc;
}


HRESULT eapCopyPrivateToPersist (void)
{
	HRESULT hResult = NO_ERROR;
	uint8 * pDst;
	
	if (!devInfo.pPersistData)
	{
		sysLogError(E_NULL_PTR, __LINE__, moduleName);
		return E_NULL_PTR;
	}
	if (!devInfo.nbPrivatePersistData) return hResult;
	
	if (!devInfo.pPrivatePersistData)
	{
		sysLogError(E_NULL_PTR, __LINE__, moduleName);
		return E_NULL_PTR;
	}
	pDst = (uint8 *)devInfo.pPersistData + devInfo.nbPersistData - devInfo.nbPrivatePersistData;
	memcpy (pDst, devInfo.pPrivatePersistData, devInfo.nbPrivatePersistData);
	
	return hResult;
}

HRESULT eapCopyPersistToPrivate (void)
{
	HRESULT hResult = NO_ERROR;
	uint8 * pSrc;
	
	if (!devInfo.pPersistData)
	{
		sysLogError(E_NULL_PTR, __LINE__, moduleName);
		return E_NULL_PTR;
	}
	if (!devInfo.nbPrivatePersistData) return hResult;
	
	if (!devInfo.pPrivatePersistData)
	{
		sysLogError(E_NULL_PTR, __LINE__, moduleName);
		return E_NULL_PTR;
	}
	pSrc = (uint8 *)devInfo.pPersistData + devInfo.nbPersistData - devInfo.nbPrivatePersistData;
	memcpy (devInfo.pPrivatePersistData, pSrc, devInfo.nbPrivatePersistData);
	
	return hResult;
}
	
	

HRESULT eapCopyCurrToPersist (void)
{
	HRESULT hResult = NO_ERROR;
	void * pStream;
	
	if (!devInfo.pPersistData)
	{
		sysLogError(E_NULL_PTR, __LINE__, moduleName);
		return E_NULL_PTR;
	}
	
	pStream = (void *) devInfo.pPersistData;
	if (eapIsMixerStored ())
	{
		serializeMixer(&pStream);
	}
	if (eapIsRouterStored())
	{
		serializeRouter(&pStream, &eapCurrCfg.lowRouter);
		serializeRouter(&pStream, &eapCurrCfg.midRouter);
		serializeRouter(&pStream, &eapCurrCfg.highRouter);
	}
	if (eapIsStrmCfgStored())
	{
		serializeStrmCfg(&pStream, &eapCurrCfg.lowStrmCfg);
		serializeStrmCfg(&pStream, &eapCurrCfg.midStrmCfg);
		serializeStrmCfg(&pStream, &eapCurrCfg.highStrmCfg);
	}
	serializeStndAlone(&pStream, &eapStndAloneCfg);
	return hResult;	
}

HRESULT eapCopyMixerToPersist (void)
{
	HRESULT hResult = NO_ERROR;
	void * pStream;
	
	if (!devInfo.pPersistData)
	{
		sysLogError(E_NULL_PTR, __LINE__, moduleName);
		return E_NULL_PTR;
	}
	
	pStream = (void *) devInfo.pPersistData;
	if (eapIsMixerStored ())
	{
		serializeMixer(&pStream);
	}
	return hResult;	
}



HRESULT eapCopyPersistToCurr (void)
{
	HRESULT hResult = NO_ERROR;
	void * pStream;
	
	if (!devInfo.pPersistData)
	{
		sysLogError(E_NULL_PTR, __LINE__, moduleName);
		return E_NULL_PTR;
	}
	
	pStream = (void *) devInfo.pPersistData;
	if (eapIsMixerStored ())
	{
		deserializeMixer(&pStream);
	}
	if (eapIsRouterStored())
	{
		deserializeRouter(&pStream, &eapCurrCfg.lowRouter);
		deserializeRouter(&pStream, &eapCurrCfg.midRouter);
		deserializeRouter(&pStream, &eapCurrCfg.highRouter);
	}
	if (eapIsStrmCfgStored())
	{
		deserializeStrmCfg(&pStream, &eapCurrCfg.lowStrmCfg);
		deserializeStrmCfg(&pStream, &eapCurrCfg.midStrmCfg);
		deserializeStrmCfg(&pStream, &eapCurrCfg.highStrmCfg);
	}
	deserializeStndAlone(&pStream, &eapStndAloneCfg);
	return hResult;	
}


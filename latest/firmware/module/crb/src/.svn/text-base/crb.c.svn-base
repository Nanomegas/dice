//===========================================================================
//
// This confidential and proprietary software may be used only as authorized
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2008 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	Config ROM Builder
  
	Description:       
  
	Revisions:
		created 05/10/2008 brian

****************************************************************************/
#include <stdlib.h>	// malloc, free
#include <string.h> // memset
#ifdef _CLI
#include "cli.h"
#endif // _CLI
#include "TCDebug.h"
#include "lal.h"
#include "csr.h"
#include "crb.h"
#include "alloc_fifo.h"
#include "crc_fifo.h"
#include "traverse_fifo.h"
#include "unit_fifo.h"
#include "crc16.h"
#include "crbCli.h"
#include "targetConfigCSR.h"
#include "targetVendorDefs.h"
#include "sysDebug.h"
#include "tcat_dice_myproduct.h"

static char 		moduleName[] = __FILE__;


// locate the static root directory singleton in this struct
//   bus info is built depending on node caps
//   root contents are added at runtime
struct
{
	BOOL		bMinBusInfo;			//generate min bus info
	uint32		busCapabilities;	//2nd quad of bus info
	uint32		wwuIDHigh;				//3rd quad of bus info
	uint32		wwuIDLow;					//4th quad of bus info
	uint32		infoGeneration;   //generation, incremented by every build
	uint32		gNodeNum;
	CRBElement	rootDir;
} state;



// create and zero an element
static CRBHANDLE crbCreateElement(void)
{
	CRBHANDLE elem;
	elem = (CRBHANDLE) 
	malloc(sizeof(CRBElement));

	if (NULL != elem)
	{
		memset(elem, 0, sizeof(CRBElement));
		alloc_fifo.push((void *)elem);
	}
	return(elem);
}


// free all element and leaf-data allocations
static HRESULT crbFree(void)
{
	void * vHandle = NULL;

	vHandle = alloc_fifo.pop();
	while (NULL != vHandle)
	{
		free(vHandle);
		vHandle = alloc_fifo.pop();
	}

	return NO_ERROR;
}


CRBHANDLE crbGetRoot(void)
{
	return (&state.rootDir);
}


HRESULT crbInitialize(void)
{
	HRESULT hResult = NO_ERROR;
#ifdef _CLI_TOOL_CRB
	hResult = crbCliInstallTools();
#endif //_CLI_TOOL_CRB

	return hResult;
}

HRESULT crbNewConfiguration(BOOL bMinimal, uint32 wwuid_hi, uint32 wwuid_low)
{
	CRBHANDLE root = crbGetRoot();

	// clear any existing allocated entries
	crbFree();
	alloc_fifo.init();

	state.gNodeNum = 0;


	state.bMinBusInfo = bMinimal;
	state.wwuIDHigh = wwuid_hi;
	state.wwuIDLow = wwuid_low;
	state.busCapabilities = BUSINFO_LLC_SPECIFIC & ~BUSINFO_MAXREC; //LLC dependent caps
	//We need to adjust the MAX rec correctly
	uint32 minRec = ((LLC_ASY_RX_DEPTH) < (LLC_ASY_TX_DEPTH)) ? 4*(LLC_ASY_RX_DEPTH) : 4*(LLC_ASY_TX_DEPTH);
	uint32 minRecIdx=1;
	
	minRec-=5*4; //reserve space for max header
	while (minRecIdx<13)
	{
		if (minRec < (4<<minRecIdx)) break;
		minRecIdx++;
	}
	minRecIdx<<=12;
	state.busCapabilities |= minRecIdx & BUSINFO_MAXREC;
	
	
#ifdef _IRMC_CAPS // iso_resource_manager capable
	state.busCapabilities |= BUSINFO_IRMC;
#endif //_IRMC_CAPS
	// cycle_master capable
	state.busCapabilities |= BUSINFO_CMC;
	state.busCapabilities = (state.busCapabilities & ~BUSINFO_NOCMC_ACC) | BUSINFO_CMC_ACC_0PPM;
	state.busCapabilities |= BUSINFO_ISC;
#ifdef _BMC_CAPS // bus_manager capable
	state.busCapabilities |= BUSINFO_BMC;
#endif //_BMC_CAPS
#ifdef _PMC_CAPS // power_manager capable
	state.busCapabilities |= BUSINFO_PMC;
#endif //_PMC_CAPS

	//we will prepare a root directory in all cases
	root->pNext = 0;
	root->nElements = 0;
	root->keyId = RDK_ID_DIRECTORY_ID;
	root->keyType = RDK_TYPE_DIRECTORY;
	root->data.immVal = 0;
	root->nodeNum = state.gNodeNum++;
	root->pRefFrom = 0;
	root->pImage = 0;
	return NO_ERROR;
}

HRESULT crbSetBusInfoCapabilities(uint32 caps)
{
	state.busCapabilities = caps;
	return NO_ERROR;
}



HRESULT crbAddImmediateEntry(CRBHANDLE dir, uint32 key_id, uint32 value)
{
	CRBHANDLE temp = dir;
	CRBHANDLE next = NULL;
	uint32 i = 0;

	if (NULL == dir)
	{
		sysLogError(E_CRB_INVALID_HANDLE, __LINE__, moduleName);
		return E_CRB_INVALID_HANDLE;
	}
	// limit prevents stack overflow
	if (dir->nElements > 255)
	{
		sysLogError(E_CRB_TOO_MANY_ITEMS, __LINE__, moduleName);
		return E_CRB_TOO_MANY_ITEMS;
	}

	// can add elements to dir's only
	if (RDK_TYPE_DIRECTORY != dir->keyType)
	{
		sysLogError(E_CRB_NOT_DIR_HANDLE, __LINE__, moduleName);
		return E_CRB_NOT_DIR_HANDLE;
	}

	// make an element to hold the immediate value
	next = crbCreateElement();
	if (next)
	{
		next->keyId = key_id;
		next->keyType = RDK_TYPE_IMMEDIATE;
		next->data.immVal = value;
		next->nodeNum = state.gNodeNum++;	// for debug

		// if directory is empty, first item hangs off of data.pList
		if (dir->nElements == 0)
		{
			dir->data.pList = next;
		}
		else
		{
			// go to the end of the list of entries in this directory
			temp = dir->data.pList;
			for (i=1; i<dir->nElements; i++)
			{
				temp = temp->pNext;
			}
			// next item hangs off of the last item's pNext
			temp->pNext = next;
		}
		dir->nElements++;

		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "\nCreated node:  %08x\n", next->nodeNum);
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "  data: %08x\n  keyId: %08x\n  keyType: %08x\n  nElements: %08x\n\n", 
		//	next->data.immVal, next->keyId, next->keyType, next->nElements);
	}
	else
	{
		sysLogError(E_CRB_NO_HEAP, __LINE__, moduleName);
		return E_CRB_NO_HEAP;
	}
	return NO_ERROR;
}

CRBHANDLE crbAddSubDirectoryEntry(CRBHANDLE dir, uint32 key_id)
{
	CRBHANDLE temp = dir;
	CRBHANDLE next = NULL;
	uint32 i = 0;

	if (NULL == dir)
	{
		sysLogError(E_CRB_INVALID_HANDLE, __LINE__, moduleName);
		return NULL;
	}
	if (dir->nElements > 255)
	{
		sysLogError(E_CRB_TOO_MANY_ITEMS, __LINE__, moduleName);
		return NULL;
	}
	if (dir->keyType != RDK_TYPE_DIRECTORY)
	{
		sysLogError(E_CRB_NOT_DIR_HANDLE, __LINE__, moduleName);
		return NULL;
	}

	// create the directory entry
	next = crbCreateElement();
	if (next)
	{
		next->keyId = key_id;
		next->keyType = RDK_TYPE_DIRECTORY;
		next->nodeNum = state.gNodeNum++;
		
		// if directory is empty, first item hangs off of data.pList
		if (dir->nElements == 0)
		{
			dir->data.pList = next;
		}
		else
		{
			// go to the end of the list of entries in this directory
			temp = dir->data.pList;
			for (i=1; i<dir->nElements; i++)
			{
				temp = temp->pNext;
			}
			// next item hangs off of the last item's pNext
			temp->pNext = next;
		}
		dir->nElements++;
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "\nCreated node:  %08x\n", next->nodeNum);
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "  data: %08x\n  keyId: %08x\n  keyType: %08x\n  nElements: %08x\n\n", 
		//	next->data.immVal, next->keyId, next->keyType, next->nElements);
	}
	else
	{
		sysLogError(E_CRB_NO_HEAP, __LINE__, moduleName);
		return NULL;
	}

	return next;
}

HRESULT crbAddTextLeafEntry(CRBHANDLE dir, char * str)
{
	CRBHANDLE	temp = dir;
	CRBHANDLE	next = NULL;
	uint32		*pData = NULL;
	uint32		len = 0;
	uint32		alloc_len = 0;
	char		*pChar = str;
	uint32		i = 0;
	uint32		index = 0;

	if (NULL == dir)
	{
		sysLogError(E_CRB_INVALID_HANDLE, __LINE__, moduleName);
		return E_CRB_INVALID_HANDLE;
	}
	if (dir->nElements > 255)
	{
		sysLogError(E_CRB_TOO_MANY_ITEMS, __LINE__, moduleName);
		return E_CRB_TOO_MANY_ITEMS;
	}
	if (dir->keyType != RDK_TYPE_DIRECTORY)
	{
		sysLogError(E_CRB_NOT_DIR_HANDLE, __LINE__, moduleName);
		return E_CRB_NOT_DIR_HANDLE;
	}

	len = (uint32)strlen(str);

	// store the string data in quadlet leaf format
	alloc_len = len + 4 - (len%4);	// quadlet alignment
	alloc_len += 8;					// 2 text_descriptor header quads
	pData = (uint32 *) malloc(alloc_len);
	if (NULL == pData)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_CRB, "could not allcate leaf data storage\n");
		return E_CRB_NO_HEAP;
	}
	else
	{
		memset(pData, 0, alloc_len);
		alloc_fifo.push((void *)pData);
	}
	pData[index++] = 0;	// text descriptor header: type = 0, specifier_ID = 0
	pData[index++] = 0;	// text header: width = 0, character_set = 0, language = 0

	// copy string data
	for (i = 0; i < len / 4; i++)
	{
		pData[index++] = CSR_4CHARS_TO_QUADLET(pChar[0], pChar[1], pChar[2], pChar[3]);
		pChar += 4;
	}
	switch (len % 4)
	{																						
		case 3: pData[index++] = CSR_4CHARS_TO_QUADLET(pChar[0],	pChar[1],	pChar[2],	0); break;
		case 2: pData[index++] = CSR_4CHARS_TO_QUADLET(pChar[0],	pChar[1],	0,			0); break;
		case 1: pData[index++] = CSR_4CHARS_TO_QUADLET(pChar[0],	0,			0,			0); break;
	}

	next = crbCreateElement();
	if (next)
	{
		next->keyId = RDK_ID_DESCRIPTOR;
		next->keyType = RDK_TYPE_LEAF;
		next->data.pLeafData = pData;
		next->nElements = (alloc_len/4);	// number of quads of leaf text
		next->nodeNum = state.gNodeNum++;

		// if directory is empty, first item hangs off of data.pList
		if (dir->nElements == 0)
		{
			dir->data.pList = next;
		}
		else
		{
			// go to the end of the list of entries in this directory
			temp = dir->data.pList;
			for (i=1; i<dir->nElements; i++)
			{
				temp = temp->pNext;
			}
			// next item hangs off of the last item's pNext
			temp->pNext = next;
		}
		dir->nElements++;

		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "\nCreated node:  %08x\n", next->nodeNum);
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "  data: %08x\n  keyId: %08x\n  keyType: %08x\n  nElements: %08x\n", 
		//	next->data.immVal, next->keyId, next->keyType, next->nElements);
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "  leaf data: \n");
		//for (i=0; i<next->nElements; i++)
		//{
		//	SYS_DEBUG(SYSDEBUG_TRACE_CRB, "    0x%08X\n", pData[i]);
		//}
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "\n");
	}
	else
	{
		free (pData);
		sysLogError(E_CRB_NO_HEAP, __LINE__, moduleName);
		return E_CRB_NO_HEAP;
	}
	return NO_ERROR;
}




HRESULT crbWalkTreeBFirst(crbElementFunc func, void * pData)
{
	CRBHANDLE pElement = NULL;
	CRBHANDLE pSubElement = NULL;

	traverse_fifo.init();
	traverse_fifo.push(&state.rootDir);

	while (1)
	{
		pElement = traverse_fifo.pop();
		if (!pElement) break;

		if (pElement->keyType == RDK_TYPE_DIRECTORY)
		{			
			//work the list of members
			pSubElement = pElement->data.pList; //first member
			while (pSubElement)
			{
				func(pSubElement,0,pData);
				if (RDK_TYPE_DIRECTORY == pSubElement->keyType)
				{
					traverse_fifo.push (pSubElement);
				}
				pSubElement = pSubElement->pNext;
			}
		}
	}
	return NO_ERROR;
}


//this is recursive, we could use a stack instead
static void recWalkTreeDFirst (CRBHANDLE pElement, uint32 depth, crbElementFunc func, void * pData)
{
	CRBHANDLE pSubElement;

	if (pElement->keyType == RDK_TYPE_DIRECTORY)
	{
		//work the list of members
		pSubElement = pElement->data.pList; //first member
		while (pSubElement)
		{
			func(pSubElement,depth,pData);
			if (RDK_TYPE_DIRECTORY == pSubElement->keyType)
			{
				recWalkTreeDFirst (pSubElement, depth+1, func, pData);
			}
			pSubElement = pSubElement->pNext;
		}
	}
}

HRESULT crbWalkTreeDFirst(crbElementFunc func, void * pData)
{
	recWalkTreeDFirst (&state.rootDir, 0, func, pData);
	return NO_ERROR;
}



static HRESULT treeSizeCB(CRBHANDLE node, uint32 depth, void * pData)
{
	uint32 * pQuads = (uint32 *)pData;

	//every entry takes up one quad
	*pQuads += 1;

	switch (node->keyType)
	{
		case RDK_TYPE_DIRECTORY:
			*pQuads += 1; //header, the actual elements will be enumerated later
			break;

		case RDK_TYPE_CSR_OFFSET:
		case RDK_TYPE_LEAF:
			*pQuads += node->nElements+1; //header and data
			break;
	}
	return NO_ERROR;
}


HRESULT crbCalculateRomSize (uint32 * pSize)
{
	if (state.bMinBusInfo)
	{
		*pSize = 1;
		return NO_ERROR;
	}
	*pSize = 5;
	*pSize += 1; //root dir header, not enumerated.
	return crbWalkTreeBFirst(treeSizeCB,pSize);
}


HRESULT crbCreateRom(uint32 * pRom, uint32 * pRomQuadSize)
{
	CRBHANDLE pElement = NULL;
	CRBHANDLE pSubElement = NULL;
	uint32 * pRomStart = pRom;
	uint32 * pRomPtr = pRom;
	uint32 * pImg = 0;
	uint32 nRomLeft;

	crc_fifo.init();
	traverse_fifo.init();
	unit_fifo.init();

	//check and create bus info
	if (!pRomPtr || !pRomQuadSize)
	{
		sysLogError(E_CRB_BAD_INPUT_PARAM, __LINE__, moduleName);
		return E_CRB_BAD_INPUT_PARAM;
	}
	nRomLeft = *pRomQuadSize;

	if (state.bMinBusInfo)
	{
		if (!nRomLeft) goto spaceError;
		*pRomPtr++ = 0x01000000 | ((state.wwuIDHigh>>8) & 0xffffff);
		nRomLeft--;
		*pRomQuadSize = pRomPtr-pRomStart;
		return NO_ERROR;
	}
	//create a regular bus info bloc
	if (nRomLeft < 5) goto spaceError;
	//update capabilities with generation
	if (state.infoGeneration != 0 && state.infoGeneration < BUSINFO_GEN_MAX)
	{
		state.infoGeneration += 1<<4;
	}
	else
	{
		state.infoGeneration = BUSINFO_GEN_MIN;
	}
	state.busCapabilities = (state.busCapabilities & ~BUSINFO_GEN_MAX) | state.infoGeneration;
	pRomPtr[0] = 0x04040000;					// info_len always 4, CRC_Length always 4, CRC inserted below
	pRomPtr[1] = 0x31333934;					// bus_name always "1394"
	pRomPtr[2] = state.busCapabilities;
	pRomPtr[3] = state.wwuIDHigh;
	pRomPtr[4] = state.wwuIDLow;
	pRomPtr[0] = (pRomPtr[0] & 0xffff0000) | generateCrc16(&pRomPtr[1], 4);
	pRomPtr+=5;
	nRomLeft-=5;

	state.rootDir.pRefFrom = 0; //no reference to the root

	traverse_fifo.push(&state.rootDir);

	while (1)
	{
		pElement = unit_fifo.pop();
		if (0 == pElement)
		{
			pElement = traverse_fifo.pop();
		}
		if (!pElement) break;

		// if this was referenced from somewhere we should fix-up
		if (pElement->pRefFrom)
		{
			*pElement->pRefFrom = (*pElement->pRefFrom & 0xff000000) | 
					((pRomPtr-pElement->pRefFrom) & 0xffffff);
		}
		if (!nRomLeft) goto spaceError;
		pElement->pImage = pRomPtr;
		*pRomPtr = pElement->nElements<<16; // insert header with length, CRC must be done later
		crc_fifo.push(pRomPtr); //need to go back and calculate all CRC's
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "0x%08X  0x%08X  0x%08X\n", pRomPtr, *pRomPtr, iRom);
		//iRom+=4;
		pRomPtr++;
		nRomLeft--;

		if (pElement->keyType == RDK_TYPE_DIRECTORY)
		{			
			//work the list of members
			pSubElement = pElement->data.pList; //first member
			while (pSubElement)
			{
				switch (pSubElement->keyType)
				{
					case RDK_TYPE_IMMEDIATE:
						//lay it down
						if (!nRomLeft) goto spaceError;	
						*pRomPtr = (pSubElement->keyType) | (pSubElement->keyId) | (pSubElement->data.immVal & 0xffffff);
						//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "0x%08X  0x%08X  0x%08X\n", pRomPtr, *pRomPtr, iRom);
						//iRom+=4;
						pRomPtr++;
						nRomLeft--;
						break;
					case RDK_TYPE_CSR_OFFSET:
					case RDK_TYPE_LEAF:
					case RDK_TYPE_DIRECTORY:
						if (!nRomLeft) goto spaceError;		
						pSubElement->pRefFrom = pRomPtr;
						*pRomPtr = (pSubElement->keyType) | (pSubElement->keyId); //fixup later
						//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "0x%08X  0x%08X  0x%08X\n", pRomPtr, *pRomPtr, iRom);
						//iRom+=4;
						pRomPtr++;
						nRomLeft--;
						if (RDK_ID_UNIT == pSubElement->keyId)
						{
							unit_fifo.push (pSubElement);
						}
						else
						{
							traverse_fifo.push (pSubElement);
						}						
						break;
				}
				pSubElement = pSubElement->pNext;
			}
		}
		else if (pElement->keyType == RDK_TYPE_LEAF)
		{
			if (nRomLeft < pElement->nElements) goto spaceError;		
			memcpy (pRomPtr, pElement->data.pLeafData, pElement->nElements*4);
			pRomPtr += pElement->nElements;
			nRomLeft -= pElement->nElements;
		}
	}

	//now we just need to fix up the CRC's
	while (0 != (pImg = crc_fifo.pop()))
	{
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "0x%08X  0x%08X pop crc\n", pImg, *pImg);
		*pImg = (*pImg & 0xffff0000) | 
						generateCrc16(pImg+1,(*pImg>>16) & 0xff);
		//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "0x%08X  0x%08X fixed\n", pImg, *pImg);
	}
	*pRomQuadSize = pRomPtr-pRomStart;
	//SYS_DEBUG(SYSDEBUG_TRACE_CRB, "\n\nConfig ROM size: %d\n\n", gCrbRomSize);

	return NO_ERROR;

spaceError:
	//we revert to minimal info
	*pRomStart = 0x01000000 | ((state.wwuIDHigh>>8) & 0xffffff);
	*pRomQuadSize = 1;
	sysLogError(E_CRB_IMAGE_TOO_SMALL, __LINE__, moduleName);
	return E_CRB_IMAGE_TOO_SMALL;
}




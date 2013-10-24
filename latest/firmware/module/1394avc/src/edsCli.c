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
  FILE PURPOSE:  

	Embedded Descriptor Services (EDS)
	
	This file is the public interface for the EDS. Primarily it describes
	the the interface for the client. There are a few interfaces that are
	used for controlling the EDS server as well.
	
	EDS Client:
		The Client API allows for the construction of descriptors, and for
		construction of the descriptor trees. (Tree are not an actual object
		but rather the description of how the descriptors are arranged 
		relative to one another.)  
		
	EDS Server:
		The Server is used to handle AV/C commands which enable another device
		to query and change the contents of descriptors on this system. 
		
		The server also manages descriptor connections from other devices, and
		makes sure that descriptor reads, occur in a mode which is 
		consistent with the originating node of the read.
		
		Primarily the EDS Server only shares data with the EDS Client interface.

  Revisions:
		created 18/08/2003 KBJ

****************************************************************************/

#ifdef _EDS
#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "avcUnitCli.h"
#include "avcCmdCli.h"
#include "eds.h"
#include "edsCli.h"
#include "edsSample.h"
#include "edsServer.h"
#include "edstypes.h"
#include "drs.h"
#include "dss.h"
#include "cli.h"
#include "lal.h"

void edsCliPrintResponse(AVC_HEADER *pHeaderInfo, PB *packetBlock)
{
	//KBJ??? Wait short while before printing -> else print locks
	TCTaskWait(500);
	avcUnitCliPrintPacketBlock(packetBlock);
	avcUnitCliPrintPacketBlockRawData(pHeaderInfo->ctype, packetBlock);
}

#ifdef _CLI_TOOL_EDS

#define AVC_DATA_SIZE_MAX					44						// Size in bytes of max. write data block

#define AVC_HEADER_BYTES					3															//  3 bytes: [fcp,ctype],[subunit type,ID],[opcode],
#define AVC_EDS_HEAD						11															// 11 bytes: maximum header opcode + descriptor_specifier	
#define AVC_CMD_OPEN_DESC_QUADLETS			(AVC_HEADER_BYTES+AVC_EDS_HEAD + 2)/4						// 16 bytes: ,[operand0]...[operand15]
#define AVC_CMD_READ_DESC_QUADLETS			(AVC_HEADER_BYTES+AVC_EDS_HEAD + 6)/4						// 20 bytes: ,[operand0]...[operand19]
#define AVC_CMD_WRITE_DESC_QUADLETS			(AVC_HEADER_BYTES+AVC_EDS_HEAD + 6 + AVC_DATA_SIZE_MAX)/4	// 64 bytes: ,[operand0]...[operand63]

/*

	TOOL DEFINITIONS

*/

static HRESULT	 edsTool_Connections(void)
{

	uint32		index=0;
	HRESULT		hResult;

	hResult = TCSemaphoreWait(edsConnectionLockSemID);
	if (hResult != NO_ERROR) return hResult;	
	
	while (index++ < EDS_SERVER_CONNECTION_COUNT)
	{
		uint32		nodeid=0;

		if(EdsConnectionList[index-1].in_use==TRUE)
		{
			EDS_SERVER_CONNECTION*	c=&EdsConnectionList[index-1];
			char					xxx[256];
			
			lalGetNodeAddrFromHandle(c->node, &nodeid);
			
			sprintf(xxx,"CID:%02d SUTYPE=%02d SUID=%d DESC=%03d MODE=%c TIMEOUT=%02d NODEID=0x%04x",index-1,
						c->avcheader.addrSubunit.subunit_type,c->avcheader.addrSubunit.subunit_ID,c->toplevel,(c->accessmode==AVC_DESC_SUBFUNCTION_READOPEN ?'R':'W'),
						c->timeout_countdown,nodeid);
			cliOutputLine(xxx);
		}
	}
	
	TCSemaphoreSignal(edsConnectionLockSemID);	
	
	return NO_ERROR;
}

static HRESULT	edsTool_DisplayTree(EDS_DESCRIPTOR_REF root)
{
	static	uint16	spacingcount=0;
	char			whitespace[80];

	uint16			counter=0;
	EDS_DESCRIPTOR*	rootdata=NULL;
	uint32			index=0;
	HRESULT			searchresult=NO_ERROR;
	
	drsLockDescriptor(root);
	drsGetDescriptor(root,&rootdata);

// Generate the whitespace string here.
	for (counter=0; counter < spacingcount; counter++)
	{
		whitespace[counter]=' ';	
	}
	whitespace[counter]=0;

	switch (rootdata->type)
	{
		case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
		{
			char	xxx[80];
			
			sprintf(xxx,"%s(%03d): EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER",whitespace,root);
			cliOutputLine(xxx);
			
			sprintf(xxx,"%s  SUBUNIT_ID=%02d",whitespace,rootdata->subunit.subunit_ID);
			cliOutputLine(xxx); 
			
			sprintf(xxx,"%s  SUBUNIT_TYPE=%02d",whitespace,rootdata->subunit.subunit_type);
			cliOutputLine(xxx); 

			sprintf(xxx,"%s  size_of_list_id=%02d",whitespace,rootdata->u.subunit_identifier.size_of_list_id);
			cliOutputLine(xxx); 

			sprintf(xxx,"%s  size_of_object_id=%02d",whitespace,rootdata->u.subunit_identifier.size_of_object_id);
			cliOutputLine(xxx); 

			sprintf(xxx,"%s  size_of_entry_position=%02d",whitespace,rootdata->u.subunit_identifier.size_of_entry_position);
			cliOutputLine(xxx); 

			sprintf(xxx,"%s  number_of_root_lists=%02d",whitespace,rootdata->u.subunit_identifier.number_of_root_lists);
			cliOutputLine(xxx);
		
			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);
				
				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_LIST)
					{
						sprintf(xxx,"%s    (%03d)RootListID=0x%04x",whitespace,item,idata->u.list_descriptor.list_id);
						cliOutputLine(xxx);
					}
					drsUnlockDescriptor(item);
				}
			}

			sprintf(xxx,"%s  infoblocks_subunitinfo",whitespace);
			cliOutputLine(xxx);

			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	

			
				searchresult=drsGetNthDescriptor(index++,&item);
	
				
				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_INFOBLOCK &&
						idata->u.info_block.buffertype==EDS_BUFFERTYPE_SUBUNITINFO)
					{
						spacingcount+=4;
						edsTool_DisplayTree(item);
						spacingcount-=4;

					}
					drsUnlockDescriptor(item);
				}
			}
			
			sprintf(xxx,"%s  infoblocks_manufacturerinfo",whitespace);
			cliOutputLine(xxx);
			
			index=1; 
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);
	

				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_INFOBLOCK &&
						idata->u.info_block.buffertype==EDS_BUFFERTYPE_MANUFACTURERINFO)
					{
						spacingcount+=4;
						edsTool_DisplayTree(item);
						spacingcount-=4;
					}
					drsUnlockDescriptor(item);
				}
			}

		}
		break;
		
		case EDS_DESCRIPTORTYPE_INFOBLOCK:
		{
			char	xxx[80];
			
			sprintf(xxx,"%s(%03d): EDS_DESCRIPTORTYPE_INFOBLOCK",whitespace,root);
			cliOutputLine(xxx);

			sprintf(xxx,"%s  info_block_type=0x%04x",whitespace,rootdata->u.info_block.type);
			cliOutputLine(xxx);
			
			sprintf(xxx,"%s  info_block_position=%02d",whitespace,rootdata->u.info_block.position);
			cliOutputLine(xxx);
			
			sprintf(xxx,"%s  info_block_buffertype=%02d",whitespace,rootdata->u.info_block.buffertype);
			cliOutputLine(xxx);

			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);
	

				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
					{
						spacingcount+=2;
						edsTool_DisplayTree(item);
						spacingcount-=2;
					}
					drsUnlockDescriptor(item);
				}
			}
		}
		break;

		case EDS_DESCRIPTORTYPE_LIST:
		{
			char	xxx[80];
			
			sprintf(xxx,"%s(%03d): EDS_DESCRIPTORTYPE_LIST",whitespace,root);
			cliOutputLine(xxx);
	
			sprintf(xxx,"%s  list_type=0x%02x",whitespace,rootdata->u.list_descriptor.type);
			cliOutputLine(xxx);
		
			sprintf(xxx,"%s  list_id=0x%04x",whitespace,rootdata->u.list_descriptor.list_id);
			cliOutputLine(xxx);
			
			sprintf(xxx,"%s  number_of_entries=%02d",whitespace,rootdata->u.list_descriptor.number_of_entries);
			cliOutputLine(xxx);

			sprintf(xxx,"%s  specific info info_blocks:",whitespace);
			cliOutputLine(xxx);


			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);

				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_INFOBLOCK &&
						idata->u.info_block.buffertype==EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO)
					{
						spacingcount+=4;
						edsTool_DisplayTree(item);
						spacingcount-=4;
					}
					drsUnlockDescriptor(item);
				}
			}			

			sprintf(xxx,"%s  list entries:",whitespace);
			cliOutputLine(xxx);

			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);

				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_ENTRY)
					{
						spacingcount+=4;
						edsTool_DisplayTree(item);
						spacingcount-=4;
					}
					drsUnlockDescriptor(item);
				}
			}	

			sprintf(xxx,"%s  extended info_blocks:",whitespace);
			cliOutputLine(xxx);

			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);

				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_INFOBLOCK &&
						idata->u.info_block.buffertype==EDS_BUFFERTYPE_EXTENDEDINFO)
					{
						spacingcount+=4;
						edsTool_DisplayTree(item);
						spacingcount-=4;
					}
					drsUnlockDescriptor(item);
				}
			}	

		}
		break;

		case EDS_DESCRIPTORTYPE_ENTRY:
		{
			char					xxx[80];

			EDS_DESCRIPTOR*			cdata=0;
			
			sprintf(xxx,"%s(%03d): EDS_DESCRIPTORTYPE_ENTRY",whitespace,root);
			cliOutputLine(xxx);
	
			sprintf(xxx,"%s  entry_type=0x%02x",whitespace,rootdata->u.entry_descriptor.type);
			cliOutputLine(xxx);
		
			sprintf(xxx,"%s  object_id=0x%04x",whitespace,rootdata->u.entry_descriptor.object_id);
			cliOutputLine(xxx);
			
			sprintf(xxx,"%s  entry_position=%02d",whitespace,rootdata->u.entry_descriptor.position);
			cliOutputLine(xxx);

			if (rootdata->u.entry_descriptor.child_list_reference)
			{
				if (NO_ERROR==drsLockDescriptor(rootdata->u.entry_descriptor.child_list_reference))
				{
					drsGetDescriptor(rootdata->u.entry_descriptor.child_list_reference,&cdata);
					
					sprintf(xxx,"%s  child_list_id=(%03d) 0x%04x",whitespace,rootdata->u.entry_descriptor.child_list_reference,cdata->u.list_descriptor.list_id);
					cliOutputLine(xxx);
					
					drsUnlockDescriptor(rootdata->u.entry_descriptor.child_list_reference);
				}
			}

			sprintf(xxx,"%s  specific info info_blocks:",whitespace);
			cliOutputLine(xxx);


			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);

				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_INFOBLOCK &&
						idata->u.info_block.buffertype==EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO)
					{
						spacingcount+=4;
						edsTool_DisplayTree(item);
						spacingcount-=4;
					}
					drsUnlockDescriptor(item);
				}
			}			


			sprintf(xxx,"%s  extended info_blocks:",whitespace);
			cliOutputLine(xxx);

			index=1;
			searchresult=NO_ERROR;	
			while (NO_ERROR==searchresult)
			{
				EDS_DESCRIPTOR_REF		item=0;
				EDS_DESCRIPTOR*			idata=NULL;	
			
				searchresult=drsGetNthDescriptor(index++,&item);

				if (NO_ERROR==searchresult)
				{
					searchresult=drsLockDescriptor(item);
				}
				
				if (NO_ERROR==searchresult)
				{
					drsGetDescriptor(item,&idata);
					if (root==idata->parent_reference && idata->type==EDS_DESCRIPTORTYPE_INFOBLOCK &&
						idata->u.info_block.buffertype==EDS_BUFFERTYPE_EXTENDEDINFO)
					{
						spacingcount+=4;
						edsTool_DisplayTree(item);
						spacingcount-=4;
					}
					drsUnlockDescriptor(item);
				}
			}	

		}
		break;

	}		
		
	drsUnlockDescriptor(root);		
	
	return NO_ERROR;
}

static HRESULT edsTool_Catalog(void)
{
	HRESULT		searchresult=NO_ERROR;
	uint32		index=1;
	char		xxx[80];
	
	cliOutputLine("Descriptor Catalog:");
	cliOutputLine("-");
	
	
	index=1;
	searchresult=NO_ERROR;	
	while (NO_ERROR==searchresult)
	{
		EDS_DESCRIPTOR_REF		item=0;
		EDS_DESCRIPTOR*			idata=NULL;	
		char*					itypename=NULL;
		uint32				objectid=0;
		uint32				objectposition=0;
		uint32				objecttype=0;
	
		searchresult=drsGetNthDescriptor(index++,&item);

		if (NO_ERROR==searchresult)
		{
			searchresult=drsLockDescriptor(item);
		}
		
		if (NO_ERROR==searchresult)
		{
			drsGetDescriptor(item,&idata);
			
			switch(idata->type)
			{
				case EDS_DESCRIPTORTYPE_LIST:
				{
					itypename="LST";
					objectid=idata->u.list_descriptor.list_id;
					objecttype=idata->u.list_descriptor.type;
				}
				break;	
				case EDS_DESCRIPTORTYPE_ENTRY:
				{
					itypename="ENT";
					objectid=idata->u.entry_descriptor.object_id;
					objecttype=idata->u.entry_descriptor.type;
					objectposition=idata->u.entry_descriptor.position;
				}
				break;
				case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
				{
					itypename="SID";
				}
				break;
				case EDS_DESCRIPTORTYPE_INFOBLOCK:
				{
					itypename="IB ";
					objecttype=idata->u.info_block.type;
					objectposition=idata->u.info_block.position;
				}
				break;
				case EDS_DESCRIPTORTYPE_OPAQUE:
				{
					itypename="OPQ";
					objecttype=idata->u.opaque_descriptor.type;
					objectposition=0;
				}
				break;
			}
			
			drsUnlockDescriptor(item);
			
			sprintf(xxx,"(%03d) %s suid=%02d sutype=0x%04x id=0x%04x type=0x%04x pos=%02d locks=%02d",
						item,itypename,idata->subunit.subunit_ID,idata->subunit.subunit_type,objectid,objecttype,objectposition, drsLockCount(item));

			

			cliOutputLine(xxx);
		}
	}
	
	return NO_ERROR;		
}

/***********************************************************************************/
// Descriptor edsSample server access example
// AV/C Embedded Descriptor System Commands
/***********************************************************************************/

#define MAX_SPECIFIER_LEVEL		2

//static uint8 level;
static uint8 size_of_list_id;
static uint8 size_of_object_id;
static uint8 size_of_entry_position;

static DSS_DESCRIPTOR_SPECIFIER specifier[MAX_SPECIFIER_LEVEL] = 
{
	{
	// Descriptor specifier level 0
	// type - default SUBUNIT
	AVC_DS_SUBUNIT_IDENTIFIER,		// Type
	// Parameters used for list and entry descriptor specifiers
	0,								// list_id
	0,								// entry_position
	0,								// object_id
	0,								// entry_type
	0,								// list_type
	// Parameters used for info block descriptor specifiers
	0,								// info_block_type
	0,								// info_block_instance_count
	0								// info_block_position
	},
	// Descriptor specifier level 1
	{
	// Descriptor specifier type - default SUBUNIT
	AVC_DS_SUBUNIT_IDENTIFIER,		// Type
	// Parameters used for list and entry descriptor specifiers
	0,								// list_id
	0,								// entry_position
	0,								// object_id
	0,								// entry_type
	0,								// list_type
	// Parameters used for info block descriptor specifiers
	0,								// info_block_type
	0,								// info_block_instance_count
	0								// info_block_position
	}
};

/*
static HRESULT edsLevel_Tool(uint32 lev)
{
	HRESULT	hResult = NO_ERROR;

	if (lev < MAX_SPECIFIER_LEVEL)
	{
		level = (uint8)lev;
		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	return hResult;
}
*/

static HRESULT edsCmdCliDescSpecifier(uint32 desc_specifier, uint32 arg1, uint32 arg2, uint32 arg3)
{
	HRESULT				hResult = NO_ERROR;
	

	if ((desc_specifier == AVC_DS_INFOBLOCK_INSTANCE) ||
		(desc_specifier == AVC_DS_INFOBLOCK_POSITION))
	{
		// Info block operations always 2 level specifier
		specifier[1].type = (uint8)desc_specifier;
	}
	else
	{
		specifier[0].type = (uint8)desc_specifier;
	}

	switch (desc_specifier)
	{
		case AVC_DS_SUBUNIT_IDENTIFIER: // 0x00
			break;
		case AVC_DS_LIST_ID:			// 0x10
			specifier[0].list_id = arg1;
			break;
		case AVC_DS_LIST_TYPE:			// 0x11
			specifier[0].list_type = (uint8)arg1;
			break;
		case AVC_DS_ENTRY_POSITION:		// 0x20
			specifier[0].list_id = arg1;
			specifier[0].entry_position = arg2;
			break;
		case AVC_DS_ENTRY_OBJECT_ID:	// 0x21
			specifier[0].list_id = arg1;
			specifier[0].list_type = (uint8)arg2;
			specifier[0].object_id = arg3;
			break;
		case AVC_DS_ENTRY_TYPE:			// 0x22
			specifier[0].entry_type = (uint8)arg1;
			break;
		case AVC_DS_OBJECT_ID:			// 0x23
			specifier[0].object_id = arg1;
			break;
		case AVC_DS_INFOBLOCK_INSTANCE: // 0x30
			specifier[1].info_block_type = (uint16)arg1;
			specifier[1].info_block_instance_count = (uint8)arg2;
			break;
		case AVC_DS_INFOBLOCK_POSITION: // 0x31
			specifier[1].info_block_position = (uint8)arg1;
			break;
	}

	return hResult;
}

static HRESULT edsCreateSpecifier(DataStream *pDs, uint8 lev)
{
	HRESULT				hResult = NO_ERROR;

	// operand[0], desc_specifier
	hResult = dsWrite8Bits(pDs, (uint8)specifier[lev].type);				
	if (hResult != NO_ERROR) return hResult;

	switch (specifier[lev].type)
	{
		case AVC_DS_SUBUNIT_IDENTIFIER:
			break;
		case AVC_DS_LIST_ID:
			// operand[1], list_id
			hResult = dsWriteBits(pDs, 8*size_of_list_id, specifier[lev].list_id);
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_DS_LIST_TYPE:
			// operand[1], list_type
			hResult = dsWrite8Bits(pDs, specifier[lev].list_type);				
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_DS_ENTRY_POSITION:
			// operand[1], list_type
			hResult = dsWriteBits(pDs, 8*size_of_list_id, specifier[lev].list_id);
			if (hResult != NO_ERROR) return hResult;
			// operand[2], entry_position
			hResult = dsWriteBits(pDs, 8*size_of_entry_position, specifier[lev].entry_position);
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_DS_ENTRY_OBJECT_ID:
			// operand[1], list_id
			hResult = dsWriteBits(pDs, 8*size_of_list_id, specifier[lev].list_id);
			if (hResult != NO_ERROR) return hResult;
			// operand[2], list_type
			hResult = dsWrite8Bits(pDs, specifier[lev].list_type);				
			if (hResult != NO_ERROR) return hResult;
			// operand[3], object_id
			hResult = dsWriteBits(pDs, 8*size_of_object_id, specifier[lev].object_id);
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_DS_ENTRY_TYPE:
			// operand[1], entry_type
			hResult = dsWrite8Bits(pDs, specifier[lev].entry_type);				
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_DS_OBJECT_ID:
			// operand[1], object_id
			hResult = dsWriteBits(pDs, 8*size_of_object_id, specifier[lev].object_id);
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_DS_INFOBLOCK_INSTANCE:
			// operand[1], info_block_type
			hResult = dsWrite16Bits(pDs, specifier[lev].info_block_type);				
			if (hResult != NO_ERROR) return hResult;
			// operand[8], instance_count
			hResult = dsWrite8Bits(pDs, specifier[lev].info_block_instance_count);				
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_DS_INFOBLOCK_POSITION:
			// operand[1], info_block_position
			hResult = dsWrite8Bits(pDs, specifier[lev].info_block_position);				
			if (hResult != NO_ERROR) return hResult;
			break;
	}

	return hResult;
}

_TC_INLINE HRESULT edsCliSendCmd(char ctype, DataStream *pDs, uint32 *data)
{
	uint32 size = pDs->cur_offset;
	uint32 rest = pDs->cur_offset%4;
	if (rest) size += (4 - rest);
	return avcCmdCliSendRequest(ctype, (uint16)size, data, pDs);
}

HRESULT edsCmdCliOpenDesc(char cctype, uint8 subfunction)
{
	HRESULT				hResult = NO_ERROR;
	uint32				data[AVC_CMD_OPEN_DESC_QUADLETS]; 
	uint8				opcode = AVC_CMD_OPEN_DESCRIPTOR; 
	DataStream			ds;
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = edsCreateSpecifier(&ds, 0);
	if (hResult != NO_ERROR) return hResult;

	// operand[1], subfunction
	hResult = dsWrite8Bits(&ds, subfunction);					
	if (hResult != NO_ERROR) return hResult;
	
	// operand[2], reserved/not used
	hResult = dsWrite8Bits(&ds, 0x00);	
	if (hResult != NO_ERROR) return hResult;

	hResult = edsCliSendCmd(ctype, &ds, data); 
	return hResult;
}

HRESULT edsCmdCliReadDesc(char cctype, uint16 address, uint16 data_length)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_READ_DESC_QUADLETS]; 
	uint8				opcode = AVC_CMD_READ_DESCRIPTOR; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = edsCreateSpecifier(&ds, 0);
	if (hResult != NO_ERROR) return hResult;

	// operand[1], read_result_status
	hResult = dsWrite8Bits(&ds, 0xFF);				
	if (hResult != NO_ERROR) return hResult;

	// operand[2], reserved
	hResult = dsWrite8Bits(&ds, 0x00);				
	if (hResult != NO_ERROR) return hResult;

	// operand[3-4], data_length
	hResult = dsWrite16Bits(&ds, data_length);		
	if (hResult != NO_ERROR) return hResult;

	// operand[5-6], address
	hResult = dsWrite16Bits(&ds, address);			
	if (hResult != NO_ERROR) return hResult;

	hResult = edsCliSendCmd(ctype, &ds, data); 
	return hResult;
}

HRESULT edsCmdCliWriteDesc(char cctype, uint16 address, uint16 data_length)
{
	HRESULT				hResult = NO_ERROR;
	uint32				data[AVC_CMD_WRITE_DESC_QUADLETS]; 
	uint8				opcode = AVC_CMD_WRITE_DESCRIPTOR; 
	DataStream			ds;
	uint16				i;
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = edsCreateSpecifier(&ds, 0);
	if (hResult != NO_ERROR) return hResult;

	// operand[1], subfunction
	hResult = dsWrite8Bits(&ds, AVC_DESC_SUBFUNCTION_PARTIAL_REPLACE);
	if (hResult != NO_ERROR) return hResult;

	// operand[2], group_tag
	hResult = dsWrite8Bits(&ds, AVC_DESC_GROUPTAG_IMMEDIATE);
	if (hResult != NO_ERROR) return hResult;

	// operand[3-4], replacement_data_length
	hResult = dsWrite16Bits(&ds, data_length);
	if (hResult != NO_ERROR) return hResult;

	// operand[5-6], address
	hResult = dsWrite16Bits(&ds, address);
	if (hResult != NO_ERROR) return hResult;
	
	// operand[7-8], original_data_length
	hResult = dsWrite16Bits(&ds, data_length);
	if (hResult != NO_ERROR) return hResult;

	// operand[9-..], data = 0, clear data
	if (data_length > AVC_DATA_SIZE_MAX)
		data_length = AVC_DATA_SIZE_MAX;

	for (i = 0; i < data_length; i++)
	{
		hResult = dsWrite8Bits(&ds, (uint8)i);			
		if (hResult != NO_ERROR) return hResult;
	}

	hResult = edsCliSendCmd(ctype, &ds, data); 
	return hResult;
}

HRESULT edsCmdCliReadInfoBlock(char cctype, uint16 address, uint16 data_length)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_READ_DESC_QUADLETS]; 
	uint8				opcode = AVC_CMD_READ_INFOBLOCK; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	// operand[0], number_of_levels
	hResult = dsWrite8Bits(&ds, 2);				
	if (hResult != NO_ERROR) return hResult;

	// Create specifier for level[0]
	hResult = edsCreateSpecifier(&ds, 0);
	if (hResult != NO_ERROR) return hResult;

	// Create specifier for level[1]
	hResult = edsCreateSpecifier(&ds, 1);
	if (hResult != NO_ERROR) return hResult;

	// operand[1], read_result_status
	hResult = dsWrite8Bits(&ds, 0xFF);				
	if (hResult != NO_ERROR) return hResult;

	// operand[2], reserved
	hResult = dsWrite8Bits(&ds, 0x00);				
	if (hResult != NO_ERROR) return hResult;

	// operand[3-4], data_length
	hResult = dsWrite16Bits(&ds, data_length);		
	if (hResult != NO_ERROR) return hResult;

	// operand[5-6], address
	hResult = dsWrite16Bits(&ds, address);			
	if (hResult != NO_ERROR) return hResult;

	hResult = edsCliSendCmd(ctype, &ds, data); 
	return hResult;
}

// Creates, delete and modifies a sample descriptor three
static HRESULT edsSample_Tool(char operation)
{

	HRESULT hResult = NO_ERROR;

	switch (operation)
	{
		case 'c':
			hResult = edsSampleCreate();
			break;
		case 'd':
			hResult = edsSampleReleaseAll();
			break;
		case 'l':
			hResult = edsSampleReassignList1ToList2();
			break;
	}

	return hResult;
}

/**********************************************************************************/

static  char	DisplayTree_USAGE[]=\
				"eds:Tree :\n\r"\
				"use: eds:tree <root descriptor handle>\n\r"\
				"------------------------------------------------------------\n\r"\
				"edsDisplayTree starts from the specified handle, and displays "\
				"a graphical tree in hierarchical";
				
static	CLIDescriptor	edsTool_DisplayTree_Descriptor[]=
{
	{kCLI_Name,				(uint32)"tree"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsTool_DisplayTree},
	{kCLI_UsageString, 		(uint32)DisplayTree_USAGE},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"Displays useful information about EDS."},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	Catalog_USAGE[]=\
				"eds:catalog :\n\r"\
				"use: eds:catalog\n\r"\
				"------------------------------------------------------------\n\r"\
				"eds:catalog, displays all the descriptors instantiated on\n\r"\
				"the device.";
				
static	CLIDescriptor	edsTool_Catalog_Descriptor[]=
{
	{kCLI_Name,				(uint32)"catalog"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsTool_Catalog},
	{kCLI_UsageString, 		(uint32)Catalog_USAGE},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"Catalog of Descriptors"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	Connections_USAGE[]=\
				"eds:connections :\n\r"\
				"use: eds:connections\n\r"\
				"------------------------------------------------------------\n\r"\
				"edsConnections displays all the open descriptor connections.";
				
static	CLIDescriptor	edsTool_Connections_Descriptor[]=
{
	{kCLI_Name,				(uint32)"connections"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsTool_Connections},
	{kCLI_UsageString, 		(uint32)Connections_USAGE},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"Shows open descriptor connections"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static  char	edsShowParsedData_USAGE[]=\
				"edsDebug:\n\r"\
				"------------------------------------------------------------\n\r"\
				"When set to true the parsed data from incoming descriptor\n\r"\
				"requests is shown in the monitor.";
				
static  CLIDescriptor   edsShowParsedData_Descriptor[]=
{
	{kCLI_Name,				(uint32)"edsDebug"},
	{kCLI_Type, 			kCLI_TYPE_VARIABLE},
	{kCLI_Variable,			(uint32)&EdsShowParsedData},
	{kCLI_VariableSize,		32},
	{kCLI_VariableDisplayType, kCLI_DISPLAYTYPE_BOOLEAN},
	{kCLI_UsageString, 		(uint32)edsShowParsedData_USAGE},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// EDS set descriptor specifier control command
static  char	edsSpecifier_UsageString[]=\
				"eds:spec: \n\r"\
				"use: eds:spec <specifier> <arg1> <arg2> <arg3>\n\r"\
				"eds:spec subunit\n\r"\
				"eds:spec listId    <list_id>\n\r"\
				"eds:spec listType  <list_type>\n\r"\
				"eds:spec entryPos  <list_id> <entry_position>\n\r"\
				"eds:spec entryObj  <list_id> <list_type> <object_id>\n\r"\
				"eds.spec entryType <entry_type>\n\r"\
				"eds.spec objectId  <object_id>\n\r"\
				"eds.spec infoType  <info_block_type> <instance_count>\n\r"\
				"eds:spec infoPos   <info_bloc_position>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Selects descriptor specifier used for next AVC command (open,read,write)\n\r";

static	CLIDescriptor	edsSpecifier_Descriptor[]=
{
	{kCLI_Name,				(uint32)"spec"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsCmdCliDescSpecifier},
	{kCLI_UsageString, 		(uint32)edsSpecifier_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"kIII"},
	{kCLI_ConstantsString,	(uint32)"subunit=0,listId=16,listType=17,entryPos=32,\
entryObj=33,entryType=34,objectId=35,infoType=48,infoPos=49"},
	{kCLI_ModuleName,		(uint32)"Setting descriptor specifier for commands"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// EDS OPEN DESCRIPTOR control command
static  char	edsOpenDesc_UsageString[]=\
				"eds:open: \n\r"\
				"use: eds:open <ctype> <action>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"action: close, read, write\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends AVC open descriptor\n\r";

static	CLIDescriptor	edsOpenDesc_Descriptor[]=
{
	{kCLI_Name,				(uint32)"open"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsCmdCliOpenDesc},
	{kCLI_UsageString, 		(uint32)edsOpenDesc_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ck"},
	{kCLI_ConstantsString,	(uint32)"close=0,read=1,write=3"},
	{kCLI_ModuleName,		(uint32)"sends AVC open descriptor"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// EDS READ DESCRIPTOR control command
static  char	edsReadDesc_UsageString[]=\
				"eds:read: \n\r"\
				"use: eds:read <ctype> <address> <length>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"address: Address offset starting point to read from descriptor\n\r"\
				"length: Number of bytes to be read from the target descriptor\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends AVC read descriptor\n\r";

static	CLIDescriptor	edsReadDesc_Descriptor[]=
{
	{kCLI_Name,				(uint32)"read"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsCmdCliReadDesc},
	{kCLI_UsageString, 		(uint32)edsReadDesc_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cdd"},
	{kCLI_ModuleName,		(uint32)"sends AVC read descriptor"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// EDS WRITE DESCRIPTOR control command
static  char	edsWriteDesc_UsageString[]=\
				"eds:write: \n\r"\
				"use: eds:write <ctype> <address> <length>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"address: Address offset starting point to be written\n\r"\
				"length: Number of bytes in the data to be written\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends AVC write descriptor\n\r";

static	CLIDescriptor	edsWriteDesc_Descriptor[]=
{
	{kCLI_Name,				(uint32)"write"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsCmdCliWriteDesc},
	{kCLI_UsageString, 		(uint32)edsWriteDesc_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cdd"},
	{kCLI_ModuleName,		(uint32)"sends AVC write descriptor"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// EDS Create sample descriptor structure
static  char	edsSample_UsageString[]=\
				"eds:sample: \n\r"\
				"use: eds:sample <operation>\n\r"\
				"operation: c - creates sample descriptors \n\r"\
				"           d - deletes sample descriptors \n\r"\
				"           l - move list1 to be a child of list2 \n\r"\
				"------------------------------------------------------------\n\r"\
				"Creates a sample descriptor structure\n\r";

static	CLIDescriptor	edsSample_Descriptor[]=
{
	{kCLI_Name,				(uint32)"sample"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsSample_Tool},
	{kCLI_UsageString, 		(uint32)edsSample_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"c"},
	{kCLI_ModuleName,		(uint32)"Creates a sample descriptor structure"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// EDS Set level for descriptor specifier
/* KBJ???
static  char	edsLevel_UsageString[]=\
				"eds:level: \n\r"\
				"use: eds:level <0|1>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set level for descriptor specifier\n\r";

static	CLIDescriptor	edsLevel_Descriptor[]=
{
	{kCLI_Name,				(uint32)"level"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsLevel_Tool},
	{kCLI_UsageString, 		(uint32)edsLevel_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"Set level for descriptor specifier"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
*/

// EDS READ INFO BLOCK control command
static  char	edsReadIB_UsageString[]=\
				"eds:readIB: \n\r"\
				"use: eds:readIB <ctype> <address> <length>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"address: Address offset starting point to read from info block\n\r"\
				"length: Number of bytes to be read from the target info block\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends AVC read info block\n\r";

static	CLIDescriptor	edsReadIB_Descriptor[]=
{
	{kCLI_Name,				(uint32)"readIB"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&edsCmdCliReadInfoBlock},
	{kCLI_UsageString, 		(uint32)edsReadIB_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cdd"},
	{kCLI_ModuleName,		(uint32)"sends AVC read info block"},
	{kCLI_GroupName,		(uint32)"eds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void edsCliInstallTools(void)
{
	HRESULT hResult = NO_ERROR;

	// Uses data from edsSample
	size_of_list_id = siData.size_of_list_id;
	size_of_object_id = siData.size_of_object_id;
	size_of_entry_position = siData.size_of_entry_position;
	//level = 0; // Specifier level

	hResult += cliInstallCLIDescriptor(edsTool_DisplayTree_Descriptor);
	hResult += cliInstallCLIDescriptor(edsTool_Catalog_Descriptor);
	hResult += cliInstallCLIDescriptor(edsTool_Connections_Descriptor);
	hResult += cliInstallCLIDescriptor(edsShowParsedData_Descriptor);
	hResult += cliInstallCLIDescriptor(edsSpecifier_Descriptor);
	hResult += cliInstallCLIDescriptor(edsOpenDesc_Descriptor);
	hResult += cliInstallCLIDescriptor(edsReadDesc_Descriptor);
	hResult += cliInstallCLIDescriptor(edsWriteDesc_Descriptor);
	hResult += cliInstallCLIDescriptor(edsSample_Descriptor);
	// KBJ??? hResult += cliInstallCLIDescriptor(edsLevel_Descriptor);
	hResult += cliInstallCLIDescriptor(edsReadIB_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing eds cli tools");
	}
}

#endif //_CLI_TOOL_EDS
#endif //_CLI
#endif //_EDS
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

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

	Embedded Descriptor Example for creating and testing a eds Client.

	Creates a descriptor three with two root lists and four entries

	Subunint -+--> Root list ID1 -> Entry OBJID1, 
			  |					    Entry OBJID2, 
			  |					    Entry OBJID3 (Info Block)
			  +--> Root list ID2 -> Entry OBJID4 (Callback)
			 					    Entry OBJID5 --> List ID3 -> Entry OBJID7
									Entry OBJID6 [--> List ID1 -> ...] Reassing

  Revisions:
		created 16/09/2003 KBJ

****************************************************************************/
#ifdef _EDS

#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTime.h"
#include "ErrorCodes.h"
#include "avc.h"
#include "systemcallbacks.h"
#include "drs.h"
#include "eds.h"
#include "edsClientTypes.h"


/*****************************************************************************
 	- Subunint identifier descriptor
*/

#define SAMPLE_INFO_BLOCK_TYPE1		0x8001		// Subunit info block
#define SAMPLE_INFO_BLOCK_TYPE2		0x8002		// Entry info block


// As an example the subunit consist of a Non Info Block and one Info Block data..
static uint32  subunit_info[] =  
{   // Subunit Non Info Block with dummy data
	0x000e0203,			// non_info_block_length = 14
	0x04050607,
	0x08090a0b,
	0x0c0d0e0f
};

static uint32  subunit_info_block[] =  
{   // Subunit Info Block with dummy data
	0xf0f1f2f3,
	0xf4f5f6f7,
	0xf8f9fafb,
	0xfcfdfeff
};

static uint32  manu_info[] =  
{   // Manufacture dependent information with dummy data
	0x20212223,
	0x24252627,
	0x28292a2b,
	0x2c2d2e2f
};

/*****************************************************************************
 	- List descriptor 1 + 2

	Two sample list descriptors is created as root lists
*/

// Only one list type in this example
#define SAMPLE_LIST_TYPE1		0x91
#define SAMPLE_LIST_TYPE2		0x92
#define SAMPLE_LIST_TYPE3		0x93

// Two list descriptors created
#define LIST_ID1				0x1111
#define LIST_ID2				0x2222
#define LIST_ID3				0x3333

static uint32  list1_specific_info[] =  
{   // List specific information with dummy data
	0x30313233,
	0x34353637,
	0x38393a3b,
	0x3c3d3e3f
};

static uint32  list2_specific_info[] =  
{   // List specific information with dummy data
	0x40414243,
	0x44454647,
	0x48494a4b,
	0x4c4d4e4f
};

static uint32  list3_specific_info[] =  
{   // List specific information with dummy data
	0xa0a1a2a3,
	0xa4a5a6a7,
	0xa8a9aaab,
	0xacadaeaf
};

/*****************************************************************************
 	- Entry descriptor

	Five sample entry descriptors is created as entries in the list 1+2 
*/

// Three entry types is defined for this example
#define SAMPLE_ENTRY_TYPE1		0xA1	// Normal entry
#define SAMPLE_ENTRY_TYPE2		0xA2	// Callback entry
#define SAMPLE_ENTRY_TYPE3		0xA3	// Parent to list entry

// Four entry descriptors created (size_of_object_ID == 1)
#define OBJECT_ID1				0x11
#define OBJECT_ID2				0x22
#define OBJECT_ID3				0x33 
#define OBJECT_ID4				0x44 
#define OBJECT_ID5				0x55
#define OBJECT_ID6				0x66
#define OBJECT_ID7				0x77 

static uint32  entry1_specific_info[] =  
{   // List specific information with dummy data
	0x50515253,
	0x54555657,
	0x58595a5b,
	0x5c5d5e5f
};

static uint32  entry2_specific_info[] =  
{   // List specific information with dummy data
	0x60616263,
	0x64656667,
	0x68696a6b,
	0x6c6d6e6f,
	0x70717273,
	0x74757677,
	0x78797a7b,
	0x7c7d7e7f
};

static uint32  entry3_specific_info[] =  
{   // List specific information with dummy data
	0x80818283,
	0x84858687
};

static uint32  entry4_specific_info[] =  
{   // List specific information with dummy data
	0x90919293,
	0x94959697,
	0x98999a9b,
	0x9c9d9e9f
};

static uint32  entry5_specific_info[] =  
{   // List specific information with dummy data
	0xb0b1b2b3,
	0xb4b5b6b7,
	0xb8b9babb,
	0xbcbdbebf
};

static uint32  entry6_specific_info[] =  
{   // List specific information with dummy data
	0xc0c1c2c3,
	0xc4c5c6c7
};

static uint32  entry7_specific_info[] =  
{   // List specific information with dummy data
	0xd0d1d2d3,
	0xd4d5d6d7,
	0xd8d9dadb,
	0xdcdddedf,
	0xe0e1e2e3,
	0xe4e5e6e7,
	0xe8e9eaeb,
	0xecedeeef
};

/*****************************************************************************
 	- EDS sample session
*/

EDS_SUBUNITIDENTIFIER siData = 
{
	2,							// uint8		size_of_list_id;
	1,							// uint8		size_of_object_id; (Max. 4 bytes)
	2,							// uint8		size_of_entry_position;
	0,							// uint16		number_of_root_lists;
	0,							// uint16		si_length;
	0,							// uint16		si_infoblock_count;
	0,							// RANGEMAP*	si_writable_mask;
	0,							// void*		subunit_information;
	0,							// uint16		mi_length;
	0,							// uint16		mi_infoblock_count;
	0,							// RANGEMAP*	mi_writable_mask;
	0,							// void*		manufacturer_information;
	0							// uint16		extended_infoblock_count; 
};

static EDS_CLIENT_SESSION sampleSession = 
{
	AVC_SU_TYPE_AUDIO,		// subunit type
	0,						// subunit id
	0,						// subunit_desc_ref
	0						// custom_resolver
};

/*****************************************************************************
 	- EDS descriptor references
*/

static	EDS_DESCRIPTOR_REF		edsSubunitInfoBlock;
// Reference for list descriptors
static	EDS_DESCRIPTOR_REF		edsListRef[3];
// Reference for entries in list 1 - 3
static EDS_DESCRIPTOR_REF		edsEntryRef[3][3];

// Reference for entry info blocks in list 1 - 3
static EDS_DESCRIPTOR_REF		edsInfoBlockRef[3][3];

/*************************************************************************
	FUNCTIONS
*************************************************************************/

// Callback registration used for OBJECT_ID4
static HRESULT edsSampleVolatileCallback(uint16 operation, 
										 EDS_DESCRIPTOR_REF  descriptor, 
										 EDS_BUFFER_TYPE  buffertype)
{
	HRESULT 				hResult = NO_ERROR;
	EDS_DESCRIPTOR*			descdata=NULL;
	uint32*					pData;

	UNUSED_ARG(buffertype);
	
	hResult=drsGetDescriptor(descriptor,&descdata);

	if (hResult == NO_ERROR)
	{
		pData = (uint32*)descdata->u.entry_descriptor.specific_info;
		
		switch (operation)
		{
			case EDS_CALLBACKOPERATION_WRITE:
				break;
			case EDS_CALLBACKOPERATION_READ:
				// Returns current time
				pData[0] = TCTimeGet();
				break;
		}
	}

	return hResult;
}

// Create and initialisation of descriptors
HRESULT edsSampleCreate(void)
{
	HRESULT 				hResult = NO_ERROR;
	uint32					i;
	
	// Create client descriptor session 
	hResult = edsRegisterSubunitClientSession(&sampleSession);

	// Create subunit identifier descriptor
	if (NO_ERROR == hResult)
	{
		hResult = edsCreateSubunitIdentifier(&sampleSession, &siData);
	}
	// Create subunit type dependent information
	if (NO_ERROR == hResult)
	{
		hResult = edsSetDataBuffer(sampleSession.subunit_desc_ref,
								   EDS_BUFFERTYPE_SUBUNITINFO,
								   sizeof(subunit_info),
								   subunit_info);

	}
	// Create subunit type dependent information
	if (NO_ERROR == hResult)
	{
		hResult = edsCreateInfoBlock(&sampleSession,
									 SAMPLE_INFO_BLOCK_TYPE1,
								     sizeof(subunit_info_block),
									 subunit_info_block,
									 &edsSubunitInfoBlock);

	}
	// Associate info block with subunit descriptor
	if (NO_ERROR == hResult)
	{
		hResult = edsAssociateChildWithParent(sampleSession.subunit_desc_ref,
											  edsSubunitInfoBlock,
											  EDS_BUFFERTYPE_SUBUNITINFO);
	}
	// Create manufacturer dependent information as a non info block
	if (NO_ERROR == hResult)
	{
		hResult = edsSetDataBuffer(sampleSession.subunit_desc_ref,
								   EDS_BUFFERTYPE_MANUFACTURERINFO,
								   sizeof(manu_info),
								   manu_info);

	}

	// Create sample list 1 descriptor
	if (NO_ERROR == hResult)
	{
		hResult = edsCreateListDescriptor(&sampleSession, 
										  SAMPLE_LIST_TYPE1, 
										  LIST_ID1, 
										  AVC_DESC_ATTRIB_OBJECT_ID,
										  sizeof(list1_specific_info), 
										  list1_specific_info, 
										  &edsListRef[0]);
		if (NO_ERROR == hResult)
		{
			// Associate list 1 with subunit identifier descriptor
			hResult = edsAssociateChildWithParent(sampleSession.subunit_desc_ref,
												  edsListRef[0],
												  EDS_BUFFERTYPE_UNSPECIFIED);
		}
		if (NO_ERROR == hResult)
		{
			// Create entry 1 descriptor
			hResult = edsCreateEntryDescriptor(&sampleSession,
											   SAMPLE_ENTRY_TYPE1,
											   OBJECT_ID1,
											   sizeof(entry1_specific_info),
											   entry1_specific_info,
											   &edsEntryRef[0][0]);
		}
		if (NO_ERROR == hResult)
		{
			// Create entry 2 descriptor
			hResult = edsCreateEntryDescriptor(&sampleSession,
											   SAMPLE_ENTRY_TYPE1,
											   OBJECT_ID2,
											   sizeof(entry2_specific_info),
											   entry2_specific_info,
											   &edsEntryRef[0][1]);
		}
		if (NO_ERROR == hResult)
		{
			// Create entry 3 descriptor
			hResult = edsCreateEntryDescriptor(&sampleSession,
											   SAMPLE_ENTRY_TYPE1,
											   OBJECT_ID3,
											   0,
											   NULL,
											   &edsEntryRef[0][2]);
		}
		if (NO_ERROR == hResult)
		{
			// Create info block for entry 3 descriptor
			hResult = edsCreateInfoBlock(&sampleSession,
										 SAMPLE_INFO_BLOCK_TYPE2,
										 sizeof(entry3_specific_info),
										 entry3_specific_info,
										 &edsInfoBlockRef[0][2]);

		}
		// Associate info block with entry 3 descriptor
		if (NO_ERROR == hResult)
		{
			hResult = edsAssociateChildWithParent(edsEntryRef[0][2],
												  edsInfoBlockRef[0][2],
												  EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
		}
		if (NO_ERROR == hResult)
		{
			for (i = 0; i < 3; i++)
			{
				// Associate entry 1-3 with list 1
				hResult = edsAssociateChildWithParent(edsListRef[0],
													  edsEntryRef[0][i],
													  EDS_BUFFERTYPE_UNSPECIFIED);
				if (NO_ERROR != hResult) break;
			}
		}
	}

	// Create sample list 2 descriptor
	if (NO_ERROR == hResult)
	{
		hResult = edsCreateListDescriptor(&sampleSession, 
										  SAMPLE_LIST_TYPE2, 
										  LIST_ID2, 
										  AVC_DESC_ATTRIB_OBJECT_ID,
										  sizeof(list2_specific_info), 
										  list2_specific_info, 
										  &edsListRef[1]);
		if (NO_ERROR == hResult)
		{
			// Associate list 2 with subunit identifier descriptor
			hResult = edsAssociateChildWithParent(sampleSession.subunit_desc_ref,
												  edsListRef[1],
												  EDS_BUFFERTYPE_UNSPECIFIED);
		}
		if (NO_ERROR == hResult)
		{
			// Create and add entry 1 descriptor
			hResult = edsCreateEntryDescriptor(&sampleSession,
											   SAMPLE_ENTRY_TYPE3,
											   OBJECT_ID4,
											   sizeof(entry4_specific_info),
											   entry4_specific_info,
											   &edsEntryRef[1][0]);
		}
		if (NO_ERROR == hResult)
		{
			// Add callback function of OBJECT_ID4
			hResult = edsSetVolatileCallback(edsEntryRef[1][0],
											 edsSampleVolatileCallback);
		}
		if (NO_ERROR == hResult)
		{
			// Create and add entry 2 descriptor
			hResult = edsCreateEntryDescriptor(&sampleSession,
											   SAMPLE_ENTRY_TYPE2,
											   OBJECT_ID5,
											   sizeof(entry5_specific_info),
											   entry5_specific_info,
											   &edsEntryRef[1][1]);
		}
		if (NO_ERROR == hResult)
		{
			// Create and add entry 3 descriptor
			hResult = edsCreateEntryDescriptor(&sampleSession,
											   SAMPLE_ENTRY_TYPE2,
											   OBJECT_ID6,
											   sizeof(entry6_specific_info),
											   entry6_specific_info,
											   &edsEntryRef[1][2]);
		}
		if (NO_ERROR == hResult)
		{
			for (i = 0; i < 3; i++)
			{
				// Associate entry 1-3 with list 2
				hResult = edsAssociateChildWithParent(edsListRef[1],
													  edsEntryRef[1][i],
													  EDS_BUFFERTYPE_UNSPECIFIED);
				if (NO_ERROR != hResult) break;
			}
		}
	}

	// Create sample list 3 descriptor
	if (NO_ERROR == hResult)
	{
		hResult = edsCreateListDescriptor(&sampleSession, 
										  SAMPLE_LIST_TYPE3, 
										  LIST_ID3, 
										  AVC_DESC_ATTRIB_NONE,
										  sizeof(list3_specific_info), 
										  list3_specific_info, 
										  &edsListRef[2]);
		if (NO_ERROR == hResult)
		{
			// Associate list 3 with list 2 descriptor
			hResult = edsAssociateChildWithParent(edsEntryRef[1][1],
												  edsListRef[2],
												  EDS_BUFFERTYPE_UNSPECIFIED);
		}
		if (NO_ERROR == hResult)
		{
			// Create and add entry 1 descriptor
			hResult = edsCreateEntryDescriptor(&sampleSession,
											   SAMPLE_ENTRY_TYPE1,
											   OBJECT_ID7,
											   sizeof(entry7_specific_info),
											   entry7_specific_info,
											   &edsEntryRef[2][0]);
		}
		if (NO_ERROR == hResult)
		{
			// Associate entry 1 with list 3
			hResult = edsAssociateChildWithParent(edsListRef[2],
												  edsEntryRef[2][0],
												  EDS_BUFFERTYPE_UNSPECIFIED);
		}
	}

	return hResult;
}


HRESULT edsSampleReassignList1ToList2(void)
{
	HRESULT 				hResult = NO_ERROR;

	hResult = edsRemoveParentalAssociation(edsListRef[0]);

	if (NO_ERROR == hResult)
	{
		// Associate list 1 with list 2 descriptor, entry ID3
		hResult = edsAssociateChildWithParent(edsEntryRef[1][2],
											  edsListRef[0],
											  EDS_BUFFERTYPE_UNSPECIFIED);
	}

	return hResult;
}

HRESULT edsSampleReleaseAll(void)
{
	HRESULT 				hResult = NO_ERROR;

	hResult = edsReleaseDescriptor(sampleSession.subunit_desc_ref, TRUE);
	
	return hResult;
}
#endif //_EDS

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

	Embedded Descriptor Services Server
	
	This file contains all the SERVER routines used in the EDS.
	

  Revisions:
		created 06/05/2003 KBJ/LM
****************************************************************************/

#ifdef _EDS

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCDebug.h"
#include "TCSemaphore.h"
#include "TCTasking.h"
#include "TCTaskingDefs.h"

#include "cr.h"
#include "systemcallbacks.h"
#include "eds.h"
#include "edstypes.h"
#include "drs.h"
#include "edsServer.h"
#include "edsWrite.h"
#include "edsRead.h"

#include "lal.h"
#include "fcp.h"
#include "avcDefs.h"
#include "avc.h"
#include "packetBlock.h"

#include "sbm.h"
#ifdef _CLI
#include "cli.h"
#include "edsCli.h"
#endif

#include "listmanager.h"
#include "1394defs.h"

static	char 						moduleName[] = __FILE__ ;

_TC_INLINE_STATIC BOOL isSubunitsEqual(AVC_ADDRESS_SUBUNIT subunit1, AVC_ADDRESS_SUBUNIT subunit2) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC BOOL isSubunitsEqual(AVC_ADDRESS_SUBUNIT subunit1, AVC_ADDRESS_SUBUNIT subunit2)
{
	return (subunit1.subunit_ID == subunit2.subunit_ID &&
			subunit1.subunit_type == subunit2.subunit_type);
}

char								EdsTempRead[512];

// task (thread) IDs for this module
uint32								edsHandlerThreadID;		// eds handler thread
const char*							edsHandlerThreadName = "edsHandlerThread";

uint32 								edsConnectionLockSemID;			

#ifdef _CLI
uint32								EdsShowParsedData=FALSE;
#endif

EDS_SERVER_CONNECTION				EdsConnectionList[EDS_SERVER_CONNECTION_COUNT];

static	EDS_SERVER_CONNECTION		EdsAttemptConnectList[EDS_SERVER_ATTEMPTCONNECTION_COUNT];  // These are connections which for some reason have failed, to be established.

static  uint32						EdsCurrentGeneration=0;

static	BYTE						EdsSessionListBuffer[LM_LISTSIZE(EDS_SERVER_SESSION_COUNT, sizeof(uint32))];

static	LM_CONTEXT*					EdsSessionList=(LM_CONTEXT*)EdsSessionListBuffer;

static	uint32						EdsEstablishedConnections=0;


HRESULT	edsBaseHandler(AVC_HEADER *pHeaderInfo, PB *packetBlock);						
HRESULT	edsAllocateConnection(EDS_SERVER_CONNECTION* init_data, 
							  EDS_SERVER_CONNECTION**   new_connection);
HRESULT	edsAlreadyConnected(EDS_SERVER_CONNECTION*		connection, 
							EDS_SERVER_CONNECTION** open_connection);
HRESULT	edsCheckForAccess(EDS_DESCRIPTOR_REF  descriptor);
HRESULT	edsLocateDescriptor(EDS_SERVER_CONNECTION*	connection);
HRESULT	edsReleaseConnection(EDS_SERVER_CONNECTION*		connection);
HRESULT	edsHandleOpenDescriptor(EDS_SERVER_CONNECTION*	connection);
HRESULT	edsPackOutgoingCommand(EDS_SERVER_CONNECTION* connection);
HRESULT	edsParseIncomingCommand(EDS_SERVER_CONNECTION*	connection);
HRESULT	edsLockAllDirectChildren(EDS_DESCRIPTOR_REF		descriptor, BOOL lock);
HRESULT	edsCreateDescriptorMap(EDS_SERVER_CONNECTION*	connection);
HRESULT	edsAddInfoBlockToMap(EDS_SERVER_CONNECTION* connection, 
							 EDS_DESCRIPTOR_REF  parent,
							 EDS_BUFFER_TYPE  buffertype);

static	CALLBACK_DESCRIPTOR	AVC_BASEHANDLER_CALLBACK[]=
{
	{CALLBACK_DESCRIPTOR_TYPE,			AVC_DESCRIPTORHANDLER_TYPE},
	{CALLBACK_DESCRIPTOR_NEXT,			0},
	{CALLBACK_DESCRIPTOR_NAME,			(uint32)"AVC_DESCRIPTORHANDLER_CALLBACK"},
	{AVC_DESCRIPTORHANDLER_CALLBACK_KEY,(uint32)&edsBaseHandler},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


HRESULT	edsCopyPayload(PB* pPacketBlock, void* buffer, uint32* sizeofbuffer)
{
	HRESULT hResult = NO_ERROR;
	uint32	maxcopysize=*sizeofbuffer;
	uint32	realsize=0;
	BYTE*	payloadbuffer=NULL;
	
	if (!pbIsValid(pPacketBlock))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	pbGetPayloadSize(pPacketBlock,&realsize);
	if (realsize < maxcopysize)
	{
		maxcopysize=realsize;
		*sizeofbuffer=realsize;	
	}
	
	pbGetPayload(pPacketBlock,(void **)&payloadbuffer);
	
	if (payloadbuffer != NULL)
	{
		memcpy(buffer,payloadbuffer,maxcopysize);
	}
	
	return hResult;	
}

/*
	This function is used to send a response to the descriptor commands.
	This uses the fcpBlockingSend command in order to accomplish this task.
*/
void edsSendResponse(EDS_SERVER_CONNECTION*	connection)
{
	uint32	responsesize=0;
	
	edsPackOutgoingCommand(connection);
	dsPadToEndOfQuadlet(&connection->response_ds,0xFF);
	responsesize=dsGetPosition(&connection->response_ds);						
	
	fcpBlockingSend(connection->node,
					(uint16)responsesize,
					(uint32*)connection->response,
					TRUE);
}

/*
	edsHandlerThread
	
	This is a thread which sleeps most of the time, but wakes up occasionally
	and closes connections which are either marked to be closed or have timed out.
*/
void edsHandlerThread(void *dummy)
{
	UNUSED_ARG(dummy);
		
	DO_FOREVER
	{
		if (NO_ERROR==TCSemaphoreWaitTimeout(edsConnectionLockSemID, 5))	// TC_NO_WAIT))
		{
			
			uint32					index;
			EDS_SERVER_CONNECTION*  attemptconnection=EdsAttemptConnectList;
			uint32					busGeneration = 0;
			
			/*
				Check the generation of the bus, if it has changed then
				we need to flush ALL open connections.
			*/
			lalGetBusGeneration(&busGeneration);
			if (busGeneration != EdsCurrentGeneration)
			{
				index = 0;
				while (index < EDS_SERVER_CONNECTION_COUNT)
				{	
					if(EdsConnectionList[index].in_use)
					{
						edsReleaseConnection(&EdsConnectionList[index]);
					}
					index++;
				}	
				
				EdsCurrentGeneration=busGeneration;			
			}
			
			index = 0;
			while (index < EDS_SERVER_CONNECTION_COUNT)
			{
					
				if(EdsConnectionList[index].in_use)
				{
					EDS_SERVER_CONNECTION*	cur_con=&EdsConnectionList[index];
				
					// Decrement the timeout counter and then check to see if the counter
					// is 0.
					cur_con->timeout_countdown--;
					
					if (!cur_con->busy)
					{
						if (cur_con->is_finished)
						{
							edsSendResponse(cur_con);
							cur_con->is_finished = FALSE;
						}	

						// Connections may have been marked closed by incoming commands. 
						// If so, we then close the connection here. Doing this here gets
						// rid of many complicated situations.
						if (cur_con->close)
						{
							edsReleaseConnection(cur_con);
							continue;	
						}
					}

					if (cur_con->timeout_countdown==0)
					{
						if (cur_con->opened_for_read)
						{
							edsLockAllDirectChildren(cur_con->toplevel,FALSE);
						}	
						edsReleaseConnection(cur_con);
						continue;
					}
				}
				index++;
			}
			
			// Make sure that attempted connections are cleaned up, and errors or responses
			// are sent as appropriate.
			
			index=0;
			while (index++ < EDS_SERVER_ATTEMPTCONNECTION_COUNT)
			{
				if (attemptconnection->in_use)
				{
					if (attemptconnection->is_finished)
					{
						edsSendResponse(attemptconnection);
					}	
				}
				attemptconnection->in_use=FALSE;
				attemptconnection++;
			}			
					

			TCSemaphoreSignal(edsConnectionLockSemID);	
		}
		
	TCTaskWait(EDS_CONNECTION_SWEEPER_SLEEP);
	}//DO_FOREVER
}


HRESULT	edsServerRegisterSession(EDS_CLIENT_SESSION*   session)
{
	return lmAddElement(EdsSessionList,&session,NULL);
}

/*

	HANDLER FUNCTIONS

	These functions handle the opening of connections and the reading/writing of 
	descriptors.
*/

HRESULT	edsBaseHandleResponse(AVC_HEADER *pHeader, PB *packetBlock)
{
	HRESULT	 				hResult = NO_ERROR;
	uint32	 				index = 0;
	uint32					subunitidcounter = 0;
	CALLBACK_DESCRIPTOR*	cb = NULL;
	AVC_SUBUNIT_CALLBACK	callback = NULL;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
	{
		edsCliPrintResponse(pHeader, packetBlock);
	}
#endif

	DO_FOREVER
	{
		hResult = crGetCallback(AVC_SUBUNIT_TYPE, index++, &cb);
		if (hResult != NO_ERROR) break;

		hResult = kvIsValue(cb, AVC_SUBUNIT_SUBUNITTYPE_KEY, pHeader->addrSubunit.subunit_type);
		if (hResult != NO_ERROR) continue;
		
		hResult = kvGetValue(cb, AVC_SUBUNIT_ID_KEY, &subunitidcounter);
		if (hResult != NO_ERROR) continue;

		if (subunitidcounter != AVC_SU_ID_IGNORE &&
			subunitidcounter != pHeader->addrSubunit.subunit_ID) continue;
			
		hResult = kvGetValue(cb, AVC_SUBUNIT_CALLBACK_KEY, (uint32 *) &callback);
		if (hResult != NO_ERROR) break;

		// found the appropriate callback so let's make it.
		hResult = (callback) (pHeader, packetBlock);
		break;
	}

	return hResult;
}

HRESULT	edsBaseHandler(AVC_HEADER *pHeaderInfo, PB *packetBlock)
{
	HRESULT					hResult = NO_ERROR;
	EDS_SERVER_CONNECTION*  attemptconnection=EdsAttemptConnectList;
	EDS_SERVER_CONNECTION*	realconnection=0;
	uint32					tempsize=0;
	BOOL					gotattemptconnection=FALSE;	
	uint32					attemptindex=0;
	
	if (avcCtypeIsResponse(pHeaderInfo->ctype))
	{
		return edsBaseHandleResponse(pHeaderInfo, packetBlock);
	}

	// KBJ??? not all descriptor commands are implemented
	switch (pHeaderInfo->opcode)
	{
		case AVC_CMD_SEARCH_DESCRIPTOR:
		case AVC_CMD_CREATE_DESCRIPTOR:
		case AVC_CMD_OBJECT_NUMBER_SELECT:
			return E_PKT_AVC_NOT_IMPLEMENTED;
	}

	hResult = TCSemaphoreWait(edsConnectionLockSemID);
	if (hResult != NO_ERROR) return hResult;

/*
	Find an empty "attempt" connection.  This gives us the ability to service more
	connections quickly. 
*/
	while(attemptindex++ < EDS_SERVER_ATTEMPTCONNECTION_COUNT)
	{
		if (attemptconnection->in_use)
		{
			attemptconnection++;
			continue;	
		}
		gotattemptconnection=TRUE;
		
		memset(attemptconnection,0,sizeof(*attemptconnection));
		attemptconnection->in_use=TRUE;
		break;
	}
	
	// This is the only case where we return an error to the AV/C Handler.
	if (FALSE==gotattemptconnection)
	{
		hResult = E_PKT_AVC_SUBUNIT_BUSY;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;	
	}

/*
	First thing we need to do is to populate the 
	tempconnection, and then determine if this connection
	already exists.
*/

	// Construct the basic portions of the connection, from the incoming packet.
	memcpy(&attemptconnection->avcheader,pHeaderInfo,sizeof(attemptconnection->avcheader));

	tempsize=sizeof(attemptconnection->iodata);
	edsCopyPayload(packetBlock,attemptconnection->iodata,&tempsize);
	dsOpenStream(&attemptconnection->iods,attemptconnection->iodata,tempsize,dsMODE_READ | dsMODE_PLATFORMENDIAN);
	dsOpenStream(&attemptconnection->response_ds,attemptconnection->response,sizeof(attemptconnection->response),dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
	pbGetNodeHandle(packetBlock,&attemptconnection->node);

	hResult=edsParseIncomingCommand(attemptconnection);

	if (hResult != NO_ERROR)
	{
		// Error in incomming command
		goto CleanUp; 
	}
	else
	{
		hResult=edsLocateDescriptor(attemptconnection);
#ifdef _CLI
		if (EdsShowParsedData)
		{
			sprintf(EdsTempRead,"TOPLEVEL DESCRIPTOR: id=%03d\n\r",attemptconnection->toplevel);
			sysDebugPrintf(EdsTempRead);
			if (attemptconnection->infoBlock)
			{
				sprintf(EdsTempRead,"INFOBLOCK DESCRIPTOR: id=%03d\n\r",attemptconnection->infoBlock);
				sysDebugPrintf(EdsTempRead);
			}
		}
#endif
	}
	
	/* If the controller node wants to know information about the status of a descriptor
	   then we do it here.  We can't do this further down in the HandleOpen, because we
	   need to be able to reply when a descriptor can't be found.
	*/
	if ( ((attemptconnection->avcheader.opcode == AVC_CMD_OPEN_DESCRIPTOR) ||
		  (attemptconnection->avcheader.opcode == AVC_CMD_OPEN_INFOBLOCK)) &&
		 ((attemptconnection->avcheader.ctype == AVC_CTYPE_STATUS) ||
		  (attemptconnection->avcheader.ctype == AVC_CTYPE_NOTIFY)) )

	{
		uint32	replystatus=AVC_DESC_STATUS_READY_FOR_OPEN;

		if (EdsEstablishedConnections <= EDS_SERVER_CONNECTION_COUNT)
		{
			replystatus=AVC_DESC_STATUS_AT_CAPACITY;
		}	
		else
		{	
			if (attemptconnection->toplevel==0)
			{
				replystatus=AVC_DESC_STATUS_NON_EXISTANT;
			}
			else if (attemptconnection->toplevel)
			{
				HRESULT	 tempresult=0;
				tempresult = edsCheckForAccess(attemptconnection->toplevel);
				if (NO_ERROR==tempresult)
				{
					replystatus=AVC_DESC_STATUS_READY_FOR_OPEN;	
				}
				else
				{
					replystatus=AVC_DESC_STATUS_WRITE_ENABLED_OPEN;	
				}			
			}	
		}
		attemptconnection->parsed_command.open.status=(uint8)replystatus;
		attemptconnection->avcheader.ctype=AVC_RESPONSE_STABLE;	
		if (attemptconnection->avcheader.ctype == AVC_CTYPE_NOTIFY)
			attemptconnection->avcheader.ctype=AVC_RESPONSE_INTERIM;	
		goto CleanUp;		
	}	


	if (hResult == NO_ERROR && 
		((attemptconnection->avcheader.opcode == AVC_CMD_OPEN_DESCRIPTOR) ||
		 (attemptconnection->avcheader.opcode == AVC_CMD_OPEN_INFOBLOCK)) )
	{
		// Check to see if the descriptor which is pointed to 
		// is already open, or if a parent of the descriptor is already
		// open, in an incompatible mode. (Write)
		if (attemptconnection->parsed_command.open.subfunction == AVC_DESC_SUBFUNCTION_CLOSE)
		{
			attemptconnection->accessmode=attemptconnection->parsed_command.open.subfunction;	
		}
		else
		{
			hResult=edsCheckForAccess(attemptconnection->toplevel);
			if (hResult == NO_ERROR)
			{
				attemptconnection->accessmode=attemptconnection->parsed_command.open.subfunction;	
			}	
		}
	}

	// Check to see if this connection has already been established.
	if (hResult == NO_ERROR)
	{
		hResult=edsAlreadyConnected(attemptconnection,&realconnection);

		if (hResult != NO_ERROR)
		{
			if ( 
				 ((attemptconnection->avcheader.opcode==AVC_CMD_OPEN_DESCRIPTOR) ||
				  (attemptconnection->avcheader.opcode==AVC_CMD_OPEN_INFOBLOCK)) &&
				 (attemptconnection->parsed_command.open.subfunction != AVC_DESC_SUBFUNCTION_CLOSE) 
			   )
			{
				// If open descriptor then create a new connection
				hResult=edsAllocateConnection(attemptconnection, &realconnection);			
				if (hResult == NO_ERROR && realconnection)
				{
					edsCreateDescriptorMap(realconnection);
				}
				else // We couldn't find an already established connection, and couldn't
					 // create a new connection either, we must be busy.
				{
					attemptconnection->is_finished=FALSE;
					attemptconnection->in_use=FALSE;
					hResult = E_PKT_AVC_SUBUNIT_BUSY;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;	
				}
			}
		}

	}
		
	if (hResult == NO_ERROR)
	{

		// Save Top level descriptor set by open command
		EDS_DESCRIPTOR_REF	toplevel = realconnection->toplevel;
		realconnection->busy=TRUE;	

		switch (realconnection->avcheader.opcode)
		{
			case AVC_CMD_OPEN_DESCRIPTOR:
			case AVC_CMD_OPEN_INFOBLOCK:
			{
				// Reset the DataStream, since it would still be pointing to the original
				// connection's data segment which won't exist.
				dsOpenStream(&realconnection->iods,realconnection->iodata,sizeof(realconnection->iodata),dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
				hResult=edsHandleOpenDescriptor(realconnection);
			}	
			break;
			
			case AVC_CMD_READ_INFOBLOCK:
				// If open with OPEN DESCRIPTOR, then different toplevel and info Block descriptors
				realconnection->toplevel = realconnection->infoBlock;
				realconnection->mapcount = 0;
				edsCreateDescriptorMap(realconnection);
			case AVC_CMD_READ_DESCRIPTOR:
			{
				// Reset the DataStream, since it would still be pointing to the original
				// connection's data segment which won't exist.
				dsOpenStream(&realconnection->iods,realconnection->iodata,sizeof(realconnection->iodata),dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
				hResult=edsHandleReadDescriptor(realconnection);
			}
			break;
			
			case AVC_CMD_WRITE_INFOBLOCK:
				// If open with OPEN DESCRIPTOR, then different toplevel and info Block descriptors
				realconnection->toplevel = realconnection->infoBlock;
				realconnection->mapcount = 0;
				edsCreateDescriptorMap(realconnection);
			case AVC_CMD_WRITE_DESCRIPTOR:
			{
				// Reset the DataStream, since it would still be pointing to the original
				// connection's data segment which won't exist.
				dsOpenStream(&realconnection->iods,realconnection->iodata,realconnection->iods.total_size,dsMODE_READ | dsMODE_PLATFORMENDIAN);
				hResult=edsHandleWriteDescriptor(realconnection);
			}
			break;
		}

		// Restore toplevel descriptor, if info block read/write
		realconnection->toplevel = toplevel;

		if (hResult == NO_ERROR)
		{
			// Mark that response has to be send, and attemptconnection is not used any more
			attemptconnection->in_use=FALSE;
			realconnection->is_finished=TRUE;
			realconnection->busy=FALSE;		
		}
	}

	
CleanUp:

	if (hResult != NO_ERROR)
	{
		attemptconnection->avcheader.ctype=AVC_RESPONSE_REJECTED;	
	}

	// Reply rejected if any error occured, no realconnection made
	attemptconnection->is_finished=TRUE;

	TCSemaphoreSignal(edsConnectionLockSemID);	
	
	return NO_ERROR;
}

HRESULT	edsHandleOpenDescriptor(EDS_SERVER_CONNECTION*	connection)
{
	HRESULT		hResult=NO_ERROR;
	
	switch (connection->avcheader.ctype)
	{
	
		case AVC_CTYPE_CONTROL:
		{
			switch (connection->accessmode)
			{
				case AVC_DESC_SUBFUNCTION_CLOSE:
				{
					connection->close=TRUE;  // Mark this connection for closure.				
					connection->avcheader.ctype=AVC_RESPONSE_ACCEPTED;
					if (connection->opened_for_read)
					{
						edsLockAllDirectChildren(connection->toplevel,FALSE);
					}	
				}	
				break;
				
				case AVC_DESC_SUBFUNCTION_READOPEN:
				{
					connection->avcheader.ctype=AVC_RESPONSE_ACCEPTED;
					connection->opened_for_read=TRUE;
					edsLockAllDirectChildren(connection->toplevel,TRUE);
					edsDoReadCallbackChildren(connection->toplevel);
				}
				break;
				
				case AVC_DESC_SUBFUNCTION_WRITEOPEN:
				{
					connection->avcheader.ctype=AVC_RESPONSE_ACCEPTED;
				}
				break;				
				
			}
		}
		break;
	}	
	
	return hResult;
}

/*
	UTILITY FUNCTIONS
*/

/*
	edsGetBufferTypeSize
	
	This calculates the size of all the children attached to the parent, at the specified
	buffer.  It returns the size of the buffer specified by the buffer type as well as the
	size of all the children attached to the specified buffertype.
*/
HRESULT	edsGetBufferTypeSize(EDS_DESCRIPTOR_REF descriptor, 
							EDS_BUFFER_TYPE  buffertype, 
							uint32* size)
{
	HRESULT				hResult=NO_ERROR;
	
	EDS_DESCRIPTOR*		pdata=NULL;
	uint32				totalsize=0;
	EDS_DESCRIPTOR_REF	cref=0;
	EDS_DESCRIPTOR*		cdata=NULL;
	uint32				cindex=1;

	
	hResult=drsLockDescriptor(descriptor);
	if (NO_ERROR==hResult)
	{
		hResult=drsGetDescriptor(descriptor,&pdata);	
	}
	
	/* 
		This is a bit different for each type of descriptor.
	*/
	switch (pdata->type)
	{
		case EDS_DESCRIPTORTYPE_LIST:
		{
			switch(buffertype)
			{
				case	EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
				{
					totalsize=pdata->u.list_descriptor.si_size;
				}
				break;	
			}
		}	
		break;
		
		case EDS_DESCRIPTORTYPE_ENTRY:
		{
			switch(buffertype)
			{
				case	EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
				{
					totalsize=pdata->u.entry_descriptor.si_size;
				}
				break;			
			}
		}
		break;
		
		case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
		{
			switch(buffertype)
			{
				case	EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
				case	EDS_BUFFERTYPE_SUBUNITINFO:
				{
					totalsize=pdata->u.subunit_identifier.si_length;
				}
				break;	
				
				case	EDS_BUFFERTYPE_MANUFACTURERINFO:
				{
					totalsize=pdata->u.subunit_identifier.mi_length;
				}
				break;		
			}
		}
		break;
		
		case EDS_DESCRIPTORTYPE_INFOBLOCK:
		{
			switch(buffertype)
			{
				case	EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO:
				{
					totalsize=pdata->u.info_block.primary_fields_length;
				}
				break;	
			}
		}
		break;
	}
	
			
	while (NO_ERROR==hResult && NO_ERROR==drsGetNthDescriptor(cindex++,&cref))
	{
		uint32	childsize=0;
		BOOL    is_appropriate=FALSE;
		
		drsLockDescriptor(cref);
		drsGetDescriptor(cref,&cdata);
			
		if (cdata->parent_reference==descriptor)
		{
			if (cdata->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
			{
				if (cdata->u.info_block.buffertype==buffertype)
				{
					is_appropriate=TRUE;	
				} 
			}
			if (cdata->type==EDS_DESCRIPTORTYPE_ENTRY)
			{
				if (EDS_BUFFERTYPE_EXTENDEDINFO==buffertype ||
					0==buffertype)
				{
					is_appropriate=TRUE;	
				}	
			}
			
			if (is_appropriate && !(pdata->type==EDS_DESCRIPTORTYPE_ENTRY && cdata->type==EDS_DESCRIPTORTYPE_LIST))
			{
				hResult=edsGetDescriptorSize(cref,TRUE,&childsize);
				if (NO_ERROR==hResult)
				{
					totalsize+=childsize;
				}
			}		
		}	
		drsUnlockDescriptor(cref);
	}//while	

	drsUnlockDescriptor(descriptor);	
	*size=totalsize;
	
	return hResult;
}


/*
	edsGetDescriptorSize
	
	This function determines the size of an entire descriptor, NOT INCLUDING
	an attached/embedded descriptors.
*/
HRESULT	edsGetDescriptorSize(EDS_DESCRIPTOR_REF	descriptor, 
							 BOOL include_children, 
							 uint32* size)
{
	HRESULT				hResult=NO_ERROR;
	uint32				mysize=0;
	EDS_DESCRIPTOR*		ddata=NULL;
	EDS_DESCRIPTOR*		sidata=NULL;
	EDS_DESCRIPTOR_REF	siref=0;
	
	hResult=drsLockDescriptor(descriptor);
	if (NO_ERROR !=hResult)
	{
		return hResult;		
	}
	hResult=drsGetDescriptor(descriptor,&ddata);
	if (hResult)
	{
		goto cleanup;	
	}
	
	// Get the subunit identifier descriptor, so that we
	// can get the sizes of fields in this descriptor.
	hResult=drsFindSubunitIdentifier(ddata->subunit, &siref);
	if (hResult)
	{
		goto cleanup;	
	}
	
	hResult=drsLockDescriptor(siref);
	if (hResult)
	{
		goto cleanup;	
	}

	drsGetDescriptor(siref,&sidata);

	// Build the size of the descriptor for each type of descriptor.
	if (NO_ERROR==hResult)
	{
			
		switch (ddata->type)
		{
			case EDS_DESCRIPTORTYPE_ENTRY:
			{
				EDS_DESCRIPTOR*			pdata=NULL;
				
				// Build the largest size possible. Then subtract optional
				// fields if they do not exist.
				mysize=4+sidata->u.subunit_identifier.size_of_list_id+
					   sidata->u.subunit_identifier.size_of_object_id;
				
				// Remove the child_list_id from the size, if we don't use it.	   
				if (ddata->u.entry_descriptor.child_list_reference==0)
				{
					mysize-=2; 	
				}
				
				// Remove the object id from the size if we don't use it.
				// In order to do this, we must look up the parent descriptor
				// and check an attribute bit to determine if this field is used.
				if (NO_ERROR==drsLockDescriptor(ddata->parent_reference))
				{
					drsGetDescriptor(ddata->parent_reference,&pdata);
					
					// If bit 4 is set then the object id is used, otherwise it 
					// is not
					if (!(pdata->u.list_descriptor.attributes & AVC_DESC_ATTRIB_OBJECT_ID))
					{
						mysize-=sidata->u.subunit_identifier.size_of_object_id;	
					}
					
					drsUnlockDescriptor(ddata->parent_reference);	
				}
				else
				{
					hResult=E_EDS_UNABLE_TO_DETERMINE_SIZE;	
					sysLogError(hResult, __LINE__, moduleName);
				}
				
				//
				//	Add in the specific entry buffersize and si_size;
				//
				mysize+=2+ddata->u.entry_descriptor.si_size;
				
			}
			break;
			
			case EDS_DESCRIPTORTYPE_LIST:
			{
				mysize=6+ddata->u.list_descriptor.si_size+2;
			}
			break;
			
			case EDS_DESCRIPTORTYPE_OPAQUE:
			{
				mysize=ddata->u.opaque_descriptor.si_size+2;
			}
			break;

			case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
			{
				mysize=8+(ddata->u.subunit_identifier.size_of_list_id * ddata->u.subunit_identifier.number_of_root_lists)
					   +2+ddata->u.subunit_identifier.si_length+2+ddata->u.subunit_identifier.mi_length;
			}
			break;
			
			case EDS_DESCRIPTORTYPE_INFOBLOCK:
			{
				mysize=EDS_INFOBLOCK_HEADER_SIZE+(ddata->u.info_block.primary_fields_length);
			}
			break;
			
			default:
				hResult = E_EDS_UNKNOWN_DESCRIPTOR;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;	
			
		}//switch		
		
		/*
			Search for all the descriptors in the DRS which use this as a parent, then 
			include the size of each child.
		*/
		if (include_children)
		{
			EDS_DESCRIPTOR_REF	cref=0;
			EDS_DESCRIPTOR*		cdata=NULL;
			uint32				cindex=1;
			
			while (NO_ERROR==hResult && NO_ERROR==drsGetNthDescriptor(cindex++,&cref))
			{
				uint32	childsize=0;
				
				drsLockDescriptor(cref);
				drsGetDescriptor(cref,&cdata);
					
				if (cdata->parent_reference==descriptor)
				{
					// We never want lists to show up as child counts.  
					// This is because lists are always referenced, and not included
					// in their entirety, in the parent descriptor.
					if (!(cdata->type==EDS_DESCRIPTORTYPE_LIST))
					{	
						hResult=edsGetDescriptorSize(cref,TRUE,&childsize);
						if (NO_ERROR==hResult)
						{
							mysize+=childsize;
						}
					}		
				}	
				drsUnlockDescriptor(cref);
			}//while	
		}// if
	}
	
	*size=mysize;
	
cleanup:
	drsUnlockDescriptor(descriptor);
	drsUnlockDescriptor(siref);
	
	return hResult;
}

/*
	edsLockAllDirectChildren
	
	This function locks/unlocks all child descriptors which are physically
	attached to this descriptor.
*/

HRESULT	edsLockAllDirectChildren(EDS_DESCRIPTOR_REF	descriptor, BOOL lock)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		ddata=NULL;
	
	if (lock)
	{
		hResult=drsLockDescriptor(descriptor);
		if (NO_ERROR !=hResult)	return hResult;		
	}

	/*
		Search for all the descriptors in the DRS which use this as a parent, then 
		include the size of each child.
	*/				
	hResult=drsGetDescriptor(descriptor,&ddata);
	if (NO_ERROR == hResult)
	{
		EDS_DESCRIPTOR_REF	cref=0;
		EDS_DESCRIPTOR*		cdata=NULL;
		uint32				cindex=1;
		
		while (NO_ERROR==hResult && NO_ERROR==drsGetNthDescriptor(cindex++,&cref))
		{
			drsLockDescriptor(cref);
			drsGetDescriptor(cref,&cdata);
				
			if (cdata->parent_reference==descriptor)
			{
				if (cdata->type!=EDS_DESCRIPTORTYPE_LIST)
				{
					edsLockAllDirectChildren(cref,lock);
				}
			}	
			drsUnlockDescriptor(cref);
		}//while	
	}// if
	
	if (!lock)
	{
		hResult=drsUnlockDescriptor(descriptor);	
	}

	return hResult;
}


/*
	edsParseOutgoingCommand
	
	This function parses out the different portions of the incoming descriptor
	commands. 
*/
HRESULT	edsParseIncomingCommand(EDS_SERVER_CONNECTION*	connection)
{
	HRESULT				hResult=NO_ERROR;
	DataStream*			ds=NULL;

#ifdef _CLI
	char			temp[120];
#endif
	//
	//	Take the Packet specified, and get the descriptor specifier portion of the packet.
	//
	hResult=dssCreateFromStream (&connection->parsed_address, &connection->iods);
	
	// If an error occurred try to use the custom resolver, if there is one.
	if (NO_ERROR!=hResult)
	{
		uint32				index=0;
		uint32				counter=0;
		EDS_CLIENT_SESSION*	session=NULL;
		
		ds=&connection->iods;
	
		//
		//	Okay we now should look for a custom resolver for this
		//  descriptor specifier. 
		//
		
		while (NO_ERROR==lmGetNthElement(EdsSessionList,(void**)&session,counter++,&index))
		{
			if  (isSubunitsEqual((AVC_ADDRESS_SUBUNIT )(*(EDS_CLIENT_SESSION **)session)->subunit, connection->avcheader.addrSubunit) &&
				 (*(EDS_CLIENT_SESSION **)session)->custom_resolver)
			{
				EDS_CUSTOM_RESOLVER 	cb=(EDS_CUSTOM_RESOLVER)(*(EDS_CLIENT_SESSION **)session)->custom_resolver;
				
				hResult=(cb)(ds,&connection->toplevel);	
			}
			lmReleaseElement(EdsSessionList,index);
		}
		
		if (NO_ERROR != hResult)
		{
			hResult=E_EDS_BAD_SPECIFIER_TYPE;
// bkbk: not always an error, refine this
//			sysLogError(hResult, __LINE__, moduleName);
		}	
	}
	
	if (NO_ERROR==hResult)
	{
		ds=&connection->iods;	
	}
		
	if (NO_ERROR==hResult)
	{
		// Get the subunitidentifier, and store it here, so that we don't have to do
		// this again and again later.
		hResult=drsFindSubunitIdentifier(connection->avcheader.addrSubunit, &connection->subunitidentifier_ref);

#ifdef _CLI
		if (EdsShowParsedData)
		{
			sprintf(temp,">>> EDS incoming packet\n\rAVC HEADER:\n\r  ctype=%d  subunit_id=%d  subunit_type=%d  opcode=%d\n\r", 
					connection->avcheader.ctype, connection->avcheader.addrSubunit.subunit_ID, connection->avcheader.addrSubunit.subunit_type,
					connection->avcheader.opcode);
			sysDebugPrintf(temp);
		}	
#endif						    
	}
	
	if (NO_ERROR==hResult)
	{
		switch(connection->avcheader.opcode)
		{
			case AVC_CMD_OPEN_DESCRIPTOR:
			case AVC_CMD_OPEN_INFOBLOCK:
			{
				
				switch(connection->avcheader.ctype)
				{	
					case AVC_CTYPE_CONTROL:
					{
						dsRead8Bits(ds,&connection->parsed_command.open.subfunction);
						dsRead8Bits(ds,&connection->parsed_command.open.reserved);
#ifdef _CLI
						if (EdsShowParsedData)
						{
							sprintf(temp,"OPEN COMMAND HEADER (CONTROL)\n\r  subfunction=%d  reserved=%d\n\r",
									connection->parsed_command.open.subfunction,
									connection->parsed_command.open.reserved);
							sysDebugPrintf(temp);
						}
#endif
					}
					break;
					
					case AVC_CTYPE_STATUS:
					case AVC_CTYPE_NOTIFY:
					{
						dsRead8Bits(ds,&connection->parsed_command.open.status);
						dsRead8Bits(ds,&connection->parsed_command.open.reserved);
						dsReadBits(ds,16,&connection->parsed_command.open.node_id);

#ifdef _CLI						
						if (EdsShowParsedData)
						{
							sprintf(temp,"OPEN COMMAND HEADER (STATUS/NOTIFY)\n\r  subfunction=%d  reserved=%d  node_id=%d\n\r",
									connection->parsed_command.open.subfunction,
									connection->parsed_command.open.reserved,
									connection->parsed_command.open.node_id);
							sysDebugPrintf(temp);
						}	
#endif
					}
					break;
					
					default:
					{
						hResult=E_PKT_AVC_REJECTED;
					}
					break;
										
				} //switch
				
				connection->avc_headersize=(uint8)dsGetPosition(ds);
				
			} //case 
			break;	
			
			case AVC_CMD_READ_DESCRIPTOR:
			case AVC_CMD_READ_INFOBLOCK:
			{
				switch(connection->avcheader.ctype)
				{
					case AVC_CTYPE_CONTROL:
					{

						dsRead8Bits(ds,&connection->parsed_command.read.read_result_status);
						dsRead8Bits(ds,&connection->parsed_command.read.reserved);
						dsRead16Bits(ds,&connection->parsed_command.read.data_length);
						dsRead16Bits(ds,&connection->parsed_command.read.address);
#ifdef _CLI
						if (EdsShowParsedData)
						{
							sprintf(temp,"READ COMMAND HEADER (CONTROL)\n\r  read_result_status=%d  reserved=%d  data_length=%d  address=%d\n\r",
									connection->parsed_command.read.read_result_status,
									connection->parsed_command.read.reserved,
									connection->parsed_command.read.data_length,
									connection->parsed_command.read.address);
							sysDebugPrintf(temp);
						}
#endif

//						if (connection->parsed_command.read.data_length)
//						{
//							dsTrimSize(&connection->iods,connection->parsed_command.read.data_length);	
//						}
//						else	// When data_length is set to 0
						{
							uint32	nodeid=0;
							uint16	remote_maxsize=0;
							uint16	local_maxsize=0;
							uint16	maxsize=0;
							
							// Get the maxsize of 1394 packets for both the
							// remote and local node, then determine the maxium
							// number of bytes to allow for transfer.
							lalGetNodeAddrFromHandle(connection->node,&nodeid);
							lalGetMaxRecordSizeBytes(nodeid,&remote_maxsize);
							lalGetThisNodeAddr(&nodeid);
							lalGetMaxRecordSizeBytes(nodeid,&local_maxsize);
							
							// bk: TODO this is a workaround for missing max_rec for sender
							if (remote_maxsize == 0) remote_maxsize = EDS_MAX_DESCRIPTOR_DATASIZE;

							maxsize = (remote_maxsize>local_maxsize) ? local_maxsize : remote_maxsize;

							// large payloads create response delay, causing host retries
							// trim size here for responsiveness
							if (maxsize > EDS_MAX_DESCRIPTOR_DATASIZE)
							{
								maxsize = EDS_MAX_DESCRIPTOR_DATASIZE;
							}

							// Remote 4 quadlets of data from the size. This
							// is the maximum number of bytes that a 1394 packet
							// will ever occupy.
							maxsize-=6*4;
							
							// remove the current number of bytes that we have parsed
							// in the incoming packet.  This is the header for the 
							// read response packet.
							maxsize = (uint16)(maxsize - dsGetPosition(ds));
							
							// Now set the size to whatever maxsize is.  This the maximum
							// number of bytes we may send back to the unit requesting
							// data.
							
							dsTrimSize(&connection->iods,maxsize);
							
							connection->parsed_command.read.data_length=maxsize;
						}
												
					}
					break;
					
					default:
					{
						hResult = E_PKT_AVC_REJECTED;
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
				}
								
				connection->avc_headersize=(uint8)dsGetPosition(ds);
			}
			break;
			
			case AVC_CMD_WRITE_DESCRIPTOR:
			case AVC_CMD_WRITE_INFOBLOCK:
			{
				switch(connection->avcheader.ctype)
				{
					case AVC_CTYPE_CONTROL:
					{
						
						dsRead8Bits(ds,&connection->parsed_command.write.subfunction);
						dsRead8Bits(ds,&connection->parsed_command.write.grouptag);
						dsRead16Bits(ds,&connection->parsed_command.write.data_length);
						dsRead16Bits(ds,&connection->parsed_command.write.address);
						
						/*
							When the descriptor write is a write of the descriptor with
							a partial_replace command, we must read an extra 2 bytes to
							acquire the original data length,  after the original data length, 
							the replacement datalength follows.
						*/
						if (connection->parsed_command.write.subfunction==AVC_DESC_SUBFUNCTION_PARTIAL_REPLACE)
						{
							connection->parsed_command.write.replacement_data_length=\
								connection->parsed_command.write.data_length;
							dsRead16Bits(ds,&connection->parsed_command.write.original_data_length);	

#ifdef _CLI
							if (EdsShowParsedData)
							{
								sprintf(temp,"WRITE COMMAND HEADER (CONTROL)\n\r  subfunction=%d  grouptag=%d  replace_data_len=%d  address=%d  orig_data_len=%d\n\r",
										connection->parsed_command.write.subfunction,
										connection->parsed_command.write.grouptag,
										connection->parsed_command.write.replacement_data_length,
										connection->parsed_command.write.address,
										connection->parsed_command.write.original_data_length);
								sysDebugPrintf(temp);
							}
#endif
						}
						else
						{
#ifdef _CLI
							if (EdsShowParsedData)
							{
								sprintf(temp,"WRITE COMMAND HEADER (CONTROL)\n\r  subfunction=%d  grouptag=%d  data_len=%d  address=%d\n\r",
										connection->parsed_command.write.subfunction,
										connection->parsed_command.write.grouptag,
										connection->parsed_command.write.data_length,
										connection->parsed_command.write.address);
								sysDebugPrintf(temp);
							}
#endif
						}
		
						dsReadChunk(ds,connection->parsed_command.write.data_length,
									(char*)connection->iodata);
									
						dsReset(ds);
						
						//	Set the input stream size to something which matches the
						//  write data_length.
						dsTrimSize(&connection->iods,connection->parsed_command.write.data_length);
						dsSwitchMode(&connection->iods,dsMODE_READ);
												
					}
					break;	
					
					default:
					{
						hResult = E_PKT_AVC_REJECTED;
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
				}
				
				connection->avc_headersize=(uint8)dsGetPosition(ds);
			}
			break;
		}
	}	
	
	return hResult;
}
/*
	edsPackOutgoingCommand
	
	This function packs the packet for the outgoing RESPONSE.
	
	It currently requires a valid request packet, otherwise it won't work.
*/

HRESULT	edsPackOutgoingCommand(EDS_SERVER_CONNECTION* connection)
{
	HRESULT			hResult=NO_ERROR;
	DataStream*		ds=NULL;
	
	ds=&connection->response_ds;
	
	if (NO_ERROR==hResult)
	{
		dsReset(ds);
		hResult=avcEncodeHeader(ds,&connection->avcheader);
	}
	
	if (NO_ERROR==hResult)
	{
		uint32	size=0;
		hResult=dssGetDataSize(&connection->parsed_address, &size);
		
		dsWriteChunk(ds,size,connection->parsed_address.data);
	}
	
	if (NO_ERROR==hResult)
	{
		switch(connection->avcheader.opcode)
		{
			case AVC_CMD_OPEN_DESCRIPTOR:
			case AVC_CMD_OPEN_INFOBLOCK:
			{
				switch(connection->avcheader.ctype)
				{	
					
					case AVC_RESPONSE_STABLE:
					case AVC_RESPONSE_INTERIM:
					{
						dsWrite8Bits(ds,connection->parsed_command.open.status);
						dsWrite8Bits(ds,connection->parsed_command.open.reserved);
						dsWrite16Bits(ds,(uint16)connection->parsed_command.open.node_id);	
					}
					break;
					
					default:
					{
						dsWrite8Bits(ds,connection->parsed_command.open.subfunction);
						dsWrite8Bits(ds,connection->parsed_command.open.reserved);
					}
					break;
										
				} //switch
			} //case 
			break;	
			
			case AVC_CMD_READ_DESCRIPTOR:
			case AVC_CMD_READ_INFOBLOCK:
			{
				switch(connection->avcheader.ctype)
				{
					case AVC_RESPONSE_ACCEPTED:
					{
						dsWrite8Bits(ds,connection->parsed_command.read.read_result_status);
						dsWrite8Bits(ds,connection->parsed_command.read.reserved);
						dsWrite16Bits(ds,connection->parsed_command.read.data_length);
						dsWrite16Bits(ds,connection->parsed_command.read.address);

						if (EDS_MAX_DESCRIPTOR_DATASIZE <= connection->parsed_command.read.data_length)
						{
							hResult=E_PKT_AVC_REJECTED;	
							sysLogError(hResult, __LINE__, moduleName);
						}
												
						if (NO_ERROR==hResult)
						{
							// Write the data from the iods datastream, to the new packet's datastream.
							dsWriteChunk(ds,dsGetPosition(&connection->iods),(char*)connection->iodata);
						}
					}
					break;
					
					default:
					{
						hResult=E_PKT_AVC_REJECTED;
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
				}
			}
			break;
			
			case AVC_CMD_WRITE_DESCRIPTOR:
			case AVC_CMD_WRITE_INFOBLOCK:
			{
				switch(connection->avcheader.ctype)
				{
					case AVC_RESPONSE_ACCEPTED:
					{
						dsWrite8Bits(ds,connection->parsed_command.write.subfunction);
						dsWrite8Bits(ds,connection->parsed_command.write.grouptag);
						dsWrite16Bits(ds,connection->parsed_command.write.data_length);
						dsWrite16Bits(ds,connection->parsed_command.write.address);

						/*
							When the descriptor write is a write of the descriptor with
							a partial_replace command, we must read an extra 2 bytes to
							acquire the original data length,  after the original data length, 
							the replacement datalength follows.
						*/
						if (connection->parsed_command.write.subfunction==AVC_DESC_SUBFUNCTION_PARTIAL_REPLACE)
						{
							dsWrite16Bits(ds,connection->parsed_command.write.original_data_length);	
						}
					}
					break;	
					
					default:
					{
						hResult=E_PKT_AVC_REJECTED;
						sysLogError(hResult, __LINE__, moduleName);
					}
					break;
				}
			}
			break;
		}
	}	
	
	return hResult;
}


/*
	edsAllocateConnection
	
	This finds an empty connection in the connection list and allocates it.
*/

HRESULT	edsAllocateConnection(EDS_SERVER_CONNECTION* init_data, 
							  EDS_SERVER_CONNECTION** new_connection)
{
	HRESULT		hResult=NO_ERROR;
	uint32		index=0;
	BOOL		found=FALSE;
	
	while (index < EDS_SERVER_CONNECTION_COUNT && found==FALSE)
	{
		if(EdsConnectionList[index].in_use==FALSE)
		{
			*new_connection=&EdsConnectionList[index];
			// Copy the initialization data.
			memcpy(*new_connection,init_data,sizeof(EDS_SERVER_CONNECTION));
			EdsConnectionList[index].in_use=TRUE;
			// Reset the DataStream, since it would still be pointing to the original
			// connection's data segment which won't exist.
				
			dsOpenStream(&((**new_connection).iods),(**new_connection).iodata,sizeof((**new_connection).iodata),dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
			dsOpenStream(&((**new_connection).response_ds),(**new_connection).response,sizeof((**new_connection).response),dsMODE_WRITE | dsMODE_PLATFORMENDIAN);

			EdsEstablishedConnections++;
			(*new_connection)->timeout_countdown=EDS_CONNECTION_TIMEOUT;
			found=TRUE;
		}
		index++;
	}
	if (!found)
	{
		hResult=E_EDS_OUT_OF_CONNECTION_SPACE;	
		sysLogError(hResult, __LINE__, moduleName);
	}
	
	return hResult;	
}	

/*
	edsReleaseConnection
	
	This returns the allocated connection to the connection list.
*/
HRESULT	edsReleaseConnection(EDS_SERVER_CONNECTION*	connection)
{
	HRESULT		hResult=NO_ERROR;
	
	if (connection)
	{	
		EdsEstablishedConnections--;
		connection->in_use=FALSE;
	}
			
	return hResult;	
}

/*
	edsAlreadyConnected
	
	This allows the caller to determine if this connection has already been opened
	by the same owner. If so then another connection would not allocated.  The open_connection
	field is the response.
	
	The unique fields are determined by the node, and the toplevel address  fields of the connection.
*/
HRESULT	edsAlreadyConnected(EDS_SERVER_CONNECTION*	connection, 
							EDS_SERVER_CONNECTION** open_connection)
{
	HRESULT		hResult=NO_ERROR;
	uint32		index=0;
	BOOL		found=FALSE;
	
	while (index < EDS_SERVER_CONNECTION_COUNT)
	{
		if(EdsConnectionList[index].in_use)
		{
			EDS_SERVER_CONNECTION*	cur_con=&EdsConnectionList[index];

			if (cur_con->node==connection->node &&
				cur_con->toplevel==connection->toplevel && 
				(!cur_con->close))
			{
				// Since the connection is really just a way to keep state information
				// for the current instance of this connection, we make it easy on 
				// ourselves and marely copy all the data from the incoming connection.
				
				// We have to copy the map though,  otherwise we won't create another
				// one.
				memcpy(connection->map, cur_con->map, sizeof(cur_con->map));
				connection->mapcount = cur_con->mapcount;

				// We have to copy the info. from current connection during open
				connection->opened_for_read = cur_con->opened_for_read;
				if ( !((connection->avcheader.opcode == AVC_CMD_OPEN_DESCRIPTOR) ||
					   (connection->avcheader.opcode == AVC_CMD_OPEN_INFOBLOCK)) )

				{
					// When not an open command then use access mode from current connection
					connection->accessmode = cur_con->accessmode;
				}
				
				// Copy info. from connection to current connection
				memcpy(cur_con,connection,sizeof(EDS_SERVER_CONNECTION));
				
				dsOpenStream(&cur_con->response_ds,cur_con->response,sizeof(cur_con->response),dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
																 
				cur_con->timeout_countdown=EDS_CONNECTION_TIMEOUT;		
				*open_connection=cur_con;
				found=TRUE;	
				break;
			}
		}
		index++;
	}
	if (!found)
	{
		hResult=E_EDS_NOT_CONNECTED;	
	}
		
	return hResult;	
}

HRESULT	edsCheckForAccess(EDS_DESCRIPTOR_REF  descriptor)
{
	HRESULT				hResult=NO_ERROR;
	uint32				index=0;
	BOOL				found=FALSE;
	EDS_DESCRIPTOR*		ddata=0;
	EDS_DESCRIPTOR_REF	pref=0;
	
	if (descriptor==0)
	{
		return NO_ERROR;
	}
	
	while (index < EDS_SERVER_CONNECTION_COUNT && !found)
	{
		if(EdsConnectionList[index].in_use)
		{
			EDS_SERVER_CONNECTION*	cur_con=&EdsConnectionList[index];
			if (cur_con->toplevel==descriptor)
			{
				if (cur_con->accessmode==AVC_DESC_SUBFUNCTION_WRITEOPEN)
				{
					hResult=E_EDS_ALREADY_OPEN_FOR_WRITE;
					found=TRUE;
					break;	
				}
			}
		}
		index++;
	}

	if (!found)
	{
		drsLockDescriptor(descriptor);
		drsGetDescriptor(descriptor,&ddata);
		pref=ddata->parent_reference;
		// if the current descriptor's type is a list, then we don't end up needing 
		// to go back any further, as the references to these descriptors are 
		// always indirect.
		if (ddata->type==EDS_DESCRIPTORTYPE_LIST ||
			ddata->type==EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER)
		{
			hResult=NO_ERROR;
		}
		else
		{
			hResult=edsCheckForAccess(pref);
		}
		drsUnlockDescriptor(descriptor);	
	}
	
	return hResult;	
}


/*
	edsLocateDescriptor
	
	This is the function responsible for translating the DSS adddress into a descriptor reference.
	This done primarily by using the DRS Find function.
*/
HRESULT	edsLocateDescriptor(EDS_SERVER_CONNECTION*	connection)
{
	HRESULT						hResult=NO_ERROR;
	EDS_DESCRIPTOR_REF			si_ref=0;
	uint32						index=0;
	DSS_DESCRIPTOR_SPECIFIER	specifier;
	EDS_DESCRIPTOR_REF			cref=0;
	EDS_DESCRIPTOR_REF			ibref=0;
	KEYVALUE					searchargs[10];
	
	
	memset(&specifier, 0, sizeof(specifier));

	// Get the subunit identifier 
	hResult=drsFindSubunitIdentifier(connection->avcheader.addrSubunit, &si_ref);

#ifdef _CLI	
	if (EdsShowParsedData)
	{
		sprintf(EdsTempRead,"DESCRIPTOR SPECIFIER\n\r");
		sysDebugPrintf(EdsTempRead);
	}
#endif
	
	while (NO_ERROR==hResult)
	{
		hResult=kvCreate(searchargs,ARRAYSIZE(searchargs));
		kvSetValue(searchargs,DRS_FINDKEY_BY_SUBUNITID,connection->avcheader.addrSubunit.subunit_ID);
		kvSetValue(searchargs,DRS_FINDKEY_BY_SUBUNITTYPE,connection->avcheader.addrSubunit.subunit_type);

		/*
			Now determine the toplevel descriptor specified by the incoming address.
		*/
		if (NO_ERROR==hResult)
		{		
			hResult=dssGetNthElement(&connection->parsed_address, index++, &specifier);
			if (NO_ERROR != hResult)
			{
				hResult=NO_ERROR;
				break;	
			}
		}
		
		if (ibref) // ibref already set once, when specifier level > 2
			cref = ibref;

		if (NO_ERROR==hResult)
		{
			switch(specifier.type)
			{
				case AVC_DS_SUBUNIT_IDENTIFIER:
				{
#ifdef _CLI
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND SUBUNIT IDENTIFIER\n\r");
						sysDebugPrintf(EdsTempRead);
					}
#endif
					cref=si_ref;
				}
				break;	
				
				case AVC_DS_SUBUNIT_STATUS_DESCRIPTOR:
				{
#ifdef _CLI
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND SUBUNIT STATUS DESCRIPTOR\n\r");
						sysDebugPrintf(EdsTempRead);
					}
#endif
					// Descriptor is an opaque type
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_OPAQUE);
					hResult=drsFindDescriptor(si_ref,searchargs,&cref);
				}
				break;	
				
				case AVC_DS_LIST_ID:
				{
					// Find the list id in the root list.
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_LIST);
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTID,specifier.list_id);
#ifdef _CLI					
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND LIST BY ID:\n\r    list_id=0x%08X\n\r",specifier.list_id);
						sysDebugPrintf(EdsTempRead);
					}
#endif
					hResult=drsFindDescriptor(0,searchargs,&cref);
				}
				break;
				
				case AVC_DS_LIST_TYPE:
				{
					// Find the list type in the root list.
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_LIST);
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTTYPE, specifier.list_type);
#ifdef _CLI
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND LIST BY TYPE:\n\r    list_type=0x%02X\n\r",specifier.list_type);
						sysDebugPrintf(EdsTempRead);
					}
#endif
					hResult=drsFindDescriptor(0,searchargs,&cref);
				}
				break;
				
				case AVC_DS_ENTRY_POSITION:
				{
					// First find the parent list containing the specified list id
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_LIST);
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTID,specifier.list_id);
#ifdef _CLI					
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND ENTRY BY POSITION:\n\r   list_id=0x%08X  entry_position=%d\n\r",specifier.list_id,specifier.entry_position);
						sysDebugPrintf(EdsTempRead);
					}
#endif		
					hResult=drsFindDescriptor(0,searchargs,&cref);
					if (NO_ERROR==hResult)
					{
						// Now look for the specified entry by position in the list descriptor.
						kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_ENTRY);	
						kvSetValue(searchargs,DRS_FINDKEY_BY_POSITION, specifier.entry_position);
						hResult=drsFindDescriptor(cref,searchargs,&cref);
					}
				}
				break;
				case AVC_DS_ENTRY_OBJECT_ID:
				{
					// First find the list id and type in the root list.
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_LIST);
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTID,specifier.list_id);
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTTYPE,specifier.list_type);
#ifdef _CLI
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND ENTRY BY OBJECT ID:\n\r   list_id=0x%08X  list_type=0x%02X  entry_objectid=0x%08X\n\r",
								specifier.list_id,specifier.list_type,specifier.object_id);
						sysDebugPrintf(EdsTempRead);
					}
#endif		
					hResult=drsFindDescriptor(0,searchargs,&cref);
					if (NO_ERROR==hResult)
					{
						// Now look for the specified entry by object id in the list descriptor.
						kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_ENTRY);	
						kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTID,specifier.object_id);
						hResult=drsFindDescriptor(cref,searchargs,&cref);
					}

				}
				break;
				/*KBJ??? only used for descriptor create 
				case AVC_DS_ENTRY_TYPE:
				{
					// Find the first entry type in the root list.
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_ENTRY);	
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTTYPE, specifier.entry_type);
#ifdef _CLI
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND ENTRY BY TYPE:\n\r   entry_type=0x%02X\n\r",
								specifier.entry_type);
						sysDebugPrintf(EdsTempRead);
					}
#endif		
					hResult=drsFindDescriptor(0,searchargs,&cref);
				}
				break;
				*/
				case AVC_DS_OBJECT_ID:
				{
					// Find the first entry object id starting from the root list.
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_ENTRY);	
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTID,specifier.object_id);
#ifdef _CLI
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND BY OBJECT ID:\n\r   entry_objectid=0x%08X\n\r",
								specifier.object_id);
						sysDebugPrintf(EdsTempRead);
					}
#endif		
					hResult=drsFindDescriptor(0,searchargs,&cref);
				}
				break;
				
				// infoblocks must already have a parent.
				case AVC_DS_INFOBLOCK_INSTANCE:
				{
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_INFOBLOCK);
					kvSetValue(searchargs,DRS_FINDKEY_BY_OBJECTTYPE,specifier.info_block_type);
					kvSetValue(searchargs,DRS_FINDKEY_BY_POSITION,specifier.info_block_instance_count);
#ifdef _CLI					
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND INFOBLOCK BY INSTANCE:\n\r   info_block_type=0x%04X  info_block_instance_count=%d\n\r",
								specifier.info_block_type,specifier.info_block_instance_count);
						sysDebugPrintf(EdsTempRead);
					}
#endif		
					hResult=drsFindDescriptor(cref,searchargs,&ibref);
				}
				break;
				
				case AVC_DS_INFOBLOCK_POSITION:
				{
					kvSetValue(searchargs,DRS_FINDKEY_EDS_TYPE,EDS_DESCRIPTORTYPE_INFOBLOCK);
					kvSetValue(searchargs,DRS_FINDKEY_BY_POSITION,specifier.info_block_position);
#ifdef _CLI					
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  FIND INFOBLOCK BY POSITION:\n\r   info_block_position=%d\n\r",
								specifier.info_block_position);
						sysDebugPrintf(EdsTempRead);
					}
#endif					
					hResult=drsFindDescriptor(cref,searchargs,&ibref);
				}
				break;
				
				default:
				{
					uint32				index=0;
					uint32				counter=0;
					EDS_CLIENT_SESSION* session=0;
					DataStream			ds;
#ifdef _CLI
					if (EdsShowParsedData)
					{
						sprintf(EdsTempRead,"  BAD DESCRIPTOR SPECIFIER TYPE == 0x%02x\n\r",
								specifier.type);
						sysDebugPrintf(EdsTempRead);
					}
#endif
					//
					//	Okay we now should look for a custom resolver for this
					//  descriptor specifier. 
					//
					dsOpenStream(&ds,connection->parsed_address.data,sizeof(connection->parsed_address.data),dsMODE_READ | dsMODE_PLATFORMENDIAN);
					
					while (NO_ERROR==lmGetNthElement(EdsSessionList,(void**)&session,counter++,&index))
					{
						if  (isSubunitsEqual((*(EDS_CLIENT_SESSION **)session)->subunit, connection->avcheader.addrSubunit) &&
							 (*(EDS_CLIENT_SESSION **)session)->custom_resolver)
						{
							EDS_CUSTOM_RESOLVER 	cb=(EDS_CUSTOM_RESOLVER)(*(EDS_CLIENT_SESSION **)session)->custom_resolver;
							
							hResult=(*cb)(&ds,&connection->toplevel);	
						}
						lmReleaseElement(EdsSessionList,index);
					}
					
					if (NO_ERROR != hResult)
					{
						hResult=E_EDS_BAD_SPECIFIER_TYPE;
						sysLogError(hResult, __LINE__, moduleName);
					}
				}
				break;
			}
		}
	}
	
	if (NO_ERROR==hResult)
	{
		connection->toplevel=cref;	
		connection->infoBlock=ibref;
	}
	
	return hResult;
}


/*
	Creates a map of all the descriptors, from the top level down. This gives
	the ability to read the descriptors without having to physically assemble more
	than one descriptor at a time.
	
	It requires that the topLevel field of the connection structure has been
	filled in. 
	
	This also makes sure that children are assembled in the order which they
	are positioned. 
*/
HRESULT	edsAddInfoBlockToMap(EDS_SERVER_CONNECTION* connection, 
							 EDS_DESCRIPTOR_REF  parent,
							 EDS_BUFFER_TYPE  buffertype)
{
	HRESULT				hResult=NO_ERROR;
	EDS_DESCRIPTOR*		pdata=NULL;
	uint32				ptype=0;
	EDS_DESCRIPTOR_REF	cref=0;
	EDS_DESCRIPTOR*		cdata=NULL;
	uint32				index=1;
	
	hResult=drsLockDescriptor(parent);
	if (NO_ERROR==hResult)
	{
		drsGetDescriptor(parent,&pdata);
		
		ptype=pdata->type;
		
		if (pdata->type==EDS_DESCRIPTORTYPE_INFOBLOCK)
		{
			uint32	dsize=0;
			
			edsGetDescriptorSize(parent, FALSE, &dsize);
			
			connection->map[connection->mapcount].descriptor=parent;
			connection->map[connection->mapcount].startpos=0;
			connection->map[connection->mapcount].length=dsize;
			connection->mapcount++;
		}
		
		drsUnlockDescriptor(parent);	
	}
	
	if (NO_ERROR==hResult)
	{
		while (NO_ERROR==drsGetNthDescriptor(index++,&cref))
		{
			if (NO_ERROR==drsLockDescriptor(cref))
			{
				drsGetDescriptor(cref,&cdata);
				
				if (cdata->type==EDS_DESCRIPTORTYPE_INFOBLOCK &&
					cdata->parent_reference==parent &&
					cdata->u.info_block.buffertype==buffertype)
				{
					if (ptype==EDS_DESCRIPTORTYPE_INFOBLOCK)
					{
						edsAddInfoBlockToMap(connection,cref,EDS_BUFFERTYPE_EXTENDEDINFO);	
					}
					else
					{
						edsAddInfoBlockToMap(connection,cref,buffertype);	
					}
				}
				
				drsUnlockDescriptor(cref);	
			}	
		}
	}
	
	return hResult;	
}


HRESULT	edsCreateDescriptorMap(EDS_SERVER_CONNECTION*	connection)
{
	HRESULT			hResult=NO_ERROR;
	EDS_DESCRIPTOR*	ddata=NULL;

	
	hResult=drsLockDescriptor(connection->toplevel);
	if (NO_ERROR==hResult)
	{
		hResult=drsGetDescriptor(connection->toplevel,&ddata);
	}
	
	switch (ddata->type)
	{
		case EDS_DESCRIPTORTYPE_SUBUNITIDENTIFIER:
		{
			uint32	hsize=0;
			uint32	curpos=0;
			
			hsize=ddata->u.subunit_identifier.size_of_list_id*ddata->u.subunit_identifier.number_of_root_lists;
			hsize+=10;
			hsize+=ddata->u.subunit_identifier.si_length;
			
			connection->map[connection->mapcount].descriptor=connection->toplevel;
			connection->map[connection->mapcount].startpos=0;
			connection->map[connection->mapcount].length=hsize;
			connection->mapcount++;
			
			edsAddInfoBlockToMap(connection,connection->toplevel,EDS_BUFFERTYPE_SUBUNITINFO);
			
			curpos=hsize;
			
			hsize=2+ddata->u.subunit_identifier.mi_length;
			
			connection->map[connection->mapcount].descriptor=connection->toplevel;
			connection->map[connection->mapcount].startpos=curpos;
			connection->map[connection->mapcount].length=hsize;
			connection->mapcount++;
			
			edsAddInfoBlockToMap(connection,connection->toplevel,EDS_BUFFERTYPE_MANUFACTURERINFO);
		
		}
		break;
		
		case EDS_DESCRIPTORTYPE_INFOBLOCK:
		{
			edsAddInfoBlockToMap(connection,connection->toplevel,EDS_BUFFERTYPE_EXTENDEDINFO);
		}
		break;
		
		case EDS_DESCRIPTORTYPE_ENTRY:
		{
			uint32	dsize=0;
			
			edsGetDescriptorSize(connection->toplevel,FALSE,&dsize);
			
			connection->map[connection->mapcount].descriptor=connection->toplevel;
			connection->map[connection->mapcount].startpos=0;
			connection->map[connection->mapcount].length=dsize;
			connection->mapcount++;
			
			edsAddInfoBlockToMap(connection,connection->toplevel,EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
			edsAddInfoBlockToMap(connection,connection->toplevel,EDS_BUFFERTYPE_EXTENDEDINFO);
		}
		break;
		
		case EDS_DESCRIPTORTYPE_OPAQUE:
		{
			uint32	dsize=0;
			
			edsGetDescriptorSize(connection->toplevel,FALSE,&dsize);
			
			connection->map[connection->mapcount].descriptor=connection->toplevel;
			connection->map[connection->mapcount].startpos=0;
			connection->map[connection->mapcount].length=dsize;
			connection->mapcount++;
			
			edsAddInfoBlockToMap(connection,connection->toplevel,EDS_BUFFERTYPE_EXTENDEDINFO);
		}
		break;
		
		case EDS_DESCRIPTORTYPE_LIST:
		{
			uint32				hsize=0;
			uint32				index=1;
			EDS_DESCRIPTOR_REF	iref=0;
			EDS_DESCRIPTOR*		idata=NULL;
			
			hsize=6+ddata->u.list_descriptor.si_size;
			
			connection->map[connection->mapcount].descriptor=connection->toplevel;
			connection->map[connection->mapcount].startpos=0;
			connection->map[connection->mapcount].length=hsize;
			connection->mapcount++;

			edsAddInfoBlockToMap(connection,connection->toplevel,EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
			
			connection->map[connection->mapcount].descriptor=connection->toplevel;
			connection->map[connection->mapcount].startpos=hsize;
			connection->map[connection->mapcount].length=2;
			connection->mapcount++;
			
			while (NO_ERROR==drsGetNthDescriptor(index++,&iref))
			{
				if (NO_ERROR==drsLockDescriptor(iref))
				{
					drsGetDescriptor(iref,&idata);
					
					if (idata->parent_reference==connection->toplevel &&
						idata->type==EDS_DESCRIPTORTYPE_ENTRY)
					{
						uint32 dsize=0;
						
						edsGetDescriptorSize(iref,FALSE,&dsize);
						
						connection->map[connection->mapcount].descriptor=iref;
						connection->map[connection->mapcount].startpos=0;
						connection->map[connection->mapcount].length=dsize;
						connection->mapcount++;

						edsAddInfoBlockToMap(connection,iref,EDS_BUFFERTYPE_DESCRIPTORSPECIFICINFO);
						edsAddInfoBlockToMap(connection,iref,EDS_BUFFERTYPE_EXTENDEDINFO);			
												
					}
				
					drsUnlockDescriptor(iref);	
				}	
			}
			edsAddInfoBlockToMap(connection,iref,EDS_BUFFERTYPE_EXTENDEDINFO);			
		}
		break;
	}
	
	drsUnlockDescriptor(connection->toplevel);
	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT edsResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc handler thread
	hResult = TCTaskResume(edsHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT edsSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc handler thread
	hResult = TCTaskSuspend(edsHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT edsInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc handler thread
	hResult = TCTaskCreate(&edsHandlerThreadID, edsHandlerThreadName, edsHandlerThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT*2);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}


/*********************************************************
	Initialize EDS server 
*/
HRESULT	edsServerInitialize(void)
{
	HRESULT hResult=NO_ERROR;

	memset(EdsConnectionList,0,sizeof(EdsConnectionList));
	memset(EdsAttemptConnectList,0,sizeof(EdsAttemptConnectList));
	
	hResult = TCSemaphoreOpen(&edsConnectionLockSemID, 1);
	if (hResult != NO_ERROR) return hResult;
	
	lmCreateList(EdsSessionList,LM_LISTSIZE(EDS_SERVER_SESSION_COUNT,sizeof(uint32)),
				sizeof(uint32),NULL);
				
	// Install this module to receive the descriptor opcodes.
	crAddCallback(AVC_BASEHANDLER_CALLBACK);

	hResult = edsResumeTasks();	
	return hResult;
}	
#endif //_EDS

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
  DESCRIPTION:  

		Common System Callback definitions
		These are used with the callback registry

//LM??? need updating/correction

  Revisions:
		created 21/03/2003 KBJ/LM

*****************************************************************************/
#ifndef	_SYSTEMCALLBACKS_H
#define _SYSTEMCALLBACKS_H

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "packetblock.h"
#include "cr.h"
#include "avcUtils.h"
#include "datastream.h"
#include "avcDefs.h"

/*

	RULES FOR THIS FILE
	
	1. ALL CALLBACK TYPES MUST BE UNIQUE
	
	2. ALL KEYS MUST HAVE DESCRIPTIVE TEXT AFTER, DECLARING THEM OPTIONAL OR REQUIRED.
	
*/

/*
	CALLBACK TYPES
*/

enum
{
	/*
		AV/C Callbacks
		
		These callback types should be used to refer to AV/C triggered callbacks.
	*/
	AVC_SUBUNIT_TYPE = 0xFF,
	AVC_POWER_TYPE,
	AVC_VENDOR_SPECIFIC_TYPE,
	AVC_TC_VENDOR_TYPE,
	AVC_SECURITY_TYPE,
	AVC_DESCRIPTORHANDLER_TYPE,
	/*
		DRD Callback types
		
		These callbacks to refer to DRD triggered callbacks.
	*/
	DRD_TYPE,
	
	/*
		TC Electronic Callbacks
		
		These callbacks are usually TC specified services, and don't 
		have an external trigger.  These are triggered by events which
		occur internally, and are different for each of the types of callbacks.
	*/
	TC_TIMER_TYPE
};

/*
	AV/C Callbacks
*/	

/*
Name:
	AVC_SUBUNIT_TYPE 
Description:
	The AVC_SUBUNIT_TYPE callback is used to register subunit callbacks
	which will receive packets from the AV/C Layer of the 1394 stack.
Callback Arguments:
	headerInfo:	
		This argument contains the AV/C header which is filled out with
		the information of incoming FCP packet.
	packetblock:
		This is a standard packet block, the payload datastream has been
		moved forward to the first operand in the AV/C packet. You may
		also use the DSMARKER_OPERAND_0 marker in the stream to get back
		to operand 0.

Return Values:
	This callback may respond with any of the following callbacks, and the
	AV/C Handler will respond with the appropriate ctype response code.
		E_PKT_AVC_SUBUNIT_BUSY			
		E_PKT_AVC_IMPLEMENTED			
		E_PKT_AVC_NOT_IMPLEMENTED		 
		E_PKT_AVC_REJECTED				
		E_PKT_AVC_ACCEPTED				
		E_PKT_AVC_INTERIM				
		E_PKT_AVC_STABLE
	If the callback returns NO_ERROR, then the subunit has assumed responsibility
	for the packet.


EXAMPLE:
	This is an example of registering a DISC subunit callback:
	
	static		CALLBACK_DESCRIPTOR	AVC_DISC_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,			AVC_SUBUNIT_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,			0},
		{CALLBACK_DESCRIPTOR_NAME,			(uint32)"AVC_DISC_CD_CALLBACK"},
		{AVC_SUBUNIT_CALLBACK_KEY,			(uint32)&AVC_DISC_CD_CALLBACK},
		{AVC_SUBUNIT_SUBUNITTYPE_KEY,		AVC_SU_TYPE_AV_DISC},
		{AVC_SUBUNIT_ID_KEY,				0},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
	
*/

typedef	HRESULT	(*AVC_SUBUNIT_CALLBACK)(AVC_HEADER *pHeader, PB* packetblock);
#define AVC_SUBUNIT_CALLBACK_KEY			0x1		// Required
#define AVC_SUBUNIT_SUBUNITTYPE_KEY			0x2		// Required. Defined in AV/C General Documentation.
#define AVC_SUBUNIT_ID_KEY					0x3		// Required.

/*
Name:
	AVC_POWER_TYPE
	 
Description:
	The AVC_POWER_TYPE callback is used to register callbacks to 
	the AVC power commands.  This allows your callbacks to receive
	power on/off commands, as well as to respond to queries about 
	the power state of your device.  This may be used by host interfaces,
	or other portions of the stack.  All registered callbacks are called
	when a command is received.
	
	When registering a device which has no determinable power state, you
	should not include an AVC_POWER_STATE_CALLBACK in your descriptor. 
	
Callback Arguments:
	AVC_POWER_ON_CALLBACK:
		NO ARGUMENTS
	AVC_POWER_OFF_CALLBACK:
		NO ARGUMENTS
	AVC_POWER_STATE_CALLBACK:
		NO ARGUMENTS:
		return arguments:
			One of the following is a valid power state return value:
				E_AVC_POWER_STATE_ON
				E_AVC_POWER_STATE_OFF
				E_AVC_POWER_STATE_SLEEP
			If the powerstate of the device is undeterminable,
			you shouldn't install a power callback.
			
EXAMPLE:
	This is an example of a power callback set which would be used for
	a device which does have a determinable power state:
	
	static		CALLBACK_DESCRIPTOR	DRD_POWER_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,		AVC_POWER_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,		0},
		{CALLBACK_DESCRIPTOR_NAME,		(uint32)"DRD_DETERMINABLE_POWER_CALLBACK"},
		{AVC_POWER_ON_CALLBACK_KEY,		(uint32)&avcUnitSetPowerOn},
		{AVC_POWER_OFF_CALLBACK_KEY,	(uint32)&avcUnitSetPowerOff},
		{AVC_POWER_STATE_CALLBACK_KEY,	(uint32)&avcUnitQueryPower},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

	This is an example of a power callback set which does not have a 
	determinable power state:

	static		CALLBACK_DESCRIPTOR	DRD_POWER_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,		AVC_POWER_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,		0},
		{CALLBACK_DESCRIPTOR_NAME,		(uint32)"DRD_UNDETERMINABLE_POWER_CALLBACK"},
		{AVC_POWER_ON_CALLBACK_KEY,		(uint32)&avcUnitSetPowerOn},
		{AVC_POWER_OFF_CALLBACK_KEY,	(uint32)&avcUnitSetPowerOff},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
*/

typedef HRESULT	(*AVC_POWER_CALLBACK)(void);
#define	AVC_POWER_ON_CALLBACK_KEY			0x1		// Optional
#define AVC_POWER_OFF_CALLBACK_KEY			0x2		// Optional
#define AVC_POWER_STATUS_CALLBACK_KEY		0x3		// Optional

/*
Name:
	AVC_VENDOR_SPECIFIC_TYPE
	 
Description:
	The AVC_VENDOR_SPECIFIC_TYPE callback is used to allow make
	UNIT callbacks for vendors other than TC Electronic.  It doesn't
	yet enable using vendor specific command sets for particular 
	subunits.  If you are intending to write a TC specific command,
	then you use the AVC_TC_VENDOR_TYPE callback instead.
	
Callback Arguments:
	packetBlock:
		standard packetblock, the payload datastream has already
		been advanced past the vendor id code.
	headerInfo:
		standard av/c header decoded for you already.

Return Values:
	This callback may respond with any of the following callbacks, and the
	AV/C Handler will respond with the appropriate ctype response code.
		E_PKT_AVC_SUBUNIT_BUSY			
		E_PKT_AVC_IMPLEMENTED			
		E_PKT_AVC_NOT_IMPLEMENTED		 
		E_PKT_AVC_REJECTED				
		E_PKT_AVC_ACCEPTED				
		E_PKT_AVC_INTERIM				
		E_PKT_AVC_STABLE
	If the callback returns NO_ERROR, then the subunit has assumed responsibility
	for the packet.

			
EXAMPLE:
	
	static		CALLBACK_DESCRIPTOR	AVC_VENDORSPECIFIC[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,			AVC_SUBUNIT_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,			0},
		{CALLBACK_DESCRIPTOR_NAME,			(uint32)"AVC_VENDORSPECIFIC"},
		{AVC_VENDOR_SPECIFIC_CALLBACK_KEY,	(uint32)&AVC_VENDORSPECIFIC},
		{AVC_VENDOR_SPECIFIC_VENDORID_KEY,	THIS_VENDOR_ID},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
	
*/
typedef HRESULT (*AVC_VENDOR_SPECIFIC_CALLBACK)(AVC_HEADER *pHeader, PB* packetblock, pDataStream pStream);
#define AVC_VENDOR_SPECIFIC_CALLBACK_KEY	0x1 // Required 
#define AVC_VENDOR_SPECIFIC_VENDORID_KEY	0x2 // Required.  

/*
Name:
	AVC_TC_VENDOR_TYPE
	 
Description:
	This is the TC Electronic vendor specific UNIT command
	registration callback.  It is used to register UNIT commands
	which are VENDOR SPECIFIC and follow the TC VENDOR SPECIFIC 
	packet format.
	
	Look at "TCVendorCommands.vsd" in $/Documents/, to find the
	format of incoming packets. Also this document lists all commands
	which are available for this set of commands.
	
	This callback may be scoped to only respond to a very specific
	command, or it may be scoped to respond to a class of command.
	If you include AVC_TC_VENDOR_COMMAND_KEY then your callback 
	will only be called once a command with the specified CLASS and COMMAND ID,
	is sent to the unit.  If you don't use the AVC_TC_VENDOR_COMMAND_KEY
	in your descriptor, then you will receive callbacks for call commands
	in the specified CLASS.
		
Callback Arguments:
	ctype:
		ctype of the incoming command.
	packetblock:
		standard incoming packet block.
	stream:
		A standard DataStream* which contains a valid datastream, 
		and has already been advanced to operand[7] by the time 
		your callback receives the packet.

Return Values:
	This callback may respond with any of the following callbacks, and the
	AV/C Handler will respond with the appropriate ctype response code.
		E_PKT_AVC_SUBUNIT_BUSY			
		E_PKT_AVC_IMPLEMENTED			
		E_PKT_AVC_NOT_IMPLEMENTED		 
		E_PKT_AVC_REJECTED				
		E_PKT_AVC_ACCEPTED				
		E_PKT_AVC_INTERIM				
		E_PKT_AVC_STABLE
	If the callback returns NO_ERROR, then the subunit has assumed responsibility
	for the packet.

			
EXAMPLE:
	
	This example shows the firmware ID specific callback:
	
	static		CALLBACK_DESCRIPTOR	AVC_FIRMWAREID_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,AVC_TC_VENDOR_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,0},
		{CALLBACK_DESCRIPTOR_NAME,(uint32)"AVC_FIRMWAREID_CALLBACK"},
		{AVC_TC_VENDOR_CALLBACK_KEY,(uint32)&sendFirmwareIdReply}, // CALLBACK PTR of TYPE AVC_SUBUNIT_CALLBACK},
		{AVC_TC_VENDOR_CLASSID_KEY,TC_VSAVC_CLASS_GENERAL},
		{AVC_TC_VENDOR_COMMAND_KEY,TC_VSAVC_CMD_PGM_IDENTIFY},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
	
*/
typedef HRESULT (*AVC_TC_VENDOR_CALLBACK)(AVC_HEADER *pHeader, PB* packetblock, pDataStream pStream);
#define AVC_TC_VENDOR_CALLBACK_KEY			0x1 // Required
#define AVC_TC_VENDOR_CLASSID_KEY			0x2 // Required
#define AVC_TC_VENDOR_COMMAND_KEY			0x3 // Optional

/*
Name:
	AVC_SECURITY_TYPE
	 
Description:
	The AVC_SECURITY_TYPE is used to respond to
	AVC_SECURITY packets sent to the UNIT.  Currently only
	5C is defined for using this security feature.
	
Callback Arguments:
	packetBlock:
		standard packetblock, the payload datastream is currently
		pointing to the data of payload. Just past the category ID of
		the security packet.

Return Values:
	This callback may respond with any of the following callbacks, and the
	AV/C Handler will respond with the appropriate ctype response code.
		E_PKT_AVC_SUBUNIT_BUSY			
		E_PKT_AVC_IMPLEMENTED			
		E_PKT_AVC_NOT_IMPLEMENTED		 
		E_PKT_AVC_REJECTED				
		E_PKT_AVC_ACCEPTED				
		E_PKT_AVC_INTERIM				
		E_PKT_AVC_STABLE
	If the callback returns NO_ERROR, then the subunit has assumed responsibility
	for the packet.

			
EXAMPLE:
	
	static		CALLBACK_DESCRIPTOR	AVC_AKE_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,		AVC_SECURITY_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,		0},
		{CALLBACK_DESCRIPTOR_NAME,		(uint32)"5C_AVC_SECURITY_CALLBACK"},
		{AVC_SECURITY_CALLBACK_KEY,		(uint32)&akeSecurityCallback},
		{AVC_SECURITY_CATEGORY_KEY,		AVC_SECURITY_CATEGORY_DTCP},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
*/

typedef HRESULT (*AVC_SECURITY_CALLBACK)(AVC_HEADER *pHeader, PB* packetblock, pDataStream pStream);
#define AVC_SECURITY_CALLBACK_KEY		0x1	// Required
#define AVC_SECURITY_CATEGORY_KEY		0x2 // Required

/*
	AV/C Callbacks
*/	

/*
Name:
	AVC_DESCRIPTORHANDLER_TYPE 
Description:
	For handling of descriptor opcodes, a subunit wide callback is put
	in place.  This callback will intercept ALL incoming packets, which 
	are descriptor based.
	
Callback Arguments:
	headerInfo:	
		This argument contains the AV/C header which is filled out with
		the information of incoming FCP packet.
	packetblock:
		This is a standard packet block, the payload datastream has been
		moved forward to the first operand in the AV/C packet. You may
		also use the DSMARKER_OPERAND_0 marker in the stream to get back
		to operand 0.

Return Values:
	This callback may respond with any of the following callbacks, and the
	AV/C Handler will respond with the appropriate ctype response code.
		E_PKT_AVC_SUBUNIT_BUSY			
		E_PKT_AVC_IMPLEMENTED			
		E_PKT_AVC_NOT_IMPLEMENTED		 
		E_PKT_AVC_REJECTED				
		E_PKT_AVC_ACCEPTED				
		E_PKT_AVC_INTERIM				
		E_PKT_AVC_STABLE
	If the callback returns NO_ERROR, then the subunit has assumed responsibility
	for the packet.


EXAMPLE:
	This is an example of registering a DISC subunit callback:
	
	static		CALLBACK_DESCRIPTOR	AVC_TC_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,			AVC_DESCRIPTORHANDLER_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,			0},
		{CALLBACK_DESCRIPTOR_NAME,			(uint32)"AVC_DESCRIPTORHANDLER_CALLBACK"},
		{AVC_DESCRIPTORHANDLER_CALLBACK_KEY,(uint32)&AVC_DESCRIPTORHANDLER_CALLBACK},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
	
*/
typedef	HRESULT	(*AVC_DESCRIPTORHANDLER_CALLBACK)(AVC_HEADER *pHeader, PB*	packetblock);
#define AVC_DESCRIPTORHANDLER_CALLBACK_KEY			0x1		// Required

/*
Name:
	TC_TIMER_TYPE
	 
Description:
	This enables the use of a periodic shared timer resource.  This is easier than
	creating a new thread for periodic tasks which are of low importance, but need
	to happen regularly.  
	
	This is an INACCURATE SLOW timer, it is not meant for tasks which need to occur
	more frequently than once every 1/4 of a second.
	  
	Because this is a shared resource, it shouldn't be used for tasks which are extremely
	time consuming otherwise the tasks may cause other tasks to be called slower than
	they were expecting.  When a callback is called, it happens at highest priority,
	which means your callback has control until the task is completed.  So you should
	make it as short as possible.
	
	There are two types of timers which can be defined using this callback:
	
	1. ONE SHOT TIMER:
		If the descriptor doesn't include a 'TC_TIMER_PERIOD_KEY' then the
		period which is defined in the "TC_TIMER_NEXTFIRE_KEY" will be used to
		define the period of time after which this timer will be called.  
		After being called, a ONE SHOT TIMER will be removed from the callback
		registry, and it will need to have the TC_TIMER_NEXTFIRE_KEY set again using
		the "kvSetValue" function.    Then the caller must re-insert the callback
		into the callback registry in order to have it called again.
		
	2. PERIODIC TIMER:
		If the descriptor includes a TC_TIMER_PERIOD_KEY, then timer proc, will 
		be called every "TC_TIMER_PERIOD_KEY" 1/4's of a second.  And the caller's
		code does not have to make any adjustments to make this work.
	
Callback Arguments:
	void*	Context1 -  This points to the data or code you registered in the callback descriptor.
	void*	Context2 -  This points to the data or code you registered in the callback descriptor.
	uint	CallbackCounter - This is number of times this callback has been called. 
	
Return Values:
	None.  The timer does not expect any results from the timer callback.
			
EXAMPLE (PERIODIC TIMER):
	
	static		CALLBACK_DESCRIPTOR		DESCRIPTOR_CLEANUP_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,		TC_TIMER_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,		0},
		{CALLBACK_DESCRIPTOR_NAME,		(uint32)"EDS_DESCRIPTOR_TIMER_CALLBACK"},
		{TC_TIMER_CALLBACK_KEY	 ,		(uint32)EDS_TIMER_CALLBACK},
		{TC_TIMER_CONTEXT1_KEY	 , 		(uint32)ContextData1},
		{TC_TIMER_CONTEXT2_KEY	 ,		(uint32)ContextData2},
		{TC_TIMER_COUNTER_KEY	 ,      (uint32)0},
		{TC_TIMER_PERIOD_KEY	 ,		(uint32)4}, // Callback period of 1 second.
		{TC_TIMER_NEXTFIRE_KEY	 ,		(uint32)0}, // First Firing happens immediately.
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

EXAMPLE (ONE SHOT TIMER):

	static		CALLBACK_DESCRIPTOR		DESCRIPTOR_CLEANUP_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,		TC_TIMER_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,		0},
		{CALLBACK_DESCRIPTOR_NAME,		(uint32)"ONE_SHOT_TIMER"},
		{TC_TIMER_CALLBACK_KEY	 ,		(uint32)EDS_TIMER_CALLBACK},
		{TC_TIMER_CONTEXT1_KEY	 , 		(uint32)ContextData1},
		{TC_TIMER_CONTEXT2_KEY	 ,		(uint32)ContextData2},
		{TC_TIMER_COUNTER_KEY	 ,      (uint32)0},
		{TC_TIMER_NEXTFIRE_KEY	 ,		(uint32)0}, // First Firing happens immediately.
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};


*/

typedef	void	(*TC_TIMER_CALLBACK)(void*	CONTEXT1, void* CONTEXT2, uint32 CALLBACKCOUNTER);
#define TC_TIMER_CALLBACK_KEY	1	 // REQUIRED, Points to a callback of type TC_TIMER_CALLBACK
#define	TC_TIMER_CONTEXT1_KEY	2	 // Optional, Not including this causes NULL to be passed to the callback.
#define	TC_TIMER_CONTEXT2_KEY	3	 // Optional, Not including this causes NULL to be passed to the callback. 
#define TC_TIMER_COUNTER_KEY	4	 // Optional, Not including this causes 0 to be passed as the callback counter argument.
#define	TC_TIMER_PERIOD_KEY		5    // Optional, Frequency in number of 1/4 seconds. If not included, then the timer will automatically be removed once the period specified by "TC_TIMER_NEXTFIRE_KEY" has expired.   
#define	TC_TIMER_NEXTFIRE_KEY	6	 // REQUIRED, This determines the number of 1/4 seconds, until the first firing of your timer function. 
									 //			  If set to 0 then this will cause the timer to callback the first time it sees your callback.
#endif // _SYSTEMCALLBACKS_H



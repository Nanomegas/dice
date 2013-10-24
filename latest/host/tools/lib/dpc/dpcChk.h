///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004-2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////
/****************************************************************************
	
	Purpose:	Abstraction of DPC Latency check service
	
	Revisions:
		created 28/09/2007 ml
****************************************************************************/
/** @addtogroup DpcChk

	C++ Class wrapper abstracting installing, opening and using the
	DPC Latency service.
	
	If the service has been installed manually this module can open and close
	it. It can also create and install the service and remove all traces again.
	
	The service is not created for multiple clients and the behavior is unpredictable
	when several applications are using the service at the same time.

	To install and start the service manually from a command prompt:
	>copy DPCLatencyChecker.sys %windir%\system32\drivers
	>sc create DpcLatencyChecker binPath= %windir%\system32\drivers\DpcLatencyChecker.sys type= kernel
	>net start DpcLatencyChecker

	After that only the Open/CLose calls are needed. A call to Remove will undo the operations
	listed above.
	@{
*/

#pragma once

#define DPC_NO_ERROR		0	//Operation succeded
#define DPC_DEV_ERROR		1	//The Device returned an error
#define DPC_ACCESS_ERROR	2	//The calling process does not have the priveledges needed
#define DPC_INSTALL_ERROR	3	//Error installing the service

//==============================================================================
class dpcChk
{
public:
    //==============================================================================
    dpcChk();
    ~dpcChk();

	
	int install (const unsigned char * pDriver, int szDriver);	///< Install the service, if pDriver is NULL the service in DPCBinaryData will be installed
	int remove ();	///< Remove the service, this call will remove the service even if it is manually installed
	int open ();	///< Open the service, the service must be installed first either using Install or from the command prompt
	int close ();	///< Close the service
	int setCheckTimer (bool bStart, long us); ///< Set the check interval, not needed if setChkTimerDefault is used
	int setChkTimerDefault (bool bStart);	///< Recomended instead of setCheckTimer as it sets the optimal interval
	bool procHasAdminCredentials();
	int startDPCInducer (bool bStart, long durationUs, long stallUs);	///< Start inducing a DPC Spike every durationUs of the size of stallUs
	int getDPCPeak (long& peak);	///< Get the largest peak since last call, it will only be assigned if it is larger than 'peak'. Set peak=0 before call.
	int getDPCSuggestedTimer (long * us); ///< Get the suggested optimal check interval
	
	
protected:
    /** @internal */
    bool m_bOpen;
    bool m_bInstalled;
   	void * m_hDevice;
};



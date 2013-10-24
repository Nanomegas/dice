//==============================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//==============================================================================
//
//	exception handling framework - addapted from Carsten Neff @ TCWorks
//

#include "tcat_exception.h"

#ifdef __MACH__
	#include <mach-o/dyld.h>
#elif WIN32	
	#include <windows.h>
#endif

#include <algorithm>
#include <deque>



using namespace tcat;
using namespace exception;



static std::deque<std::string> g_exception_list;



///////////////////////////////////////////////////////////////////////////////
//
//	WIN32 specific
//
///////////////////////////////////////////////////////////////////////////////
//
#ifdef WIN32 //\cond
struct PARAM_ASSERT
{
	const	std::string	caption;
	const	std::string	msg; 
			int			result;
};
DWORD WINAPI static_thread_dialog (LPVOID lpParameter)
{
	if(lpParameter)
	{
		PARAM_ASSERT& prm = *(PARAM_ASSERT*)lpParameter;
		prm.result = ::MessageBox (0, prm.msg.c_str(), prm.caption.c_str(), MB_YESNOCANCEL | MB_TASKMODAL | MB_SETFOREGROUND | MB_ICONERROR | MB_DEFBUTTON2);
		return 0;
	}
	else
	{
		return -1;
	}
}
#elif TARGET_OS_MAC
inline void strpcpy(Str255& dst, const std::string src)
{
	dst[0]=std::min<int>(src.size(),255);
	strncpy((char*)&dst[1],src.c_str(),dst[0]);
}
#endif //\endcond WIN32




///////////////////////////////////////////////////////////////////////////////
//
//	assert_dialog
//
///////////////////////////////////////////////////////////////////////////////
//
void assert_dialog (std::string caption, std::string msg, std::string explenation)
{
#ifdef WIN32
	// Fill the parameters for the static_thread_dialog thread:
	PARAM_ASSERT prm = {caption, msg + "\n\n" + explenation + "\n\nBreak into Debugger? (Cancel to abort application)", -1};

	//
	// We create a new thread for the MessageBox to get our own message queue.
	// This is nessesary because otherwise, our program will continue thru the local message
	// loop of the MessageBox.
	//
	// NOTE:
	//  StaticDialogWindowThread is the entrypoint of the thread, and simply creates a MessageBox
	//  by using the parameters from prm.
	//
	HANDLE hThread = ::CreateThread (NULL, 0, &static_thread_dialog, &prm, 0, NULL);
	
	// wait until StaticDialogWindowThread thread has terminated:
	::WaitForSingleObject (hThread, INFINITE);
	hThread = NULL;

	// interpret the dialog result:
	switch (prm.result)
	{
	case IDNO:
		break;
	case IDYES:
		::DebugBreak();
		break;
	case IDCANCEL:
		::ExitProcess(0);
		break;
	}
#elif TARGET_OS_MAC
	if (!::MPTaskIsPreemptive(::MPCurrentTaskID()))
	{	
		// convert into pascal string
		Str255 pascal_message;
		Str255 pascal_explenation;		
		Str255 pascal_button_default;
		Str255 pascal_button_cancel;
		Str255 pascal_button_other;

		strpcpy(pascal_message, caption+"\n"+msg+"\n\n");
		strpcpy(pascal_explenation, explenation);
		strpcpy(pascal_button_default, "Ignore");
		strpcpy(pascal_button_cancel, "Quit");			
		strpcpy(pascal_button_other, "Debug");
		
		// struct to initialize alert box	
		AlertStdAlertParamRec allert =
		{	
			false,
			false,
			nil,
			pascal_button_default,
			pascal_button_cancel,
			pascal_button_other,
			1,
			0,
			kWindowDefaultPosition
		};
		
		SInt16 buttonPressed = 1;    
	    OSErr err = ::StandardAlert (kAlertCautionAlert, pascal_message, pascal_explenation, &allert, &buttonPressed);
	    if (err!=noErr)
		{
			// stop Here
		}
		
	    switch (buttonPressed) 
	    {
	    case kAlertStdAlertOKButton:		// ignore
	    	break;
	    case kAlertStdAlertCancelButton:	// quit
	    	::ExitToShell();
	    	break;
	    case kAlertStdAlertOtherButton:		// debug
	    	::Debugger();
	    	break;
	    }
    }// is task preemtive
#endif	// WIN32
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::exception::base::base
//
///////////////////////////////////////////////////////////////////////////////
//
base::base(std::string filename, int line, std::string dbg_msg, std::string usr_msg, std::string name):
	m_filename	(filename), 
	m_name		(name), 
	m_dbg_msg	(dbg_msg), 
	m_usr_msg 	(usr_msg),
	m_line		(line)
{
	//
	// break here to debug all exceptions
	//
	if ((std::find (g_exception_list.begin(), g_exception_list.end(), "*") != g_exception_list.end()) ||
		(std::find (g_exception_list.begin(), g_exception_list.end(), name) != g_exception_list.end()))
		assert_dialog (usr_msg, name+"\n"+dbg_msg,
			m_filename
//			+ "\nline:" + std::string(line)
			);
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::exception::base::enable
//
///////////////////////////////////////////////////////////////////////////////
//
void base::enable (const std::string exceptions[])
{
	g_exception_list.clear();
	while (exceptions->size())
		g_exception_list.push_back(std::string(*exceptions++));
}

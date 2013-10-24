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

#include <windows.h>
#include <winbase.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "dpcChk.h"
#include "DpcLatencyDriverInterface.h"
#if !defined(_WIN64)
#include "DpcLatencyDriverBinary32.h"
#endif
#include "DpcLatencyDriverBinary64.h"


#define DRV_SYMLINK		"\\\\.\\LatencyCheckerDevice"


#if !defined(_WIN64)

typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

static BOOL IsWow64()
{
	LPFN_ISWOW64PROCESS fnIsWow64Process;
	BOOL bIsWow64 = FALSE;

	fnIsWow64Process = (LPFN_ISWOW64PROCESS) GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"IsWow64Process");

	if (fnIsWow64Process != NULL)
	{
		if (!fnIsWow64Process( GetCurrentProcess(), &bIsWow64 ))
		{
			// handle error
		}
	}
	return bIsWow64;
}


typedef BOOL (WINAPI *LPFN_WOW64DISABLEWOW64FSREDIRECTION)(PVOID *);

static void DisableWow64FsRedirection( PVOID *oldValue )
{
	LPFN_WOW64DISABLEWOW64FSREDIRECTION	func;

	func = (LPFN_WOW64DISABLEWOW64FSREDIRECTION) GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"Wow64DisableWow64FsRedirection");

	if (func != NULL)
	{
		func( oldValue );
	}
}


typedef BOOL (WINAPI *LPFN_WOW64REVERTWOW64FSREDIRECTION)(PVOID);

static void RevertWow64FsRedirection( PVOID oldValue )
{
	LPFN_WOW64REVERTWOW64FSREDIRECTION func;

	func = (LPFN_WOW64REVERTWOW64FSREDIRECTION) GetProcAddress(
		GetModuleHandle(TEXT("kernel32")),"Wow64RevertWow64FsRedirection");

	if (func != NULL)
	{
		func( oldValue );
	}
}

#endif  // !defined(_WIN64)



dpcChk::dpcChk(): 
	m_bOpen(false),
	m_bInstalled(false),
    m_hDevice(INVALID_HANDLE_VALUE)
{
}


dpcChk::~dpcChk ()
{
	if (m_bOpen) 
		close();
}


int dpcChk::open ()
{
	//open a handle to the driver
	m_hDevice = CreateFile(DRV_SYMLINK,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (m_hDevice == INVALID_HANDLE_VALUE)	
		return DPC_DEV_ERROR;
	m_bOpen = true;
	return DPC_NO_ERROR;
}


bool dpcChk::procHasAdminCredentials()
{
	bool bResult = false;
	SC_HANDLE hSc = OpenSCManager(NULL, NULL, GENERIC_ALL);
	if (NULL != hSc)
	{
		if (ERROR_ACCESS_DENIED != GetLastError())
		{
			bResult = true;
		}
		CloseServiceHandle(hSc);
	}
	return bResult;
}


int dpcChk::install (const unsigned char * pDriver, int szDriver)
{
	HANDLE hDriverFile;
	DWORD bytesWritten = 0;
	TCHAR szPath[MAX_PATH];
	DWORD error;

	if (!pDriver)
	{
#if defined(_WIN64)
		pDriver = DpcLatencyDriverBinary64;
		szDriver = sizeof(DpcLatencyDriverBinary64);
#else
		if (IsWow64())
		{
			// need to load the 64-bit driver
			pDriver = DpcLatencyDriverBinary64;
			szDriver = sizeof(DpcLatencyDriverBinary64);
		}
		else
		{
			pDriver = DpcLatencyDriverBinary32;
			szDriver = sizeof(DpcLatencyDriverBinary32);
		}
#endif
	}

	//Let's prepare the path for the driver
	if (!SUCCEEDED(SHGetFolderPath( NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szPath )))
	{
		return DPC_DEV_ERROR;
	}
	PathAppend (szPath,TEXT("\\drivers\\TCAT_DpcLatencyChecker.sys"));

	//Open the SCManager, this requires admin rights so it might fail.
	SC_HANDLE hSc = OpenSCManager(NULL, NULL, GENERIC_ALL);
	if (hSc == NULL)
	{
		return DPC_ACCESS_ERROR;	
	}

	//let's see if the service already exist, typically we will clean up after closing the app, but it might not have
	//happened if the application crashed or if another app is using the service.
	SC_HANDLE hService = OpenService(hSc, TEXT("DpcLatencyChecker"), GENERIC_ALL);
	if (hService == NULL)
	{
		//so the service is not installed, we will copy the file to the system directory
#if !defined(_WIN64)
		PVOID	oldValue;

		DisableWow64FsRedirection( &oldValue );
#endif
		hDriverFile = CreateFile(szPath,
								GENERIC_WRITE,
								FILE_SHARE_READ,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);
		if (hDriverFile == INVALID_HANDLE_VALUE)
		{
			// we can't get much further now, so clean up and return
			CloseServiceHandle(hSc);
			return DPC_INSTALL_ERROR;
		}

		//Let's write the file to the system directory
		if (!WriteFile(hDriverFile, pDriver, szDriver, &bytesWritten,NULL))
		{
			//we can't get much further now, so clean up and return
			CloseServiceHandle(hSc);
			CloseHandle (hDriverFile);
			return DPC_INSTALL_ERROR;
		}
		CloseHandle (hDriverFile);

#if !defined(_WIN64)
		RevertWow64FsRedirection( oldValue );
#endif

		//So now, lets create the service.
		hService = CreateServiceA(hSc,TEXT("TCAT_DpcLatencyChecker"),TEXT("DpcLatencyChecker"),
								GENERIC_ALL,
								SERVICE_KERNEL_DRIVER,
								SERVICE_DEMAND_START,
								SERVICE_ERROR_NORMAL,
								szPath,
								NULL,
								NULL,
								NULL,
								NULL,
								TEXT(""));
		if (hService == NULL)
		{
			error = GetLastError();
			CloseServiceHandle(hService);
			CloseServiceHandle(hSc);	
			return DPC_INSTALL_ERROR;
		}
		m_bInstalled = true; // remember the dpcChk installed it - so dpcChk is responsible to clean it up.
	}
	//At this point we do have a handle to the service, let's start it
	
	if (!StartService(hService,NULL,NULL))
	{
		error = GetLastError();
		if (error != ERROR_SERVICE_ALREADY_RUNNING)
		{
			CloseServiceHandle(hService);
			CloseServiceHandle(hSc);		
			return DPC_INSTALL_ERROR;			
		}
	}	
	CloseServiceHandle(hService);
	CloseServiceHandle(hSc);		
	return DPC_NO_ERROR;
}


int dpcChk::remove ()
{
	TCHAR szPath[MAX_PATH];
	DWORD error;
	SERVICE_STATUS status;

	//Let's prepare the path for the driver
	if (!SUCCEEDED(SHGetFolderPath( NULL, CSIDL_SYSTEM, NULL, SHGFP_TYPE_CURRENT, szPath )))
	{
		return DPC_DEV_ERROR;
	}
	PathAppend (szPath,TEXT("\\drivers\\TCAT_DpcLatencyChecker.sys"));

	//Open the SCManager, this requires admin rights so it might fail.
	SC_HANDLE hSc = OpenSCManager(NULL, NULL, GENERIC_ALL);
	if (hSc == NULL)
	{
		return DPC_ACCESS_ERROR;	
	}

	//let's see if the service already exist, it should if install succeded
	SC_HANDLE hService = OpenService(hSc, TEXT("TCAT_DpcLatencyChecker"), GENERIC_ALL);
	if (hService != NULL)
	{
		if (!ControlService(hService, SERVICE_CONTROL_STOP , &status))
		{
			error = GetLastError();
		}
		//it is stopped, let's remove it
		if (!DeleteService(hService))
		{
			error = GetLastError();
		}
		CloseServiceHandle(hService);
	}
	CloseServiceHandle(hSc);

	//now let's remove the file itself
#if !defined(_WIN64)
		PVOID	oldValue;

		DisableWow64FsRedirection( &oldValue );
#endif
	if (!DeleteFile(szPath))
	{
		error = GetLastError();
	}
#if !defined(_WIN64)
	RevertWow64FsRedirection( oldValue );
#endif

	return DPC_NO_ERROR;
}


int dpcChk::close ()
{
    if(m_hDevice)
    {
        CloseHandle (m_hDevice);
        m_hDevice = NULL;
    }
    m_bOpen = false;
    return DPC_NO_ERROR;
}


int dpcChk::setCheckTimer (bool bStart, long us)
{
	SET_DPC_LATENCY_CHECKER_STRUCT dpcLatencyChecker;
	DWORD nOutput;
	
	dpcLatencyChecker.actionFlag = bStart ? START_TIMER : STOP_TIMER;
	dpcLatencyChecker.checkerDuration = us;
	if (!DeviceIoControl(m_hDevice,
						 IOCTL_SET_DPC_TIMER,
						 &dpcLatencyChecker,
						 sizeof (dpcLatencyChecker),
						 &dpcLatencyChecker,
						 sizeof (dpcLatencyChecker),
						 &nOutput, NULL)) return DPC_DEV_ERROR;
	return DPC_NO_ERROR;
}


int dpcChk::getDPCSuggestedTimer (long * us)
{
	ULONG suggestedTimerValue = 0;
	DWORD nOutput;
	if (!DeviceIoControl(m_hDevice,
						 IOCTL_GET_SUGGESTED_TIMER_DURATION,
						 &suggestedTimerValue,
						 sizeof (suggestedTimerValue),
						 &suggestedTimerValue,
						 sizeof (suggestedTimerValue),
						 &nOutput,
						 NULL)) return DPC_DEV_ERROR;
	*us = suggestedTimerValue;
	return DPC_NO_ERROR;
}	


int dpcChk::setChkTimerDefault (bool bStart)
{
	long suggestedTimerValue = 0;
	
	if (getDPCSuggestedTimer (&suggestedTimerValue)!=DPC_NO_ERROR) return DPC_DEV_ERROR;
	return setCheckTimer (bStart, suggestedTimerValue);
}


int dpcChk::startDPCInducer (bool bStart, long durationUs, long stallUs)
{
	SET_DPC_LATENCY_INDUCER_STRUCT dpcInduceParam;
	DWORD nOutput;
	
	dpcInduceParam.actionFlag = bStart ? START_TIMER : STOP_TIMER;
	dpcInduceParam.inducerDuration = durationUs;
	dpcInduceParam.stallDuration = stallUs;

	//now send the IOCTL
	if (!DeviceIoControl(m_hDevice,
			IOCTL_SET_DPC_INDUCER,
			&dpcInduceParam,
			sizeof (dpcInduceParam),
			&dpcInduceParam,
			sizeof (dpcInduceParam),
			&nOutput,
			NULL)) return DPC_DEV_ERROR;
	return DPC_NO_ERROR;
}


int dpcChk::getDPCPeak (long& peak)
{
	DWORD nOutput;
	long max_peak;

	if (!DeviceIoControl(
		m_hDevice,
		IOCTL_GETDPC_LATENCY,
		NULL, 0,
		&max_peak, sizeof max_peak,
		&nOutput,
		NULL)) return DPC_DEV_ERROR;

	if (max_peak > peak)
		peak = max_peak;

	return DPC_NO_ERROR;
}

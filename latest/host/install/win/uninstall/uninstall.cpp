//==============================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2006 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//==============================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#define WIN32_LEAN_AND_MEAN		// exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <setupapi.h>
#include <Winsvc.h>
#include <objbase.h>
#include <initguid.h>
#include <stdio.h>
#include <string>
#include <list>
#include <tchar.h>

#pragma warning(disable: 4786)			// symbol greater than 255
#pragma comment (lib, "setupapi.lib")

using namespace std;

typedef list<string> LSTSTR;


//------------------------------------------------------------------------------
//
// Function Prototypes
//
//------------------------------------------------------------------------------

BOOL SearchFilesForAString( string pPathToSearch,
							string sPath,
							LSTSTR *sFilesFound,
							UINT* numFilesFound = NULL,
							char* pStringToSearch = NULL,
							LPSTR InfSectionName = NULL,
							LPSTR InfKeyName = NULL );
BOOL DeleteAllFiles( LSTSTR pFiles, BOOL bUnregister = TRUE );
BOOL IsStringPresentInFile( char* sFilename = NULL, char* SectionName = NULL,
							char* KeyName = NULL, char* stringToSearch = NULL );
BOOL WINAPI FindAndRemoveExistingDevice( LPSTR HardwareId );
BOOL WINAPI DeleteReadOnlyFile( CHAR* lpFilename );
BOOL WINAPI SearchAndRemoveInf( LPCSTR pStringToSearch, LPCSTR InfSectionName, LPCSTR InfKeyName );
BOOL WINAPI FindExistingDevice( LPSTR HardwareId );
BOOL WINAPI DeleteThisService( LPSTR ServiceName );



//------------------------------------------------------------------------------

BOOL WINAPI SearchAndRemoveInf( LPSTR pStringToSearch, LPSTR InfSectionName, LPSTR InfKeyName )
{
	char		systemFolder[MAX_PATH] = {0}; 
	LSTSTR		listOfFilesToDelete;
	string		dirPathStr;
	UINT		numfiles = 0;
	BOOL		result = FALSE;

	OutputDebugString( "SearchAndRemoveInf:" );

	if (     (pStringToSearch) && (lstrlen(pStringToSearch) > 0)
		  && (InfSectionName) && (lstrlen(InfSectionName) > 0)
		  && (InfKeyName) && (lstrlen(InfKeyName) > 0)
	   )
	{
		OutputDebugString( pStringToSearch );
		OutputDebugString( InfSectionName );
		OutputDebugString( InfKeyName );

		// Find the system folder path
		GetWindowsDirectory( systemFolder, MAX_PATH );
		dirPathStr = systemFolder;

		// Start searching inf folder for a particular section
		dirPathStr.append( "\\INF\\" );

		SearchFilesForAString( dirPathStr + "*.inf", dirPathStr, &listOfFilesToDelete, &numfiles, pStringToSearch, InfSectionName, InfKeyName );

		// Finally Delete Files
		DeleteAllFiles( listOfFilesToDelete );

		result = TRUE;
	}
	else
	{
		OutputDebugString( "text Searching is not done" );
	}

	return result;
}



/*------------------------------------------------------------------------------
Routine Description:

    This routine finds an existing devnode if present.

Arguments:

    HardwareIdList - Supplies a string containing a hardware ID to 
    be associated with the device.

Return Value:

    The function returns TRUE if it is successful. 

    Otherwise it returns FALSE and the logged error can be retrieved 
    with a call to GetLastError. 

    The most common error will be ERROR_NO_MORE_ITEMS, which means the 
    function could not find a devnode with the HardwareID.
------------------------------------------------------------------------------*/
BOOL WINAPI FindAndRemoveExistingDevice( LPSTR HardwareId )
{
	HDEVINFO			DeviceInfoSet;
	SP_DEVINFO_DATA		DeviceInfoData;
	DWORD				i, err;
//	BOOL				found = false;

	OutputDebugString( "FindAndRemoveExistingDevice:" );
	OutputDebugString( HardwareId );

	//
	// Create a Device Information Set with all present devices.
	//
	DeviceInfoSet = SetupDiGetClassDevs( NULL, 0, 0, DIGCF_ALLCLASSES ); // All devices present or not present on system
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//
	// Enumerate through all Devices.
	//
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo( DeviceInfoSet, i , &DeviceInfoData ); i++)
	{
		DWORD	DataT;
		PSTR	p, buffer = NULL;
		DWORD	buffersize = 0;

		//
		// We won't know the size of the HardwareID buffer until we call
		// this function. So call it with a null to begin with, and then 
		// use the required buffer size to Alloc the nessicary space.
		// Keep calling we have success or an unknown failure.
		//
		while (!SetupDiGetDeviceRegistryProperty(
					DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,
					&DataT, (PBYTE)buffer, buffersize, &buffersize))
		{
			if (GetLastError() == ERROR_INVALID_DATA)
			{
				// May be a Legacy Device with no HardwareID. Continue.
				break;
			}
			else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				// We need to change the buffer size.
				if (buffer)
				{
					LocalFree(buffer);
				}
				buffer = (char*)LocalAlloc(LPTR,buffersize);
			}
			else
			{
				// Unknown Failure.
				goto cleanup_DeviceInfo;
			}            
		}

		if (GetLastError() == ERROR_INVALID_DATA) 
			continue;

		//
		// Compare each entry in the buffer multi-sz list with our HardwareID.
		//
		for (p = buffer; *p && (p < &buffer[buffersize]); p += lstrlen(p) + sizeof(CHAR))
		{
			if (!_stricmp( HardwareId, p ))
			{
				if (SetupDiRemoveDevice( DeviceInfoSet, &DeviceInfoData ))
				{
					// Device removed
					OutputDebugString( HardwareId );
				}
				else
				{
					// Device not removed
					return FALSE;
				}
//				found = TRUE;
				break;
			}
		}

		if (buffer)
			LocalFree(buffer);

//		if (found)
//			break;
	}

cleanup_DeviceInfo:
	err = GetLastError();
	SetupDiDestroyDeviceInfoList( DeviceInfoSet );
	SetLastError( err );

	return (err == NO_ERROR); //???
}



/*------------------------------------------------------------------------------
Routine Description:

    This routine finds an existing devnode if present.

Arguments:

    HardwareIdList - Supplies a string containing a hardware ID to 
    be associated with the device.

Return Value:

    The function returns TRUE if it is successful. 

    Otherwise it returns FALSE and the logged error can be retrieved 
    with a call to GetLastError. 

    The most common error will be ERROR_NO_MORE_ITEMS, which means the 
    function could not find a devnode with the HardwareID.
------------------------------------------------------------------------------*/
BOOL WINAPI FindExistingDevice( LPSTR HardwareId )
{
	HDEVINFO			DeviceInfoSet;
	SP_DEVINFO_DATA		DeviceInfoData;
	DWORD				i, err;
	BOOL				found = FALSE;

	OutputDebugString( "FindExistingDevice:" );
	OutputDebugString( HardwareId );

	//
	// Create a Device Information Set with all present devices
	//
	DeviceInfoSet = SetupDiGetClassDevs( NULL, 0, 0, DIGCF_ALLCLASSES | DIGCF_PRESENT ); 
	if (DeviceInfoSet == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//
	//  Enumerate through all devices
	//
	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	for (i = 0; SetupDiEnumDeviceInfo( DeviceInfoSet, i, &DeviceInfoData ); i++)
	{
		DWORD	DataT;
		PSTR	p, buffer = NULL;
		DWORD	buffersize = 0;

		//
		// We won't know the size of the HardwareID buffer until we call
		// this function. So call it with a null to begin with, and then 
		// use the required buffer size to Alloc the nessicary space.
		// Keep calling we have success or an unknown failure.
		//
		while (!SetupDiGetDeviceRegistryProperty(
					DeviceInfoSet, &DeviceInfoData, SPDRP_HARDWAREID,
					&DataT, (PBYTE)buffer, buffersize, &buffersize)
			  )
		{
			if (GetLastError() == ERROR_INVALID_DATA)
			{
				// May be a Legacy Device with no HardwareID. Continue.
				break;
			}
			else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				// We need to change the buffer size.
				if (buffer)
				{
					LocalFree( buffer );
				}
				buffer = (char*)LocalAlloc( LPTR, buffersize );
			}
			else
			{
				// Unknown failure
				goto cleanup_DeviceInfo;
			}         
		}

		if (GetLastError() == ERROR_INVALID_DATA) 
			continue;

		//
		// Compare each entry in the buffer multi-sz list with our HardwareID.
		//
		for (p = buffer; *p && (p < &buffer[buffersize]); p += lstrlen(p) + sizeof(CHAR))
		{
			OutputDebugString( p );
			if (_stricmp( HardwareId, p ) == 0)
			{
				found = TRUE;
				break;
			}
		}

		if (buffer)
			LocalFree( buffer );

		if (found)
			break;
	}

cleanup_DeviceInfo:
	err = GetLastError();
	SetupDiDestroyDeviceInfoList( DeviceInfoSet );
	SetLastError( err );

	return found;
}



BOOL WINAPI DeleteReadOnlyFile( CHAR* lpFilename )
{
	BOOL	result = FALSE;

	OutputDebugString( "DeleteReadOnlyFile:" );
	OutputDebugString( lpFilename );

	if (!SetFileAttributes( lpFilename, FILE_ATTRIBUTE_NORMAL ))
	{
		OutputDebugString( "Failed to set attributes of the file" );
	}

	if (!DeleteFile( lpFilename ))
	{
		OutputDebugString( "Failed to delete file" );
	}
	else
	{
		OutputDebugString( "Delete done successfully." );
		result = TRUE;
	}

	return result;
}



/*------------------------------------------------------------------------------
*  Name : DeleteAllFiles
*
*  Description : It deletes all files in the list pFiles, If Dlls are not required
*				to be Unregistered pass a FALSE as bUnregister 
*
*  Parameters : pFiles - list (STL) - List of files to be Deleted.
*				bUnregister - Whether we want to unregister the DLLs. (Default : TRUE)
*						
*	 Returns  : True a normal return
*				False 
*
*  Globals variables  : none
------------------------------------------------------------------------------*/
BOOL DeleteAllFiles( LSTSTR pFiles, BOOL bUnregister )
{
	list <string>::iterator		sBegin, sEnd; 
	string						sExtension;
	UINT						uLength = 0;

	OutputDebugString( "DeleteAllFiles: " );

	for (sBegin = pFiles.begin(); sBegin != pFiles.end(); sBegin++)
	{
		// Whether to Unregister Dlls ?
		if (bUnregister)
		{
			// Check the extension of the file 
			uLength = lstrlen( &((string)*sBegin)[0] );

			sExtension = &((string)*sBegin)[uLength - 3];

			if (lstrcmpi( &sExtension[0], "DLL" ) == 0)
			{
				HINSTANCE hLib = LoadLibrary( &((string)*sBegin)[0] );

				if (hLib)
				{
					FARPROC Unregister = GetProcAddress( hLib, "DllUnregisterServer" );

					if (Unregister != NULL)
					{
						 // Unregister the Dll
						 (Unregister)();
					}

					FreeLibrary( hLib );
				}
			}
		}

		OutputDebugString( "Deleting file: " );
		OutputDebugString( &((string)*sBegin)[0] );

		if (!SetFileAttributes( (&((string)*sBegin)[0]), FILE_ATTRIBUTE_NORMAL ))
		{
			OutputDebugString( "Failed to set attributes of the file" );
		}

		if (!DeleteFile( &((string)*sBegin)[0] ))
		{
			OutputDebugString( "Failed to delete file" );
		}
		else
		{
			OutputDebugString( "Delete done successfully." );	
		}
	}

	return TRUE;
}



/*------------------------------------------------------------------------------
*  Name : SearchFilesForAString
*
*  Description : It searches for files with a pattern like *.inf,
*				 It Enumerates through the files in the directory
*				 It then calls IsStringPresentInFile to find whether the inf has a specified
*				 key in a section, which are defined.																	 
*
*  Parameters : sPaternToSearch : Absolute Path + Pattern (ex: C:\\Windows\\Inf\\*.inf)
*				sPath			: Absolute Path only
*				sFilesFound		: List of files found, STL <list>
*				numFilesFound	: not used
*			
*	 Returns  : True if success
*				False if failed
*
*  Globals variables  : none
------------------------------------------------------------------------------*/
BOOL SearchFilesForAString( string sPaternToSearch,string sPath,
							LSTSTR *sFilesFound, UINT* numFilesFound,
							char* pStringToSearch,LPSTR InfSectionName,
							LPSTR InfKeyName )
{
	WIN32_FIND_DATA		win32FindData;
	HANDLE				hSearch = NULL;
	string				tempStr;

	OutputDebugString( "Find First" );

	hSearch = FindFirstFile( &sPaternToSearch[0], &win32FindData );
	if (hSearch == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	tempStr = sPath + win32FindData.cFileName;   

	if (IsStringPresentInFile( &tempStr[0], InfSectionName, InfKeyName, pStringToSearch ))
	{
		OutputDebugString( "Found file : " );
		OutputDebugString( tempStr.c_str() );
		sFilesFound->push_back( tempStr );
	}

	while (FindNextFile( hSearch, &win32FindData ))
	{
		tempStr = sPath + win32FindData.cFileName;
		if (IsStringPresentInFile( &tempStr[0], InfSectionName, InfKeyName, pStringToSearch ))
		{
			OutputDebugString( "Found file : " );
			OutputDebugString( tempStr.c_str() );
			sFilesFound->push_back( tempStr );
		}
	}

	FindClose( hSearch );

	return TRUE;
}



/*------------------------------------------------------------------------------
*  Name : IsStringPresentInFile
*
*  Description : It finds the KEY_NAME in SECTION_NAME of an Inf file
*
*  Parameters : sFilename :: Inf file in which to search the string. 
*
*	 Returns		: True if the key found in the inf
*								False if key not found
*
*  Globals variables  : none
------------------------------------------------------------------------------*/

BOOL IsStringPresentInFile( char* sFilename, char* SectionName, char* KeyName, char* stringToSearch )
{
	char	returnedStr[MAX_PATH] = {0};
	UINT	numCharCopied = 0;
	UINT	lengthStr = lstrlen( stringToSearch );

	numCharCopied = GetPrivateProfileString( SectionName, KeyName, NULL, returnedStr, MAX_PATH, sFilename );

	if ((numCharCopied == lengthStr) && !lstrcmpi( returnedStr, stringToSearch ))
	{
		return TRUE;
	}

	return FALSE;
}



BOOL WINAPI DeleteThisService( LPSTR ServiceName ) 
{ 
	SC_HANDLE	schSCManager;
	SC_HANDLE	schService;
	BOOL		result = FALSE;

	schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );
	if (!schSCManager)
	{
		OutputDebugString( "Failed to open Service Manager" );
		goto cleanup;
	}
	OutputDebugString( "Opened Service Manager" );

	schService = OpenService( schSCManager, ServiceName, DELETE );  // only need DELETE access 
	if (schService == NULL)
	{
		OutputDebugString( "Failed to open Service!" );
		goto cleanup;
	}
	OutputDebugString( "Opened Service" );

	if (!DeleteService(schService))
	{
		OutputDebugString( "Failed to Delete Service!" );
		goto cleanup;
	}
	OutputDebugString( "Deleted Service" );
	result = TRUE;

cleanup:
	if (schService)
	{
		OutputDebugString( "Closing Service" );
		CloseServiceHandle( schService );
	}
	if (schSCManager)
	{
		OutputDebugString( "Closing Service Manager" );
		CloseServiceHandle( schSCManager );
	}

	return result;
}


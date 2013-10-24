//==============================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//==============================================================================

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include "DifxAPI.h"


int _tmain( int argc, _TCHAR* argv[] )
{
	DWORD		retValue = 0;
	BOOL		needToReboot = false;
	_TCHAR		str[256];

	OutputDebugStringW( L"Vista64Install args:" );
	OutputDebugStringW( argv[1] );
	OutputDebugStringW( argv[2] );

	if (!argc)
		return 0;

	// check if we want to install or uninstall
	if (lstrcmpi( argv[1], (LPCWSTR)"I" ) == 0)
	{
		retValue = DriverPackageInstall( argv[2], (DRIVER_PACKAGE_LEGACY_MODE | DRIVER_PACKAGE_FORCE), 0, &needToReboot );
		wsprintf( str, L"Return Value Install: 0x%X\n", retValue );
		OutputDebugStringW( str );
	}
	else if (lstrcmpi( argv[1], (LPCWSTR)"U" ) == 0)
	{
		retValue = DriverPackageUninstall( argv[2], (DRIVER_PACKAGE_DELETE_FILES | DRIVER_PACKAGE_FORCE), 0, &needToReboot );
		wsprintf( str, L"Return Value Uninstall: 0x%X\n", retValue );
		OutputDebugStringW( str );
	}

	return 0;
}

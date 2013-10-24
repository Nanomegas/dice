///////////////////////////////////////////////////////////////////////////////
//
//	see cplApplet.h
//
#include "windows.h"
#include "cplApplet.h"
#include "tcat.h"
#include "tcat_dice_defaults.h"
#include "tcat_dice.h"
#include "tcat_dice_win.h"
#include "resource.h"

#define BUFSIZE 256

///////////////////////////////////////////////////////////////////////////////
//
// DLL Main Entry point
//
///////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /* lpvReserved */)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		CPLWrapperApplet::SetInstanceHandle(hInstance);
	}

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//
// The only required export used by Control.exe
//
///////////////////////////////////////////////////////////////////////////////
LONG CALLBACK CPlApplet( HWND hWnd, UINT uMsg, LPARAM lParam1, LPARAM lParam2 )
{
	LONG lResult = 0;

	switch( uMsg )
	{
		case CPL_INIT:
			lResult = 1;
			break;

		case CPL_GETCOUNT:
			lResult = 1;
			break;

		case CPL_INQUIRE:
			lResult = CPLWrapperApplet::Inquire((LONG)lParam1, (LPCPLINFO)lParam2);
			break;

		case CPL_NEWINQUIRE:
			lResult = CPLWrapperApplet::NewInquire((LONG)lParam1, (LPNEWCPLINFO)lParam2);
			break;

		case CPL_SELECT:
			lResult = 1;
			break;

		case CPL_DBLCLK :
			lResult = CPLWrapperApplet::DoubleClick(hWnd, (LONG)lParam1, (LONG)lParam2);
			break;

#if(WINVER >= _WIN32_WINNT)
		case CPL_STARTWPARMS:           // likely started by a shell customization
			lResult = CPLWrapperApplet::StartWithParams(hWnd, (LONG)lParam1, (LPSTR)lParam2);
			break;
#endif

		case CPL_STOP:					// chance to clean up before dll decrement ref count
			lResult = 1;
			break;

		case CPL_EXIT:                  // DLL decrement ref count
			lResult = 1;
			break;

		default:
			break;
	}

	return lResult;
}

///////////////////////////////////////////////////////////////////////////////
// CPLWrapperApplet
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Static member variables
///////////////////////////////////////////////////////////////////////////////

CPLWrapperApplet *CPLWrapperApplet::s_pWrapperApplet = 0;

HINSTANCE CPLWrapperApplet::s_hInstance = (HINSTANCE)INVALID_HANDLE_VALUE;

///////////////////////////////////////////////////////////////////////////////
// Construction and destruction
///////////////////////////////////////////////////////////////////////////////

CPLWrapperApplet::CPLWrapperApplet(
   int icon, 
   int name, 
   int descr)
   :  m_icon(icon), 
      m_name(name), 
      m_desc(descr)
{
	s_pWrapperApplet = this;
}

CPLWrapperApplet::~CPLWrapperApplet()
{
	s_pWrapperApplet = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Static members
///////////////////////////////////////////////////////////////////////////////

void CPLWrapperApplet::SetInstanceHandle(HINSTANCE hInstance)
{
   s_hInstance = hInstance;
}

LONG CPLWrapperApplet::Inquire(LONG appletIndex, LPCPLINFO lpCPlInfo)
{
	if ( (0 == appletIndex) && (s_pWrapperApplet) )
	{
		lpCPlInfo->lData = 0;
		lpCPlInfo->idIcon = s_pWrapperApplet->m_icon;
		lpCPlInfo->idName = s_pWrapperApplet->m_name;
		lpCPlInfo->idInfo = s_pWrapperApplet->m_desc;

		return 0;
	}

   return 1;
}

LONG CPLWrapperApplet::NewInquire(LONG appletIndex, LPNEWCPLINFO lpNewCPlInfo)
{
	if ( (0 == appletIndex) && (s_pWrapperApplet) )
	{
		if (lpNewCPlInfo->dwSize == sizeof(NEWCPLINFO))
		{
			lpNewCPlInfo->dwSize = (DWORD)sizeof(NEWCPLINFO);
			lpNewCPlInfo->dwFlags = 0;
			lpNewCPlInfo->dwHelpContext = 0;
			lpNewCPlInfo->lData = 0;
			lpNewCPlInfo->hIcon = LoadIcon( s_hInstance, MAKEINTRESOURCE( s_pWrapperApplet->m_icon ) );
			lpNewCPlInfo->szHelpFile[ 0 ] = '\0';

			LoadString( s_hInstance, s_pWrapperApplet->m_name, lpNewCPlInfo->szName, 32 );
			LoadString( s_hInstance, s_pWrapperApplet->m_desc, lpNewCPlInfo->szInfo, 64 );

			return 0;
		}
	}

	return 1;
}

LONG CPLWrapperApplet::DoubleClick(
   HWND hWnd, 
   LONG appletIndex, 
   LONG appletData)
{
	if (s_pWrapperApplet && s_pWrapperApplet->OnDoubleClick(hWnd, appletData))
	{
		return 0;
	}

	return 1;
}

LONG CPLWrapperApplet::StartWithParams(
   HWND hWnd, 
   LONG appletIndex, 
   LPSTR params)
{
	if ( (0 == appletIndex) && (s_pWrapperApplet) )
	{
		if (s_pWrapperApplet->OnDoubleClick(hWnd, 0))
		{
			return 0;
		}
	}

	return 1;
}

class WrapperApplet : public CPLWrapperApplet 
{
	public :
		WrapperApplet() : CPLWrapperApplet(ID_ICON, ID_NAMESTRING, ID_DESCSTRING) {}

		BOOL OnDoubleClick(HWND hWnd, LONG appletData)
		{
			HKEY hKey;
			char szErrStr[BUFSIZE] = "Could not find the application.\nCheck your driver installation.";
			char szRegStr[BUFSIZE];
			DWORD dwBufLen=BUFSIZE;

			wsprintf(szRegStr, "SOFTWARE\\ASIO\\%s", kTCAT_DICE_DRV_STR);

			LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szRegStr, 0, KEY_QUERY_VALUE, &hKey);
			if ( lRet != ERROR_SUCCESS)
			{
				MessageBox(hWnd, szErrStr, "Control Panel", MB_ICONINFORMATION);
				return FALSE;
			}

			lRet = RegQueryValueEx( hKey, kTCAT_DICE_CPL_PATH_STR, NULL, NULL, (LPBYTE) szRegStr, &dwBufLen);
			 if( (lRet != ERROR_SUCCESS) || (dwBufLen > BUFSIZE) )
			{
				MessageBox(hWnd, szErrStr, "Control Panel",  MB_ICONINFORMATION);
				return FALSE;
			}

			RegCloseKey( hKey );

			ShellExecute(hWnd, "open", szRegStr, "", "", SW_SHOWNORMAL);

			return TRUE;
		}
};

WrapperApplet cplApplet;

# pragma once
///////////////////////////////////////////////////////////////////////////////
// Platform SDK include
///////////////////////////////////////////////////////////////////////////////
#include "cpl.h"

///////////////////////////////////////////////////////////////////////////////
// Entry point
///////////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved);

///////////////////////////////////////////////////////////////////////////////
// The one export required
///////////////////////////////////////////////////////////////////////////////
LONG CALLBACK CPlApplet(HWND hWnd, UINT uMsg, LPARAM lParam1, LPARAM lParam2);

///////////////////////////////////////////////////////////////////////////////
// Applet Base class
///////////////////////////////////////////////////////////////////////////////

class CPLWrapperApplet
{
public: 
	CPLWrapperApplet(int nIconID, int nNameID, int nDescID);      
	virtual ~CPLWrapperApplet();
	virtual BOOL OnDoubleClick(HWND hWnd, LONG appletData) = 0;

private:
	friend BOOL WINAPI ::DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved);
	friend LONG CALLBACK ::CPlApplet(HWND hWnd, UINT uMsg, LPARAM lParam1, LPARAM lParam2);

	// instance variables
	int m_icon;
	int m_name;
	int m_desc;

	// pointer to currently indexed instance
	CPLWrapperApplet *m_pNext;

	// Static access functions
	static void SetInstanceHandle(HINSTANCE hInstance);

	// message handlers for host app (control.exe)
	static LONG Inquire(LONG appletIndex, LPCPLINFO lpCPlInfo);
	static LONG NewInquire(LONG appletIndex, LPNEWCPLINFO lpCPlInfo);
	static LONG DoubleClick(HWND hWnd, LONG lParam1, LONG lParam2);
	static LONG StartWithParams(HWND hWnd, LONG lParam1, LPSTR lParam2);

	// Static applet data 

	static HINSTANCE s_hInstance;
	static CPLWrapperApplet *s_pWrapperApplet;

	// (prevent copy constructor and operator= being generated..)
	CPLWrapperApplet(const CPLWrapperApplet&);
	CPLWrapperApplet &operator= (const CPLWrapperApplet&);
};

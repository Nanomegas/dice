///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "time.h"

#include "DevsXmlFile.h"

// import with no smart pointers, rename colliding keywords and symbols
//   avoids need for headers, otherwise would require MSXML6 SDK installed
#import <msxml6.dll> raw_interfaces_only rename("value","_value") rename("vararg","_vararg")

// Macro that calls a COM method returning HRESULT value:
#define HRCALL(a, errmsg) \
do { \
    hr = (a); \
    if (FAILED(hr)) { \
        dprintf( "%s:%d  HRCALL Failed: %s\n  0x%.8x = %s\n", \
                __FILE__, __LINE__, errmsg, hr, #a ); \
        goto clean; \
    } \
} while (0)

// vararg below causes spurious compiler warning
#pragma warning( disable : 4793 )

// Helper function that put output in stdout and debug window
// in Visual Studio:
void dprintf( char * format, ...)
{
	static char buf[1024];
    va_list args;
    va_start( args, format );
    vsprintf_s( buf, format, args );
    va_end( args);
    OutputDebugStringA( buf);
    printf("%s", buf);
}

// Helper function to create a DOM instance: 
MSXML2::IXMLDOMDocument * DomFromCOM()
{
    HRESULT hr;
    MSXML2::IXMLDOMDocument *pxmldoc = NULL;

    HRCALL( CoCreateInstance(__uuidof(DOMDocument),
                    NULL,
                    CLSCTX_INPROC_SERVER,
                    __uuidof(MSXML2::IXMLDOMDocument),
                    (void**)&pxmldoc),
                    "Create a new DOMDocument");

    HRCALL( pxmldoc->put_async(VARIANT_FALSE),
            "should never fail");
    HRCALL( pxmldoc->put_validateOnParse(VARIANT_FALSE),
            "should never fail");
    HRCALL( pxmldoc->put_resolveExternals(VARIANT_FALSE),
            "should never fail");

    return pxmldoc;
clean:
    if (pxmldoc)
    {
        pxmldoc->Release();
    }
    return NULL;
}

VARIANT VariantString(BSTR str)
{
    VARIANT var;
    VariantInit(&var);
    V_BSTR(&var) = SysAllocString(str);
    V_VT(&var) = VT_BSTR;
    return var;
}

void ReportParseError(MSXML2::IXMLDOMDocument *pDom, char *desc) {
    MSXML2::IXMLDOMParseError *pXMLErr=NULL;
    BSTR bstrReason = NULL;
    HRESULT hr;
    HRCALL(pDom->get_parseError(&pXMLErr),
                "dom->get_parseError: ");
    HRCALL(pXMLErr->get_reason(&bstrReason),
                "parseError->get_reason: ");

    dprintf("%s %S\n",desc, bstrReason);
clean:
    if (pXMLErr) pXMLErr->Release();
    if (bstrReason) SysFreeString(bstrReason);
}

static SYSTEMTIME gLastLoadTime = { 0 };

long getDBCacheAge(void)
{
	SYSTEMTIME curTime = { 0 };
	long lPrevTime, lCurTime = 0;

	lPrevTime = (gLastLoadTime.wDay*24*60) + (gLastLoadTime.wHour*60) + gLastLoadTime.wMinute;

	GetLocalTime(&curTime);
	lCurTime = (curTime.wDay*24*60) + (curTime.wHour*60) + curTime.wMinute;

	return (lCurTime - lPrevTime);
}

static MSXML2::IXMLDOMDocument *gpXMLDom=NULL;

void devsXMLInitialize(void)
{
	gpXMLDom = DomFromCOM();
}

void devsXMLUninitialize(void)
{
	if (gpXMLDom) gpXMLDom->Release();
}

long findControllerInfoXML(fwi::OHCI1394_DEV_INFO * devInfo)
{
    MSXML2::IXMLDOMNodeList *pNodes=NULL;
    MSXML2::IXMLDOMNode *pNode=NULL;
	static VARIANT_BOOL status;
	static VARIANT var;
    BSTR bstr = NULL;
    HRESULT hr;
    long num_controllers;
    long num_attributes;
	BOOL bVendorMatch = FALSE;
	BOOL bDeviceMatch = FALSE;
	long lMinutes = 0;

	MSXML2::IXMLDOMElementPtr pXMLDocElement = NULL;
	MSXML2::IXMLDOMNodeListPtr pXMLDomNodeList = NULL;

    VariantInit(&var);

	// the URL below has a server-side redirect to the actual xml file in the svn repo
	var = VariantString(L"http://www.tctechnologies.tc/appdata/ohci1394db.xml");

	// reload online file if cached data is stale
	lMinutes = getDBCacheAge();
	if ( (lMinutes < 0) || (lMinutes > 10) )
	{
		if (gpXMLDom) gpXMLDom->Release();
		gpXMLDom = DomFromCOM();
		if (!gpXMLDom) return 1;

	    HRCALL(gpXMLDom->load(var, &status), "dom->load(): ");

		if (status!=VARIANT_TRUE)
		{
			if (&var) VariantClear(&var);

			return FWI_ERROR_FILE_NOT_FOUND;
		}
		else
		{
			GetLocalTime(&gLastLoadTime);
		}
	}

	gpXMLDom->get_documentElement(&pXMLDocElement);
	pXMLDocElement->selectNodes(TEXT("//controller"), &pXMLDomNodeList);
	pXMLDomNodeList->get_length(&num_controllers);

	for (long n=0; n<num_controllers; n++)
	{
		TCHAR buf[32];
		StringCchPrintf( buf, 20, TEXT("//controller[%i]/*"), n);

		// Query the node-set.
		HRCALL(gpXMLDom->selectNodes(buf, &pNodes), "selectNodes ");
		if (!pNodes) {
			ReportParseError(gpXMLDom, "Error while calling selectNodes ");
		}
		else {
			HRCALL(pNodes->get_length(&num_attributes), "get_length: ");
			for (long i=0; i<num_attributes; i++) {
				if (pNode) pNode->Release();
				HRCALL(pNodes->get_item(i, &pNode), "get_item: ");
				if (bstr) SysFreeString(bstr);
				HRCALL(pNode->get_nodeName(&bstr), "get_nodeName: ");

				if (_tcsicmp((TCHAR*)bstr,TEXT("vendorid"))==0)
				{
					BSTR nodeTextStr;
					HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
					if (_tcsicmp((TCHAR*)nodeTextStr, devInfo->vendorId)==0)
					{
						bVendorMatch = TRUE;
					}
					SysFreeString(nodeTextStr);
				}
				else if (_tcsicmp((TCHAR*)bstr,TEXT("deviceid"))==0)
				{
					BSTR nodeTextStr;
					HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
					if (_tcsicmp((TCHAR*)nodeTextStr, devInfo->deviceId)==0)
					{
						bDeviceMatch = TRUE;
					}
					SysFreeString(nodeTextStr);
				}
				
				if (bVendorMatch)
				{
					if ( (_tcsicmp((TCHAR*)bstr,TEXT("vendorname"))==0) && (_tcsicmp(TEXT("Unknown"),&devInfo->vendorName[0]))==0 )
					{
						BSTR nodeTextStr;
						HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
						_tcscpy_s(&devInfo->vendorName[0], VENDOR_SIZE, nodeTextStr);
						SysFreeString(nodeTextStr);
					}
				}

				if (bVendorMatch && bDeviceMatch)
				{
					devInfo->bFound = TRUE;
					if (_tcsicmp((TCHAR*)bstr,TEXT("chipset"))==0)
					{
						BSTR nodeTextStr;
						HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
						_tcscpy_s(&devInfo->chipset[0], CHIPSET_SIZE, nodeTextStr);
						SysFreeString(nodeTextStr);
					}
					else if (_tcsicmp((TCHAR*)bstr,TEXT("maxtx"))==0)
					{
						BSTR nodeTextStr;
						HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
						_tcscpy_s(&devInfo->maxTx[0], NUM_STR_SIZE, nodeTextStr);
						SysFreeString(nodeTextStr);
					}
					else if (_tcsicmp((TCHAR*)bstr,TEXT("maxrx"))==0)
					{
						BSTR nodeTextStr;
						HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
						_tcscpy_s(&devInfo->maxRx[0], NUM_STR_SIZE, nodeTextStr);
						SysFreeString(nodeTextStr);
					}
					else if (_tcsicmp((TCHAR*)bstr,TEXT("maxspeed"))==0)
					{
						BSTR nodeTextStr;
						HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
						swscanf_s(nodeTextStr, TEXT("%04d"), &devInfo->maxspeed);
						SysFreeString(nodeTextStr);
					}
					else if (_tcsicmp((TCHAR*)bstr,TEXT("notes"))==0)
					{
						BSTR nodeTextStr;
						HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
						_tcscpy_s(&devInfo->notes[0], NOTES_SIZE, nodeTextStr);
						SysFreeString(nodeTextStr);
					}
					else if (_tcsicmp((TCHAR*)bstr,TEXT("support"))==0)
					{
						BSTR nodeTextStr;
						HRCALL(pNode->get_text(&nodeTextStr), "get_text: ");
						swscanf_s(nodeTextStr, TEXT("%04x"), &devInfo->support);
						devInfo->bValid = true;
						SysFreeString(nodeTextStr);
						if (&var) VariantClear(&var);
						if (bstr) SysFreeString(bstr);
						if (pNodes) pNodes->Release();
						if (pNode) pNode->Release();

						return FWI_NO_ERROR;
					}
				}
			}
		}
	}
clean:
	if (&var) VariantClear(&var);
	if (bstr) SysFreeString(bstr);
	if (pNodes) pNodes->Release();
	if (pNode) pNode->Release();

	return FWI_ERROR_END_OF_FILE;
}


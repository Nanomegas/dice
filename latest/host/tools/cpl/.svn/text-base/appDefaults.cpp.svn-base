///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2008 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "appDefaults.h"
#include "DefaultsFile.h"

// Application Defaults

//==============================================================================
AppDefaults::AppDefaults()
    : appProps (0),
      defaultProps (0)
{
}

AppDefaults::~AppDefaults()
{
    closeFiles();
}

//==============================================================================
void AppDefaults::setStorageParameters (const String& fileNamePrefix,
										const String& fileNameSuffix) throw()
{
    filePrefix = fileNamePrefix;
    fileSuffix = fileNameSuffix;
}

//==============================================================================
void AppDefaults::openFiles()
{
    // You need to call setStorageParameters() before trying to get hold of the
    // properties!
    jassert (filePrefix.isNotEmpty());
	
    if (filePrefix.isNotEmpty())
    {
        if (appProps == 0)
		{
			appProps = DefaultsFile::createDefaultAppSettingsFile (filePrefix, fileSuffix);
		}
		
		// application should add default values to this backup property set somewhere
		if (defaultProps == 0)
		{
			defaultProps = new PropertySet(true);
		}
        appProps->setFallbackPropertySet (defaultProps);
    }
}

PropertySet* AppDefaults::getDefaultSettings()
{
    if (defaultProps == 0)
        openFiles();
	
    return defaultProps;
}

DefaultsFile* AppDefaults::getAppSettings()
{
    if (appProps == 0)
        openFiles();
	
    return appProps;
}

void AppDefaults::closeDefaultsFile()
{
}

void AppDefaults::closeFiles()
{
    deleteAndZero (defaultProps);
}

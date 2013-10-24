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

#pragma once

#include "JuceHeader.h"
#include "DefaultsFile.h"

//==============================================================================
/**
 Modelled after juce::ApplicationProperties
 
 Manages a collection of properties.
 
 This is a slightly higher-level wrapper for PropertiesFile.
 
 It holds a DefaultsFile object and a PropertyList object internally. The 
 DefaultsFile object gives access to settings in a file that control how the
 application behaves and which features are visible at runtime. Settings in the
 file override defaults used in the PropertyList.
 
 The PropertyList contains default settings that are used when the defaults file 
 is not present, and is configurd at runtime in the application initialise().
 
 @see PropertiesFile
*/
class AppDefaults
{
public:
    //==============================================================================
    /**
	 Creates an AppDefaults object.
	 
	 Before using it, you must call setStorageParameters() to give it the info
	 it needs to find the defaults property file.
	 */
    AppDefaults();
	
    /** Destructor.
	 */
    ~AppDefaults();
	
	
    //==============================================================================
    /** Gives the object the information it needs to find the defaults property file.
	 
	 See the comments for PropertiesFile::createDefaultAppPropertiesFile() for more
	 info about how these parameters are used.
	 */
    void setStorageParameters (const String& fileNamePrefix,
                               const String& fileNameSuffix) throw();
	
    //==============================================================================
    /** Returns the app properties file.
	 
	 The first time this is called, it will load the properties file.
	 
	 Note that when you search the application DefaultsFile for a value that it doesn't 
	 contain, the default settings are used as a second-chance place to look. This is done 
	 via the PropertySet::setFallbackPropertySet() method - by default the default settings 
	 are set to the fallback for the app settings. Fallback settings are added in the 
	 app initialise() function.
	 
	 @see getDefaultSettings
	 */
    DefaultsFile* getAppSettings();
	
	//==============================================================================
    /** Returns the defaults settings PropertyList.
	 
	 When you search the user PropertiesFile for a value that it doesn't contain,
	 the default settings are used as a second-chance place to look. By default the 
	 default settings are set to the fallback for the app settings.
	 
	 Use this function to get a pointer to the defaults, and add new settings 
	 programmatically:
	 
	 PropertySet * defaults = appDefaults->getDefaultSettings();
	 defaults->setValue("mySetting1", true);
	 defaults->setValue("mySetting2", "two");
	 
	 @see getAppSettings
	 */
	PropertySet* getDefaultSettings();
	
   	/** Flushes and closes the Defaults file if it is open.
	 
	 This flushes any pending changes to disk with PropertiesFile::saveIfNeeded()
	 and closes both files. It will then be re-opened the next time 
	 getDefaultSettings() is called.
	 */
    void closeDefaultsFile();
	
	/** Flushes and closes both files if they are open.
	 
	 This flushes any pending changes to disk with PropertiesFile::saveIfNeeded()
	 and closes both files. They will then be re-opened the next time getAppSettings()
	 or getDefaultSettings() is called.
	 */
    void closeFiles();
	
	
    //==============================================================================
    juce_UseDebuggingNewOperator
	
private:
    //==============================================================================
    ScopedPointer <DefaultsFile> appProps;
    PropertySet* defaultProps;
	
    String filePrefix, fileSuffix;
	
    void openFiles();
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AppDefaults);
};

extern AppDefaults* appDefaults;

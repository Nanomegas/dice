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
#include "eapPropertiesFile.h"


//==============================================================================
/**
	Modelled after juce::ApplicationProperties

    Manages a collection of properties.

    This is a slightly higher-level wrapper for PropertiesFile, which can be used
    as a singleton.

    It holds a PropertiesFile object and a PropertyList object internally. The 
	PropertiesFile object gives access to settings in a file that control how the
	application behaves and which features are visible at runtime. Settings in the
	file override defaults used in the PropertyList.
	
	The PropertyList contains default settings that are used when the defaults file 
	is not present, and is configurd at runtime in the application initialise().

    If you're using one of these as a singleton, then your app's start-up code should
    first of all call setStorageParameters() to tell it the parameters to use to find
    the defaults file.

    @see PropertiesFile
*/
class EapProperties : public DeletedAtShutdown
{
public:
    //==============================================================================
    /**
        Creates an EapDefaults object.

        Before using it, you must call setStorageParameters() to give it the info
        it needs to find the defaults property file.
    */
    EapProperties() throw();

    /** Destructor.
    */
    ~EapProperties();

    //==============================================================================
    juce_DeclareSingleton (EapProperties, false)

    //==============================================================================
    /** Gives the object the information it needs to find the defaults property file.

		See the comments for PropertiesFile::createDefaultAppPropertiesFile() for more
        info about how these parameters are used.
    */
    void setStorageParameters (const String& fileNamePrefix,
                               const String& fileNameSuffix,
                               const int millisecondsBeforeSaving,
                               const int propertiesFileOptions) throw();


      /** Tests whether the files can be successfully written to, and can show
        an error message if not.

        Returns true if none of the tests fail.

        @param testUserSettings     if true, the user settings file will be tested
        @param testCommonSettings   if true, the common settings file will be tested
        @param showWarningDialogOnFailure   if true, the method will show a helpful error
                                    message box if either of the tests fail
    */
    bool testWriteAccess (const bool showWarningDialogOnFailure);

	//==============================================================================
    /** Returns the app properties file.

        The first time this is called, it will load the properties file.
    */
    EapPropertiesFile* getEapSettings() throw();

	//==============================================================================
	/** Saves file if it needs to be saved.

		@see PropertiesFile::saveIfNeeded
	*/
	bool saveIfNeeded();

  	/** Flushes and closes the Properties file if it is open.

        This flushes any pending changes to disk with PropertiesFile::saveIfNeeded()
        and closes both files. It will then be re-opened the next time 
    */
    void closePropertiesFile();

	/** Flushes and closes both files if they are open.

        This flushes any pending changes to disk with PropertiesFile::saveIfNeeded()
        and closes both files. They will then be re-opened the next time getEapSettings()
		is called.
    */
    void closeFiles();


    //==============================================================================
    juce_UseDebuggingNewOperator

private:
    //==============================================================================
    EapPropertiesFile* eapProps;

    String filePrefix, fileSuffix, folderName;
    int msBeforeSaving, options;

    EapProperties (const EapProperties&);
    const EapProperties& operator= (const EapProperties&);

    void openFiles() throw();
};

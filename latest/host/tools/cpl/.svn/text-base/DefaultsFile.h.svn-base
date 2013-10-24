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

#ifndef __DEFAULTSFILE_H
#define __DEFAULTSFILE_H

#include "JuceHeader.h"

//==============================================================================
/** Wrapper on a file that stores a list of key/value data pairs.

	Based on juce::PropertiesFile

    Useful for storing application settings, etc. See the PropertySet class for
    the interfaces that read and write values.

    Not designed for very large amounts of data, as it keeps all the values in
    memory and writes them out to disk lazily when they are changed.

    @see PropertySet
*/
class JUCE_API  DefaultsFile  : public PropertySet
{
public:
    //==============================================================================
    /**
        Creates a DefaultsFile object.

        @param file                         the file to use
    */
    DefaultsFile (const File& file) throw();

    /** Destructor.
    */
    ~DefaultsFile();


    //==============================================================================
    /** Returns the file that's being used. */
    const File getFile() const throw();

    //==============================================================================
    /** Handy utility to create a properties file in whatever the standard OS-specific
        location is for these things.

        This uses getDefaultAppSettingsFile() to decide what file to create, then
        creates a DefaultsFile object with the specified properties. See
        getDefaultAppSettingsFile() and the class's constructor for descriptions of
        what the parameters do.

        @see getDefaultAppSettingsFile
    */
    static DefaultsFile* createDefaultAppSettingsFile (const String& fileNamePrefix,
                                                           const String& fileNameSuffix);

	/** Handy utility to choose a file in the standard OS-dependent location for application
        settings files.

		So on a Mac, this will return a file called:
        [PackageName]/Contents/Resources/[fileNamePrefix].[fileNameSuffix]

        On Windows it'll return something like:
        [ExecutableFolder]\\[fileNamePrefix].[fileNameSuffix]
    */
    static const File getDefaultAppSettingsFile (const String& FileNamePrefix,
                                                 const String& fileNameSuffix);

    //==============================================================================
    juce_UseDebuggingNewOperator


private:
    //==============================================================================
    File file;

    DefaultsFile (const DefaultsFile&);
    const DefaultsFile& operator= (const DefaultsFile&);
};

#endif   // __DEFAULTSFILE_H

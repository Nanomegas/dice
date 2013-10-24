///////////////////////////////////////////////////////////////////////////////
//
// The contents of this file are derived from the file juce_PropertiesFile.h, in
// the JUCE library. The JUCE copyright is included below.
//
///////////////////////////////////////////////////////////////////////////////

/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-7 by Raw Material Software ltd.

  ------------------------------------------------------------------------------

   JUCE can be redistributed and/or modified under the terms of the
   GNU General Public License, as published by the Free Software Foundation;
   either version 2 of the License, or (at your option) any later version.

   JUCE is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with JUCE; if not, visit www.gnu.org/licenses or write to the
   Free Software Foundation, Inc., 59 Temple Place, Suite 330,
   Boston, MA 02111-1307 USA

  ------------------------------------------------------------------------------

   If you'd like to release a closed-source product which uses JUCE, commercial
   licenses are also available: visit www.rawmaterialsoftware.com/juce for
   more information.

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

//==============================================================================
/** Wrapper on a file that stores a list of key/value data pairs.

    Useful for storing application settings, etc. See the PropertySet class for
    the interfaces that read and write values.

    Not designed for very large amounts of data, as it keeps all the values in
    memory and writes them out to disk lazily when they are changed.

    Because this class derives from ChangeBroadcaster, ChangeListeners can be registered
    with it, and these will be signalled when a value changes.

    @see PropertySet
*/
class JUCE_API  EapPropertiesFile  : public PropertySet,
                                  public ChangeBroadcaster,
                                  private Timer
{
public:
    //==============================================================================
    enum FileFormatOptions
    {
        ignoreCaseOfKeyNames            = 1,
        storeAsBinary                   = 2,
        storeAsCompressedBinary         = 4,
        storeAsXML                      = 8
    };

    //==============================================================================
    /**
        Creates a PropertiesFile object.

        @param file                         the file to use
        @param millisecondsBeforeSaving     if this is zero or greater, then after a value
                                            is changed, the object will wait for this amount
                                            of time and then save the file. If zero, the file
                                            will be written to disk immediately on being changed
                                            (which might be slow, as it'll re-write synchronously
                                            each time a value-change method is called). If it is
                                            less than zero, the file won't be saved until
                                            save() or saveIfNeeded() are explicitly called.
        @param options                      a combination of the flags in the FileFormatOptions
                                            enum, which specify the type of file to save, and other
                                            options.
    */
    EapPropertiesFile (const File& file,
                    const int millisecondsBeforeSaving,
                    const int options) throw();

    /** Destructor.

        When deleted, the file will first call saveIfNeeded() to flush any changes to disk.
    */
    ~EapPropertiesFile();

    //==============================================================================
    /** This will flush all the values to disk if they've changed since the last
        time they were saved.

        Returns false if it fails to write to the file for some reason (maybe because
        it's read-only or the directory doesn't exist or something).

        @see save
    */
    bool saveIfNeeded();

    /** This will force a write-to-disk of the current values, regardless of whether
        anything has changed since the last save.

        Returns false if it fails to write to the file for some reason (maybe because
        it's read-only or the directory doesn't exist or something).

        @see saveIfNeeded
    */
    bool save();

    /** Returns true if the properties have been altered since the last time they were
        saved.
    */
    bool needsToBeSaved() const throw();

    //==============================================================================
    /** Returns the file that's being used. */
    const File getFile() const throw();

    //==============================================================================
    /** Handy utility to create a properties file in whatever the standard OS-specific
        location is for these things.

        This uses getDefaultAppSettingsFile() to decide what file to create, then
        creates a PropertiesFile object with the specified properties. See
        getDefaultAppSettingsFile() and the class's constructor for descriptions of
        what the parameters do.

        @see getDefaultAppSettingsFile
    */
    static EapPropertiesFile* createDefaultAppPropertiesFile (const String& applicationName,
                                                           const String& fileNameSuffix,
                                                           const String& folderName,
                                                           const bool commonToAllUsers,
                                                           const int millisecondsBeforeSaving,
                                                           const int propertiesFileOptions);

    /** Handy utility to choose a file in the standard OS-dependent location for application
        settings files.


		So on a Mac, this will return a file called:
        [PackageName]/Contents/Resources/[fileNamePrefix].[fileNameSuffix]

        On Windows it'll return something like:
        [ExecutableFolder]\\[fileNamePrefix].[fileNameSuffix]
    */
    static const File getDefaultAppSettingsFile (const String& applicationName,
                                                 const String& fileNameSuffix,
                                                 const String& folderName,
                                                 const bool commonToAllUsers);

    //==============================================================================
    juce_UseDebuggingNewOperator


protected:
    virtual void propertyChanged();

private:
    //==============================================================================
    File file;
    int timerInterval;
    const int options;
    bool needsWriting;

    void timerCallback();

    EapPropertiesFile (const EapPropertiesFile&);
    const EapPropertiesFile& operator= (const EapPropertiesFile&);
};

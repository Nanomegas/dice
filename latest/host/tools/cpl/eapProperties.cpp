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

#include "eapPropertiesFile.h"
#include "eapProperties.h"

//==============================================================================
juce_ImplementSingleton (EapProperties)

//==============================================================================
EapProperties::EapProperties() throw()
    : eapProps (0),
      msBeforeSaving (-1)
{
}

EapProperties::~EapProperties()
{
    closeFiles();
    clearSingletonInstance();
}

//==============================================================================
void EapProperties::setStorageParameters (const String& fileNamePrefix,
                                          const String& fileNameSuffix,
                                          const int millisecondsBeforeSaving,
                                          const int propertiesFileOptions) throw()
{
    filePrefix = fileNamePrefix;
    fileSuffix = fileNameSuffix;
    msBeforeSaving = millisecondsBeforeSaving;
    options = propertiesFileOptions;
}

bool EapProperties::testWriteAccess (const bool showWarningDialogOnFailure)
{
    const bool writeOk = getEapSettings()->save();

    if (!writeOk)
    {
        if (showWarningDialogOnFailure)
        {
            String filename;

            if (eapProps != 0 && ! writeOk)
                filename << '\n' << eapProps->getFile().getFullPathName();

            AlertWindow::showMessageBox (AlertWindow::WarningIcon,
                                         filePrefix + TRANS(" - Unable to save settings"),
                                         TRANS("An error occurred when trying to save the application's settings file...\n\nIn order to save and restore its settings, ")
                                          + filePrefix + TRANS(" needs to be able to write to the following file:\n")
                                          + filename
                                          + TRANS("\n\nMake sure that this file isn't read-only, and that the disk isn't full.")
                                          + TRANS("\n\nDefaults will be used, but changes will not be saved."));
        }
        return false;
    }
    return true;
}

//==============================================================================
void EapProperties::openFiles() throw()
{
    // You need to call setStorageParameters() before trying to get hold of the
    // properties!
    jassert (filePrefix.isNotEmpty());

    if (filePrefix.isNotEmpty())
    {
        if (eapProps == 0)
		{
			eapProps = EapPropertiesFile::createDefaultAppPropertiesFile (filePrefix, fileSuffix, folderName,
                                                                        false, msBeforeSaving, options);
		}
    }
}

EapPropertiesFile* EapProperties::getEapSettings() throw()
{
    if (eapProps == 0)
        openFiles();

    return eapProps;
}

void EapProperties::closePropertiesFile()
{
    deleteAndZero (eapProps);
}

bool EapProperties::saveIfNeeded()
{
    return (eapProps == 0 || eapProps->saveIfNeeded());
}

void EapProperties::closeFiles()
{
    deleteAndZero (eapProps);
}

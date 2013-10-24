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

#include "DefaultsFile.h"

static const char* const propertyFileXmlTag    = "PROPERTIES";
static const char* const propertyTagName       = "VALUE";

//==============================================================================
DefaultsFile::DefaultsFile (const File& f) throw()
    : PropertySet (true),
      file (f)
{
    // Make sure it's an XML file..
    XmlDocument parser (f);
    XmlElement* doc = parser.getDocumentElement (true);

    if (doc != 0 && doc->hasTagName (propertyFileXmlTag))
    {
        delete doc;
        doc = parser.getDocumentElement();

        if (doc != 0)
        {
            forEachXmlChildElementWithTagName (*doc, e, propertyTagName)
            {
                const String name (e->getStringAttribute ("name"));

                if (name.isNotEmpty())
				{
                    getAllProperties().set (name, e->getStringAttribute ("val"));

					String valStr = e->getStringAttribute ("val");
				}
            }
        }
        else
        {
            // must be a pretty broken XML file we're trying to parse here!
            jassertfalse
        }

        delete doc;
    }
}

DefaultsFile::~DefaultsFile()
{
}

const File DefaultsFile::getFile() const throw()
{
    return file;
}

//==============================================================================
const File DefaultsFile::getDefaultAppSettingsFile (const String& fileNamePrefix,
                                                      const String& fileNameSuffix)
{
    // mustn't have illegal characters in this name..
	jassert ((fileNamePrefix + "." + fileNameSuffix) == File::createLegalFileName (fileNamePrefix + "." + fileNameSuffix));

#if JUCE_MAC
	File dir (File::getSpecialLocation (File::currentApplicationFile));

    dir = dir.getChildFile ("Contents/Resources");
#endif

#if JUCE_WIN32
	File dir (File::getSpecialLocation (File::currentApplicationFile));

    if (dir == File::nonexistent)
        return File::nonexistent;

	dir = dir.getParentDirectory();

#endif

    return dir.getChildFile (fileNamePrefix)
              .withFileExtension (fileNameSuffix);
}

DefaultsFile* DefaultsFile::createDefaultAppSettingsFile (const String& fileNamePrefix,
                                                                const String& fileNameSuffix)
{
    const File file (getDefaultAppSettingsFile (fileNamePrefix,
                                                fileNameSuffix));

    jassert (file != File::nonexistent);

    if (file == File::nonexistent)
        return 0;

	return new DefaultsFile (file);
}

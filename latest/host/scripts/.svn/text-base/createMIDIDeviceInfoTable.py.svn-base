#!/usr/bin/env python
##==============================================================================
##
## This confidential and proprietary software may be used only as authorised
## by a licensing agreement from TC Applied Technologies Ltd.
##
## (C) COPYRIGHT 2008 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
##
## Unauthorized use, duplication or distribution of this software is  
## strictly prohibited by law.
##
## The entire notice above must be reproduced on all authorised copies and
## copies may only be made to the extent permitted by a licensing agreement
## from TC Applied Technologies Ltd.
##
##==============================================================================

import sys
if sys.hexversion < 0x020400F0:
	print "error: this script requires Python 2.4 or higher"
	sys.exit( 1 )
import os, os.path, re, time, optparse, subprocess, random, string


# the maximum number of device entries supported in tcat_dice_myproduct.h
kMaxDeviceDefineEntries = 16



def getFlattenedText( inFilePath ):
	"""Retrieves the text of the file, expanding any #include "file" directives in place."""

	flatText = ''

	# open the file and read in its text
	fileObj = file( inFilePath, 'r' )
	fileLines = fileObj.readlines()
	fileObj.close()

	if options.debug:
		print '\n\n********** getFlattenedText( "%s" ) tokens **********' % (inFilePath)

	for line in fileLines:
		# strip any trailing comments
		line = line.partition( '//' )[0]
		line = line.partition( '/*' )[0]
		# strip any leading or trailing whitespace
		line = line.strip()
		# skip empty lines and comment lines
		if len(line) == 0 or line.startswith( '//' ):
			continue

		if line.startswith( '#' ):
			tokens = line.split()
			if options.debug:
				print tokens
			if tokens[0] == '#':
				keyword = tokens[0] + tokens[1]
				dataOffset = 2
			else:
				keyword = tokens[0]
				dataOffset = 1

			if keyword == '#include':
				include = tokens[dataOffset].strip('"')
				dir = os.path.dirname( inFilePath )
				includeFilePath = os.path.join( dir, include )
				try:
					flatText = flatText + getFlattenedText( includeFilePath )
				except:
					print 'warning: unable to open the include file "%s"' % (includeFilePath)
				else:
					flatText = flatText + '\n'
			else:
				flatText = flatText + line + '\n'

	return flatText



def gatherDefines( inText ):
	"""Walks through the provided text, extracting any #define statements.
	It filters out any #define that defines a macro (ex: "#define FOO(x,y) x=y")
	and filters out header guard defines (start and end with '_').  It returns a
	list of the defines found."""

	definesList = []

	if options.debug:
		print '\n\n********** gatherDefines() tokens **********'

	lines = inText.split( '\n' )
	for line in lines:
		if line.startswith( '#' ):
			tokens = line.split()
			if options.debug:
				print tokens
			if tokens[0] == '#':
				keyword = tokens[0] + tokens[1]
				dataOffset = 2
			else:
				keyword = tokens[0]
				dataOffset = 1

			if keyword == '#define':
				define = tokens[dataOffset]
				if define.find('(') != -1:
					# skip macros
					pass
				elif define.startswith('_') and define.endswith('_'):
					# skip header guards
					pass
				else:
					if define not in definesList:
						definesList.append( define )

	return definesList



def getDefineValues( inHeaderFilePath ):
	"""Uses the platform compiler's preprocessor to get the values of all defines found
	in the specified header file and any of its #include files.  It returns a dictionary
	with the defines and their values."""

	definesDict = {}
	randomChars = ''.join(random.choice(string.ascii_letters + string.digits) for x in range(8))
	tempSrcFilePath = "temp_%s_defines.c" % (randomChars)
	tempDestFilePath = "temp_%s_defines.pp" % (randomChars)

	flatText = getFlattenedText( inHeaderFilePath )
	definesList = gatherDefines( flatText )

	fileText = ''
	for define in definesList:
		fileText = fileText + "%s //$$%s\n" % (define, define)

	fileObj = file( tempSrcFilePath, 'w' )
	fileObj.write( fileText )
	fileObj.close()

	# preprocess the file
	if (sys.platform == "win32"):
		preprocessCmd = 'cl -nologo -C -FI "%s" -EP "%s" >"%s"' % (inHeaderFilePath, tempSrcFilePath, tempDestFilePath)
	elif (sys.platform == "darwin"):
		preprocessCmd = '/usr/bin/xcrun gcc -include "%s" -C -E -x c -P "%s" >"%s"' % (inHeaderFilePath, tempSrcFilePath, tempDestFilePath)
	else:
		print 'error: unsupported platform "%s" for preprocessing' % (sys.platform)
		return definesDict

	try:
		p = subprocess.Popen( preprocessCmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True )
		stdOutText, stdErrText = p.communicate()
		returnCode = p.returncode

		# workaround to ignore gcc errors about concatenating with "." which can be ignored
		if (sys.platform == "darwin") and stdErrText:
			foundError = False
			for line in stdErrText.splitlines():
				if line.find( 'and "." does not give a valid preprocessing token' ) != -1:
					# ignore these errors
					pass
				else:
					foundError = True
					break

			if not foundError:
				returnCode = 0

		if returnCode != 0:
			print "---------- stdout ----------"
			print stdOutText
			print "---------- stderr ----------"
			print stdErrText
			print "----------------------------"
	except:
		print 'error: the preprocessor command threw an exception'
		returnCode = -1

	if returnCode != 0:
		print 'error: preprocessor command line returned an error (' + str(returnCode) + ')'
		sys.exit( 1 )

	# make sure the file exists
	if os.path.isfile( tempDestFilePath ):
		# open the file and read in its text
		fileObj = file( tempDestFilePath, 'r' )
		fileLines = fileObj.readlines()
		fileObj.close()

		if options.debug and not options.verbose:
			print '\n\n********** getDefineValues() tokens **********'

		for line in fileLines:
			tokens = line.partition( '//' )
			define = tokens[2].strip()
			value = tokens[0].strip()
			if not define.startswith( '$$' ):
				continue
			if options.debug and not options.verbose:
				print tokens
			define = define.lstrip( '$' )
			if options.verbose:
				print '%s = %s' % (define, value)
			if define == value:
				# the define was not replaced by the preprocessor
				continue
			definesDict[define] = value
			
		os.remove( tempDestFilePath )

	os.remove( tempSrcFilePath )

	return definesDict



def getEntryTextForDevice( inDeviceIndex, inDefinesDict ):
	"""Creates the MIDI info struct for a device."""

	entryText = ''
	lines = []

	defineBase = 'kTCAT_DICE_DEVICE_%d_MIDI_' % (inDeviceIndex)
	midiNameDefine = defineBase + 'NAME'
	if midiNameDefine in inDefinesDict:
		print "found define:  %s" % (midiNameDefine)
		guid64ProductIdMask = inDefinesDict[ 'kTCAT_DICE_GUID64_PRODUCT_ID_MASK' ]

		lines.append( '\t{' )
		if guid64ProductIdMask == '0':
			lines.append( '\t\t0,' )
		else:
			lines.append( '\t\t%s,' % (inDefinesDict[ 'kTCAT_DICE_DEVICE_%d_ID' % (inDeviceIndex) ]) )
		lines.append( '\t\t"%s",' % (inDefinesDict[ midiNameDefine ]) )
		if (sys.platform == "darwin"):
			midiManufacturerDefine = defineBase + 'MANUFACTURER'
			if midiManufacturerDefine in inDefinesDict:
				print "found define:  %s" % (midiManufacturerDefine)
				lines.append( '\t\t"%s",' % (inDefinesDict[ midiManufacturerDefine ]) )
			else:
				lines.append( '\t\tNULL,' )

			midiDeviceNameDefine = defineBase + 'DEVICE_NAME'
			if midiDeviceNameDefine in inDefinesDict:
				print "found define:  %s" % (midiDeviceNameDefine)
				lines.append( '\t\t"%s",' % (inDefinesDict[ midiDeviceNameDefine ]) )
			else:
				lines.append( '\t\tNULL,' )

			iconFileDefine = defineBase + 'ICON_FILE'
			if iconFileDefine in inDefinesDict:
				print "found define:  %s" % (iconFileDefine)
				lines.append( '\t\t"%s",' % (inDefinesDict[ iconFileDefine ]) )
			else:
				lines.append( '\t\tNULL,' )

		lines.append( '\t\t{' )
		for portIndex in range(1,kMaxDeviceDefineEntries+1):
			portDefineBase = 'kTCAT_DICE_DEVICE_%d_MIDI_PORT_%d_' % (inDeviceIndex, portIndex)
			lineText = '\t\t\t{ '

			portNameDefine = portDefineBase + 'NAME'
			if portNameDefine in inDefinesDict:
				print "  found port define:  %s" % (portNameDefine)
				lineText = lineText + '"%s"' % (inDefinesDict[ portNameDefine ])
			else:
				lineText = lineText + 'NULL'

			speedDefine = portDefineBase + 'SPEED'
			if speedDefine in inDefinesDict:
				print "  found port define:  %s" % (speedDefine)
				lineText = lineText + ', %s' % (inDefinesDict[ speedDefine ])
			else:
				lineText = lineText + ', 0'

			if (sys.platform == "darwin"):
					embeddedDefine = portDefineBase + 'EMBEDDED'
					if embeddedDefine in inDefinesDict:
						if (inDefinesDict[ embeddedDefine ] != '0'):
							print "  found port define:  %s" % (embeddedDefine)
							lineText = lineText + ', true'
						else:
							lineText = lineText + ', false'
					else:
						lineText = lineText + ', false'

			lineText = lineText + ' },'
			lines.append( lineText )

		lines.append( '\t\t},' )

		lines.append( '\t},' )

	if len(lines) > 0:
		entryText = '\n'.join(lines) + '\n'

	return entryText



gFileHeader = """///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT %s TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////
//
//         ****** DO NOT EDIT THIS FILE ******
//
// This file is auto-generated by a script.
//
// Generated at:  %s
//
///////////////////////////////////////////////////////////////////////////////

"""


def main():
	usage = "usage: \%prog [options] defaultsHeaderFile destinationFile"
	parser = optparse.OptionParser( usage )
	parser.add_option('-v', '--verbose', action="store_true", dest="verbose", default=False, help ="verbose output")
	parser.add_option('-d', '--debug', action="store_true", dest="debug", default=False, help ="verbose debug output")
	global options
	options, args = parser.parse_args()

	if len( args ) != 2:
		parser.print_help()
		sys.exit( 1 )

	hdrFilePath = args[0]
	dstFilePath = args[1]

	# make sure the files exists
	if not os.path.isfile( hdrFilePath ):
		print 'error: the header file "%s" does not exist' % hdrFilePath
		sys.exit( 1 )
	destDir = os.path.dirname( dstFilePath )
	if destDir and not os.path.isdir( destDir ):
		print 'error: the destination directory "%s" does not exist' % destDir
		sys.exit( 1 )

	print 'generating header file "%s"' % (dstFilePath)

	definesDict = getDefineValues( hdrFilePath )

	# start with a header
	timeStamp = time.localtime( time.time() )
	copyrightText = time.strftime( '%Y', timeStamp )
	dateTimeText = time.strftime( '%b %d %H:%M:%S %Y', timeStamp )
	fileText = gFileHeader % (copyrightText, dateTimeText)

	# add entries for each device that has MIDI attributes declared
	entriesExist = False
	for deviceIndex in range(1,20):
		entryText = getEntryTextForDevice( deviceIndex, definesDict )
		if entryText:
			entriesExist = True
		fileText = fileText + entryText

	if not entriesExist:
		# no entries exist; there must be at least one so add a default
		definesDict['kTCAT_DICE_DEVICE_999_ID'] = '0xD1CED1CE'
		definesDict['kTCAT_DICE_DEVICE_999_MIDI_NAME'] = 'No Name'
		fileText = fileText + getEntryTextForDevice( 999, definesDict )

	# write out the file
	fileObj = file( dstFilePath, 'w' )
	fileObj.write( fileText )
	fileObj.close()



if __name__ == '__main__':
	main()

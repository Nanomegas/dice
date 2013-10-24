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



def replaceKeyWords( inFilePath, inDefinesDict, inOutFilePath ):
	"""Replaces all defines in the specified input file and writes out the
	changes to the specified output file."""

	fileObj = file( inFilePath, 'r' )
	fileLines = fileObj.readlines()
	fileObj.close()

	def byLength( inWordOne, inWordTwo ):
		"""This sorts the defines by length, the longest defines come first.
		Defines of the same length are sorted alphabetically."""
		lengthOne = len( inWordOne )
		lengthTwo = len( inWordTwo )
		if lengthOne == lengthTwo:
			# sort alphabetically
			return cmp( inWordOne.lower(), inWordTwo.lower() )
		else:
			return lengthTwo - lengthOne

	# build the regular expression used for keyword matching
	searchReList = map( re.escape, inDefinesDict )
	searchReList.sort( cmp = byLength )
	searchReStr = '|'.join( searchReList )
	if options.debug:
		print '\n\n********** replaceKeyWords() reg exp **********'
		print searchReStr
	rx = re.compile( searchReStr )

	def keywordRepl( inMatchObj ):
		return inDefinesDict[ inMatchObj.group(0) ]

	newFileText = ''
	skipTextStack = []
	skipText = False

	if options.debug:
		print '\n\n********** getDefineValues() tokens **********'

	for line in fileLines:
		line = line.rstrip()
		if line:
			tokens = line.strip().split()
			if options.debug:
				print tokens
			keyword = tokens[0]
			if keyword == '#ifdef':
				value = tokens[1]
				skipTextStack.append( skipText )
				if not skipText:
					skipText = value not in inDefinesDict
				if options.debug:
					print 'IFDEF %s: %s  [%s]' % (value, line, skipText)
				continue
			elif keyword == '#else':
				skipText = not skipText
				if options.debug:
					print 'ELSE: %s  [%s]' % (line, skipText)
			elif keyword == '#if':
				print 'error: "#if" is an unsupported keyword'
				sys.exit( 1 )
			elif keyword == '#elif':
				print 'error: "#elif" is an unsupported keyword'
				sys.exit( 1 )
			elif keyword == '#endif':
				skipText = skipTextStack.pop()
				if options.debug:
					print 'ENDIF: %s  [%s]' % (line, skipText)
				continue

			if not skipText:
				newFileText = newFileText + rx.sub( keywordRepl, line )

		if not skipText:
			newFileText = newFileText + '\n'

	fileObj = file( inOutFilePath, 'w' )
	fileObj.write( newFileText )
	fileObj.close()



def getMidiTextForINF( inDeviceIndex, inDeviceId, inDefinesDict ):
	"""Creates the appropriate lines of text that should be placed in the INF for MIDI support."""

	addInterfaceLines = []
	addRegLines = []
	vendorShortName = inDefinesDict[ 'kTCAT_DICE_VENDOR_SHORT' ]

	for portIndex in range(1,kMaxDeviceDefineEntries+1):
		midiInDefine = 'kTCAT_DICE_DEVICE_%d_MIDI_IN_%d' % (inDeviceIndex, portIndex)
		if midiInDefine in inDefinesDict:
			installStr = 'MidiInstall.MIDIC%d%d' % (inDeviceIndex, portIndex)
			interfaceStr = 'D%d_IN%d_%s' % (inDeviceId, portIndex, vendorShortName)

			addInterfaceLines.append( 'AddInterface=%%KSCATEGORY_AUDIO%%,%s, %s' % (interfaceStr, installStr) )
			addInterfaceLines.append( 'AddInterface=%%KSCATEGORY_CAPTURE%%,%s, %s' % (interfaceStr, installStr) )

			addRegLines.append( '[%s]' % (installStr) )
			addRegLines.append( 'AddReg=%s.AddReg' % (installStr) )
			addRegLines.append( '[%s.AddReg]' % (installStr) )
			addRegLines.append( 'HKR,,FriendlyName,,"%s"' % inDefinesDict[ midiInDefine ] )
			addRegLines.append( 'HKR,,CLSID,,%Proxy.CLSID%' )
			addRegLines.append( '' )

		midiOutDefine = 'kTCAT_DICE_DEVICE_%d_MIDI_OUT_%d' % (inDeviceIndex, portIndex)
		if midiOutDefine in inDefinesDict:
			installStr = 'MidiInstall.MIDIR%d%d' % (inDeviceIndex, portIndex)
			interfaceStr = 'D%d_OUT%d_%s' % (inDeviceId, portIndex, vendorShortName)

			addInterfaceLines.append( 'AddInterface=%%KSCATEGORY_AUDIO%%,%s, %s' % (interfaceStr, installStr) )
			addInterfaceLines.append( 'AddInterface=%%KSCATEGORY_RENDER%%,%s, %s' % (interfaceStr, installStr) )

			addRegLines.append( '[%s]' % (installStr) )
			addRegLines.append( 'AddReg=%s.AddReg' % (installStr) )
			addRegLines.append( '[%s.AddReg]' % (installStr) )
			addRegLines.append( 'HKR,,FriendlyName,,"%s"' % inDefinesDict[ midiOutDefine ] )
			addRegLines.append( 'HKR,,CLSID,,%Proxy.CLSID%' )
			addRegLines.append( '' )

	return ( '\n'.join(addInterfaceLines), '\n'.join(addRegLines) )



def main():
	usage = "usage: \%prog [options] sourceFile headerFile destinationFile"
	parser = optparse.OptionParser( usage )
	parser.add_option('-b', '--buildtype', action="store", dest="buildType", default="Release", help ="specifies the build type")
	parser.add_option('-v', '--verbose', action="store_true", dest="verbose", default=False, help ="verbose output")
	parser.add_option('-d', '--debug', action="store_true", dest="debug", default=False, help ="verbose debug output")
	global options
	options, args = parser.parse_args()

	if (options.buildType != "Release") and (options.buildType != "Debug"):
		print 'error: the buildType argument must be either "Release" or "Debug"'
		sys.exit( 1 )

	if len( args ) != 3:
		parser.print_help()
		sys.exit( 1 )

	srcFilePath = args[0]
	hdrFilePath = args[1]
	dstFilePath = args[2]

	# make sure the files exists
	if not os.path.isfile( srcFilePath ):
		print 'error: the source file "%s" does not exist' % srcFilePath
		sys.exit( 1 )
	if not os.path.isfile( hdrFilePath ):
		print 'error: the header file "%s" does not exist' % hdrFilePath
		sys.exit( 1 )
	destDir = os.path.dirname( dstFilePath )
	if destDir and not os.path.isdir( destDir ):
		print 'error: the destination directory "%s" does not exist' % destDir
		sys.exit( 1 )

	print 'preprocessing "%s" --> "%s"' % (srcFilePath, dstFilePath)

	definesDict = getDefineValues( hdrFilePath )

	# add custom defines
	timeStamp = time.localtime( time.time() )
	definesDict['[TIMESTAMP]'] = time.strftime('%b %d %H:%M:%S %Y', timeStamp )
	definesDict['[DRIVERVER_TIMESTAMP]'] = time.strftime('%m/%d/%Y', timeStamp )
	definesDict['[BUILDTYPE]'] = options.buildType

	# add custom defines for MIDI in the INF file
	if srcFilePath.endswith( '.inf' ):
		for devIndex in range(1,9):
			devIdDefine = 'kTCAT_DICE_DEVICE_%d_ID' % (devIndex)
			if devIdDefine in definesDict:
				devIdInt = int( definesDict[devIdDefine], 16 )
				addInterface, addReg = getMidiTextForINF( devIndex, devIdInt, definesDict )
				definesDict['[DEVICE_%d_MIDI_ADDINTERFACE]' % (devIndex)] = addInterface
				definesDict['[DEVICE_%d_MIDI_ADDREG]' % (devIndex)] = addReg

	replaceKeyWords( srcFilePath, definesDict, dstFilePath )



if __name__ == '__main__':
	main()

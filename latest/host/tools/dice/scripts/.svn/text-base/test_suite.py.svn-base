#!/usr/bin/env python
##==============================================================================
##
## This confidential and proprietary software may be used only as authorised
## by a licensing agreement from TC Applied Technologies Ltd.
##
## (C) COPYRIGHT 2008-2011 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
##
## Unauthorized use, duplication or distribution of this software is  
## strictly prohibited by law.
##
## The entire notice above must be reproduced on all authorised copies and
## copies may only be made to the extent permitted by a licensing agreement
## from TC Applied Technologies Ltd.
##
##==============================================================================

import sys, os, os.path, string, re, time, socket
if sys.hexversion < 0x020400F0:
	print "error: this script requires Python 2.4 or higher"
	sys.exit( 1 )
import optparse, subprocess, csv


kVersionStr = "1.1.1"
kDeviceResetDelaySecs = 10

kTDLNumStdLogEvents = 8192
kTDLNumRltLogEvents = 131072
kTDLFlags = 0xffffffff			# enable all flags
kTDLStopFlags = 0x00000040		# stop if any messages have the kTDLFlag_Error flag set


def diceTool( argumentText="", captureOutput=False, printError=True, dice64bit=False ):
	"""
	This runs the dice tool application.
	You can specify the arguments to pass to it.
	It can optionally capture the output (both stdout and stderr combined).
	You can choose to run the 64-bit version of the dice tool.

	It returns a tuple (errorCode, stdOutText)
	"""

	stdOutText = None

	diceCmd = "dice"
#	if dice64bit:
#		diceCmd = diceCmd + "64"

	if (sys.platform != "win32"):
		diceCmd = "./" + diceCmd
		argumentText = argumentText.replace( '#', '\\#' )

	if (sys.platform == "darwin") and not options.dice64bit:
		diceCmd = "arch -arch i386 " + diceCmd

	if captureOutput:
		stdOutArg = subprocess.PIPE
		stdErrArg = subprocess.STDOUT
	else:
		stdOutArg = None
		stdErrArg = None

	try:
		p = subprocess.Popen( "%s %s" % (diceCmd, argumentText), stdout=stdOutArg, stderr=stdErrArg, shell=True )
		if captureOutput:
			stdOutText, stderrText = p.communicate()
			returnCode = p.returncode
		else:
			returnCode = p.wait()
	except KeyboardInterrupt:
		raise
	except:
		print 'ERROR: the dicetool command threw an exception'
		returnCode = -1
	else:
		if options.debug and captureOutput:
			print "---------- stdout ----------"
			print stdOutText
			print "----------------------------"
		if options.debug:
			print "dice tool returned " + str(returnCode)

		if printError and (returnCode != 0):
			print "ERROR: dice tool status %d returned by command \"%s\"" % (returnCode, argumentText)

	return returnCode, stdOutText




def tdlCmdLine( argumentText="", captureOutput=False, printError=True ):
	"""
	This runs the TDL command line application.
	You can specify the arguments to pass to it.
	It can optionally capture the output (both stdout and stderr combined).
	You can choose to run the 64-bit version.

	It returns a tuple (errorCode, stdOutText)
	"""

	stdOutText = None

	tdlCmd = "TDL_CmdLine"

	if (sys.platform != "win32"):
		tdlCmd = "./" + tdlCmd

	if captureOutput:
		stdOutArg = subprocess.PIPE
		stdErrArg = subprocess.STDOUT
	else:
		stdOutArg = None
		stdErrArg = None

	try:
		p = subprocess.Popen( "%s %s" % (tdlCmd, argumentText), stdout=stdOutArg, stderr=stdErrArg, shell=True )
		if captureOutput:
			stdOutText, stderrText = p.communicate()
			returnCode = p.returncode
		else:
			returnCode = p.wait()
	except KeyboardInterrupt:
		raise
	except:
		print 'ERROR: the TDL_CmdLine command threw an exception'
		returnCode = -1
	else:
		if options.debug and captureOutput:
			print "---------- stdout ----------"
			print stdOutText
			print "----------------------------"
		if options.debug:
			print "TDL_CmdLine returned " + str(returnCode)

		if printError and (returnCode != 0):
			print "ERROR: TDL_CmdLine status %d returned by command \"%s\"" % (returnCode, argumentText)

	return returnCode, stdOutText




def tdlDriverIsInstalled():
	if (sys.platform == "win32"):
		tdlCmdPath = "TDL_CmdLine.exe"
	else:
		tdlCmdPath = "TDL_CmdLine"

	if os.path.isfile( tdlCmdPath ):
		# verify that TDL has been installed
		returnCode, stdOutText = tdlCmdLine( "isInstalled", captureOutput=True, printError=False )
		if returnCode == 0:
			return True
		else:
			print "The TDL driver is not installed\n"

	return False




def getHostNameStr():
	hostNameStr = socket.gethostname()
	if (sys.platform == "darwin") and hostNameStr.endswith( ".local" ):
		return hostNameStr[:-6]
	return hostNameStr




def getOSVersionStr():
	versionStr = "Unsupported"

	if (sys.platform == "darwin"):
		stdOutText, stdErrText = subprocess.Popen( ["/usr/bin/sw_vers", "-productVersion"], stdout=subprocess.PIPE ).communicate()
		versionStr = stdOutText.strip()
		if not versionStr:
			print "ERROR: unable to retrieve Mac OS X version"
		if options.debug:
			print "Mac OS X version:  %s" % (versionStr)
	elif (sys.platform == "win32"):
		osName = "Unknown"
		versionTuple = sys.getwindowsversion()

		osVersion = "%d.%d.%d" % (versionTuple[0], versionTuple[1], versionTuple[2])

		if (versionTuple[0] == 5):
			osName = "XP"
		elif (versionTuple[0] == 6) and (versionTuple[1] == 0):
			osName = "Vista"
		elif (versionTuple[0] == 6) and (versionTuple[1] == 1):
			osName = "Win7"

		osServicePack = versionTuple[4]
		osServicePack = osServicePack.replace( "Service Pack ", "SP" )

		osBitSize = 32
		stdOutText, stderrText = subprocess.Popen( ["systeminfo"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT ).communicate()
		outputLines = stdOutText.split( '\n' )
		for line in outputLines:
			if line == None:
				continue
			line = line.strip()
			try:
				label, value = line.split( ':', 1 )
				label = label.strip()
				value = value.strip()
			except:
				label = ""
				value = line
			if osName == "XP":
				if label.startswith( "Processor" ) and value.find( "AMD64" ) >= 0:
					osBitSize = 64	
			else:
				if label.startswith( "System Type" ) and value.startswith( "x64" ):
					osBitSize = 64
		if osServicePack:
			versionStr = "%s-%s-%d" % (osName, osServicePack, osBitSize)
		else:
			versionStr = "%s-%d" % (osName, osBitSize)

		if options.debug:
			print "Windows version:  %s" % (versionStr)

	else:
		print "Unsupported operating system: \"" + sys.platform + "\""
		sys.exit( 1 )

	return versionStr




def putMachineToSleep():
	if (sys.platform == "darwin"):
		appleScript = "tell application \"System Events\"\n\tsleep\nend"
		p = subprocess.Popen( "/usr/bin/osascript", stdin=subprocess.PIPE, shell=True )
		stdOutText, stderrText = p.communicate( appleScript )
		returnCode = p.wait()
	elif (sys.platform == "win32"):
		p = subprocess.Popen( "rundll32 powrprof.dll,SetSuspendState" )
		returnCode = p.wait()
	else:
		print "Support for sleep is not implemented for this operating system: \"" + sys.platform + "\""




def getTimeDurationStr( duration, verbose=False ):
	durationStr = ""

#	days  = (duration / (24 * 60 * 60))
	hours = (duration / (60 * 60))
	mins =  (duration / 60) % 60
	secs =  (duration % 60)

	durationStr = "%02d:%02d:%02d" % (hours, mins, secs)

	return durationStr




def yesNoString( value ):
	if (value == 0):
		return "no"
	else:
		return "yes"




class diceLogTestLabels:
	def __init__( self, labelsList ):
		self.labelsList = labelsList
		self.labelsDict = {}
		for n, label in enumerate( labelsList ):
			self.labelsDict[ label ] = n


	def index( self, columnName ):
		"""
		provides the ability to look up a column's index by name,
		so as not to be dependent on the dice tool's column ordering
		or addition of new columns to its output
		"""
		return self.labelsDict[ columnName ]


	def getColumnName( self, index ):
		try:
			return self.labelsList[ index ]
		except:
			return "N/A"




class diceLogTest:
	def __init__( self, labelsObj, rowList ):
		self.labelsObj = labelsObj
		self.rowsList = []
		self.rowsList.append( rowList )


	def addcsvRow( self, rowList ):
		self.rowsList.append( rowList )


	def getNumRows( self ):
		return len( self.rowsList )


	def __getitem__( self, index ):
		return self.rowsList[ index ]


	def getMaxCommonColumnIndex( self ):
		return (self.labelsObj.index( 'out' ) - 1)


	def getOpMode( self ):
		columnIndex = self.labelsObj.index( 'op mode' )
		columnValueStr = self.rowsList[0][columnIndex]
		return int( columnValueStr )


	def adjustLatencyForInternalLoopback( self ):
		# using the internal routing for the audio loopback is 5 samples faster on DiceJr/Mini
		for rowIndex, rowList in enumerate( self.rowsList ):
			for columnIndex, columnText in enumerate( rowList ):
				columnLabel = self.labelsObj.getColumnName( columnIndex )
				columnValueStr = self.rowsList[rowIndex][columnIndex]

				if columnLabel == 'diff':
					try:
						columnValue = int( columnValueStr )
						self.rowsList[rowIndex][columnIndex] = columnValue - 5
					except:
						pass


	def analyzeColumn( self, rowIndex, columnIndex ):
		if (rowIndex > 0) and (columnIndex <= self.getMaxCommonColumnIndex()):
			return None

		columnLabel = self.labelsObj.getColumnName( columnIndex )
		columnValueStr = self.rowsList[rowIndex][columnIndex]

		errorIfNotZero = [ 'qi', ]

		errorIfNotBlank = [ 'CA overloads', 'drops', 'first drop', 'min drop', 'max drop', 'slips', 'first slip',
						  'min slip size', 'max slip size', 'slips+drops', 'streaming restarts',
						  'RX unlocks', 'RX slips', 'WatchdogTimer', 'MissedCallbacks', 'DbcFail',
						  'SytFail', 'UsermodeDelays', 'ATX_BOUNDARY', 'ADO_SYT_AGEOUT',
						  'ARX_DBC_FAIL', 'ARX_PKT_ABORT', 'ARXDO_SYT_OF' ]

		errorIfBlank = [ 'init gap', 'min latency', 'max latency', 'last latency', 'exp latency' ]

		# FIXME FIXME -- disabled 'diff' warning check for now, customized check below
		#warningIfNotZero = [ 'diff' ]
		warningIfNotZero = []

		result = None

		if columnLabel in errorIfNotZero:
			if (columnValueStr != '0'):
				result = "error"
		elif columnLabel in errorIfNotBlank:
			if (columnValueStr != ''):
				result = "error"
		elif columnLabel in errorIfBlank:
			if (columnValueStr == ''):
				result = "error"
		elif columnLabel in warningIfNotZero:
			if (columnValueStr != '0'):
				result = "warning"
		elif (columnLabel == 'MA'):
			columnValue = int( columnValueStr )
			if (columnValue != 0):
				if (columnValue < 16):
					result = "warning"
				else:
					result = "error"
		elif (columnLabel == 'phase%'):
			phasePercentageStrs = columnValueStr.split( "," )
			for percentStr in phasePercentageStrs:
				percentInt = int( percentStr )
				if (percentInt < -15) or (percentInt > 15):
					result = "error"
					break
				elif (percentInt < -8) or (percentInt > 8):
					result = "warning"
		elif (columnLabel == 'drv CB drops'):
			if (columnValueStr != ''):
				columnValue = int( columnValueStr )
				if (columnValue == 1) and (sys.platform == "darwin"):
					result = "warning"
				else:
					result = "error"
		elif (columnLabel == 'CPU:drv min us(%)') or (columnLabel == 'CPU:dsp min us(%)'):
			if (columnValueStr == '') or columnValueStr.startswith( "4294967295" ):
				result = "error"
		elif (columnLabel == 'max us(%)'):
			if (columnValueStr == '') or (columnValueStr == "0(0)"):
				result = "error"
		elif columnLabel == 'delayed CB us':
			if (columnValueStr == '') or (int( columnValueStr ) > 20000):
				result = "error"
		elif columnLabel == 'diff':
			if (columnValueStr == ''):
				result = "error"
			else:
				columnValue = int( columnValueStr )
				if (columnValue >= 8):
					result = "error"
				elif (columnValue >= 2):
					result = "warning"
		elif columnLabel.startswith("MIDI") and columnLabel.endswith("drops"):
			if (columnValueStr != ''):
				result = "error"
		elif (columnValueStr == "not streaming"):
			result = "error"
		else:
#			print "WARNING: unable to determine if column \"columnLabel\" value is within acceptable range"
			pass

		return result




class diceLogFile:
	def __init__( self, csvLogFilePath ):
		self.csvLogFilePath = csvLogFilePath
		self.configurationLines = []
		self.labelsObj = None
		self.testObjs = []

		try:
			readerObj = csv.reader( file( self.csvLogFilePath ) )
		except IOError:
			print "ERROR: unable to read the file \"%s\"" % (csvLogFilePath)
			raise
		except:
			raise

		testObj = None

		for rowList in readerObj:
			if not self.labelsObj:
				if (rowList[0] == "op mode" or rowList[0] == "outs"):
					self.labelsObj = diceLogTestLabels( rowList )
				else:
					self.configurationLines.append( rowList[0].rstrip() )
			else:
				# FIXME FIXME -- workaround since the dice's CSV lines end with an extra comma, creating an additional blank field
				rowList = rowList[:-1]

				if rowList[0] != "":
					testObj = diceLogTest( self.labelsObj, rowList )
					self.testObjs.append( testObj )
				else:
					testObj.addcsvRow( rowList )

		if not self.labelsObj:
			print "ERROR:  unable to find the row of column name labels in the csv log file"

	def __getitem__( self, index ):
		return self.testObjs[ index ]


	def getNumTestResults( self ):
		return len( self.testObjs )


	def getLabelsList( self ):
		return self.labelsObj.labelsList




class TestCaseBase:
	def __init__( self ):
		self.dpcLatencyCheckerInstalled = False
		self.logDirPath = None
		self.TDLisInstalled = None

	def getTestName( self ):
		return None


	def getHelpText( self ):
		return "N/A"


	def resetDeviceAfterTest( self ):
		return False


	def usingInternalLoopback( self ):
		return True


	def timeTheTest( self ):
		return True


	def processTestArgs( self, testArgs ):
		pass


	def doTest( self ):
		pass


	def getNumofDevicesAttached( self ):
		returnCode, outputText = diceTool( "-am bi", captureOutput=True, printError=False )
		outputLines = outputText.split( '\n' )
		for line in outputLines:
			matchObj = re.match( r'^.+number of devices:\s+(\d+)', line )
			if (matchObj != None):
				return int( matchObj.group( 1 ) )
		return 0


	def getDriverVersionStr( self ):
		returnCode, outputText = diceTool( "-am bi", captureOutput=True )
		outputLines = outputText.split( '\n' )
		for line in outputLines:
			matchObj = re.match( r'^.+drv version:\s+([0-9.]+)', line )
			if (matchObj != None):
				return matchObj.group( 1 )
		return ""


	def getCurrentOperationModeStr( self ):
		returnCode, outputText = diceTool( "-am bo", captureOutput=True )
		outputLines = outputText.split( '\n' )
		for line in outputLines:
			matchObj = re.match( r'^op_mode:\s+(\d+)', line )
			if (matchObj != None):
				return matchObj.group( 1 )
		return ""


	def getSupportedSampleRates( self ):
		sampleRateStrs = []
		returnCode, outputText = diceTool( "-am di", captureOutput=True, printError=False )
		outputLines = outputText.split( '\n' )
		for line in outputLines:
			line = line.strip()
			matchObj = re.match( r'^clock caps bitmap:.+\((.+)\)', line )
			if (matchObj != None):
				clockCapsStr = matchObj.group( 1 )
				clockCapsStr = clockCapsStr.strip()
				clockCapsStr = clockCapsStr.replace( "kHz", "" )
				clockCapsStr = clockCapsStr.replace( " ", "" )
				if clockCapsStr.endswith( "," ):
					clockCapsStr = clockCapsStr[:-1]
				if options.debug:
					print 'clockCapsStr:  "%s"' % (clockCapsStr)

				rateStrs = clockCapsStr.split( ',' )
				for rateStr in rateStrs:
					decimalPos = rateStr.find( '.' )
					if (decimalPos >= 0):
						# strip off the sample rate's fraction
						rateStr = rateStr[0:decimalPos]
					sampleRateStrs.append( int( rateStr ) )

		return sampleRateStrs


	def getMinimumBufferSize( self, sampleRate ):
		minimumBufferSize = 32

		if (sys.platform == "win32"):
			if sampleRate <= 48:
					minimumBufferSize = 32
			elif sampleRate <= 96:
				minimumBufferSize = 64
			else:
				minimumBufferSize = 128
		elif (sys.platform == "darwin"):
			if sampleRate <= 48:
				minimumBufferSize = 32
			elif sampleRate <= 96:
				minimumBufferSize = 64
			else:
				minimumBufferSize = 128

		return minimumBufferSize


	def getMaximumBufferSize( self, sampleRate ):
		maximumBufferSize = 8192

		if (sys.platform == "win32"):
			pass
		elif (sys.platform == "darwin"):
			#
			# this only applies to the pre-3.5.0 driver
			#
			# if  (sampleRate >= 176):
			#	maximumBufferSize = 1600
			pass

		return maximumBufferSize


	def installDPCLatencyChecker( self ):
		if (sys.platform == "win32") and not self.dpcLatencyCheckerInstalled:
			# uninstall it first, so that our install attempt won't return an "already installed" error
			diceTool( "-am lu", captureOutput=True, printError=False )
			# install it
			returnCode, outputText = diceTool( "-am li", captureOutput=True, printError=False )
			outputTextLower = outputText.lower()
			if outputTextLower.find( "error" ) >= 0:
				print "WARNING: unable to install DPC latency checker (must run this as an Administrator)"
			else:
				print "DPC latency checker installed"
				self.dpcLatencyCheckerInstalled = True


	def uninstallDPCLatencyChecker( self ):
		if (sys.platform == "win32") and self.dpcLatencyCheckerInstalled:
			print "Uninstalling DPC latency checker..."
			diceTool( "-am lu", printError=False )


	def createLogDir( self ):
		print "Getting system information..."
		driverVersionStr = self.getDriverVersionStr()
		if not driverVersionStr:
			print "ERROR: unable to retrieve driver version; please check software installation and check hardware connections"
			sys.exit( 1 )
		hostNameStr = getHostNameStr()
		osVersionStr = getOSVersionStr()
		dateTimeStr = time.strftime('%Y-%m-%d_%H-%M', time.localtime( time.time() ) )

		self.logDirPath = "%s_%s_%s_%s" % (dateTimeStr, driverVersionStr, hostNameStr, osVersionStr )
		if options.debug:
			print "LogFile: %s" % (self.logDirPath)
		os.mkdir( self.logDirPath )


	def tdlInstalled( self ):
		if self.TDLisInstalled is None:
			self.TDLisInstalled = tdlDriverIsInstalled()
		return self.TDLisInstalled


	def startTDL( self, numStdLogEvents, numRltLogEvents ):
		if not self.tdlInstalled():
			return

		tdlCmdLine( "init std %d" % (numStdLogEvents) )
		tdlCmdLine( "init rlt %d" % (numRltLogEvents) )

		tdlCmdLine( "stopflags std 0x%08x" % (kTDLStopFlags) )
		tdlCmdLine( "stopflags rlt 0x%08x" % (kTDLStopFlags) )

		tdlCmdLine( "flags std 0x%08x" % (kTDLFlags) )
		tdlCmdLine( "flags rlt 0x%08x" % (kTDLFlags) )


	def stopTDLAndSaveLogIfErrors( self, filePostfixStr ):
		errorOccurred = False

		if not self.tdlInstalled():
			return

		# see if any of the logs stopped (which indicates an error occurred)
		logs = [ "std", "rlt" ]
		for logID in logs:
			returnCode, outputText = tdlCmdLine( "state %s" % (logID), captureOutput=True )
			outputLines = outputText.split( '\n' )
			for line in outputLines:
#				print "line: \"%s\"" % (line.rstrip())
				matchObj = re.match( r'^\s+flags:\s+(\w+)', line )
				if (matchObj != None):
					flagsHexStr = matchObj.group( 1 )
#					print "%s flags = \"%s\"" % (logID, flagsHexStr)
					if (flagsHexStr != "0x%08x" % (kTDLFlags) ):
						errorOccurred = True
					break

		tdlCmdLine( "flags std 0x00000000" )
		tdlCmdLine( "flags rlt 0x00000000" )

		if errorOccurred or options.tdl:
			if errorOccurred:
				tdlFileName = "Error_%s.tdl" % (filePostfixStr)
			else:
				tdlFileName = "Log_%s.tdl" % (filePostfixStr)

			tdlSaveFilePath = self.logDirPath + os.sep + tdlFileName

			if options.verbose:
				print "saving TDL log to \"%s\"" % (tdlFileName)
			tdlCmdLine( "save %s" % (tdlSaveFilePath) )

			if os.path.isfile( "tcatDriverLogFromTdlFile.py" ) and os.path.isfile( "logDictionary.tdd" ):
				try:
					stdOutArg = subprocess.PIPE
					stdErrArg = subprocess.STDOUT
					convertCmd = "tcatDriverLogFromTdlFile.py -t logDictionary.tdd %s %s_std.txt %s_rlt.txt" % (tdlSaveFilePath, tdlSaveFilePath, tdlSaveFilePath)
					if (sys.platform != "win32"):
						convertCmd = "./" + convertCmd
					p = subprocess.Popen( convertCmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True )
					stdOutText, stderrText = p.communicate()
					returnCode = p.returncode
				except KeyboardInterrupt:
					raise
				except:
					print 'ERROR: the dicetool command threw an exception'
				else:
					if options.debug:
						print "---------- stdout ----------"
						print stdOutText
						print "----------------------------"
					if (returnCode != 0):
						print "ERROR: tcatDriverLogFromTdlFile.py returned " + str(returnCode)

		tdlCmdLine( "free std" )
		tdlCmdLine( "free rlt" )


	def stopTDL( self ):
		if not self.tdlInstalled():
			return

		tdlCmdLine( "free std" )
		tdlCmdLine( "free rlt" )
		

	def printLabelRow( self, diceLogFileObj, htmlFile ):
		# labels row
		htmlFile.write( '<tr>' )
		htmlFile.write( '<th width="30">&nbsp;&nbsp;&nbsp;</td>' )
		for row in diceLogFileObj.getLabelsList():
			htmlFile.write( '<th><b>%s</b></td>' % (row) )
		htmlFile.write( '</tr>\n' )


	def convertLogFileToHtml( self, printResults=True ):
		if printResults:
			print "Analyzing log file..."

		if self.logFilePath.endswith( ".csv" ):
			fileBaseName = self.logFilePath[:-4]
			htmlFilePath = fileBaseName + ".html"
		else:
			print "ERROR: the specified log file does not have a .csv file extension"
			return

		# parse the log file
		diceLogFileObj = diceLogFile( self.logFilePath )

		# create the html output file
		htmlFile = open( htmlFilePath, 'w' )

		htmlFile.write( '<html>\n' )
		htmlFile.write( '<head>\n' )
		htmlFile.write( '<title>%s</title>\n' % (fileBaseName) )

		# CSS
		htmlFile.write( '<style type="text/css">\n' )
		htmlFile.write( '   body { font: 16px Arial, Helvetica, Verdana, sans-serif; background-color: #FFFFFF; }\n' )
		htmlFile.write( '   p.Configuration { font: 12px Arial, Helvetica, Verdana, sans-serif; }\n' )
		htmlFile.write( '   table.Basic { BORDER-COLLAPSE: collapse }\n' )
		htmlFile.write( '   table.Basic tr th { font: bold 12px Arial, Helvetica, Verdana, sans-serif; border:1px solid black; background-color: #99C1E9; }\n' )
		htmlFile.write( '   table.Basic tr td { font: 12px Arial, Helvetica, Verdana, sans-serif; border:1px solid black; text-align: center; }\n' )
		htmlFile.write( '   td.test-success { background-color: #33FF00; }\n' )
		htmlFile.write( '   td.test-error { font-weight: bold; background-color: #FF4444; }\n' )
		htmlFile.write( '   td.test-warning { font-weight: bold; background-color: #FFCC00; }\n' )
		htmlFile.write( '</style>\n' )

		htmlFile.write( '</head>\n' )	
		htmlFile.write( '<body>\n' )

		totalTests = diceLogFileObj.getNumTestResults()
		testsWithErrors = 0
		testsWithWarnings = 0
		testsSuccessful = 0
		testResults = []

		if self.usingInternalLoopback():
			for testIndex, testObj in enumerate( diceLogFileObj ):
				testObj.adjustLatencyForInternalLoopback()

		# summary
		for testIndex, testObj in enumerate( diceLogFileObj ):
			testResultCSSClass = None
			testHasError = False
			testHasWarning = False
			numRowsInTestResult = testObj.getNumRows()

			for rowIndex, rowList in enumerate( testObj ):
				for columnIndex, columnText in enumerate( rowList ):
					resultStr = testObj.analyzeColumn( rowIndex, columnIndex )
					if (resultStr == "error"):
						testHasError = True
					if (resultStr == "warning"):
						testHasWarning = True

			if testHasError or testHasWarning:
				if testHasError:
					testsWithErrors = testsWithErrors + 1
					testResultCSSClass = "test-error"
				if testHasWarning:
					testsWithWarnings = testsWithWarnings + 1
					if not testResultCSSClass:
						testResultCSSClass = "test-warning"
			else:
				testsSuccessful = testsSuccessful + 1
				testResultCSSClass = "test-success"

			testResults.append( testResultCSSClass )

		if printResults:
			print "----------------------------------------"
			print "    %d of %d tests were successful" % (testsSuccessful, totalTests)
			print "    %d tests had one or more errors" % (testsWithErrors)
			print "    %d tests had one or more warnings" % (testsWithWarnings)
			print "----------------------------------------"

		htmlFile.write( '<b>%d</b> of <b>%d</b> tests were successful<br>' % (testsSuccessful, totalTests) )
		htmlFile.write( '<b>%d</b> tests had one or more errors<br>' % (testsWithErrors) )
		htmlFile.write( '<b>%d</b> tests had one or more warnings<br>' % (testsWithWarnings) )

		# configuration info
		if diceLogFileObj.configurationLines:
			htmlFile.write( '<p class="Configuration">' )
			for line in diceLogFileObj.configurationLines:
				if line.startswith( "  " ):
					htmlFile.write( '&nbsp;&nbsp;&nbsp;&nbsp;%s<br>' % (line.strip()) )
				else:
					htmlFile.write( '%s<br>' % (line) )
			htmlFile.write( '</p>' )
			htmlFile.write( '<br>\n' )

		# table
		htmlFile.write( '<table class="Basic" width="100%" cellspacing="0" border="1">\n' )

		# labels row
		self.printLabelRow( diceLogFileObj, htmlFile )

		lastOpMode = -1
		labelRowCount = 0

		# test result rows
		for testIndex, testObj in enumerate( diceLogFileObj ):
			# print the label row if the opMode changes and there was at least 20 rows since the last label
			opMode = testObj.getOpMode()
			if (lastOpMode != -1) and (opMode != lastOpMode) and (labelRowCount >= 20):
				self.printLabelRow( diceLogFileObj, htmlFile )
				labelRowCount = 0
			lastOpMode = opMode
			labelRowCount = labelRowCount + 1

			numRowsInTestResult = testObj.getNumRows()
			for rowIndex, rowList in enumerate( testObj ):
				htmlFile.write( '<tr>' )
				for columnIndex, columnText in enumerate( rowList ):
					skipColumn = False
					rowSpanStr = ''
					cssClass = ''

					try:
						resultStr = testObj.analyzeColumn( rowIndex, columnIndex )
						if (resultStr == "error"):
							cssClass = ' class="test-error"'
						elif (resultStr == "warning"):
							cssClass = ' class="test-warning"'
					except:
						pass

					if (numRowsInTestResult > 1) and (rowIndex == 0) and (columnIndex <= testObj.getMaxCommonColumnIndex()):
						rowSpanStr = ' rowspan="%d"' % (numRowsInTestResult)
					elif (numRowsInTestResult > 1) and (rowIndex > 0) and (columnIndex <= testObj.getMaxCommonColumnIndex()):
						skipColumn = True

					if not skipColumn:
						if rowIndex == 0 and columnIndex == 0:
							# add a leading result row
							htmlFile.write( '<td class="%s"%s width="30"></td>' % (testResults[ testIndex ], rowSpanStr ) )
						htmlFile.write( '<td%s%s>%s</td>' % (cssClass, rowSpanStr, columnText) )
				htmlFile.write( '</tr>\n' )

		htmlFile.write( '</table>\n' )
		htmlFile.write( '</body>\n</html>\n' )
		htmlFile.close()




class StreamingTestCase( TestCaseBase ):
	def __init__( self ):
		TestCaseBase.__init__(self)
		self.numofDevices = 0
		self.logFilePath = None
		# defaults
		self.duration = 10
		self.opModes = ( 0, 1, 2, 3 )
		self.deviceModes = ( 10, 9 )
		self.sampleRates = None
		self.bufferSizes = ( 32, 64, 128, 256, 512, 1024, 2048 )
		self.useInternalLoopback = False


	def getTestName( self ):
		return "streaming"


	def getHelpText( self ):
		return "This is the standard streaming test.\n" \
				"\n" \
				"Usage: %s duration [options]\n" \
				"\n" \
				"  duration should specify the number of seconds to run each test for\n" \
				"\n" \
				"Options:\n" \
				"  sr=[sampleRates]    comma separated list of sample rates to test\n" \
				"  bs=[bufferSizes]    comma separated list of buffer sizes to test\n" \
				"  om=[opModes]        comma separated list of operation modes to test at\n" \
				"  dm=[deviceModes]    comma separated list of device modes to test at\n" \
				"                      (specifying '-' indicates that the device does not support modes)\n" \
				"  il=[0 or 1]         use internal routing within the DICE to loopback audio\n" \
				% (self.getTestName())


	def resetDeviceAfterTest( self ):
		try:
			if (self.getNumofDevicesAttached() == 0) or (self.deviceModes[0] == 'N/A'):
				return False
		except:
			pass
		return True


	def usingInternalLoopback( self ):
		return self.useInternalLoopback


	def processTestArgs( self, testArgs ):
		try:
			self.duration = int( testArgs[0] )
		except:
			print "ERROR: this test requires a duration argument"
			sys.exit( 1 )
		if (self.duration <= 0):
			print "ERROR: this test requires a duration argument that is greater than zero"
			sys.exit( 1 )

		for testArg in testArgs[1:]:
			argPrefixStr = testArg[0:3]
			argValueStr = testArg[3:]

			if (len(argPrefixStr) != 3) or (argPrefixStr[2] != '='):
				print 'ERROR: unknown option "%s"' % (argPrefixStr)
				sys.exit( 1 )
			if not argValueStr:
				print 'ERROR: no argument supplied for option "%s"' % (argPrefixStr)
				sys.exit( 1 )

			if argPrefixStr == 'sr=':
				sampleRateStrs = argValueStr.split( ',' )
				self.sampleRates = []
				for sampleRateStr in sampleRateStrs:
					try:
						sampleRateInt = int( sampleRateStr )
					except:
						print 'ERROR: illegal sample rate argument "%s"' % (sampleRateStr)
						sys.exit( 1 )
					else:
						self.sampleRates.append( sampleRateInt )

			elif argPrefixStr == 'bs=':
				bufferSizeStrs = argValueStr.split( ',' )
				self.bufferSizes = []
				for bufferSizeStr in bufferSizeStrs:
					try:
						bufferSizeInt = int( bufferSizeStr )
					except:
						print 'ERROR: illegal buffer size argument "%s"' % (bufferSizeStr)
						sys.exit( 1 )
					else:
						self.bufferSizes.append( bufferSizeInt )

			elif argPrefixStr == 'om=':
				opModeStrs = argValueStr.split( ',' )
				self.opModes = []
				for opModeStr in opModeStrs:
					try:
						opModeInt = int( opModeStr )
					except:
						print 'ERROR: illegal operation mode argument "%s"' % (opModeStr)
						sys.exit( 1 )
					else:
						if (opModeInt < 0) or (opModeInt > 3):
							print 'ERROR: illegal operation mode argument "%d" (must be between 0 and 3)' % (opModeInt)
							sys.exit( 1 )
						self.opModes.append( opModeInt )

			elif argPrefixStr == 'dm=':
				self.deviceModes = []
				if argValueStr == '-':
					# indicates that the device does not support modes
					self.deviceModes.append( 'N/A' )
				else:
					deviceModeStrs = argValueStr.split( ',' )
					for deviceModeStr in deviceModeStrs:
						try:
							deviceModeInt = int( deviceModeStr )
						except:
							print 'ERROR: illegal device mode argument "%s"' % (deviceModeStr)
							sys.exit( 1 )
						else:
							self.deviceModes.append( deviceModeInt )

			elif argPrefixStr == 'il=':
				try:
					loopbackInt = int( argValueStr )
				except:
					print 'ERROR: illegal internal loopback argument "%s"' % (argValueStr)
					sys.exit( 1 )
				else:
					if (loopbackInt != 0) and (loopbackInt != 1):
						print 'ERROR: illegal internal loopback argument "%d" (must be between 0 or 1)' % (loopbackInt)
						sys.exit( 1 )

					if (loopbackInt == 0):
						self.useInternalLoopback = False
					else:
						self.useInternalLoopback = True

			else:
				print 'ERROR: unknown option "%s"' % (argPrefixStr)
				sys.exit( 1 )


	def doTest( self ):
		print "Checking for attached devices..."
		self.numofDevices = self.getNumofDevicesAttached()
		if (self.numofDevices < 1) or (self.numofDevices > 4):
			print "ERROR: this test requires 1 device to be attached (devices found: %d)" % (self.numofDevices)
			sys.exit( 1 )

		try:

			self.createLogDir()
			self.logFilePath = self.logDirPath + os.sep + "DiceToolLog.csv"

			if self.sampleRates == None:
				print "Getting supported sample rates..."
				self.sampleRates = self.getSupportedSampleRates()

			self.installDPCLatencyChecker()

			kDiceToolOverheadSecs = 6
			numofOpModeChanges = len( self.opModes )
			numofSampleRateChanges = len( self.sampleRates )
			numofDeviceModeChanges = 0
			numofTests = 0

			for opMode in self.opModes:
				for deviceMode in self.deviceModes:
					if deviceMode != 'N/A':
						numofDeviceModeChanges = numofDeviceModeChanges + 1
					for sampleRate in self.sampleRates:
						numofSampleRateChanges = numofSampleRateChanges + 1
						for bufferSize in self.bufferSizes:
							minimumBufferSize = self.getMinimumBufferSize( sampleRate )
							maximumBufferSize = self.getMaximumBufferSize( sampleRate )
							if (bufferSize >= minimumBufferSize) and (bufferSize <= maximumBufferSize):
								numofTests = numofTests + 1

			estimatedTestTime =   (numofOpModeChanges * kDiceToolOverheadSecs) \
								+ (numofDeviceModeChanges * (kDeviceResetDelaySecs + kDiceToolOverheadSecs)) \
								+ (numofSampleRateChanges * (kDiceToolOverheadSecs)) \
								+ (numofTests * (self.duration + kDiceToolOverheadSecs))

			print "========================================"
			print "=   Test Configuration:"
			print "=     SampleRates To Test:  %s" % (", ".join( str(n) for n in self.sampleRates ))
			print "=     Duration Per Test  :  %d secs  (%s)" % (self.duration, getTimeDurationStr( self.duration ))
			print "=     Operation Modes    :  %s" % (", ".join( str(n) for n in self.opModes ))
			print "=     Device Modes       :  %s" % (", ".join( str(n) for n in self.deviceModes ))
			print "=     Buffer Sizes       :  %s" % (", ".join( str(n) for n in self.bufferSizes ))
			print "=     Internal Loopback  :  %s" % yesNoString( self.useInternalLoopback )
			print "= "
			print "=   Estimated Test Time  :  %s" % (getTimeDurationStr( estimatedTestTime ))
			print "========================================"
			print ""

			if deviceMode != 'N/A':
				deviceModeStr = "%02d" % (deviceMode)
			else:
				deviceModeStr = "X"

			self.stopTDL()

			for opMode in self.opModes:
				print "========================================"
				print "======= Testing Operation Mode %d =======" % (opMode)
				print "========================================"

				diceTool( "+q -am sg2 bo%d sg2" % (opMode) )

				for deviceMode in self.deviceModes:
					if deviceMode != 'N/A':
						print "----- Testing Device Mode %d -----" % (deviceMode)
						for deviceNum in range(self.numofDevices):
							diceTool( "+q -am #d%d dm%d dr" % (deviceNum, deviceMode) )
							print "waiting %d secs for device %d to reset..." % (kDeviceResetDelaySecs, deviceNum + 1)
							time.sleep( kDeviceResetDelaySecs )

					for sampleRate in self.sampleRates:
						print "----- Testing %dkHz -----" % (sampleRate)
						if options.verbose:
							print "setting samplerate..."
						self.startTDL( kTDLNumStdLogEvents, kTDLNumRltLogEvents )
						diceTool( "+q -am byInternal bc%d" % (sampleRate) )
						if self.useInternalLoopback:
							if options.verbose:
								print "setting internal audio loopback within device..."
							for deviceNum in range(self.numofDevices):
								diceTool( "+q -am sg4 #d%d dl" % (deviceNum) )
						self.stopTDLAndSaveLogIfErrors( "om%d_dm%s_sr%003d_setSR" % (opMode, deviceModeStr, sampleRate) )

						if options.verbose:
							print "testing..."

						printedMinimumBufferSize = False

						for bufferSize in self.bufferSizes:
							minimumBufferSize = self.getMinimumBufferSize( sampleRate )
							if (bufferSize < minimumBufferSize):
								if options.verbose:
									print "skipping test at buffer size %d (below the driver minimum %d)" % (bufferSize, minimumBufferSize)
								else:
									if not printedMinimumBufferSize:
										print "the minimum driver buffer size is %d" % (minimumBufferSize)
										# only print this message once per sample rate
										printedMinimumBufferSize = True
								continue
							maximumBufferSize = self.getMaximumBufferSize( sampleRate )
							if (bufferSize > maximumBufferSize):
								if options.verbose:
									print "skipping test at buffer size %d (above the driver maximum %d)" % (bufferSize, maximumBufferSize)
								continue
							self.startTDL( kTDLNumStdLogEvents, kTDLNumRltLogEvents )
							diceTool( "+q bb%d sg5 be de bt%d,%s" % (bufferSize, self.duration, self.logFilePath) )
							self.stopTDLAndSaveLogIfErrors( "om_%d_dm%s_sr%003d_stream_bs%d" % (opMode, deviceModeStr, sampleRate, bufferSize) )

					print ""

			print "========================================"
		except KeyboardInterrupt:
			self.stopTDL()
			self.uninstallDPCLatencyChecker()

			if os.path.isfile( self.logFilePath ):
				self.convertLogFileToHtml()
			raise
		except:
			self.stopTDL()
			self.uninstallDPCLatencyChecker()
			raise

		self.uninstallDPCLatencyChecker()

		if os.path.isfile( self.logFilePath ):
			self.convertLogFileToHtml()




class MicroAlignmentTestCase( TestCaseBase ):
	def __init__( self ):
		TestCaseBase.__init__(self)
		self.numofDevices = 0
		self.logFilePath = None
		# defaults
		self.numRestarts = 10
		self.duration = 2
		self.opMode = 0
		self.deviceMode = 10
		self.sampleRates = None
		self.bufferSize = 512


	def getTestName( self ):
		return "micro-alignment"


	def getHelpText( self ):
		return "This tests the driver's micro-alignment.\n" \
				"\n" \
				"Usage: %s numRestarts [options]\n" \
				"\n" \
				"  numRestarts should specify the number of times to restart streaming per sample rate\n" \
				"\n" \
				"Options:\n" \
				"  sr=[sampleRates]    comma separated list of sample rates to test\n" \
				"  bs=[bufferSizes]    comma separated list of buffer sizes to test\n" \
				"  om=[opMode]         the operation modes to test at\n" \
				"  dm=[deviceMode]     the device modes to test at\n" \
				"                      (specifying '-' indicates that the device does not support modes)\n" \
				% (self.getTestName())


	def resetDeviceAfterTest( self ):
		try:
			if self.deviceMode == 'N/A':
				return False
		except:
			pass
		return True


	def processTestArgs( self, testArgs ):
		try:
			self.numRestarts = int( testArgs[0] )
		except:
			print "ERROR: this test requires a numRestarts argument"
			sys.exit( 1 )
		if (self.numRestarts <= 0):
			print "ERROR: this test requires a numRestarts argument that is greater than zero"
			sys.exit( 1 )

		for testArg in testArgs[1:]:
			argPrefixStr = testArg[0:3]
			argValueStr = testArg[3:]

			if (len(argPrefixStr) != 3) or (argPrefixStr[2] != '='):
				print 'ERROR: unknown option "%s"' % (argPrefixStr)
				sys.exit( 1 )
			if not argValueStr:
				print 'ERROR: no argument supplied for option "%s"' % (argPrefixStr)
				sys.exit( 1 )

			if argPrefixStr == 'sr=':
				sampleRateStrs = argValueStr.split( ',' )
				self.sampleRates = []
				for sampleRateStr in sampleRateStrs:
					try:
						sampleRateInt = int( sampleRateStr )
					except:
						print 'ERROR: illegal sample rate argument "%s"' % (sampleRateStr)
						sys.exit( 1 )
					else:
						self.sampleRates.append( sampleRateInt )

			elif argPrefixStr == 'bs=':
				try:
					bufferSizeInt = int( argValueStr )
				except:
					print 'ERROR: illegal buffer size argument "%s"' % (argValueStr)
					sys.exit( 1 )
				else:
					self.bufferSize = bufferSizeInt

			elif argPrefixStr == 'om=':
				try:
					opModeInt = int( argValueStr )
				except:
					print 'ERROR: illegal operation mode argument "%s"' % (argValueStr)
					sys.exit( 1 )
				else:
					if (opModeInt < 0) or (opModeInt > 3):
						print 'ERROR: illegal operation mode argument "%d" (must be between 0 and 3)' % (opModeInt)
						sys.exit( 1 )
					self.opMode = opModeInt

			elif argPrefixStr == 'dm=':
				if argValueStr == '-':
					# indicates that the device does not support modes
					self.deviceMode = 'N/A'
				else:
					try:
						deviceModeInt = int( argValueStr )
					except:
						print 'ERROR: illegal device mode argument "%s"' % (argValueStr)
						sys.exit( 1 )
					else:
						self.deviceMode = deviceModeInt

			else:
				print 'ERROR: unknown option "%s"' % (argPrefixStr)
				sys.exit( 1 )


	def doTest( self ):
		try:
			print "Checking for attached devices..."
			self.numofDevices = self.getNumofDevicesAttached()
			if (self.numofDevices < 1) or (self.numofDevices > 4):
				print "ERROR: this test requires 1 device to be attached (devices found: %d)" % (self.numofDevices)
				sys.exit( 1 )

			self.createLogDir()
			self.logFilePath = self.logDirPath + os.sep + "DiceToolLog.csv"

			if self.sampleRates == None:
				print "Getting supported sample rates..."
				self.sampleRates = self.getSupportedSampleRates()

			print "========================================"
			print "=   Test Configuration:"
			print "=     SampleRates To Test:  %s" % (", ".join( str(n) for n in self.sampleRates ))
			print "=     Restarts Per Rate  :  %d" % (self.numRestarts)
			print "=     Operation Mode     :  %d" % (self.opMode)
			print "=     Device Mode        :  %s" % (str(self.deviceMode))
			print "=     Buffer Size        :  %d" % (self.bufferSize)
			print "========================================"

			# switch the operation mode before/after each test in order to force the driver to restart streaming
			if self.opMode == 0:
				opModeToSwitchTo = 1
			else:
				opModeToSwitchTo = 0

			if self.deviceMode != 'N/A':
				print "Setting device mode"
				for deviceNum in range(self.numofDevices):
					diceTool( "+q -am #d%d dm%d byInternal dr" % (deviceNum, self.deviceMode) )
					print "waiting %d secs for device %d to reset..." % (kDeviceResetDelaySecs, deviceNum + 1)
					time.sleep( kDeviceResetDelaySecs )

			for sampleRate in self.sampleRates:
				print "----- Testing %dkHz -----" % (sampleRate)
				if options.verbose:
					print "setting samplerate..."
				diceTool( "+q -am bc%d" % (sampleRate) )
				if options.verbose:
					print "setting internal audio loopback within device..."
				diceTool( "+q -am sg4 dl" )

				if options.verbose:
					print "testing..."
				for n in range( self.numRestarts ):
					diceTool( "+q -m bo%d sg2 bb%d sg5 be de bt%d,%s bo%d sg2" % (self.opMode, self.bufferSize, self.duration, self.logFilePath, opModeToSwitchTo) )

			print "========================================"
		except KeyboardInterrupt:
			if os.path.isfile( self.logFilePath ):
				self.convertLogFileToHtml()
			raise
		except:
			raise

		if os.path.isfile( self.logFilePath ):
			self.convertLogFileToHtml()




class ConvertLogFileTestCase( TestCaseBase ):
	def __init__( self ):
		TestCaseBase.__init__(self)
		self.logFilePath = None


	def getTestName( self ):
		return "convertLogFile"


	def getHelpText( self ):
		return "This \"test\" just converts a dice tool .csv output file to an html log file.\n" \
				"\n" \
				"Usage: %s csvFileToConvert\n" % (self.getTestName())

		return "This will convert a dice tool .csv output file to an html log file"


	def timeTheTest( self ):
		return False


	def processTestArgs( self, testArgs ):
		try:
			self.logFilePath = testArgs[0]
		except:
			print "ERROR: analyze requires an argument that specifies the path to the log file"
			sys.exit( 1 )


	def doTest( self ):
		if os.path.isfile( self.logFilePath ):
			self.convertLogFileToHtml()
		else:
			print "ERROR: the specified log file does not exist"
			sys.exit( 1 )




def main():
	availableTestCases = \
	(
		StreamingTestCase,
		MicroAlignmentTestCase,
		ConvertLogFileTestCase
	)

	usage = "usage: %prog [options] testName [testArgs]\n" +\
		"\n" +\
		"Type '%prog help <testName>' for help on a specific test.\n" +\
		"\n" +\
		"Available TestNames:"

	try:
		for testCase in availableTestCases:
			usage = usage + "\n  %s" % (testCase().getTestName())
	except:
		print "ERROR: an exception occurred while building the list of test names"

	parser = optparse.OptionParser( usage, version=kVersionStr )
	if (sys.platform == "darwin"):
		parser.add_option('-6', '--64bit', action="store_true", dest="dice64bit", default=False, help="run the dice tool as a 64-bit application")
	parser.add_option('-d', '--debug', action="store_true", dest="debug", default=False, help="verbose debug output")
	parser.add_option('-s', '--sleep', action="store_true", dest="sleep", default=False, help="put the computer to sleep after the test suite is completed")
	parser.add_option('-t', '--tdl', action="store_true", dest="tdl", default=False, help="if TDL is present, capture each test step into a TDL log file")
	parser.add_option('-v', '--verbose', action="store_true", dest="verbose", default=False, help="verbose output")
	global options
	options, args = parser.parse_args()

	if len( args ) < 1:
		parser.print_help()
		sys.exit( 1 )

	testName = args[0]
	numTestArgs = len( args ) - 1
	testArgs = args[1:]

	if options.debug:
		print "test arguments: " + str(testArgs)

	# handle "help <testName>"
	if testName == "help":
		try:
			testName = args[1]
		except:
			parser.print_help()
			sys.exit( 1 )
		else:
			testCaseObj = None
			for testCase in availableTestCases:
				testCaseObj = testCase()
				if (testName == testCaseObj.getTestName()):
					break
				testCaseObj = None

			if testCaseObj:
				print testCaseObj.getHelpText()
				sys.exit( 0 )
			else:
				print "ERROR:  unknown test case name"
				parser.print_help()
				sys.exit( 1 )

	# evaluate <testName>
	testCaseObj = None
	for testCase in availableTestCases:
		testCaseObj = testCase()
		if (testName == testCaseObj.getTestName()):
			break
		testCaseObj = None

	if not testCaseObj:
		print "ERROR:  unknown test case name"
		parser.print_help()
		sys.exit( 1 )

	# execute <testName>
	resetDeviceAfterTest = False
	testStartTime = None
	try:
		print "Starting %s test..." % (testCaseObj.getTestName())
		testCaseObj.processTestArgs( testArgs )
		resetDeviceAfterTest = testCaseObj.resetDeviceAfterTest()
		if testCaseObj.timeTheTest():
			testStartTime = time.time()
		testCaseObj.doTest()
	except KeyboardInterrupt:
		print ''
		print '-----'
		print ' Cntrl-C detected, exiting...'
		print '-----'
	except:
		raise

	if resetDeviceAfterTest:
		print "Resetting device(s) to 44.1kHz and to Device Mode 0"
		try:
			numofDevices = testCaseObj.getNumofDevicesAttached()
			diceTool( "+q -am bc44 sg4" )
			for deviceNum in range(numofDevices):
				diceTool( "+q -am #d%d dm0 dr" % (deviceNum) )
				print "waiting %d secs for device %d to reset..." % (kDeviceResetDelaySecs, deviceNum + 1)
				time.sleep( kDeviceResetDelaySecs )
		except KeyboardInterrupt:
			print ' Cntrl-C detected, exiting...'

	if testStartTime:
		testEndTime = time.time()
		print "=============================="
		print "Total Test Time:  %s" % (getTimeDurationStr( (testEndTime - testStartTime) + 0.5 ))
		print "=============================="

	if options.sleep:
		print "Putting the machine to sleep..."
		putMachineToSleep()

	print "Done"




if __name__ == '__main__':
	main()

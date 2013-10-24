#!/usr/bin/env python
##==============================================================================
##
## This confidential and proprietary software may be used only as authorised
## by a licensing agreement from TC Applied Technologies Ltd.
##
## (C) COPYRIGHT 2010 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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
import os, os.path, optparse, subprocess, time



def main():
	usage = "usage: \%prog [options] fileToSign\n\n" +\
		"This script depends on several environment variables:\n" +\
		"  W7BASE                 [path to the Windows DDK 7600 dir]      (required or WLHBASE)\n" +\
		"  WLHBASE                [path to the Windows DDK 6001 dir]      (required or W7BASE)\n" +\
		"  SELFSIGN_CERT_NAME     [the Subject Name of the signing cert]  (required)\n" +\
		"                           if the Subject Name is \"automatic\" it will select the best signing cert automatically\n" +\
		"  SELFSIGN_TIMESTAMP_URL [the timestamp server's URL]            (optional)\n" +\
		"  SELFSIGN_CROSS_CERT    [path to the cross-sign cert file]      (only required with the -c option)"

	parser = optparse.OptionParser( usage )
	parser.add_option('-c', '--crosscert', action="store_true", dest="addCrossCert", default=False, help ="add the cross-sign signature ")
	parser.add_option('-v', '--verbose', action="store_true", dest="verbose", default=False, help ="verbose output")
	global options
	options, args = parser.parse_args()

	if len( args ) != 1:
		parser.print_help()
		sys.exit( 1 )

	fileToSignPath = args[0]

	# make sure the files exists (or okay if it has the special pattern matching character '*')
	if not os.path.isfile( fileToSignPath ) and fileToSignPath.rfind( "*" ) < 0:
		print 'error: the file to sign "%s" does not exist' % fileToSignPath
		sys.exit( 1 )

	# get the selfsign settings from environment variables
	try:
		selfsignCertName = os.environ["SELFSIGN_CERT_NAME"]
	except:
		selfsignCertName = None

	try:
		selfsignCrossCert = os.environ["SELFSIGN_CROSS_CERT"]
	except:
		selfsignCrossCert = None

	try:
		selfsignTimeStampURL = os.environ["SELFSIGN_TIMESTAMP_URL"]
	except:
		selfsignTimeStampURL = None

	selfsignTimeStampRetries = 0
	if selfsignTimeStampURL:
		try:
			selfsignTimeStampRetries = int( os.environ["SELFSIGN_TIMESTAMP_RETRIES"] )
			if selfsignTimeStampRetries < 0:
				selfsignTimeStampRetries = 0
		except:
			pass

	# there's nothing to do if the "SELFSIGN_CERT_NAME" environment variable isn't set
	if not selfsignCertName:
		print "warning: skipped signing of \"%s\" because the SELFSIGN_CERT_NAME environment variable is not set" % (fileToSignPath)
		sys.exit( 0 )

	# get the WDK 6001 base path from the environment variable
	try:
		whlBasePath = os.environ["WLHBASE"]
	except:
		foundWLHBASE = False
	else:
		foundWLHBASE = True

	# get the WDK 7600 base path from the environment variable
	try:
		w7BasePath = os.environ["W7BASE"]
	except:
		foundW7BASE = False
	else:
		foundW7BASE = True

	if not foundWLHBASE and not foundW7BASE:
		print "error: unable to get the WDK base path from either the WLHBASE or W7BASE environment variable"
		sys.exit( 1 )

	# favor using signtool.exe from the newer WDK 7600 if it exists
	if foundW7BASE:
		signtoolCmd = os.path.join( w7BasePath, "bin", "x86", "signtool.exe" )
	else:
		signtoolCmd = os.path.join( whlBasePath, "bin", "SelfSign", "signtool.exe" )

	signtoolCmd = signtoolCmd + ' sign'

	if options.addCrossCert:
		if not selfsignCrossCert:
			print "error: unable to get the path to the cross-sign cert file from the SELFSIGN_CROSS_CERT environment variable"
			sys.exit( 1 )
		signtoolCmd = signtoolCmd + ' /ac "%s"' % (selfsignCrossCert)

	if selfsignCertName == "automatic":
		signtoolCmd = signtoolCmd + ' /a'
	else:
		signtoolCmd = signtoolCmd + ' /n "%s"' % (selfsignCertName)

	if selfsignTimeStampURL:
		signtoolCmd = signtoolCmd + ' /t "%s"' % (selfsignTimeStampURL)
	else:
		print "warning: signing with no timestamp because the SELFSIGN_TIMESTAMP_URL environment variable is not set"

	signtoolCmd = signtoolCmd + ' %s' % fileToSignPath

	if options.verbose:
		print ""
		print "cmd: %s" % (signtoolCmd)
		print ""

	# sign the file
	while True:
		try:
			p = subprocess.Popen( signtoolCmd, bufsize=-1, shell=True )
			returnCode = p.wait()
		except:
			print 'error: the signtool command caused an exception'
			returnCode = -1

		if returnCode == 0:
			break
		else:
			if selfsignTimeStampRetries == 0:
				print "error: the signtool returned an error (" + str(returnCode) + ")"
				sys.exit( 1 )

			print "warning: the signtool returned an error (" + str(returnCode) + "), waiting 30 seconds before retrying..."
			try:
				time.sleep( 30 )
			except:
				sys.exit( 2 )
			selfsignTimeStampRetries = selfsignTimeStampRetries - 1



if __name__ == '__main__':
	main()

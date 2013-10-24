#!/bin/bash
##==============================================================================
##
## This confidential and proprietary software may be used only as authorised
## by a licensing agreement from TC Applied Technologies Ltd.
##
## (C) COPYRIGHT 2008 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
##
## Unauthorized use, duplication or distribution of this software is  
## strictly prohibited by law.
##
## The entire notice above must be reproduced on all authorised copies and
## copies may only be made to the extent permitted by a licensing agreement
## from TC Applied Technologies Ltd.
##
##==============================================================================


#-------------------------------------------------------------------------------
# configuration
#-------------------------------------------------------------------------------

binaryDir="../../../binary"
resourceDir="../../../resource"

defaultsHdr="../../../interface/tcat_dice_defaults.h"
myproductHdr="../../../interface/tcat_dice_myproduct.h"
versionHdr="../../../interface/tcat_dice_version.h"


#-------------------------------------------------------------------------------
# functions
#-------------------------------------------------------------------------------

printUsage()
{
	echo "usage: -- ${0##./*/} [options]"
	echo "    options:  -b BuildType             use the specified build type, instead of the default (Release)"
	echo "              -c ControlPanelAppPath   use the specified Control Panel application, instead of the default"
	echo "              -i BackgroundImagePath   use the specified installer background image (620 x 418 pixels; jpg, tiff, gif, pict, eps, or pdf)"
	echo "              -l LicensePath           use the specified license doc (rtf, rtfd, txt, or html)"
	echo "              -r ReadMePath            use the specified Readme doc (rtf, rtfd, txt, or html)"
	echo "              -u                       do not include the uninstaller application on the disk image"
	echo "              -w WelcomePath           use the specified Welcome doc (rtf, rtfd, txt, or html)"
}


parseScriptArguments()
{
	BUILD_TYPE="Release"
	CONTROL_PANEL_APP_PATH=""
	BACKGROUND_IMAGE_PATH=""
	LICENSE_DOC_PATH=""
	README_DOC_PATH=""
	INCLUDE_UNINSTALLER=yes
	WELCOME_DOC_PATH=""

	VERBOSE=no
	DEV_DEBUG=no
	version=""

	while getopts ":b:c:di:l:r:uw:v" opt; do
		case ${opt} in
			b  ) BUILD_TYPE=$OPTARG ;;
			c  ) CONTROL_PANEL_APP_PATH="$OPTARG"
				 # workaround since getopts cannot support spaces in the option's path argument
				 let count=1
				 for arg; do
					if [ $count -eq $OPTIND ]; then
						case $arg in
							-* ) break ;;
							 * ) CONTROL_PANEL_APP_PATH="${CONTROL_PANEL_APP_PATH} ${arg}"
								 let OPTIND=OPTIND+1
						esac
					fi
					let count=count+1
				 done ;;
			d  ) DEV_DEBUG="yes" ;;
			i  ) BACKGROUND_IMAGE_PATH="$OPTARG" ;;
			l  ) LICENSE_DOC_PATH="$OPTARG" ;;
			r  ) README_DOC_PATH="$OPTARG" ;;
			u  ) INCLUDE_UNINSTALLER="no" ;;
			v  ) VERBOSE="yes" ;;
			w  ) WELCOME_DOC_PATH="$OPTARG" ;;
			:  ) printUsage
				 echo "error: the option \"-${OPTARG}\" requires an argument"
				 exit 1 ;;
			\? ) printUsage
				 echo "error: unsupported option \"-${OPTARG}\""
				 exit 1
		esac
	done
	shift $(($OPTIND - 1))

	if [ $# -ne 0 ]; then
		printUsage
		echo "error: unsupported argument \"$@\"."
		exit 1
	fi

	# extract drv and vendor short values
	drv=$(awk '/#define kTCAT_DICE_DRV\t/{print $3}' "${myproductHdr}")
	if [ -z ${drv} ]; then
		echo "error: unable to extract kTCAT_DICE_DRV from \"${myproductHdr}\"."
		exit 1
	fi
	vendorshort=$(awk '/#define kTCAT_DICE_VENDOR_SHORT\t/{print $3}' "${myproductHdr}")
	if [ -z ${vendorshort} ]; then
		echo "error: unable to extract kTCAT_DICE_VENDOR_SHORT from \"${myproductHdr}\"."
		exit 1
	fi

	# extract UTI
	uti=$(awk '/#define kTCAT_DICE_VENDOR_UTI\t/{print $3}' "${myproductHdr}")
	if [ -z ${uti} ]; then
		# it has't been overridden in myproduct.h, so grab it from defaults.h
		uti=$(awk '/#define kTCAT_DICE_VENDOR_UTI\t/{print $3}' "${defaultsHdr}")
		if [ -z ${uti} ]; then
			echo "error: unable to extract kTCAT_DICE_VENDOR_UTI from \"${myproductHdr}\" or \"${defaultsHdr}\"."
			exit 1
		fi
	fi

	if [ -z "${version}" ]; then
		# default to extracting the version from the version header file
		version=$(awk '/kTCAT_DICE_VERSION_MAJOR\t/{printf $3"."}/kTCAT_DICE_VERSION_MINOR\t/{printf $3"."}/kTCAT_DICE_VERSION_SUB\t/{printf $3"."}/kTCAT_DICE_VERSION_BUILD\t/{print $3}' "${versionHdr}")
		if [ -z ${version} ]; then
			echo "error: unable to extract kTCAT_DICE_VERSION fields from \"${versionHdr}\"."
			exit 1
		fi
		versionMajMinSub=$(awk '/kTCAT_DICE_VERSION_MAJOR\t/{printf $3"."}/kTCAT_DICE_VERSION_MINOR\t/{printf $3"."}/kTCAT_DICE_VERSION_SUB\t/{printf $3}' "${versionHdr}")
		if [ -z ${versionMajMinSub} ]; then
			echo "error: unable to extract kTCAT_DICE_VERSION fields from \"${versionHdr}\"."
			exit 1
		fi
	fi

	if [ ! -z "${CONTROL_PANEL_APP_PATH}" ]; then
		# make sure the specified file exists
		if [ ! -d "${CONTROL_PANEL_APP_PATH}" ]; then
			printUsage
			echo "error: the specified Control Panel application (.app directory) does not exist."
			exit 1
		fi
	else
		# set the default
		CONTROL_PANEL_APP_PATH="${binaryDir}/tools/cpl/osx/${BUILD_TYPE}/${drv} Control Panel.app"
	fi
	CONTROL_PANEL_APP_NAME=`basename "${CONTROL_PANEL_APP_PATH}"`

	if [ ! -z "${BACKGROUND_IMAGE_PATH}" ]; then
		checkBackgroundImage "${BACKGROUND_IMAGE_PATH}"
	else
		# default to "resource/installer_background.tiff" if it exists
		if [ -f "${resourceDir}/installer_background.tiff" ]; then
			BACKGROUND_IMAGE_PATH="${resourceDir}/installer_background.tiff"
		fi
	fi

	if [ ! -z "${LICENSE_DOC_PATH}" ]; then
		checkDocument "License" "${LICENSE_DOC_PATH}"
	else
		# default to "resource/License.rtf" if it exists
		if [ -f "${resourceDir}/License.rtf" ]; then
			LICENSE_DOC_PATH="${resourceDir}/License.rtf"
		fi
	fi

	if [ ! -z "${README_DOC_PATH}" ]; then
		checkDocument "ReadMe" "${README_DOC_PATH}"
	else
		# default to "resource/ReadMe.rtf" if it exists
		if [ -f "${resourceDir}/ReadMe.rtf" ]; then
			README_DOC_PATH="${resourceDir}/ReadMe.rtf"
		fi
	fi

	if [ ! -z "${WELCOME_DOC_PATH}" ]; then
		checkDocument "Welcome" "${WELCOME_DOC_PATH}"
	else
		# default to "resource/Welcome.rtf" if it exists
		if [ -f "${resourceDir}/Welcome.rtf" ]; then
			WELCOME_DOC_PATH="${resourceDir}/Welcome.rtf"
		fi
	fi
}


MakeDir()
{
	mkdir -p "$1"
	if [ $? -ne 0 ]; then
		echo "error: mkdir failed"
		exit 1
	fi
}


MoveFile()
{
	mv "$1" "$2"
	if [ $? -ne 0 ]; then
		echo "error: move failed"
		exit 1
	fi
}


CopyFile()
{
	cp -f "$1" "$2"
	if [ $? -ne 0 ]; then
		echo "error: copy failed"
		exit 1
	fi
}


CopyDir()
{
	cp -Rf "$1" "$2"
	if [ $? -ne 0 ]; then
		echo "error: copy failed"
		exit 1
	fi

	# Remove any SVN sub-directories from the destination
	/usr/bin/find "$2" -name .svn -print0 | xargs -0 rm -rf

	# Remove any .DS_Store files from the destination
	/usr/bin/find "$2" -name .DS_Store -print0 | xargs -0 rm -rf
}


customizeFile()
{
	../../scripts/customize.py "$1" "${defaultsHdr}" "$2"
	if [ $? -ne 0 ]; then
		echo "error: customize failed"
		exit 1
	fi
}


checkBackgroundImage()
{
	IMAGE_PATH="$1"

	# make sure the specified file exists
	if [ ! -f "${IMAGE_PATH}" ]; then
		printUsage
		echo "error: the specified Background image does not exist."
		exit 1
	fi

	IMAGE_EXT="${IMAGE_PATH##*.}"
	case "${IMAGE_EXT}" in
		jpg | tif | tiff | gif | pict | eps | pdf ) ;;
		* ) printUsage
			echo "error: the specified Background image format \"${IMAGE_EXT}\" is not supported."
			exit 1
	esac
}


checkDocument()
{
	DOC_NAME="$1"
	DOC_PATH="$2"

	# make sure the specified file exists
	if [ ! -f "${DOC_PATH}" ]; then
		printUsage
		echo "error: the specified ${DOC_NAME} document does not exist."
		exit 1
	fi

	DOC_EXT="${DOC_PATH##*.}"
	case "${DOC_EXT}" in
		rtf | rtfd | txt | html ) ;;
		* ) printUsage
			echo "error: the specified ${DOC_NAME} document format \"${DOC_EXT}\" is not supported."
			exit 1
	esac
}



#-------------------------------------------------------------------------------
# script starts here
#-------------------------------------------------------------------------------

parseScriptArguments $@

installerName="${drv}-${version}"

echo "Creating ${drv} installer version ${version} ${BUILD_TYPE} [${installerName}.pkg, ${drv}.dmg]"

build="build"
diskImageDir="${build}/diskimage/${installerName}"

if [ -e "${build}" ]; then
	rm -rf "${build}"
fi

# Create the top-level work directories
MakeDir "${build}"
MakeDir "${diskImageDir}"

#-------------------------------------------------------------------------------

echo "Creating install file hierarchy..."

pkgrootdirs="${build}/package-roots"
pkgsdir="${build}/packages"

MakeDir "${pkgrootdirs}"
MakeDir "${pkgsdir}"

#-------------------------------------------------------------------------------

echo
echo "----------------------------------------"
echo "Creating Control Panel package..."
echo "----------------------------------------"

pkgroot="${pkgrootdirs}/Control Panel/root"
pkgscripts="${pkgrootdirs}/Control Panel/scripts"
MakeDir "${pkgroot}"
MakeDir "${pkgscripts}"

# Control Panel
MakeDir "${pkgroot}/Applications"
CopyDir "${CONTROL_PANEL_APP_PATH}" "${pkgroot}/Applications/"

# preinstall script
customizeFile "templates/cpl/preinstall" "${pkgscripts}/preinstall"
/bin/chmod 755 "${pkgscripts}/preinstall"

/usr/bin/pkgbuild --version "${versionMajMinSub}" --identifier "${uti}.pkg.${drv}.controlpanel" --scripts "${pkgscripts}" --root "${pkgroot}" "${pkgsdir}/Control Panel.pkg"
status=$?
if [ ${status} -ne 0 ]; then
		echo "error: pkgbuild returned error ${status}"
		exit 1
fi

echo "fixing up PackageInfo"
/usr/sbin/pkgutil --expand "${pkgsdir}/Control Panel.pkg" "${pkgsdir}/Control Panel.pkg--expanded"
/usr/bin/perl -pi -w -e 's/overwrite-permissions="true"/overwrite-permissions="false"/g;' "${pkgsdir}/Control Panel.pkg--expanded/PackageInfo"
# disable relocation support
/usr/bin/perl -pi -w -e 's/<relocate>/<!-- <relocate>/g;' "${pkgsdir}/Control Panel.pkg--expanded/PackageInfo"
/usr/bin/perl -pi -w -e 's/<\/relocate>/<\/relocate> -->/g;' "${pkgsdir}/Control Panel.pkg--expanded/PackageInfo"
mv "${pkgsdir}/Control Panel.pkg" "${pkgsdir}/Control Panel.pkg--OLD"
/usr/sbin/pkgutil --flatten "${pkgsdir}/Control Panel.pkg--expanded" "${pkgsdir}/Control Panel.pkg"
rm -rf "${pkgsdir}/Control Panel.pkg--expanded"

#-------------------------------------------------------------------------------

echo
echo "----------------------------------------"
echo "Creating Audio Driver package..."
echo "----------------------------------------"

pkgroot="${pkgrootdirs}/Audio Driver/root"
pkgscripts="${pkgrootdirs}/Audio Driver/scripts"
MakeDir "${pkgroot}"
MakeDir "${pkgscripts}"

# Kernel Extension
MakeDir "${pkgroot}/System/Library/Extensions"
CopyDir "${binaryDir}/drv/osx/${BUILD_TYPE}/${drv}.kext" "${pkgroot}/System/Library/Extensions/"

# Launch Daemon
MakeDir "${pkgroot}/Library/Application Support/${drv}"
CopyFile "${binaryDir}/drv/osx/${BUILD_TYPE}/${drv}Daemon" "${pkgroot}/Library/Application Support/${drv}/"
MakeDir "${pkgroot}/Library/LaunchDaemons"
CopyFile "${binaryDir}/drv/osx/${BUILD_TYPE}/${uti}.${drv}.plist" "${pkgroot}/Library/LaunchDaemons/"

# postinstall script
customizeFile "templates/audio/postinstall" "${pkgscripts}/postinstall"
/bin/chmod 755 "${pkgscripts}/postinstall"

/usr/bin/pkgbuild --version "${versionMajMinSub}" --identifier "${uti}.pkg.${drv}.audio_driver" --scripts "${pkgscripts}" --root "${pkgroot}" "${pkgsdir}/Audio Driver.pkg"
status=$?
if [ ${status} -ne 0 ]; then
		echo "error: pkgbuild returned error ${status}"
		exit 1
fi

echo "fixing up PackageInfo"
/usr/sbin/pkgutil --expand "${pkgsdir}/Audio Driver.pkg" "${pkgsdir}/Audio Driver.pkg--expanded"
/usr/bin/perl -pi -w -e 's/overwrite-permissions="true"/overwrite-permissions="false"/g;' "${pkgsdir}/Audio Driver.pkg--expanded/PackageInfo"
mv "${pkgsdir}/Audio Driver.pkg" "${pkgsdir}/Audio Driver.pkg--OLD"
/usr/sbin/pkgutil --flatten "${pkgsdir}/Audio Driver.pkg--expanded" "${pkgsdir}/Audio Driver.pkg"
rm -rf "${pkgsdir}/Audio Driver.pkg--expanded"

#-------------------------------------------------------------------------------

echo
echo "----------------------------------------"
echo "Creating MIDI Driver package..."
echo "----------------------------------------"

pkgroot="${pkgrootdirs}/MIDI Driver/root"
MakeDir "${pkgroot}"

# MIDI Driver
MakeDir "${pkgroot}/Library/Audio/MIDI Drivers"
CopyDir "${binaryDir}/drv/osx/${BUILD_TYPE}/${drv}MIDIDriver.plugin" "${pkgroot}/Library/Audio/MIDI Drivers/"

# Audio MIDI Setup icons (optional)
if [ -d "${resourceDir}/AudioMIDISetupIcons" ]; then
	MakeDir "${pkgroot}/Library/Audio/MIDI Devices/${vendorshort}"
	CopyDir "${resourceDir}/AudioMIDISetupIcons" "${pkgroot}/Library/Audio/MIDI Devices/${vendorshort}/Images"
fi

/usr/bin/pkgbuild --version "${versionMajMinSub}" --identifier "${uti}.pkg.${drv}.midi_driver" --root "${pkgroot}" "${pkgsdir}/MIDI Driver.pkg"
status=$?
if [ ${status} -ne 0 ]; then
		echo "error: pkgbuild returned error ${status}"
		exit 1
fi

echo "fixing up PackageInfo"
/usr/sbin/pkgutil --expand "${pkgsdir}/MIDI Driver.pkg" "${pkgsdir}/MIDI Driver.pkg--expanded"
/usr/bin/perl -pi -w -e 's/overwrite-permissions="true"/overwrite-permissions="false"/g;' "${pkgsdir}/MIDI Driver.pkg--expanded/PackageInfo"
mv "${pkgsdir}/MIDI Driver.pkg" "${pkgsdir}/MIDI Driver.pkg--OLD"
/usr/sbin/pkgutil --flatten "${pkgsdir}/MIDI Driver.pkg--expanded" "${pkgsdir}/MIDI Driver.pkg"
rm -rf "${pkgsdir}/MIDI Driver.pkg--expanded"

#-------------------------------------------------------------------------------

echo
echo "----------------------------------------"
echo "Building installer package..."
echo "----------------------------------------"

customizeFile "templates/Distribution" "${build}/Distribution"

MakeDir "${build}/resources/en.lproj"

# Installer Welcome, ReadMe, and License documents
if [ ! -z "${WELCOME_DOC_PATH}" ]; then
	WELCOME_DOC_EXT="${WELCOME_DOC_PATH##*.}"
	CopyFile "${WELCOME_DOC_PATH}" "${build}/resources/en.lproj/welcome.${WELCOME_DOC_EXT}"
	/usr/bin/perl -pi -w -e "s/!-- welcome file --/welcome file=\"welcome.${WELCOME_DOC_EXT}\"\//g;" "${build}/Distribution"
fi
if [ ! -z "${README_DOC_PATH}" ]; then
	README_DOC_EXT="${README_DOC_PATH##*.}"
	CopyFile "${README_DOC_PATH}" "${build}/resources/en.lproj/readme.${README_DOC_EXT}"
	/usr/bin/perl -pi -w -e "s/!-- readme file --/readme file=\"readme.${README_DOC_EXT}\"\//g;" "${build}/Distribution"
fi
if [ ! -z "${LICENSE_DOC_PATH}" ]; then
	LICENSE_DOC_EXT="${LICENSE_DOC_PATH##*.}"
	CopyFile "${LICENSE_DOC_PATH}" "${build}/resources/en.lproj/license.${LICENSE_DOC_EXT}"
	/usr/bin/perl -pi -w -e "s/!-- license file --/license file=\"license.${LICENSE_DOC_EXT}\"\//g;" "${build}/Distribution"
fi

# Installer background image
if [ ! -z "${BACKGROUND_IMAGE_PATH}" ]; then
	CopyFile "${BACKGROUND_IMAGE_PATH}" "${build}/resources/background"
	/usr/bin/perl -pi -w -e 's/!-- background file --/background file="background" scaling="none" alignment="bottomleft"\//g;' "${build}/Distribution"
fi


/usr/bin/productbuild --distribution "${build}/Distribution" --resources "${build}/resources" --package-path "${pkgsdir}" "${diskImageDir}/${installerName}.pkg"
status=$?
if [ ${status} -ne 0 ]; then
		echo "error: productbuild returned error ${status}"
		exit 1
fi

#---------------------------------------

if [ ${DEV_DEBUG} == "yes" ]; then
	MakeDir "${build}/expanded"
	/usr/sbin/pkgutil --expand "${pkgsdir}/Control Panel.pkg" "${build}/expanded/Control Panel"
	/usr/bin/lsbom "${build}/expanded/Control Panel/Bom" > "${build}/expanded/Control Panel/Bom.txt"
	/usr/sbin/pkgutil --expand "${pkgsdir}/Audio Driver.pkg" "${build}/expanded/Audio Driver"
	/usr/bin/lsbom "${build}/expanded/Audio Driver/Bom" > "${build}/expanded/Audio Driver/Bom.txt"
	/usr/sbin/pkgutil --expand "${pkgsdir}/MIDI Driver.pkg" "${build}/expanded/MIDI Driver"
	/usr/bin/lsbom "${build}/expanded/MIDI Driver/Bom" > "${build}/expanded/MIDI Driver/Bom.txt"
	/usr/sbin/pkgutil --expand "${diskImageDir}/${installerName}.pkg" "${build}/expanded/${installerName}"
fi

#-------------------------------------------------------------------------------

if [ ! -z "${DEVELOPER_ID_CERT_NAME}" ]; then
	echo
	echo "----------------------------------------"
	echo "Signing installer package..."
	echo "----------------------------------------"
	mv "${diskImageDir}/${installerName}.pkg" "${diskImageDir}/${installerName}--unsigned.pkg"
	/usr/bin/productsign --sign "Developer ID Installer: ${DEVELOPER_ID_CERT_NAME}" \
		"${diskImageDir}/${installerName}--unsigned.pkg" \
		"${diskImageDir}/${installerName}.pkg"
	status=$?
	if [ ${status} -ne 0 ]; then
			echo "error: productsign returned error ${status}"
			exit 1
	fi
	rm -f "${diskImageDir}/${installerName}--unsigned.pkg"
fi

#-------------------------------------------------------------------------------

echo
echo "----------------------------------------"
echo "Creating disk image..."
echo "----------------------------------------"

# Uninstaller application
if [ "${INCLUDE_UNINSTALLER}" = "yes" ]; then
	CopyDir "${binaryDir}/drv/osx/${BUILD_TYPE}/${drv}Uninstaller.app" "${diskImageDir}/"
fi

# Create the disk image
installDir="${binaryDir}/install/osx/${BUILD_TYPE}"
MakeDir "${installDir}"
dmg="${installDir}/${drv}.dmg"
rm -f "${dmg}"
/usr/bin/hdiutil create -srcdir "${diskImageDir}" -volname "${installerName}" "${dmg}"
status=$?
if [ ${status} -ne 0 ]; then
		echo "error: hdiutil returned error ${status}"
		rm -f "${dmg}"
		exit 1
fi

#-------------------------------------------------------------------------------

echo
echo "----------------------------------------"

# cleanup
if [ ${DEV_DEBUG} == "no" ]; then
	echo "Cleaning up..."
	rm -rf "${build}"
fi

echo "Completed."
exit 0

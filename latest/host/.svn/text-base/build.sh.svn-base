echo "$(awk '/kTCAT_DICE_VENDOR_SHORT\t/{print $3}' ../interface/tcat_dice_myproduct.h): [$(awk '/kTCAT_DICE_VERSION_MAJOR\t/{printf $3"."}/kTCAT_DICE_VERSION_MINOR\t/{printf $3"."}/kTCAT_DICE_VERSION_SUB\t/{printf $3"."}/kTCAT_DICE_VERSION_BUILD\t/{printf $3}' ../interface/tcat_dice_version.h)]"

logFilePath="$(pwd)/build-osx-log.txt"

buildType="Release"
if [ -z ${AUTOBUILDER} ]; then
xcodeCommands="clean build"
else
xcodeCommands="build"
fi


buildProject()
{
	xcodeProjPath="$1"
	xcodeProjDir=$(dirname "${xcodeProjPath}")
	xcodeProjName=$(basename "${xcodeProjPath}")
	if [ -z "$2" ]; then
		xcodeProjTarget="-alltargets"
	else
		xcodeProjTarget="-target $2"
	fi

	pushd "${xcodeProjDir}" >/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "error: unable to cd to the directory \"${xcodeProjDir}\""
		exit 1
	fi

	xcodebuild -project "${xcodeProjName}" ${xcodeProjTarget} -configuration ${buildType} ${xcodeCommands} >"${logFilePath}" 2>&1
	if [ $? -ne 0 ]; then
		echo "error: build failed:"
		cat "${logFilePath}"
		exit 1
	fi

	popd >/dev/null
}


runScript()
{
	scriptPath="$1"
	scriptArgs="$2"
	scriptDir=$(dirname "${scriptPath}")
	scriptName=$(basename "${scriptPath}")

	pushd "${scriptDir}" >/dev/null 2>&1
	if [ $? -ne 0 ]; then
		echo "error: unable to cd to the directory \"${scriptDir}\""
		exit 1
	fi

	./${scriptName} ${scriptArgs} >"${logFilePath}" 2>&1
	if [ $? -ne 0 ]; then
		echo "error: build failed:"
		cat "${logFilePath}"
		exit 1
	fi

	popd >/dev/null
}



if [ -f "TDL/build.sh" ]; then
	echo "TDL"
	runScript "TDL/build.sh"
fi

if [ -d "drv" ]; then
	echo "drv/osx"
	buildProject "drv/osx/TcatDice.xcodeproj"
fi

echo "pal"
buildProject "pal/tcat_dice_pal.xcodeproj"

echo "tools/cpl"
buildProject "tools/cpl/cpl.xcodeproj"

echo "tools/dice"
buildProject "tools/dice/dice.xcodeproj" "dice"

echo "tools/inspector"
buildProject "tools/inspector/inspector.xcodeproj"

# disabled until Case 226 is resolved
#echo "tools/prefPane"
#buildProject "tools/prefPane/prefPane.xcodeproj"

echo "uninstaller"
buildProject "install/osx/uninstaller/DiceUninstaller.xcodeproj"

echo "installer"
runScript "install/osx/build-installer.sh" "-b ${buildType} -w ../../../docs/public/drv/tcat_dice_drv_osx_release_notes.html"


rm -f "${logFilePath}"

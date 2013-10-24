@echo off

echo %0 
echo Project directory: %1 
echo Project name: %2
echo.

if "%1"=="" goto usage 
if "%2"=="" goto usage 

echo Launching Insight debugger

if not defined CYGWIN_PATH ( 
	echo ************************************************************
	echo CYGWIN_PATH environment variable not found
	echo default to "c:\cygwin"
	echo ************************************************************
	set CYGWIN_PATH=c:\cygwin
)


rem arg 1 is the path to the project make directory, and $(ProjDir) is passed in from VC8 external tool
rem remove quotes
for %%v in (%1) do set PROJ_DIR=%%~v

rem arg 2 is the project name, and $(TargetName) is passed in from VC8 external tool
rem remove quotes
for %%v in (%2) do set TARGET_NAME=%%~v

set DEBUG_TARGET=%TARGET_NAME%Debug

echo loading debug image: %PROJ_DIR%..\bin\%DEBUG_TARGET%
if exist %PROJ_DIR%..\bin\%DEBUG_TARGET% (
	%CYGWIN_PATH%\bin\bash.exe -login -c "BIN_DIR=`eval cygpath -a '${PROJ_DIR}../bin' `; cd ${BIN_DIR}; arm-elf-insight.exe ${DEBUG_TARGET}" 
) else (
	echo ************************************************************
	echo Debug image not found. Check batch file arguments, or build 
	echo the executable and try again.
	echo ************************************************************
)
goto end

:usage
	echo ************************************************************
	echo For VS8 External Tool, call the batch file with:
	echo Arguments: $(ProjectDir) $(TargetName)
	echo Initial directory: $(ProjectDir)
	echo ************************************************************

:end

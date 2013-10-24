@echo off
::==============================================================================
::
:: This confidential and proprietary software may be used only as authorised
:: by a licensing agreement from TC Applied Technologies Ltd.
::
:: (C) COPYRIGHT 2007 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
::
:: Unauthorized use, duplication or distribution of this software is  
:: strictly prohibited by law.
::
:: The entire notice above must be reproduced on all authorised copies and
:: copies may only be made to the extent permitted by a licensing agreement
:: from TC Applied Technologies Ltd.
::
::==============================================================================

if "%AUTOBUILDER%" == "" goto NONAUTOBUILDER
set EXIT_SCRIPT=
set DEVENV_ACTION=/build
goto ABCHECKDONE
:NONAUTOBUILDER
set EXIT_SCRIPT=/b
set DEVENV_ACTION=/rebuild
:ABCHECKDONE


echo installer

..\..\scripts\customize.py --buildtype Release generic.iss ..\..\..\interface\tcat_dice_defaults.h custom.iss >build.log 2>&1
if not %ERRORLEVEL%==0 (
	type build.log
	exit %EXIT_SCRIPT% 1
)

iscc.exe custom.iss >>build.log 2>&1
if not %ERRORLEVEL%==0 (
	type build.log
	exit %EXIT_SCRIPT% 1
)


python ..\..\scripts\sign.py ..\..\..\binary\install\win\release\*Installer.exe >>build.log 2>&1
if not %ERRORLEVEL%==0 (
	type build.log
	exit %EXIT_SCRIPT% 1
)

@echo off

if "%AUTOBUILDER%" == "" goto NONAUTOBUILDER
set EXIT_SCRIPT=
set DEVENV_ACTION=/build
goto ABCHECKDONE
:NONAUTOBUILDER
set EXIT_SCRIPT=/b
set DEVENV_ACTION=/rebuild
:ABCHECKDONE



echo dice/32
devenv dice.sln %DEVENV_ACTION% "Release|Win32" >build.win32.log
if not %ERRORLEVEL%==0 (
	type build.win32.log
	exit %EXIT_SCRIPT% 1
)

echo dice/64
devenv dice.sln %DEVENV_ACTION% "Release|x64" >build.x64.log
if not %ERRORLEVEL%==0 (
	type build.x64.log
	exit %EXIT_SCRIPT% 1
)
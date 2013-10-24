@echo off
rem make sure to have a ~/.bash_profile: executed by bash for login shells.
rem make sure the CYGWIN_PATH Windows (system) environment
rem	variable is set to your cygwin installation directory,
rem	i.e. "c:\cygwin"
rem or 
rem set the path here, i.e. uncomment out the following line and edit the path
rem set CYGWIN_PATH=c:\cygwin
if not defined CYGWIN_PATH ( 
	echo ************************************************************
	echo CYGWIN_PATH environment variable not found
	echo default to "c:\cygwin"
	echo ************************************************************
	set CYGWIN_PATH=c:\cygwin
)

set TC_DIR=%2..\..\..
echo tc_dir %TC_DIR%
@if "%1"=="all" goto all
@if "%1"=="rebuild" goto rebuild
@if "%1"=="clean" goto clean

echo unknown make target
goto end

:all
echo make all
%CYGWIN_PATH%\bin\bash.exe -login -c "cd ${TC_DIR}/project/cms_tcd22x0/make;make -s dep AWK_PIPE=1"
@goto end

:rebuild
echo rebuild all
%CYGWIN_PATH%\bin\bash.exe -login -c "cd ${TC_DIR}/project/cms_tcd22x0/make;make -s clean AWK_PIPE=1;make install AWK_PIPE=1"
@goto end

:clean
echo make clean
%CYGWIN_PATH%\bin\bash.exe -login -c "cd ${TC_DIR}/project/cms_tcd22x0/make;make clean"
@goto end
:end
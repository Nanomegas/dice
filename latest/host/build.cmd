@echo off

if "%AUTOBUILDER%" == "" goto NONAUTOBUILDER

taskkill /im mspdbsrv.exe /f >nul 2>&1

type NUL > ..\win.log

if not exist TDL\build.cmd goto BUILD_TDL_END
cd TDL
call build.cmd >>..\..\win.log
cd ..
:BUILD_TDL_END

if not exist drv\win\NUL goto BUILD_DRV_END
cd drv\win
call build.cmd >>..\..\..\win.log
cd ..\..
:BUILD_DRV_END

cd tools

cd cpl
call build.cmd >>..\..\..\win.log
cd ..

cd dice
call build.cmd >>..\..\..\win.log
cd ..

cd inspector
call build.cmd >>..\..\..\win.log
cd ..

cd ..

cd install\win
call build.cmd >>..\..\..\win.log
cd ..\..

goto ABCHECKDONE



:NONAUTOBUILDER

if not exist TDL\build.cmd goto BUILD_TDL_END
cd TDL
call build.cmd
cd ..
:BUILD_TDL_END

if not exist drv\win\NUL goto BUILD_DRV_END
cd drv\win
call build.cmd
cd ..\..
:BUILD_DRV_END

cd tools

cd cpl
call build.cmd
cd ..

cd dice
call build.cmd
cd ..

cd inspector
call build.cmd
cd ..

cd ..

cd install\win
call build.cmd
cd ..\..


:ABCHECKDONE

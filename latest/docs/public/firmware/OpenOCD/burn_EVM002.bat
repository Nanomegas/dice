@echo off
rem burn_EVM002.bat � program EVM002 flash specifying JTAG interface and DICE configuration
rem OpenOCD version 0.4.0 is assumed
rem if you have as different jtag device, specify the corresponding cfg file

"C:\Program Files\OpenOCD\0.4.0\bin\openocd.exe" ^
-f "C:\Program Files\OpenOCD\0.4.0\interface\jtagkey.cfg" ^
-f dice_oocd_flash_EVM002.cfg

pause

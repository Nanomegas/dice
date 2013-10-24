Complete flash image files.

The files in this directory are 2MB images containing a RedBoot boot loader, initialized flash file system, setup and persistent storage files.

completeFlashImage_redboot.bin can be used with EVM002, EVM003 or EVM004.

Some images also include DICE firmware applications. 

Flash file descriptions:

RedBoot: 
	The boot loader. Always included.

RedBoot config: 
	Always included.

FIS directory: 
	Flash file table. Always included.

setup: 
	Contains the device serial number. In this case a serial number of 0x3fffff is used. Your production device will each have a unique valid serial number here. Always included. Always keep this file before the application file.

evm_sps: 
	Persistent storage for dice application. Always keep this file before the application file. If this file is missing, the dice application will create it.

eap_vol: 
	Persistent storage for dice applications which have EAP functionality. Always keep this file before the application file. If this file is missing, the EAP-based application will create it.

dice:
	Dice firmware application.


The file systems in each image are shown below.


* completeFlashImage_redboot.bin
Name              FLASH addr  Mem addr    Length      Entry point
RedBoot           0x04000000  0x04000000  0x00020000  0x00000000
RedBoot config    0x041E0000  0x041E0000  0x00001000  0x00000000
FIS directory     0x041F0000  0x041F0000  0x00010000  0x00000000
setup             0x04020000  0x00030000  0x00010000  0x00030040
evm_sps           0x04030000  0x00000000  0x00020000  0x00000000
eap_vol           0x04050000  0x00000000  0x00020000  0x00000000


* completeFlashImage_EVM002.bin
Name              FLASH addr  Mem addr    Length      Entry point
RedBoot           0x04000000  0x04000000  0x00020000  0x00000000
RedBoot config    0x041E0000  0x041E0000  0x00001000  0x00000000
FIS directory     0x041F0000  0x041F0000  0x00010000  0x00000000
setup             0x04020000  0x00030000  0x00010000  0x00030040
evm_sps           0x04030000  0x00000000  0x00020000  0x00000000
eap_vol           0x04050000  0x00000000  0x00020000  0x00000000
dice              0x04070000  0x00030000  0x00060000  0x00030040

* completeFlashImage_EVM003.bin
Name              FLASH addr  Mem addr    Length      Entry point
RedBoot           0x04000000  0x04000000  0x00020000  0x00000000
RedBoot config    0x041E0000  0x041E0000  0x00001000  0x00000000
FIS directory     0x041F0000  0x041F0000  0x00010000  0x00000000
setup             0x04020000  0x00030000  0x00010000  0x00030040
am_sps            0x04030000  0x00000000  0x00020000  0x00000000
eap_vol           0x04050000  0x00000000  0x00020000  0x00000000
dice              0x04070000  0x00030000  0x00060000  0x00030040

* completeFlashImage_EVM004.bin
Name              FLASH addr  Mem addr    Length      Entry point
RedBoot           0x04000000  0x04000000  0x00020000  0x00000000
RedBoot config    0x041E0000  0x041E0000  0x00001000  0x00000000
FIS directory     0x041F0000  0x041F0000  0x00010000  0x00000000
setup             0x04020000  0x00030000  0x00010000  0x00030040
am_sps            0x04030000  0x00000000  0x00020000  0x00000000
eap_vol           0x04050000  0x00000000  0x00020000  0x00000000
dice              0x04070000  0x00030000  0x00050000  0x00030040

When the board is reset, it automatically runs the dice application.


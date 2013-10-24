#!/bin/sh

# prune the ecos distribution for DICE use
#
# original size from svn is 151MB, ~15250 files
# new size after pruning is 84MB, ~8000 files
#
# to be used in /firmware/os/ecos/src/packages
#
# the resulting database file is /firmware/os/ecos/src/packages/ecos.db
#
# the list of original packages was found using 
#  $ tclsh ecosadmin.tcl list

# remove the doc folder (large size and number of files)
rm -rf ../doc

# some source directories are already missing in the distro
#  remove the references to them from the database file
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH7750_DREAMCAST
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH2
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH3
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH4
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_EDK7708
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH7708_CQ7708
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH7750_CQ7750
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH7729_HS7729PCI
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH77X9_SE77X9
tclsh ecosadmin.tcl remove CYGPKG_HAL_SH_SH7751_SE7751
tclsh ecosadmin.tcl remove CYGPKG_HAL_H8300
tclsh ecosadmin.tcl remove CYGPKG_HAL_H8300_H8300H
tclsh ecosadmin.tcl remove CYGPKG_HAL_H8300_H8300H_SIM
tclsh ecosadmin.tcl remove CYGPKG_HAL_H8300_H8300H_AKI3068NET
tclsh ecosadmin.tcl remove CYGPKG_HAL_I386
tclsh ecosadmin.tcl remove CYGPKG_HAL_I386_GENERIC
tclsh ecosadmin.tcl remove CYGPKG_HAL_I386_PCMB
tclsh ecosadmin.tcl remove CYGPKG_HAL_I386_PC
tclsh ecosadmin.tcl remove CYGPKG_HAL_SYNTH
tclsh ecosadmin.tcl remove CYGPKG_HAL_SYNTH_I386
tclsh ecosadmin.tcl remove CYGPKG_HAL_SPARCLITE
tclsh ecosadmin.tcl remove CYGPKG_HAL_SPARCLITE_SLEB
tclsh ecosadmin.tcl remove CYGPKG_HAL_SPARCLITE_SIM
tclsh ecosadmin.tcl remove CYGPKG_HAL_SPARC
tclsh ecosadmin.tcl remove CYGPKG_HAL_SPARC_ERC32
tclsh ecosadmin.tcl remove CYGPKG_HAL_SPARC_LEON
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_MPC8260
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_MPC8xx
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_PPC60x
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_MPC5xx
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_PPC40x
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_COGENT
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_CME555
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_EC555
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_FADS
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_MBX
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_VIPER
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_ADDER
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_TS6
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_VADS
tclsh ecosadmin.tcl remove CYGPKG_HAL_QUICC
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_SIM
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_SIM
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_TX39
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_TX39_JMR3904
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_MIPS32
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_MIPS64
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_ATLAS
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_MALTA
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_TX49
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_TX49_REF4955
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_RM7000
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_RM7000_OCELOT
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_VR4300
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_VR4300_VRC4373
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_VR4300_VRC4375
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_VR4300_VRC437X
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_UPD985XX
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_IDT32334
tclsh ecosadmin.tcl remove CYGPKG_HAL_MIPS_IDT32334_REFIDT334
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300_AM31
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300_AM31_SIM
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300_AM31_STDEVAL1
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300_AM33
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300_AM33_STB
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300_AM33_ASB
tclsh ecosadmin.tcl remove CYGPKG_HAL_MN10300_AM33_ASB2305
tclsh ecosadmin.tcl remove CYGPKG_HAL_V85X
tclsh ecosadmin.tcl remove CYGPKG_HAL_V85X_V850
tclsh ecosadmin.tcl remove CYGPKG_HAL_V85X_V850_CEB
tclsh ecosadmin.tcl remove CYGPKG_HAL_M68K
tclsh ecosadmin.tcl remove CYGPKG_HAL_M68K_MCF52xx
tclsh ecosadmin.tcl remove CYGPKG_HAL_M68K_MCF52xx_MCF5272
tclsh ecosadmin.tcl remove CYGPKG_HAL_M68K_MCF52xx_MCF5272_MCF5272C3
tclsh ecosadmin.tcl remove CYGPKG_HAL_CALM16
tclsh ecosadmin.tcl remove CYGPKG_HAL_CALM16_CORE
tclsh ecosadmin.tcl remove CYGPKG_HAL_CALM16_CEB
tclsh ecosadmin.tcl remove CYGPKG_HAL_CALM32
tclsh ecosadmin.tcl remove CYGPKG_HAL_CALM32_CORE
tclsh ecosadmin.tcl remove CYGPKG_HAL_CALM32_CEB
tclsh ecosadmin.tcl remove CYGPKG_HAL_FRV
tclsh ecosadmin.tcl remove CYGPKG_HAL_FRV_FRV400
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_CSB281
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_TS1000

# remove unused directories and database entries

# CYGPKG_HAL: common
# CYGPKG_INFRA: infra
# CYGPKG_IO: common
# CYGPKG_IO_SERIAL: serial
tclsh ecosadmin.tcl remove CYGPKG_IO_PCMCIA #: pcmcia
tclsh ecosadmin.tcl remove CYGPKG_DEVS_PCMCIA_ASSABET #: assabet
tclsh ecosadmin.tcl remove CYGPKG_DEVS_PCMCIA_CERF #: cerf
tclsh ecosadmin.tcl remove CYGPKG_DEVS_PCMCIA_CERFPDA #: cerfpda
tclsh ecosadmin.tcl remove CYGPKG_DEVS_PCMCIA_IPAQ #: ipaq
# CYGPKG_IO_FLASH: flash
# CYGPKG_DEVS_FLASH_INTEL_BOOTBLOCK: bootblock
# CYGPKG_DEVS_FLASH_ARM_AAED2000: aaed2000
# CYGPKG_DEVS_FLASH_AMD_AM29XXXXX: am29xxxxx
# CYGPKG_DEVS_FLASH_ATMEL_AT29CXXXX: at29cxxxx
# CYGPKG_DEVS_FLASH_ARM_PID: pid
# CYGPKG_DEVS_FLASH_SH_EDK7708: edk7708
# CYGPKG_DEVS_FLASH_CQ7750: cq7750
# CYGPKG_DEVS_FLASH_MIPS_OCELOT: ocelot
# CYGPKG_DEVS_FLASH_MIPS_VRC437X: vrc437x
# CYGPKG_DEVS_FLASH_MIPS_REFIDT334: idt79s334a
# CYGPKG_DEVS_FLASH_SH_HS7729PCI: hs7729pci
# CYGPKG_DEVS_FLASH_SH_SE77X9: se77x9
# CYGPKG_DEVS_FLASH_SH_SE7751: se7751
# CYGPKG_DEVS_FLASH_EBSA285: ebsa285
# CYGPKG_DEVS_FLASH_SA1100MM: sa1100mm
# CYGPKG_DEVS_FLASH_EDB7XXX: edb7xxx
# CYGPKG_DEVS_FLASH_AT91: at91
# CYGPKG_DEVS_FLASH_ARM_NS50: ns50
# CYGPKG_DEVS_FLASH_ARM_DICE: dice
# CYGPKG_DEVS_FLASH_ARM_DICEJR: diceJr
# CYGPKG_DEVS_FLASH_ASSABET: assabet
# CYGPKG_DEVS_FLASH_CERF: cerf
# CYGPKG_DEVS_FLASH_CERFPDA: cerfpda
# CYGPKG_DEVS_FLASH_IPAQ: ipaq
# CYGPKG_DEVS_FLASH_FLEXANET: flexanet
# CYGPKG_DEVS_FLASH_MPC50: mpc50
# CYGPKG_DEVS_FLASH_MALTA: malta
# CYGPKG_DEVS_FLASH_ARM_EXCALIBUR: excalibur
# CYGPKG_DEVS_FLASH_ARM_INNOVATOR: innovator
# CYGPKG_DEVS_FLASH_NANO: nano
# CYGPKG_DEVS_FLASH_STRATA: strata
# CYGPKG_DEVS_FLASH_INTEL_28FXXX: 28fxxx
# CYGPKG_DEVS_FLASH_ARM_XSCALE_IQ80310: iq80310
# CYGPKG_DEVS_FLASH_IQ80321: iq80321
# CYGPKG_DEVS_FLASH_ARM_INTEGRATOR: integrator
# CYGPKG_DEVS_FLASH_MBX: mbx
# CYGPKG_DEVS_FLASH_TS6: ts6
# CYGPKG_DEVS_FLASH_VADS: vads
# CYGPKG_DEVS_FLASH_CME555: cme555
# CYGPKG_DEVS_FLASH_EC555: ec555
# CYGPKG_DEVS_FLASH_VIPER: viper
# CYGPKG_DEVS_FLASH_POWERPC_ADDER: adder
# CYGPKG_DEVS_FLASH_MN10300_STB: stb
# CYGPKG_DEVS_FLASH_MN10300_ASB2303: asb2303
# CYGPKG_DEVS_FLASH_MN10300_ASB2305: asb2305
# CYGPKG_DEVS_FLASH_ATLAS: atlas
# CYGPKG_DEVS_FLASH_SYNTH: synth
# CYGPKG_DEVS_FLASH_FRV_FRV400: frv400
tclsh ecosadmin.tcl remove CYGPKG_DEVS_TOUCH_IPAQm #: ipaq
tclsh ecosadmin.tcl remove CYGPKG_DEVS_KBD_IPAQ #: ipaq
tclsh ecosadmin.tcl remove CYGPKG_DEVS_TOUCH_AAED2000 #: aaed2000
tclsh ecosadmin.tcl remove CYGPKG_DEVS_KBD_AAED2000 #: aaed2000
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_EBSA285 #: ebsa285
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_XSCALE_IOP310 #: iop310
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_IQ80321 #: iq80321
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_SA11X0 #: sa11x0
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_PID #: pid
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_CERFPDA #: cerfpda
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_AEB #: aeb
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_AAED2000 #: aaed2000
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_EDB7XXX #: edb7xxx
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_CMA230 #: cma230
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_E7T #: e7t
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_INTEGRATOR #: integrator
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_AT91 #: at91
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_ARM_NS50 #: ns50
# CYGPKG_IO_SERIAL_ARM_DICE: dice
# CYGPKG_IO_SERIAL_ARM_DICEJR: diceJr
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_POWERPC_COGENT #: cogent
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_POWERPC_EC555 #: ec555
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_POWERPC_CME555 #: cme555
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_POWERPC_QUICC_SMC #: quicc
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_POWERPC_QUICC2_SCC #: quicc2
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_SPARCLITE_SLEB #: sleb
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_TX39_JMR3904 #: jmr3904
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_MIPS_ATLAS #: atlas
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_MIPS_REF4955 #: ref4955
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_MIPS_VRC437X #: vrc437x
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_MN10300 #: mn10300
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_I386_PC #: pc
# CYGPKG_IO_SERIAL_GENERIC_16X5X: 16x5x
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_SH_SCI #: sci
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_SH_SCIF #: scif
tclsh ecosadmin.tcl remove CYGPKG_DEVS_ETH_ARM_CERF #: cerf
tclsh ecosadmin.tcl remove CYGPKG_DEVS_ETH_ARM_CERFPDA #: cerfpda
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_SH_EDK7708 #: edk7708
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_SH_CQ7708 #: cq7708
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_SH_SE77X9 #: se77x9
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_V85X_V850 #: v850
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_MIPS_UPD985XX #: upd985xx
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_MIPS_IDT79S334A #: idt79s334a
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_H8300_SCI #: h8300h
tclsh ecosadmin.tcl remove CYGPKG_IO_SERIAL_MCF5272_UART #: mcf5272
# CYGPKG_IO_SERIAL_LOOP: loop
# CYGPKG_IO_FILEIO: fileio
# CYGPKG_FS_RAM: ram
# CYGPKG_FS_ROM: rom
# CYGPKG_FS_JFFS2: jffs2
# CYGPKG_LINUX_COMPAT: linux
# CYGPKG_IO_ETH_DRIVERS: eth
# CYGPKG_IO_USB: common
# CYGPKG_IO_USB_SLAVE: slave
# CYGPKG_IO_USB_SLAVE_ETH: slave
tclsh ecosadmin.tcl remove CYGPKG_DEVS_USB_SA11X0 #: sa11x0
tclsh ecosadmin.tcl remove CYGPKG_DEVS_USB_UPD985XX #: nec_upd985xx
# CYGPKG_NET: common
# CYGPKG_NET_OPENBSD_STACK: tcpip
# CYGPKG_NET_FREEBSD_STACK: bsd_tcpip
# CYGPKG_NET_FTPCLIENT: ftpclient
# CYGPKG_SNMPLIB: lib
# CYGPKG_SNMPAGENT: agent
# CYGPKG_HTTPD: httpd
# CYGPKG_NS_DNS: dns
# CYGPKG_DEVS_ETH_ARM_EDB7XXX: edb7xxx
# CYGPKG_DEVS_ETH_ARM_KS32C5000: ks32c5000
# CYGPKG_DEVS_ETH_ARM_AAED2000: aaed2000
# CYGPKG_DEVS_ETH_AMD_PCNET: pcnet
# CYGPKG_DEVS_ETH_MIPS_MIPS32_MALTA: malta
# CYGPKG_DEVS_ETH_MN10300_ASB2305: asb2305
# CYGPKG_DEVS_ETH_VIA_RHINE: rhine
# CYGPKG_DEVS_ETH_SH_HS7729PCI: hs7729pci
# CYGPKG_DEVS_ETH_SH_SE77X9: se77x9
# CYGPKG_DEVS_ETH_SH_SE7751: se7751
# CYGPKG_DEVS_ETH_SH_ETHERC: etherc
# CYGPKG_DEVS_ETH_ARM_EBSA285: ebsa285
# CYGPKG_DEVS_ETH_ARM_FLEXANET: flexanet
# CYGPKG_DEVS_ETH_ARM_INNOVATOR: innovator
# CYGPKG_DEVS_ETH_ARM_INTEGRATOR: integrator
# CYGPKG_DEVS_ETH_POWERPC_QUICC: quicc
# CYGPKG_DEVS_ETH_POWERPC_MBX: mbx
# CYGPKG_DEVS_ETH_POWERPC_QUICC2: quicc2
# CYGPKG_DEVS_ETH_POWERPC_FEC: fec
# CYGPKG_DEVS_ETH_POWERPC_VIPER: viper
# CYGPKG_DEVS_ETH_POWERPC_ADDER: adder
# CYGPKG_DEVS_ETH_INTEL_I82559: i82559
# CYGPKG_DEVS_ETH_INTEL_I82544: i82544
# CYGPKG_DEVS_ETH_INTEL_I21143: i21143
# CYGPKG_DEVS_ETH_MIPS_VRC4375: vrc4375
# CYGPKG_DEVS_ETH_MIPS_RM7000_OCELOT: ocelot
# CYGPKG_DEVS_ETH_ARM_IQ80321: iq80321
# CYGPKG_DEVS_ETH_ARM_XSCALE_IQ80310: iq80310
# CYGPKG_DEVS_ETH_I386_PC_I82559: i82559
# CYGPKG_DEVS_ETH_I386_PC_I82544: i82544
# CYGPKG_DEVS_ETH_MIPS_ATLAS: atlas
# CYGPKG_DEVS_ETH_NS_DP83902A: dp83902a
# CYGPKG_DEVS_ETH_CL_CS8900A: cs8900a
# CYGPKG_DEVS_ETH_MIPS_UPD985XX: upd985xx
# CYGPKG_DEVS_ETH_MIPS_REFIDT334: idt79s334a
# CYGPKG_DEVS_ETH_CF: cf
# CYGPKG_DEVS_ETH_H8300_AKI3068NET: aki3068net
# CYGPKG_DEVS_ETH_MCF5272: mcf5272
# CYGPKG_DEVS_ETH_ECOSYNTH: ecosynth
tclsh ecosadmin.tcl remove CYGPKG_IO_PCI #: pci
# CYGPKG_IO_WALLCLOCK: wallclock
tclsh ecosadmin.tcl remove CYGPKG_DEVICES_WALLCLOCK_SH3 #: sh3
tclsh ecosadmin.tcl remove CYGPKG_DEVICES_WALLCLOCK_MPC5xx #: mpc5xx
tclsh ecosadmin.tcl remove CYGPKG_DEVICES_WALLCLOCK_DALLAS_DS1742 #: ds1742
tclsh ecosadmin.tcl remove CYGPKG_DEVICES_WALLCLOCK_DALLAS_DS12887 #: ds12887
tclsh ecosadmin.tcl remove CYGPKG_DEVICES_WALLCLOCK_MIPS_REF4955 #: ref4955
tclsh ecosadmin.tcl remove CYGPKG_DEVICES_WALLCLOCK_I386_PC #: pc
tclsh ecosadmin.tcl remove CYGPKG_DEVICES_WALLCLOCK_SH_HS7729PCI #: hs7729pci
# CYGPKG_DEVICES_WATCHDOG_ARM_AEB: aeb
# CYGPKG_DEVICES_WATCHDOG_ARM_AT91: at91
# CYGPKG_DEVICES_WATCHDOG_ARM_EBSA285: ebsa285
# CYGPKG_DEVICES_WATCHDOG_ARM_SA11X0: sa11x0
# CYGPKG_DEVICES_WATCHDOG_MN10300_MN10300: mn10300
# CYGPKG_DEVICES_WATCHDOG_SH_SH3: sh3
# CYGPKG_DEVICES_WATCHDOG_H8300_H8300H: h8300h
# CYGPKG_DEVS_WATCHDOG_SYNTH: synth
# CYGPKG_IO_WATCHDOG: watchdog
# CYGPKG_KERNEL: kernel
# CYGPKG_MEMALLOC: common
# CYGPKG_LOADER: loader
# CYGPKG_COMPRESS_ZLIB: zlib
# CYGPKG_POWER: common
# CYGPKG_CRC: crc
# CYGPKG_CPULOAD: cpuload
# CYGPKG_PROFILE_GPROF: gprof
# CYGPKG_UITRON: uitron
# CYGPKG_ISOINFRA: isoinfra
# CYGPKG_POSIX: posix
# CYGPKG_LIBC: common
# CYGPKG_LIBC_I18N: i18n
# CYGPKG_LIBC_SETJMP: setjmp
# CYGPKG_LIBC_SIGNALS: signals
# CYGPKG_LIBC_STARTUP: startup
# CYGPKG_LIBC_STDIO: stdio
# CYGPKG_LIBC_STDLIB: stdlib
# CYGPKG_LIBC_STRING: string
# CYGPKG_LIBC_TIME: time
# CYGPKG_LIBM: libm
# CYGPKG_ERROR: error
# CYGPKG_CYGMON: cygmon
# CYGPKG_REDBOOT: redboot
tclsh ecosadmin.tcl remove CYGPKG_MICROWINDOWS #: mw
# CYGPKG_HAL_ARM: arch
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_PID #: pid
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_AEB #: aeb
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_E7T #: e7t
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_INTEGRATOR #: integrator
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_AT91 #: at91
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_NS50 #: ns50
# CYGPKG_HAL_ARM_DICE: dice
# CYGPKG_HAL_ARM_DICEJR: diceJr
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_EBSA285 #: ebsa285
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_ARM9 #: var
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_ARM9_AAED2000 #: aaed2000
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_ARM9_EXCALIBUR #: excalibur
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_ARM9_INNOVATOR #: innovator
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0 #: var
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_ASSABET #: assabet
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_CERF #: cerf
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_CERFPDA #: cerfpda
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_IPAQ #: ipaq
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_FLEXANET #: flexanet
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SNDS #: snds
# CYGPKG_DEVS_ETH_SMSC_LAN91CXX: lan91cxx
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_NANO #: nano
tclsh ecosadmin.tcl remove CYGPKG_DEVS_ETH_ARM_NANO #: nano
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_BRUTUS #: brutus
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_SA11X0_SA1100MM #: sa1100mm
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_EDB7XXX #: edb7xxx
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_CMA230 #: cma230
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_CORE #: cores
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_IOP310 #: iop310
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_IQ80310 #: iq80310
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_NPWR #: npwr
tclsh ecosadmin.tcl remove CYGPKG_DEVS_ETH_ARM_XSCALE_NPWR #: npwr
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_VERDE #: verde
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_IQ80321 #: iq80321
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_PXA2X0 #: pxa2x0
tclsh ecosadmin.tcl remove CYGPKG_HAL_ARM_XSCALE_MPC50 #: mpc50 
tclsh ecosadmin.tcl remove CYGPKG_DEVS_ETH_FRV_FRV400 #: frv400
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_CSB281 #: 
tclsh ecosadmin.tcl remove CYGPKG_DEVS_FLASH_CSB281 #: csb281
tclsh ecosadmin.tcl remove CYGPKG_DEVS_ETH_CSB281 #: csb281
tclsh ecosadmin.tcl remove CYGPKG_HAL_POWERPC_TS1000 #: ts1000
tclsh ecosadmin.tcl remove CYGPKG_DEVS_FLASH_POWERPC_TS1000 #: ts1000
tclsh ecosadmin.tcl remove CYGPKG_DEVS_ETH_POWERPC_TS1000 #: ts1000
# CYGPKG_IO_FLASH_NONBLOCK: flash_nonblock

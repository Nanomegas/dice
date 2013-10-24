##===========================================================================
##
## This confidential and proprietary software may be used only as authorised
## by a licensing agreement from TC Applied Technologies Ltd.
##
## (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
##
## Unauthorized use, duplication or distribution of this software is  
## strictly prohibited by law.
##
## The entire notice above must be reproduced on all authorised copies and
## copies may only be made to the extent permitted by a licensing agreement
## from TC Applied Technologies Ltd.
##
##===========================================================================
## $Id: Makefile,v 1.2 2007/02/14 19:19:48 zluo Exp $
##===========================================================================

# Platform specific setups
include Make.params

all: 
	@echo making target: $(APP_FILE)
	make allall

allall: 
	$(MAKE) -C ${TCKERNEL_DIR}/src
	$(MAKE) -C ${COMMON_DIR}/misc/src

ifeq (frmwload, $(findstring frmwload, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/frmwload/src
endif

ifeq (cli, $(findstring cli, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/cli/src
endif

ifeq (1394lal, $(findstring 1394lal, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/1394lal/src 
endif

ifeq (fcp, $(findstring fcp, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/fcp/src
endif

ifeq (1394avc, $(findstring 1394avc, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/1394avc/src
endif

ifeq (crb, $(findstring crb, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/crb/src
endif

ifeq (fis, $(findstring fis, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/fis/src
endif

ifeq (dicedriver, $(findstring dicedriver, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/dicedriver/src
endif

ifeq (dal, $(findstring dal, $(MODULE)))
	$(MAKE) -C ${CHIP_DIR}/dal/src
endif

ifeq (spi, $(findstring spi, $(MODULE)))
	$(MAKE) -C ${CHIP_DIR}/spi/src
endif

ifeq (gray, $(findstring gray, $(MODULE)))
	$(MAKE) -C ${CHIP_DIR}/gray/src
endif

ifeq (dice, $(findstring dice, $(MODULE)))
	$(MAKE) -C ${CHIP_DIR}/dice/src
endif

ifeq (avs, $(findstring avs, $(MODULE)))
	$(MAKE) -C ${CHIP_DIR}/avs/src
endif

ifeq (1394llc, $(findstring 1394llc, $(MODULE)))
	$(MAKE) -C ${CHIP_DIR}/llc/src
endif

ifeq (i2c, $(findstring i2c, $(MODULE)))
	$(MAKE) -C ${CHIP_DIR}/i2c/src
endif

ifeq (target, $(findstring target, $(MODULE)))
	$(MAKE) -C ${PROJECT_DIR}/target/src
endif

ifeq (aml, $(findstring aml, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/aml/src
endif

ifeq (sps, $(findstring sps, $(MODULE)))
	$(MAKE) -C ${COMMON_DIR}/sps/src
endif

	$(MAKE) -C ${PROJECT_DIR}/test/src

	$(MAKE) -C ${PROJECT_DIR}/main	

rebuild:
	@echo rebuilding target: $(APP_FILE)
	make clean
	make dep
	make allall

dep:
	mkdir -p ${PROJECT_DIR}/lib
	mkdir -p ${PROJECT_DIR}/bin
	mkdir -p ${PROJECT_DIR}/obj
	cp ${ECOS_KERNEL_DIR}/lib/target.ld  ${LIB_DIR}/mytarget.ld
	sed  -e '/GROUP/{s/libsupc++.a)/libsupc++.a libtcat.a)/g}' ${LIB_DIR}/mytarget.ld > tmp
	mv tmp  ${LIB_DIR}/mytarget.ld

ifeq (frmwload, $(findstring frmwload, $(MODULE)))
	sed  -e '/GROUP/{s/libfrmwload.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libfrmwload.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (cli, $(findstring cli, $(MODULE)))
	sed  -e '/GROUP/{s/libcli.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libcli.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (1394lal, $(findstring 1394lal, $(MODULE)))
	sed  -e '/GROUP/{s/lib1394lal.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a lib1394lal.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (fcp, $(findstring fcp, $(MODULE)))
	sed  -e '/GROUP/{s/libfcp.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libfcp.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (1394avc, $(findstring 1394avc, $(MODULE)))
	sed  -e '/GROUP/{s/lib1394avc.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a lib1394avc.a/}' tmp > ${LIB_DIR}/mytarget.ld
	rm tmp
endif

ifeq (crb, $(findstring crb, $(MODULE)))
	sed  -e '/GROUP/{s/libcrb.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libcrb.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (fis, $(findstring fis, $(MODULE)))
	sed  -e '/GROUP/{s/libfis.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libfis.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (dicedriver, $(findstring dicedriver, $(MODULE)))
	sed  -e '/GROUP/{s/libdicedriver.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libdicedriver.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (dal, $(findstring dal, $(MODULE)))
	sed  -e '/GROUP/{s/libdal.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libdal.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (spi, $(findstring spi, $(MODULE)))
	sed  -e '/GROUP/{s/libspi.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libspi.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif


ifeq (gray, $(findstring gray, $(MODULE)))
	sed  -e '/GROUP/{s/libgray.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libgray.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (dice, $(findstring dice, $(MODULE)))
	sed  -e '/GROUP/{s/libdice.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libdice.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (avs, $(findstring avs, $(MODULE)))
	sed  -e '/GROUP/{s/libavs.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libavs.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (1394llc, $(findstring 1394llc, $(MODULE)))
	sed  -e '/GROUP/{s/lib1394llc.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a lib1394llc.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (i2c, $(findstring i2c, $(MODULE)))
	sed  -e '/GROUP/{s/libi2c.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libi2c.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif


ifeq (dsp, $(findstring dsp, $(MODULE)))
	sed  -e '/GROUP/{s/libdsp.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libdsp.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (aml, $(findstring aml, $(MODULE)))
	sed  -e '/GROUP/{s/libaml.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libaml.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

ifeq (sps, $(findstring sps, $(MODULE)))
	sed  -e '/GROUP/{s/libsps.a//}' ${LIB_DIR}/mytarget.ld > tmp
	sed  -e '/GROUP/{s/libtcat.a/libtcat.a libsps.a/}' tmp > ${LIB_DIR}/mytarget.ld 
	rm tmp
endif

	make allall

kernel: $(OS)kernel

ecoskernel:
	mkdir -p ${KERNEL_DIR}/${KERNEL_NAME}
	cd ${KERNEL_DIR}/${KERNEL_NAME}; \
	${TC_DIR}/os/ecos/src/tools/bin/ecosconfig.exe --config=${TC_DIR}/kernel/configurations/diceJrKernel.ecc tree; \
	make 

redboot_ram:
	mkdir -p ${KERNEL_DIR}/rbram; \
	cd ${KERNEL_DIR}/rbram; \
	${TC_DIR}/os/ecos/src/tools/bin/ecosconfig.exe --config=${TC_DIR}/kernel/configurations/redboot_ram.ecc tree; \
	make;

redboot_romram:
	mkdir -p ${KERNEL_DIR}/rbromram; \
	cd ${KERNEL_DIR}/rbromram; \
	${TC_DIR}/os/ecos/src/tools/bin/ecosconfig.exe --config=${TC_DIR}/kernel/configurations/redboot_romram.ecc tree; \
	make;

in_ram_test_dice2:
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_ram.ldi /tmp/mlt_arm_dice_VB_ram.ldi
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_ram.h /tmp/mlt_arm_dice_VB_ram.h
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_in_ram_test.ldi ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_ram.ldi
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_in_ram_test.h ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_ram.h
	mkdir -p ${KERNEL_DIR}/in_ram_test_dice2; \
	cd ${KERNEL_DIR}/in_ram_test_dice2; \
	${TC_DIR}/os/ecos/src/tools/bin/ecosconfig.exe --config=${TC_DIR}/kernel/configurations/internal_ram_test.ecc tree; \
	make;
	make -C ${TC_DIR}/chip/dice2/internal_ram_test INSTALL_DIR=${KERNEL_DIR}/in_ram_test_dice2/install

	mv /tmp/mlt_arm_dice_VB_ram.ldi ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_ram.ldi
	mv /tmp/mlt_arm_dice_VB_ram.h ${TC_DIR}/os/ecos/src/packages/hal/arm/dice/v2_0/include/pkgconf/mlt_arm_dice_VB_ram.h

in_ram_test_jr:
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_ram.ldi /tmp/mlt_arm_diceJr_EVM_ram.ldi
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_ram.h /tmp/mlt_arm_diceJr_EVM_ram.h
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_in_ram_test.ldi ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_ram.ldi
	cp ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_in_ram_test.h ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_ram.h
	mkdir -p ${KERNEL_DIR}/in_ram_test_jr; \
	cd ${KERNEL_DIR}/in_ram_test_jr; \
	${TC_DIR}/os/ecos/src/tools/bin/ecosconfig.exe --config=${TC_DIR}/kernel/configurations/internal_ram_test_jr.ecc tree; \
	make;
	make -C ${TC_DIR}/chip/diceJr/internal_ram_test INSTALL_DIR=${KERNEL_DIR}/in_ram_test_jr/install

	mv /tmp/mlt_arm_diceJr_EVM_ram.ldi ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_ram.ldi
	mv /tmp/mlt_arm_diceJr_EVM_ram.h ${TC_DIR}/os/ecos/src/packages/hal/arm/diceJr/v2_0/include/pkgconf/mlt_arm_diceJr_EVM_ram.h

install:
	rm -rf ${KERNEL_DIR}/${KERNEL_NAME}
	make $(OS)kernel
	make dep

doxygen:
	cd ../doxygen; \
	cp -rf images/ GeneratedDocumentation/html/; \
	doxygen

clean: 
	rm -fr ${PROJECT_DIR}/lib
	rm -fr ${PROJECT_DIR}/bin
	rm -fr ${PROJECT_DIR}/obj

cleanKernel:
	make clean
	rm -rf ${KERNEL_DIR}/${KERNEL_NAME}

cleanAll:
	make clean
	find ${TC_DIR} -name "*.[oa]" |xargs rm -f
	find ${TC_DIR} -name "*#" |xargs rm -f
	find ${TC_DIR} -name "*~" |xargs rm -f
	find ${TC_DIR} -name "*.bak" |xargs rm -f
	find ${TC_DIR} -name "*.stackdump" |xargs rm -f
	rm -rf ${KERNEL_DIR}/${KERNEL_NAME}


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
## $Id: Makefile,v 1.2 2006/09/27 20:35:45 zluo Exp $
##===========================================================================

# Platform specific setups

include ${MAKE_DIR}/Make.def

SRCS= \
aqm.c cr.c crCli.c rangemaps.c \
avc.c avcUtils.c avcCmdCli.c avcRspCli.c avcUnitCli.c \
avcCmdCCM.c avcCmdConnection.c avcCmdGeneral.c \
avcPanel.c avcPanelCli.c avcPanelCmd.c avcPanelRsp.c \
avcUnit.c avcUnitChannelUsage.c avcUnitConnect.c avcUnitConnectAV.c avcUnitNotify.c avcUnitPower.c \
avcUnitReserve.c avcUnitSecurity.c avcUnitSignalFormat.c avcUnitSignalSource.c avcUnitUtils.c avcUnitVendor.c \
avrDrd.c drd.c drdCli.c \
drs.c dss.c eds.c edsRead.c edsWrite.c edsServer.c edsSample.c edsCli.c sbm.c \
cmp.c cmpBC.c cmpCli.c cmpP2P.c cmpUtils.c \
avcCmdStreamFormat.c avcAudio.c avcMusic.c avcDriver.c avcdriverCli.c

DST=lib1394avc.a

${OBJ_DIR}/%.o : %.c
	$(MAKE_CC) 

${DST}: ${OBJS}
	$(MAKE_AR)

depend:
	$(MAKE_DEPEND) 

clean:
	rm -f ${OBJ_DIR}/${OBJS}; rm -f ${OBJ_DIR}/${DEPS}; rm -f ${LIB_DIR}/${DST}

-include ${DEPS}


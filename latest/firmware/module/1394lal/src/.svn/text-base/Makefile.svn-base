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
## $Id: Makefile,v 1.1.1.1 2006/08/10 20:00:56 zluo Exp $
##===========================================================================

# Platform specific setups

include ${MAKE_DIR}/Make.def

SRCS=\
bri.c \
csr.c csrCli.c \
irm.c irmCli.c \
lal.c lalAddrRng.c lalAsync.c lalAsyncStream.c lalCli.c \
lhl.c lhlCli.c lhlIRM.c lhlMsgQueue.c lhlPendingTx.c lhlReq.c lhlRsp.c lhlStatistics.c lhlStatusQueue.c lhlTlabel.c \
nci.c nciCB.c nciBM.c nciCli.c nciBMCli.c nciHandles.c nciIRM.c \
 

DST=lib1394lal.a

${OBJ_DIR}/%.o : %.c
	$(MAKE_CC) 

${DST}: ${OBJS}
	$(MAKE_AR)

depend:
	$(MAKE_DEPEND) 

clean:
	rm -f ${OBJ_DIR}/${OBJS}; rm -f ${OBJ_DIR}/${DEPS}; rm -f ${LIB_DIR}/${DST}

-include ${DEPS}


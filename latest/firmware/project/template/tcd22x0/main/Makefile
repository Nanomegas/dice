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
## $Id: Makefile,v 1.2 2007/02/07 01:54:27 ml Exp $
##===========================================================================

# Platform specific setups

include ${MAKE_DIR}/Make.def

SRCS= \
	cyg_main.c myApp.c myAppCli.c myModes.c \
	AKM4620.c metering.c \
	myMode0_JR.c myMode1_JR.c myMode2_JR.c myMode3_JR.c \
	myMode4_JR.c myMode5_JR.c myMode6_JR.c myMode7_JR.c myMode8_JR.c \
	myMode14_JR.c myMode15_JR.c \
	myMode0_Mini.c myMode1_Mini.c myMode2_Mini.c myMode3_Mini.c \
	myMode4_Mini.c myMode5_Mini.c myMode6_Mini.c myMode7_Mini.c myMode8_Mini.c \
	myMode14_Mini.c myMode15_Mini.c \
	myMode11_All.c

DST=$(APP_FILE)

${OBJ_DIR}/%.o : %.c
	$(MAKE_CC) 

${DST}: ${OBJS}
	$(MAKE_VERSION) 
	$(MAKE_APP) 

depend:
	$(MAKE_DEPEND) 

clean:
	rm -f ${OBJ_DIR}/${OBJS}; rm -f ${OBJ_DIR}/${DEPS}; rm -f ${LIB_DIR}/${DST}

-include ${DEPS}

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
## $Id: Makefile,v 1.5 2007/02/02 21:53:52 ml Exp $
##===========================================================================

# Platform specific setups

include ${MAKE_DIR}/Make.def

ifeq (1394avc, $(findstring 1394avc, ${MODULE}))
	SRCS= \
	cyg_main.c myAppAVC.c myAppCli.c 
else
	SRCS= \
	cyg_main.c myApp.c myAppCli.c myModes.c \
	myMode0.c myMode1.c myMode2.c myMode3.c myMode4.c myMode5.c myMode6.c myMode7.c myMode8.c \
	myMode9.c myMode10.c myMode11.c myMode12.c myMode13.c myMode14.c myMode15.c
endif

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

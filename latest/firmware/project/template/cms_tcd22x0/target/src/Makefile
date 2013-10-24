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
## $Id: Makefile,v 1.1.1.1 2006/11/13 19:56:40 zluo Exp $
##===========================================================================

# Platform specific setups

include ${MAKE_DIR}/Make.def

SRCS= \
targetBoard.c targetConfigCSR.c targetPlugs.c targetVendorDefs.c \

DST=libtcat.a

${OBJ_DIR}/%.o : %.c
	$(MAKE_CC) 

${DST}: ${OBJS}
	$(MAKE_AR)

depend:
	$(MAKE_DEPEND) 

clean:
	rm -f ${OBJ_DIR}/${OBJS}; rm -f ${OBJ_DIR}/${DEPS}; rm -f ${LIB_DIR}/${DST}

-include ${DEPS}


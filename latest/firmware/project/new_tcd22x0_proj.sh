#!/bin/sh
##===========================================================================
##
## This confidential and proprietary software may be used only as authorised
## by a licensing agreement from TC Applied Technologies Ltd.
##
## (C) COPYRIGHT 2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
##
## Unauthorized use, duplication or distribution of this software is  
## strictly prohibited by law.
##
## The entire notice above must be reproduced on all authorised copies and
## copies may only be made to the extent permitted by a licensing agreement
## from TC Applied Technologies Ltd.
##
##===========================================================================

# This is the utility to make a new project from /firmware/project/template/tcd22x0
# A project will be created based on the EVM002 evaluation board with a DICE-MINI 
# or DICE-JR microboard installed.
#
# For example, to create a project named productFoo, at this directory:
# run this command:
# ./new_tcd22x0_proj.sh productFoo
#
# This is provided for backward-compatibility. Users can use copyProject.sh directly.
#

TEMPLATE_NAME="tcd22x0"

SCRIPT_NAME="new_${TEMPLATE_NAME}_proj.sh"

PROJECT="$1"

if [ $# -ne 1 ] || [ -z "${PROJECT}" ]; then
	echo ""
	echo "This utility script will make a new project based on the ${TEMPLATE_NAME} template."
	echo ""
	echo "usage: ./${SCRIPT_NAME} <new project name>"
	echo ""
	exit
fi

./copyProject.sh template "${TEMPLATE_NAME}" "${PROJECT}"

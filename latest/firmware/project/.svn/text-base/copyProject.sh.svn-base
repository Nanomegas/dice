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

# This is the utility to make a new project from any existing project.
# It is required that projects have the same name as the directory they
# reside in.
# 
# ./copyProject <source path> <source name> <new name>
#
# <source path>: relative path to base of source directory
# <source name>: name of source project (copy from ./<source path>/<source name>)
# <new name>: name of new project (copy to ./<new name> )

# Example1: Create a copy of the tcd22x0 template and call it productFoo
# ./copyProject template tcd22x0 productFoo
#
#
# Example2: Create a copy of project productFoo and call it productBoo
# ./copyProject . productFoo productBoo
#
#
# Note: It is suggested that the project is cleaned before copy as it would
#       prevent unnecessary copying of temporary and binary files.


SRCDIR=$1
SRCNAME=$2
PROJECT=$3

if [ $# -lt 3 ]
then
	echo ""
	echo "usage: ./copyProject <source path> <source name> <new name>"
	echo ""
	echo "<source path> : relative path to base of source directory"
	echo "<source name> : name of source project (copy from ./<source path>/<source name>)"
	echo "<new name>    : name of new project (copy to ./<new name> )"
	echo ""
	echo "Example1: Create a copy of the tcd22x0 template and call it productFoo"
	echo "./copyProject template tcd22x0 productFoo"
	echo ""
	echo "Example2: Create a copy of project productFoo and call it productBoo"
	echo "./copyProject . productFoo productBoo"
	echo ""
	exit
fi

if [ ! -d ${SRCDIR}/${SRCNAME} ]; then 
    echo "The source project does not exist"
    exit
fi 

###Make sure we don't overwrite something important
if [ -d ${PROJECT} ]
then
	echo "${PROJECT} directory already exists, exiting"
	exit
fi

### copy the files
cp -r ${SRCDIR}/${SRCNAME}/ ${PROJECT}
	
###if the project name is the same as the template name just remove version control info
if test ${SRCNAME} = ${PROJECT}
then
	### remove any version control info
	find ${PROJECT} -name CVS|xargs rm -rf
	find ${PROJECT} -name ".svn"|xargs rm -rf 
	exit
fi

### remove any version control info
find ${PROJECT} -name CVS|xargs rm -rf
find ${PROJECT} -name ".svn"|xargs rm -rf 

sed  -e 's/'${SRCNAME}'/'${PROJECT}'/g' ${PROJECT}/make/Make.params > x
mv x ${PROJECT}/make/Make.params

sed  -e 's/'${SRCNAME}'/'${PROJECT}'/g' ${PROJECT}/make/vc_make.bat > x
mv x ${PROJECT}/make/vc_make.bat

sed  -e 's/'${SRCNAME}'/'${PROJECT}'/g' ${PROJECT}/make/vc_insight.bat > x
mv x ${PROJECT}/make/vc_insight.bat

sed  -e 's/'${SRCNAME}'/'${PROJECT}'/g' ${PROJECT}/make/buildDiceFromMSVC.txt > x
mv x ${PROJECT}/make/buildDiceFromMSVC.txt

sed  -e 's/'${SRCNAME}'/'${PROJECT}'/g' ${PROJECT}/make/${SRCNAME}.sln > x
rm ${PROJECT}/make/${SRCNAME}.sln
mv x ${PROJECT}/make/${PROJECT}.sln

sed  -e 's/'${SRCNAME}'/'${PROJECT}'/g' ${PROJECT}/make/${SRCNAME}.vcproj > x
rm ${PROJECT}/make/${SRCNAME}.vcproj
mv x ${PROJECT}/make/${PROJECT}.vcproj

### remove binary and tempory files
rm -rf ${PROJECT}/bin
rm -rf ${PROJECT}/obj
rm -rf ${PROJECT}/lib

### remove old left over VC files
find ${PROJECT} -name "${SRCNAME}.*"|xargs rm -rf 
find ${PROJECT}/make -name "Debug"|xargs rm -rf 
find ${PROJECT}/make -name "Release"|xargs rm -rf 


### remove .bak files
find ${PROJECT} -name "*.bak"|xargs rm -rf 


# set executable
chmod +x ${PROJECT}/make/*.bat

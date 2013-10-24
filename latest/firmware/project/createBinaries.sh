#!/bin/sh
##===========================================================================
##
## This confidential and proprietary software may be used only as authorised
## by a licensing agreement from TC Applied Technologies Ltd.
##
## (C) COPYRIGHT 2007-2011 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
##
## Unauthorized use, duplication or distribution of this software is  
## strictly prohibited by law.
##
## The entire notice above must be reproduced on all authorised copies and
## copies may only be made to the extent permitted by a licensing agreement
## from TC Applied Technologies Ltd.
##
##===========================================================================

# This utility is used to create the binaries for the main TCAT trunk.
# The two projects will be created, the binaries will be copied to the correct
# place in the main trunk.

# This script must be run from the project directory.

# TC_DIR=$PWN/../
# export TC_DIR

#######################################################################
#
# yes/no confirmation from the user
#
#######################################################################
questionYesNo()
{
	case "$2" in
		[Yy] | "Yes" | "yes" )  questionText="${1} [Yes] ";;
		[Nn] | "No" | "no" )    questionText="${1} [No] ";;
		 * )  questionText="${1} ";;
	esac

	while :; do
		read -p "$questionText" answer ;

		if [ -z "$answer" ] && [ -n "$2" ]; then
			answer="$2"
		fi

		case "$answer" in
			[Yy] | "Yes" | "yes" )     return 0;;
			[Nn] | "No" | "no" )       return 1;;
		esac
		echo "Please answer Y or N."  
	done
}


#######################################################################
#
# create the binaries
#
#######################################################################

if ! questionYesNo "This script will destroy any default TCAT project directories that currently exist (it will recreate them).  Continue?" No ; then
	exit 0
fi
echo

if [ ! -d template ]; then
    echo "No template directory"
    exit 1
fi

echo "--------------------------------------------------------------------------------"
echo "Creating evm_dice2 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/DICEII_EVM/evm_dice2Debug.bin
rm -rf evm_dice2
./new_diceII_proj.sh evm_dice2
echo "Building evm_dice2 project"
make -s -C evm_dice2/make/ install
cp evm_dice2/bin/evm_dice2Debug.bin ../../binary/firmware/DICEII_EVM/evm_dice2Debug.bin
echo

echo "--------------------------------------------------------------------------------"
echo "Creating evm002_dice2 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/EVM002/evm002_dice2Debug.bin
rm -rf evm002_dice2
./new_diceII_EVM002_proj.sh evm002_dice2
echo "Building evm002_dice2 project"
make -s -C evm002_dice2/make/ install
cp evm002_dice2/bin/evm002_dice2Debug.bin ../../binary/firmware/EVM002/evm002_dice2Debug.bin
echo

echo "--------------------------------------------------------------------------------"
echo "Creating evm002_tcd22x0 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/EVM002/evm002_tcd22x0Debug.bin
rm -rf evm002_tcd22x0
./new_tcd22x0_proj.sh evm002_tcd22x0
echo "Building evm002_tcd22x0 project"
make -s -C evm002_tcd22x0/make/ install
cp evm002_tcd22x0/bin/evm002_tcd22x0Debug.bin ../../binary/firmware/EVM002/evm002_tcd22x0Debug.bin
echo

echo "--------------------------------------------------------------------------------"
echo "Creating evm002_eap_tcd22x0 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/EVM002/evm002_eap_tcd22x0Debug.bin
rm -rf evm002_eap_tcd22x0
./new_eap_tcd22x0_proj.sh evm002_eap_tcd22x0
echo "Building evm002_eap_tcd22x0 project"
make -s -C evm002_eap_tcd22x0/make/ install
cp evm002_eap_tcd22x0/bin/evm002_eap_tcd22x0Debug.bin ../../binary/firmware/EVM002/evm002_eap_tcd22x0Debug.bin
echo

echo "--------------------------------------------------------------------------------"
echo "Creating evm002_avc_tcd22x0 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/EVM002/evm002_avc_tcd22x0Debug.bin
rm -rf evm002_avc_tcd22x0
./new_avc_tcd22x0_proj.sh evm002_avc_tcd22x0
echo "Building evm002_avc_tcd22x0 project"
make -s -C evm002_avc_tcd22x0/make/ install
cp evm002_avc_tcd22x0/bin/evm002_avc_tcd22x0Debug.bin ../../binary/firmware/EVM002/evm002_avc_tcd22x0Debug.bin
echo

echo "--------------------------------------------------------------------------------"
echo "Creating evm003_tcd22x0 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/EVM003/evm003_tcd22x0Debug.bin
rm -rf evm003_tcd22x0
./new_tcd22x0_EVM003_proj.sh evm003_tcd22x0
echo "Building evm003_tcd22x0 project"
make -s -C evm003_tcd22x0/make/ install
cp evm003_tcd22x0/bin/evm003_tcd22x0Debug.bin ../../binary/firmware/EVM003/evm003_tcd22x0Debug.bin
echo

echo "--------------------------------------------------------------------------------"
echo "Creating evm003_eap_tcd22x0 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/EVM003/evm003_eap_tcd22x0Debug.bin
rm -rf evm003_eap_tcd22x0
./new_eap_tcd22x0_EVM003_proj.sh evm003_eap_tcd22x0
echo "Building evm003_eap_tcd22x0 project"
make -s -C evm003_eap_tcd22x0/make/ install
cp evm003_eap_tcd22x0/bin/evm003_eap_tcd22x0Debug.bin ../../binary/firmware/EVM003/evm003_eap_tcd22x0Debug.bin
echo

echo "--------------------------------------------------------------------------------"
echo "Creating evm004_tcd22x0 project"
echo "--------------------------------------------------------------------------------"
rm -f ../../binary/firmware/EVM004/evm004_tcd22x0Debug.bin
rm -rf evm004_tcd22x0
./new_tcd22x0_EVM004_proj.sh evm004_tcd22x0
echo "Building evm004_tcd22x0 project"
make -s -C evm004_tcd22x0/make/ install
cp evm004_tcd22x0/bin/evm004_tcd22x0Debug.bin ../../binary/firmware/EVM004/evm004_tcd22x0Debug.bin
echo


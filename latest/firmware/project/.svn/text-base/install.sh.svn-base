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

# Used after the Firmware SDK files have beed copied. Creates the initial EVM 
# projects from templates and builds the install target for each.

# This script must be run from the project directory.


if [ ! -d template ]; then 
    echo "No template"
    exit
fi 

echo
if [ -d evm002_dice2 ]; then
	echo "project evm002_dice2 already exists, skipping.."
else
	echo "Creating evm002_dice2 project (DICEII microboard on the EVM002)"
	./new_diceII_EVM002_proj.sh evm002_dice2
	echo "Building evm002_dice2 project"
	make -s -C evm002_dice2/make/ install
fi

echo "----------------------------------------"
if [ -d evm002_tcd22x0 ]; then
	echo "project evm002_tcd22x0 already exists, skipping.."
else
	echo "Creating evm002_tcd22x0 project (DICE JR/MINI microboard on the EVM002)"
	./new_tcd22x0_proj.sh evm002_tcd22x0
	echo "Building evm002_tcd22x0 project"
	make -s -C evm002_tcd22x0/make/ install
fi

echo "----------------------------------------"
if [ -d evm002_eap_tcd22x0 ]; then
	echo "project evm002_eap_tcd22x0 already exists, skipping.."
else
	echo "Creating evm002_eap_tcd22x0 project (EAP DICE JR/MINI microboard on the EVM002)"
	./new_eap_tcd22x0_proj.sh evm002_eap_tcd22x0
	echo "Building evm002_eap_tcd22x0 project"
	make -s -C evm002_eap_tcd22x0/make/ install
fi

echo "----------------------------------------"
if [ -d evm_dice2 ]; then
	echo "project evm_dice2 already exists, skipping.."
else
	echo "Creating evm_dice2 project (DICEII on the classic EVM)"
	./new_diceII_proj.sh evm_dice2
	echo "Building evm_dice2 project"
	make -s -C evm_dice2/make/ install
fi

echo "----------------------------------------"
if [ -d evm002_avc_tcd22x0 ]; then
	echo "project evm002_avc_tcd22x0 already exists, skipping.."
else
	echo "Creating evm002_avc_tcd22x0 project (AV/C DICE JR/MINI microboard on the EVM002)"
	./new_avc_tcd22x0_proj.sh evm002_avc_tcd22x0
	echo "Building evm002_avc_tcd22x0 project"
	make -s -C evm002_avc_tcd22x0/make/ install
fi

echo "----------------------------------------"
if [ -d evm003_tcd22x0 ]; then
	echo "project evm003_tcd22x0 already exists, skipping.."
else
	echo "Creating evm003_tcd22x0 project (DICE-MINI on the EVM003)"
	./new_tcd22x0_EVM003_proj.sh evm003_tcd22x0
	make -s -C evm003_tcd22x0/make/ install
fi

echo "----------------------------------------"
if [ -d evm003_eap_tcd22x0 ]; then
	echo "project evm003_eap_tcd22x0 already exists, skipping.."
else
	echo "Creating evm003_eap_tcd22x0 project (EAP DICE-MINI on the EVM003)"
	./new_eap_tcd22x0_EVM003_proj.sh evm003_eap_tcd22x0
	make -s -C evm003_eap_tcd22x0/make/ install
fi

echo "----------------------------------------"
if [ -d evm003_eap_tcd22x0 ]; then
	echo "project evm003_eap_tcd22x0 already exists, skipping.."
else
	echo "Creating evm004_tcd22x0 project (EAP DICE-MINI on the EVM004-FW843)"
	./new_tcd22x0_EVM004_proj.sh evm004_tcd22x0
	make -s -C evm004_tcd22x0/make/ install
fi


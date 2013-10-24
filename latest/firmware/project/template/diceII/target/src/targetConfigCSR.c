//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
 * DESCRIPTION: Target Config CSR Handling

	Configuration of target specific CSR handling

	Revisions:
		created 10/23/2003 lm


*****************************************************************************/

static char 		moduleName[] = __FILE__ ;




#include "TCTasking.h"
#include "csr.h"
//#include "crc16.h"
//#include "lhlLLCDefs.h"
#include "lhlLLC.h"
#include "crb.h"
#include "targetConfigCSR.h"
#include "targetVendorDefs.h"

#define CSR_ROM_MAX_LENGTH		100 // (CSR_ROM_MAX_SIZE/4)

static QUADLET	csrConfigROM[CSR_ROM_MAX_LENGTH];
static uint32	csrConfigROMLen = 0;



// Protocol and unit directory configuration

#ifdef _DICE_DRIVER
//////////////// DICE DRIVER ///////////////////////////
// DiceDriver unit directories uses a private protocol
// The protocol is specified by the Vendor ID and the
// protocol sw version is always 0x000001
#define DICEDRIVER_SW_VERSION_ID	0x00000001

static HRESULT buildDiceDriverUnitDir(CRBHANDLE root)
{
	CRBHANDLE unit_dir = 0;

	unit_dir = crbAddSubDirectoryEntry(root, RDK_ID_UNIT);
	
	// following unit entries are for dicedriver
	crbAddImmediateEntry(unit_dir, RDK_ID_SPECIFIER_ID, THIS_VENDOR_ID);
	crbAddImmediateEntry(unit_dir, RDK_ID_VERSION, DICEDRIVER_SW_VERSION_ID);
	crbAddImmediateEntry(unit_dir, RDK_ID_MODEL, THIS_PRODUCT_ID);
	crbAddTextLeafEntry(unit_dir, THIS_PRODUCT_NAME);
	return NO_ERROR;
}
#endif //_DICE_DRIVER

#ifdef _FRMWLOAD_CSR
//////////////// SIMPLE DRIVER ///////////////////////////
// This driver is used for simple applications where no
// audio driver is required. It calls up a driver which
// supports Async. Read/Write. For historic reasons this
// was called the firmware load driver. It has nothing to
// with firmware loading.
#define SIMPLEDRIVER_SW_VERSION_ID	0x00001000

static HRESULT buildSimpleDriverUnitDir(CRBHANDLE root)
{
	CRBHANDLE unit_dir = 0;

	unit_dir = crbAddSubDirectoryEntry(root, RDK_ID_UNIT);
	
	// following unit entries are for dicedriver
	crbAddImmediateEntry(unit_dir, RDK_ID_SPECIFIER_ID, THIS_VENDOR_ID);
	crbAddImmediateEntry(unit_dir, RDK_ID_VERSION, SIMPLEDRIVER_SW_VERSION_ID);
	crbAddImmediateEntry(unit_dir, RDK_ID_MODEL, THIS_PRODUCT_ID);
	crbAddTextLeafEntry(unit_dir, THIS_PRODUCT_NAME);
	return NO_ERROR;
}
#endif //_FRMWLOAD_CSR



#ifdef _AVC
//////////////// AV/C DRIVER //////////////////////////
// The AV/C driver uses a public protocol. The protocol
// specifier ID is the UNIT_SPEC_ID_61883.
// The current AV/C version is UNIT_SW_VERSION_AVC.

static HRESULT buildAVCUnitDir(CRBHANDLE root)
{
	CRBHANDLE unit_dir = 0;

	unit_dir = crbAddSubDirectoryEntry(root, RDK_ID_UNIT);
	
	// following unit entries are for dicedriver
	crbAddImmediateEntry(unit_dir, RDK_ID_SPECIFIER_ID, UNIT_SPEC_ID_61883);
	crbAddImmediateEntry(unit_dir, RDK_ID_VERSION, UNIT_SW_VERSION_AVC);
	crbAddImmediateEntry(unit_dir, RDK_ID_MODEL, THIS_PRODUCT_ID);
	crbAddTextLeafEntry(unit_dir, THIS_PRODUCT_NAME);
	return NO_ERROR;
}
#endif //_AVC


#ifdef _MLAN
//////////////// MLAN DRIVER //////////////////////////
// The mLan driver comes in different flavors.
// They all use the Yamaha OID for the protocol
// specifier.
// In case of the original mLan a HAL specification is
// required for the driver to know which HAL driver to
// associate with the device. In case of OGT a generic
// HAL is specified.

#ifdef _OPEN_MLAN
#define MLAN_HAL_VENDOR_ID		VENDOR_ID_YAMAHA	/* Using Generic HAL */
#define MLAN_HAL_VENDOR_NAME	"YAMAHA"					/* Using Generic HAL */
#define MLAN_HAL_MODEL_ID			0xF00006					/* Using Generic HAL */
#define MLAN_HAL_MODEL_NAME		"GenericHAL"			/* Using Generic HAL */
#else //_OPEN_MLAN
#define MLAN_HAL_VENDOR_ID		THIS_VENDOR_ID		/* Using own HAL */
#define MLAN_HAL_VENDOR_NAME	THIS_VENDOR_NAME	/* Using own HAL */
#define MLAN_HAL_MODEL_ID		0x1									/* Using own HAL */
#define MLAN_HAL_MODEL_NAME		"DICE II"					/* Using own HAL */
#endif //_OPEN_MLAN

static HRESULT buildMLANUnitDir(CRBHANDLE root)
{
	CRBHANDLE unit_dir = 0;

	unit_dir = crbAddSubDirectoryEntry(root, RDK_ID_UNIT);
	
	// following unit entries are for dicedriver
	crbAddImmediateEntry(unit_dir, RDK_ID_SPECIFIER_ID, UNIT_SPEC_ID_YAMAHA);
	crbAddImmediateEntry(unit_dir, RDK_ID_VERSION, UNIT_SW_VERSION_MLAN);
	crbAddImmediateEntry(root, RDK_ID_VENDOR, THIS_VENDOR_ID);
	crbAddTextLeafEntry(root, THIS_VENDOR_NAME);
	crbAddImmediateEntry(unit_dir, RDK_ID_MODEL, THIS_PRODUCT_ID);
	crbAddTextLeafEntry(unit_dir, THIS_PRODUCT_NAME);

	//and now the HAL part
	crbAddImmediateEntry(root, RDK_ID_HAL_VENDOR, MLAN_HAL_VENDOR_ID);
	crbAddTextLeafEntry(root, MLAN_HAL_VENDOR_NAME);
	crbAddImmediateEntry(unit_dir, RDK_ID_HAL_MODEL, MLAN_HAL_MODEL_ID);
	crbAddTextLeafEntry(unit_dir, MLAN_HAL_MODEL_NAME);

	return NO_ERROR;
}
#endif //_MLAN





// generate the Configuration ROM based on chip_id
HRESULT targetConfigBuildConfigROM(uint32 chip_id_low, uint32 chip_id_hi)
{
	crbNewConfiguration(FALSE, chip_id_hi, chip_id_low);

	CRBHANDLE root = crbGetRoot();

	crbAddImmediateEntry(root, RDK_ID_VENDOR, THIS_VENDOR_ID);
	crbAddTextLeafEntry(root, THIS_VENDOR_NAME);
	crbAddImmediateEntry(root, RDK_ID_MODEL, THIS_PRODUCT_ID);
	crbAddTextLeafEntry(root, THIS_PRODUCT_NAME);
	crbAddImmediateEntry(root, RDK_ID_NODE_CAPS, NODE_MIN_CAPS);

#ifdef _AVC
	buildAVCUnitDir(root);
#endif
#ifdef _DICE_DRIVER
	buildDiceDriverUnitDir(root);
#endif
#ifdef _MLAN
	buildMLANUnitDir(root);
#endif
#ifdef _FRMWLOAD_CSR
	buildSimpleDriverUnitDir(root);
#endif
	csrConfigROMLen = CSR_ROM_MAX_LENGTH;
	crbCreateRom(csrConfigROM, &csrConfigROMLen);
	return NO_ERROR;
}
	
	

// return number of quadlet values in Configuration ROM
uint32 targetConfigGetROMQuadletSize(void)
{
	return csrConfigROMLen;
}

// read quadlet value in Configuration ROM based on quadlet index
QUADLET targetConfigReadConfigROMIndex(uint32 index)
{
	QUADLET quadlet = 0;

	if (index <= csrGetConfigROMQuadletSize())
	{
		quadlet = csrConfigROM[index];
	}

	return quadlet;
}


//This will change the Config to contain minimum configuration
//and is used before soft reset of the device.
HRESULT targetSetMinConfigROM(void)
{
	crbNewConfiguration(TRUE, THIS_VENDOR_ID<<8, 0);
	csrConfigROMLen = CSR_ROM_MAX_LENGTH;
	crbCreateRom(csrConfigROM, &csrConfigROMLen);
	return NO_ERROR;
}



// complete update the Configuration ROM, for debug and testing only
HRESULT targetConfigUpdateConfigROM(uint32 *data, uint32 size)
{
	HRESULT		hResult = NO_ERROR;

	if (size > CSR_ROM_MAX_LENGTH)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	memcpy(csrConfigROM, data, size*sizeof(uint32));
	csrConfigROMLen = size;
	lhlLLCConfigROMUpdated();

	return hResult;
}

// return base address of Configuration ROM, for debug and testing only
uint32 targetConfigGetConfigROMAddress()
{
	return (uint32) csrConfigROM;
}




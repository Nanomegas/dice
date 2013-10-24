//==============================================================================
//####TCAT-SOURCE-HEADER-BEGIN####
//
// This confidential and proprietary source may be used only as authorized
// by a licensing agreement from TC Applied Technologies.
//
// Copyright (c) 2004-2013 TC Applied Technologies
//                         a division of TC Group Americas Ltd.
//                         All rights reserved.
//
// Unauthorized use, duplication or distribution of this source is strictly
// prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies.
//
//####TCAT-SOURCE-HEADER-END####
//==============================================================================
//
//	Defaults for third party product specific defines.
//
//	To override these defaults, define them in tcat_dice_myproduct.h
//
//==============================================================================

#ifndef _TCAT_DICE_DEFAULTS_H_
#define _TCAT_DICE_DEFAULTS_H_


#include "tcat_dice_myproduct.h"

// uniform type identifier (UTI) string used in Mac OS X bundle identifiers
#ifndef kTCAT_DICE_VENDOR_UTI
#define kTCAT_DICE_VENDOR_UTI				tc.tctechnologies
#endif

// the bundle ID of the OS X application to be launched by the HAL for configuration of the device and its engine(s)
#ifndef kTCAT_DICE_CONFIG_APP_BUNDLE_ID
#define kTCAT_DICE_CONFIG_APP_BUNDLE_ID		kTCAT_DICE_VENDOR_UTI.kTCAT_DICE_DRV.controlpanel
#endif

// the bundle ID of the installer package
// (this must not change once a product ships, otherwise the installer will not correctly detect an older version when upgrading)
#ifndef kTCAT_DICE_INSTALLER_PKG_BUNDLE_ID
#define kTCAT_DICE_INSTALLER_PKG_BUNDLE_ID	kTCAT_DICE_DRV
#endif

#ifndef kTCAT_DICE_GUID64_VENDOR_ID_SHIFT
#define kTCAT_DICE_GUID64_VENDOR_ID_SHIFT	40
#endif

#ifndef kTCAT_DICE_GUID64_VENDOR_ID_MASK
#define kTCAT_DICE_GUID64_VENDOR_ID_MASK	0xffffff
#endif

// the device Unit SW Version field, used on Mac OS X for device matching
#ifndef kTCAT_DICE_UNIT_SW_VERSION
#define kTCAT_DICE_UNIT_SW_VERSION		0x000001
#endif

#ifndef kTCAT_DICE_GUID64_CAT_ID
#define	kTCAT_DICE_GUID64_CAT_ID		4
#endif

#ifndef kTCAT_DICE_GUID64_CAT_ID_SHIFT
#define	kTCAT_DICE_GUID64_CAT_ID_SHIFT	32
#endif

#ifndef kTCAT_DICE_GUID64_CAT_ID_MASK
#define	kTCAT_DICE_GUID64_CAT_ID_MASK	0xff
#endif

#ifndef kTCAT_DICE_GUID64_PRODUCT_ID_SHIFT
#define	kTCAT_DICE_GUID64_PRODUCT_ID_SHIFT	22
#endif

#ifndef kTCAT_DICE_GUID64_PRODUCT_ID_MASK
#define	kTCAT_DICE_GUID64_PRODUCT_ID_MASK	0x3ff
#endif

#ifndef kTCAT_DICE_GUID64_PRODUCT_SERIAL_SHIFT
#define	kTCAT_DICE_GUID64_PRODUCT_SERIAL_SHIFT	0
#endif

#ifndef kTCAT_DICE_GUID64_PRODUCT_SERIAL_MASK
#define	kTCAT_DICE_GUID64_PRODUCT_SERIAL_MASK	0x3fffff
#endif

#ifndef kTCAT_DICE_SUPPORTED_DEVICES
#define kTCAT_DICE_SUPPORTED_DEVICES			kTCAT_DICE_DEVICES ///< customizable number of devices that can stack under this driver, up to kTCAT_DICE_DEVICES.
#endif

#ifndef kTCAT_DICE_VENDOR_VERSION_STR
#define kTCAT_DICE_VENDOR_VERSION_STR			None Specified ///< an optional vendor specified version string that is returned by kIoctlVendorVersionStrGet
#endif

#ifndef kTCAT_DICE_USR_FS_SAFE
#define kTCAT_DICE_USR_FS_SAFE					kTCAT_DICE_USR ///< a file system safe version of kTCAT_DICE_USR used to create Start menu entries (cannot contain \/:*?"<>| )
#endif

#ifndef kTCAT_DICE_INSTALL_DEFAULT_DIR_NAME
#define kTCAT_DICE_INSTALL_DEFAULT_DIR_NAME		kTCAT_DICE_DRV
#endif

#ifndef kTCAT_DICE_INSTALL_DEFAULT_GROUP_NAME
#define kTCAT_DICE_INSTALL_DEFAULT_GROUP_NAME	kTCAT_DICE_VENDOR
#endif

#ifndef kTCAT_DICE_SUPPORTS_DUALWIRE
#define kTCAT_DICE_SUPPORTS_DUALWIRE			1
#endif

#ifndef kTCAT_DICE_USE_NICKNAMES
#define kTCAT_DICE_USE_NICKNAMES				1
#endif

#ifndef kTCAT_DICE_DEFAULT_SAMPLE_RATE
#define kTCAT_DICE_DEFAULT_SAMPLE_RATE			48000
#endif

#ifndef kTCAT_DICE_DEFAULT_OPERATION_MODE
#define kTCAT_DICE_DEFAULT_OPERATION_MODE		1
#endif

// default buffer size which is specified at the base sample rates (44.1/48kHz),
// so a value of 512 provides 512 @ 44.1/48kHz, 1024 @ 88.2/96kHz, and 2048 @ 176.4/192kHz
#ifndef kTCAT_DICE_DEFAULT_ASIO_BUFFER_SIZE
#define kTCAT_DICE_DEFAULT_ASIO_BUFFER_SIZE		512
#endif

// controls whether the WDM audio interface is made available on Windows
#ifndef kTCAT_DICE_INSTALL_WDM
#define kTCAT_DICE_INSTALL_WDM					1
#endif

// controls whether the AC3 audio interface is made available on Windows
// (by default it's only made available if the device indicates that it supports AC3)
#ifndef kTCAT_DICE_INSTALL_AC3
#define kTCAT_DICE_INSTALL_AC3					1
#endif

#ifndef kTCAT_DICE_MAX_WDM_IN_CHANNELS
#define kTCAT_DICE_MAX_WDM_IN_CHANNELS			8
#endif

#ifndef kTCAT_DICE_MAX_WDM_OUT_CHANNELS
#define kTCAT_DICE_MAX_WDM_OUT_CHANNELS			8
#endif

#ifndef kTCAT_DICE_DEFAULT_WDM_IN_BIT_DEPTH
#define kTCAT_DICE_DEFAULT_WDM_IN_BIT_DEPTH		16
#endif

#ifndef kTCAT_DICE_DEFAULT_WDM_OUT_BIT_DEPTH
#define kTCAT_DICE_DEFAULT_WDM_OUT_BIT_DEPTH	16
#endif

#ifndef kTCAT_DICE_WDM_LINE_CONNECTOR_OUTPUT
#define kTCAT_DICE_WDM_LINE_CONNECTOR_OUTPUT	0
#endif

#ifndef kTCAT_DICE_CREATE_WDM_STEREO_PAIRS
#define kTCAT_DICE_CREATE_WDM_STEREO_PAIRS		0
#endif

// amount of time in milliseconds to wait for a device to send a clock locked notification
// after the clock source or clock rate has been changed
#ifndef kTCAT_DICE_DEFAULT_CLOCK_LOCK_WAIT_TIME
#define kTCAT_DICE_DEFAULT_CLOCK_LOCK_WAIT_TIME	1000
#endif


#endif  //_TCAT_DICE_DEFAULTS_H_

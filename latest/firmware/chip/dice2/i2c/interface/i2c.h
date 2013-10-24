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

#ifndef _I2C_H
#define _I2C_H

#include "TCTypes.h"

/** @addtogroup I2C

	This module provides an interface between this device to other i2c slave devices. Dice device always acts as master device.

	@{	
*/


/// I2C  address mode
typedef enum 
{ 
	I2C_7_BIT_ADDR,		//!< ... 7 bit address mode
	I2C_10_BIT_ADDR,	//!< ... 10 bit address mode
} i2c_mode_t;

/// I2C speed
typedef enum 
{
	I2C_STANDARD_SPEED = 1,		//!< ... standard speed, up to 100kbps
	I2C_FAST_SPEED,				//!< ... fast speed, up to 400kbps
	I2C_HIGH_SPEED,				//!< ... high speed, up to 3.4Mbps
} i2c_speed_t;

#define I2C_RX_TX_BUF_SIZE 8

#define E_I2C_TRY_AGAIN 0xF00000F

/** Initializes the Dice Driver support
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT i2cInitialize(void);

//! Setup I2C address mode and speed
void i2cSetup(i2c_mode_t addrMode, i2c_speed_t speed);

//! Write a single byte to targetAddress
uint32 i2cWriteValue(uint16 targetAddress, uint8 value);

//! Write 2 bytes to target address
uint32 i2cWrite2Value(uint16 targetAddress, uint8 v1, uint8 v2);

//! Write bufLength (<=I2C_RX_TX_BUF_SIZE) bytes to targetAddress
uint32 i2cWrite(uint16 targetAddress, uint8 *buf, uint8 bufLength);

//! Read one byte from target address
uint32 i2cReadValue(uint16 targetAddress, uint8 *value);

//! Read bufLength bytes into buf, max I2C_RX_TX_BUF_SIZE bytes at a time
uint32 i2cRead(uint16 targetAddress, uint8 *buf, uint32 bufLength);

/// Report number of errors happened
uint32 i2cErrorCounter(void);

// Report source of last error
uint32 i2cLastError(void);

//! @}

#endif //_I2C_H


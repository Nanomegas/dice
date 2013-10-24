//===========================================================================
//
//===========================================================================
/****************************************************************************
 * DESCRIPTION: AXM20.c

	Purpose:	Support for AXM20 analog expander board for DICEII EVM
	
	Revisions:
		created 05/21/2005 brian

****************************************************************************/
#include <string.h>
#include "ErrorCodes.h"
#include "TCDebug.h"
#include "i2c.h"
#include "AXM20.h"
#include "cliSIO.h"
#include "dicedriver.h"
#include "diceDriverInterface.h"
#include "targetboard.h"
#include "gray.h"

static struct
{
	uint32 knobSwitch;
	uint32 DIPsw2;

} EVMvals;

static uint8 CODECids[2] = { 0 };

HRESULT AXM20Initialize(void)
{
	HRESULT hResult = NO_ERROR;

	uint16 addr = 0;
	uint8 reg = 0;

	// read chip id for each CODEC at 0x4a, 0x01 and 0x49, 0x01
	// if zero, return
	reg = 0x01;
	addr = CODEC_LOWER_CH_I2C_ADDR;
	// write offset first to set up read addr
	i2cWriteValue(addr, reg);
	// get current values
	i2cReadValue(addr, &CODECids[0]);

	addr = CODEC_UPPER_CH_I2C_ADDR;
	i2cWriteValue(addr, reg);
	i2cReadValue(addr, &CODECids[1]);

	memset(&EVMvals, 0, sizeof(EVMvals));
	EVMvals.knobSwitch = (uint32)IsSwitchOn(0);
	EVMvals.DIPsw2 = (uint32)IsSwitchOn(2);

	hResult = AXM20InitGrayIF(0);

	return hResult;
}

HRESULT AXM20SetInChVol(int ch, int vol)
{
	HRESULT hResult = NO_ERROR;
	uint16 addr = 0;
	uint8 reg = 0;

//	sysDebugPrintf("AXM20SetInChVol: ch:%i, vol:%x\n", ch, vol );

	ch -= 1;

	// send volume to codec based on channel
	if ((ch >= 0) && (ch <= 5))
	{
		addr = CODEC_LOWER_CH_I2C_ADDR;
	}
	else if ((ch >= 8) && (ch <= 13))
	{
		ch -= 8;
		addr = CODEC_UPPER_CH_I2C_ADDR;
	}
	else
	{	
		return E_GEN_ILLEGAL_PARAM;
	}

	reg = ch + CODEC_INVOL_I2C_OFFSET;
	i2cWrite2Value(addr, reg, vol);

	return hResult;
}

HRESULT AXM20SetOutChVol(int ch, int vol)
{
	HRESULT hResult = NO_ERROR;
	uint16 addr = 0;
	uint8 reg = 0;
//	int channel = ch;

	// GUI sends channels 1-16
	if ((ch < 1) || (ch > 16))
	{
		return E_GEN_ILLEGAL_PARAM;
	}

	// GUI sends values 0-255
	//	dB values 0 to -127.5 in 0.5 dB steps = 0 - 255 in codec vol register
	if (vol < 0)
	{
		vol = 0;
	}
	else if (vol > 255)
	{
		vol = 255;
	}

	ch -= 1;

	// send volume to codec based on channel
	if (ch > 7)
	{
		addr = CODEC_UPPER_CH_I2C_ADDR;
		ch -= 8;
	}
	else
	{
		addr = CODEC_LOWER_CH_I2C_ADDR;
	}

	reg = ch + CODEC_OUTVOL_I2C_OFFSET;
	i2cWrite2Value(addr, reg, vol);

//	sysDebugPrintf("AXM20SetOutChVol: ch:%i, vol:%x\n", channel, vol );

	return hResult;
}

// no input mutes on AXM20

HRESULT AXM20SetOutChMute(int ch, int mute)
{
	HRESULT hResult = NO_ERROR;

	uint16 addr = 0;
	uint8 reg = 0;
	uint8 val = 0;
	uint8 ret = 0;

	if ((ch < 1) || (ch > 16))
	{
		return E_GEN_ILLEGAL_PARAM;
	}

	reg = CODEC_MUTE_I2C_OFFSET;
	val = (mute == 0) ? 0 : 1;
//	sysDebugPrintf("AXM20SetOutChMute: ch:%i, val:%i\n", ch, val );

	ch -= 1;

	// send to codec based on channel
	if (ch > 7)
	{
		addr = CODEC_UPPER_CH_I2C_ADDR;
		ch -= 8;
	}
	else
	{
		addr = CODEC_LOWER_CH_I2C_ADDR;
	}

	// write offset first to set up read addr
	i2cWriteValue(addr, reg);
	// get current value
	i2cReadValue(addr, &ret);
	// set or clear mute bit
//	sysDebugPrintf("  From codec: %i\n", ret );

	if (val)
		ret |= (1 << ch);
	else
		ret ^= (1 << ch);

	// send it back
//	sysDebugPrintf("  To codec: %i\n", ret );

	i2cWrite2Value(addr, reg, ret);

	return hResult;
}

HRESULT AXM20GetOutChVol(int ch, int *muted, int *vol)
{
	HRESULT hResult = NO_ERROR;
	uint8 reg = 0;
	uint8 ret;
	uint16 addr = 0;
//	int channel = ch;

	// GUI sends 1-based ch numbers, codec uses 0-based ch num
	ch -= 1;

	if ((ch < 0) || (ch > 15))
	{
		return E_GEN_ILLEGAL_PARAM;
	}

	// send to codec based on channel
	if (ch > 7)
	{
		addr = CODEC_UPPER_CH_I2C_ADDR;
		ch -= 8;
	}
	else
	{
		addr = CODEC_LOWER_CH_I2C_ADDR;
	}

	reg = CODEC_MUTE_I2C_OFFSET;
	// write offset first to set up read addr
	i2cWriteValue(addr, reg);
	// get current value
	i2cReadValue(addr, &ret);

	ret &= (1 << ch);
	
	*muted = (ret == 0) ? 0 : 1;

	reg = ch + CODEC_OUTVOL_I2C_OFFSET;
	// write offset first to set up read addr
	i2cWriteValue(addr, reg);
	// get current value
	i2cReadValue(addr, &ret);

	*vol = ret;

//	sysDebugPrintf("AXM20GetOutChVol: ch:%i, muted:%i, vol:%i\n", channel, *muted, *vol );

	return hResult;
}

HRESULT AXM20GetInChVol(int ch, int *muted, int *vol)
{
	HRESULT hResult = NO_ERROR;
	uint8 reg = 0;
	uint8 ret;
	uint16 addr = 0;
	int channel = 0;

	// GUI sends 1-based ch numbers, codec uses 0-based ch num
	ch -= 1;
	channel = ch;

	// send to codec based on channel
	// input volumes supported for 0-5, 8-13
	if ((ch >=0) && (ch <= 5))
	{
		addr = CODEC_LOWER_CH_I2C_ADDR;
	}
	else if ((ch >= 8) && (ch <= 13))
	{
		addr = CODEC_UPPER_CH_I2C_ADDR;
		ch -= 8;
	}
	else
	{
		return E_GEN_ILLEGAL_PARAM;
	}

	*muted = 0;	// no mute in current codec

	reg = ch + CODEC_INVOL_I2C_OFFSET;
	// write offset first to set up read addr
	i2cWriteValue(addr, reg);
	// get current value
	i2cReadValue(addr, &ret);

	*vol = ret;

//	sysDebugPrintf("AXM20GetInChVol: ch:%i, muted:%i, vol:%i\n", channel, *muted, *vol );

	return hResult;
}

HRESULT AXM20Update(void)
{
	HRESULT hResult = NO_ERROR;
	uint32 uiVal = 0;
	uint32 uiNotification = 0;

	// knob switch
	uiVal = IsSwitchOn(0);
	if (EVMvals.knobSwitch != uiVal)
	{
		EVMvals.knobSwitch = uiVal;
//		if (uiVal == 0)	// F1418
		if (uiVal)		// AXM20
		{
			uiNotification |= DD_NOTIFY_USER4;
		}
		else
		{
			uiNotification |= DD_NOTIFY_USER3;
		}
	}

	// dip sw 2
	uiVal = IsSwitchOn(2);
	if (EVMvals.DIPsw2 != uiVal)
	{
		EVMvals.DIPsw2 = uiVal;
		if (uiVal)
		{
			uiNotification |= DD_NOTIFY_USER1;
		}
		else
		{
			uiNotification |= DD_NOTIFY_USER2;
		}
	}

	if (uiNotification)
	{
		diceDriverNotifyHost(uiNotification);
	}

	return hResult;
}

// DD_NOTIFY_RESERVED1-4 are used for sending gray code changes to a host UI
// DD_NOTIFY_RESERVED1: coder 0 increment
// DD_NOTIFY_RESERVED2: coder 0 decrement 
// DD_NOTIFY_RESERVED3: coder 1 increment
// DD_NOTIFY_RESERVED4: coder 1 decrement
void AXM20GrayCallback(uint8 enc, int8 change)
{
/*	int32		newvalue = 0;

	newvalue = EVMvals.knobAbs + change;
	if (newvalue < 0x000000) newvalue = 0x000000;
	if (newvalue > 1000) newvalue = 1000;

	EVMvals.knobAbs = newvalue;
	EVMvals.knobDelta = change;
	EVMvals.knobSwitch = (uint32)IsSwitchOn(0);		// switch on gray coder knob
	EVMvals.DIPsw2 = (uint32)IsSwitchOn(2);			// switch 2 on DIPSW3

//	sysDebugPrintf("gray callBackValue%i: change:%i newvalue:%i sw0:%i sw2:%i\n\r", enc, EVMvals.knobDelta, EVMvals.knobAbs, EVMvals.knobSwitch, EVMvals.DIPsw2);
*/
	if (change == -1)	// invert for clockwise increment
	{
		diceDriverNotifyHost(DD_NOTIFY_RESERVED1);
	}
	if (change == 1)
	{
		diceDriverNotifyHost(DD_NOTIFY_RESERVED2);
	}
}

HRESULT AXM20InitGrayIF(uint32 encId)
{
	HRESULT hResult = NO_ERROR;

	memset(&EVMvals, 0, sizeof(EVMvals));
	EVMvals.knobSwitch = (uint32)IsSwitchOn(0);
	EVMvals.DIPsw2 = (uint32)IsSwitchOn(2);

	hResult = grayEventSetPollEnable(FALSE);
	hResult += grayEventInitializeTasks();
	hResult += grayEventInitialize();

	if (hResult == NO_ERROR)
		grayEventAddCallback(encId, AXM20GrayCallback);

	return hResult;
}


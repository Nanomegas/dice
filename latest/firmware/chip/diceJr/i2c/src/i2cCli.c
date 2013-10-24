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

#ifdef _I2C
#ifdef _CLI 
#include "cliToolDefs.h"
#include "cli.h"
#include "i2c.h"
#include "ErrorCodes.h"

#define OP_SIZE 1
static HRESULT i2c_write(uint32 target_addr, uint32 offset, uint32 src, uint32 length)
{
	int i, remain = length;;
	uint8 off = offset, *ptr = (uint8 *)src;
	uint8 buffer[OP_SIZE+1];

	cliPrintf("%x %x %x %x\n", target_addr, offset, src, length);
	//set the pointer
	while(remain) {
		i = (remain > OP_SIZE) ? OP_SIZE : remain;
		buffer[0] = off;
		memcpy(&buffer[1], ptr, i);
		//if there is any error, maybe slave is not fast enough, 
		if(i2cWrite((uint16)target_addr, buffer, i+1)) continue;
		remain -= i;
		ptr += i;
		off += i;
	}	
	return NO_ERROR;
}

uint8 read_buf[256];
HRESULT i2c_read(uint32 target_addr, uint32 offset, uint32 length)
{
	int i, n;
	//write offset first
	i2cWriteValue(target_addr, offset);
	n = i2cRead(target_addr, read_buf, length);

	for(i = 0; i < n; i++) {
		if(i%8 == 0) 
			cliPrintf("\n");
		cliPrintf("0x%02x ", read_buf[i]);
	}
	cliPrintf("\n");
	if(n == length)
		return NO_ERROR;
	else
		return E_FAIL;
}

static  char	WRITE_UsageString[]= 
"i2c.write target_address start_address src_address src_length\n\r";

static	CLIDescriptor	Write_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"write"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&i2c_write},
		{kCLI_UsageString, 		(uint32)WRITE_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"iiii"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Write to a i2c device"},
		{kCLI_GroupName,		(uint32)"i2c"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static HRESULT i2c_writevalue(uint32 target_addr, uint32 value)
{
	return i2cWriteValue(target_addr, value);
}
static HRESULT i2c_write2value(uint32 target_addr, uint32 v1, uint32 v2)
{
	return i2cWrite2Value(target_addr, v1, v2);
}

static  char	WRITEVALUE_UsageString[]= 
"i2c.writevalue target_address value\n\r";

static	CLIDescriptor	Writevalue_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"writevalue"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&i2c_writevalue},
		{kCLI_UsageString, 		(uint32)WRITEVALUE_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"ii"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Write value to i2c device"},
		{kCLI_GroupName,		(uint32)"i2c"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static  char	WRITE2VALUE_UsageString[]= 
"i2c.write2value target_address value1 value2\n\r";

static	CLIDescriptor	Write2value_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"write2value"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&i2c_write2value},
		{kCLI_UsageString, 		(uint32)WRITE2VALUE_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"iii"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Write value1 and value2 to i2c device"},
		{kCLI_GroupName,		(uint32)"i2c"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

static  char	READ_UsageString[]= 
"i2c.read target_address src_start_address src_length\n\r";

static	CLIDescriptor	Read_Descriptor[]=
	{
		{kCLI_Name,			(uint32)"read"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&i2c_read},
		{kCLI_UsageString, 		(uint32)READ_UsageString},
		{kCLI_ArgumentPatternString, (uint32)"iii"},
		{kCLI_ConstantsString,	(uint32)""},
		{kCLI_ModuleName,		(uint32)"Read from a i2c device"},
		{kCLI_GroupName,		(uint32)"i2c"},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

// This module demonstrate the use of i2c. The serial EEPROM Atmel AT24C16A 
// is at the i2c address of 0x50 - 0x57. 
HRESULT i2cInstallCLITools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(Writevalue_Descriptor);
	hResult += cliInstallCLIDescriptor(Write2value_Descriptor);
	hResult += cliInstallCLIDescriptor(Write_Descriptor);
	hResult += cliInstallCLIDescriptor(Read_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing fis list cli tools");
	}
	return hResult;
}

#endif //_CLI
#endif //_I2c

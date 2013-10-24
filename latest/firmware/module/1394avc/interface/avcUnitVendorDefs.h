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

 DESCRIPTION:

	AV/C definitions from 1394ta specifications

	Define TC Electronic vendor specific avc commands.

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifndef _AVCUNITVENDORDEFS_H
#define _AVCUNITVENDORDEFS_H

/*
	AV/C defines a vendor-specific command as opcode 0x00 followed by a 4-byte vendor ID.
	Everything after that is vendor-specific.
	For TC Electronic, the vendor ID is followed by a 4-byte TC header.
	The TC header is followed by 'n' bytes of data:

		offset	length contents
		------  -----  --------
		opcode	  1	   AVC - vendor-specific opcode 
		 0x00     3    AVC - vendor id (for TC this is 0x000166)
		 0x03     1    TC - class id
		 0x04     1    TC - sequence number
		 0x05     2    TC - command id
		 0x07     n    TC - data specific to the class/sequence/command
*/

// class id
typedef	uint8	TC_VSAVC_CLASS_TYPE;
// sequence number
typedef	uint8	TC_VSAVC_SEQ_TYPE;
// command id
typedef	uint16	TC_VSAVC_CMD_TYPE;

#define TC_VENDOR_HEADER_SIZE		(sizeof(TC_VSAVC_CLASS_TYPE)+sizeof(TC_VSAVC_SEQ_TYPE)+sizeof(TC_VSAVC_CMD_TYPE))
/*
	TC ELECTRONIC VENDOR SPECIFIC CALLBACK CLASS ID DEFINITIONS
*/

#define TC_VSAVC_CLASS_COMMON		0		// firmware common commands
#define	TC_VSAVC_CLASS_GENERAL		1		// general command
							     
/*
	TC_VSAVC_CLASS_COMMON command id definitions
*/
#define TC_VSAVC_CMD_SQUAWK				6	// 
#define TC_VSAVC_CMD_SELF_IDENTIFY      7   // self identify command

#define TC_VSAVC_CMD_CODELOAD			14	// commands for uploading code into device

/*
	TC_VSAVC_CLASS_GENERAL command id definitions
*/
#define	TC_VSAVC_CMD_PGM_IDENTIFY		1	// firmware identify command
#define	TC_VSAVC_CMD_TUNER_FREQ			2	// tuner frequency command
#define	TC_VSAVC_CMD_TUNER_PRESET		3	// tuner preset command
#define	TC_VSAVC_CMD_TUNER_SCAN_MODE	4	// tuner scan mode command
#define	TC_VSAVC_CMD_TUNER_OUTPUT		5	// tuner output command
#define TC_VSAVC_CMD_RAW_SERIAL			10  // outputs raw serial out the host port.

// Subfunctions of the CODE LOAD CONTROL command
#define AVC_TC_CODELOAD_CONTROL_IMAGETYPES	0
#define AVC_TC_CODELOAD_CONTROL_START		1

/**************************************/
/* Firmware identify command 

		offset	length contents
		------  -----  --------
		 0x07    24    TC_PROGRAM_ATTRIBUTES
*/


#if 1 //LM??? testing
typedef enum 
{
	// boot loaders
	FIRST_BOOT_PROG				= 1,
	PROG_POCO_BOOT				= 1,
	LAST_BOOT_PROG				= 99,

	// base application
	FIRST_APPLICATION_PROG		= 100,
	PROG_POCO_APPLICATION		= 100,
	PROG_POCO_PRODUCTION_TEST	= 101,
	LAST_BASE_PROG				= 199,

	// DRD's
	FIRST_DRD_PROG				= 200,
	PROG_AVR_DRD				= 200,
	PROG_JUKE_DRD				= 201,
	PROG_DENON_AVR_DRD			= 202,
	PROG_HAVI_AVR_DRD			= 203,
	LAST_DRD_PROG				= 299,

	// Test programs - for development only
	FIRST_TEST_PROG				= 10000,
	PROG_POCO_TEST				= 10000,
	LAST_TEST_PROG				= 10099,

	// final value to force enum to be 32-bits regardless of compiler options
	PROG_TYPE_UNDEFINED			= 0xffffffff
} TC_PROGRAM_TYPES;

typedef struct
{
	uint32		attributeVersion;
	uint32		programType;
	uint32		programVersion;
	uint32		reserved[5];
} TC_PROGRAM_ATTRIBUTES;

#endif //LM???

/**************************************/
/* Code Upload command 

		op[N]	length contents
		------  -----  --------
		 0x07      1	sub-function ID (enumerated below)
		 0x08	  ...	sub-function dependent parameters


	All types are prefaced by TC_VSAVC_CL_.....

*/

// op[7] = sub-function id
typedef enum 
{
	TC_VSAVC_CL_BEGIN_UPLOAD = 	0x00,
	TC_VSAVC_CL_END_UPLOAD = 	0x01,
	TC_VSAVC_CL_ABORT_UPLOAD =	0x02,
	TC_VSAVC_CL_DATA =			0x0A
} TC_VSAVC_CL_SUBFUNC_ID ;


/* BEGIN_UPLOAD sub-function

		op[N]	length contents
		------  -----  --------
		 0x07      1	0x00 (sub-func id)
		 0x08	   1	upload id - supplied by target in response
		 0x09	   2	image type (enumerated below)
		 0x0b	   2	image sub-type
		 0x0d	   2	reserved
		 0x0f	   1	image format (enumerated below)
*/

// op[9] = image type
typedef enum
{
	TC_VSAVC_CL_BOOT_IMAGE =	0x00,	// boot program
	TC_VSAVC_CL_APP_IMAGE =		0x01,	// application program
	TC_VSAVC_CL_FPGA_IMAGE =	0x02	// fpga image
} TC_VSAVC_CL_IMAGE_TYPE ;

// op[0x0f] = image format (currently only TC_CUSTOM is implemented
typedef enum
{
	TC_VSAVC_CL_MOT_S_REC = 	0x00,	// Motorola s-records
	TC_VSAVC_CL_INTEL_HEX =		0x01,	// Intel hex format
	TC_VSAVC_CL_TC_CUSTOM = 	0xFF	// Our own format
} TC_VSAVC_CL_IMAGE_FORMAT ;

/* END_UPLOAD sub-function

		op[N]	length contents
		------  -----  --------
		 0x07      1	0x01 (sub-func id)
		 0x08	   1	upload id
*/

/* ABORT_UPLOAD sub-function

		op[N]	length contents
		------  -----  --------
		 0x07      1	0x02 (sub-func id)
		 0x08	   1	upload id
*/

/* DATA sub-function

		This subfunction is the one that actually contains the upload data.  The data format is
		specified in the BEGIN_UPLOAD command.  Currently, only the TC_CUSTOM format is defined.

		op[N]	length contents
		------  -----  --------
		 0x07      1	0x0A (sub-func id)
		 0x08	   1	upload id
		 0x09	   1	reserved
		 0x0A	   1	Number of records
		 0x0B	  ...	Image records

		 The first image record is at op[11], subsequent records (if any) are simply concatenated to
		 the end of the previous record.

		 TCREC_BEGIN

			offset	length  contents
			------  ------  --------
			 0x00	   1	 0x00 (TCREC_BEGIN)
			 0x01     11	 Module name - asciiz
			 0x1c      1	 Module version	
			 0x1d	   1	 Module revision

		 TCREC_DATA

			offset	length  contents
			------  ------  --------
			 0x00	   1	 0x01 (TCREC_DATA)
			 0x01      4	 address
			 0x05	   4	 data length
			 0x09	 <data len> binary data bytes

		 TCREC_COUNT
									  
			offset	length  contents
			------  ------  --------
			 0x00	   1	 0x02 (TCREC_COUNT)
			 0x01      4	 count of data records in the entire upload

		 TCREC_END

			offset	length  contents
			------  ------  --------
			 0x00	   1	 0x03 (TCREC_END)
			 0x01      4	 start address

		 TCREC_EXT_ADDR

			offset	length  contents
			------  ------  --------
			 0x00	   1	 0x04 (TCREC_EXT_ADDR)
			 0x01      4	 extended linear address

*/

typedef enum
{
	TC_VSAVC_CL_TCREC_BEGIN =		0x00,
	TC_VSAVC_CL_TCREC_DATA =		0x01,
	TC_VSAVC_CL_TCREC_COUNT =		0x02,
	TC_VSAVC_CL_TCREC_END =			0x03,
	TC_VSAVC_CL_TCREC_EXT_ADDR = 	0x04
} TC_VSAVC_CL_TC_RECORD_TYPE ;


// Length of module name in the TCREC_BEGIN record 
#define	TC_VSAVC_CL_MODULE_NAME_LEN	10	// does not include the 0 terminator


/**************************************/
/* Tuner frequency command 

		offset	length contents
		------  -----  --------
		 0x07      1   control type
		 0x08      4   frequency
*/

typedef enum 
{
	TUNER_FREQ_DIRECT	= 0,	// direct selection of frequency
	TUNER_FREQ_UP		= 1,	// up
	TUNER_FREQ_DOWN		= 2		// down
} TC_VSAVC_TUNER_FREQ_CONTROL ;

typedef uint32		TC_VSAVC_FREQ_VALUE	;

#define	TC_VSAVC_FREQ_MIN_VALUE		0
#define	TC_VSAVC_FREQ_MAX_VALUE		999999	// maximum 6 decimal digits (2 implied decimal places)

/**************************************/
/* Tuner preset command 

		offset	length contents
		------  -----  --------
		 0x07      1   control type
		 0x08      1   preset number
*/

typedef enum 
{
	TUNER_PRESET_DIRECT	= 0,	// direct selection of preset
	TUNER_PRESET_UP		= 1,	// up
	TUNER_PRESET_DOWN	= 2		// down
} TC_VSAVC_TUNER_PRESET_CONTROL ;

typedef uint8	TC_VSAVC_TUNER_PRESET_VALUE ;

#define	TC_VSAVC_PRESET_MIN_VALUE		1
#define	TC_VSAVC_PRESET_MAX_VALUE		40

/**************************************/
/* Tuner scan mode command

		offset	length contents
		------  -----  --------
		 0x07      1   scan mode
*/

typedef enum 
{
	VS_TUNER_SCAN_AUTO		= 0,
	VS_TUNER_SCAN_MANUAL	= 1
} TC_VSAVC_TUNER_SCAN_MODE ;

/**************************************/
/* Tuner output mode command

		offset	length contents
		------  -----  --------
		 0x07      1   output mode
*/

typedef enum 
{
	VS_TUNER_OUTPUT_STERO	= 0,
	VS_TUNER_OUTPUT_MONO	= 1
} TC_VSAVC_TUNER_OUTPUT_MODE ;


#endif //_AVCUNITVENDORDEFS_H


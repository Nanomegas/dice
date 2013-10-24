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

#ifndef _TC_TYPES_H_
#define _TC_TYPES_H_

#define INLINE __inline__

#define _TC_INLINE inline extern
#define _TC_INLINE_ALWAYS __attribute__((always_inline))

#define _TC_INLINE_STATIC static inline


#define ATTRIBUTE_PACKED __attribute__((packed)) 

typedef unsigned char	uint8;
typedef signed char		int8;
typedef unsigned short	uint16;
typedef signed short	int16;

typedef unsigned long	uint32;
typedef signed long		int32;
typedef unsigned long	HRESULT;
// BOOLEAN
typedef	uint32	BOOL;

/* Define NULL pointer value */

#ifndef NULL
#ifdef  __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

/* Define FALSE and TRUE, if they have not already been defined */

#ifndef FALSE					
#define FALSE			0
#endif

#ifndef TRUE
#define	TRUE			(!FALSE)
#endif

#ifndef EVEN
#define EVEN			0
#endif
#ifndef ODD
#define ODD				1
#endif

#ifndef ON
#define ON				1
#endif
#ifndef OFF
#define OFF				0
#endif

// IEEE Standard types 
typedef uint8		BYTE;
typedef uint16		DOUBLET;
typedef uint32		QUADLET;
// OCTLET - doesn't map very well, array of bytes?

  
// Quadlet & double as unions
#ifdef _LITTLE_ENDIAN
typedef union
{
	QUADLET q;
	struct 
	{
		DOUBLET lsd, msd;
	}d;
	struct 
	{
		BYTE lsb, mlsb, mmsb, msb;
	} b;
	BYTE byte[4];
	DOUBLET doub[2];
} UQUAD;

typedef union
{
	DOUBLET d;
	struct 
	{
		BYTE lsb, msb;
	} b;
	BYTE byte[2];  /* never use a number here, always use a constant. */
} UDOUB;

#else

typedef union
{
	QUADLET q;
	struct 
	{
		DOUBLET msd, lsd;
	} d;
	struct 
	{
		BYTE msb, mmsb, mlsb, lsb;
	} b;
 BYTE byte[4];
 DOUBLET doub[2];
} UQUAD;

typedef union
{
	DOUBLET d;
	struct 
	{
		BYTE msb, lsb;
	} b;
	BYTE byte[2];  /* never use a number here, always use a constant. */
} UDOUB;
 
#endif
 
  

// Four quadlet data structure used for OHCI descriptores
typedef struct 
{
	QUADLET quad1;  
	QUADLET quad2;
	QUADLET quad3;
	QUADLET quad4;
} FOUR_QUAD;

//*************************************************************
//	Common macros

//	When an argument is unused, use this to keep the warning from being generated.
#define		UNUSED_ARG(x) (x=x)

// ARRAYSIZE - macro for number of elements in an array (as opposed to # of bytes)
#define ARRAYSIZE(a) (sizeof(a)/sizeof(a[0]))

// MIN & MAX macros
#define MIN(a,b)	(a<b ? a : b)
#define MAX(a,b)	(a>b ? a : b)

// Infinite Loop macro.  this is not while(TRUE) due to compiler nagging
#define DO_FOREVER	for (;;)

//*************************************************************
// bit values

#define BIT0					0x00000001
#define BIT1					0x00000002
#define BIT2					0x00000004
#define BIT3					0x00000008
#define BIT4					0x00000010
#define BIT5					0x00000020
#define BIT6					0x00000040
#define BIT7					0x00000080
#define BIT8					0x00000100
#define BIT9					0x00000200
#define BIT10					0x00000400
#define BIT11					0x00000800
#define BIT12					0x00001000
#define BIT13					0x00002000
#define BIT14					0x00004000
#define BIT15					0x00008000
#define BIT16					0x00010000
#define BIT17					0x00020000
#define BIT18					0x00040000
#define BIT19					0x00080000
#define BIT20					0x00100000
#define BIT21					0x00200000
#define BIT22					0x00400000
#define BIT23					0x00800000
#define BIT24					0x01000000
#define BIT25					0x02000000
#define BIT26					0x04000000
#define BIT27					0x08000000
#define BIT28					0x10000000
#define BIT29					0x20000000
#define BIT30					0x40000000
#define BIT31					0x80000000
#define BITALL					0xffffffff
#define BITNONE					0x00000000

#define ISBITSET(value,bit)		((value&bit)?TRUE:FALSE)


// DSP Specific global types, this might not be the right place for those, ML
#define X_MEM 0x02
#define Y_MEM 0x04
#define P_MEM 0x00


#endif

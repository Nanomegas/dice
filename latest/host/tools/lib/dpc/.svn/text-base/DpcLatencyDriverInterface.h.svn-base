///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004-2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __DPCLATENCYDRIVERINTERFACE_H__
#define __DPCLATENCYDRIVERINTERFACE_H__


#define FILE_DEVICE_DPCLATENCYCHECKER  0x8000


// Define Interface reference/dereference routines for
// Interfaces exported by IRP_MN_QUERY_INTERFACE

#define IOCTL_SET_DPC_TIMER \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SET_DPC_INDUCER \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GETDPC_LATENCY \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_SUGGESTED_TIMER_DURATION \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x803, METHOD_BUFFERED, FILE_ANY_ACCESS)


#define START_TIMER	1
#define STOP_TIMER	0


typedef struct _SET_DPC_LATENCY_CHECKER_STRUCT
{
	ULONG actionFlag;		// start or stop
	LONG checkerDuration;	// duration in MS for timer callback
}
SET_DPC_LATENCY_CHECKER_STRUCT, *PSET_DPC_LATENCY_CHECKER_STRUCT;


typedef struct _SET_DPC_LATENCY_INDUCER_STRUCT
{
	ULONG actionFlag;		// start or stop
	LONG inducerDuration;	// duration in MS for timer callback
	LONG stallDuration;		// Duration to stall the DPC in MS.
}
SET_DPC_LATENCY_INDUCER_STRUCT, *PSET_DPC_LATENCY_INDUCER_STRUCT;


#endif // __DPCLATENCYDRIVERINTERFACE_H__

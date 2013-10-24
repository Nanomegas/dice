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
	This file is just a page for doxygen and is not part of the
	source tree.

	This file defines the structure of the modules.  We do it here so that
	we can control the ordering.

****************************************************************************/

//////////////////////////

/** @defgroup Application Application Building Blocks
	A number of application building blocks have been provided in the Firmware.  These building
	blocks are at the application layer and can be easily incorporated into the application.
	@{
*/

/** @defgroup myApp myApp  Application entry point
*/

/** @defgroup myMode myMode  Configure Audio Interfaces and Streaming Modes
*/

/** @defgroup DAL DICE Abstraction Layer (DAL)
*/

/** @defgroup FCP Function Control Protocol (FCP)
*/

/** @defgroup AVC AV/C Handling
*/

/** @defgroup mLAN mLAN Support	
	OGT (previously known as mLAN) support will be provided in Firmware updates. The OGT module is provided as drop-in additions at the Project level in future updates. Contact TCAT for urther information.
*/

/** @defgroup DiceDriver DiceDriver Support
*/

/** @defgroup AvcDriver AvcDriver Support
*/

/** @defgroup FirmwareLoad Firmware Load over 1394
*/

//! @}

//////////////////////////

/** @defgroup Core Core Modules
	The core modules provide essential functionality for the application layers
	to utilize.
	@{
*/

/**	@defgroup LinkAbstractionLayer Link Abstraction Layer
	
*/

/**	@defgroup AVS AV System (AVS)
	
*/
	
/**	@defgroup CMP Connection Management Procedures (CMP)
	
*/

/**	@defgroup IRM Isochronous Resource Management (IRM)
	
*/

/**	@defgroup AML Abstraction MIDI Layer (AML)
	
*/

//!	@}

//////////////////////////////

/** @defgroup Programming Programming Utilities
	Various programming utilities are provided to simplify the development process.

	@{
*/

/**	@defgroup CLI Command Line Interpreter (CLI)
	
*/

/** @defgroup PacketBlocks Packet Blocks
	
*/

/** @defgroup MemoryPoolManager Memory Pool Manager (MPM)
	
*/

/** @defgroup LockHandler Lock Transaction Handler
*/

/** @defgroup KeyValue Key-Value Maps
*/

/** @defgroup DataStreams	Data Stream Parsing
*/

/** @defgroup KeyValue Key-Value Maps
*/

/** @defgroup I2C I2C Interface
*/

//!	@}

//////////////////////////

/** @defgroup RTOS Real-Time Operation System
	@{
*/

/** @defgroup RTOSTask Tasks / Threads
*/

/** @defgroup RTOSTime Timer Functions
*/

/** @defgroup RTOSSemaphore Semaphores
*/

/** @defgroup RTOSQueue Message Queues
*/

//! @}





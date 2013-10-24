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
//===========================================================================
/****************************************************************************
	
	Purpose:	DICE  Abstraction Layer API
	
	Revisions:
		created 01/03/2005 ml
		rewritten 21/03/2005 ml
****************************************************************************/
/** @addtogroup DAL

	Exposes an Audio configuration API for router, clock and stream management, etc.

	This module can be used from the command prompt by the @ref cli_dal commands.

	These are the most commonly used functions for Application development.

	@{
*/

#ifndef _DAL_H_
#define _DAL_H_

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "dice.h"
#include "diceAES.h"
#include "diceADAT.h"
#include "diceTDIF.h"
#include "avs.h"
#include "dalRouting.h"

/// DAL Interface ID's
typedef enum
{
	eDAL_INTERFACE_1 = 0,			//!< ...
	eDAL_INTERFACE_2,				//!< ...
	eDAL_NUM_INTERFACES
} DAL_INTERFACE_ID_ENUM;

/// Sample rate ranges
typedef enum {
	eDAL_RATE_MODE_LOW = 0,			//!< = 0, accept nominal rates from 32k to 48k
	eDAL_RATE_MODE_LOW_MID,			//!< accept nominal rates from 32k to 96k
	eDAL_RATE_MODE_MID,				//!< accept nominal rates from 88.2k to 96k
	eDAL_RATE_MODE_HIGH,			//!< accept nominal rates from 176.4k to 192k
	eDAL_RATE_MODE_SLAVE_LOW,		//!< slave to eDAL_INTERFACE_1 in low mode (only valid for eDAL_INTERFACE_1)
	eDAL_RATE_MODE_SLAVE_MID,		//!< slave to eDAL_INTERFACE_1 in mid mode (only valid for eDAL_INTERFACE_1)
	eDAL_RATE_MODE_SLAVE_HIGH,		//!< slave to eDAL_INTERFACE_1 in high mode (only valid for eDAL_INTERFACE_1)
	eDAL_RATE_MODE_COUNT
} DAL_RATE_MODE_ENUM;

/// Sample rate definitions
typedef enum {
	eDAL_NOMINAL_RATE_32 = 0,		//!< ...
	eDAL_NOMINAL_RATE_44_1,		    //!< ...
	eDAL_NOMINAL_RATE_48,			//!< ...
	eDAL_NOMINAL_RATE_88_2,			//!< ...
	eDAL_NOMINAL_RATE_96,		    //!< ...
	eDAL_NOMINAL_RATE_176_4,		//!< ...
	eDAL_NOMINAL_RATE_192,			//!< ...
	eDAL_NOMINAL_RATE_USER_1,		//!< uses the rate specified by the dalSetUserRate
	eDAL_NOMINAL_RATE_USER_2,		//!< ...
	eDAL_NOMINAL_RATE_USER_3,		//!< ...
	eDAL_NOMINAL_RATE_USER_4,		//!< ...
	eDAL_NOMINAL_RATE_USER_5,		//!< ...
	eDAL_NOMINAL_RATE_USER_6,		//!< ...
	eDAL_NOMINAL_RATE_USER_7,		//!< ...
	eDAL_NOMINAL_RATE_USER_8,		//!< ...
	eDAL_NOMINAL_RATE_ANY,			//!< any rate within the given rate mode, not valid for internal source
	eDAL_NOMINAL_RATE_NONE
} DAL_NOMINAL_RATE_ENUM;

/// Clock Source definitions
typedef enum {
	eDAL_CLK_SRC_AESRX = 0,			//!< use the first available AES receiver
	eDAL_CLK_SRC_AESRX0,			//!< 1fs clock from AES0 receiver
	eDAL_CLK_SRC_AESRX1,			//!< 1fs clock from AES1 receiver
	eDAL_CLK_SRC_AESRX2,			//!< 1fs clock from AES2 receiver
	eDAL_CLK_SRC_AESRX3,			//!< 1fs clock from AES3 receiver
	eDAL_CLK_SRC_ADATRX,			//!< 1fs clock from ADAT receiver
	eDAL_CLK_SRC_TDIFRX,			//!< 1fs clock from TDIF receiver
	eDAL_CLK_SRC_WORD,				//!< Word clock input
	eDAL_CLK_SRC_AVS_SYT1,			//!< 1394 syt clock 1
	eDAL_CLK_SRC_AVS_SYT2,			//!< 1394 syt clock 2
	eDAL_CLK_SRC_AVS_SYT3,			//!< 1394 syt clock 3
	eDAL_CLK_SRC_AVS_SYT4,			//!< 1394 syt clock 4
	eDAL_CLK_SRC_EXTERNAL,			//!< External baserate input
	eDAL_CLK_SRC_DSAI_RX0,			//!< DSAI rx0 sync input
	eDAL_CLK_SRC_DSAI_RX1,			//!< DSAI rx1 sync input
	eDAL_CLK_SRC_DSAI_RX2,			//!< DSAI rx2 sync input
	eDAL_CLK_SRC_DSAI_RX3,			//!< DSAI rx3 sync input
	eDAL_CLK_SRC_INTERNAL,			//!< Internal sync 
	eDAL_CLK_SRC_NONE				//!< No clock source selected
} DAL_CLOCK_SOURCE_ENUM;

/// States used for the clock state machine
typedef enum 
{
	eDAL_STATE_DISABLED = 0,		//!< The interface is disabled
	eDAL_STATE_SETUP_SRC,			//!< The interface is selecting a new source (transitional)
	eDAL_STATE_WAIT_SRC_LOCK,		//!< The interface is waiting for the source to be locked
	eDAL_STATE_WAIT_RATE_STABLE,	//!< The interface is waiting for the rate to be stable (transitional)
	eDAL_STATE_CHECK_RATE,			//!< The interface is checking if the rate is valid (transitional)
	eDAL_STATE_LOCKING,				//!< The interface is abut to enter the locked state
	eDAL_STATE_ILLEGAL_RATE,		//!< The detected rate is not valid under current configuration
	eDAL_STATE_RATE_IS_FLAKY,		//!< The rate is unstable
	eDAL_STATE_LOCKED				//!< The interface is locked
} DAL_STATE_ENUM;

/// Clock Lock Status
typedef struct
{
	DAL_STATE_ENUM			state;				//!< ...
	DAL_RATE_MODE_ENUM		lockedRateMode;		//!< updated in locked state
	DAL_NOMINAL_RATE_ENUM	lockedNominalRate;	//!< updated in locked state, should be used for DSP coeff calc. etc.
	uint32					lockedRateHz;		//!< updated in locked state
	uint32					badRateHz;			//!< when illegal rate detected, this is the rate
} DAL_STATUS; 

typedef struct
{
	DAL_CLOCK_SOURCE_ENUM	src;  //clock source selected
	BOOL					locked; 
	DAL_NOMINAL_RATE_ENUM	nomRate;
	uint8					adatUser;
}DAL_SYNC_SRC_INFO;
	
	
/// Clock Status
typedef struct
{
	DAL_RATE_MODE_ENUM      rateMode;			//!< ...
	DAL_CLOCK_SOURCE_ENUM	clockSource;		//!< current clock source selected
	DAL_NOMINAL_RATE_ENUM	sampleRate;			//!< current sample rate selected
	uint32					inputDevices;		//!< ...
	uint32					outputDevices;		//!< ...
} DAL_CONFIG; 


/** Definitions for the event call back system

  Event summary definitions
		Clock domain FSM events
*/
#define   DAL_CB_DOMAIN1				0x00000001	
#define   DAL_CB_DOMAIN2				0x00000010	

///	Receiver notifications
#define   DAL_CB_AES0					0x00010000
#define   DAL_CB_AES1					0x00020000
#define   DAL_CB_AES2					0x00040000
#define   DAL_CB_AES3					0x00080000
#define   DAL_CB_ADAT					0x00100000
#define   DAL_CB_TDIF					0x00200000
#define   DAL_CB_AVS1					0x00400000
#define   DAL_CB_AVS2					0x00800000
#define   DAL_CB_AVS3					0x01000000
#define   DAL_CB_AVS4					0x02000000
#define   DAL_CB_DSAI0					0x04000000
#define   DAL_CB_DSAI1					0x08000000
#define   DAL_CB_DSAI2					0x10000000
#define   DAL_CB_DSAI3					0x20000000

#define   DAL_CB_UNSUPPORTED_EVENTS		0x80000000

/** This type holds a bit mask of the bits defined above. This is to provide the
    callback with a summary of the events.
*/
typedef uint32              DAL_CB_EVENT_MASK;


/** Clock domain Extended notifications
*/
#define   DAL_CB_LOCK1_AQUIRED			0x00000001	// The interface has entered the locked state.
#define   DAL_CB_LOCK1_LOST				0x00000002	// The interface lost lock
#define   DAL_CB_RATE1_REJECTED			0x00000004
#define   DAL_CB_RATE1_NOM_CHANGED		0x00000008
#define   DAL_CB_RATE1_CHANGED			0x00000010
#define   DAL_CB_CREATED1				0x00000020
#define   DAL_CB_DESTROYED1				0x00000040
#define   DAL_CB_DOM1_MASK				0x000000FF

#define   DAL_CB_LOCK2_AQUIRED			0x00000100	// The interface has entered the locked state.
#define   DAL_CB_LOCK2_LOST				0x00000200	// The interface lost lock
#define   DAL_CB_RATE2_REJECTED			0x00000400
#define   DAL_CB_RATE2_NOM_CHANGED		0x00000800
#define   DAL_CB_RATE2_CHANGED			0x00001000
#define   DAL_CB_CREATED2				0x00002000
#define   DAL_CB_DESTROYED2				0x00004000
#define   DAL_CB_DOM2_MASK				0x0000FF00


/// AES Entended notifications
#define   DAL_CB_AES0_LOCK_ACQUIRED		EVT_AES1_LOCK_ACQUIRED
#define   DAL_CB_AES0_LOCK_LOST			EVT_AES1_LOCK_LOST
#define   DAL_CB_AES0_CRC				EVT_AES1_CRC
#define   DAL_CB_AES0_PARITY			EVT_AES1_PARITY
#define   DAL_CB_AES0_VALIDITY			EVT_AES1_VALIDITY
#define   DAL_CB_AES0_SLIP				EVT_AES1_SLIP
#define   DAL_CB_AES0_RPT				EVT_AES1_RPT
#define   DAL_CB_AES0_MASK				0x000000FF

#define   DAL_CB_AES1_LOCK_ACQUIRED		(EVT_AES1_LOCK_ACQUIRED<<8)
#define   DAL_CB_AES1_LOCK_LOST			(EVT_AES1_LOCK_LOST<<8)
#define   DAL_CB_AES1_CRC				(EVT_AES1_CRC<<8)
#define   DAL_CB_AES1_PARITY			(EVT_AES1_PARITY<<8)
#define   DAL_CB_AES1_VALIDITY			(EVT_AES1_VALIDITY<<8)
#define   DAL_CB_AES1_SLIP				(EVT_AES1_SLIP<<8)
#define   DAL_CB_AES1_RPT				(EVT_AES1_RPT<<8)
#define   DAL_CB_AES1_MASK				0x0000FF00

#define   DAL_CB_AES2_LOCK_ACQUIRED		(EVT_AES1_LOCK_ACQUIRED<<16)
#define   DAL_CB_AES2_LOCK_LOST			(EVT_AES1_LOCK_LOST<<16)
#define   DAL_CB_AES2_CRC				(EVT_AES1_CRC<<16)
#define   DAL_CB_AES2_PARITY			(EVT_AES1_PARITY<<16)
#define   DAL_CB_AES2_VALIDITY			(EVT_AES1_VALIDITY<<16)
#define   DAL_CB_AES2_SLIP				(EVT_AES1_SLIP<<16)
#define   DAL_CB_AES2_RPT				(EVT_AES1_RPT<<16)
#define   DAL_CB_AES2_MASK				0x00FF0000

#define   DAL_CB_AES3_LOCK_ACQUIRED		(EVT_AES1_LOCK_ACQUIRED<<24)
#define   DAL_CB_AES3_LOCK_LOST			(EVT_AES1_LOCK_LOST<<24)
#define   DAL_CB_AES3_CRC				(EVT_AES1_CRC<<24)
#define   DAL_CB_AES3_PARITY			(EVT_AES1_PARITY<<24)
#define   DAL_CB_AES3_VALIDITY			(EVT_AES1_VALIDITY<<24)
#define   DAL_CB_AES3_SLIP				(EVT_AES1_SLIP<<24)
#define   DAL_CB_AES3_RPT				(EVT_AES1_RPT<<24)
#define   DAL_CB_AES3_MASK				0xFF000000

///  ADAT Extended notifications
#define   DAL_CB_ADAT_SLIP				EVT_ADAT_SLIP			
#define   DAL_CB_ADAT_RPT				EVT_ADAT_RPT			
#define   DAL_CB_ADAT_LOCK_ACQUIRED		EVT_ADAT_LOCK_ACQUIRED
#define   DAL_CB_ADAT_LOCK_LOST			EVT_ADAT_LOCK_LOST	
#define   DAL_CB_ADAT_USR_CHANGE		EVT_ADAT_USR_CHANGE	


///  TDIF Extended notifications
#define   DAL_CB_TDIF_PARITY1			EVT_TDIF_PARITY1			
#define   DAL_CB_TDIF_PARITY2			EVT_TDIF_PARITY2			
#define   DAL_CB_TDIF_PARITY3			EVT_TDIF_PARITY3			
#define   DAL_CB_TDIF_PARITY4			EVT_TDIF_PARITY4			
#define   DAL_CB_TDIF_PARITY5			EVT_TDIF_PARITY5			
#define   DAL_CB_TDIF_PARITY6			EVT_TDIF_PARITY6			
#define   DAL_CB_TDIF_PARITY7			EVT_TDIF_PARITY7			
#define   DAL_CB_TDIF_PARITY8			EVT_TDIF_PARITY8			
#define   DAL_CB_TDIF_SLIP				EVT_TDIF_SLIP				
#define   DAL_CB_TDIF_RPT				EVT_TDIF_RPT				
#define   DAL_CB_TDIF_LOCK_ACQUIRED		EVT_TDIF_LOCK_ACQUIRED	
#define   DAL_CB_TDIF_LOCK_LOST			EVT_TDIF_LOCK_LOST		


///	AVS Extended notifications
#define   DAL_CB_AVS1_SLIP				EVT_AVS_SLIP			
#define   DAL_CB_AVS1_RPT				EVT_AVS_RPT			
#define   DAL_CB_AVS1_LOCK_ACQUIRED		EVT_AVS_LOCK_ACQUIRED	
#define   DAL_CB_AVS1_LOCK_LOST			EVT_AVS_LOCK_LOST		
#define   DAL_CB_AVS1_ENABLE			EVT_AVS_ENABLE		
#define   DAL_CB_AVS1_DISABLE			EVT_AVS_DISABLE		
#define   DAL_CB_AVS1_MASK				0x000000FF


#define   DAL_CB_AVS2_SLIP				(EVT_AVS_SLIP << 8)
#define   DAL_CB_AVS2_RPT				(EVT_AVS_RPT << 8)
#define   DAL_CB_AVS2_LOCK_ACQUIRED		(EVT_AVS_LOCK_ACQUIRED << 8)
#define   DAL_CB_AVS2_LOCK_LOST			(EVT_AVS_LOCK_LOST << 8)
#define   DAL_CB_AVS2_ENABLE			(EVT_AVS_ENABLE << 8)
#define   DAL_CB_AVS2_DISABLE			(EVT_AVS_DISABLE << 8)	
#define   DAL_CB_AVS2_MASK				0x0000FF00

#define   DAL_CB_AVS3_SLIP				(EVT_AVS_SLIP << 16)
#define   DAL_CB_AVS3_RPT				(EVT_AVS_RPT << 16)
#define   DAL_CB_AVS3_LOCK_ACQUIRED		(EVT_AVS_LOCK_ACQUIRED << 16)
#define   DAL_CB_AVS3_LOCK_LOST			(EVT_AVS_LOCK_LOST << 16)
#define   DAL_CB_AVS3_ENABLE			(EVT_AVS_ENABLE << 16)
#define   DAL_CB_AVS3_DISABLE			(EVT_AVS_DISABLE << 16)	
#define   DAL_CB_AVS3_MASK				0x00FF0000

#define   DAL_CB_AVS4_SLIP				(EVT_AVS_SLIP << 24)
#define   DAL_CB_AVS4_RPT				(EVT_AVS_RPT << 24)
#define   DAL_CB_AVS4_LOCK_ACQUIRED		(EVT_AVS_LOCK_ACQUIRED << 24)
#define   DAL_CB_AVS4_LOCK_LOST			(EVT_AVS_LOCK_LOST << 24)
#define   DAL_CB_AVS4_ENABLE			(EVT_AVS_ENABLE << 24)
#define   DAL_CB_AVS4_DISABLE			(EVT_AVS_DISABLE << 24)	
#define   DAL_CB_AVS4_MASK				0xFF000000

///	DSAI Extended notifications
#define   DAL_CB_DSAI0_SLIP				0x00000001
#define   DAL_CB_DSAI0_RPT				0x00000002
#define   DAL_CB_DSAI0_MASK				0x0000000F

#define   DAL_CB_DSAI1_SLIP				0x00000010
#define   DAL_CB_DSAI1_RPT				0x00000020
#define   DAL_CB_DSAI1_MASK				0x000000F0

#define   DAL_CB_DSAI2_SLIP				0x00000100
#define   DAL_CB_DSAI2_RPT				0x00000200
#define   DAL_CB_DSAI2_MASK				0x00000F00

#define   DAL_CB_DSAI3_SLIP				0x00001000
#define   DAL_CB_DSAI3_RPT				0x00002000
#define   DAL_CB_DSAI3_MASK				0x0000F000

#define   DAL_CB_MAX_NUM               32     

/// DAL Events
typedef struct
{
	uint32 fsm_events;		//!< ...
	uint32 aes_events;		//!< ...
	uint32 adat_events;		//!< ...
	uint32 tdif_events;		//!< ...
	uint32 avs_events;		//!< ...
	uint32 dsai_events;		//!< ...
} DAL_EVENTS;


typedef HRESULT (*DAL_CALLBACK_FUNC)(DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data);

typedef struct tDAL_CALLBACK* DAL_CALLBACK_PTR;


/// Routing Entry Descriptions
typedef	union 
{
	uint16 entry;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned type		      : 2;	//!< Defines entry type (channel/8block/2channel/4block)
		unsigned moduleId		  : 6;	//!< Module Id
		unsigned channel		  : 6;	//!< Channel
		unsigned      		    : 2;	//!< Not used
	} bit_fields;
#else
	struct
	{
		unsigned      		    : 2;	//!< Not used
		unsigned channel		  : 6;	//!< Channel
		unsigned moduleId		  : 6;	//!< Module Id
		unsigned type		      : 2;	//!< Defines entry type (channel/8block/2channel/4block)
	} bit_fields;
#endif
} SRC_DESCRIPTION, DEST_DESCRIPTION;

/// Routing Entry
typedef struct tDAL_ROUTING_ENTRY
{
	DEST_DESCRIPTION          dest_description;  //!< 16 bits
	SRC_DESCRIPTION           src_description;   //!< 16 bits
} DAL_ROUTING_ENTRY;




#define DEVICE_BIT( dev )   (0x1 << dev)


/** 
//  Creates initial configuration of DiceII router based on selected physical devices.
//  All devices belonging to the interface will be muted. The routing will initialize 
//  to empty implying that all destinations will source from the system muted source.
//  If there is a conflict with the other domain this function will return an error. The
//  proper way to move member ship between domains is to call dalDestroyInterface on both
//  interfaces and then create them accordingly.
//  
//  If the interface has been created, dalDestroyInterface will be called first.
//
//  The only way to change the rate mode of an interface is to call this function. Changing
//  rate mode might change the channel configuration of the DICE I/O system and in some cases
//  the channel configuration of external devices. The following rate modes are available:
//	eDAL_RATE_MODE_LOW		// accept nominal rates from 32k to 48k
//	eDAL_RATE_MODE_LOW_MID	// accept nominal rates from 32k to 96k
//	eDAL_RATE_MODE_MID		// accept nominal rates from 88.2k to 96k
//	eDAL_RATE_MODE_HIGH		// accept nominal rates from 176.4k to 192k
//
//	The channel membership is a bit mask for the input devices and the output devices. The
//	mask can be created uding the macro DEVICE_BIT( dev ), the following example creates a mask
//  including the AES and ADAT receiver: DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX)
//
//	Typical interface initialization sequence:
//		dalDestroyInterface (interfaceId);
//      dalCreateInterface (interfaceId, eDAL_RATE_MODE_LOW, <input devices>, <output devices>);
//      dalSetRoute and or dalSetRoutes
//      dalSetClockSource (interfaceId, eDAL_CLK_SRC_INTERNAL, eDAL_NOMINAL_RATE_48);
//		dalStartInterface (interfaceId);
//
//  After the interface is started calls to dalSetRoute, dalSetRoutes and dalSetClockSource will have
//  immidiate effect.
//  
//  @param dalInterfaceId  - abstracts the concept of the DiceII router, 0/1 selects Router/Sync 1 or 2 respectively
//  @param rateMode        - selects the rate mode for the interface, DAL_RATE_MODE_ENUM 
//  @param inputDevices    - selects the input device membership for this interface
//  @param outputDevice    - selects the output device membership for this interface
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
//  @return E_DAL_IODEVICE_CONFLICT			- the IO device is used by the other interface
//  
*/
HRESULT dalCreateInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_RATE_MODE_ENUM rateMode, uint32 inputDevices, uint32 outputDevices);

/** 
//  Destroy the specified interface if it has been created. This involves muting all
//  Rx and Tx devices belonging to this interface, freeing all resources and clearing 
//  the router and device membership. This will also put the clock state machine into
//  disabled state.
//  This function is automatically called by dalCreateInterface before creating a new
//  configuration.
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
*/
HRESULT dalDestroyInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId);

/**
//  After an interface has been created with dalCreateInterface all IO devices will
//  be muted and the clock state machine will be put in disabled state 
//  This function starts the clock state machine and will unmute the IO when lock
//  is obtained.
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
//  @return E_DAL_INTERFACE_NOT_CREATED     - dalCreateInterface was not called
*/
HRESULT dalStartInterface(DAL_INTERFACE_ID_ENUM dalInterfaceId);

/**
//  This function can be called on any interface that has been created. If the interface
//  is started this will have immidiate effect if not it will have effect when the interface 
//	is started. 
//  If the clockSource parameter specifies an internal rate the nominalRate must be specified,
//  if an external source is selected eDAL_NOMINAL_RATE_ANY indicates that any rate within the
//  selected rate mode will be accepted. If a specific nominal rate is specified only rates which
//  fall within that nominal rates window will be accepted. Different mechanisms are used to 
//  detect the nominal rate depending on the interface.
//	AES		The detected rate is based on measuring the clock.
//	ADAT	The detected rate is based on measuring the clock and inspecting the
//			SMUX status. SMUX can either be set to auto detect (using user bit) or user selected
//			based on dalSetAdatSmuxMode.
//	TDIF	The detected rate is based on measuring the clock.
//	WC		The detected rate is based on measuring the clock and inspecting the WC mode. WC mode
//			can either be set to base rate or actual rate based on dalSetWcInMode.
//	AVS		The detected rate is based on measuring the clock and inspecting the SFC field in the
//			CIP1 header.
//	EXT		The detected rate is based on measuring the clock, this is illegal with eDAL_RATE_MODE_LOW_MID
//			As there is no way of determining whether the system should run low or mid.
//	DSAI	The detected rate is based on measuring the clock
//
//	The actual and nominal clock will always be calculated and will be available through various functions.
//  If a clock is not within the legal rate of the rateMode and nominalRate setting a notification will be
//	posted once and the clock state machine will enter a state waiting for the condition to change.
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//	@param clockSource		- sets the source to be used for clock. DAL_CLOCK_SOURCE_ENUM
//	@param nominalRate		- sets the nominal rate to be used. DAL_NOMINAL_RATE_ENUM
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
//  @return E_DAL_INTERFACE_NOT_CREATED     - dalCreateInterface was not called
*/
HRESULT dalSetClockSource (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_CLOCK_SOURCE_ENUM clockSource, DAL_NOMINAL_RATE_ENUM nominalRate);


/**
//  Add a route to the destination of the interface. The source and 
//  destination must be members of the interface. The source and destination
//	should each be one of the predefined RX_ or TX_ macros defined in dalRoutes.h. 
//	Not all routings are possible, there are some restrictions on the upper 8 channels
//  when the AVS transmitters are used as destination.
//
//  The same source can be routed to multiple destinations. For example:
//	all ADAT -> all AES
//		dalSetRoute (dalInterface, TX_AES_CH1_8, RX_ADAT_CH1_8); 
//	all ADAT -> all ADAT
//		dalSetRoute (dalInterface, TX_ADAT_CH1_8, RX_ADAT_CH1_8);
//
//  Later a route to an interface can be changed:
//		AVS1 -> all AES
//		dalSetRoute (dalInterface, TX_AES_CH1_8, RX_AVS1_CH1_8);
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//  @param dst             - the destination to route audio to. (see dalRoutes.h)
//  @param src             - the source to route audio from. (see dalRoutes.h)
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
//  @return E_DAL_INVALID_ROUTE				- the route is conflicting with the configuration
//  @return E_DAL_INTERFACE_NOT_CREATED     - dalCreateInterface was not called
*/
HRESULT dalSetRoute(DAL_INTERFACE_ID_ENUM dalInterfaceId, DEST_DESCRIPTION dst, SRC_DESCRIPTION src);

/**
//  Add several routes to the interface. See dalSetRoute.
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//  @param pRouting        - Points to an array of routing entries, defining the desired routing.
//  @param numEntries      - indicates the number of entries to apply.
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
//  @return E_DAL_INVALID_ROUTE				- the route is conflicting with the configuration
//  @return E_DAL_INTERFACE_NOT_CREATED     - dalCreateInterface was not called
*/
HRESULT dalSetRoutes(DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_ROUTING_ENTRY *pRouting, uint8 numEntries);


/**
//  Get the current status of the interface. This includes the FSM state
//  and the detected sample rate. The struct returned contains the following:
//	state;
//	lockedRateMode
//		The ratemode obtained when locked, this can only be eDAL_RATE_MODE_LOW
//		eDAL_RATE_MODE_MID or eDAL_RATE_MODE_HIGH
//	lockedNominalRate
//		The nominal rate obtained when locked, this rate is never NONE or ANY.
//	lockedRateHz
//		The actual rate obtained when locked in Hz.
//	badRateHz
//		When the state machine enters eDAL_STATE_ILLEGAL_RATE this field will
//		contain the rate illegal rate. This is for diagnostics use
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//  @param pStatus         - pointer to struct to receive the status information
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id
//
*/
HRESULT dalGetCurrentStatus  (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_STATUS * pStatus);


/**
//  Get the locked state of the interface. This is an easy way of comparing
//  the current state to eDAL_STATE_LOCKED.
//	If an illegal interface is issued FALSE will be returned.
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//  @return TRUE			- if interface is locked
//  @return FALSE			- if interface is not locked, not created or invalid interface id issued
//
*/
BOOL dalIsLocked (DAL_INTERFACE_ID_ENUM dalInterfaceId);

/** 
//  Get the current configuration of the interface. 
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//  @param pConfig         - pointer to struct to receive the configuration
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id
//
*/
HRESULT dalGetCurrentConfiguration  (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_CONFIG * pConfig);



/**
// Install/Uninstall callbacks. 
//
//  Installs a function for notification by system on events
//  Note - when a callback is issued, the eventMask which is passed to the function
//  will hold *all* accumulated events since last called.
//  There are 6 groups of events covering the state machine for the clock domains and
//  the individual receiver interfaces. The callback function will receive the event
//  summary in the 'events' parameter and the specific extended events in the structure
//	pointed to by pExtEvents.
//
//	The same callback can be installed several times. This will allow different holdoff for
//  different events. The same function should not be installed more then once for one event
//  as this would result in the event being notified more than once.
//
//  @param pEventMask		- specifies, by way of bits set, which events should trigger notification.
//							The members of this structure contains bits for each of the different 
//							events which can be issued.
//  @param holdoff_ms      - specifies a time in approx. ms (+/- a 'system tick') that should pass, 
//							as a minimum, between consecutive notifications. If set to zero, notifications 
//							are issued at the rate of the system. 
//  @param func            - points to callback function
//  @param data            - data to be passed to callback function on any notification
// 
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id
//  @return E_DAL_TOO_MANY_CALLBACKS		- too many callbacks installed, max is DAL_CB_MAX_NUM
//	@return E_DAL_INVALID_CALLBACK			- invalid function pointer passed
//
*/
HRESULT dalInstallCallback  (const DAL_EVENTS * pEventMask, uint32 holdoff_ms, DAL_CALLBACK_FUNC func, uint32 data);

/** Uninstalls a function for notification by system on events
//  @param func								- points to callback function used in dalInstallCallBack
// 
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id
*/
HRESULT dalUninstallCallback(DAL_CALLBACK_FUNC func);



/**
//  Sets global mute on or off. If global mute is on, the router and all it's rx'ers and tx'ers will be muted
//
//  @param dalInterfaceId  - abstracts the concept of the DiceII router
//                    0/1 selects Router/Sync 1 or 2 respectively
//  @param mute           - mute on/off
//
//  @return NO_ERROR                        - no errors occured
//  @return E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
*/
HRESULT dalGlobalMute   (DAL_INTERFACE_ID_ENUM dalInterfaceId, BOOL mute);


/** Set JetPLL Configuration
*/
HRESULT dalSetConfig (uint32 msWaitStable, uint32 msWaitLock, BOOL bPLLoffWhenNoSource);

HRESULT dalResumeTasks(void);
HRESULT dalSuspendTasks(void);

HRESULT dalInitialize(void);
HRESULT dalInitializeTasks(void);

//
//	The system allows for a number of custom rates to be defined. Those rates can be used when generating
//	internal clock, but they can also be used by the detection mechanism when locking to external clocks.
//	A user specified rate must specify the closest system nominal rate for indication in the various interface
//	meta data streams.
//  


//dalInstallUserRate (uint8 UserID, RATE_DESC * pRateDesc);

//Get info about the state of the selected sync source
HRESULT dalGetSyncSrcInfo (DAL_INTERFACE_ID_ENUM dalInterfaceId, DAL_SYNC_SRC_INFO * pInfo);



#if defined(_SYSDEBUG) || defined (_CLI) 
extern const char * dalStateNames[eDAL_STATE_LOCKED+1];
#endif // _SYSDEBUG

//! @}

//********* Debug function to determine the AVS phase offset **************
//This function does only operate on eDAL_INTERFACE_1
//Rx id is either 0 or 1 for AVS1 or AVS2

HRESULT dalGetAVSPhaseOffset (uint32 rxId, int32 * pPhase10us, int8 * pPhasePercent);

//

HRESULT dalSkewRates (int32 tenthPercent);

#endif  //_DAL_H_

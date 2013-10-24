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

#ifndef _1394_ERROR_CODES_H_
#define _1394_ERROR_CODES_H_

/*********************** error bases ***********************************/
#ifndef NO_ERROR
#define NO_ERROR		0x0
#endif

#define E_LAL			0xA0000000
#define E_LHL			0xA1000000
#define E_NCI			0xA2000000
#define E_BRI			0xA3000000

#define E_PB			0xA4000000
#define E_OHCI			0xA5000000
#define E_MPM			0xA6000000
#define E_KEYVALUE		0xA7000000
#define E_LM			0xA7100000
#define E_CLI			0xA8000000
#define E_SIO			0xA9000000

#define E_TXPPCORE		0xAB000000
#define E_TEST_PCORE	0xAB100000
#define E_DEBUGPCORE	0xAB200000
#define E_LOCK			0xAC000000
#define E_IRM			0xAD000000
#define E_CMP			0xAE000000
#define E_AVC			0xAF000000
#define E_DRD			0xAF100000
#define E_EDS			0xAF200000
#define E_SBM			0xAF210000
#define E_DRS			0xAF220000
#define E_DSS			0xAF230000
#define E_AVS			0xAF300000
#define E_DTCP			0xAF400000
#define E_AKE			0xAF500000

#define E_TASKING		0xB1000000
#define E_SEMAPHORE		0xB1100000
#define E_MSGQUEUE		0xB1200000
#define E_COMMON		0xB0000000

/************************ error codes **********************************/

/**************************** LAL **************************************/
#define E_LAL_ALREADY_INITIALIZED			(E_LAL + 1)
#define	E_LAL_UNMAPPED_ERROR				(E_LAL + 2)
#define E_LAL_TOO_MANY_CALLBACKS			(E_LAL + 3)
#define E_LAL_GENERIC_FAILURE				(E_LAL + 4)
#define E_LAL_INVALID_REQUEST				(E_LAL + 5)
#define E_LAL_WWWUID_INVALID				(E_LAL + 6)
#define E_LAL_WWWUID_NOTFOUND				(E_LAL + 7)
#define E_LAL_HANDLE_INVALID				(E_LAL + 8)
#define E_LAL_HANDLE_NOTOPEN				(E_LAL + 9)
#define E_LAL_SPEEDMAP_ERROR				(E_LAL + 10)
#define E_LAL_NUM_DESTINATIONS_ERROR		(E_LAL + 11)
#define E_LAL_RESPONSE_UNEXPECTED			(E_LAL + 12)
#define E_LAL_RESPONSE_CONFLICT				(E_LAL + 13)
#define E_LAL_RESPONSE_DATAERR				(E_LAL + 14)
#define E_LAL_TYPEERR						(E_LAL + 15)
#define E_LAL_RESPONSE_ADDRERR				(E_LAL + 16)
#define E_LAL_RESPONSE_ZERO_DATA			(E_LAL + 17)
#define E_LAL_RESPONSE_TIMEOUT				(E_LAL + 18)
#define E_LAL_REQUEST_TIMEOUT				(E_LAL + 19)
#define E_LAL_LOCK_COMPARE_SWAP_FAIL		(E_LAL + 20)
#define E_LAL_ISOALLOCRES_MEM_FAIL			(E_LAL + 21)
#define E_LAL_SPEED_NOT_AVAIL				(E_LAL + 22)
#define E_LAL_INVALID_ADDR_RNG				(E_LAL + 23)
#define E_LAL_INTERNAL_DEV_OPEN				(E_LAL + 24)
#define E_LAL_INTERNAL_DEV_ADDR				(E_LAL + 25)
#define E_LAL_INTERNAL_CSR_INIT				(E_LAL + 26)
#define E_LAL_INTERNAL_CSC_SPACE			(E_LAL + 27)
#define E_LAL_BANDWIDTH_INVALID				(E_LAL + 28)
#define E_LAL_BANDWIDTH_UNAVAIL				(E_LAL + 29)
#define E_LAL_CHANNEL_INVALID				(E_LAL + 30)
#define E_LAL_CHANNEL_UNAVAIL				(E_LAL + 31)
#define E_LAL_ISO_PCL_MEMORY_FAIL			(E_LAL + 32)
#define E_LAL_ISO_SCATTERLOCK_FAIL			(E_LAL + 33)
#define E_LAL_ISO_RESOURCE_INVALID			(E_LAL + 34)
#define E_LAL_DMA_CHAN_NOT_FOUND			(E_LAL + 35)
#define E_LAL_TRANSMISSION_FAILURE			(E_LAL + 36)
#define E_LAL_BUS_RESET_IN_PROGRESS			(E_LAL + 37)
#define E_LAL_INTERFACE_NOT_INITIALIZED 	(E_LAL + 38)
#define E_LAL_OHCI_COULDNOT_CREATE_WINDOW	(E_LAL + 39)
#define E_LAL_ADDR_RNG_NOT_FOUND			(E_LAL + 40)
#define E_LAL_ADDR_RNG_CONFLICT				(E_LAL + 41)
#define E_LAL_INVALID_PB					(E_LAL + 42)
#define E_LAL_UNKNOWN_MAX_REC				(E_LAL + 43)

/**************************** LHL **************************************/
#define E_LHL_MODULE_INIT_ERROR				(E_LHL + 1)
#define E_LHL_SELF_ID_DATA_ERROR			(E_LHL + 3)
#define E_LHL_SELF_ID_INVERSE_ERROR			(E_LHL + 4)
#define E_LHL_SELF_ID_TOO_MANY				(E_LHL + 5)
#define E_LHL_SELF_ID_ACK_ERROR				(E_LHL + 6)
#define E_LHL_SELF_ID_NONE					(E_LHL + 7)
#define E_LHL_SELF_ID_TIMEOUT				(E_LHL + 8)
#define E_LHL_SELF_ID_INTERRUPTED			(E_LHL + 9)
#define E_LHL_SELF_ID_CYCLESTART_IN_SID		(E_LHL + 10)
#define E_LHL_DATA_NOT_SELFID				(E_LHL + 11)
#define E_LHL_UNEXPECTED_SID_DATA			(E_LHL + 12)
#define E_LHL_CYMAS_NODEISROOT				(E_LHL + 13)
#define E_LHL_CYMAS_NODENOTROOT				(E_LHL + 14)
#define E_LHL_CYMAS_INVALID_ARG				(E_LHL + 15)
#define E_LHL_CABLE_LOOP_DETECTED			(E_LHL + 16)
#define E_LHL_NODEID_NOT_AVAIL				(E_LHL + 17)
#define E_LHL_PHY_ADDR_ERROR				(E_LHL + 18)
#define E_LHL_PHY_POLL_TIMEOUT				(E_LHL + 19)
#define E_LHL_PHY_IO_ERROR					(E_LHL + 20)
#define E_LHL_RX_EVENT_GROUP_ERROR			(E_LHL + 21)
#define E_LHL_TX_EVENT_GROUP_ERROR			(E_LHL + 22)
#define E_LHL_SEMAPHORE_CREATE_ERROR		(E_LHL + 23)
#define E_LHL_Q_CREATE_ERROR				(E_LHL + 24)
#define E_LHL_WD_CREATE_ERROR				(E_LHL + 25)
#define E_LHL_UNSUPPORTED_TCODE				(E_LHL + 26)
#define E_LHL_RREQ_RD_ERROR					(E_LHL + 27)
#define E_LHL_RRSP_RD_ERROR					(E_LHL + 28)
#define E_LHL_NO_REQ_DATA					(E_LHL + 29)
#define E_LHL_NO_RSP_DATA					(E_LHL + 30)
#define E_LHL_PACKET_RX_ERROR				(E_LHL + 31)
#define E_LHL_INVALID_REQ_ADDR				(E_LHL + 32)
#define E_LHL_INVALID_RSP_ADDR				(E_LHL + 33)
#define E_LHL_PACKET_TOO_LARGE				(E_LHL + 34)
#define E_LHL_INVALID_SIZE					(E_LHL + 35)
#define E_LHL_PB_CREATE_ERROR				(E_LHL + 36)
#define E_LHL_INVALID_PB					(E_LHL + 37)
#define E_LHL_TX_REQQ_XFER_ERROR			(E_LHL + 38)
#define E_LHL_TX_RSPQ_XFER_ERROR			(E_LHL + 39)
#define E_LHL_TX_INVALID_HEADER				(E_LHL + 40)
#define E_LHL_TX_INVALID_PAYLOAD			(E_LHL + 41)
#define E_LHL_TX_INVALID_PACKET				(E_LHL + 42)
#define E_LHL_TX_PACKET_AGED_OUT			(E_LHL + 43)
#define E_LHL_UNKNOWN_RX_EVENT				(E_LHL + 44)
#define E_LHL_INVALID_Q						(E_LHL + 45)
#define E_LHL_UNKNOWN_CONF					(E_LHL + 46)
#define E_LHL_TLABEL_MISMATCH				(E_LHL + 47)
#define E_LHL_OUT_OF_PENDING_REQS			(E_LHL + 48)
#define E_LHL_PENDING_REQ_NOT_FOUND			(E_LHL + 49)
#define E_LHL_PENDING_FOUND_CONTINUE		(E_LHL + 50)
#define E_LHL_NODE_NOT_FOUND				(E_LHL + 51)
#define E_LHL_ASYNC_READ_FAILURE			(E_LHL + 52)
#define E_LHL_ASYNC_WRITE_FAILURE			(E_LHL + 53)
#define E_LHL_ASYNC_LOCK_FAILURE			(E_LHL + 54)
#define E_LHL_ASYNC_STREAM_FAILURE			(E_LHL + 55)
#define E_LHL_UNSUPPORTED_LOCK_TYPE			(E_LHL + 56)
#define E_LHL_ASYNC_READ_RSP_FAILURE		(E_LHL + 57)
#define E_LHL_ASYNC_WRITE_RSP_FAILURE		(E_LHL + 58)
#define E_LHL_ASYNC_LOCK_RSP_FAILURE		(E_LHL + 59)
#define E_LHL_ASYNC_ERROR_RSP_FAILURE		(E_LHL + 60)
#define E_LHL_INVALID_BROADCAST				(E_LHL + 61)  
#define	E_LHL_AUTH_EVENT_GROUP_ERROR		(E_LHL + 62)
#define	E_LHL_AUTH_HW_TIMEOUT				(E_LHL + 63)
#define	E_LHL_AUTH_INVALID_SIGNATURE		(E_LHL + 64)
#define	E_LHL_AUTH_ILLEGAL_SIGNATURE		(E_LHL + 65)
#define	E_LHL_AUTH_RETRYSIGN				(E_LHL + 66)
#define	E_LHL_PENDING_TX_INTERRUPTED		(E_LHL + 67)
#define E_LHL_BUS_RESET_IN_PROGRESS			(E_LHL + 68)
#define E_LHL_WRONG_PACKET_TYPE				(E_LHL + 69)
#define E_LHL_STATUSINFO_UNDEF				(E_LHL + 70)

																	// refer to "1394 Specification for Response Code Usage Rev 1.0"
#define E_LHL_REQ_BASE						(E_LHL + 0x100)			// 0: (Request Errors) //LM???

																	// refer to "1394 Specification for Response Code Usage Rev 1.0"
#define E_LHL_RSP_BASE						(E_LHL + 0x200)			// 0: resp_complete (Response Code Encoding)
#define E_LHL_RSP_RES1						(E_LHL_RSP_BASE + 1)	// 1: reserved 
#define E_LHL_RSP_RES2						(E_LHL_RSP_BASE + 2)	// 2: reserved
#define E_LHL_RSP_RES3						(E_LHL_RSP_BASE + 3)	// 3: reserved
#define E_LHL_RSP_CONFLICT					(E_LHL_RSP_BASE + 4)	// 4: resp_conflict_error
#define E_LHL_RSP_DATAERR					(E_LHL_RSP_BASE + 5)	// 5: resp_data_error
#define E_LHL_RSP_TYPEERR					(E_LHL_RSP_BASE + 6)	// 6: resp_type_error
#define E_LHL_RSP_ADDRERR					(E_LHL_RSP_BASE + 7)	// 7: resp_address_error

#define E_LHL_CONF_BASE						(E_LHL + 0x300)			// 
																	// DMA ContextControl.event register values
																	// OHCI_EVTMASK = 0x1F

/**************************** NCI **************************************/
#define E_NCI_PACKET_NOT_SELFID				(E_NCI + 1)
#define E_NCI_ID_SEQ_ERROR_0				(E_NCI + 2)
#define E_NCI_PACKET0_FORMAT_ERROR			(E_NCI + 3)
#define E_NCI_EXT_SELFID_ERROR				(E_NCI + 4)
#define E_NCI_NODE_SEQ_ERROR				(E_NCI + 5)
#define E_NCI_NO_CONTENDERS					(E_NCI + 6)
#define E_NCI_NODE_COUNT_ERROR				(E_NCI + 7)
#define E_NCI_ROOT_HAS_PARENTS				(E_NCI + 8)
#define E_NCI_NODE0_HAS_KIDS				(E_NCI + 9)
#define E_NCI_TOO_MANY_PARENTS				(E_NCI + 10)
#define E_NCI_BAD_TOPOLOGY_MAP				(E_NCI + 11)
#define E_NCI_INVALID_NODE_TREE				(E_NCI + 12)
#define E_NCI_ZERO_INFO_LENGTH				(E_NCI + 13)
#define E_NCI_NODE_ID_OUT_OF_RANGE			(E_NCI + 14)
#define E_NCI_NODE_INFO_NOT_AVAILABLE		(E_NCI + 15)
#define E_NCI_SID_INFO_NOT_AVAILABLE		(E_NCI + 16)
#define E_NCI_BUS_INFO_NOT_AVAILABLE		(E_NCI + 17)
#define E_NCI_CREATE_HANDLE_FAILED			(E_NCI + 18)
#define E_NCI_CLOSE_HANDLE_FAILED			(E_NCI + 19)
#define E_NCI_HANDLE_INVALID_ENTRY			(E_NCI + 20)
#define E_NCI_HANDLE_NOT_FOUND				(E_NCI + 21)
#define E_NCI_OUT_OF_HANDLES				(E_NCI + 22)
#define E_NCI_INVALID_SPEED					(E_NCI + 23)
#define E_NCI_BAD_SPEED_MAP					(E_NCI + 24)

/**************************** BRI **************************************/
#define	E_BRI_NOT_INITIALIZED_FATAL			(E_BRI + 1)		// function called before init
#define	E_BRI_NOT_INITIALIZED_OK			(E_BRI + 2)		// function called out of order, init will be called
#define E_BRI_ALREADY_INITIALIZED			(E_BRI + 3)		// duplicate init
#define	E_BRI_TOO_MANY_CBS					(E_BRI + 4)		// callback list full
#define	E_BRI_TIMEOUT						(E_BRI + 5)		// timed out
#define E_BRI_TIMEOUT_RESET_STORM			(E_BRI + 6)		// bus reset storm happening
#define E_BRI_NEW_BUS_RESET					(E_BRI + 7)

/***************************** PB **************************************/
#define E_PB_BAD_PACKET_BLOCK_PTR			(E_PB + 1)		// packet block pointer does not appear to point at a packet block
#define E_PB_TRAP_LIST_FULL					(E_PB + 2)		// pbAddDebugTrap,  no room to add another trap
#define E_PB_PAYLOAD_TOO_LARGE				(E_PB + 3)		// createPacket - requested payload size is too big
#define E_PB_DIFFERENT_PAYLOAD_SIZES		(E_PB + 4)		// duplicatePayload - payloads are not the same size
#define E_PB_INVALID_SPEED_CODE				(E_PB + 5)		// speed out of range for src link or signaling path
#define E_PB_TR_INVALID_HANDLE				(E_PB + 6)		// destination node is no longer valid
#define E_PB_TR_TIMEOUT						(E_PB + 7)		// packet did not transfer to destination node
#define E_PB_TR_ACK_ERROR					(E_PB + 8)		// packet acknowledge from destinaiton node is corrupted
#define E_PB_TR_ACK_PENDING					(E_PB + 9)		// packet confirmed with ack pending
#define E_PB_INVALID_Q_SPECIFIED			(E_PB + 10)		// packet q type out of range

/*************************** OHCI **************************************/
#define E_OHCI_DMA_ALLOCATE_DESC_ERR		(E_OHCI+1)
#define E_OHCI_DMA_RELEASE_DESC_ERR			(E_OHCI+2)
#define E_OHCI_DMA_ALLOCATE_DATA_ERR		(E_OHCI+3)
#define E_OHCI_DMA_RELEASE_DATA_ERR			(E_OHCI+4)
#define E_OHCI_ARDMA_INVALID_DESC			(E_OHCI+5)
#define E_OHCI_ARDMA_CONTEXT_DEAD			(E_OHCI+6)
#define E_OHCI_ARDMA_BAD_EVENT_CODE			(E_OHCI+7)
#define E_OHCI_ARDMA_INACTIVE_TIMEOUT		(E_OHCI+8)
#define E_OHCI_ATDMA_INVALID_DESC			(E_OHCI+9)
#define E_OHCI_ATDMA_CONTEXT_DEAD			(E_OHCI+10)
#define E_OHCI_ATDMA_NO_MORE_DONE			(E_OHCI+11)
#define E_OHCI_ATDMA_INACTIVE_TIMEOUT		(E_OHCI+12)
#define E_OHCI_INTEVENT_CB_ERR				(E_OHCI+13)
#define E_OHCI_INTEVENT_CB_RANGE_ERR		(E_OHCI+14)
#define E_OHCI_COMPARE_AND_SWAP_ERR	 		(E_OHCI+15)
#define E_OHCI_INVALID_VENDORID				(E_OHCI+16)
#define E_OHCI_INVALID_DEVICEID				(E_OHCI+17)
#define E_OHCI_SELFID_ERR					(E_OHCI+18)
#define E_OHCI_SELFID_GEN_ERR				(E_OHCI+19)
#define E_OHCI_NODEID_ERR					(E_OHCI+20)
#define E_OHCI_CSR_POLL_TIMEOUT				(E_OHCI+21)
#define E_OHCI_LOCKRESPERR					(E_OHCI+22)
#define E_OHCI_REQACCESSFAIL				(E_OHCI+23)
#define E_OHCI_CYCLELOST					(E_OHCI+24)
#define E_OHCI_CYCLETOOLONG					(E_OHCI+25)
#define E_OHCI_ADDR_RANGE					(E_OHCI+26)
#define E_OHCI_SOFT_RESET_TIMEOUT			(E_OHCI+27)

/*********************** MEMORY POOL (MPM) *****************************/
#define	E_MPM_REQ_SIZE_TOO_LARGE			(E_MPM+1)		// requested block is larger than largest defined
#define	E_MPM_NO_BLOCK_AVAILABLE			(E_MPM+2)		// allocate block failed - none available
#define	E_MPM_GUARD_BAND_VIOLATION			(E_MPM+3)		// release detected guard band has been altered
#define	E_MPM_BAD_BLOCK_SIZE				(E_MPM+4)		// block size must be a multiple of 4
#define	E_MPM_BAD_HEADER_SIZE				(E_MPM+5)		// size must be a multiple of 4
#define	E_MPM_DESCRIPTOR_ORDER				(E_MPM+6)		// descriptors out of order in the table
#define	E_MPM_ERROR							(E_MPM+7)		// unknown mpm error

/*********************** KEY VALUE  ************************************/
#define E_KV_BADKEYVALUE					(E_KEYVALUE+1)
#define	E_KV_ILLEGALKEY						(E_KEYVALUE+2)	// Caller passed in the key 0xFFFFFFFF

/******************************* LM ************************************/
#define E_LM_ELEMENT_UNAVAILABLE			(E_LM+1)		// List Manager Errors
#define E_LM_ELEMENT_LOCKED					(E_LM+2)
#define E_LM_ELEMENT_NOT_FOUND				(E_LM+3)
#define E_LM_ELEMENT_NOT_ALLOCATED			(E_LM+4)
#define E_LM_OUTOFSPACE						(E_LM+5)

/****************************** CLI ************************************/
#define	E_CLI_OUTOFSPACE					(E_CLI+1)		// The CLI doesn't have enough space left to install your command.
#define E_CLI_DUPLICATECOMMAND				(E_CLI+2)		// The command you want to install has the same name as a command already installed.
#define E_CLI_BADARGUMENTS					(E_CLI+3)		// The command you installed has a bad argument list.
#define E_CLI_BADCONSTANTS					(E_CLI+4)		// The command you installed has bad constants.
#define E_CLI_BADCONTEXT					(E_CLI+5)		// This command doesn't work outside of the context of a CLITool
#define E_CLI_BADDESCRIPTORTYPE				(E_CLI+6)		// The specified type of the descriptor does not exist, or is a bad type.
#define E_CLI_BADDESCRIPTORNAME				(E_CLI+7)		// The specified name of the descriptor does not exist, or is a bad type.
#define E_CLI_BADDESCRIPTORFUNCTION			(E_CLI+8)		// The specified function of descriptor does not exist.
															// Return codes which should be returned by tool functions.
#define E_CLI_TOOFEWARGUMENTS				(E_CLI+9)		// The tool didn't receive enough arguments.
#define E_CLI_TOOMANYARGUMENTS				(E_CLI+10)		// The tool received too many arguments.
#define E_CLI_INVALIDARGUMENTS				(E_CLI+11)		// the tool received bad argument values.
#define E_CLI_INVALIDARGUMENTTYPE			(E_CLI+12)		// One or more of the arguments was a bad type.
															// Return codes from a Variable Changed Callback.
#define E_CLI_INVALIDRANGE					(E_CLI+13)		// The new value that the user attempted to assign to the variable, is out of range.
#define E_CLI_BADOPERATION					(E_CLI+14)		// The variable callback does not support the operation passed in.
#define E_CLI_UNKNOWNCOMMAND				(E_CLI+15)		// The command/variable is not registered

/************************* SIO *****************************************/
#define	E_SIO_NO_RX_DATA		   			(E_SIO+1)		// no receive data available
#define	E_SIO_OVRE_ERROR			   		(E_SIO+2)		// receiver overrun error (hardware layer)
#define	E_SIO_FRAME_ERROR					(E_SIO+3)		// framing error (hardware layer)
#define	E_SIO_PARE_ERROR					(E_SIO+4)		// parity error (hardware layer)
#define E_SIO_RX_OVERFLOW					(E_SIO+5)		// receive buffer overflow (software)
#define	E_SIO_STRING_TOO_LONG				(E_SIO+6)		// read string function, no string terminator found
#define	E_SIO_TX_BUSY						(E_SIO+7)		// transmitter busy (tx buf is full)
#define	E_SIO_CHAN_NOT_OPEN					(E_SIO+8)		// all sio functions, can't perform i/o on closed channel
#define	E_SIO_CHAN_INVALID					(E_SIO+9)		// all sio functions, channel does not exist
#define E_SIO_TX_TOO_LONG					(E_SIO+10)		// transmit, string or buffer too large
#define E_SIO_RX_UNDERFLOW					(E_SIO+11)		// received fewer characters than requested
#define E_SIO_ALREADY_INITIALIZED			(E_SIO+12)		// init called when already initialized
#define	E_SIO_CHAN_ALREADY_OPEN				(E_SIO+13)		// attempt to open channel aready opened
#define	E_SIO_WIN_COM_ERROR					(E_SIO+14)		// some failure in windoze communications api
#define	E_SIO_WIN_THREAD_ERROR				(E_SIO+15)		// some failure in windoze thread api
#define	E_SIO_TXBUF_TOO_LARGE				(E_SIO+16)		// kbd driver, tx buffer too big
#define	E_SIO_RX_INCONSISTENT				(E_SIO+17)		// kbd driver, internal inconsistency
#define	E_SIO_INIT_BAD_PARMS				(E_SIO+18)		// bad parameter(s) passed to initialization
#define E_SIO_CHAN_WRITE_ONLY				(E_SIO+19)		// cannot read on a channel opened for write only
#define E_SIO_PORT_ALREADY_OPEN_READ		(E_SIO+20)		// port can only be opened for reading one time

/************************* TXPPCORE ************************************/
#define	E_TXPPCORE_DATA_ERROR				(E_TXPPCORE+1)// 
#define	E_TXPPCORE_TYPE_ERROR				(E_TXPPCORE+2)// 
#define	E_TXPPCORE_ADDR_ERROR				(E_TXPPCORE+3)// 
#define	E_TXPPCORE_LOCK_ERROR				(E_TXPPCORE+4)// 
#define	E_TXPPCORE_STATUS_ERROR				(E_TXPPCORE+5)// 

#define	E_TEST_PCORE_SET_COMMAND_FAIL		(E_TEST_PCORE+1)// 
#define	E_TEST_PCORE_SUSPEND_FAIL			(E_TEST_PCORE+2)// 
#define	E_TEST_PCORE_RESUME_FAIL			(E_TEST_PCORE+3)// 
#define	E_TEST_PCORE_INTERNAL_ERROR			(E_TEST_PCORE+4)// 
#define	E_TEST_PCORE_RUN_TEST_ERROR			(E_TEST_PCORE+5)// 

#define	E_DEBUGPCORE_WRONG_TESTDATA			(E_DEBUGPCORE+1)	// 
#define	E_DEBUGPCORE_ADDRESS_RANGE			(E_DEBUGPCORE+2)	// 
#define	E_DEBUGPCORE_NOT_CONNECTED			(E_DEBUGPCORE+3)	// 

/************************ LOCK *****************************************/
															// lock transaction utility
#define E_LOCK_TYPE_NOT_IMPLEMENTED			(E_LOCK+1)		// The specific lock type is not implemented yet
#define E_LOCK_TYPE_NOT_ALLOWED				(E_LOCK+2)		// The lock type requested is not allowed (specified by lockTypesAllowed)
#define E_LOCK_TYPE_INVALID					(E_LOCK+3)		// The packetblock contained an invalid lock type
#define E_LOCK_TRANSACTION_INVALID			(E_LOCK+4)		// The packetblock is not a lock request subaction
#define E_LOCK_COMPARE_SWAP_FAIL			(E_LOCK+5)		// The compare-swap operation failed

/************************* IRM *****************************************/
#define	E_IRM_INSUFFICIENT_BANDWIDTH		(E_IRM+1)		// insufficient bandwidth from IRM
#define	E_IRM_CHANNEL_OUT_OF_RANGE			(E_IRM+2)		// isoch resource channel out of range
#define	E_IRM_CHANNEL_NOT_ALLOCATED			(E_IRM+3)		// 
#define	E_IRM_CHANNEL_NOT_AVAILABLE			(E_IRM+4)		// 
#define	E_IRM_NO_CHANNEL_AVAILABLE			(E_IRM+5)		// 
#define	E_IRM_BUS_MANAGER_ALREADY_SET		(E_IRM+6)		// 

/************************* CMP *****************************************/
#define	E_CMP_PCR_INVALID					(E_CMP+1)		// attempt to access invalid PCR
#define	E_CMP_PCR_BUSY						(E_CMP+2)		// Could not lock, because lock was busy.
#define	E_CMP_PCR_NOT_CONNECTED				(E_CMP+3)		// plug is not connected
#define	E_CMP_PCR_ALREADY_CONNECTED			(E_CMP+4)		// plug is already connected

/************************* AVC *****************************************/
															// AVC PayloadParser
															// ALL NON NO_ERROR errors returned by the PayloadParser functions, 
															// mean that the data returned is now undefined, and cannot be used.  	
#define	E_AVC_PP_BADDATA					(E_AVC+1)		// The data argument was either NIL or contained a bad FCP packet
#define	E_AVC_PP_BADCTYPE					(E_AVC+2)		// The ctype contained either a reserved, or unknown value
#define	E_AVC_PP_RESERVED_SUBUNITTYPE		(E_AVC+3)		// The subunit type contained data which is reserved for AVC 3.0
#define	E_AVC_PP_RESERVED_SUBUNITID			(E_AVC+4)		// The subunit ID contained data which is reserved for AVC 3.0
#define	E_AVC_PP_RESERVED_EXTENDED			(E_AVC+5)		// TOne of the extended type or ID fields contained a 0
#define	E_AVC_PP_UNSUPPORTED_EXTENSION		(E_AVC+6)		// Either the type or ID was extended more than once, which we don't hand yet
#define	E_AVC_DS_BAD_MODE					(E_AVC+8)		// Bad Mode specified or Stream can't operate in this mode.
#define	E_AVC_DS_TOO_MUCH					(E_AVC+9)		// Can't read more than 32 bits of data from the stream.
#define	E_AVC_DS_EOF						(E_AVC+10)		// While reading, you have reached the end of the memory block.
#define	E_AVC_DS_BOF						(E_AVC+11)		// While rewinding, you have reached beginning of the block.
#define	E_AVC_DS_INVALID_STREAM				(E_AVC+12)		// Invalid DataStream pointer.
#define	E_AVC_DS_NO_ROOM_TO_GROW			(E_AVC+13)		// While writing, you have reached the end of the allocated block.
#define	E_AVC_DS_NOT_ON_BYTE_BOUNDARY		(E_AVC+14)		// FastForward and Rewind functions may only take place when the stream pointer is on a BYTE boundary.
#define E_AVC_PKT_AVC_NOT_IMPLEMENTED		(E_AVC+15)
															// AVC Callback Registry Errors
#define	E_AVC_CR_REQUIRE_NEXT_KEY			(E_AVC+16)		// The Callback descriptor you wanted to add, could not be added,
															// because the descriptor did not have "CALLBACK_DESCRIPTOR_NEXT" key.
#define E_AVC_CR_REQUIRE_TYPE_KEY			(E_AVC+17)  	// The Callback descriptor you wanted to add, could not be added,
															// because the descriptor did not have a "CALLBACK_DESCRIPTOR_TYPE"  key.
#define E_AVC_CR_CALLBACK_NOT_FOUND			(E_AVC+18)		// The callback of the particular type you wanted, could not be found.
															// PKT_AVC - returned by subunit callback functions to the AV/C layer 
#define	E_AVC_UNEXPECTED_RESPONSE			(E_AVC+19)

#define E_PKT_AVC_SUBUNIT_BUSY				(E_AVC+30)		// subunit cannot process command at this time
#define E_PKT_AVC_IMPLEMENTED				(E_AVC+31)  
#define E_PKT_AVC_NOT_IMPLEMENTED			(E_AVC+32)	 
#define E_PKT_AVC_REJECTED					(E_AVC+33)
#define E_PKT_AVC_ACCEPTED					(E_AVC+34)
#define E_PKT_AVC_INTERIM					(E_AVC+35)
#define	E_PKT_AVC_STABLE					(E_AVC+36)

/**************************** DRD **************************************/
#define	E_DRD_INVALID_MSG_PARMS				(E_DRD+1)		// Message from host has invalid parameters
#define	E_DRD_UNKNOWN_MSG_TYPE				(E_DRD+2)		// message type is unknown
#define	E_DRD_BAD_INTERNAL_STATE			(E_DRD+3)		// state machine f-u
#define	E_DRD_INCONSISTENT					(E_DRD+4)		// software f-u
#define	E_DRD_BAD_SET_PARAM					(E_DRD+5)		// application 'set' command, bad parameter to 'set' function

/**************** SMB - SEGMENTED BUFFER MANAGER (SBM) *****************/
#define E_SBM_OUT_OF_SPACE					(E_SBM+1)		// The SBM has no more space left in it.
#define E_SBM_EOF							(E_SBM+2)		// The SBM buffer you were reading didn't have enough data to fulfill the request.

/*************** DRS - Descriptor Registration Service *****************/
#define	E_DRS_NO_DESCRIPTOR					(E_DRS+1)		// Could not find the specified descriptor.
#define E_DRS_BAD_REFERENCE					(E_DRS+2)		// Bad descriptor reference.
#define	E_DRS_NO_DESCRIPTOR_SPACE			(E_DRS+3)		// Out of descriptor memory space.
#define E_DRS_NOT_YOUR_LOCK					(E_DRS+4)		// This descriptor was not locked by the current thread.
#define E_DRS_NOT_LOCKED_YET				(E_DRS+5)		// This descriptor has not been locked by any thread.
#define E_DRS_ALREADY_LOCKED				(E_DRS+6)		// This descriptor is already locked by this thread.
#define	E_DRS_INDEX_OUT_OF_RANGE			(E_DRS+7)		// The index used was outside of the range of indexes
#define	E_DRS_FIND_REQUIRES_SUBUNITID		(E_DRS+8)		// The descriptor you are attempting to find requires that the  subunitID be a component of the search.
#define	E_DRS_FIND_REQUIRES_SUBUNITTYPE		(E_DRS+9)		// The descriptor you are attempting to find requires that the subunittype be a component of the search.
#define	E_DRS_FIND_REQUIRES_EDSTYPE			(E_DRS+10)		// All descriptor searches require an EDSTYPE lack of one, generates this error.
#define E_DRS_FIND_REQUIRES_EDSTYPE_LIST	(E_DRS+11)		// When searching without a parent, you can only search for a list.
#define	E_DRS_FIND_MISMATCHED_CHILD_PARENT	(E_DRS+12)		// The type of the child and the type of the parent are not compatible search arguments.
#define	E_DRS_FIND_MISSING_KEY_ARGUMENT		(E_DRS+13)		// There wasn't any key information to search for. (i.e. the Type or the ID of the object.)

/**************** DSS - Descriptor Specifier Service *******************/
#define	E_DSS_BAD_DSSADDRESSREFERENCE		(E_DSS+1)		// The reference to the specified address was bad.
#define	E_DSS_NO_DSSA_SPACE_LEFT			(E_DSS+2)		// No more DSS Addresses can be created.
#define E_DSS_BAD_DS						(E_DSS+3)		// Bad DataStream.
#define	E_DSS_BAD_DATA						(E_DSS+4)		// The data is not a correctly formatted AV/C descriptor specifier.
#define E_DSS_ELEMENT_OUT_OF_RANGE			(E_DSS+5)		// The index of the element you chose, is out of range.

/*************** EDS - Embedded Descriptor Service *********************/
#define	E_EDS_UNKNOWN_BUFFERTYPE			(E_EDS+1)		// The buffertype passed into the function did not map to a buffer in the descriptor.
#define E_EDS_INCOMPATIBLE_TYPES			(E_EDS+2)		// The descriptor types you chose, for the child and parent parameters, are incompatible. 
#define	E_EDS_NOTSAMESUBUNIT				(E_EDS+3)		// The descriptors you chose for the parent and child, are not specified by the same subunitid and subunittype.
#define	E_EDS_BAD_BUFFERTYPE				(E_EDS+4)		// The buffertype is not compatible with the specified descriptor.
#define E_EDS_UNABLE_TO_DETERMINE_SIZE		(E_EDS+5)		// The descriptor you wanted to know the size of, does not have a parent.
#define E_EDS_UNKNOWN_DESCRIPTOR			(E_EDS+6)		// The descriptor you passed in either is bad, or is incompatible with this function.
#define	E_EDS_OUT_OF_CONNECTION_SPACE		(E_EDS+7)		// There aren't any more incoming connection handles available.
#define	E_EDS_ALREADY_CONNECTED				(E_EDS+8)		// The incoming connection is already connected.
#define	E_EDS_NOT_CONNECTED					(E_EDS+9)		// the connection could not be found.
#define	E_EDS_BAD_SPECIFIER_TYPE			(E_EDS+10)		// The address you wanted to look up, contained a bad specifier type.
#define E_EDS_ALREADY_OPEN_FOR_WRITE		(E_EDS+11)		// The descriptor is already open by another connection for the purpose of writing.
#define E_EDS_OUT_OF_RANGE					(E_EDS+12)		// The read on the descriptor, started out of range.
#define	E_EDS_EOF							(E_EDS+13)		// The read is done, the descriptor has been read to the end.
															// callback return values from eds callbacks
#define	E_EDS_CR_CANT_RESOLVE				(E_EDS+14)		// The custom resolver was not able to resolve the address.

/*************** AVS - Audio and Video System **************************/
#define E_AVS_NO_PARTIONS_FREE				(E_AVS+1)		// No more free partitions in Media FIFO buffer
#define E_AVS_INVALID_PLUGID				(E_AVS+2)		// Invalid plugID used in call to AVS function
#define E_AVS_INVALID_FORMAT				(E_AVS+3)		// Invalid CIP1 format in call to AVS
#define E_AVS_INVALID_PARTITION				(E_AVS+4)		// Invalid partition used in Media FIFO

/************ DTCP Digital Transmission Content Protection *************/
#define E_DTCP_SIGNATURE_INVALID			(E_DTCP+1)		// invalid DTCP signature
#define E_DTCP_ECC_POINT_AT_INF				(E_DTCP+2)		// ECC function return Point At Infinity
#define E_DTCP_ECC_BUSY						(E_DTCP+3)		// ECC is busy processing data (ECPM)
#define E_DTCP_ECC_ZERO_OPERAND				(E_DTCP+4)		// ECC integer operand is zero
#define E_DTCP_ECC_REL_PRIME				(E_DTCP+5)		// ECC intermediate operands are not relative prime

/************* AKE - DTCP Authentication and Key Exchange **************/
#define E_AKE_BAD_STATUS					(E_AKE+1)		// invalid results	
#define E_AKE_CANCEL_PROC					(E_AKE+2)		// a cancel subfunction has been received
#define E_AKE_STOP_AKE						(E_AKE+3)		// a stop ake message has been received
#define E_AKE_DEVICE_ALREADY_AT_MAX_USE		(E_AKE+4)		// the device already has the maximum number of sources and sinks allocated
#define E_AKE_NODE_NOT_INUSE				(E_AKE+5)		// a node referenced as being used is not actually so
#define E_AKE_IGNORED_STATUS				(E_AKE+6)		// a status command response has been received 
#define E_AKE_EXPECT_SRM_RESP				(E_AKE+7)		// when a SRM response is expected
#define E_AKE_UNKNOWN_SUBFUNCTION			(E_AKE+8)		// bad subfunction value encountered
#define E_AKE_UNSUPPORTED_STATUS			(E_AKE+9)		// bad status encountered
#define E_AKE_PARSE_DATA_ERROR				(E_AKE+10)		// bad value encountered during parsing data
#define E_AKE_SIZE_ERROR					(E_AKE+11)		// bad size encountered
#define E_AKE_AUTH_ERROR					(E_AKE+12)		// bad auth mode encountered
#define E_AKE_STATE_ERROR					(E_AKE+13)		// bad ake task state encountered

/**************************** TASKING ***********************************/
#define	E_TASKING_CREATE_ERROR				(E_TASKING+1)
#define	E_TASKING_ID_NOT_VALID				(E_TASKING+2)
#define	E_TASKING_SUSPEND_FAIL				(E_TASKING+3)
#define	E_TASKING_RESUME_FAIL				(E_TASKING+4)

/**************************** SEMAPHORE *********************************/
#define	E_SEMAPHORE_OPEN_ERROR				(E_SEMAPHORE+1)
#define	E_SEMAPHORE_CLOSE_ERROR				(E_SEMAPHORE+2)
#define	E_SEMAPHORE_ID_NOT_VALID			(E_SEMAPHORE+3)
#define	E_SEMAPHORE_TIMEOUT					(E_SEMAPHORE+4)

/**************************** MSGQUEUE **********************************/
#define	E_MSGQUEUE_OPEN_ERROR				(E_MSGQUEUE+1)
#define	E_MSGQUEUE_CLOSE_ERROR				(E_MSGQUEUE+2)
#define	E_MSGQUEUE_ID_NOT_VALID				(E_MSGQUEUE+3)
#define	E_MSGQUEUE_TIMEOUT					(E_MSGQUEUE+4)
#define	E_MSGQUEUE_FULL						(E_MSGQUEUE+5)
#define	E_MSGQUEUE_EMPTY					(E_MSGQUEUE+6)

/**************************** COMMON ************************************/
#define	E_BAD_INPUT_PARAMETERS				(E_COMMON+1)	// funky function input parameters
#define E_ASSERT_FAILED						(E_COMMON+2)	
#define E_NOT_IMPLEMENTED					(E_COMMON+3)	
#define E_CALLBACK_REQUEST_FAILED			(E_COMMON+4)	// no room to add callback
#define E_NOT_INITIALIZED					(E_COMMON+5)	// someone forgot to initialize something
#define E_ALREADY_INITIALIZED				(E_COMMON+6)	// module already initialized
#define E_FAIL								(E_COMMON+7)	// generic non-specific failure
#define E_VERIFY_FAILED						(E_COMMON+8)	// common to various download / flash procedures
#define E_EOF								(E_COMMON+9)	// end-of-file
#define E_UNREACHABLE_CODE_REACHED			(E_COMMON+10)	// you shouldnt be here
#define	E_UNKNOWN							(E_COMMON+11)	
#define E_QUEUE_FULL						(E_COMMON+12)	
#define E_NULL_PTR							(E_COMMON+13)	
#define E_SIZE_TOO_BIG						(E_COMMON+14)

#endif

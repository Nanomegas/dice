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

#ifndef _DICE2_ERROR_CODES_H_
#define _DICE2_ERROR_CODES_H_

#define E_LLC			0xC0000000			// inSilicon LLC errors 
#define E_TX			0xC1000000			// ThreadX errors see ThreadX manual
#define E_TX1			0xC2000000			// ThreadX errors see ThreadX manual
#define E_DICE			0xC3000000		    // All other codes
#define E_DAL			0xC4000000			// Dice Abstraction Layer errors
#define E_FIS			0xC5000000			// FIS errors
#define E_SPI			0xC6000000			// SPI errors
#define E_CRB			0xC7000000			// CRB errors

/**************************** LLC **************************************/
#define E_LLC_WRONG_VERSION					(E_LLC+1)
#define E_LLC_HOST_ACC_ERR					(E_LLC+2)
#define E_LLC_NODE_ID_INVALID				(E_LLC+3)
#define	E_LLC_CABLE_PWR_ERROR				(E_LLC+4)
#define E_LLC_INTEVENT_CB_ERR				(E_LLC+5)
#define E_LLC_INTEVENT_CB_RANGE_ERR			(E_LLC+6)
#define E_LLC_INT_HDR_ERR					(E_LLC+7)
#define E_LLC_INT_TCODE_ERR					(E_LLC+8)
#define E_LLC_INT_INVALID_ACK				(E_LLC+9)
#define E_LLC_INT_CYCLOST					(E_LLC+10)
#define E_LLC_INT_CYCCRCERR					(E_LLC+11)
#define E_LLC_INT_CYCARBFAIL				(E_LLC+12)
#define E_LLC_INT_CYCTOOLONG				(E_LLC+13)
#define E_LLC_INVALID_NODE_ID				(E_LLC+14)
#define E_LLC_NODE_ID_NOT_OVER				(E_LLC+15)
#define E_LLC_ROOT_ID_NOT_OVER				(E_LLC+16)
#define E_LLC_ROOT_ID_ERR					(E_LLC+17)
#define E_LLC_NODE_ID_ERR					(E_LLC+18)
#define E_LLC_CABLE_LOOP					(E_LLC+19)
#define E_LLC_SELF_ID_INCR_ERR				(E_LLC+20)
#define E_LLC_SELF_ID_PAR_ERR				(E_LLC+21)
#define E_LLC_SELF_ID_RX_FAIL				(E_LLC+22)
#define E_LLC_BUS_ERROR						(E_LLC+23)
#define E_LLC_BUS_RESET_CNT_ERR				(E_LLC+24)
#define E_LLC_HEADER_ERR					(E_LLC+25)
#define E_LLC_TCODE_ERR						(E_LLC+26)
#define E_LLC_ASY_DATA_ERR					(E_LLC+27)
#define E_LLC_INC_SEC_COUNT					(E_LLC+28)
#define E_LLC_CY_TOO_LONG					(E_LLC+29)
#define E_LLC_CY_LOST						(E_LLC+30)
#define E_LLC_ISO_TX_ERR					(E_LLC+31)
#define E_LLC_ISO_RX_ERR					(E_LLC+32)
#define E_LLC_HOST_ACSS_ERR					(E_LLC+33)
#define E_LLC_NO_BUS_GRANT					(E_LLC+34)
#define E_LLC_ABORT_TRANSMISSION			(E_LLC+35)
#define E_LLC_TX_NEVER_COMPLETED			(E_LLC+36)
#define E_LLC_RX_OVERFLOW					(E_LLC+37)
#define E_LLC_TX_OVERFLOW					(E_LLC+38)
#define E_LLC_AR_NO_MORE_DATA				(E_LLC+39)
#define E_LLC_ERROR_EVENT_CODE				(E_LLC+40)

#define E_TX_INVALID_TASKID					(E_TX1+1)
#define E_TX_INVALID_SEMAID					(E_TX1+2)
#define E_TX_NOMORE_FREE_SEMA				(E_TX1+3)
#define E_TX_ERROR_RESET_SEMA				(E_TX1+4)
#define E_TX_CREATE_TASK					(E_TX1+5)

/**************************** DICE **************************************/
#define E_DICE_ROUTER_NOT_IN_USE			(E_DICE+1)	// Try to allocate channels 
#define E_DICE_MAX_ROUTE_ENTRIES			(E_DICE+2)	// Maximum number of route entries
#define E_DICE_BAD_INPUT_PARAM				(E_DICE+3)  // Wrong input paramet for function

/**************************** DAL **************************************/
#define E_DAL_BAD_INPUT_PARAM				(E_DAL+1)
#define E_DAL_INTERFACE_NOT_DEFINED			(E_DAL+2)
#define E_DAL_INTERFACE_NOT_CREATED			(E_DAL+3)
#define E_DAL_IODEVICE_CONFLICT				(E_DAL+4)
#define E_DAL_INVALID_ROUTE					(E_DAL+5)
#define E_DAL_TOO_MANY_CALLBACKS			(E_DAL+6)
#define E_DAL_INVALID_CALLBACK				(E_DAL+7)
#define E_DAL_ILLEGAL_NOMINAL_RATE			(E_DAL+8)
#define E_DAL_CONTEXT_ERROR					(E_DAL+9)
#define E_DAL_ILLEGAL_RATE					(E_DAL+10)

/**************************** FIS **************************************/
#define E_FIS_ILLEGAL_IMAGE					(E_FIS+1)
#define E_FIS_FLASH_OP_FAILED				(E_FIS+2)
#define E_FIS_NO_SPACE						(E_FIS+3)
#define E_FIS_MEMORY_ERROR					(E_FIS+4)

/**************************** SPI **************************************/
#define E_SPI_INVALID_SLAVEID				(E_SPI+1)
#define E_SPI_TX_ERROR						(E_SPI+2)
#define E_SPI_RX_ERROR						(E_SPI+3)
#define E_SPI_NO_FREE_DEVICE				(E_SPI+4)
#define E_SPI_UNKNOWN_CMD					(E_SPI+5)
#define E_SPI_BUF_FULL						(E_SPI+6)

/**************************** SPI **************************************/
#define E_CRB_BAD_INPUT_PARAM				(E_CRB+1)
#define E_CRB_TOO_MANY_ITEMS				(E_CRB+2)
#define E_CRB_INVALID_HANDLE				(E_CRB+3)
#define E_CRB_NO_ENTRIES						(E_CRB+4)
#define E_CRB_ROM_EMPTY							(E_CRB+5)
#define E_CRB_NOT_DIR_HANDLE				(E_CRB+6)
#define E_CRB_NO_HEAP								(E_CRB+7)
#define E_CRB_IMAGE_TOO_SMALL				(E_CRB+8)

#endif // _DICE2_ERROR_CODES_H_

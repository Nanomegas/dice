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
 * DESCRIPTION: General 1394 defines

	Purpose:	general 1394 defines
	
	Revisions:
		created 04/23/2002 lm

*****************************************************************************/

#ifndef _1394_H_
#define _1394_H_

/* Define IEEE 1394 Async Transaction Codes */

#define TCODE_WRQUAD			0x00		/* Write request for data quadlet	*/
#define TCODE_WRBLOCK			0x01		/* Write request for data block		*/
#define TCODE_WRRESPONSE		0x02		/* Write response					*/
#define TCODE_RESERVED_1		0x03		/* Reserved							*/
#define TCODE_RDQUAD			0x04		/* Read request for data quadlet	*/
#define TCODE_RDBLOCK			0x05		/* Read request for data block		*/
#define TCODE_RDQUADRSP			0x06		/* Read response to request quad	*/
#define TCODE_RDBLOCKRSP		0x07		/* Read response to request block	*/
#define TCODE_CYCLESTART		0x08		/* Request to start iso cycle		*/
#define TCODE_LOCKRQST			0x09		/* Lock request						*/
#define TCODE_ISOBLOCK			0x0a		/* ISO data block					*/
#define TCODE_LOCKRSP			0x0b		/* Lock response to lock request	*/
#define TCODE_RESERVED_2		0x0c		/* Reserved							*/
#define TCODE_RESERVED_3		0x0d		/* Reserved							*/
#define TCODE_LLC_SPECIFIC		0x0e		/* internally used by link designs  */
#define TCODE_RESERVED_4		0x0f		/* Reserved							*/
#define MASK_TCODE				0xf0
#define SHIFT_TCODE				0x04

/* Define IEEE 1394 Extended Transaction Codes */

#define EXT_TCODE_RESERVED		0x00		/* reserved */
#define EXT_TCODE_MASK_SWAP		0x01		/* mask_swap */
#define EXT_TCODE_COMPARE_SWAP	0x02		/* compare_swap */
#define EXT_TCODE_FETCH_ADD		0x03		/* fetch_add */
#define EXT_TCODE_LITTLE_ADD	0x04		/* little_add */
#define EXT_TCODE_BOUNDED_ADD	0x05		/* bounded_add */
#define EXT_TCODE_WRAP_ADD		0x06		/* wrap_add */
#define EXT_TCODE_VENDOR_DEPEND	0x07		/* vendor dependednt */
#define MASK_EXT_TCODE			0xff

/* Define IEEE 1394a Acknowledge Codes */

#define ACK_1394_RESERVED_0		0x00		/* reserved in 1394a */
#define ACK_1394_COMPLETE		0x01		/* packet accepted and no response shall follow */
#define ACK_1394_PENDING		0x02		/* packet accepted and response shall follow */
#define ACK_1394_RESERVED_3		0x03		/* reserved in 1394a */
#define ACK_1394_BUSY_X			0x04		/* packet could not be accepted, ok to retry now */
#define ACK_1394_BUSY_A			0x05		/* packet could not be accepted, ok to retry during next retry phase a */
#define ACK_1394_BUSY_B			0x06		/* packet could not be accepted, ok to retry during next retry phase b */
#define ACK_1394_RESERVED_7		0x07		/* reserved in 1394a */
#define ACK_1394_RESERVED_8		0x08		/* reserved in 1394a */
#define ACK_1394_RESERVED_9		0x09		/* reserved in 1394a */
#define ACK_1394_RESERVED_A		0x0a		/* reserved in 1394a */
#define ACK_1394_TARDY			0x0b		/* not accepted but retry possible in subsequent fairness interval */
#define ACK_1394_CONFLICT_ERROR	0x0c		/* resource conflick prevented packet from being accepted */
#define ACK_1394_DATA_ERROR		0x0d		/* data failed CRC check or data length field does not match data size */
#define ACK_1394_TYPE_ERROR		0x0e		/* unsupported value in packet field, or invalid transaction attempted */
#define ACK_1394_ADDRESS_ERROR	0x0f		/* target address (destination_offset) no accessible within dest. node */

/* Define IEEE 1394 Response Codes */

#define RESP_CODE_COMPLETE		0x00		/* the node successfully completed the comand */
#define RESP_CODE_CONFLICT		0x04		/* a resource conflict was detected.  the request may be retried */
#define RESP_CODE_DATA_ERROR	0x05		/* hardware error, data is unavailable */
#define RESP_CODE_TYPE_ERROR	0x06		/* a field in the request packet was set to an unsupported or incorrect value */
#define RESP_CODE_ADDRESS_ERROR	0x07		/* the destination offset field in the request was set to an address not accessible */
#define SHIFT_RESP_CODE			0x0c

/* Define IEEE 1394 Retry Codes */

#define RETRY_CODE_1			0x00		/* retry_A/retry_B supported and a new reservation is requested */
#define RETRY_CODE_X			0x01		/* retry_A/retry_B not supported or (if supported) reservation not requested */
#define RETRY_CODE_A			0x02		/* retry_A/retry_B supported and a reservation has been assigned (ack_busy_a previously returned) */
#define RETRY_CODE_B			0x03		/* retry_A/retry_B supported and a reservation has been assigned (ack_busy_b previously returned) */
#define SHIFT_RETRY_CODE		0x08

/* Define IEEE 1394 Speed Signaling Codes */

#define SPEED_100				0x00
#define SPEED_200				0x01
#define SPEED_400				0x02
#define SPEED_RESERVED			0x03
#define MASK_SPEEDCODE			0x00030000
#define SHIFT_SPEEDCODE			16

/* Define Packet Field Positions */

#define SHIFT_TLABEL			10
#define MASK_TLABEL				0x0000fc00
#define SHIFT_NODEID			16
#define MASK_NODEID				0x003f
#define SHIFT_NODEADDR			16
#define MASK_NODEADDR			0xffff0000
#define SHIFT_RCODE				12
#define MASK_RCODE				0xf
#define MASK_DATA_LEN			0xffff0000
#define SHIFT_DATA_LEN			16
#define SHIFT_DATA_LEN_IN_QUADS	(SHIFT_DATA_LEN + 1)

/* Define busId for Non-bridged Bus Environment, == local bus in bridged environment */

#define LOCAL_BUS_ID			0xffc00000
#define SHIFT_LOCAL_BUS_ID		16
#define LOCAL_BUS_ID_MASK		0xffc0

/* Define some common packet fields */
#define SELF_ID_CONT_PACKET		0x00800000
#define SELF_ID_PHY_ID			0x3f000000
#define SELF_ID_PHY_ID_SHIFT	24

#define SELF_ID_SPEED_BYTE_SHIFT 6	// 3rd byte of self-id packet #0 has speed in bits 6-7


#endif


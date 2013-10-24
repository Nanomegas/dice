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
	
	Purpose:	Abstract MIDI Layer
  
  Description:       
  
	Revisions:
		created 09/18/2006 zluo

****************************************************************************/
#ifndef _AML_H_
#define _AML_H_

/** @addtogroup AML

	This module provides Abstraction MIDI layer function to user
	@{	
*/

#define AML_BUF_SIZE			8
#define AML_DEV_NUM				64

/// AML call back function
typedef void (*aml_callback_t)(uint32);

// AML device type
typedef enum {
	AML_DEV_TX = 0xF000,
	AML_DEV_RX, 
} aml_dev_type_t;

typedef struct{
	void				(*init)(uint32, uint32, uint32 *);
	int 				(*putchar)(uint32, uint8);
	aml_callback_t		start;
	aml_callback_t		stop;
} midi_dev_cb_t;

/** Get one byte from a rx device
*/
//aml interface with upper (application) layer
HRESULT amlAppGetByte(uint32 midi_dev_id, uint8 *byte);

/** Send one byte to a tx device
*/
//aml interface with upper (application) layer
HRESULT amlAppPutByte(uint32 midi_dev_id, uint8 value);

/// Initialize AML module 
HRESULT amlInitialize(void);

//! Create a midi device
HRESULT amlCreateDev(uint32 *midi_dev_id, midi_dev_cb_t *cbs, aml_dev_type_t type, uint32 arg, char *name);

//! Destroy a midi device
HRESULT amlDestroyDev(uint32 midi_dev_id);

//! Start a midi device
HRESULT amlStartDev(uint32 midi_dev_id);

//! Stop a midi device
HRESULT amlStopDev(uint32 midi_dev_id);

//! Set a destionation of an Rx device
HRESULT amlSetDestination(uint32 midi_dev_id_src, uint32 midi_dev_id_dest);

//! Set monitor mode of a midi device
HRESULT amlSetDevMonitorMode(uint32 midi_dev_id, BOOL enable);

#ifdef _CLI
//! Display the content of a midi device
HRESULT amlDisplayDev(uint32 midi_dev_id);
#endif //_CLI

//aml interface with lower(driver) layer
//! Called by aml drivers when lower level received a byte
HRESULT amlDrvIndRxByte(uint32 midi_dev_id, uint8 byte);

//! Called by aml drivers when lower level is ready to transmitt
HRESULT amlDrvIndTxEmpty(uint32 midi_dev_id, uint8 *data);
//! @}

#endif //_AML_H_

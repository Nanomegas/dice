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
#ifdef _AML

#include "TCTypes.h"

#include "aml.h"
#include "TCSemaphore.h"
#include "TCDebug.h"
#include "avsDefs.h"
#include "avs.h"
#include "avsAml.h"

#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_platform_ints.h>
#include "TCTasking.h"
#include "TCInterrupt.h"

#include "avsMidi.h"

//***************************************************************************
//* avs mid dev code
//***************************************************************************

static BOOL bInterruptEanbled = FALSE;

typedef struct {
	BOOL		enabled;
	uint32		aml_dev_id;
} avs_midi_drv_dev_t;

static avs_midi_drv_dev_t avsMidiDevTable[AVS_STREAM][AVS_CHAN_PER_STREAM]; 


//forward decl. of upcall, quadlet received
static void midiReceived1394 (uint32 data);

//forward decl. of upcall, has midi data
static BOOL get1394MidiOut(uint8 strm, uint32 *data);


static void AvsMidiEventHandler(void)
{
  uint32  MidiStatus;	
  uint32  MidiCtrl;	
  uint32  data;

	//Read Status and control
  avsRegRead(APBA_MIDI_APB_IF_STATUS, &MidiStatus);
  avsRegRead(APBA_MIDI_APB_IF_CONTROL,&MidiCtrl);

  //handle reception of bytes
  while (!(MidiStatus & MIDI_BUF_EMPTY_TX))
  {
    //Read Midi, up to 3 bytes from one port, interrupt cleared when all quadlets are read.
    avsRegRead(APBA_MIDI_APB_IF_QUAD_OUT, &data);		
    midiReceived1394 (data);
    avsRegRead(APBA_MIDI_APB_IF_STATUS, &MidiStatus); //check if there is more
  }
  //now check to see if we need to send something to stream 0
  while (!(MidiStatus & MIDI_BUF_FULL_RX0))
  {
    //we have space, call up to see if we have anything to send on stream 0
    if (get1394MidiOut(0,&data))
    {
      avsRegWrite(APBA_MIDI_APB_IF_QUAD_IN0, data);
      
    }
    else
    {
       MidiCtrl &=~MIDI_RX0_IRQ_EN; //disable further interrupts unless we write something
       break;      
    }    
  }
  //now check to see if we need to send something to stream 1
  while (!(MidiStatus & MIDI_BUF_FULL_RX1))
  {
    //we have space, call up to see if we have anything to send on stream 1
    if (get1394MidiOut(1,&data))
    {
      avsRegWrite(APBA_MIDI_APB_IF_QUAD_IN1, data);
    }
    else
    {
       MidiCtrl &=~MIDI_RX1_IRQ_EN; //disable further interrupts unless we write something
       break;      
    }    
  }
  avsRegWrite(APBA_MIDI_APB_IF_CONTROL, MidiCtrl);      
}

///////////////////////////////// 1394 MIDI RECEPTION //////////////////////////
// Handle reception of data into AML
////////////////////////////////////////////////////////////////////////////////

static void midiReceived1394 (uint32 data)
{
  //format of data
  //bit 29-31 - midi port 0-7
  //bit 27-28 - stream 0-3 (0-1 for Mini/JR)
  //bit 26    - reserved 0
  //bit 24-25 - bytes 0-3
  //bit 23-16 - byte 0
  //bit 15-8  - byte 1
  //bit 7-0   - byte 2
  //
  avs_midi_drv_dev_t * pdev; 
  
  //SYS_TRACE2(SYSDEBUG_TRACE_AML, 0, data);

  uint8 port   = (data>>29) & 0x7;
  uint8 strm   = (data>>27) & 0x3;
  uint8 bytes  = (data>>24) & 0x3;
  
  if (bytes==0) return; //there are no bytes (should not happen)
  if (strm>=AVS_RX_STREAM); //only AVS_RX_STREAM streams
  if (port>=AVS_CHAN_PER_STREAM) return; //AVS_CHAN_PER_STREAM

  pdev = &avsMidiDevTable[AVS_RX_FIRST+strm][port];

  
  if(!pdev->enabled) return; //port not enabled
  while (bytes--)
  {
    uint8 c = (uint8)(data>>16);
    amlDrvIndRxByte(pdev->aml_dev_id, c);
    data<<=8;
  }
}

///////////////////////////////// 1394 MIDI TRANSMISSION ///////////////////////
//Handle buffered transmission from  AML
////////////////////////////////////////////////////////////////////////////////

typedef struct
{
  uint8  buf[256]; //256 byte buffer, assume simple put/get wrap
  uint8  put;
  uint8  get;
}TCBUF;

static TCBUF midiBuf1394[AVS_TX_STREAM][AVS_CHAN_PER_STREAM];

// To optimize we keep track of which buffers might have data
static uint8 midiHasData[AVS_TX_STREAM]; //one bit for each port


//we want to do round robin on the buffers and ports so we keep track
//of the last port for each stream;
uint8 nxtPort[AVS_TX_STREAM];

//Tx wants some data
static BOOL get1394MidiOut(uint8 strm, uint32 *data)
{
  int8  i;
  uint8 port = nxtPort[strm];
  uint8 sft;
  uint8 msk;
  uint32 w;
  
  
  TCBUF * pBuf = &midiBuf1394[strm][port];
  
  if (strm>=AVS_TX_STREAM) return FALSE;
  
  msk = midiHasData[strm];
  if (!msk) return FALSE;
  
  //Check if there is any data in the buffers. (use round robin on the buffers)
  sft = 0x01<<port;
  for (i=0; i<8; i++)
  {
    if (msk & sft)
    {
      //this might have data
      pBuf = &midiBuf1394[strm][port];
      if (pBuf->put != pBuf->get) break;
      msk &=~sft;
      midiHasData[strm] = msk; //no more data
    }
    port++;
    sft<<=1;
    if (port==AVS_CHAN_PER_STREAM) 
    {
      port=0;
      sft = 1;
    }
  }
  if (i==8) return FALSE; //nothing to send
  
  
  
  // if we found one, build the packed word
  //format 'data' with up to 3 bytes.
  //bit 29-31 - midi port 0-7
  //bit 26-28 - reserved = 0
  //bit 24-25 - bytes 0-3
  //bit 23-16 - byte 0
  //bit 15-8  - byte 1
  //bit 7-0   - byte 2

  w = ((uint32)port)<<29;
  i=0;
  sft=16;
  while ((i<3) & (pBuf->put != pBuf->get))
  {
    w |= ((uint32)pBuf->buf[pBuf->get++])<< sft;
    sft -= 8;
    i++;
  }
  w |= ((uint32)i)<<24;
  //check if we emptied the port
  if (pBuf->put == pBuf->get)
  {
      msk &=~sft;
      midiHasData[strm] = msk; //no more data    
  }
  port++;
  if (port==AVS_CHAN_PER_STREAM) port=0;
  *data=w;  
  nxtPort[strm] = port;
    
  SYS_TRACE2(SYSDEBUG_TRACE_AML, strm, *data);
    
  return TRUE;
}

bool avsMidiDev_putChar(uint32 handle, uint8 data)
{
	uint8 	port = handle%AVS_CHAN_PER_STREAM;
	uint8   strm = handle/AVS_CHAN_PER_STREAM;

  TCBUF * pBuf;
  
  SYS_TRACE2(SYSDEBUG_TRACE_AML, handle, data);
  
  if (!avsMidiDevTable[strm][port].enabled) return FALSE;
  
  pBuf = &midiBuf1394[strm][port];
  if ((pBuf->put+1)==pBuf->get)
  {    
    return FALSE;
  }
  pBuf->buf[pBuf->put++] = data;
  midiHasData[strm] |= 1<<port;
   
  //reenable transmit interrupt
  uint32	MidiCtrl;	
  avsRegRead(APBA_MIDI_APB_IF_CONTROL,&MidiCtrl);
  MidiCtrl |= strm ? MIDI_RX1_IRQ_EN : MIDI_RX0_IRQ_EN;
  avsRegWrite(APBA_MIDI_APB_IF_CONTROL, MidiCtrl);
  return TRUE;
}



void avsMidiDev_init(uint32 dev_id, uint32 port, uint32* retHandle)
{
	if(port / 8 > 4) {
		SYS_DEBUG(SYSDEBUG_TRACE_AML, "wrong port number.\n");
		return;
	}

	avsMidiDevTable[port/8][port%8].aml_dev_id = dev_id;
	avsMidiDevTable[port/8][port%8].enabled = 0; //dont' enable it yet.

	*retHandle = port;
	if(!bInterruptEanbled) {
		bInterruptEanbled = TRUE;
		TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_MIDI, NULL, &AvsMidiEventHandler);
		avsEnableMidiInterrupt(true,true,true);
	}
}



void avsMidiDev_enable(uint32 port)
{
	avsMidiDevTable[port/8][port%8].enabled = 1; 
}

void avsMidiDev_disable(uint32 port)
{
	avsMidiDevTable[port/8][port%8].enabled = 0; 
};

midi_dev_cb_t avs_midi_dev_cbs = {
	&avsMidiDev_init,
	&avsMidiDev_putChar, 
	&avsMidiDev_enable, 
	&avsMidiDev_disable
};

#endif //_AML

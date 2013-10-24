//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd..
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//

/****************************************************************************
 * DESCRIPTION: AvcDriver

	Purpose:	AvcDriver
				Adapted directly from the DiceDriver module

	Revisions:
		created 03/20/2006 bk
****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "TCDebug.h"
#include "lal.h"
#include "avs.h"
#include "avsMidi.h"
#include "avsFmt.h"
#include "avcDriverInterface.h"
#include "avcDriver.h"
#include "avcCmdStreamFormat.h"
#include "targetBoard.h"
#include "dal.h"
#include "targetPlugs.h"
#include "cmpUtils.h"

#ifdef _CLI
#include "avcdriverCli.h"
#endif //_CLI

#include <cyg/hal/hal_platform_ints.h> //for interrupt number

#ifdef _AVC

static char 	moduleName[] = __FILE__ ;


// task (thread) ID for this module
uint32				AvcDriverThreadID;
const char*			AvcDriverThreadName = "AvcDriverThread";
uint32				AvcDriverSemID;
uint32				semID_notify;

static AVC_DRIVER_STRUCT	AvcDriver;

// these differ from dicedriver
uint32	CLOCK_SOURCE[]= {
							eDAL_CLK_SRC_AESRX,
							eDAL_CLK_SRC_AESRX0,
							eDAL_CLK_SRC_AESRX1,
							eDAL_CLK_SRC_AESRX2,
							eDAL_CLK_SRC_AESRX3,
							eDAL_CLK_SRC_ADATRX,
#ifdef _DICE2
							eDAL_CLK_SRC_TDIFRX,
							eDAL_CLK_SRC_WORD,
#elif defined (_DICEJR)
							eDAL_CLK_SRC_ADATRXAUX,
							eDAL_CLK_SRC_EXTERNAL,
#endif
							eDAL_CLK_SRC_AVS_SYT1,
							eDAL_CLK_SRC_AVS_SYT2,
#ifdef _DICE2
							eDAL_CLK_SRC_AVS_SYT3,
							eDAL_CLK_SRC_AVS_SYT4,
#elif defined (_DICEJR)
							0,
#endif
							eDAL_CLK_SRC_INTERNAL
						};

uint32	CLOCK_RATE[]=	{	eDAL_NOMINAL_RATE_32,
							eDAL_NOMINAL_RATE_44_1,
							eDAL_NOMINAL_RATE_48,
							eDAL_NOMINAL_RATE_88_2,
							eDAL_NOMINAL_RATE_96,
							eDAL_NOMINAL_RATE_176_4,
							eDAL_NOMINAL_RATE_192
						};

#define MAX_RX_QSEL	17
#define AVC_DRIVER_OFFSET(a)		((uint32)(&AvcDriver.a) - (uint32)&AvcDriver)
#define	LAST_AVC_DRIVER_ADDRESS	sizeof(AVC_DRIVER_STRUCT)





//local status for application notification
static AVC_DRIVER_CB_DATA appData;			
static AVCDRIVER_CALLBACK_FUNC appCB;

#define APP_EVT_ATTACHCHG	0x0001
#define APP_EVT_ENABLECHG	0x0002
#define APP_EVT_CLOCKCHG	0x0004
#define APP_EVT_AC3CHG		0x0008

static uint32	appEvents = 0;
					
static BOOL isDriverAttached (void)
{
	BOOL bAttached = FALSE;
	uint32			iPCR = 0;
	uint32			pNodeAddr = 0;

	lalGetThisNodeAddr(&pNodeAddr);
	cmpReadiPCR(0, (uint16) pNodeAddr, &iPCR);
	if (cmpGetPCRState(iPCR) == PLUG_ACTIVE)
	{
		bAttached = TRUE;
	}

	return bAttached;

// bkbk: with AVC PAL we can use this instead
//	return !((AvcDriver.GlobalParameters.Owner.High == 0xffff0000) && 
//	         (AvcDriver.GlobalParameters.Owner.Low == 0x00000000));
}
#ifdef _DICEJR

static const uint32 clksrcDal2driver [] =
{
	DD_CLK_SRC_AES_ANY,		//eDAL_CLK_SRC_AESRX
	DD_CLK_SRC_AES1,		//eDAL_CLK_SRC_AESRX0
	DD_CLK_SRC_AES2,		//eDAL_CLK_SRC_AESRX1
	DD_CLK_SRC_AES3,		//eDAL_CLK_SRC_AESRX2
	DD_CLK_SRC_AES4,		//eDAL_CLK_SRC_AESRX3
	DD_CLK_SRC_ADAT,		//eDAL_CLK_SRC_ADATRX
	DD_CLK_SRC_ADATAUX,		//eDAL_CLK_SRC_ADATRXAUX
	DD_CLK_SRC_WC,			//eDAL_CLK_SRC_WORD
	DD_CLK_SRC_ARX1,		//eDAL_CLK_SRC_AVS_SYT1
	DD_CLK_SRC_ARX2,		//eDAL_CLK_SRC_AVS_SYT2
	DD_CLK_SRC_WC,			//eDAL_CLK_SRC_EXTERNAL
	DD_CLK_SRC_INTERNAL,	//eDAL_CLK_SRC_INTERNAL
	0x0d				 	//eDAL_CLK_SRC_NONE
};

#else

static const uint32 clksrcDal2driver [] =
{
	DD_CLK_SRC_AES_ANY,		//eDAL_CLK_SRC_AESRX
	DD_CLK_SRC_AES1,		//eDAL_CLK_SRC_AESRX0
	DD_CLK_SRC_AES2,		//eDAL_CLK_SRC_AESRX1
	DD_CLK_SRC_AES3,		//eDAL_CLK_SRC_AESRX2
	DD_CLK_SRC_AES4,		//eDAL_CLK_SRC_AESRX3
	DD_CLK_SRC_ADAT,		//eDAL_CLK_SRC_ADATRX
	DD_CLK_SRC_TDIF,		//eDAL_CLK_SRC_TDIFRX
	DD_CLK_SRC_WC,			//eDAL_CLK_SRC_WORD
	DD_CLK_SRC_ARX1,		//eDAL_CLK_SRC_AVS_SYT1
	DD_CLK_SRC_ARX2,		//eDAL_CLK_SRC_AVS_SYT2
	DD_CLK_SRC_ARX3,		//eDAL_CLK_SRC_AVS_SYT3
	DD_CLK_SRC_ARX4,		//eDAL_CLK_SRC_AVS_SYT4
	DD_CLK_SRC_WC,			//eDAL_CLK_SRC_EXTERNAL
	DD_CLK_SRC_INTERNAL,	//eDAL_CLK_SRC_DSAI_RX0
	DD_CLK_SRC_INTERNAL,	//eDAL_CLK_SRC_DSAI_RX1
	DD_CLK_SRC_INTERNAL,	//eDAL_CLK_SRC_DSAI_RX2
	DD_CLK_SRC_INTERNAL,	//eDAL_CLK_SRC_DSAI_RX3
	DD_CLK_SRC_INTERNAL,	//eDAL_CLK_SRC_INTERNAL
	0x0d				 	//eDAL_CLK_SRC_NONE
};


#endif

static void updateSyncInfo(void)
{
	//update the sync info data structure
	DAL_SYNC_SRC_INFO info;
	dalGetSyncSrcInfo(eDAL_INTERFACE_1,&info);
	
	AvcDriver.ExtSyncInfo.src = clksrcDal2driver [info.src];
	AvcDriver.ExtSyncInfo.locked = (uint32)info.locked;
	AvcDriver.ExtSyncInfo.nomRate = (info.nomRate > eDAL_NOMINAL_RATE_192) ? DD_RATE_NONE : info.nomRate;
	AvcDriver.ExtSyncInfo.adatUser = (uint32)info.adatUser;
}

static bool CheckForWrite(uint32 RegionOffSet, uint32 payload)
{
	bool	Result = false;
	int		i;
	
	if((RegionOffSet >= AVC_DRIVER_OFFSET(GlobalParameters.NickName[0])) && ((RegionOffSet + payload) <= AVC_DRIVER_OFFSET(GlobalParameters.ClockSelect)))
		Result = true;
	else if ((RegionOffSet >= AVC_DRIVER_OFFSET(GlobalParameters.ClockSelect)) && ((RegionOffSet + payload) <= AVC_DRIVER_OFFSET(GlobalParameters.ExtendedStatus)))
		Result = true;
	else if((RegionOffSet >= AVC_DRIVER_OFFSET(TxParameters.NumIsocTransmitter)) 
		&& (RegionOffSet < AVC_DRIVER_OFFSET(RxParameters.NumIsocReceiver)))
	{
		for(i=0;i<AvcDriver.TxParameters.NumIsocTransmitter;i++)
		{
			if ((RegionOffSet == AVC_DRIVER_OFFSET(TxParameters.IsocTransmitter[i].ChannelNumber)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == AVC_DRIVER_OFFSET(TxParameters.IsocTransmitter[i].Speed)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == AVC_DRIVER_OFFSET(TxParameters.IsocTransmitter[i].AC3Enable)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else
				Result = false;
		}
	}
	else if((RegionOffSet >= AVC_DRIVER_OFFSET(RxParameters.NumIsocReceiver))
		&& (RegionOffSet < AVC_DRIVER_OFFSET(ExtSyncInfo)))
	{
		for(i=0;i<AvcDriver.RxParameters.NumIsocReceiver;i++)
		{
			if ((RegionOffSet == AVC_DRIVER_OFFSET(RxParameters.IsocReceiver[i].ChannelNumber)) && (payload <= 8))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == AVC_DRIVER_OFFSET(RxParameters.IsocReceiver[i].FirstSequenceNumber)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == AVC_DRIVER_OFFSET(RxParameters.IsocReceiver[i].AC3Enable)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else
				Result = false;
		}
	}
	else
		Result = false;
#ifdef _SYSDEBUG	
	if (Result == false) sysDebugPrintf("AvcDriver, illegal write command! at OffSet = %08X\n\r",RegionOffSet);
#endif // _SYSDEBUG
	return Result;
}


static HRESULT AvcDriverCoreCallback(PB *packetBlock)
{
	HRESULT hResult = NO_ERROR;
	PB_HEADER		*pHeader = NULL;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	PB_LOCKTYPE		lockType;
	OFFSET_1394		OffSetDest;
	uint32			payloadSize = 0;
	uint32			RegionOffSet = 0;	
	QUADLET			*pPayload = NULL;	
	uint32			srcNodeAddr;
	BOOL				bChg;

	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketType(packetBlock,&packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetDestinationOffset(packetBlock, &OffSetDest);
	if (hResult != NO_ERROR) return hResult;

	RegionOffSet = OffSetDest.Low - AVC_DRIVER_BASE_START;	

	hResult = pbGetDataLen(packetBlock,&payloadSize);
	if(hResult != NO_ERROR) 
		return hResult;	

	if (packetType == PB_TYPE_WRITE_REQUEST_QUADLET)
	{
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		payloadSize = 4;

		if(CheckForWrite(RegionOffSet, payloadSize))
		{
			if( RegionOffSet == AVC_DRIVER_OFFSET(GlobalParameters.Enable))
			{
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				appEvents |= APP_EVT_ENABLECHG;
				TCSemaphoreSignal(AvcDriverSemID);
			}
			else if( RegionOffSet == AVC_DRIVER_OFFSET(GlobalParameters.ClockSelect))
			{
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				appEvents |= APP_EVT_CLOCKCHG;
				TCSemaphoreSignal(AvcDriverSemID);
			}
			else if( RegionOffSet == AVC_DRIVER_OFFSET(TxParameters.IsocTransmitter[0].AC3Enable))
			{
				bChg = AvcDriver.TxParameters.IsocTransmitter[0].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(AvcDriverSemID);
				}
			}
			else if( RegionOffSet == AVC_DRIVER_OFFSET(TxParameters.IsocTransmitter[1].AC3Enable))
			{
				bChg = AvcDriver.TxParameters.IsocTransmitter[1].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(AvcDriverSemID);
				}
			}
			else if( RegionOffSet == AVC_DRIVER_OFFSET(RxParameters.IsocReceiver[0].AC3Enable))
			{
				bChg = AvcDriver.RxParameters.IsocReceiver[0].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(AvcDriverSemID);
				}
			}
			else if( RegionOffSet == AVC_DRIVER_OFFSET(RxParameters.IsocReceiver[1].AC3Enable))
			{
				bChg = AvcDriver.RxParameters.IsocReceiver[1].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(AvcDriverSemID);
				}
			}
			else if( RegionOffSet == AVC_DRIVER_OFFSET(RxParameters.IsocReceiver[2].AC3Enable))
			{
				bChg = AvcDriver.RxParameters.IsocReceiver[2].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(AvcDriverSemID);
				}
			}
			else if( RegionOffSet == AVC_DRIVER_OFFSET(RxParameters.IsocReceiver[3].AC3Enable))
			{
				bChg = AvcDriver.RxParameters.IsocReceiver[3].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(AvcDriverSemID);
				}
			}
			else if( RegionOffSet >= (LAST_AVC_DRIVER_ADDRESS + 4))
			{
				hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
			}
			else
			{				
				memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);				
			}			
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);			
		}		
	}
	if (packetType == PB_TYPE_WRITE_REQUEST)
	{
		hResult = pbGetPayload(packetBlock, (void **) &pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;

		if(CheckForWrite(RegionOffSet, payloadSize))
		{			
			memcpy((uint32*) ((int)&AvcDriver + (int)RegionOffSet), pPayload, payloadSize);
			hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
	}

	if (packetType == PB_TYPE_READ_REQUEST)
	{
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;		

		if((RegionOffSet >= AVC_DRIVER_OFFSET(ExtSyncInfo))
		&& (RegionOffSet < sizeof(AVC_DRIVER_STRUCT)))
		{
			updateSyncInfo();
		}
		
		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&AvcDriver + (int)RegionOffSet), TRUE);
		
	}

	if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
	{
		payloadSize = 4;		
		
		if((RegionOffSet >= AVC_DRIVER_OFFSET(ExtSyncInfo))
		&& (RegionOffSet < sizeof(AVC_DRIVER_STRUCT)))
		{
			updateSyncInfo();
		}

		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&AvcDriver + (int)RegionOffSet), TRUE);
		//clear ext status if it has been read as a quadlet
		if (RegionOffSet == AVC_DRIVER_OFFSET(GlobalParameters.ExtendedStatus)) 
			AvcDriver.GlobalParameters.ExtendedStatus &= 0x0000FFFF;
		
	}

	if (packetType == PB_TYPE_LOCK_REQUEST)
	{
		hResult = pbGetLockType(packetBlock, &lockType);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetPayload(packetBlock, (void **) &pPayload);
		if (hResult != NO_ERROR) return hResult;

		if (payloadSize == 16)
		{
			if (lockType == PB_LOCKTYPE_COMPARE_SWAP)
			{
				QUADLET lockArgHi  = pPayload[0];
				QUADLET lockArgLo  = pPayload[1];
				QUADLET lockDataHi = pPayload[2];
				QUADLET lockDataLo = pPayload[3];

				pPayload[0] = AvcDriver.GlobalParameters.Owner.High;
				pPayload[1] = AvcDriver.GlobalParameters.Owner.Low;
				payloadSize /= 2;
				
				if ((lockArgHi == AvcDriver.GlobalParameters.Owner.High) && (lockArgLo == AvcDriver.GlobalParameters.Owner.Low))
				{					
					pbGetPacketSrcNodeAddr(packetBlock,&srcNodeAddr);					
					if( (srcNodeAddr == (lockDataHi >> 16)) || ( (lockDataHi >> 16) == 0xFFFF ) )
					{						
						AvcDriver.GlobalParameters.Owner.High	= lockDataHi;
						AvcDriver.GlobalParameters.Owner.Low 	= lockDataLo;
						appEvents |= APP_EVT_ATTACHCHG;
						TCSemaphoreSignal(AvcDriverSemID);					
					}
					else
					{
						sysDebugPrintf("Lock Data High = %08X Source Node Address = %08X\n\r",lockDataHi,srcNodeAddr);
						hResult = lalReplyLockResponse(packetBlock, RSP_DATA_ERROR, (uint16)payloadSize, pPayload, TRUE);
						return hResult;
					}				
					
				}				
			}
		}
		hResult = lalReplyLockResponse(packetBlock, RSP_COMPLETE, (uint16)payloadSize, pPayload, TRUE);
	}

	return hResult;
}

static void AvcEnableStreaming(void)
{	
	uint32	i,j;
	uint32	AudioSequences;
	uint32	FirstSequence;		
	uint32	SamplingRate;


	//Enable Streaming
	if(AvcDriver.GlobalParameters.Enable)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "Enable Rx\n\r");
		//Setup Rx
		for(i=0;i<AvcDriver.RxParameters.NumIsocReceiver;i++)
		{			
			if(AvcDriver.RxParameters.IsocReceiver[i].ChannelNumber != -1)
			{				
				SamplingRate	=	(AvcDriver.GlobalParameters.ClockSelect>>8)&0xff;
				AudioSequences 	= 	AvcDriver.RxParameters.IsocReceiver[i].NumAudioSequences;
				FirstSequence	=	AvcDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber;			
				
#ifdef AVS_USE_SYT32

#else
				if(SamplingRate > DD_RATE_96K)
				{
					AudioSequences = AudioSequences * 2;
					SamplingRate = SamplingRate - 2;	
				}
#endif
				//Stop Receiver
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStopRx(%d)\n\r", i);
				avsStopRx(i);
				//Setup QSEL
				for(j=0;j<AudioSequences;j++)
				{
					avsSetRxQSelSlot(i,j,FirstSequence+j+1);	
				}
				//Set DBS 				
				avsSetDBS(i,0,AudioSequences);
				//Check for midi
				if(AvcDriver.RxParameters.IsocReceiver[i].NumMidiSequences)
				{
					//Add Midi QSEL Slot
					avsSetRxQSelSlot(i,j,FirstSequence+AudioSequences+1);	
					AudioSequences++;
					//Enable midi on receiver
					avsSetMidi(i,false,true);
					avsEnableMidiInterrupt(true,false,false);
				}
				else
				{
					avsSetMidi(i,false,false);
				}
				//Zero out the others
				for(j=AudioSequences;j<MAX_RX_QSEL;j++)
				{
					avsSetRxQSelSlot(i,j,0);
				}
				//Setup Sampling Rate
				avsSetSampleRate(i,0,(uint8)SamplingRate);			
				//Setup Channel
				if (AvcDriver.RxParameters.IsocReceiver[i].ChannelNumber < 64)
				{
					avsSetCh(i,0,(uint8)AvcDriver.RxParameters.IsocReceiver[i].ChannelNumber);
				}
				//Start Receiver
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStartRx(%d)\n\r", i);
				avsStartRx(i);					
			}
			else
			{
				//Stop Receiver
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStopRx(%d)\n\r", i);
				avsStopRx(i);	
			}
		} 
		//Setup Tx
		SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "Enable Tx\n\r");
		for(i=0;i<AvcDriver.TxParameters.NumIsocTransmitter;i++)
		{
			if(AvcDriver.TxParameters.IsocTransmitter[i].ChannelNumber != -1)
			{	
				//Stop Transmitter
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStopTx(%d)\n\r", i);
				avsStopTx(i);						
				//Write Router Entries
				//Setup Midi
				if(AvcDriver.TxParameters.IsocTransmitter[i].NumMidiSequences)
				{
					// Enables all RX midiPlugs
					// Enable midi on transmitter
					avsSetMidi(i,true,true);					
				}
				else
				{
					avsSetMidi(i,true,false);
				}
				SamplingRate	=	(AvcDriver.GlobalParameters.ClockSelect>>8)&0xff;
				AudioSequences 	= 	AvcDriver.TxParameters.IsocTransmitter[i].NumAudioSequences;
#ifdef AVS_USE_SYT32

#else
				if(SamplingRate > DD_RATE_96K)
				{
					AudioSequences = AudioSequences * 2;
					SamplingRate = SamplingRate - 2;	
				}
#endif
				//Set DBS 				
				avsSetDBS(i,1,AudioSequences);				
				//Setup Sampling Rate				
				avsSetSampleRate(i,1,(uint8)SamplingRate);			
				//Setup Speed				
				avsSetTxSpeed(i,AvcDriver.TxParameters.IsocTransmitter[i].Speed);//SPEED_400);
				//Setup Channel
				if (AvcDriver.TxParameters.IsocTransmitter[i].ChannelNumber < 64)
				{
					avsSetCh(i,1,(uint8)AvcDriver.TxParameters.IsocTransmitter[i].ChannelNumber);
					//Setup Format
					/*
					AC3Capabilities = DiceDriver.TxParameters.IsocTransmitter[i].AC3Capabilities;
					AC3Enable = DiceDriver.TxParameters.IsocTransmitter[i].AC3Enable;
					for(j=0;j<AudioSequences;j++)
					{
						if( AC3Capabilities & AC3Enable & ( 1 << j) )
							avsFmtTxSetLabelCfg(i,j,TXDI_LABEL_TRANSPARENT);
						else
						{
							avsFmtTxSetLabel(i, j, (uint8) 0x40);						
							avsFmtTxSetLabelCfg(i, j, TXDI_LABEL_MASK_MODE);
						}
					}
					*/
				}
				//Setup Format				
				avsFmtConfig(i,1,0,AVS_FMT_LABEL);
				//Start Transmitter
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStartTx(%d)\n\r", i);
				avsStartTx(i);									
			}
			else
			{
				//Stop Transmitter
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStopTx(%d)\n\r", i);
				avsStopTx(i);
			}								
		}
		//Flag Enable
		if (appCB)
		{
			appData.bEnabled = TRUE;
		//	appData.bAttached = isDriverAttached ();
			appCB(eAVCDR_MSG_ENABLE, &appData);
		}
	}
	//Disable Streaming
	else
	{			
		SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "Disable\n\r");
		//Rx
		for(i=0;i<AvcDriver.RxParameters.NumIsocReceiver;i++)
		{			
			//Stop Transmitter
			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStopRx(%d)\n\r", i);
			avsStopRx(i);
		}		
		//Tx
		for(i=0;i<AvcDriver.TxParameters.NumIsocTransmitter;i++)
		{
			//Stop Transmitter
			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avsStopTx(%d)\n\r", i);
			avsStopTx(i);
		}
		if (appCB)
		{
			appData.bEnabled = FALSE;
		//	appData.bAttached = isDriverAttached ();
			appCB(eAVCDR_MSG_DISABLE, &appData);
		}
	}
}

static void busResetIndicationCallback(void)
{
	AvcDriver.GlobalParameters.Enable = false;
	AvcDriver.GlobalParameters.Owner.High 		= 	0xffff0000;
	AvcDriver.GlobalParameters.Owner.Low 		= 	0x00000000;
	appEvents |= APP_EVT_ENABLECHG | APP_EVT_ATTACHCHG;
	TCSemaphoreSignal(AvcDriverSemID); //Make sure all streaming is stopped (indirectly call EnableStreaming)
}


static void AvcDriverThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;
	uint32 events;
	uint32 ClkSrcIndex,ClkRate;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(AvcDriverSemID);
		events = appEvents;
		appEvents = 0;
		if (hResult == NO_ERROR) 
		{
			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "app event: %x\n\r", events);

			if (events & APP_EVT_ATTACHCHG)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "  APP_EVT_ATTACHCHG\n\r");
				if (appCB && (appData.bAttached != isDriverAttached ()))
				{					
					appData.bAttached = isDriverAttached ();
					appCB(appData.bAttached ? eAVCDR_MSG_ATTACH : eAVCDR_MSG_DETACH, &appData);
				}
			}
			if (events & APP_EVT_ENABLECHG)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "  APP_EVT_ENABLECHG\n\r");
				AvcEnableStreaming();
				//if(appCB)
				//{
				//	appData.bEnabled = DiceDriver.GlobalParameters.Enable;
				//	appCB(appData.bEnabled ? eDDR_MSG_ENABLE : eDDR_MSG_DISABLE, &appData);
				//}
			}
			if (events & APP_EVT_CLOCKCHG)
			{				
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "  APP_EVT_CLOCKCHG\n\r");

				ClkSrcIndex = AvcDriver.GlobalParameters.ClockSelect&0xff;
				if (ClkSrcIndex > DD_CLK_SRC_INTERNAL) 
					ClkSrcIndex = DD_CLK_SRC_INTERNAL;
				ClkRate = ((AvcDriver.GlobalParameters.ClockSelect>>8)&0xff);		
				if (ClkRate > DD_RATE_ANY_HIGH) 
					ClkRate = DD_RATE_48K;
				if (appCB )
				{						
					appData.rate = CLOCK_RATE[ClkRate];
					appData.source = CLOCK_SOURCE[ClkSrcIndex];					
					SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "  appCB eAVCDR_MSG_CLOCK_CHG\n\r");
					appCB(eAVCDR_MSG_CLOCK_CHG, &appData);
				}				
			}
			if (events & APP_EVT_AC3CHG)
				if(appCB)
					appCB(eAVCDR_MSG_AC3_CHG, &appData);
		}				
	}
}

static HRESULT AvcDriverRegisterAddressCallbacks(void)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394  AvcDriverAddress, AvcDriverAddressHi;
	
	AvcDriverAddress.High = AVC_DRIVER_BASE_START_HI;  
	AvcDriverAddress.Low  = AVC_DRIVER_BASE_START;  

	AvcDriverAddressHi.High = AVC_DRIVER_BASE_END_HI;  
	AvcDriverAddressHi.Low  = AVC_DRIVER_BASE_END;

	hResult = lalAddressRangeRegisterCB(&AvcDriverCoreCallback,AvcDriverAddress,AvcDriverAddressHi);

	return hResult;
}

//initial and when running
HRESULT avcDriverConfigure (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
							 char * nickName,
                             AVCDRIVER_CALLBACK_FUNC cbFunc)
{
	HRESULT hResult = NO_ERROR;
	uint8 i, plug;
	
	if ((nbTxStreams > NUM_AVS_TX) || (nbRxStreams > NUM_AVS_RX) || !cbFunc)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;	
	}	
	
	appCB = cbFunc;
	AvcDriver.TxParameters.NumIsocTransmitter 	= nbTxStreams;
	//Individual Tx Initialization
	for(i = 0; i < AvcDriver.TxParameters.NumIsocTransmitter; i++)
	{
		AvcDriver.TxParameters.IsocTransmitter[i].ChannelNumber = 0xffffffff;			
		AvcDriver.TxParameters.IsocTransmitter[i].NumAudioSequences = pTxStreamConfig->AudioChannels;
		AvcDriver.TxParameters.IsocTransmitter[i].NumMidiSequences = pTxStreamConfig->midiChannels;
		AvcDriver.TxParameters.IsocTransmitter[i].Speed = SPEED_400;
		strncpy(AvcDriver.TxParameters.IsocTransmitter[i].Names,pTxStreamConfig->names,DD_SIZE_OF_NAMES);
		for (plug = 0; plug < 8; plug++)
		{
			avsSetTxMidiMap(i, (uint8) plug, (uint8) plug);
		}
		pTxStreamConfig++;
	}
	//Main Rx Parameters Intialization
	AvcDriver.RxParameters.NumIsocReceiver = nbRxStreams;
	//Individual Rx Initialization
	for(i=0;i<AvcDriver.RxParameters.NumIsocReceiver;i++)
	{
		AvcDriver.RxParameters.IsocReceiver[i].ChannelNumber = 0xffffffff;
		AvcDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber = 0;		
		AvcDriver.RxParameters.IsocReceiver[i].NumAudioSequences = pRxStreamConfig->AudioChannels;		
		AvcDriver.RxParameters.IsocReceiver[i].NumMidiSequences = pRxStreamConfig->midiChannels;		
		strncpy(AvcDriver.RxParameters.IsocReceiver[i].Names,pRxStreamConfig->names,DD_SIZE_OF_NAMES);	
		for (plug = 0; plug < 8; plug++)
		{
			avsSetRxMidiMap(i, (uint8) plug, (uint8) plug);
		}

		pRxStreamConfig++;
	}
	strncpy(AvcDriver.GlobalParameters.NickName,nickName,DD_SIZE_OF_NICK_NAME);
	
	return hResult;
}

//initial and when running
HRESULT avcDriverConfigureNew (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
                             char * nickName,							 
                             AVCDRIVER_CALLBACK_FUNC cbFunc,
							 uint32 ClockCapabilities,
							 char * ClockSourceNames)
{
	HRESULT hResult = NO_ERROR;
	uint8 i, plug;
	
	if ((nbTxStreams > NUM_AVS_TX) || (nbRxStreams > NUM_AVS_RX) || !cbFunc)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;	
	}	
	
	appCB = cbFunc;
	//before we get too far we will close down the existing Tx and Rx as the new config
	//might have fewer channels now so shut them down
	//Rx
	for(i=0;i<AvcDriver.RxParameters.NumIsocReceiver;i++)
	{			
		//Stop Transmitter
		avsStopRx(i);
	}		
	//Tx
	for(i=0;i<AvcDriver.TxParameters.NumIsocTransmitter;i++)
	{
		//Stop Transmitter
		avsStopTx(i);
	}

	
	AvcDriver.TxParameters.NumIsocTransmitter 	= nbTxStreams;
	//Individual Tx Initialization
	for(i = 0; i < AvcDriver.TxParameters.NumIsocTransmitter; i++)
	{
		AvcDriver.TxParameters.IsocTransmitter[i].ChannelNumber = 0xffffffff;			
		AvcDriver.TxParameters.IsocTransmitter[i].NumAudioSequences = pTxStreamConfig->AudioChannels;
		AvcDriver.TxParameters.IsocTransmitter[i].NumMidiSequences = pTxStreamConfig->midiChannels;
		AvcDriver.TxParameters.IsocTransmitter[i].Speed = SPEED_400;
		strncpy(AvcDriver.TxParameters.IsocTransmitter[i].Names,pTxStreamConfig->names,DD_SIZE_OF_NAMES);
		for (plug = 0; plug < 8; plug++)
		{
			avsSetTxMidiMap(i, (uint8) plug, (uint8) plug);
		}
		if (pTxStreamConfig->AC3Capabilities)
		{			
			//AC3 capture is not supported by driver		
			sysLogError(E_BAD_INPUT_PARAMETERS, __LINE__, moduleName);
		}
		AvcDriver.TxParameters.IsocTransmitter[i].AC3Capabilities = 0;
		pTxStreamConfig++;
	}
	//Main Rx Parameters Intialization
	AvcDriver.RxParameters.NumIsocReceiver = nbRxStreams;
	//Individual Rx Initialization
	for(i=0;i<AvcDriver.RxParameters.NumIsocReceiver;i++)
	{
		AvcDriver.RxParameters.IsocReceiver[i].ChannelNumber = 0xffffffff;
		AvcDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber = 0;		
		AvcDriver.RxParameters.IsocReceiver[i].NumAudioSequences = pRxStreamConfig->AudioChannels;		
		AvcDriver.RxParameters.IsocReceiver[i].NumMidiSequences = pRxStreamConfig->midiChannels;		
		strncpy(AvcDriver.RxParameters.IsocReceiver[i].Names,pRxStreamConfig->names,DD_SIZE_OF_NAMES);	
		for (plug = 0; plug < 8; plug++)
		{
			avsSetRxMidiMap(i, (uint8) plug, (uint8) plug);
		}
		AvcDriver.RxParameters.IsocReceiver[i].AC3Capabilities = pRxStreamConfig->AC3Capabilities;
		pRxStreamConfig++;
	}

	AvcDriver.GlobalParameters.ClockCapabilities = ClockCapabilities;
	strncpy(AvcDriver.GlobalParameters.ClockSourceNames,ClockSourceNames,DD_SIZE_OF_CLK_SRC_NAME);

	strncpy(AvcDriver.GlobalParameters.NickName,nickName,DD_SIZE_OF_NICK_NAME);
	
	return hResult;
}

HRESULT avcDriverConfigureStreaming (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
									  STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams)
{
	return avcDriverConfigureNew (pTxStreamConfig, nbTxStreams, 
                                   pRxStreamConfig, nbRxStreams,
                                   AvcDriver.GlobalParameters.NickName,							 
								   appCB,
								   AvcDriver.GlobalParameters.ClockCapabilities,
							       AvcDriver.GlobalParameters.ClockSourceNames);
}


HRESULT avcDriverConfigureInitial   (char * nickName,
									  AVCDRIVER_CALLBACK_FUNC cbFunc,
							          uint32 ClockCapabilities,
							          char* ClockSourceNames)
{
	HRESULT hResult = NO_ERROR;
	
	AvcDriver.GlobalParameters.ClockCapabilities = ClockCapabilities;
	strncpy(AvcDriver.GlobalParameters.ClockSourceNames,ClockSourceNames,DD_SIZE_OF_CLK_SRC_NAME);

	strncpy(AvcDriver.GlobalParameters.NickName,nickName,DD_SIZE_OF_NICK_NAME);
	appCB = cbFunc;
	
	return hResult;
}


void avcDriverNotifyHost (uint32 notification)
{
	return; // bkbk temp

	uint16 nodeAddr;
	OFFSET_1394	destOffset;

	TCMutexAppLock(semID_notify);

	if (notification && isDriverAttached())
	{
		nodeAddr = 	AvcDriver.GlobalParameters.Owner.High >> 16;
		destOffset.High = AvcDriver.GlobalParameters.Owner.High & 0xffff;
		destOffset.Low = AvcDriver.GlobalParameters.Owner.Low;
		AvcDriver.GlobalParameters.Notification = notification;
		lalWriteNode(nodeAddr,destOffset,sizeof(notification),&AvcDriver.GlobalParameters.Notification);
	}
	TCMutexAppUnlock(semID_notify);
}

HRESULT avcDriverSetStatus (BOOL bLocked, DAL_NOMINAL_RATE_ENUM nominalRate, uint32 rateHz, uint32 extStatus)
{
	HRESULT hResult = NO_ERROR;
	uint32 notify = 0;
	uint32 previousNominalRate;
	
	
	if (nominalRate > eDAL_NOMINAL_RATE_192) nominalRate = DD_RATE_NONE;
	AvcDriver.GlobalParameters.SampleRate = rateHz;
	
	if (bLocked)
	{
		if (!(AvcDriver.GlobalParameters.Status & DD_STATUS_SRC_LOCKED))
		{
			//clock has been locked
			AvcDriver.GlobalParameters.Status = DD_STATUS_SRC_LOCKED;
			notify |= DD_NOTIFY_LOCK_CHG_BIT;
		}
	}
	else
	{
		if (AvcDriver.GlobalParameters.Status & DD_STATUS_SRC_LOCKED)
		{
			//clock has been unlocked
			AvcDriver.GlobalParameters.Status = 0;
			notify |= DD_NOTIFY_LOCK_CHG_BIT;
		}
	}
	previousNominalRate = ((AvcDriver.GlobalParameters.Status & DD_STATUS_NOM_RATE_MASK) >> DD_STATUS_NOM_RATE_SHIFT);
	if (previousNominalRate != nominalRate)
		notify |= DD_NOTIFY_LOCK_CHG_BIT;
	if((AvcDriver.GlobalParameters.ExtendedStatus & 0xFFFF) != (extStatus & 0xFFFF))
		notify |= DD_INTERFACE_CHG_BIT;
	AvcDriver.GlobalParameters.ExtendedStatus = extStatus;
	AvcDriver.GlobalParameters.Status &= (~DD_STATUS_NOM_RATE_MASK);
	AvcDriver.GlobalParameters.Status |= ((nominalRate << DD_STATUS_NOM_RATE_SHIFT) & DD_STATUS_NOM_RATE_MASK);
	avcDriverNotifyHost (notify);
	return hResult;
}



HRESULT avcDriverInitialize(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32 i,plug;

	
	//AvcDriver Initialization
	AvcDriver.GlobalParametersOffSet		= 	AVC_DRIVER_OFFSET(GlobalParameters)/4;
	AvcDriver.GlobalParametersSize			=	sizeof(DD_GLOBAL_PARAMETERS)/4;
	AvcDriver.TxParametersOffSet			=	AVC_DRIVER_OFFSET(TxParameters)/4;
	AvcDriver.TxParametersSize				=	sizeof(DD_TX_PARAMETERS)/4;
	AvcDriver.RxParametersOffSet			=	AVC_DRIVER_OFFSET(RxParameters)/4;
	AvcDriver.RxParametersSize				=	sizeof(DD_RX_PARAMETERS)/4;
	AvcDriver.Dummy1ParametersOffSet		=   0;
	AvcDriver.Dummy1ParametersSize			=	0;
	AvcDriver.Dummy2UpdateSpaceOffSet		=	0;
	AvcDriver.Dummy2UpdateSpaceSize			=	0;
	//Global Parameters Initialization
	AvcDriver.GlobalParameters.Owner.High 		= 	0xffff0000;
	AvcDriver.GlobalParameters.Owner.Low 		= 	0x00000000;
	AvcDriver.GlobalParameters.Notification	=	0x0;	
	strcpy(AvcDriver.GlobalParameters.NickName,"TCAT DICE");	
	AvcDriver.GlobalParameters.ClockSelect		= 	((DD_RATE_48K<<8) + DD_CLK_SRC_INTERNAL);
	AvcDriver.GlobalParameters.Enable			=	0;
	AvcDriver.GlobalParameters.Status			= 	0;	
	AvcDriver.GlobalParameters.ExtendedStatus	=	0;
	AvcDriver.GlobalParameters.SampleRate		=	0;
	//Main Tx Parameters Intialization
	AvcDriver.TxParameters.IsocTransmitterSize = sizeof(DD_ISOC_TRANSMITTER)/4;
	AvcDriver.TxParameters.NumIsocTransmitter 	= 1;
	//Individual Tx Initialization
	for(i = 0; i < AvcDriver.TxParameters.NumIsocTransmitter; i++)
	{
		AvcDriver.TxParameters.IsocTransmitter[i].ChannelNumber = 0xffffffff;
		AvcDriver.TxParameters.IsocTransmitter[i].NumAudioSequences = 2;
		AvcDriver.TxParameters.IsocTransmitter[i].NumMidiSequences = 0;
		AvcDriver.TxParameters.IsocTransmitter[i].Speed = SPEED_400;
		strcpy(AvcDriver.TxParameters.IsocTransmitter[i].Names,"Main L\\Main R\\\\");
		for (plug = 0; plug < 8; plug++)
		{
			avsSetTxMidiMap(i, (uint8) plug, (uint8) plug);
		}

	}
	//Main Rx Parameters Intialization
	AvcDriver.RxParameters.NumIsocReceiver = 1;
	AvcDriver.RxParameters.IsocReceiverSize = sizeof(DD_ISOC_RECEIVER)/4;
	//Individual Rx Initialization
	for(i=0;i<AvcDriver.RxParameters.NumIsocReceiver;i++)
	{
		AvcDriver.RxParameters.IsocReceiver[i].ChannelNumber = 0xffffffff;
		AvcDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber = 0;
		AvcDriver.RxParameters.IsocReceiver[i].NumAudioSequences = 2;		
		AvcDriver.RxParameters.IsocReceiver[i].NumMidiSequences = 0;		
		strcpy(AvcDriver.RxParameters.IsocReceiver[i].Names,"Main L\\Main R\\\\");	
		for (plug = 0; plug < 8; plug++)
		{
			avsSetRxMidiMap(i, (uint8) plug, (uint8) plug);
		}
	}

	//AvcDriver version number
	AvcDriver.GlobalParameters.Version = AVC_DRIVER_VERSION_NUMBER;

	//Create Semaphore
	hResult = TCSemaphoreOpen(&AvcDriverSemID, 0);
	if (hResult != NO_ERROR) return hResult;
	
	//Resume the task
	hResult = avcDriverResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	//register the address range
	hResult = AvcDriverRegisterAddressCallbacks();
	if (hResult != NO_ERROR) return hResult;

    //install bus reset indication function
    hResult = lalRegisterBRIndicationCallback(busResetIndicationCallback);
    if (hResult != NO_ERROR) return hResult;

	hResult = TCMutexAppOpen(&semID_notify);
	
#ifdef _CLI
	avcDriverCliInstallTools();
#endif //_CLI	

	return hResult;
}

AVC_DRIVER_STRUCT*	avcDriverGetState(void)
{
	return &AvcDriver;
}

/*********************************************************
	Resume tasks for this module
*/	
HRESULT avcDriverResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskResume(AvcDriverThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avcDriverSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskSuspend(AvcDriverThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcDriverInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskCreate(&AvcDriverThreadID, AvcDriverThreadName, AvcDriverThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// map table 5.7 in AVC Stream Fmt Spec 1.1 to DAL rates from DAL_NOMINAL_RATE_ENUM
BYTE rateMap[] = {				// Stream Format rate
	0xff,						// 0, 22.05kHz not supported in DAL
	0xff,						// 1, 24kHz not supported
	0,							// 2, 32kHz
	1,							// 3, 44.1kHz
	2,							// 4, 48kHz
	4,							// 5, 96kHz
	5,							// 6, 176.4kHz
	6,							// 7, 192kHz
	0xff,						// rsvd
	0xff,						// rsvd
	3,							// A, 88kHz
	0xff,						// rsvd
	0xff,						// rsvd
	0xff,						// rsvd
	0xff,						// rsvd
	0x0f						// F, any
};

// map cmp rate to dal rate
HRESULT avcDriverMapSampleRate(BYTE cmpRate, BYTE *dalRate)
{
	HRESULT hResult = NO_ERROR;

	if (cmpRate > 0x0f)
	{
		return E_FAIL;
	}

	*dalRate = rateMap[cmpRate];
	
	return hResult;
}

HRESULT avcDriverSetRate(DAL_NOMINAL_RATE_ENUM dal_rate)
{
	HRESULT hResult = NO_ERROR;
	uint32	clockSelect = 0;

	SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avcDriverSetRate: dal_rate %d\n\r", dal_rate);

	clockSelect = AvcDriver.GlobalParameters.ClockSelect;
	clockSelect &= 0xff;
	clockSelect |= (dal_rate<<8);
	AvcDriver.GlobalParameters.ClockSelect = clockSelect;
	appEvents |= APP_EVT_CLOCKCHG;
	hResult = TCSemaphoreSignal(AvcDriverSemID);

	return hResult;
}

HRESULT avcDriverGetRate(DAL_NOMINAL_RATE_ENUM *rate)
{
	*rate = (AvcDriver.GlobalParameters.ClockSelect & 0xff00)>>8;

	return NO_ERROR;
}

BOOL avcDriverPlugIsSyncSource(BYTE dst_su, BYTE dst_plug_id, BYTE src_su, BYTE src_plug_id)
{
	return targetPlugIsSyncSource(src_su, src_plug_id);
}

BOOL avcDriverPlugIsSyncPath(BYTE dst_su, BYTE dst_plug_id, BYTE src_su, BYTE src_plug_id)
{
	BOOL bResult = FALSE;

	bResult = targetPlugIsSyncPlug(dst_su, dst_plug_id);
	if (bResult)
	{
		bResult = targetPlugIsSyncSource(src_su, src_plug_id);
	}

	return bResult;
}

HRESULT avcDriverSetClockSrc(uint8 src_su, uint8 src_plug_id)
{
	HRESULT hResult = NO_ERROR;
	DAL_CLOCK_SOURCE_ENUM clockVal = 0;
	uint32 clockSelect = 0;

	hResult = targetPlugGetClockSource(src_su, src_plug_id, &clockVal);
	if (NO_ERROR != hResult) return hResult;
	
	SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avcDriverSetClockSrc: %x\n\r", clockVal);

	clockSelect = AvcDriver.GlobalParameters.ClockSelect;
	clockSelect &= 0xff00;
	clockSelect |= clockVal;

	AvcDriver.GlobalParameters.ClockSelect = clockSelect;
	appEvents |= APP_EVT_CLOCKCHG;
	hResult = TCSemaphoreSignal(AvcDriverSemID);

	return hResult;
}

HRESULT avcDriverGetClockSrc(DAL_CLOCK_SOURCE_ENUM *clkSrc)
{
	*clkSrc = AvcDriver.GlobalParameters.ClockSelect;

	return NO_ERROR;
}

HRESULT avcDriverSetCh(uint32 plugID, BOOL bTx, uint8 channel)
{
	HRESULT hResult = NO_ERROR;

	SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avcDriverSetCh, plug: %x , bTx: %x, ch: %x\n\r", plugID, bTx, channel);

	if (bTx)
	{
		if (0 == plugID)
		{
			if (channel == AvcDriver.TxParameters.IsocTransmitter[1].ChannelNumber)
			{
				channel = -1;
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "error: avcDriverSetCh, duplicate tx channel");
			}
		}
		else if (1 == plugID)
		{
			if (channel == AvcDriver.TxParameters.IsocTransmitter[0].ChannelNumber)
			{
				channel = -1;
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "error: avcDriverSetCh, duplicate tx channel");
			}
		}
		AvcDriver.TxParameters.IsocTransmitter[plugID].ChannelNumber = channel;
	}
	else
	{
		if (0 == plugID)
		{
			if (channel == AvcDriver.RxParameters.IsocReceiver[1].ChannelNumber)
			{
				channel = -1;
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "error: avcDriverSetCh, duplicate rx channel");
			}
		}
		else if (1 == plugID)
		{
			if (channel == AvcDriver.RxParameters.IsocReceiver[0].ChannelNumber)
			{
				channel = -1;
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "error: avcDriverSetCh, duplicate rx channel");
			}
		}
		AvcDriver.RxParameters.IsocReceiver[plugID].ChannelNumber = channel;
	}

	return hResult;
}

HRESULT avcDriverSetEnable(BOOL bEnable)
{
	HRESULT	hResult = NO_ERROR;

	SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "avcDriverEnable: %x\n\r", bEnable);
	AvcDriver.GlobalParameters.Enable = bEnable;

	appEvents |= APP_EVT_ENABLECHG;
	hResult = TCSemaphoreSignal(AvcDriverSemID);

	return hResult;
}


HRESULT extFmt2DalRate(BYTE ext_fmt_rate, DAL_NOMINAL_RATE_ENUM *dal_rate)
{
	HRESULT hResult = NO_ERROR;

	switch (ext_fmt_rate)
	{
	case STREAM_FMT_SAMPLING_FREQ_32000:
		*dal_rate = eDAL_NOMINAL_RATE_32;
		break;
	case STREAM_FMT_SAMPLING_FREQ_44100:
		*dal_rate = eDAL_NOMINAL_RATE_44_1;
		break;
	case STREAM_FMT_SAMPLING_FREQ_48000:
		*dal_rate = eDAL_NOMINAL_RATE_48;
		break;
	case STREAM_FMT_SAMPLING_FREQ_88200:
		*dal_rate = eDAL_NOMINAL_RATE_88_2;
		break;
	case STREAM_FMT_SAMPLING_FREQ_96000:
		*dal_rate = eDAL_NOMINAL_RATE_96;
		break;
	case STREAM_FMT_SAMPLING_FREQ_176400:
		*dal_rate = eDAL_NOMINAL_RATE_176_4;
		break;
	case STREAM_FMT_SAMPLING_FREQ_192000:
		*dal_rate = eDAL_NOMINAL_RATE_192;
		break;

	default:
		*dal_rate = 0xff;
		hResult = E_FAIL;
		break;
	}

	return hResult;
}

HRESULT dal2ExtFmtRate(DAL_NOMINAL_RATE_ENUM dal_rate, BYTE *ext_fmt_rate)
{
	HRESULT hResult = NO_ERROR;

	switch (dal_rate)
	{
	case eDAL_NOMINAL_RATE_32:
		*ext_fmt_rate = STREAM_FMT_SAMPLING_FREQ_32000;
		break;
	case eDAL_NOMINAL_RATE_44_1:
		*ext_fmt_rate = STREAM_FMT_SAMPLING_FREQ_44100;
		break;
	case eDAL_NOMINAL_RATE_48:
		*ext_fmt_rate = STREAM_FMT_SAMPLING_FREQ_48000;
		break;
	case eDAL_NOMINAL_RATE_88_2:
		*ext_fmt_rate = STREAM_FMT_SAMPLING_FREQ_88200;
		break;
	case eDAL_NOMINAL_RATE_96:
		*ext_fmt_rate = STREAM_FMT_SAMPLING_FREQ_96000;
		break;
	case eDAL_NOMINAL_RATE_176_4:
		*ext_fmt_rate = STREAM_FMT_SAMPLING_FREQ_176400;
		break;
	case eDAL_NOMINAL_RATE_192:
		*ext_fmt_rate = STREAM_FMT_SAMPLING_FREQ_192000;
		break;

	default:
		*ext_fmt_rate = 0xff;
		hResult = E_FAIL;
		break;
	}

	return hResult;
}



#endif //_AVC

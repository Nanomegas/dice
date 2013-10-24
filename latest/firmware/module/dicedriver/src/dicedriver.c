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
 * DESCRIPTION: DiceDriver

	Purpose:	DiceDriver
				

	Revisions:
		created 21/12/2004 AliA
		rewritten 21/03/2005 ml

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
#include "dicedriver.h"
#include "diceDriverInterface.h"
#include "targetBoard.h"

#ifdef _CLI
#include "dicedriverCli.h"
#endif //_CLI

#include <cyg/hal/hal_platform_ints.h> //for interrupt number

#ifdef _DICE_DRIVER

static char 	moduleName[] = __FILE__ ;


// task (thread) ID for this module
uint32				DiceDriverThreadID;
const char*			DiceDriverThreadName = "DiceDriverThread";
uint32				DiceDriverSemID;
uint32				semID_notify;

static DICE_DRIVER_STRUCT	DiceDriver;


uint32	CLOCK_SOURCE[]= {	eDAL_CLK_SRC_AESRX0,
							eDAL_CLK_SRC_AESRX1,
							eDAL_CLK_SRC_AESRX2,
							eDAL_CLK_SRC_AESRX3,
							eDAL_CLK_SRC_AESRX,
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
#define DICE_DRIVER_OFFSET(a)		((uint32)(&DiceDriver.a) - (uint32)&DiceDriver)
#define	LAST_DICE_DRIVER_ADDRESS	sizeof(DICE_DRIVER_STRUCT)





//local status for application notification
static DICE_DRIVER_CB_DATA appData;			
static DICEDRIVER_CALLBACK_FUNC appCB;

#define APP_EVT_ATTACHCHG	0x0001
#define APP_EVT_ENABLECHG	0x0002
#define APP_EVT_CLOCKCHG	0x0004
#define APP_EVT_AC3CHG		0x0008

static uint32	appEvents = 0;
					
static BOOL isDriverAttached (void)
{
	return !((DiceDriver.GlobalParameters.Owner.High == 0xffff0000) && 
	         (DiceDriver.GlobalParameters.Owner.Low == 0x00000000));
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
	
	DiceDriver.ExtSyncInfo.src = clksrcDal2driver [info.src];
	DiceDriver.ExtSyncInfo.locked = (uint32)info.locked;
	DiceDriver.ExtSyncInfo.nomRate = (info.nomRate > eDAL_NOMINAL_RATE_192) ? DD_RATE_NONE : info.nomRate;
	DiceDriver.ExtSyncInfo.adatUser = (uint32)info.adatUser;
}

static bool CheckForWrite(uint32 RegionOffSet, uint32 payload)
{
	bool	Result = false;
	int		i;
	
	if((RegionOffSet >= DICE_DRIVER_OFFSET(GlobalParameters.NickName[0])) && ((RegionOffSet + payload) <= DICE_DRIVER_OFFSET(GlobalParameters.ClockSelect)))
		Result = true;
	else if ((RegionOffSet >= DICE_DRIVER_OFFSET(GlobalParameters.ClockSelect)) && ((RegionOffSet + payload) <= DICE_DRIVER_OFFSET(GlobalParameters.ExtendedStatus)))
		Result = true;
	else if((RegionOffSet >= DICE_DRIVER_OFFSET(TxParameters.NumIsocTransmitter)) 
		&& (RegionOffSet < DICE_DRIVER_OFFSET(RxParameters.NumIsocReceiver)))
	{
		for(i=0;i<DiceDriver.TxParameters.NumIsocTransmitter;i++)
		{
			if ((RegionOffSet == DICE_DRIVER_OFFSET(TxParameters.IsocTransmitter[i].ChannelNumber)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == DICE_DRIVER_OFFSET(TxParameters.IsocTransmitter[i].Speed)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == DICE_DRIVER_OFFSET(TxParameters.IsocTransmitter[i].AC3Enable)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else
				Result = false;
		}
	}
	else if((RegionOffSet >= DICE_DRIVER_OFFSET(RxParameters.NumIsocReceiver))
		&& (RegionOffSet < DICE_DRIVER_OFFSET(ExtSyncInfo)))
	{
		for(i=0;i<DiceDriver.RxParameters.NumIsocReceiver;i++)
		{
			if ((RegionOffSet == DICE_DRIVER_OFFSET(RxParameters.IsocReceiver[i].ChannelNumber)) && (payload <= 8))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == DICE_DRIVER_OFFSET(RxParameters.IsocReceiver[i].FirstSequenceNumber)) && (payload == 4))
			{
				Result = true;
				return Result;
			}
			else if ((RegionOffSet == DICE_DRIVER_OFFSET(RxParameters.IsocReceiver[i].AC3Enable)) && (payload == 4))
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
	if (Result == false) sysDebugPrintf("DiceDriver, illegal write command! at OffSet = %08X\n\r",RegionOffSet);
#endif // _SYSDEBUG
	return Result;
}


static HRESULT DiceDriverCoreCallback(PB *packetBlock)
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

	RegionOffSet = OffSetDest.Low - DICE_DRIVER_BASE_START;	

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
			if( RegionOffSet == DICE_DRIVER_OFFSET(GlobalParameters.Enable))
			{
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				appEvents |= APP_EVT_ENABLECHG;
				TCSemaphoreSignal(DiceDriverSemID);
			}
			else if( RegionOffSet == DICE_DRIVER_OFFSET(GlobalParameters.ClockSelect))
			{
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				appEvents |= APP_EVT_CLOCKCHG;
				TCSemaphoreSignal(DiceDriverSemID);
			}
			else if( RegionOffSet == DICE_DRIVER_OFFSET(TxParameters.IsocTransmitter[0].AC3Enable))
			{
				bChg = DiceDriver.TxParameters.IsocTransmitter[0].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(DiceDriverSemID);
				}
			}
			else if( RegionOffSet == DICE_DRIVER_OFFSET(TxParameters.IsocTransmitter[1].AC3Enable))
			{
				bChg = DiceDriver.TxParameters.IsocTransmitter[1].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(DiceDriverSemID);
				}
			}
			else if( RegionOffSet == DICE_DRIVER_OFFSET(RxParameters.IsocReceiver[0].AC3Enable))
			{
				bChg = DiceDriver.RxParameters.IsocReceiver[0].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(DiceDriverSemID);
				}
			}
			else if( RegionOffSet == DICE_DRIVER_OFFSET(RxParameters.IsocReceiver[1].AC3Enable))
			{
				bChg = DiceDriver.RxParameters.IsocReceiver[1].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(DiceDriverSemID);
				}
			}
			else if( RegionOffSet == DICE_DRIVER_OFFSET(RxParameters.IsocReceiver[2].AC3Enable))
			{
				bChg = DiceDriver.RxParameters.IsocReceiver[2].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(DiceDriverSemID);
				}
			}
			else if( RegionOffSet == DICE_DRIVER_OFFSET(RxParameters.IsocReceiver[3].AC3Enable))
			{
				bChg = DiceDriver.RxParameters.IsocReceiver[3].AC3Enable != *pPayload;
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				if (bChg)
				{
					appEvents |= APP_EVT_AC3CHG;
					TCSemaphoreSignal(DiceDriverSemID);
				}
			}
			else if( RegionOffSet >= (LAST_DICE_DRIVER_ADDRESS + 4))
			{
				hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
			}
			else
			{				
				memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
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
			memcpy((uint32*) ((int)&DiceDriver + (int)RegionOffSet), pPayload, payloadSize);
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

		if((RegionOffSet >= DICE_DRIVER_OFFSET(ExtSyncInfo))
		&& (RegionOffSet < sizeof(DICE_DRIVER_STRUCT)))
		{
			updateSyncInfo();
		}
		
		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&DiceDriver + (int)RegionOffSet), TRUE);
		
	}

	if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
	{
		payloadSize = 4;		
		
		if((RegionOffSet >= DICE_DRIVER_OFFSET(ExtSyncInfo))
		&& (RegionOffSet < sizeof(DICE_DRIVER_STRUCT)))
		{
			updateSyncInfo();
		}

		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize, (uint32*) ((int)&DiceDriver + (int)RegionOffSet), TRUE);
		//clear ext status if it has been read as a quadlet
		if (RegionOffSet == DICE_DRIVER_OFFSET(GlobalParameters.ExtendedStatus)) 
			DiceDriver.GlobalParameters.ExtendedStatus &= 0x0000FFFF;
		
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

				pPayload[0] = DiceDriver.GlobalParameters.Owner.High;
				pPayload[1] = DiceDriver.GlobalParameters.Owner.Low;
				payloadSize /= 2;
				
				if ((lockArgHi == DiceDriver.GlobalParameters.Owner.High) && (lockArgLo == DiceDriver.GlobalParameters.Owner.Low))
				{					
					pbGetPacketSrcNodeAddr(packetBlock,&srcNodeAddr);					
					if( (srcNodeAddr == (lockDataHi >> 16)) || ( (lockDataHi >> 16) == 0xFFFF ) )
					{						
						DiceDriver.GlobalParameters.Owner.High	= lockDataHi;
						DiceDriver.GlobalParameters.Owner.Low 	= lockDataLo;
						appEvents |= APP_EVT_ATTACHCHG;
						TCSemaphoreSignal(DiceDriverSemID);					
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

static void EnableStreaming(void)
{	
	uint32	i,j;
	uint32	AudioSequences;
	uint32	FirstSequence;		
	uint32	SamplingRate;
	//uint32	AC3Capabilities;
	//uint32	AC3Enable;

	//Enable Streaming
	if(DiceDriver.GlobalParameters.Enable)
	{
		//sysDebugPrintf("Enable\n\r");
		//Setup Rx
		for(i=0;i<DiceDriver.RxParameters.NumIsocReceiver;i++)
		{			
			if(DiceDriver.RxParameters.IsocReceiver[i].ChannelNumber != -1)
			{				
				SamplingRate	=	(DiceDriver.GlobalParameters.ClockSelect>>8)&0xff;
				AudioSequences 	= 	DiceDriver.RxParameters.IsocReceiver[i].NumAudioSequences;
				FirstSequence	=	DiceDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber;			
				
#ifdef AVS_USE_SYT32

#else
				if(SamplingRate > DD_RATE_96K)
				{
					AudioSequences = AudioSequences * 2;
					SamplingRate = SamplingRate - 2;	
				}
#endif
				//Stop Receiver
				avsStopRx(i);
				//Setup QSEL
				for(j=0;j<AudioSequences;j++)
				{
					avsSetRxQSelSlot(i,j,FirstSequence+j+1);	
				}
				//Set DBS 				
				avsSetDBS(i,0,AudioSequences);
				//Check for midi
				if(DiceDriver.RxParameters.IsocReceiver[i].NumMidiSequences)
				{
					//Add Midi QSEL Slot
					avsSetRxQSelSlot(i,j,FirstSequence+AudioSequences+1);	
					AudioSequences++;
					//Enable midi on receiver
					avsSetMidi(i,false,true);
#ifdef _DICE2
					avsEnableMidiInterrupt(true,false,false);
#else
					avsEnableMidiInterrupt(true,true,true);
#endif
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
				avsSetCh(i,0,(uint8)DiceDriver.RxParameters.IsocReceiver[i].ChannelNumber);
				//Adjust Presenetation Time
				//avsSetRxPresntAdjust(i,-24,0);
				//Start Receiver
				avsStartRx(i);				
			}
			else
			{
				//Stop Receiver
				avsStopRx(i);	
			}
		}
		//Setup Tx
		for(i=0;i<DiceDriver.TxParameters.NumIsocTransmitter;i++)
		{
			if(DiceDriver.TxParameters.IsocTransmitter[i].ChannelNumber != -1)
			{	
				//Stop Transmitter
				avsStopTx(i);						
				//Write Router Entries
				//Setup Midi
				if(DiceDriver.TxParameters.IsocTransmitter[i].NumMidiSequences)
				{
					// Enables all RX midiPlugs
					// Enable midi on transmitter
					avsSetMidi(i,true,true);					
				}
				else
				{
					avsSetMidi(i,true,false);
				}
				SamplingRate	=	(DiceDriver.GlobalParameters.ClockSelect>>8)&0xff;
				AudioSequences 	= 	DiceDriver.TxParameters.IsocTransmitter[i].NumAudioSequences;
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
				avsSetTxSpeed(i,DiceDriver.TxParameters.IsocTransmitter[i].Speed);//SPEED_400);
				//Setup Channel
				avsSetCh(i,1,(uint8)DiceDriver.TxParameters.IsocTransmitter[i].ChannelNumber);
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
				avsFmtConfig(i,1,0,AVS_FMT_LABEL);
				//Start Transmitter
				avsStartTx(i);									
			}
			else
			{
				//Stop Transmitter
				avsStopTx(i);
			}								
		}
		//Flag Enable
		if (appCB)
		{
			appData.bEnabled = TRUE;
		//	appData.bAttached = isDriverAttached ();
			appCB(eDDR_MSG_ENABLE, &appData);
		}
	}
	//Disable Streaming
	else
	{			
		//Rx
		for(i=0;i<DiceDriver.RxParameters.NumIsocReceiver;i++)
		{			
			//Stop Transmitter
			avsStopRx(i);
		}		
		//Tx
		for(i=0;i<DiceDriver.TxParameters.NumIsocTransmitter;i++)
		{
			//Stop Transmitter
			avsStopTx(i);
		}
		if (appCB)
		{
			appData.bEnabled = FALSE;
		//	appData.bAttached = isDriverAttached ();
			appCB(eDDR_MSG_DISABLE, &appData);
		}
	}
}

static void busResetIndicationCallback(void)
{
	DiceDriver.GlobalParameters.Enable = false;
	DiceDriver.GlobalParameters.Owner.High 		= 	0xffff0000;
	DiceDriver.GlobalParameters.Owner.Low 		= 	0x00000000;
	appEvents |= APP_EVT_ENABLECHG | APP_EVT_ATTACHCHG;
	TCSemaphoreSignal(DiceDriverSemID); //Make sure all streaming is stopped (indirectly call EnableStreaming)
}

static void DiceDriverThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;
	uint32 events;
	uint32 ClkSrcIndex,ClkRate;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(DiceDriverSemID);
		events = appEvents;
		appEvents = 0;
		if (hResult == NO_ERROR) 
		{
			if (events & APP_EVT_ATTACHCHG)
			{
				if (appCB && (appData.bAttached != isDriverAttached ()))
				{					
					appData.bAttached = isDriverAttached ();
					appCB(appData.bAttached ? eDDR_MSG_ATTACH : eDDR_MSG_DETACH, &appData);
				}
			}
			if (events & APP_EVT_ENABLECHG)
			{
				EnableStreaming();
				//if(appCB)
				//{
				//	appData.bEnabled = DiceDriver.GlobalParameters.Enable;
				//	appCB(appData.bEnabled ? eDDR_MSG_ENABLE : eDDR_MSG_DISABLE, &appData);
				//}
			}
			if (events & APP_EVT_CLOCKCHG)
			{				
				ClkSrcIndex = DiceDriver.GlobalParameters.ClockSelect&0xff;
				if (ClkSrcIndex > DD_CLK_SRC_INTERNAL) 
					ClkSrcIndex = DD_CLK_SRC_INTERNAL;
				ClkRate = ((DiceDriver.GlobalParameters.ClockSelect>>8)&0xff);		
				if (ClkRate > DD_RATE_ANY_HIGH) 
					ClkRate = DD_RATE_48K;
				if (appCB )
				{						
					appData.rate = CLOCK_RATE[ClkRate];
					appData.source = CLOCK_SOURCE[ClkSrcIndex];					
					appCB(eDDR_MSG_CLOCK_CHG, &appData);
				}				
			}
			if (events & APP_EVT_AC3CHG)
				if(appCB)
					appCB(eDDR_MSG_AC3_CHG, &appData);
		}				
	}
}

static HRESULT DiceDriverRegisterAddressCallbacks(void)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394  DiceDriverAddress, DiceDriverAddressHi;
	
	DiceDriverAddress.High = DICE_DRIVER_BASE_START_HI;  
	DiceDriverAddress.Low  = DICE_DRIVER_BASE_START;  

	DiceDriverAddressHi.High = DICE_DRIVER_BASE_END_HI;  
	DiceDriverAddressHi.Low  = DICE_DRIVER_BASE_END;

	hResult = lalAddressRangeRegisterCB(&DiceDriverCoreCallback,DiceDriverAddress,DiceDriverAddressHi);

	return hResult;
}

//initial and when running
HRESULT diceDriverConfigure (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
                             char * nickName,
                             DICEDRIVER_CALLBACK_FUNC cbFunc)
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
	DiceDriver.TxParameters.NumIsocTransmitter 	= nbTxStreams;
	//Individual Tx Initialization
	for(i = 0; i < DiceDriver.TxParameters.NumIsocTransmitter; i++)
	{
		DiceDriver.TxParameters.IsocTransmitter[i].ChannelNumber = 0xffffffff;			
		DiceDriver.TxParameters.IsocTransmitter[i].NumAudioSequences = pTxStreamConfig->AudioChannels;
		DiceDriver.TxParameters.IsocTransmitter[i].NumMidiSequences = pTxStreamConfig->midiChannels;
		DiceDriver.TxParameters.IsocTransmitter[i].Speed = SPEED_400;
		strncpy(DiceDriver.TxParameters.IsocTransmitter[i].Names,pTxStreamConfig->names,DD_SIZE_OF_NAMES);
		for (plug = 0; plug < 8; plug++)
		{
			avsSetTxMidiMap(i, (uint8) plug, (uint8) plug);
		}
		pTxStreamConfig++;
	}
	//Main Rx Parameters Intialization
	DiceDriver.RxParameters.NumIsocReceiver = nbRxStreams;
	//Individual Rx Initialization
	for(i=0;i<DiceDriver.RxParameters.NumIsocReceiver;i++)
	{
		DiceDriver.RxParameters.IsocReceiver[i].ChannelNumber = 0xffffffff;
		DiceDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber = 0;		
		DiceDriver.RxParameters.IsocReceiver[i].NumAudioSequences = pRxStreamConfig->AudioChannels;		
		DiceDriver.RxParameters.IsocReceiver[i].NumMidiSequences = pRxStreamConfig->midiChannels;		
		strncpy(DiceDriver.RxParameters.IsocReceiver[i].Names,pRxStreamConfig->names,DD_SIZE_OF_NAMES);	
		for (plug = 0; plug < 8; plug++)
		{
			avsSetRxMidiMap(i, (uint8) plug, (uint8) plug);
		}
		
		pRxStreamConfig++;
	}
	strncpy(DiceDriver.GlobalParameters.NickName,nickName,DD_SIZE_OF_NICK_NAME);
	
	return hResult;
}

//initial and when running
HRESULT diceDriverConfigureNew (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
                             STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams,
                             char * nickName,							 
                             DICEDRIVER_CALLBACK_FUNC cbFunc,
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
	for(i=0;i<DiceDriver.RxParameters.NumIsocReceiver;i++)
	{			
		//Stop Transmitter
		avsStopRx(i);
	}		
	//Tx
	for(i=0;i<DiceDriver.TxParameters.NumIsocTransmitter;i++)
	{
		//Stop Transmitter
		avsStopTx(i);
	}

	DiceDriver.TxParameters.NumIsocTransmitter 	= nbTxStreams;
	//Individual Tx Initialization
	for(i = 0; i < DiceDriver.TxParameters.NumIsocTransmitter; i++)
	{
		DiceDriver.TxParameters.IsocTransmitter[i].ChannelNumber = 0xffffffff;			
		DiceDriver.TxParameters.IsocTransmitter[i].NumAudioSequences = pTxStreamConfig->AudioChannels;
		DiceDriver.TxParameters.IsocTransmitter[i].NumMidiSequences = pTxStreamConfig->midiChannels;
		DiceDriver.TxParameters.IsocTransmitter[i].Speed = SPEED_400;
		strncpy(DiceDriver.TxParameters.IsocTransmitter[i].Names,pTxStreamConfig->names,DD_SIZE_OF_NAMES);
		for (plug = 0; plug < 8; plug++)
		{
			avsSetTxMidiMap(i, (uint8) plug, (uint8) plug);
		}
		if (pTxStreamConfig->AC3Capabilities)
		{			
			//AC3 capture is not supported by driver		
			sysLogError(E_BAD_INPUT_PARAMETERS, __LINE__, moduleName);
		}
		DiceDriver.TxParameters.IsocTransmitter[i].AC3Capabilities = 0;
		pTxStreamConfig++;
	}
	//Main Rx Parameters Intialization
	DiceDriver.RxParameters.NumIsocReceiver = nbRxStreams;
	//Individual Rx Initialization
	for(i=0;i<DiceDriver.RxParameters.NumIsocReceiver;i++)
	{
		DiceDriver.RxParameters.IsocReceiver[i].ChannelNumber = 0xffffffff;
		DiceDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber = 0;		
		DiceDriver.RxParameters.IsocReceiver[i].NumAudioSequences = pRxStreamConfig->AudioChannels;		
		DiceDriver.RxParameters.IsocReceiver[i].NumMidiSequences = pRxStreamConfig->midiChannels;		
		strncpy(DiceDriver.RxParameters.IsocReceiver[i].Names,pRxStreamConfig->names,DD_SIZE_OF_NAMES);	
		for (plug = 0; plug < 8; plug++)
		{
			avsSetRxMidiMap(i, (uint8) plug, (uint8) plug);
		}
		DiceDriver.RxParameters.IsocReceiver[i].AC3Capabilities = pRxStreamConfig->AC3Capabilities;
		pRxStreamConfig++;
	}

	DiceDriver.GlobalParameters.ClockCapabilities = ClockCapabilities;
	strncpy(DiceDriver.GlobalParameters.ClockSourceNames,ClockSourceNames,DD_SIZE_OF_CLK_SRC_NAME);

	strncpy(DiceDriver.GlobalParameters.NickName,nickName,DD_SIZE_OF_NICK_NAME);
	
	return hResult;
}


HRESULT diceDriverSetSortID( uint32 sortID )
{
	DiceDriver.GlobalParameters.SortID = sortID;
	return NO_ERROR;
}


HRESULT diceDriverSetHardwareLatencies( uint32 txHwLatency, uint32 rxHwLatency )
{
	DiceDriver.GlobalParameters.TxHwLatency = txHwLatency;
	DiceDriver.GlobalParameters.RxHwLatency = rxHwLatency;
	return NO_ERROR;
}


uint32 diceDriverGetCapabilites()
{
	return DiceDriver.GlobalParameters.Capabilities;
}


HRESULT diceDriverSetCapabilites( uint32 capabilities )
{
	DiceDriver.GlobalParameters.Capabilities = capabilities;
	return NO_ERROR;
}


HRESULT diceDriverConfigureStreaming (STREAM_CONFIG * pTxStreamConfig, uint8 nbTxStreams, 
									  STREAM_CONFIG * pRxStreamConfig, uint8 nbRxStreams)
{
	return diceDriverConfigureNew (pTxStreamConfig, nbTxStreams, 
                                   pRxStreamConfig, nbRxStreams,
                                   DiceDriver.GlobalParameters.NickName,							 
								   appCB,
								   DiceDriver.GlobalParameters.ClockCapabilities,
							       DiceDriver.GlobalParameters.ClockSourceNames);
}


HRESULT diceDriverConfigureInitial   (char * nickName,
									  DICEDRIVER_CALLBACK_FUNC cbFunc,
							          uint32 ClockCapabilities,
							          char* ClockSourceNames)
{
	HRESULT hResult = NO_ERROR;
	
	DiceDriver.GlobalParameters.ClockCapabilities = ClockCapabilities;
	strncpy(DiceDriver.GlobalParameters.ClockSourceNames,ClockSourceNames,DD_SIZE_OF_CLK_SRC_NAME);

	strncpy(DiceDriver.GlobalParameters.NickName,nickName,DD_SIZE_OF_NICK_NAME);
	appCB = cbFunc;
	
	return hResult;
}


void diceDriverNotifyHost (uint32 notification)
{
	uint16 nodeAddr;
	OFFSET_1394	destOffset;

	TCMutexAppLock(semID_notify);

	if (notification && isDriverAttached())
	{
		nodeAddr = 	DiceDriver.GlobalParameters.Owner.High >> 16;
		destOffset.High = DiceDriver.GlobalParameters.Owner.High & 0xffff;
		destOffset.Low = DiceDriver.GlobalParameters.Owner.Low;
		DiceDriver.GlobalParameters.Notification = notification;
		lalWriteNode(nodeAddr,destOffset,sizeof(notification),&DiceDriver.GlobalParameters.Notification);
	}
	TCMutexAppUnlock(semID_notify);
}

HRESULT diceDriverSetStatus (BOOL bLocked, DAL_NOMINAL_RATE_ENUM nominalRate, uint32 rateHz, uint32 extStatus)
{
	HRESULT hResult = NO_ERROR;
	uint32 notify = 0;
	uint32 previousNominalRate;

	if (nominalRate > eDAL_NOMINAL_RATE_192) nominalRate = DD_RATE_NONE;
	DiceDriver.GlobalParameters.SampleRate = rateHz;
	
	if (bLocked)
	{
		if (!(DiceDriver.GlobalParameters.Status & DD_STATUS_SRC_LOCKED))
		{
			//clock has been locked
			DiceDriver.GlobalParameters.Status = DD_STATUS_SRC_LOCKED;
			notify |= DD_NOTIFY_LOCK_CHG_BIT;
		}
	}
	else
	{
		if (DiceDriver.GlobalParameters.Status & DD_STATUS_SRC_LOCKED)
		{
			//clock has been unlocked
			DiceDriver.GlobalParameters.Status = 0;
			notify |= DD_NOTIFY_LOCK_CHG_BIT;
		}
	}
	previousNominalRate = ((DiceDriver.GlobalParameters.Status & DD_STATUS_NOM_RATE_MASK) >> DD_STATUS_NOM_RATE_SHIFT);
	if (previousNominalRate != nominalRate)
		notify |= DD_NOTIFY_LOCK_CHG_BIT;
	if((DiceDriver.GlobalParameters.ExtendedStatus & 0xFFFF) != (extStatus & 0xFFFF))
		notify |= DD_INTERFACE_CHG_BIT;
	DiceDriver.GlobalParameters.ExtendedStatus = extStatus;
	DiceDriver.GlobalParameters.Status &= (~DD_STATUS_NOM_RATE_MASK);
	DiceDriver.GlobalParameters.Status |= ((nominalRate << DD_STATUS_NOM_RATE_SHIFT) & DD_STATUS_NOM_RATE_MASK);
	diceDriverNotifyHost (notify);
	return hResult;
}



HRESULT diceDriverInitialize(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32 i,plug;

	//DiceDriver Initialization
	DiceDriver.GlobalParametersOffSet		= 	DICE_DRIVER_OFFSET(GlobalParameters)/4;
	DiceDriver.GlobalParametersSize			=	sizeof(DD_GLOBAL_PARAMETERS)/4;
	DiceDriver.TxParametersOffSet			=	DICE_DRIVER_OFFSET(TxParameters)/4;
	DiceDriver.TxParametersSize				=	sizeof(DD_TX_PARAMETERS)/4;
	DiceDriver.RxParametersOffSet			=	DICE_DRIVER_OFFSET(RxParameters)/4;
	DiceDriver.RxParametersSize				=	sizeof(DD_RX_PARAMETERS)/4;
	DiceDriver.Dummy1ParametersOffSet		=   DICE_DRIVER_OFFSET(ExtSyncInfo)/4;;
	DiceDriver.Dummy1ParametersSize			=	sizeof(DD_EXT_SYNC_INFO)/4;
	DiceDriver.Dummy2UpdateSpaceOffSet		=	0;
	DiceDriver.Dummy2UpdateSpaceSize		=	0;
	//Global Parameters Initialization
	DiceDriver.GlobalParameters.Owner.High 		= 	0xffff0000;
	DiceDriver.GlobalParameters.Owner.Low 		= 	0x00000000;
	DiceDriver.GlobalParameters.Notification	=	0x0;	
	strcpy(DiceDriver.GlobalParameters.NickName,"TCAT DICE");	
	DiceDriver.GlobalParameters.ClockSelect		= 	((DD_RATE_48K<<8) + DD_CLK_SRC_INTERNAL);
	DiceDriver.GlobalParameters.Enable			=	0;
	DiceDriver.GlobalParameters.Status			= 	0;	
	DiceDriver.GlobalParameters.ExtendedStatus	=	0;
	DiceDriver.GlobalParameters.SampleRate		=	0;
	//Main Tx Parameters Intialization
	DiceDriver.TxParameters.IsocTransmitterSize = sizeof(DD_ISOC_TRANSMITTER)/4;
	DiceDriver.TxParameters.NumIsocTransmitter 	= 1;
	//Individual Tx Initialization
	for(i = 0; i < DiceDriver.TxParameters.NumIsocTransmitter; i++)
	{
		DiceDriver.TxParameters.IsocTransmitter[i].ChannelNumber = 0xffffffff;
		DiceDriver.TxParameters.IsocTransmitter[i].NumAudioSequences = 2;
		DiceDriver.TxParameters.IsocTransmitter[i].NumMidiSequences = 0;
		DiceDriver.TxParameters.IsocTransmitter[i].Speed = SPEED_400;
		strcpy(DiceDriver.TxParameters.IsocTransmitter[i].Names,"Main L\\Main R\\\\");
		for (plug = 0; plug < 8; plug++)
		{
			avsSetTxMidiMap(i, (uint8) plug, (uint8) plug);
		}

	}
	//Main Rx Parameters Intialization
	DiceDriver.RxParameters.NumIsocReceiver = 1;
	DiceDriver.RxParameters.IsocReceiverSize = sizeof(DD_ISOC_RECEIVER)/4;
	//Individual Rx Initialization
	for(i=0;i<DiceDriver.RxParameters.NumIsocReceiver;i++)
	{
		DiceDriver.RxParameters.IsocReceiver[i].ChannelNumber = 0xffffffff;
		DiceDriver.RxParameters.IsocReceiver[i].FirstSequenceNumber = 0;
		DiceDriver.RxParameters.IsocReceiver[i].NumAudioSequences = 2;		
		DiceDriver.RxParameters.IsocReceiver[i].NumMidiSequences = 0;		
		strcpy(DiceDriver.RxParameters.IsocReceiver[i].Names,"Main L\\Main R\\\\");	
		for (plug = 0; plug < 8; plug++)
		{
			avsSetRxMidiMap(i, (uint8) plug, (uint8) plug);
		}
	}

	//DiceDriver Implemented Specification version number
	DiceDriver.GlobalParameters.Version = DICE_DRIVER_SPECIFICATION_VERSION_NUMBER;

	DiceDriver.GlobalParameters.Capabilities = 0;
#if !defined(AVS_USE_SYT32)
	DiceDriver.GlobalParameters.Capabilities |= kHighRatesUseDualWire;
#endif
#if !defined(_DICE2)
	DiceDriver.GlobalParameters.Capabilities |= kSupportEmptyPacketsWithPayload;
#endif
#if defined(_EAP)
	DiceDriver.GlobalParameters.Capabilities |= kSupportsEAP;
#endif
	DiceDriver.GlobalParameters.MaxFwSpeed = SPEED_400;
	DiceDriver.GlobalParameters.TxHwLatency = 0;
	DiceDriver.GlobalParameters.RxHwLatency = 0;
	DiceDriver.GlobalParameters.SortID = 0;

	//Create Semaphore
	hResult = TCSemaphoreOpen(&DiceDriverSemID, 0);
	if (hResult != NO_ERROR) return hResult;
	
	//Resume the task
	hResult = diceDriverResumeTasks ();
	if (hResult != NO_ERROR) return hResult;

	//register the address range
	hResult = DiceDriverRegisterAddressCallbacks();
	if (hResult != NO_ERROR) return hResult;

    //install bus reset indication function
    hResult = lalRegisterBRIndicationCallback(busResetIndicationCallback);
    if (hResult != NO_ERROR) return hResult;

	hResult = TCMutexAppOpen(&semID_notify);
	
#ifdef _CLI
	diceDriverCliInstallTools();
#endif //_CLI	

	return hResult;
}

DICE_DRIVER_STRUCT*	diceDriverGetState(void)
{
	return &DiceDriver;
}

/*********************************************************
	Resume tasks for this module
*/	
HRESULT diceDriverResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskResume(DiceDriverThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT diceDriverSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskSuspend(DiceDriverThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT diceDriverInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskCreate(&DiceDriverThreadID, DiceDriverThreadName, DiceDriverThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#endif //_DICE_DRIVER

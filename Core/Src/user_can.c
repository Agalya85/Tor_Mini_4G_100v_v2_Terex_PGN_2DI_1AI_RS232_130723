/*
 * PGN CAN Code
 *
 */

/*******************************************************************************
* Title                 :   CAN peripheral Interface
* Filename              :   user_can.c
* Author                :   Hrishikesh Limaye
* Origin Date           :   1/6/2019
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32L433 - TorMini
* Notes                 :   None
*
* Copyright (c) by KloudQ Technologies Limited.

  This software is copyrighted by and is the sole property of KloudQ
  Technologies Limited.
  All rights, title, ownership, or other interests in the software remain the
  property of  KloudQ Technologies Limited. This software may only be used in
  accordance with the corresponding license agreement. Any unauthorized use,
  duplication, transmission, distribution, or disclosure of this software is
  expressly forbidden.

  This Copyright notice may not be removed or modified without prior written
  consent of KloudQ Technologies Limited.

  KloudQ Technologies Limited reserves the right to modify this software
  without notice.
*
*
*******************************************************************************/
/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	  Description
*  01/06/20   1.0.0    Hrishikesh Limaye   Initial Release.
*
*******************************************************************************/

/** @file  user_can.c
 *  @brief Utilities for CAN interface
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "user_can.h"
#include "externs.h"
#include "applicationdefines.h"
#include "error_handler.h"
#include "string.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * Doxygen tag for documenting variables and constants
 */

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/
CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
//CAN_FilterTypeDef  sFilterConfig;

uint8_t               TxData[8];
uint8_t               RxData[8];

uint32_t              TxMailbox;
uint32_t u32CanIdsReceived[CAN_BUFFER_LENGTH] = {'0'};
uint32_t u32CanRxMsgLoopCounter = 0;
uint32_t canTestVariable = 0;
uint32_t u32IdSearchFlag = FALSE;
volatile uint32_t gu32ProcessCanMessage = 0;


uint64_t gu64CanMessageArray[MAX_CAN_IDS_SUPPORTED] = {0};
uint64_t gu64CanMessageIDPGN[MAX_CAN_IDS_SUPPORTED] = {0};
uint64_t u64CanMessageReceived[CAN_BUFFER_LENGTH] = {0};

strCanReceivedMsg unCanReceivedMsgs[CAN_BUFFER_LENGTH];
unCan1939CommandId unCan1939ReceivedId[CAN_BUFFER_LENGTH]={0};

/* Configuration Array for Can Peripheral
 * Prerequisite : [0] - Will / Should always contain ONLY CAN BaudRate / Bit Rate !
 * CommandIds   : [1 to 50] Sequential list of CAN Command Ids to be captured .
 * Defaults / Example : { 500, 1, 2, 3, 0x3AD,0x1FF,6,7,0x7FF,9  ,0x3AB};
 *                        BR ,Id,Id,Id, Id  , Id  ,Id , Id  ,Id , Id
 * */
uint32_t gu32CanConfigurationArray[(MAX_CAN_IDS_SUPPORTED + 1)] = { 250,0xCF00400,0xCF00300,0x18FEF200,0x18FEDF00,0x18FEF500,
																	0x18FEE400,0x18FEE500,0x18FEE900,0x18FEEE00,0x18FEEF00,
																	0x18FEF600,0x18FEF700,0x18FEFF00,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
															 	   };

/* Configuration Array for Can Peripheral - PGN Filter
 * Prerequisite : [0] - Will / Should always contain ONLY CAN BaudRate / Bit Rate !
 * CommandIds   : [1 to 50] Sequential list of CAN Command Ids to be captured .
 * Defaults / Example : { 500, 1, 2, 3, 0x3AD,0x1FF,6,7,0x7FF,9  ,0x3AB};
 *                        BR ,Id,Id,Id, Id  , Id  ,Id , Id  ,Id , Id
 * */
/*
 * GNB Forklift -
65263
65262
61444
65253
65300
65276
65254
65254
65254
65254
65254
65254
65241
65241*/
uint32_t gu32CanConfigurationArrayPGN[(MAX_CAN_IDS_SUPPORTED + 1)] = { 250,
																	61444,61443,65266,65247,65269,
																	65252,65253,65257,65262,65263,
																	65270,65271,65279,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
																	2,2,2,2,2,
															 	   };





/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/


/****************************************************************************
 Function: canFilterConfig
 Purpose: Init CAN peripheral with filter configuration
 Input: None.

 Return value: None

 Refer Link for timing calculations :
 http://www.bittiming.can-wiki.info/

 Clock = 80 Mhz (Refer Clock Configuration in CubeMX for details)

 Bit Rate    Pre-scaler  time quanta  Seg 1  Seg 2   Sample Point
 kbps

 1000			5			16			13	   2	    87.5
 500			10			16			13     2		87.5
 250			20			16			13     2    	87.5
 125			40			16			13     2		87.5
 100			50			16			13     2		87.5
 83.33			60			16			13     2		87.5
 50				100			16			13     2		87.5
 20				250			16			13     2		87.5
 10				500			16			13     2		87.5


 Note(s)(if-any) :

 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/03/2020			initial Definitions
 kloudq				27/03/2020			Bit Calculation Added
 kloudq				20/04/2021			Added support for STM32L433 MCU
******************************************************************************/
void canFilterConfig(void)
{
	hcan1.Instance = CAN1;
	hcan1.Init.Prescaler = 10;
	hcan1.Init.TimeTriggeredMode = DISABLE;
	hcan1.Init.AutoBusOff = ENABLE;
	hcan1.Init.AutoWakeUp = ENABLE;//DISABLE;
	hcan1.Init.AutoRetransmission = ENABLE;//DISABLE;
	hcan1.Init.ReceiveFifoLocked = DISABLE;//ENABLE;//
	hcan1.Init.TransmitFifoPriority = ENABLE;//DISABLE;

	hcan1.Init.Mode = CAN_MODE_NORMAL;
//	hcan1.Init.Mode = CAN_MODE_SILENT;

	hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
	hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;

	switch(gu32CanConfigurationArray[0])
  	{
  		case 1000:
  			hcan1.Init.Prescaler = 5;
  		break;

  		case 500:
  			hcan1.Init.Prescaler = 10;
  		break;

  		case 250:
  			hcan1.Init.Prescaler = 20;
  		break;

  		case 125:
  			hcan1.Init.Prescaler = 40;
  		break;

  		case 100:
  			hcan1.Init.Prescaler = 50;
  		break;

  		case 83:
  			hcan1.Init.Prescaler = 60;
  		break;

  		case 50:
  			hcan1.Init.Prescaler = 100;
  		break;

  		case 20:
  			hcan1.Init.Prescaler = 250;
  		break;

  		case 10:
  			hcan1.Init.Prescaler = 500;
  		break;

  		default:
  			/* Illegal BaudRate Configured . Use Default 500 Kbps */
  			hcan1.Init.Prescaler = 10;
  		break;
  	}

  	if (HAL_CAN_Init(&hcan1) != HAL_OK)
  		assertError(enmTORERRORS_CAN1_INIT,enmERRORSTATE_ACTIVE);
  	else
  		 assertError(enmTORERRORS_CAN1_INIT,enmERRORSTATE_NOERROR);

  	/*##-2- Configure the CAN Filter ###########################################*/
  	  sFilterConfig.FilterBank = 0;
  	  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  	  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  	  sFilterConfig.FilterIdHigh = 0x0000;
  	  sFilterConfig.FilterIdLow = 0x0000;
  	  sFilterConfig.FilterMaskIdHigh = 0x0000;
  	  sFilterConfig.FilterMaskIdLow = 0x0000;
  	  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  	  sFilterConfig.FilterActivation = ENABLE;
  	  sFilterConfig.SlaveStartFilterBank = 14;

  	if (HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  		assertError(enmTORERRORS_CAN1_CONFIGFILTER,enmERRORSTATE_ACTIVE);
  	else
  		assertError(enmTORERRORS_CAN1_CONFIGFILTER,enmERRORSTATE_NOERROR);

  	/*##-3- Start the CAN peripheral ###########################################*/
  	  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  	  {
  	    /* Start Error */
  	    Error_Handler();
  	  }

  	/*##-4- Activate CAN RX notification #######################################*/
  	  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  	  {
  		/* Notification Error */
  		  assertError(enmTORERRORS_CAN1_CONFIGFILTER,enmERRORSTATE_ACTIVE);
  	  }

  	  /*##-5- Configure Transmission process #####################################*/
  	TxHeader.StdId = 0x321;
  	TxHeader.ExtId = 0x01;
  	TxHeader.RTR = CAN_RTR_DATA;
  	TxHeader.IDE = CAN_ID_STD;
  	TxHeader.DLC = 2;
  	TxHeader.TransmitGlobalTime = DISABLE;

  	memset(u32CanIdsReceived,0x00,sizeof(u32CanIdsReceived));
  	memset(unCanReceivedMsgs,0x00,sizeof(unCanReceivedMsgs));
}

/******************************************************************************
* Function : HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
*//**
* \b Description:
*
* This function is Rx complete callback in non blocking mode
*
* PRE-CONDITION: Enable CAN interface in CubeMx . Enable CAN Rx Interrupt
*
* POST-CONDITION: Buffers received data
*
* @return 		None.
*
* \b Example Example:
* @code
*
*
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	/* LED Only for testing/ Indication . Can be removed in production if not required  */
//	HAL_GPIO_TogglePin(COMM_LED_uC_GPIO_Port,COMM_LED_uC_Pin);
	HAL_GPIO_TogglePin(Comm_LED_GPIO_Port,Comm_LED_Pin);
	/* Get RX message */
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
	{
		/* Reception Error */
		Error_Handler();
	}
	/* Parse the incoming data only if array location is available
	 * Added on 3/3/21 - For payload overwrite issue */
	if(unCan1939ReceivedId[u32CanRxMsgLoopCounter].u32J1939CommandId == 0)
	{
		if(RxHeader.IDE == CAN_ID_EXT)
		{
			//u32CanIdsReceived[u32CanRxMsgLoopCounter] = RxHeader.ExtId;
			unCan1939ReceivedId[u32CanRxMsgLoopCounter].u32J1939CommandId = RxHeader.ExtId;
		}
		else if(RxHeader.IDE == CAN_ID_STD)
		{
			//u32CanIdsReceived[u32CanRxMsgLoopCounter] = RxHeader.StdId;
			unCan1939ReceivedId[u32CanRxMsgLoopCounter].u32J1939CommandId = RxHeader.StdId;
		}
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte0 = (RxData[0]);
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte1 = (RxData[1]);
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte2 = (RxData[2]);
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte3 = (RxData[3]);
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte4 = (RxData[4]);
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte5 = (RxData[5]);
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte6 = (RxData[6]);
//		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte7 = (RxData[7]);
//		if(unCan1939ReceivedId[u32CanRxMsgLoopCounter].u32J1939CommandId == 0xF00400)
//		{
//			unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte0 = (RxData[7]);
//		}
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte0 = (RxData[7]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte1 = (RxData[6]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte2 = (RxData[5]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte3 = (RxData[4]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte4 = (RxData[3]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte5 = (RxData[2]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte6 = (RxData[1]);
		unCanReceivedMsgs[u32CanRxMsgLoopCounter].u8CanMsgByte7 = (RxData[0]);

		u32CanRxMsgLoopCounter++;

	}

	if(u32CanRxMsgLoopCounter >= CAN_BUFFER_LENGTH)
		u32CanRxMsgLoopCounter = 0;

}

/******************************************************************************
* Function : HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
*//**
* \b Description:
*
* This function is CAN Error callback
*
* PRE-CONDITION: Enable CAN interface in CubeMx . Enable CAN Interrupt
*
* POST-CONDITION: Gives Error Code
*
* @return 		None.
*
* \b Example Example:
* @code
*
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
	canTestVariable = hcan->ErrorCode;
	HAL_CAN_DeInit(&hcan1);
	canFilterConfig();
}

/******************************************************************************
* Function : isCommandIdConfigured(uint32_t canId)
*//**
* \b Description:
*
* This function Checks if received Id is configured for CAN
*
* PRE-CONDITION: Enable CAN interface in CubeMx . Enable CAN Interrupt
*
* POST-CONDITION: Gives Error Code
*
* @return 		uint32_t Id Position in PGN configuration Array.
*
* \b Example Example:
* @code
*	uint32_t idIndex = 0;
	do
	{
		idIndex = isCommandIdConfigured(0x0803FF00);
	}while(u32IdSearchFlag != 2);

	*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t isCommandIdConfigured(uint32_t canId)
{
	static uint32_t LoopCounter = 0;
	static uint32_t u32PositioninConfigArray = 0;

	if(LoopCounter == 0)
	{
		u32IdSearchFlag = 1;
		u32PositioninConfigArray = 0;
	}

	if(u32IdSearchFlag == 1)
	{
		if(gu32CanConfigurationArrayPGN[LoopCounter] == canId)
		{
			/*
			 * If Received CanID is found in configuration Array
			 * then parse the frame else ignore .
			 */
			u32PositioninConfigArray = LoopCounter;
			u32IdSearchFlag = 2;
			LoopCounter = 0;
		}
		else
		{
			LoopCounter++;
			if (LoopCounter == MAX_CAN_IDS_SUPPORTED)
			{
				LoopCounter = 0;
				u32IdSearchFlag = 2;
			}
		}
	}
	return u32PositioninConfigArray;
}

/******************************************************************************
* Function : parseCanMessageQueue(uint32_t canId)
*//**
* \b Description:
*
* This function is used Parse CAN Message . If command ID is configured the store the message
*
* PRE-CONDITION: Enable CAN interface in CubeMx . Enable CAN Interrupt
*
* POST-CONDITION: Stored messsage of configured ID
*
* @return 		None.
*
* \b Example Example:
* @code
*
	parseCanMessageQueue();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t temp = 0;
uint32_t gu32CanIdParserCounter = 0;
void parseCanMessageQueue(void)
 {
	static uint32_t u32CanMsgID = 0;
	static uint32_t u32ParserState = 0;
	static uint32_t u32IdStatus = 0;

	if(unCan1939ReceivedId[gu32CanIdParserCounter].u32J1939CommandId != 0)
	{
		if(u32ParserState == 0)
		{
			/* Message Available. Parse The Message */
			u32CanMsgID = unCan1939ReceivedId[gu32CanIdParserCounter].u16J1939PGN;
			u32ParserState = 1;
		}
		else if(u32ParserState == 1)
		{
			/* In Process */
			if(u32IdSearchFlag == 2)
			{
				/* Search Process Completed */
				if(u32IdStatus != 0)
				{

					gu64CanMessageArray[u32IdStatus] =  ((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte7 << 56)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte6 << 48)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte5 << 40)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte4 << 32)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte3 << 24)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte2 << 16)|
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte1 << 8) |
														((uint64_t)unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte0);

					gu64CanMessageIDPGN[u32IdStatus] = unCan1939ReceivedId[gu32CanIdParserCounter].u32J1939CommandId;

					// Reset Array Value for new Message
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte7 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte6 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte5 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte4 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte3 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte2 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte1 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte0 = 0;
					u32CanIdsReceived[gu32CanIdParserCounter] = 0;
					unCan1939ReceivedId[gu32CanIdParserCounter].u32J1939CommandId = 0;
					u32IdStatus = 0;
				}
				else if(u32IdStatus == 0)
				{
					/* Command Id is not Configured . Discard the Message*/
//					gu64CanMessageArray[gu32CanIdParserCounter] = 0; // change by VEDANT on 21/11/22
					//u64CanMessageReceived[gu32CanIdParserCounter] = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte7 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte6 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte5 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte4 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte3 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte2 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte1 = 0;
					unCanReceivedMsgs[gu32CanIdParserCounter].u8CanMsgByte0 = 0;
					u32CanIdsReceived[gu32CanIdParserCounter] = 0;
					unCan1939ReceivedId[gu32CanIdParserCounter].u32J1939CommandId = 0;
				}
				u32IdSearchFlag = 0;
				u32ParserState = 0;
				u32CanMsgID = 0;
				gu32CanIdParserCounter++;
			}
			else
				u32IdStatus = isCommandIdConfigured(u32CanMsgID);
		}
	}
	else
	{
		gu32CanIdParserCounter++;
	}
	if(gu32CanIdParserCounter >= CAN_BUFFER_LENGTH)
		gu32CanIdParserCounter = 0;
}

/*** End of Functions **************************************************************/

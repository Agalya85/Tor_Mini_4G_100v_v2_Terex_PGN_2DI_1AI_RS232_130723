///*
// * user_can.h
// *
// *  Created on: Mar 25, 2022
// *      Author: admin
// */
//
//#ifndef INC_USER_CAN_H_
//#define INC_USER_CAN_H_
//
//#define MAX_CAN_IDS_SUPPORTED (50)
////#define CAN_BUFFER_LENGTH     (80)
//#define CAN_BUFFER_LENGTH     (200)
//#define KWP_DATA_BYTE_BUF_LEN (45) // tot 45 but counter start with 0 so 45-1 = 44
//
//typedef enum
//{
//	enmCANQUERY_IDLE = 0,
//	enmCANQUERY_UPDATEQUERY,
//	enmCANQUERY_SENDQUERY,
//	enmCANQUERY_AWAITRESPONSE,
//	enmCANQUERY_PASRERESPONSE,
//	enmCANQUERY_RESPONSETIMEOUT
//}enmCanQueryState;
//
//typedef enum
//{
//	enumKWPDATA_IDLE = 0,
//	enumKWPDATA_LIVE_DATA = 1,
//	enumKWPDATA_CHARGE_SUMMARY_DATA = 2,
//	enumKWPDATA_DRIVE_SUMMARY_DATA = 3,
//}enumKWPData;
//
//typedef struct
//{
//	uint8_t u8CanMsgByte7;
//	uint8_t u8CanMsgByte6;
//	uint8_t u8CanMsgByte5;
//	uint8_t u8CanMsgByte4;
//	uint8_t u8CanMsgByte3;
//	uint8_t u8CanMsgByte2;
//	uint8_t u8CanMsgByte1;
//	uint8_t u8CanMsgByte0;
//}strCanReceivedMsg;
//
//void canFilterConfig(void);
//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
//uint32_t isCommandIdConfigured(uint32_t canId);
//void parseCanMessageQueue(void);
//void parseKWPCanMessageQueue(void);
//void executeCANQueries(void);
//void updateCANQuery(void);
//void sendMessageCAN (void);
//void VerifyKWDRequest(void);
//
//#endif /* INC_USER_CAN_H_ */
//


/*
 * PGN Code
 */

/*******************************************************************************
* Title                 :   CAN peripheral Interface Header
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

/** @file  user_can.h
 *  @brief Utilities for CAN interface
 */

#ifndef INC_USER_CAN_H_
#define INC_USER_CAN_H_

/******************************************************************************
* Includes
*******************************************************************************/
/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * \brief Defines Max CAN Ids supported \a .
   \param None.
   \returns None \a .
 */
#define MAX_CAN_IDS_SUPPORTED (50)
/**
 * \brief CAN buffer array length \a .
   \param None.
   \returns None \a .
 */
#define CAN_BUFFER_LENGTH     (200)

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/
/**
 * This union stores Command IDs and its parsed info.
 * This is used to filter PGNs from incomming IDs
 */
typedef union{
	struct{
		uint8_t u16J1939SA : 8;		// bit 0 to 8
		uint32_t u16J1939PGN : 16;  // bit 9 to 24
		uint8_t u16J1939DataPage:1; // bit 25
		uint8_t u16J1939Reserved:1; // bit 26
		uint8_t u16J1939Priority:3; // bit 27,28,29
	};
	uint32_t u32J1939CommandId;
}unCan1939CommandId;

/**
 * This structure stores Command Msgs bytes.
 *
 */
typedef struct
{
	uint8_t u8CanMsgByte7;
	uint8_t u8CanMsgByte6;
	uint8_t u8CanMsgByte5;
	uint8_t u8CanMsgByte4;
	uint8_t u8CanMsgByte3;
	uint8_t u8CanMsgByte2;
	uint8_t u8CanMsgByte1;
	uint8_t u8CanMsgByte0;
}strCanReceivedMsg;

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

/******************************************************************************
* Function Prototypes
*******************************************************************************/
#ifdef __cplusplus
extern "C"{
#endif

void canFilterConfig(void);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
uint32_t isCommandIdConfigured(uint32_t canId);
void parseCanMessageQueue(void);

#ifdef __cplusplus
} // extern "C"
#endif
#endif /* INC_USER_CAN_H_ */

/*** End of File **************************************************************/

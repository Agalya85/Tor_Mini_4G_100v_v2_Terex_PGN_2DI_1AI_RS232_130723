/*
 * serial_comm.c
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

/*
  *****************************************************************************
  * @file    serial_communication.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   UART related functions
*******************************************************************************
*/
/******************************************************************************

            Copyright (c) by KloudQ Technologies Limited.

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

  KloudQ Technologies Limited

*******************************************************************************
*/
#include "main.h"
#include "stm32l4xx_hal.h"
#include "applicationdefines.h"
#include <string.h>
#include "externs.h"
#include "timer.h"
#include "serial_comm.h"

volatile uint32_t u8GSMCharRcv = 0;
uint8_t u8gsmResponse = 0;
volatile uint32_t u8GPSCharRcv = 0;
uint8_t u8gpsResponse = 0;
/******************************************************************************
 Function Name: GSM_CharReception_Callback
 Purpose: Byte Received Interrupt (GSM)
 Input:	None
 Return value: None.

	Note(s)(if any)
	-> Called from ISR
	-> USART4

 Change History:
 Author           	Date                Remarks
 KloudQ Team       11-04-18
******************************************************************************/

void GSM_CharReception_Callback(void)
{
	u8gsmResponse =LL_USART_ReceiveData8(USART1);
	gsmInstance.as8GSM_Response_Buff[gsmInstance.u8GSM_Response_Character_Counter++] = u8gsmResponse;
	gu32GSMCharacterTimeout = FIVEHUNDRED_MS;

	if(u8GSMCharRcv == 0)
		u8GSMCharRcv = 1;
}

/******************************************************************************
 Function Name: GSP_CharReception_Callback
 Purpose: Byte Received Interrupt (GSM)
 Input:	None
 Return value: None.

	Note(s)(if any)
	-> Called from ISR
	-> USART4

 Change History:
 Author           	Date                Remarks
 KloudQ Team       11-04-18
******************************************************************************/

void GPS_CharReception_Callback(void)
{
//	u8gpsResponse = LL_USART_ReceiveData8(USART2);
//	gsmInstance.as8GPS_Response_Buff[gsmInstance.u8GPS_Response_Character_Counter++] = u8gpsResponse;
//	if(strlen(gsmInstance.as8GPS_Response_Buff) >= GPS_RESPONSE_ARRAY_SIZE )
//	{
//		memset(gsmInstance.as8GPS_Response_Buff,0x00,sizeof(gsmInstance.as8GPS_Response_Buff));
//		gsmInstance.u8GPS_Response_Character_Counter = 0;
//	}
////	gu32GSMCharacterTimeout = FIVEHUNDRED_MS;
//
//	if(u8GPSCharRcv == 0)
//		u8GPSCharRcv = 1;
//	if(strlen(gsmInstance.as8GPS_Response_Buff) <= GPS_RESPONSE_ARRAY_SIZE )
//	{
//		u8gpsResponse = LL_USART_ReceiveData8(USART2);
//		gsmInstance.as8GPS_Response_Buff[gsmInstance.u8GPS_Response_Character_Counter++] = u8gpsResponse;
//	}
//	else
//	{
//
//		LL_USART_DisableIT_RXNE(USART2);
//		if(u8GPSCharRcv == 0)
//			u8GPSCharRcv = 1;
//	}



	//	gu32GSMCharacterTimeout = FIVEHUNDRED_MS;

//		if(u8GPSCharRcv == 0)
//			u8GPSCharRcv = 1;
}

//******************************* End of File *******************************************************************

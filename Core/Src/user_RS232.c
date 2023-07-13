/*
 * user_RS232.c
 *
 *  Created on: Jul 11, 2023
 *      Author: Admin
 */
#include "main.h"
#include "stm32l4xx_hal.h"
//#include "applicationdefines.h"
#include <string.h>
#include "externs.h"
#include "timer.h"
#include "user_RS232.h"
#include "string.h"

volatile uint32_t u8RS232_Counter;
volatile uint32_t u8RS232_Response_Character_Counter;
volatile uint32_t u8RS232CharRcv = 0;
uint8_t u8RS232Response = 0;
uint16_t RS232len = 0;
//#define RS232_RESPONSE_ARRAY_SIZE     (1000)
uint8_t RS232Buffer[1000] = "";
char buff[RS232_RESPONSE_ARRAY_SIZE] = "";
void RS232_Callback(void)
{
//	memset(RS232Buffer,0x00,sizeof(RS232Buffer));
	u8RS232Response = LL_USART_ReceiveData8(USART2);
//	RS232len = strlen(u8RS232Response);
	RS232Buffer[u8RS232_Response_Character_Counter++]=u8RS232Response;
//	gu32RS232CharacterTimeout = FIVEHUNDRED_MS;
	gu32RS232CharacterTimeout = HUNDRED_MS;

	if(!u8RS232CharRcv)
	{

		u8RS232CharRcv = 1;
//		if(u8RS232_Response_Character_Counter==100)
//			u8RS232_Response_Character_Counter=0;
	}// End of if(!u8RS232CharRcv)
}

void RS232(void)
{

	  if((gu32RS232CharacterTimeout == 0) && (u8RS232CharRcv == 1))
	  {
		  memset(buff,0x00,sizeof(buff));
		  memcpy(buff,RS232Buffer, strlen((char*)RS232Buffer));
		  memset(RS232Buffer,0x00,sizeof(RS232Buffer));
		  u8RS232_Response_Character_Counter = 0;
		  u8RS232CharRcv = 0;

//		  for(u8RS232_Counter=0;u8RS232_Counter<strlen(RS232Buffer);u8RS232_Counter++)
//		  {
//			  if(!LL_USART_IsActiveFlag_TXE(USART2))
//			  {
//				  /*Do nothing . Wait For Previous Character Transmission */
//			  }
//			  else
//			  {
//				  LL_USART_TransmitData8(USART2, RS232Buffer[u8RS232_Counter++]);
//			  }
//			  u8RS232CharRcv = 0;
//		  }
//		  if(u8RS232_Counter==strlen(RS232Buffer))
//		  {
//			  u8RS232_Response_Character_Counter=0;
//		  }
	  }
}

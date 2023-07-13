/*
 * serial_flash_fsm.c
 *
 *  Created on: 01-Nov-2021
 *      Author: Admin
 */

#include "serial_flash_w25_fsm.h"
#include "main.h"
#include "stm32l4xx_hal.h"
#include "applicationDefines.h"
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include "externs.h"
#include "timer.h"
#include "spi.h"



#define BUFF_MEM_DEBUG		0

uint8_t u8DebugDataRead=0;

uint32_t g_u32SpiRxCmpltFlag = 0;
uint32_t g_u32SpiTxCmpltFlag = 0;

/* Variable declarations */
extern SPI_HandleTypeDef hspi2;

StructSFlash FlashStr;
uint32_t gu32BufferCounter = 1;

uint32_t SFLASH_SIZE = 0;
const uint32_t g_u32FlashPayloadMaxSize = 512;

/****************************************************************************
 Function SerialFlashFSM()
 Purpose: SerialFlashFSM
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SerialFlashFSM(void)
{
	static uint16_t u16writeDataSize = 0;
	uint16_t u16tempVar = 0;
	uint8_t u8Flash_Busy_status = 0;
	uint8_t DataToWrite[5];
	char model_number[50];

	if(FlashStr.u32SerialFlashDelay != 0)
		return;

	switch(FlashStr.enmCurrentOperation)
	{
		case enmFLASH_MEMORY_IDEAL:
		{
			FlashStr.u32SerialFlashDelay = FIVEHUNDRED_MS;

			if((Queue_IsBuffQueueEmpty(&gsmBuffPayload) == 1) && (g_stAdcData.u32PwrSupplyVtg > 9)&&(g_u8HttpConEstablishedFlag != 0))
			{
				if((FlashStr.u32FlashReadAddress < FlashStr.u32FlashWriteAddress))// && (FlashStr.u32FlashWriteAddress != 0)
				{
					if((FlashStr.u32FlashReadAddress + g_u32FlashPayloadMaxSize) < SFLASH_SIZE)
					{
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_READ;
						FlashStr.enmCurrentReadState = enmFlashReadStatusReg;
					}
					else
					{
						SFlash_Read(FlashStr.u32FlashReadAddress, (char *)FlashStr.cSFlashTransmitData, (SFLASH_SIZE - FlashStr.u32FlashReadAddress));
						FlashStr.u32FlashReadAddress = SFLASH_SIZE;
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_READ;
						FlashStr.enmCurrentReadState = enmFlashReadArrangeData;
					}
				}
				else if(GSMQueueFullFlag == 1)
				{
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_WRITE;
					FlashStr.enmCurrentWriteState = enmFlashWrCopyData;
					GSMQueueFullFlag = 0;

				}
				else
				{
					/* Reading is complete */
					if(FlashStr.u32FlashReadAddress != 0 || FlashStr.u32FlashWriteAddress != 0)
					{
//						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_ERASE;
					}
				}
			}
			/* Memory Write states */
			else if(GSMQueueFullFlag == 1)
			{
				FlashStr.enmCurrentOperation = enmFLASH_MEMORY_WRITE;
				FlashStr.enmCurrentWriteState = enmFlashWrCopyData;
				GSMQueueFullFlag = 0;

			}
			else if(((FlashStr.u32FlashReadAddress != 0) && (FlashStr.u32FlashWriteAddress != 0)) && ((FlashStr.u32FlashReadAddress >= FlashStr.u32FlashWriteAddress) || (FlashStr.u32FlashReadAddress >= SFLASH_SIZE) || (FlashStr.u32FlashWriteAddress >= SFLASH_SIZE)))
			{
				FlashStr.enmCurrentOperation = enmFLASH_MEMORY_ERASE;
			}
		}
		break;
		case enmFLASH_MEMORY_WRITE:
		{
			switch(FlashStr.enmCurrentWriteState)
			{
				case enmFlashWrCopyData:
				{
					if((Queue_IsLiveQueueEmpty(&gsmLivePayload) == FALSE )&& (gsmLivePayload.data[gsmLivePayload.tail] != NULL))
					{
						if((FlashStr.u32FlashWriteAddress + GSM_PAYLOAD_MAX_SIZE) < SFLASH_SIZE)
						{
							memset((char *)FlashStr.cSFlashWriteData, 0x00, (sizeof(FlashStr.cSFlashWriteData) * sizeof(char)));
							strcpy((char *)FlashStr.cSFlashWriteData, (char *)gsmLivePayload.data[gsmLivePayload.tail]);
#if(DEBUGENABLE == TRUE)
		strcat(DebugBuff,"Write to Flash : ");
		strcat(DebugBuff,(char * )FlashStr.cSFlashWriteData);
		strcat(DebugBuff,"\r\n");
#endif						
							Queue_LiveDequeue(&gsmLivePayload);

							u16writeDataSize = strlen((const char *)FlashStr.cSFlashWriteData);
							memset(model_number, 0, sizeof(model_number));
							strcpy(model_number, MODEL_NUMBER);

							if(strstr((char *)FlashStr.cSFlashWriteData, model_number) != NULL)
							{

//								char *ptr=NULL;
//								ptr = strstr(FlashStr.cSFlashWriteData,",DATA,");
								if(FlashStr.cSFlashWriteData[u16writeDataSize - 3] != '9')
								{
//									ptr[1]='S';
//									ptr[2]='T';
//									ptr[3]='D';
//									ptr[4]='T';
									// 9 is added to identify history pay load
									FlashStr.cSFlashWriteData[u16writeDataSize - 3] = '9';
									u16writeDataSize = strlen((const char *)FlashStr.cSFlashWriteData);
									FlashStr.u16FlashWriteIndex = 0;
									FlashStr.enmCurrentWriteState = enmFlashWriteStatusReg;
//								//	sprintf(u8TempDebugBuff,"*********** DATA Store in memory at the 0x%08X address \r\n",(int)FlashStr.u32FlashWriteAddress);
//									memset(u8TempDebugBuff,0x00,sizeof(u8TempDebugBuff));
//									SerialDebug_Print("*********** DATA Store in memory at the \0");
//									itoa(FlashStr.u32FlashWriteAddress ,u8TempDebugBuff,16);
//									SerialDebug_Print(u8TempDebugBuff);
//									SerialDebug_Print(" address \r\n\0");

//									if(sizeof(u8TempDebugBuff) > (strlen(FlashStr.cSFlashWriteData)+50))
//									{
//										strcpy(u8TempDebugBuff,"DATA Start-\r\n");
//										strcat(u8TempDebugBuff,FlashStr.cSFlashWriteData);
//										strcat(u8TempDebugBuff,"DATA End:-\r\n\0");
//										SerialDebug_Print(&u8TempDebugBuff[0]);
//									}
#if(DEBUGENABLE == TRUE)
		strcat(DebugBuff,"buff sys as 9 \r\n");
#endif						
								}
								else
								{
									FlashStr.enmCurrentOperation = enmFLASH_MEMORY_WRITE;
									FlashStr.enmCurrentWriteState = enmFlashWrCopyData;
								}
							}
							else
							{
								FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
							}
						}
						else
						{
							/* Memory Full */
							FlashStr.bitFlagMemoryFull = 1;
							FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
						}
					}
					else
					{
						FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					}
				}break;
				case  enmFlashWriteStatusReg:
				{
					u8Flash_Busy_status = SFlash_Read_Status_Register();
					if(u8Flash_Busy_status == 1)
					{
						/* Flash is busy */
						FlashStr.u32SerialFlashDelay = FIVEHUNDRED_MS;
						FlashStr.enmCurrentWriteState = enmFlashWriteStatusReg;
					}
					else
					{
						FlashStr.enmCurrentWriteState = enmFlashWriteEnable;
					}

				}break;
				case enmFlashWriteEnable:
				{
					SFlash_Write_Enable();
					FlashStr.enmCurrentWriteState = enmFlashWriteEnableCheck;
					FlashStr.u32SerialFlashDelay = TWENTY_MS;
				}
				break;
				case enmFlashWriteEnableCheck:
				{
					u8Flash_Busy_status = SFlash_Read_Status_Register();
					if(u8Flash_Busy_status == 2)
						FlashStr.enmCurrentWriteState = enmFlashWriteData;
					else
						FlashStr.enmCurrentWriteState = enmFlashWriteEnable;
					FlashStr.u32SerialFlashDelay = TWENTY_MS;
				}
				break;

				case enmFlashWriteData:
				{
					u16tempVar = FlashStr.u32FlashWriteAddress / 256;
					u16tempVar *= 256;
					u16tempVar = FlashStr.u32FlashWriteAddress - u16tempVar;
					FlashStr.u16FlashPageEmptyByte = 256 - u16tempVar;
					if(u16writeDataSize > FlashStr.u16FlashPageEmptyByte)
					{
						// Multiple write cycles
						FlashStr.u16FlashByteWriten = FlashStr.u16FlashPageEmptyByte;
					}
					else
					{
						// Last write cycle
						FlashStr.u16FlashByteWriten = u16writeDataSize;
						//FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					}

					memset(DataToWrite, 0, sizeof(DataToWrite));
					DataToWrite[0] = SFLASH_WRITE_PAGE_CODE;
					DataToWrite[1] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 16);
					DataToWrite[2] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 8);
					DataToWrite[3] = (uint8_t)(FlashStr.u32FlashWriteAddress >> 0);
					SFlash_Chip_Select(ENABLE);
					HAL_Delay(1);
				  	HAL_SPI_Transmit_IT(&hspi2, (uint8_t *) DataToWrite, 4);
				  	HAL_SPI_Transmit_IT(&hspi2, (uint8_t *) &FlashStr.cSFlashWriteData[FlashStr.u16FlashWriteIndex], FlashStr.u16FlashByteWriten);
				  	HAL_Delay(100);
					SFlash_Chip_Select(DISABLE);

					FlashStr.u32FlashWriteAddress += FlashStr.u16FlashByteWriten;
					FlashStr.u16FlashWriteIndex += FlashStr.u16FlashByteWriten;
					FlashStr.u16FlashPageEmptyByte -= FlashStr.u16FlashByteWriten;
					u16writeDataSize -= FlashStr.u16FlashByteWriten;
					if(FlashStr.u16FlashPageEmptyByte >= u16writeDataSize)	// Write is over write next queue
					{
						/**/
						HAL_Delay(5);
						SFlash_Write_Disable();
						FlashStr.enmCurrentWriteState = enmFlashWrCopyData ;
						memset((char *)FlashStr.cSFlashWriteData, 0x00, (sizeof(FlashStr.cSFlashWriteData) * sizeof(char)));

						if(Queue_IsLiveQueueEmpty(&gsmLivePayload) == TRUE)
						{
							//TODO check this is last cycle
							/*	Writing SFlash Write address into Inernal Flash*/
							g_stEepromBuffMemAdd.u32BuffFlashWriteAddress = FlashStr.u32FlashWriteAddress;
							g_stEepromBuffMemAdd.u32BuffFlashReadAddress =FlashStr.u32FlashReadAddress;
							EEPROM_WriteBuffMemData();
						}
					}
					else
					{
						FlashStr.enmCurrentWriteState = enmFlashWriteEnable;
					}
				}
				break;
				default:
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					break;
			}
		}
		break;

		case enmFLASH_MEMORY_READ:
		{
			switch(FlashStr.enmCurrentReadState)
			{
				case enmFlashReadStatusReg:
				{
					u8Flash_Busy_status = SFlash_Read_Status_Register();
					if(u8Flash_Busy_status == 1)
					{
						/* Flash is busy */
						FlashStr.u32SerialFlashDelay = FIVEHUNDRED_MS;
						FlashStr.enmCurrentReadState = enmFlashReadStatusReg;
					}
					else
					{
						FlashStr.enmCurrentReadState = enmFlashReadData;
					}
				}
				break;

				case enmFlashReadData:
				{
#if(BUFF_MEM_DEBUG != 0)
					FlashStr.bitFlagReadInProgress = 1;
					if(u8DebugDataRead)
					{
						u32DebugMsglen =0;
						u32DebgMsgTxCntr=0;
						memset(u8DebugBuff,0x00,DBG_BUF_LEN);

						u8DebugDataRead =12;
						SerialImpDebug_Print("BUFFER MEM DATA START/r/n");
//						HAL_UART_Transmit_IT(&huart3, (uint8_t*)&"BUFFER MEM DATA START/r/n", 2048);
						FlashStr.u32FlashReadAddress =0;
						while(FlashStr.u32FlashReadAddress<FlashStr.u32FlashWriteAddress)
						{
							HAL_IWDG_Refresh(&hiwdg);
							memset((char *)FlashStr.cSFlashTransmitData,0x00,sizeof(FlashStr.cSFlashTransmitData));

							SFlash_Read(FlashStr.u32FlashReadAddress, (char *)FlashStr.cSFlashTransmitData, 2048);
							FlashStr.u32FlashReadAddress += 2048;
							HAL_IWDG_Refresh(&hiwdg);
							HAL_Delay(1000);
							HAL_IWDG_Refresh(&hiwdg);
							SerialImpDebug_Print(FlashStr.cSFlashTransmitData);

							//HAL_UART_Transmit_IT(&huart3, (uint8_t*)&FlashStr.cSFlashTransmitData, 2048);
						}
						SerialImpDebug_Print("BUFFER MEM DATA END/r/n");
//						HAL_UART_Transmit_IT(&huart3, (uint8_t*)&"BUFFER MEM DATA END/r/n", 2048);
						u8DebugDataRead = 0;
						FlashStr.u32FlashReadAddress = 0;
					}
					else
#endif
					{
						FlashStr.bitFlagReadInProgress = 1;
						memset((char *)FlashStr.cSFlashTransmitData, 0, sizeof(FlashStr.cSFlashTransmitData));
						if(FlashStr.u32FlashWriteAddress < (FlashStr.u32FlashReadAddress + g_u32FlashPayloadMaxSize))
							SFlash_Read(FlashStr.u32FlashReadAddress, (char *)FlashStr.cSFlashTransmitData, (FlashStr.u32FlashWriteAddress - FlashStr.u32FlashReadAddress));
						else
							SFlash_Read(FlashStr.u32FlashReadAddress, (char *)FlashStr.cSFlashTransmitData, g_u32FlashPayloadMaxSize);
						FlashStr.u32FlashReadAddress += g_u32FlashPayloadMaxSize;
						FlashStr.enmCurrentReadState = enmFlashReadArrangeData;
					}
				}
				break;

				case enmFlashReadArrangeData:
				{
					SFlash_Arrange_Read_Data();
//					if(strlen((char *)FlashStr.cSFlashTransmitData) != 0)
//					{
//						/* To separate individual payload data as max upload size is 1024 bytes	*/
//						char ctempUploadArray1[GSM_PAYLOAD_MAX_SIZE];	//[7][500]
//						char * cTempPtr = FlashStr.cSFlashTransmitData;
//						uint16_t u16TempSize = 0, u16Index = 0, u16SortedDataSize = 0;
//						uint16_t u16TransmitDataSize = strlen(FlashStr.cSFlashTransmitData);
//						FlashStr.u8BufferQueueCounts = 0;
//						memset(ctempUploadArray1, 0, sizeof(ctempUploadArray1));
//						while(cTempPtr != NULL)
//						{
//							//TODO CHECK.
//							u16TempSize = strlen(strtok(cTempPtr,"#"));    //
//
//							u16TempSize += 1;
//							memset(ctempUploadArray1, 0, sizeof(ctempUploadArray1));
//							strcpy(ctempUploadArray1, cTempPtr);//    , (size + 5));	[u16Index]
//					        strcat(ctempUploadArray1, "#");		//[u16Index]
//					        if(strlen(ctempUploadArray1) > 0 && u16Index < MAX_BUFF_QUEUE_SIZE)
//					        {
//					        	Queue_BuffEnequeue(&gsmBuffPayload,(char *)getSFlashDataString((char *)ctempUploadArray1, u16TempSize));
//					        	FlashStr.u8BufferQueueCounts++;
//					        }
//					        cTempPtr += u16TempSize;
//					        u16Index++;
//					        u16SortedDataSize += u16TempSize;
//					        if(u16SortedDataSize >= u16TransmitDataSize)
//					        {
//					        	cTempPtr = NULL;
//					       	}
//						}
//						FlashStr.bitFlagSFlashData = 1;
//					}

					//TODO check this state
					if(FlashStr.u32FlashReadAddress >= FlashStr.u32FlashWriteAddress)
					{
					  if(strlen((char *)FlashStr.cSFlashHalfReadData) != 0)
					  {
						  Queue_BuffEnequeue(&gsmBuffPayload,(char *)getSFlashDataString((char *)FlashStr.cSFlashHalfReadData, g_u32FlashPayloadMaxSize+1));
						  FlashStr.bitFlagSFlashData = 1;
						  FlashStr.u8BufferQueueCounts++;
					  }
					}


					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					FlashStr.enmCurrentReadState = enmFlashReadStatusReg;
				}
				break;

				default:
					FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
					break;
			}
		}
		break;

		case enmFLASH_MEMORY_ERASE:
		{
				FlashStr.bitFlagReadInProgress = 0;
				SFlash_Chip_Erase();
				FlashStr.bitFlagMemoryFull = 0;
				FlashStr.u16FlashPageEmptyByte = 0;

				/*	Writing SFlash address into EEPROM*/
				FlashStr.u32FlashWriteAddress = 0;
				FlashStr.u32FlashReadAddress = 0;

				g_stEepromBuffMemAdd.u32BuffFlashWriteAddress = FlashStr.u32FlashWriteAddress;
				g_stEepromBuffMemAdd.u32BuffFlashReadAddress =FlashStr.u32FlashReadAddress;
				EEPROM_WriteBuffMemData();

			  FlashStr.u32SerialFlashDelay =FIVEHUNDRED_MS;//TWO_MIN;
			  FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
		}
		break;

		default:
			FlashStr.enmCurrentOperation = enmFLASH_MEMORY_IDEAL;
			break;
	}
}

/****************************************************************************
 Function
 Purpose:
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
uint8_t SFlash_Read_Status_Register(void)
{
	uint8_t tx_var[3], rx_var[3];
//	SFlash_Chip_Select(DISABLE);
//	HAL_Delay(1);
	tx_var[0] = SFLASH_STATUS_REG_READ_CODE;
	SFlash_Chip_Select(ENABLE);
	HAL_Delay(1);
	HAL_SPI_TransmitReceive_IT(&hspi2, tx_var, rx_var, 2);
	/* wait till operation complete */
	HAL_Delay(5);
	SFlash_Chip_Select(DISABLE);

	return rx_var[1];
}

/**********************************************************************************
 Function Name: SFlash_Write_Enable
 Purpose: To make Serial flash Write enable, Making Status Register - 0x02
 Input: None.
 Return value: None.

 Change History:
 Author           Date                Remarks
 100229			23-03-2020
************************************************************************************/
void SFlash_Write_Enable(void)
{
	uint8_t tx_var[2];
	tx_var[0] = SFLASH_WRITE_EN_CODE;	//0x06;
	SFlash_Chip_Select(ENABLE);
	HAL_Delay(1);
	HAL_SPI_Transmit_IT(&hspi2, (uint8_t *)tx_var, 1);
	HAL_Delay(5);
	SFlash_Chip_Select(DISABLE);

}

/**********************************************************************************
 Function Name: SFlash_Write_Disable
 Purpose: To make Serial flash Write Disable & protection for all blocks
 	 	  i.e. Making Status Register - 0x1C
 Input:	None.
 Return value: None.

 Change History:
 Author           Date                Remarks
 100229			23-03-2020
************************************************************************************/
void SFlash_Write_Disable(void)
{
	uint8_t tx_var[2];
	tx_var[0] = SFLASH_WRITE_DIS_CODE;	//0x04;
	SFlash_Chip_Select(ENABLE);
	HAL_Delay(1);
	HAL_SPI_Transmit_IT(&hspi2, (uint8_t *) tx_var,1);
	SFlash_Chip_Select(DISABLE);
	HAL_Delay(5);
}

/**********************************************************************************
 Function Name: SFlash_Chip_Select
 Purpose: Enable or Disable chip select pin of Serial flash
 Input:	  uint8_t state - ENABLE/DISABLE
 Return value: None.
 Note(s)(if-any) :
					ENABLE  - To make chip select pin LOW
					DISABLE - To make chip select pin HIGH

 Change History:
 Author           Date                Remarks
 100229			03-05-2021
************************************************************************************/
void SFlash_Chip_Select(uint8_t state)
{
	if(state == ENABLE)
	{
		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET);		// CE LOW
	}
	else if(state == DISABLE)
	{
		HAL_GPIO_WritePin(SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET);		// CE High
	}
}

/****************************************************************************
 Function SFlash_Read
 Purpose: To read data from serial flash
 Input:	address - memory address
  	  	*ptr_read_data - pointer where to copy read data
  	  	size - size of data to be read
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SFlash_Read(uint32_t address, char * ptr_read_data, uint16_t size)
{
	char tx_var[10];
	memset(tx_var, 0, sizeof(tx_var));
	tx_var[0] = SFLASH_READ_HSPEED_CODE;
	tx_var[1] = (uint8_t)(address >> 16);
	tx_var[2] = (uint8_t)(address >> 8);
	tx_var[3] = (uint8_t)(address >> 0);
	tx_var[4] = 0;
	SFlash_Chip_Select(ENABLE);
	HAL_Delay(1);
	HAL_SPI_Transmit_IT(&hspi2, (uint8_t *)tx_var, 5);
	HAL_SPI_Receive_IT(&hspi2, (uint8_t *)ptr_read_data, size);
	HAL_Delay(150);
	SFlash_Chip_Select(DISABLE);
}

/****************************************************************************
 Function SFlash_Arrange_Read_Data()
 Purpose: Extract complete pay-load data from read data/ separate out half pay-load data
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      04-05-2021			100229
******************************************************************************/
void SFlash_Arrange_Read_Data(void)
{
	char temparray[GSM_PAYLOAD_MAX_SIZE+1]={0};
//	char ctempUploadArray1[GSM_PAYLOAD_MAX_SIZE]={0};
	//const char endstart[] = "#*";
	const char endstart[] = "*";
	char *ptrchar = FlashStr.cSFlashHalfReadData;
	char *ptrDataStartchar = FlashStr.cSFlashHalfReadData;
	char *ptrendchar;
	uint16_t size = 0,u16BuffQueueIndex=0;

	if(strlen(FlashStr.cSFlashHalfReadData) == 0)
		memcpy(FlashStr.cSFlashHalfReadData, FlashStr.cSFlashTransmitData, sizeof(FlashStr.cSFlashTransmitData));
	else
		strcat(FlashStr.cSFlashHalfReadData, FlashStr.cSFlashTransmitData);


	FlashStr.u8BufferQueueCounts = 0;

	ptrendchar = ptrchar;
	while( ptrchar != NULL)
	{
		ptrchar = strstr(ptrchar, endstart);
		if(ptrchar != NULL)
		{
			/*******************************************************/
			//TODO Fill in Queue
			ptrendchar = ptrchar;
			size = ptrchar - ptrDataStartchar;
			memset(temparray, 0, sizeof(temparray));
			memcpy(temparray,ptrDataStartchar,size);
//			size++;
			if(strlen(temparray) > 0 && u16BuffQueueIndex < MAX_BUFF_QUEUE_SIZE)
			{
				Queue_BuffEnequeue(&gsmBuffPayload,(char *)getSFlashDataString((char *)temparray, size));
				FlashStr.u8BufferQueueCounts++;
			}

			u16BuffQueueIndex++;

			ptrDataStartchar = ptrchar;
			++ptrchar;
			/*****************************************************/
		}
	}
	FlashStr.bitFlagSFlashData = 1;

//	++ptrendchar;
	size = ptrendchar - FlashStr.cSFlashHalfReadData;
	if(size > sizeof(FlashStr.cSFlashHalfReadData))
		size = sizeof (FlashStr.cSFlashHalfReadData) - 1;
	memset(FlashStr.cSFlashTransmitData, 0x00, sizeof(FlashStr.cSFlashTransmitData));
//	memcpy(FlashStr.cSFlashTransmitData, FlashStr.cSFlashHalfReadData, size);
	memset(temparray, 0, sizeof(temparray));
	if(strlen(ptrendchar) < sizeof(temparray) )
	{
		strcpy(temparray, ptrendchar);
		memset(FlashStr.cSFlashHalfReadData, 0, sizeof(FlashStr.cSFlashHalfReadData));
		if(strlen(temparray) < sizeof(FlashStr.cSFlashHalfReadData) )
		{
			strcpy(FlashStr.cSFlashHalfReadData, temparray);
		}
	}
	/* Single payload is copied in queue */

}

/**********************************************************************************
 Function Name: getSFlashDataString
 Purpose: to store flash read data into dynamic memory for uploading
 Input:	  uint8_t *ptr_read_data - Pointer to array in which read data is copied.

 Return value: char * chrptr - Dynamic mem address
 Note(s)(if-any) :

 Change History:
 Author           Date                Remarks
 100229			03-05-2021
************************************************************************************/
char * getSFlashDataString(char * chrptr, uint16_t memory_size)
{
//	char * sflashPayload = malloc(sizeof(char) * SFLASH_PAYLOAD_MAX_SIZE);
	char * sflashPayload = malloc(sizeof(char) * memory_size);
	if(sflashPayload != NULL)
	{
		memset((char *)sflashPayload, 0x00, (sizeof(sflashPayload) * sizeof(char)));
		strcpy((char *)sflashPayload, (char *)chrptr);
		return sflashPayload;
	}
	return NULL;
}

/****************************************************************************
 Function SFlash_Chip_Erase()
 Purpose: To erase serial flash memory
 Input:	None.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SFlash_Chip_Erase(void)
{
	uint8_t var = SFLASH_CHIP_ERASE_CODE;
	SFlash_Write_Enable();
	SFlash_Chip_Select(ENABLE);
	HAL_Delay(1);
	HAL_SPI_Transmit_IT(&hspi2, &var, 1);
	HAL_Delay(200);		// Time to erase full chip 100 ms
	SFlash_Chip_Select(DISABLE);
	HAL_Delay(200);		// Time to erase full chip 100 ms
	SFlash_Write_Disable();
}

/****************************************************************************
 Function SFlash_Read_Device_ID()
 Purpose: To read device id to set flash write limit
 Input:	None.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/
void SFlash_Read_Device_ID(void)
{
	uint8_t tx_var[10], rx_var[10];
	uint32_t address = 0;
	//uint8_t read_status = 0;

	//read_status =
	SFlash_Read_Status_Register();

	memset(tx_var, 0, sizeof(tx_var));
	memset(rx_var, 0, sizeof(rx_var));
	tx_var[0] = SFLASH_READ_DEVICE_ID;	//0x9F;	//

	tx_var[1] = (uint8_t)(address >> 16);
	tx_var[2] = (uint8_t)(address >> 8);
	tx_var[3] = (uint8_t)(address >> 0);
	SFlash_Chip_Select(ENABLE);
	HAL_Delay(1);
	HAL_SPI_Transmit_IT(&hspi2, (uint8_t *)tx_var, 4); //HAL_SPI_Transmit(&hspi2, (uint8_t *)tx_var, 4,20);//HAL_SPI_Transmit_IT(&hspi2, (uint8_t *)tx_var, 4);
	HAL_SPI_Receive_IT(&hspi2, (uint8_t *)rx_var, 2);//HAL_SPI_Receive(&hspi2, (uint8_t *)rx_var, 2,20);//HAL_SPI_Receive_IT(&hspi2, (uint8_t *)rx_var, 2);
	HAL_Delay(3);
	SFlash_Chip_Select(DISABLE);
	switch(rx_var[1])
	{
		case 0x16:
			SFLASH_SIZE = SFLASH_SIZE_64Mbit; /* 64 Mbit memory */
			break;
		case 0x17:
			SFLASH_SIZE = SFLASH_SIZE_128Mbit; /* 128 Mbit memory */
			break;
		default:
			SFLASH_SIZE = 0; /* Chip problem or absent */
			break;
	}
}
/****************************************************************************
 Function
 Purpose:
 Input:	None.
 Return value: None.

 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      03-05-2021			100229
******************************************************************************/

/**
  * @brief  Tx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	g_u32SpiTxCmpltFlag = 1;
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	g_u32SpiRxCmpltFlag = 1;
}

/**
  * @brief  Tx and Rx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
 void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	 g_u32SpiTxCmpltFlag = 1;
	 g_u32SpiRxCmpltFlag = 1;

}


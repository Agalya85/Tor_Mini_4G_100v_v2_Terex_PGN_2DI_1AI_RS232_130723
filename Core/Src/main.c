/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "system_reset.h"
#include "timer.h"
#include "deviceinfo.h"
#include "externs.h"
#include "payload.h"
#include "user_adc.h"
#include "string.h"
#include <stdlib.h>
#include "queue.h"
#include "user_can.h"
#include "user_MqttSubSperator.h"
#include "user_RS232.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define ON (1)
#define OFF (!ON)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if(DEBUGENABLE == TRUE)
    char temp[10] = "";
	static uint32_t counter;
	uint32_t CharacterCounter;
	char DebugBuff[3000]={'0'};
#endif
uint8_t gu8ModuleInitNotifyCnt= 0;
_Bool GSMInitCompleteFlag = FALSE;
_Bool boolstartCan = TRUE;

char * strLoc = NULL;
char * payload = NULL;

/*
 * uint8_t u8daignostic		7 				6 		   5		 4				 3 			   2   1   0
 *                     device Startup Module Re/Start Rev Service_Provider4 Service_Provider3 NW2 NW1 NW0
 */
uint8_t u8daignostic = 0;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_CAN1_Init();
  MX_IWDG_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_SPI2_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  updateDeviceSignature();
  HAL_TIM_Base_Start_IT(&htim7);
  LL_USART_EnableIT_RXNE(USART1);
  LL_USART_EnableIT_RXNE(USART2);
	#if(DEBUGENABLE == TRUE)
	 LL_USART_EnableIT_RXNE(USART3);
	#endif

	gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
	gu32PayloadQueueEnqueue = ONE_MIN;
	u8daignostic|=(1<<7); // Device Started

	g_stEepromBuffMemAdd.u32BuffFlashWriteAddress = 0;
	g_stEepromBuffMemAdd.u32BuffFlashReadAddress = 0;
	EEPROM_RetriveBuffMemData();
	FlashStr.u32FlashReadAddress =  g_stEepromBuffMemAdd.u32BuffFlashReadAddress;
	FlashStr.u32FlashWriteAddress = g_stEepromBuffMemAdd.u32BuffFlashWriteAddress;
	SFlash_Chip_Select(DISABLE);
	SFlash_Read_Device_ID();
	FlashStr.bitFlagSFlashData = 0;
	FlashStr.bitFlagMemoryFull = 0;
	FlashStr.bitFlagReadInProgress = 0;
	Queue_InitLiveQ(&gsmLivePayload);
	Queue_InitBuffQ(&gsmBuffPayload);

//	ReadImmobilizedStateFromEEPROM();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_IWDG_Refresh(&hiwdg);
	  	  HAL_GPIO_TogglePin(WD_FEED_uC_GPIO_Port, WD_FEED_uC_Pin);

	  	  if(boolstartCan)// && (Internal_Battery_Flag == FALSE))
	  	  {
	  		  canFilterConfig();
	  		  boolstartCan = FALSE;
	  		  Diagnostic(enmDiagnostic_CAN_ID);
	  		#if(DEBUGENABLE == TRUE)
	  		  strcat(DebugBuff,"\r\nDevice Started ......");
	  		#endif

	  	  }//end of if(startCan)

	  	  /*1. ADC module Task*/
	  	 Adc_FSM();
	  	 /* Serial Flash operations */
	  	 SerialFlashFSM();
	  	 /*RS232*/
       RS232();
	  #if(DEBUGENABLE == TRUE)
	  		CharacterCounter=strlen(DebugBuff);
	  	   if(CharacterCounter>1)
	  		{
	  		   if(counter<CharacterCounter)
	  			{
	  				if(!LL_USART_IsActiveFlag_TXE(USART3))
	  				{
	  				  /*Do Nothing . Wait For Previous Character Transmission */
	  				}
	  				else
	  				{
	  					//LL_USART_ReceiveData8(USART3, DebugBuff[counter++]);
	  					LL_USART_TransmitData8(USART3, DebugBuff[counter++]);
	  				}
	  			}
	  		   else if(counter==CharacterCounter)
	  	//                else
	  			{
	  			  memset(DebugBuff, 0x00,  sizeof(DebugBuff)); /* Clear  Buffer */

	  			  counter=0;
	  			  CharacterCounter=0;
	  			}
	  			else{}
	  		}//end of if(CharacterCounter>1)
	  #endif

	  	 parseCanMessageQueue();
//	  	executeCANQueries();
	  	   if((boolFOTACompleteFlag == TRUE) && (Queue_IsLiveQueueEmpty(&gsmLivePayload) == TRUE))
	  	   {
	  	   		systemReset();
	  	   }
	  	   if((gu32PayloadQueueEnqueue == 0) && (gu32FotaRquestFlag == FALSE)) //&& (GSMInitCompleteFlag == TRUE)
	  	   {
	  	   		Queue_LiveEnequeue(&gsmLivePayload,(char *)getMachineDataString());
//	  	   		if(boolInginitionStatus == ON)
//	  	   		{
//	  	   			gu32PayloadQueueEnqueue = TEN_SEC; /* Upload Frequency */
//	  	   		}
//	  	   		else
//	  	   		{
	  	   			gu32PayloadQueueEnqueue = TEN_SEC; /* Upload Frequency */
//	  	   		}

	  	   }//end of if((gu32PayloadQueueEnqueue == 0) && (gu32FotaRquestFlag == FALSE) ))

	  	   if(gsmInstance.u32GSMTimer > TWO_MIN)
	  		  gsmInstance.u32GSMTimer =TWO_SEC;

	  	  if(gu32GSMHangTimer == 0)
	  		  initGSMSIM868();

	  	  operateGSMSIM868();

	  	  if(SUBTriggerFlag == TRUE)
	  	  {
	  		SubRevicedString();
	  	  }//end of  if(SUBTriggerFlag == TRUE)
//	  	CheckImmobilization();
	  	/* LED Indications */
	  	  if(gu32LEDDelay == 0)
	  	  {
	  		  HAL_GPIO_WritePin(Comm_LED_GPIO_Port, Comm_LED_Pin, GPIO_PIN_RESET);

	  		  //for module reset
	  		  if((gu8ModuleInitNotifyCnt <= 3) && (gu8ModuleInitFlag == TRUE))
	  		  {
	  			  HAL_GPIO_TogglePin(GPS_LED_GPIO_Port, GPS_LED_Pin);
	  			  gu8ModuleInitNotifyCnt++;
	  		  }
	  		  else
	  		  {
	  			 gu8ModuleInitNotifyCnt = 0;
	  			 gu8ModuleInitFlag = FALSE;
	  		  }

	  		  if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI4_Pin) == HIGH)            //check pin state
	  		  {
	  			  boolInginitionStatus1 = FALSE;
	  		  }
	  		  else if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI4_Pin) == LOW)
	  		  {
	  			  boolInginitionStatus1 = TRUE;
//	  			  if(gu32PayloadQueueEnqueue > TEN_SEC)
//	  			  {
//	  				  gu32PayloadQueueEnqueue = TEN_SEC;
//	  			  }
	  		  }

	  		  if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI5_Pin) == HIGH)            //check pin state
	  		  {
	  			  boolInginitionStatus2 = FALSE;
	  		  }
	  		  else if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI5_Pin) == LOW)
	  		  {
	  			  boolInginitionStatus2 = TRUE;
//	  			  if(gu32PayloadQueueEnqueue > TEN_SEC)
//	  			  {
//	  				  gu32PayloadQueueEnqueue = TEN_SEC;
//	  			  }
	  		  }

	  		  gu32LEDDelay = ONE_SEC;
	  	  }//End of if(gu32LEDDelay == 0)
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

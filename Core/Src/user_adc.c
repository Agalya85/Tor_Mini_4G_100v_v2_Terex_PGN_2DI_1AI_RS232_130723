/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "adc.h"
#include "user_adc.h"
#include "externs.h"
#include "payload.h"
/* USER CODE BEGIN 0 */
#include "tim.h"
#include "timer.h"

static uint32_t s_u32AdcConvCpltFlag = 0;
StructAdc g_stAdcData;



#define ADC_STAE_START_READING    	1
#define ADC_STATE_WAIT			 	2
#define ADC_STATE_CALC				3
#define ADC_STAE_IDLE           	4



/****************************************************************************
 Function: void ADCFSM(void)
 Purpose: Runs from main to process the ADC values as per the ADC states
 Input:	None.
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        21/01/19			Updated function name as per guidelines, Added info header
******************************************************************************/

void Adc_FSM (void)
{
	static	uint32_t s_u32AdcRawDataTotal[8],s_u32AdcRawData[8];
	static uint32_t s_u32AdcState=ADC_STAE_START_READING;
	static uint32_t s_u32AdcSamplingIndex = 0;
	uint8_t i=0;
	float f32TempVar = 0;
	//uint32_t u32TempValue=0;
	switch(s_u32AdcState)
	{
		case ADC_STAE_START_READING:
		{
			s_u32AdcConvCpltFlag = 0;
			HAL_ADC_Start_DMA(&hadc1, (uint32_t*)s_u32AdcRawData, 4);
			gu32ADCRestartTimer = 0;
			s_u32AdcState = ADC_STATE_WAIT;
		}break;
		case ADC_STATE_WAIT:
		{
			if(s_u32AdcConvCpltFlag != 0)
			{
				s_u32AdcConvCpltFlag = 0;
				for(i=0;i<4;i++)
				{
					s_u32AdcRawDataTotal[i] += s_u32AdcRawData[i];
				}

				for(i=0;i<4;i++)
				{
					s_u32AdcRawData[i] = 0;
				}

				s_u32AdcSamplingIndex++;
				gu32ADCRestartTimer = TEN_MS;
				s_u32AdcState = ADC_STATE_CALC;
			}
		}break;
		case ADC_STATE_CALC:
		{
			if(s_u32AdcSamplingIndex >= ADC_AVG_SAMPLE_COUNT)
			{
				s_u32AdcSamplingIndex = 0;
				//Avg All AIRawData
				for(i=0;i<5;i++)
				{
					s_u32AdcRawDataTotal[i] /= ADC_AVG_SAMPLE_COUNT;
				}

				/*1 ADC BATT*/
				f32TempVar = 0;
				f32TempVar = ((float)s_u32AdcRawDataTotal[0]*(float)3.3)*(float)1.5;
				f32TempVar /= (float)4095;
				g_stAdcData.u32IntBatVtg =  f32TempVar;//*(float)1000.0;//Volt to Mili Volt

				/*2&3 AI1 And AI2 Raw data*/
				g_stAdcData.u32AI1RawData = s_u32AdcRawDataTotal[2];

				g_stAdcData.u32AI2RawData = s_u32AdcRawDataTotal[1];

				/*4 Vin */
				f32TempVar = 0;
				s_u32AdcRawDataTotal[3] += (uint32_t)80;
				/*
				 As per Discuss with milind vaze we need add 80 ADC Count for match the reading as diode Drop*/
				f32TempVar = (float)(s_u32AdcRawDataTotal[3])*(float)3.3*(float)37.58536585;
				f32TempVar /= (float)4095;
//				f32TempVar += (float)1.5; //Diode Drop
				g_stAdcData.u32PwrSupplyVtg = f32TempVar;




				for(i=0;i<4;i++)
				{
					s_u32AdcRawDataTotal[i] = 0;
				}
				s_u32AdcState = ADC_STAE_IDLE;
			}
			else
			{
				if(gu32ADCRestartTimer == 0)
				{
					s_u32AdcState = ADC_STAE_START_READING;
				}
			}
		}break;
		case ADC_STAE_IDLE:
		{
			if(gu32ADCRestartTimer == 0)
			{
				s_u32AdcState = ADC_STAE_START_READING;
				gu32ADCRestartTimer = TEN_SEC;
			}
		}break;
		default:
		{
			HAL_ADC_Stop_DMA(&hadc1);
			s_u32AdcState = ADC_STAE_START_READING;
		}
	}
}

/****************************************************************************
 Function: void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
 Purpose: callback function for ADC conversion complete ISR
 Input:	ADC structure.
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        21/01/19			Added info header
******************************************************************************/
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_ADC_Stop_DMA(&hadc1);
	s_u32AdcConvCpltFlag = (uint32_t)1;
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

#include "main.h"
#include "gpio.h"
#include "timer.h"
#include "applicationDefines.h"
#include "externs.h"


_Bool boolInginitionStatus1 = FALSE;
_Bool boolInginitionStatus2 = FALSE;

 void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)             //interrupt handler
 {
 		if(GPIO_Pin == DI_EXTI4_Pin)                         //check interrupt for DI EXTI4 pin
 		{
 			if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI4_Pin) == HIGH)            //check pin state
 			{
 				boolInginitionStatus1 = FALSE;
 		    }
 			else if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI4_Pin) == LOW)
 			{
 				boolInginitionStatus1 = TRUE;
// 				if(gu32PayloadQueueEnqueue > TEN_SEC)
// 				{
// 					gu32PayloadQueueEnqueue = TEN_SEC;
// 				}
 			}
 		}

 		if(GPIO_Pin == DI_EXTI5_Pin)                         //check interrupt for DI EXTI4 pin
 		{
 			if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI5_Pin) == HIGH)            //check pin state
 			{
 				boolInginitionStatus2 = FALSE;
 		    }
 			else if(HAL_GPIO_ReadPin(GPIOB, DI_EXTI5_Pin) == LOW)
 			{
 				boolInginitionStatus2 = TRUE;
// 				if(gu32PayloadQueueEnqueue > TEN_SEC)
// 				{
// 					gu32PayloadQueueEnqueue = TEN_SEC;
// 				}
 			}
 		}
 }

///*
// * user_Immobilization.c
// *
// *  Created on: Jun 15, 2023
// *      Author: admin
// */
//
//#include "user_Immobilization.h"
//
///*Should be added to extern.h*/
//uint8_t Current_Immobilization_State = FALSE;
//uint8_t Previous_Immobilization_State = FALSE;
//
//
//
//
//void CheckImmobilization()
//{
//
//	if(Previous_Immobilization_State == TRUE)
//	{
//		HAL_GPIO_WritePin(DO_uC_GPIO_Port, DO_uC_Pin, GPIO_PIN_SET);
//	}
//	else if(Previous_Immobilization_State == FALSE)
//	{
//		HAL_GPIO_WritePin(DO_uC_GPIO_Port, DO_uC_Pin, GPIO_PIN_RESET);
//	}
//	else
//	{
//		HAL_GPIO_WritePin(DO_uC_GPIO_Port, DO_uC_Pin, GPIO_PIN_RESET);
//	}
//}
//
//void WriteImobilizedStateToEEPROM(uint8_t value)
//{
//	if(HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, EEPROM_IMMOBI_MEM_ADD_DATA ,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&value,sizeof(value),1000) != HAL_OK)
//		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
//	else
//		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
//}
//
//void ReadImmobilizedStateFromEEPROM()
//{
//	if(HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, EEPROM_IMMOBI_MEM_ADD_DATA ,I2C_MEMADD_SIZE_16BIT,	(uint8_t *)&Previous_Immobilization_State,sizeof(Previous_Immobilization_State),1000) != HAL_OK)
//			assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
//		else
//			assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);
//}


/*
  *****************************************************************************
  * @file    UserEeprom.c
  * @author  Anil More
  * @version
  * @date
  * @brief   EEPROM over I2C functions
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

#include "UserEeprom.h"
#include "externs.h"
#include "i2c.h"
#include "error_handler.h"
#include"iwdg.h"



/* Global variable, structures, enum s Definition Start */

stEepromBuffMem g_stEepromBuffMemAdd;



/* Global variable Definition End */


/* Private Functions Declaration Start */
static uint32_t EEPROM_CalcCheckSum(uint8_t* pu8Data, uint16_t u16Size);
/* Private Functions Declaration End  */

/* Private Functions Definition Start */
static uint32_t EEPROM_CalcCheckSum(uint8_t* pu8Data, uint16_t u16Size)
{
	uint32_t u32Checksum = 0;
	uint16_t u16LoopCntr = 0;
	for(u16LoopCntr =0;u16LoopCntr<(u16Size-4);u16LoopCntr++)
	{
		u32Checksum += (pu8Data[u16LoopCntr]);
	}

	//Increment it by one for Ignoring the  All Zeros data
	u32Checksum++;
	return u32Checksum;
}

/* Private Functions Definition End */
void EEPROM_RetriveBuffMemData(void)
{
	/*1 read first copy*/
	memset((uint8_t*)&g_stEepromBuffMemAdd,0x00,sizeof(g_stEepromBuffMemAdd));
	if(HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, EEPROM_BUFF_MEM_ADD_DATA_COPY_1_ADD ,I2C_MEMADD_SIZE_16BIT,	(uint8_t *)&g_stEepromBuffMemAdd,sizeof(g_stEepromBuffMemAdd),1000) != HAL_OK)
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
	else
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);

	if(g_stEepromBuffMemAdd.u32CheckSum  != EEPROM_CalcCheckSum((uint8_t*)&g_stEepromBuffMemAdd,sizeof(g_stEepromBuffMemAdd)))
	{
		/*2 First copy CRC is not matched Read Second copy*/
		memset((uint8_t*)&g_stEepromBuffMemAdd,0x00,sizeof(g_stEepromBuffMemAdd));
		if(HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, EEPROM_BUFF_MEM_ADD_DATA_COPY_2_ADD ,I2C_MEMADD_SIZE_16BIT,	(uint8_t *)&g_stEepromBuffMemAdd,sizeof(g_stEepromBuffMemAdd),1000) != HAL_OK)
			assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
		else
			assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);

		if(g_stEepromBuffMemAdd.u32CheckSum != EEPROM_CalcCheckSum((uint8_t*)&g_stEepromBuffMemAdd,sizeof(g_stEepromBuffMemAdd)))
		{
			/*3. If both copy are corrupted the fill default values*/
			memset((uint8_t*)&g_stEepromBuffMemAdd,0x00,sizeof(g_stEepromBuffMemAdd));
			g_stEepromBuffMemAdd.u32BuffFlashReadAddress = 0;
			g_stEepromBuffMemAdd.u32BuffFlashWriteAddress  = 0;
			//TODO ERASE The Buffer memory or Read Last write location and increment the write address
		}
		else
		{
			//Second copy read successfully.
		}
	}
	else
	{
		//First copy read successfully.
	}
}

void EEPROM_WriteBuffMemData(void)
{
	  HAL_IWDG_Refresh(&hiwdg);

	g_stEepromBuffMemAdd.u32CheckSum = EEPROM_CalcCheckSum((uint8_t*)&g_stEepromBuffMemAdd,sizeof(g_stEepromBuffMemAdd));
	if(HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, EEPROM_BUFF_MEM_ADD_DATA_COPY_1_ADD ,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&g_stEepromBuffMemAdd,sizeof(g_stEepromBuffMemAdd),1000) != HAL_OK)
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
	else
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);

	HAL_IWDG_Refresh(&hiwdg);
	HAL_Delay(6);

	if(HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, EEPROM_BUFF_MEM_ADD_DATA_COPY_2_ADD ,I2C_MEMADD_SIZE_16BIT,(uint8_t *)&g_stEepromBuffMemAdd,sizeof(g_stEepromBuffMemAdd),1000) != HAL_OK)
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_ACTIVE);
	else
		assertError(enmTORERRORS_MEM_I2C,enmERRORSTATE_NOERROR);

}


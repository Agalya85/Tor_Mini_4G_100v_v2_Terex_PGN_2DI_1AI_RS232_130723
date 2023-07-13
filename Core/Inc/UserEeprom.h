/*
 * UserEeprom.h
 *
 *  Created on: Dec 9, 2022
 *      Author: anilm
 */

#ifndef INC_USEREEPROM_H_
#define INC_USEREEPROM_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define CONFIG_AT24XX_SIZE 		(32)			/* Configure Used EEPROM memory */


/*AT24C08, 8K SERIAL EEPROM:
 * Internally organized with 64 pages of 16 bytes each,
 * the 8K requires a 10-bit data word address for random word addressing*/

#define EEPROM_MEMORY_ADDRESS_SIZE					(I2C_MEMADD_SIZE_16BIT)
#define EEPROM_ADDRESS 								(0xA0)




#define EEPROM_PAGE1_ADD						(uint16_t)0x00
#define EEPROM_PAGE2_ADD						(uint16_t)0x10
#define EEPROM_PAGE3_ADD						(uint16_t)0x20
#define EEPROM_PAGE4_ADD						(uint16_t)0x30
#define EEPROM_PAGE5_ADD						(uint16_t)0x40
#define EEPROM_PAGE6_ADD						(uint16_t)0x50
#define EEPROM_PAGE7_ADD						(uint16_t)0x60
#define EEPROM_PAGE8_ADD						(uint16_t)0x70
#define EEPROM_PAGE9_ADD						(uint16_t)0x80
#define EEPROM_PAGE10_ADD						(uint16_t)0x90
#define EEPROM_PAGE11_ADD						(uint16_t)0xA0
#define EEPROM_PAGE12_ADD						(uint16_t)0xB0
#define EEPROM_PAGE13_ADD						(uint16_t)0xC0
#define EEPROM_PAGE14_ADD						(uint16_t)0xD0 //(uint16_t)0xB0


//total 12 Bytes required for Buffer memory Add storing.
#define EEPROM_BUFF_MEM_ADD_DATA_COPY_1_ADD			EEPROM_PAGE1_ADD //(uint16_t)0x00  //0x00 to 0x0B	Buff Mem add First Copy of Buff mem Read And Wr Add with
																	//total 12 bytes 0 to 0x0B ( with 0 and 0x0B)
#define EEPROM_BUFF_MEM_ADD_DATA_COPY_2_ADD			EEPROM_PAGE2_ADD //(uint16_t)0x10 	//0x10 to 0x1B	Buff Mem add	Second Copy of Buff mem Read And Wr Add



void EEPROM_RetriveBuffMemData(void);
void EEPROM_WriteBuffMemData(void);


/*Internal Hour meter*/
typedef struct
{
	uint32_t u32BuffFlashReadAddress;
	uint32_t u32BuffFlashWriteAddress;
	uint32_t u32CheckSum;
}stEepromBuffMem;

extern stEepromBuffMem g_stEepromBuffMemAdd;


#endif /* INC_USEREEPROM_H_ */

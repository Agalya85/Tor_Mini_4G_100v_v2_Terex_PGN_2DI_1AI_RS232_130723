/*
 * serial_flash_fsm.h
 *
 *  Created on: 01-Nov-2021
 *      Author: Admin
 */

#ifndef INC_SERIAL_FLASH_W25_FSM_H_
#define INC_SERIAL_FLASH_W25_FSM_H_

#include "main.h"

#define GSM_PAYLOAD_MAX_SIZE        (2048) //1024
#define ENABLEMEMORYSPI 			(TRUE)

/* Device IDs */
#define SERIAL_FLASH_SST25VF010A_DEVICE_ID	0x49
#define SERIAL_FLASH_SST25VF010A_MNF_ID		0xBF

/* Chip select operations */
//#define ENABLE		 (uint8_t)	1
//#define DISABLE		 (uint8_t)	0

#define FLASHMEMORYBUSY			1
#define FLASHMEMORYNOTBUSY		0

#define FLASHMEMORYWRITEDISABLED	1
#define FLASHMEMORYWRITEENABLED		0

/* Serial Flash Defines */
#define DUMMY_DATA						(0x00)
#define SFLASH_WRITE_STATUS_CODE		(0x01)
#define SFLASH_WRITE_BYTE_CODE			(0x02)
#define SFLASH_WRITE_PAGE_CODE			(0x02)	// for W25Q128
#define SFLASH_READ_CODE				(0x03)
#define SFLASH_READ_HSPEED_CODE			(0x0B)
#define SFLASH_WRITE_DIS_CODE			(0x04)
#define SFLASH_STATUS_REG_READ_CODE		(0x05)
#define SFLASH_STATUS_REG_WRITE_CODE	(0x50)
#define SFLASH_WRITE_EN_CODE			(0x06)
#define SFLASH_CHIP_ERASE_CODE			(0x60)		// or 0xC7
#define SFLASH_AAI_CODE					(0xAF)		// NA in W25Q128
#define SFLASH_SECTOR_ERASE_CODE		(0x20)
#define SFLASH_READ_DEVICE_ID			(0x90)
#define SFLASH_START_WRITE_ADDRESS		(0x00)
#define SFLASH_SIZE_128Mbit				(0x00FFFFFF) // 128MBit-16MByte size
#define SFLASH_SIZE_64Mbit				(0x007FFFFF) // 64MBit-8MByte size
//#define SFLASH_SIZE						SFLASH_SIZE_64Mbit
#define SFLASH_PAYLOAD_MAX_SIZE 			(2048)//(2048+1024)//(750)	//(350)

typedef enum
{
	enmFLASH_MEMORY_IDEAL = 0,
	enmFLASH_MEMORY_WRITE,
	enmFLASH_MEMORY_READ,
	enmFLASH_MEMORY_ERASE
}enmFlashMemoryOperation;

typedef enum
{
	enmFlashWrCopyData=0,
	enmFlashWriteStatusReg,
	enmFlashWriteEnable,
	enmFlashWriteEnableCheck,
	enmFlashWriteData,

}enmFlashWriteStates;

typedef enum
{
	enmFlashReadStatusReg = 0,
	enmFlashReadData,
	enmFlashReadArrangeData
}enmFlashReadStates;


typedef struct
{
	uint32_t u32FlashWriteAddress;
	uint32_t u32FlashReadAddress;
	uint16_t u16FlashPageEmptyByte;
	uint16_t u16FlashByteWriten;
	uint16_t u16FlashWriteIndex;
	volatile uint32_t u32SerialFlashDelay;
	uint8_t u8BufferQueueCounts;

	char cSFlashTransmitData[SFLASH_PAYLOAD_MAX_SIZE+1];
	char cSFlashHalfReadData[SFLASH_PAYLOAD_MAX_SIZE+SFLASH_PAYLOAD_MAX_SIZE];
	char cSFlashWriteData[GSM_PAYLOAD_MAX_SIZE];

	_Bool bitFlagDataWrite;
	_Bool bitFlagMemoryFull;
	_Bool bitFlagSFlashData;
	_Bool bitFlagReadInProgress;

	volatile enmFlashMemoryOperation enmCurrentOperation;
	volatile enmFlashWriteStates enmCurrentWriteState;
	volatile enmFlashReadStates enmCurrentReadState;
}StructSFlash;

extern StructSFlash FlashStr;

void SerialFlashFSM(void);
uint8_t SFlash_Read_Status_Register(void);
void SFlash_Write_Enable(void);
void SFlash_Write_Disable(void);
void SFlash_Chip_Select(uint8_t state);
void SFlash_Read(uint32_t address, char * ptr_read_data, uint16_t size);
void SFlash_Arrange_Read_Data(void);
char * getSFlashDataString(char * chrptr, uint16_t memory_size);
void SFlash_Chip_Erase(void);
void SFlash_Read_Device_ID(void);
#endif /* INC_SERIAL_FLASH_W25_FSM_H_ */

/*
 * user_flash.c
 *
 *  Created on: Apr 22, 2022
 *      Author: admin
 */

#include"main.h"
#include "user_flash.h"
#include"stm32l4xx_hal_flash.h"
#include <string.h>
#include <stdlib.h>
#include "externs.h"
#include "gsmEC200U.h"


//#if(ENABLEFWUPGRADE == TRUE)

//#if   (defined ( __CC_ARM ))
//__IO uint32_t VectorTable[48] __attribute__((at(0x20000000)));
//#elif (defined (__ICCARM__))
//#pragma location = 0x20000000
//__no_init __IO uint32_t VectorTable[48];
//#elif defined   (  __GNUC__  )
//__IO uint32_t VectorTable[48] __attribute__((section(".RAMVectorTable")));
//#endif

//typedef void (*jumpFunction) (void);
//
//pFunction JumpToApplication = NULL;

uint32_t u32MemoryDestination = 0;
uint32_t u32LastMemoryWriteLocation = 0;
char u32BOOT_MEM_Y_JUMP[]	 = {"0x59"};


/****************************************************************************
 Function: initVectorTable
 Purpose: Copy the vector table from the Flash (mapped at the base of the application
     	  load address) to the base address of the SRAM at 0x20000000.

 Input:	None
 Return value: None

 Note(s)(if-any) :

 Copy the vector table from the Flash (mapped at the base of the application
 load address ) to the base address of the SRAM at 0x20000000.

 Change History:
 Author            	Date                Remarks
 KloudQ Team        26/03/2020			initial Definitions
******************************************************************************/
//void initVectorTable(enmBootMemorySegment memorytouse)
//{
//	uint32_t u32LoopCounter = 0;
//
//	switch(memorytouse)
//	{
//		case enmBOOTSEGMENT_X:
//			for(u32LoopCounter = 0; u32LoopCounter < 48; u32LoopCounter++)
//			{
////				VectorTable[u32LoopCounter] = *(__IO uint32_t*)(FLASH_X_START_ADDR + (u32LoopCounter << 2));
//			}
//			 /* Enable the SYSCFG peripheral clock*/
//			__HAL_RCC_SYSCFG_CLK_ENABLE();
//			/* Remap SRAM at 0x00000000 */
//			__HAL_SYSCFG_REMAPMEMORY_SRAM();
//		break;
//
//		case enmBOOTSEGMENT_Y:
//			for(u32LoopCounter = 0; u32LoopCounter < 48; u32LoopCounter++)
//			{
////				VectorTable[u32LoopCounter] = *(__IO uint32_t*)(FLASH_Y_START_ADDR + (u32LoopCounter << 2));
//			}
//			/* Enable the SYSCFG peripheral clock*/
//			__HAL_RCC_SYSCFG_CLK_ENABLE();
//			/* Remap SRAM at 0x00000000 */
//			__HAL_SYSCFG_REMAPMEMORY_SRAM();
//		break;
//
//		default:
//			/* Memory Region Not supported for FOTA / IAP */
//		break;
//	}
//
//	u32LoopCounter = 0;
//}

/****************************************************************************
 Function: initFWSelection
 Purpose: Jump to production FW

 Input:	None
 Return value: None

 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        31/03/2020			initial Definitions
******************************************************************************/
//uint32_t gu32firmware_addr = 0;
//uint32_t JumpAddress = 0;
//void initFWSelection(void)
//{
//
//	/* Get the starting address from BOOTMEMORYINFO */
//	/* Jump to user application */
//
//	gu32firmware_addr = BOOTMEMORYINFO;
//
//
//	if(gu32firmware_addr == BOOT_MEM_X_JUMP)
//	{
//		/* User Application in X memory Region */
////		initVectorTable(enmBOOTSEGMENT_X);
//
////		if (((*(__IO uint32_t*)FLASH_X_START_ADDR) & 0x2FFE0000 ) == 0x20000000)
////		{
////			JumpAddress = *(__IO uint32_t*) (FLASH_X_START_ADDR + 4);
////			JumpToApplication = (pFunction) JumpAddress;
////			/* Initialize user application's Stack Pointer */
////			__set_MSP(*(__IO uint32_t*) FLASH_X_START_ADDR);
////			JumpToApplication();
////		}
//	}
//	else if(gu32firmware_addr == BOOT_MEM_Y_JUMP)
//	{
//		/* User Application in Y memory Region */
////		initVectorTable(enmBOOTSEGMENT_Y);
//		if (((*(__IO uint32_t*)FLASH_Y_START_ADDR) & 0x2FFE0000 ) == 0x20000000)
//		{
//			JumpAddress = *(__IO uint32_t*) (FLASH_Y_START_ADDR + 4);
//			JumpToApplication = (pFunction) JumpAddress;
//			/* Initialize user application's Stack Pointer */
//			__set_MSP(*(__IO uint32_t*) FLASH_Y_START_ADDR);
//			JumpToApplication();
//		}
//	}
//	else
//	{
////		if(gu32firmware_addr == BOOT_MEM_X)
////			initVectorTable(enmBOOTSEGMENT_X);
////		else if(gu32firmware_addr == BOOT_MEM_Y)
////			initVectorTable(enmBOOTSEGMENT_Y);
////		else
////			initVectorTable(enmBOOTSEGMENT_X);
//
////		JumpAddress = 0;
//	}
//}


/**
  * @brief  Unlocks Flash for write access
  * @param  None
  * @retval None
  */
void FLASH_If_Init(void)
{
  HAL_FLASH_Unlock();

  /* Clear pending flags (if any) */
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);
}

/****************************************************************************
 Function WriteDatatoFlash
 Purpose: Write Data to Internal Flash
 Input:	uint8_t *data,uint32_t writeCycle
 Return value: uint32_t status SUCCESS/ERROR

 Note(s)(if-any) :

 	 // Continuation data ...start after last known config
 u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,0);
 	 //Fresh Data ..Start from Sector Start
 u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,1);


 Change History:
 Author           	Date                Remarks
 KloudQ Team      26-07-2021			Initial Definition
******************************************************************************/
uint32_t WriteDatatoFlash(uint8_t *data,uint32_t writeCycle)
{
	u32FotaChunckLength = gsmInstance.u32FotaFileSizeBytes;
	if(FLASH_If_Write((uint64_t *)data,u32FotaChunckLength,getNewFirmwareStorageLocation(),writeCycle) == FLASHIF_OK)
//	if(FLASH_If_Write((uint64_t *)data,u32FotaChunckLength,enmBOOTSEGMENT_Y_JUMP,writeCycle) == FLASHIF_OK)

		return SUCCESS;
	else
		return FAIL;
}

/**
  * @brief  This function writes a data buffer in flash (data are 32-bit aligned).
  * @note   After writing data buffer, the flash content is checked.
  * @param  FlashAddress: start address for writing data buffer
  * @param  Data: pointer on data buffer
  * @param  DataLength: length of data buffer (unit is 32-bit word)
  * @retval 0: Data successfully written to Flash memory
  *         1: Error occurred while writing data in Flash memory
  *         2: Written Data in flash memory is different from expected one
  */
uint32_t FLASH_If_Write(uint64_t* Data ,uint32_t DataSize,enmBootMemorySegment BootMemoryRegion, uint32_t writeCycle)
{

   uint32_t sofar=0,numofwords = 0;

  numofwords = ((DataSize/8)+((DataSize%8)!=0));

  if(BootMemoryRegion == enmBOOTSEGMENT_X_JUMP )
  {
	  if(writeCycle == 1)
	  		  u32MemoryDestination = FLASH_X_START_ADDR;
	  else
		  u32MemoryDestination = u32LastMemoryWriteLocation;

	  /* Unlock the Flash to enable the flash control register access *************/
	  FLASH_If_Init();

	 /* Program the user Flash area word by word
	  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
	  while (sofar<numofwords)
	  {
		  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, u32MemoryDestination,*(uint64_t*)(Data+sofar)) == HAL_OK)
		  {
			  /* Check the written value */
			  if (*(uint64_t*)u32MemoryDestination != *(uint64_t*)(Data+sofar))
			  {
				  HAL_FLASH_Lock();
				  /* Flash content doesn't match SRAM content */
				  return(FLASHIF_WRITEVERIFY_ERROR);

			  }
			  u32MemoryDestination += 8;  // use StartPageAddress += 2 for half word and 8 for double word
			  sofar++;
		  }
		  else
		  {
			  /* Error occurred while writing data in Flash memory*/
			  /* Lock the Flash to disable the flash control register access (recommended
				      to protect the FLASH memory against possible unwanted operation) *********/
			  HAL_FLASH_Lock();
			  return (FLASHIF_WRITING_ERROR);
		  }
		}//end of while (sofar<numofwords)
		  /* Lock the Flash to disable the flash control register access (recommended
		  	      to protect the FLASH memory against possible unwanted operation) *********/
	  	  HAL_FLASH_Lock();
	  	 u32LastMemoryWriteLocation = u32MemoryDestination;
		  return (FLASHIF_OK);
	}//end of if(BootMemoryRegion == enmBOOTSEGMENT_X_JUMP )

  else if(BootMemoryRegion == enmBOOTSEGMENT_Y_JUMP )
    {
  	  if(writeCycle == 1)
  	  		  u32MemoryDestination = FLASH_Y_START_ADDR;
  	  else
  		  u32MemoryDestination = u32LastMemoryWriteLocation;

  	  /* Unlock the Flash to enable the flash control register access *************/
  	  FLASH_If_Init();

  	 /* Program the user Flash area word by word
  	  (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/
  	  while (sofar<numofwords)
  	  {
  		  if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, u32MemoryDestination,*(uint64_t*)(Data+sofar)) == HAL_OK)
  		  {
  			  /* Check the written value */
  			  if (*(uint64_t*)u32MemoryDestination != *(uint64_t*)(Data+sofar))
  			  {
  				  HAL_FLASH_Lock();
  				  /* Flash content doesn't match SRAM content */
  				  return(FLASHIF_WRITEVERIFY_ERROR);

  			  }
  			  u32MemoryDestination += 8;  // use StartPageAddress += 2 for half word and 8 for double word
  			  sofar++;
  		  }
  		  else
  		  {
  			  /* Error occurred while writing data in Flash memory*/
  			  /* Lock the Flash to disable the flash control register access (recommended
  				      to protect the FLASH memory against possible unwanted operation) *********/
  			  HAL_FLASH_Lock();
  			  return (FLASHIF_WRITING_ERROR);
  		  }
  		}//end of while (sofar<numofwords)
  		  /* Lock the Flash to disable the flash control register access (recommended
  		  	      to protect the FLASH memory against possible unwanted operation) *********/
  	  	  HAL_FLASH_Lock();
  	  	 u32LastMemoryWriteLocation = u32MemoryDestination;
  		  return (FLASHIF_OK);
  	}//end of else if(BootMemoryRegion == enmBOOTSEGMENT_y_JUMP )
  else
  {
	  HAL_FLASH_Lock();
	  return (FLASHIF_WRITING_ERROR);
  }


}


uint32_t GetPage(uint32_t Addr)
{
  uint32_t page = 0;

  page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
  return page;
}

uint32_t  FLASH_If_Erase(uint32_t state, enmBootMemorySegment BootMemoryRegion)
{
	uint32_t FirstPage = 0,EndPage = 0, NbOfPages = 0,PAGEError = 0;
	static FLASH_EraseInitTypeDef EraseInitStruct;

	if(BootMemoryRegion == enmBOOTSEGMENT_X_JUMP )
	{
		/* Get the 1st page to erase */
		FirstPage = GetPage(FLASH_X_START_ADDR);

		/* Get the last page to erase */
		EndPage = GetPage(FLASH_X_END_ADDR);

		/* Get the number of pages to erase from 1st page */
//		NbOfPages = ((EndPage - FirstPage))+1;
		NbOfPages = ((EndPage - FirstPage));

		/* Fill EraseInit structure*/
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Banks     = FLASH_BANK_1;
		EraseInitStruct.Page		= FirstPage;
		EraseInitStruct.NbPages	= NbOfPages;

		/* Unlock the Flash to enable the flash control register access *************/
		FLASH_If_Init();

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return (FLASHIF_ERASE_ERROR);
		}
		else
		{
			HAL_FLASH_Lock();
			return (FLASHIF_ERASE_OK);
		}
	}// end of if(BootMemoryRegion == enmBOOTSEGMENT_X_JUMP )

	else if(BootMemoryRegion == enmBOOTSEGMENT_Y_JUMP )
	{
		/* Get the 1st page to erase */
		FirstPage = GetPage(FLASH_Y_START_ADDR);

		/* Get the last page to erase */
		EndPage = GetPage(FLASH_Y_END_ADDR);

		/* Get the number of pages to erase from 1st page */
		NbOfPages = ((EndPage - FirstPage));

		/* Fill EraseInit structure*/
		EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
		EraseInitStruct.Banks     = FLASH_BANK_1;
		EraseInitStruct.Page		= FirstPage;
		EraseInitStruct.NbPages	= NbOfPages;

		/* Unlock the Flash to enable the flash control register access *************/
		FLASH_If_Init();

		if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
		{
			HAL_FLASH_Lock();
			return (FLASHIF_ERASE_ERROR);
		}
		else
		{
			HAL_FLASH_Lock();
			return (FLASHIF_ERASE_OK);
		}
	}// end of if(BootMemoryRegion == enmBOOTSEGMENT_Y_JUMP )
	else
	{
		HAL_FLASH_Lock();
		return (FLASHIF_ERASE_ERROR);
	}
}//end of uint32_t  FLASH_If_Erase(uint32_t state, enmBootMemorySegment BootMemoryRegion)


/****************************************************************************
 Function: getNewFirmwareStorageLocation
 Purpose: Get New Firmware download / Storage location

 Input: None

 Return value: enmBootMemorySegment

 Note(s)(if-any) :

 Change History:
 Author            	Date                Remarks
 KloudQ Team        30/03/2020			initial Definitions
******************************************************************************/
enmBootMemorySegment getNewFirmwareStorageLocation(void)
{
	if((BOOTMEMORYINFO == BOOT_MEM_X_JUMP) || (BOOTMEMORYINFO == BOOT_MEM_X))
	{
		/* Current Firmware Location is in X-Memory Region .
		   Store New FW in Y Memory Region */
		return enmBOOTSEGMENT_Y_JUMP;

	}
	else if((BOOTMEMORYINFO == BOOT_MEM_Y_JUMP) || (BOOTMEMORYINFO == BOOT_MEM_Y))
	{
		/* Current Firmware Location is in Y-Memory Region .
		   Store New FW in X Memory Region */
		return enmBOOTSEGMENT_X_JUMP;
	}
	else
	{
		/* Illegal Firmware Informations . Abort FOTA Jumping */
		return enmBOOTSEGMENT_Y_JUMP;
	}
}


/****************************************************************************
 Function: flashWriteBootSection
 Purpose: Updates Boot Memory Info for FOTA / IAP Jumping

 Input: enmBootMemorySegment memorySegment

 Return value: uint8_t 0 -: Memory Write Successful
					   1 -: Error in Memory Write
 Note(s)(if-any) :
 Code Ref : Reference Document reference code
 This function will only use JUMP address as this will be called once fota file
 validation is complete and reboot/JUMP is initiated .

 Usage :

   	if(flashWriteBootSection(enmBOOTSEGMENT_X) == 0)
  	{
		// Flash Write Successful. Add your code
  	}
  	else
  	{
  		 // Flash Boot Memory Write Failed. Add your code
  	}

	Use timeout when this function is called from program / NOT before While(1) /
	Not until timer has been activated .


 Change History:
 Author            	Date                Remarks
 KloudQ Team        26/03/2020			initial Definitions
 Kloudq Team		27/03/2020			Wr/Rd Tested , Timeout added
******************************************************************************/
uint32_t gu32DataFromMemory = 0;
uint32_t flashWriteBootSection(enmBootMemorySegment memorySegment)
{
	uint32_t u32TempDataBuffer = 0;
	switch(memorySegment)
	{
		case enmBOOTSEGMENT_X:
			/* Will not be used during FOTA */
			if(EraseSinglePage(BOOTMEMORYADDRESS)== FLASHIF_ERASE_OK)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, BOOTMEMORYADDRESS, BOOT_MEM_X);
				u32TempDataBuffer = BOOT_MEM_X;
			}
			else
			{
				HAL_FLASH_Lock();
				return 1;/* Memory Write Failed */
			}
			break;
		case enmBOOTSEGMENT_X_JUMP:
			if(EraseSinglePage(BOOTMEMORYADDRESS)== FLASHIF_ERASE_OK)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, BOOTMEMORYADDRESS, BOOT_MEM_X_JUMP);
				u32TempDataBuffer = BOOT_MEM_X_JUMP;
			}
			else
			{
				HAL_FLASH_Lock();
				return 1;/* Memory Write Failed */
			}
			break;
		case enmBOOTSEGMENT_Y:
			/* Will not be used during FOTA */
			if(EraseSinglePage(BOOTMEMORYADDRESS)== FLASHIF_ERASE_OK)
			{
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, BOOTMEMORYADDRESS, BOOT_MEM_Y);
				u32TempDataBuffer = BOOT_MEM_Y;
			}
			else
			{
				HAL_FLASH_Lock();
				return 1;/* Memory Write Failed */
			}
			break;
		case enmBOOTSEGMENT_Y_JUMP:
			if(EraseSinglePage(BOOTMEMORYADDRESS)== FLASHIF_ERASE_OK)
			{
				/* Unlock the Flash to enable the flash control register access *************/
				FLASH_If_Init();
				HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, BOOTMEMORYADDRESS, BOOT_MEM_Y_JUMP);
				u32TempDataBuffer = BOOT_MEM_Y_JUMP;
				HAL_FLASH_Lock();
			}
			else
			{
				HAL_FLASH_Lock();
				return 1;/* Memory Write Failed */
			}
			break;

		default:
			HAL_FLASH_Lock();
			break;
	}

	HAL_FLASH_Lock();
	/* Verify Correctness of the data written */
	gu32DataFromMemory= *((uint32_t *) BOOTMEMORYADDRESS);
	if(gu32DataFromMemory == u32TempDataBuffer)
		return 0; /* Memory Write Successful */
	else
		return 1; /* Memory Write Failed */

}

uint32_t EraseSinglePage(uint32_t EraseAddr)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PAGEError = 0;

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase	 = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks    	 = FLASH_BANK_1;
	EraseInitStruct.Page		 = ((EraseAddr - FLASH_BASE) / FLASH_PAGE_SIZE);
	EraseInitStruct.NbPages		 = 1;

	/* Unlock the Flash to enable the flash control register access *************/
	FLASH_If_Init();

	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
	{
		HAL_FLASH_Lock();
		return (FLASHIF_ERASE_ERROR);
	}
	else
	{
		HAL_FLASH_Lock();
		return (FLASHIF_ERASE_OK);
	}
}

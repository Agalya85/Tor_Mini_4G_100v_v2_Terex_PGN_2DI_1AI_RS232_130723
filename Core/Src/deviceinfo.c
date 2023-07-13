/*
 * deviceinfo.c
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

/*
  *****************************************************************************
  * @file    deviceinfo.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   Functions for fetching device information (UUID and FlashSize)
*******************************************************************************
*/
/*
 Notes : All STM32 microcontrollers feature a 96-bit factory-programmed
 	     unique device ID.

 	     Device electronic signature :
 	     The electronic signature is stored in the System memory area in the
 	     Flash memory module,and can be read using the JTAG/SWD or the CPU.
 	     It contains factory-programmed identification data that allow the
 	     user firmware or other external devices to automatically
		 match its interface to the characteristics

		 Memory size register :
		 Flash size register :
		 Base address: 0x1FFFF7CC
		 Read only = 0xXXXX where X is factory-programmed

		 Bits 15:0 F_SIZE: Flash memory size
		 The value stored in this field indicates the Flash memory size
		 of the device expressed in Kbytes.
		 Example: 0x0040 = 64 Kbytes.

 	     Unique device ID registers (96 bits) :
 	     The unique device identifier is ideally suited:
			• for use as serial numbers
			• for use as security keys in order to increase the security of code in
			  Flash memory while using and combining this unique ID with software
			  cryptographic primitives and protocols before programming the internal Flash
			  memory
			• to activate secure boot processes, etc.

		The 96-bit unique device identifier provides a reference number which is
		unique for any device and in any context.
		These bits can never be altered by the user.

		The 96-bit unique device identifier can also be read in single
		bytes/half-words/words in different ways and then be concatenated
		using a custom algorithm.

		Base address: 0x1FFFF7CC
		Address offset: 0x00
		Read only = 0xXXXX XXXX where X is factory-programmed

		U_ID(23:0): LOT_NUM[55:32]
		Lot number (ASCII code)
		U_ID(31:24): WAF_NUM[7:0]
		Wafer number (8-bit unsigned number)
		U_ID(63:32): LOT_NUM[31:0]
		Lot number (ASCII code)
		U_ID(95:64): 95:64 unique ID bits

 */
/*****************************************************************************/
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "applicationdefines.h"
#include "deviceinfo.h"
#include "externs.h"

char dinfo[50] = {'0'};
char buffuuid2[32] = {'0'};
uint32_t u32FlashSize = 0;

uint32_t gu32DeviceID = 0;

/****************************************************************************
 Function getDeviceSignature
 Purpose: Fetch Device UUID and Flash Size
 Input:	None.
 Return value: None .


 Note(s)(if-any) : Signature ASCII Array is updated

	 UUID -> 96 bit value
	 FlashSize -> 16 bit Value


 Change History:
 Author           	Date                Remarks
 KloudQ Team       11-04-18
 KloudQ Team	   19-09-18				Update . Flash Size removed from payload
******************************************************************************/
void updateDeviceSignature(void)
{

#if(USEMEMSIZEINDINFO == TRUE)
	char buffflash[16] = {'0'};

	u32FlashSize = (uint16_t)STM32_FLASHSIZE;
#endif

	gu32DeviceID = STM32_UUID[0];
	uint32_t u32UUID0 = 0;
	uint32_t u32UUID1 = 0;
	uint32_t u32UUID2 = 0;
	u32UUID0 = STM32_UUID[0];
	u32UUID1 = STM32_UUID[1];
	u32UUID2 = STM32_UUID[2];


#if(USEMEMSIZEINDINFO == TRUE)
	/*Integer to ASCII Flash Size */
	itoa(u32FlashSize,buffflash,10);
#endif
	/* Append Device Info to Array */
	itoa(u32UUID2,buffuuid2,10);
	strcat(dinfo,buffuuid2);

	itoa(u32UUID1,buffuuid2,10);
	strcat(dinfo,buffuuid2);

	itoa(u32UUID0,buffuuid2,10);
	strcat(dinfo,buffuuid2);
#if(USEMEMSIZEINDINFO == TRUE)
	/* 16 bit Flash Size use if required */
	//strcat(dinfo,buffflash);
#endif

	memset(g_u8PaylodLenMsgBuff,0,sizeof(g_u8PaylodLenMsgBuff));
	strcpy(g_u8PaylodLenMsgBuff,(char * )"*,");
	strcat(g_u8PaylodLenMsgBuff,(char * )dinfo);
	strcat(g_u8PaylodLenMsgBuff,",Memory allocation error,#");
}

//******************************* End of File *******************************************************************

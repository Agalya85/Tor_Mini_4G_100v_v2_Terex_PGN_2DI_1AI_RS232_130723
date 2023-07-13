/*
 * deviceinfo.h
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

#ifndef INC_DEVICEINFO_H_
#define INC_DEVICEINFO_H_

#define STM32_UUID ((uint32_t *) 0x1FFF7590)						/* 96 bit Unique Identification ID */
#define STM32_FLASHSIZE (*((uint32_t *)  0x1FFF75E0))				/* 16 bit Flash Size */

#define USEMEMSIZEINDINFO (0)
typedef struct
{
	uint32_t u32UUID0;
	uint32_t u32UUID1;
	uint32_t u32UUID2;
	uint32_t u32FlashSize;
}strSTMDeviceSignature;

extern uint32_t u32FlashSize;
void updateDeviceSignature(void);

#endif /* INC_DEVICEINFO_H_ */

/*
 * user_MqttSubSperator.h
 *
 *  Created on: Jun 8, 2023
 *      Author: admin
 */

#ifndef INC_USER_MQTTSUBSPERATOR_H_
#define INC_USER_MQTTSUBSPERATOR_H_

#include "main.h"
#include "externs.h"
#include "payload.h"
#include "stdlib.h"
#include "string.h"
#include "error_handler.h"
#include "user_Immobilization.h"
typedef enum
{
	enmSUBSTRINGINDETIFY_TWOWAY = 2,
	enmSUBSTRINGINDETIFY_DO = 3,
	enmSUBSTRINGINDETIFY_CAN_Update = 4,
	enmSUBSTRINGINDETIFY_CAN_Get_Data = 5,
	enmSUBSTRINGINDETIFY_MOD = 6,
	enmSUBSTRINGINDETIFY_COMMAND = 7,
	enmSUBSTRINGINDETIFY_FOTA = 8,
	enmSUBSTRINGINDETIFY_MODULE = 9,
	enmSUBSTRINGINDETIFY_OTA = 10,
	enmSUBSTRINGINDETIFY_APN = 11,
	enmSUBSTRINGINDETIFY_MQTT_Config = 12,
	enmSUBSTRINGINDETIFY_Payload = 13,
	enmSUBSTRINGINDETIFY_MultiSlave = 14,
	enmSUBSTRINGINDETIFY_CanPayload = 15,
	enmSUBSTRINGINDETIFY_Config = 16,
//	enmSUBSTRINGINDETIFY_OTA = 500,
	enmSUBSTRINGINDETIFY_RESTART = 101,
	enmSUBSTRINGINDETIFY_Send_Diagnostic_Data = 102,

}enmSUBSTRINGINDETIFY;

void SubRevicedString(void);
#endif /* INC_USER_MQTTSUBSPERATOR_H_ */

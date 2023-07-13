/*
 * payload.h
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

#ifndef INC_PAYLOAD_H_
#define INC_PAYLOAD_H_

#include "user_error.h"
#include "queue.h"

#define START_OF_FRAME			"*"									/* Payload SOF Character */
#define END_OF_FRAME			"#"									/* Payload EOF Character */
#define FIRMWARE_VER			"1.0"					 		/* Firmware Version */
#define BOOT_REGION				"Xv"

#define PAYLOAD (1)
#define GEO (2)

/* Payload Funtion Prototypes */
void updateSystemParameters(void);
char * getMachineDataString(void);
char * getdynamicCanString(strctQUEUE * canqueue);
void updateCanPayload(uint32_t data ,char * systemPayload );
void PaylodTime(char * systemPayload);
void GPS_SperateString(void);
char * getGeoDataString(void);
void Diagnostic(enmDiagnosticStatus DiagnosticStatus );
char * getDiagDataString(enmDiagnosticStatus DiagnosticStatus);
void DiagnosticString(char * systemPayload, enmDiagnosticStatus DiagnosticStatus);
void CANIDPrase(char * systemPayload);
void manufacturerDate(char *systemPayload );
void updateInputVoltage(float Volt);

#endif /* INC_PAYLOAD_H_ */

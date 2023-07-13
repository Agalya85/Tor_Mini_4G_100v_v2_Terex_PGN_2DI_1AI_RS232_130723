/*
 * user_Immobilization.h
 *
 *  Created on: Jun 15, 2023
 *      Author: admin
 */

#ifndef INC_USER_IMMOBILIZATION_H_
#define INC_USER_IMMOBILIZATION_H_

#include "main.h"
#include "externs.h"
#include "payload.h"
#include "stdlib.h"
#include "string.h"
#include "error_handler.h"
#include "UserEeprom.h"


#define EEPROM_IMMOBI_MEM_ADD_DATA 					EEPROM_PAGE4_ADD //(uint16_t)0x30

void CheckImmobilization(void);
void WriteImobilizedStateToEEPROM(uint8_t value);
void ReadImmobilizedStateFromEEPROM(void);

#endif /* INC_USER_IMMOBILIZATION_H_ */

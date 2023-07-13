/*
 * user_error.h
 *
 *  Created on: Nov 29, 2022
 *      Author: admin
 */

#ifndef INC_USER_ERROR_H_
#define INC_USER_ERROR_H_

typedef enum
{
	enmDiagnostic_SUCCESS_OK = 0, // FOTA SUCCESS
	enmDiagnostic_CAN_ID,
	enmDiagnostic_DEVICE_POWER_ON,
	enmDiagnostic_HW_ID_MISMATCH_ERROR, // Device HW ID Mismatch
	enmDiagnostic_FOTA_REQ_RX,
	enmDiagnostic_POWER_SUPPLY_ERROR,
	enmDiagnostic_RX_FILE_SIZE_ERROR, //Received File Size is Greater than (FLASH_SIZE-4k)/2
	enmDiagnostic_FILE_DONWLOAD_ERROR,
	enmDiagnostic_FLASH_ERRASE_ERROR,
	enmDiagnostic_FLASH_Write_ERROR,
	enmDiagnostic_BOOT_REGION_SELECTION_ERROR,
	enmDiagnostic_MEM_WR_CHK_SUM_ERROR, //MEM sector written, but downloaded DATA and Written data in MEM Mismatch
	enmDiagnostic_BOOT_REGION_JUMP_ERROR,
	enmDiagnostic_IMOBI_REQ_RX,
	enmDiagnostic_IMOBI_SUCCESS_OK,
	enmDiagnostic_IDLE = 99,

}enmDiagnosticStatus;


#endif /* INC_USER_ERROR_H_ */

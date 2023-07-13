/*
 * extern.h
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

#ifndef INC_EXTERNS_H_
#define INC_EXTERNS_H_

#include "queue.h"
#include "gsmEC200U.h"
#include "user_rtc.h"
#include "applicationdefines.h"
#include "user_adc.h"
#include "stdbool.h"
#include "user_can.h"
#include "user_RS232.h"
#include "string.h"
#include "UserEeprom.h"

#define DEBUGENABLE (FALSE)
#if(DEBUGENABLE == TRUE)
	extern char DebugBuff[3000];
#endif

extern stEepromBuffMem g_stEepromBuffMemAdd;
extern IWDG_HandleTypeDef hiwdg;

#define NW_2G 		0x02
#define NW_4G 		0x04
#define NO_SERVICE 	0x00

extern RTC_TimeTypeDef STime1;
extern RTC_DateTypeDef SDate1;
extern RTC_HandleTypeDef hrtc;
extern I2C_HandleTypeDef hi2c1;

extern RTC_DateTypeDef lastKnownSDate1;

extern uint32_t              TxMailbox;
extern uint8_t               TxData[8];

extern strctGSM gsmInstance;
extern uint8_t g_u8HttpConEstablishedFlag;

extern strctQUEUE gsmLivePayload;
extern strctBuffQUEUE gsmBuffPayload;

extern strTimeElapsedAfterSync strTimeUpdate;

extern volatile uint32_t gu32GSMHangTimer;
extern volatile uint32_t u8GSMCharRcv;
extern char dinfo[50];

extern uint8_t gu8OperateSystemStatus;
extern volatile uint32_t gu32rtcBackupTimer;
extern volatile uint32_t gu32GSMCharacterTimeout;
extern volatile uint32_t gu32FotaFileReadTimer;
extern uint32_t  gu32TimeSyncFlag;
extern volatile uint32_t gu32I2CMemoryOperationTimeout;

extern char gau8Year[5];
extern uint8_t gu8YY;
extern char gau8Month[3];
extern char gau8Date[3];
extern char gau8Hour[3];
extern char gau8Minutes[3];
extern char gau8Seconds[3];

extern uint32_t urlLength;

extern char gu8NewURL[150];
extern char gau8GSM_url[150];
extern char gau8GSM_smsto[15];

extern uint8_t gu32MemoryOperation;
extern volatile uint8_t gu8MemoryCycleComplete;
extern uint8_t gu8RestoreSystemSettings;
extern volatile uint32_t gu32PayloadQueueEnqueue;
extern volatile uint32_t gu32GEOQueueEnqueue;
extern volatile uint32_t gu8rtcUpdateTimer;
extern volatile uint32_t gu32TimeSyncVariable;
extern volatile uint32_t gu32LEDDelay;
extern uint8_t gu8SignatureReadFlag;
extern volatile uint32_t u32DefautParamWriteStatus;

extern uint8_t GPSDataValid;
extern uint8_t SystemOnStatus;
extern uint8_t gu8ModuleInitFlag;
extern uint8_t gu8ModuleInitNotifyCnt;
extern volatile uint32_t gu32ModuleInitTimer;

extern uint32_t u32FotaChunckLength;
extern char gau8SUBRequest[150];
extern uint8_t SUBTriggerFlag;
extern uint32_t gu32FotaRquestFlag;
extern _Bool boolFOTACompleteFlag;
extern char gau8FotaURL[100];

extern volatile uint32_t  gu32HeartBeatDelay;
extern volatile uint32_t gu32ADCRestartTimer ;


extern char g_u8PaylodLenMsgBuff[60] ;

extern volatile uint32_t u8GPSCharRcv;

extern uint8_t Payload_GEO_Toggle_Flag;
extern char gu32FotaFileSizeinBytes[8];

extern _Bool GSMInitCompleteFlag;

extern uint8_t u8daignostic;
extern uint32_t adcData[6];
extern StructAdc g_stAdcData;

extern uint8_t signal_qaulity;
extern char gau8GSM_NWINFO[15];
extern uint8_t hexadecimalnum[64];

extern char GPSUTCTIME[8];
extern uint64_t cu32IMEI;

extern char gau8GSM_IMEI[18];

extern _Bool boolInginitionStatus1;
extern _Bool boolInginitionStatus2;

/*CAN */
extern uint32_t gu32CanConfigurationArray[(MAX_CAN_IDS_SUPPORTED + 1)];
extern uint64_t gu64CanMessageArray[MAX_CAN_IDS_SUPPORTED];
extern uint32_t gu32CanConfigurationArrayPGN[(MAX_CAN_IDS_SUPPORTED + 1)];
extern uint32_t gu64CanMessageTimeStampArray[MAX_CAN_IDS_SUPPORTED];
extern uint8_t CAN_ID_95[70];
extern uint8_t CAN_ID_96[70];
extern uint8_t TelematicHeartBeat[8];
extern volatile uint32_t gu32CANQueryPollTimer;
extern volatile uint32_t gu32CANCommandResponseTimeout;
extern CAN_HandleTypeDef hcan1;
extern CAN_FilterTypeDef  sFilterConfig;
extern uint64_t gu64CanMessageIDPGN[MAX_CAN_IDS_SUPPORTED];


extern uint32_t u32FOTAFileBaseAddress;
extern uint32_t u32FotaFileChunkCounter;

extern uint8_t Current_Immobilization_State;
extern uint8_t Previous_Immobilization_State;

/*RS232*/
//extern uint8_t RS232Buffer[1000];
extern char buff[RS232_RESPONSE_ARRAY_SIZE];
extern volatile uint32_t gu32RS232CharacterTimeout;
//extern RS232_RESPONSE_ARRAY_SIZE ;
#include "serial_flash_w25_fsm.h"
#endif /* INC_EXTERNS_H_ */

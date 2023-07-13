/*
 * user_rtc.h
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

#ifndef INC_USER_RTC_H_
#define INC_USER_RTC_H_

typedef struct
{

	uint32_t u32RefTimeHH;
	uint32_t u32RefTimeMin;
	uint32_t u32RefTimeSec;
	uint32_t u32RefTimeMilliSec;

	uint32_t u32TimeHH;
	uint32_t u32TimeMin;
	uint32_t u32TimeSec;
	uint32_t u32TimeMilliSec;

	uint32_t u32ActTimeHH;
	uint32_t u32ActTimeMin;
	uint32_t u32ActTimeSec;
	uint32_t u32ActTimeMilliSec;

}strTimeElapsedAfterSync;

void getrtcStamp(void);
void rtcSetTime(void);
uint32_t DecimalToBCD (uint32_t Decimal);
uint32_t BCDToDecimal(uint32_t BCD);
uint32_t rtcreadbackupreg(uint32_t BackupRegister);
void rtcwritebackupreg(uint32_t BackupRegister, uint32_t data);
void updateElapsedTime(uint32_t millisecond);
void backupCurrentRTC(void);
void readrtcbackupdata(void);

#endif /* INC_USER_RTC_H_ */

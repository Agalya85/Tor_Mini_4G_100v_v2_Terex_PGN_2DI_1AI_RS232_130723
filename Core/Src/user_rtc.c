/*
 * user_rtc.c
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */

/*
  *****************************************************************************
  * @file    internalrtc.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   Functions for Accessing Internal RTC
*******************************************************************************
*/
/******************************************************************************

            Copyright (c) by KloudQ Technologies Limited.

  This software is copyrighted by and is the sole property of KloudQ
  Technologies Limited.
  All rights, title, ownership, or other interests in the software remain the
  property of  KloudQ Technologies Limited. This software may only be used in
  accordance with the corresponding license agreement. Any unauthorized use,
  duplication, transmission, distribution, or disclosure of this software is
  expressly forbidden.

  This Copyright notice may not be removed or modified without prior written
  consent of KloudQ Technologies Limited.

  KloudQ Technologies Limited reserves the right to modify this software
  without notice.

*/
#include"stm32l4xx_hal.h"
#include "stm32l4xx_hal_rtc.h"
#include "rtc.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "user_rtc.h"
#include "applicationdefines.h"
#include "system_reset.h"

 RTC_TimeTypeDef STime1;
 RTC_DateTypeDef SDate1;


char gau8Year[5] = {'0'};
char gau8Month[3] = {'0'};
char gau8Date[3] = {'0'};
char gau8Hour[3] = {'0'};
char gau8Minutes[3] = {'0'};
char gau8Seconds[3] = {'0'};
char CorrectRTC = 0;
strTimeElapsedAfterSync strTimeUpdate = {0,0,0,0,0,0,0,0,0,0,0,0};

volatile uint32_t gu32TimeSyncVariable = 0;
uint32_t  gu32TimeSyncFlag = 0;
/****************************************************************************
 Function getrtcStamp
 Purpose: Get Time and Date
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      11-04-18
******************************************************************************/
void getrtcStamp(void)
{
	HAL_RTC_GetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);

	itoa(BCDToDecimal(SDate1.Year),gau8Year,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(SDate1.Month),gau8Month,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(SDate1.Date),gau8Date,PAYLOAD_DATA_STRING_RADIX);

	itoa(BCDToDecimal(STime1.Hours),gau8Hour,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(STime1.Minutes),gau8Minutes,PAYLOAD_DATA_STRING_RADIX);
	itoa(BCDToDecimal(STime1.Seconds),gau8Seconds,PAYLOAD_DATA_STRING_RADIX);
}

/****************************************************************************
 Function DecimalToBCD
 Purpose: Convert Decimal to BCD
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      13-10-2020			100133
******************************************************************************/
uint32_t DecimalToBCD (uint32_t Decimal)
{
   return (((Decimal/10) << 4) | (Decimal % 10));
}

/****************************************************************************
 Function BCDToDecimal
 Purpose: Convert BCD to Decimal
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      13-10-2020			100133
******************************************************************************/
uint32_t BCDToDecimal(uint32_t BCD)
{
   return (((BCD >> 4) * 10) + (BCD & 0xF));
}

/****************************************************************************
 Function updateElapsedTime
 Purpose: Calculates elapsed time after rtc sync (network)
 Input:	None.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      13-10-2020			100133
******************************************************************************/
void updateElapsedTime(uint32_t millisecond)
{
	strTimeUpdate.u32TimeHH = (millisecond / (1000 * 60 * 60)) % 24;
	strTimeUpdate.u32TimeMin = (millisecond / (1000 * 60 )) % 60;
	strTimeUpdate.u32TimeSec = (millisecond / 1000) % 60;
	strTimeUpdate.u32TimeMilliSec = millisecond % 1000;

	strTimeUpdate.u32ActTimeHH = strTimeUpdate.u32RefTimeHH + strTimeUpdate.u32TimeHH;
	strTimeUpdate.u32ActTimeMin = strTimeUpdate.u32RefTimeMin + strTimeUpdate.u32TimeMin;
	strTimeUpdate.u32ActTimeSec = strTimeUpdate.u32RefTimeSec + strTimeUpdate.u32TimeSec;
	strTimeUpdate.u32ActTimeMilliSec = strTimeUpdate.u32RefTimeMilliSec + strTimeUpdate.u32TimeMilliSec;

	if(strTimeUpdate.u32ActTimeSec > 59)
	{
		strTimeUpdate.u32ActTimeMin++;
		strTimeUpdate.u32ActTimeSec = strTimeUpdate.u32ActTimeSec - 60;
		strTimeUpdate.u32TimeSec = 0;
	}

	if(strTimeUpdate.u32ActTimeMin > 59)
	{
		strTimeUpdate.u32ActTimeHH++;
		strTimeUpdate.u32ActTimeMin = strTimeUpdate.u32ActTimeMin - 60;
		 strTimeUpdate.u32TimeMin = 0;
	}

	if(strTimeUpdate.u32ActTimeHH > 23)
	{
		strTimeUpdate.u32ActTimeHH = strTimeUpdate.u32ActTimeHH - 24;
		/* Increment date / month and year also */
		// take system reset so that device syncs the date
		systemReset();
	}

	HAL_RTC_GetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);

	if(BCDToDecimal(STime1.Hours) != strTimeUpdate.u32ActTimeHH)
		STime1.Hours = DecimalToBCD(strTimeUpdate.u32ActTimeHH);

	if(BCDToDecimal(STime1.Minutes) != strTimeUpdate.u32ActTimeMin)
			STime1.Minutes = DecimalToBCD(strTimeUpdate.u32ActTimeMin);

	if(BCDToDecimal(STime1.Seconds) != strTimeUpdate.u32ActTimeSec)
		STime1.Seconds = DecimalToBCD(strTimeUpdate.u32ActTimeSec);

	HAL_RTC_SetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);
	//backupCurrentRTC();

}

/****************************************************************************
 Function rtcreadbackupreg
 Purpose: Read RTC backup register
 Input:	uint32_t BackupRegister.
 Return value: None.


 Note(s)(if-any) :
 RTC features 20 internal backup registers where you can store anything
 and will be available whole time RTC is active and has power.

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021				100133
******************************************************************************/
uint32_t rtcreadbackupreg(uint32_t BackupRegister)
{
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    return HAL_RTCEx_BKUPRead(&RtcHandle, BackupRegister);
}

/****************************************************************************
 Function rtcwritebackupreg
 Purpose: Write to rtc backup reg
 Input:	uint32_t BackupRegister, uint32_t data.
 Return value: None.


 Note(s)(if-any) :

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021			100133
******************************************************************************/
void rtcwritebackupreg(uint32_t BackupRegister, uint32_t data)
{
    RTC_HandleTypeDef RtcHandle;
    RtcHandle.Instance = RTC;
    HAL_PWR_EnableBkUpAccess();
    HAL_RTCEx_BKUPWrite(&RtcHandle, BackupRegister, data);
    HAL_PWR_DisableBkUpAccess();
}

/****************************************************************************
 Function backupCurrentRTC
 Purpose: Write to rtc backup reg
 Input:	None
 Return value: None.


 Note(s)(if-any) :
 Stores rtc data in backup registers

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021			100133
******************************************************************************/
void backupCurrentRTC(void)
{
	uint32_t hrs = BCDToDecimal(STime1.Hours);
	uint32_t min = BCDToDecimal(STime1.Minutes);
	uint32_t sec = BCDToDecimal(STime1.Seconds);

	uint32_t date = BCDToDecimal(SDate1.Date);
	uint32_t month = BCDToDecimal(SDate1.Month);
	uint32_t yr = BCDToDecimal(SDate1.Year);
	/* Update Backup Registers */
	HAL_PWR_EnableBkUpAccess();
	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR0 ,hrs);
	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR1 ,min);
	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR2 ,sec);
	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR3 ,date);
	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR4 ,month);
	HAL_RTCEx_BKUPWrite(&hrtc , RTC_BKP_DR5 ,yr);
	HAL_PWR_DisableBkUpAccess();
}

/****************************************************************************
 Function backupCurrentRTC
 Purpose: Write to rtc backup reg
 Input:	None
 Return value: None.


 Note(s)(if-any) :
 Stores rtc data in backup registers

 Change History:
 Author           	Date                Remarks
 KloudQ Team      28-4-2021			100133
******************************************************************************/
void readrtcbackupdata(void)
{
	uint32_t hrs = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0));
	uint32_t min = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1));
	uint32_t sec = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR2));

	uint32_t date = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR3));
	uint32_t month = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR4));
	uint32_t yr = DecimalToBCD(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR5));

	STime1.Hours = hrs;
	STime1.Minutes =  min;
	STime1.Seconds = sec;

	SDate1.Date =  date;
	SDate1.Month = month;
	SDate1.Year =  yr;

	/*Update RTC with fetched data */
	HAL_RTC_SetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);
}

//******************************* End of File *******************************************************************


/*
 * payload.c
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
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
  Note  : : 59.163.219.179:8014/
  http://10.3.8.62:8014/api/Values/PostStringData
String Sequence should be
1.	Start of frame
2.	Hardware ID
3.	Model No.
4.	UTC date time
5.	Firmware Version
*/

#include "main.h"
#include "stm32l4xx_hal.h"
#include "payload.h"
#include "externs.h"
#include "user_can.h"
#include "user_error.h"
#include "timer.h"

/* USER CODE BEGIN Includes */
#include "applicationdefines.h"
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

char * gpu8NewGPSString = ",0,0,0,0,0,0,0,0,0,0,0,0,0"; //13
char * gpu8NoString = "0,0,0,0,0";//,0,0,0,0,0,0,0,0,0,0,0"; //15
char * gpu8Norssi = "0,0";
char * gpu8Nohrs = "0,0";

char GPSUTCDATE[8] = "";
char GPSUTCTIME[8] = "";
char u32GPSUTCTime[17];
char GPS_LAT[10]="";
char GPS_LONG[10]="";
char GPS_SPEED_OVR_GND[6]="";
char voltage[6] = "0.00";

char * gau8CanPayload[MAX_CAN_IDS_SUPPORTED + 1];

uint8_t Payload_GEO_Toggle_Flag = TRUE;
_Bool FOTACompleteFlag = FALSE;


/******************************************************************************
 Function Name: getMachineDataString
 Purpose: Generate System Payload
 Input:	None
 Return value: (char *)
	Note(s)(if any)
 	*,TorMini,Intg 1.0,20/5/28 12:23:69,05406193121196442369524317,
 	*: 1,0,19800106000027.000,,,,0.00,0.0,0,,,,,,0,0,,,,,,0,0,0,0,
 	*4030201,0,0,1040404,2020202,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 	*0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,,21,0,#
 Change History:
 Author           	Date                Remarks
 KloudQ Team       07-02-19
 Kloudq Team	   30-03-2020			Malloc Error countered.(CANPayloadErr)
 Kloudq Team	   28-05-2020			Added long long int to string (CAN)
 Kloudq Team	   28-10-2020			Time stamp sequence made DD/MM/YY
******************************************************************************/
uint32_t gu32CanIdsLoopCounter = 1;
//uint32_t gu32CanPayloadLoopCounter = 0;
//uint32_t gu32StringLength = 0;
char * getMachineDataString(void)
{
	//updateSystemParameters();
	  char *systemPayload = (char *)malloc(sizeof(char) * GSM_PAYLOAD_MAX_SIZE);// GSM_PAYLOAD_MAX_SIZE = 1500

	  char temp[8] = "";
//	uint16_t len = strlen(systemPayload);
//	uint16_t size = sizeof(systemPayload);

	if(systemPayload != NULL)
	{
		/*
			1.	Start of frame
			2.	Hardware ID
			3.	Model No.
			4.	UTC date time
			5.	Firmware Version
		*/
		memset(systemPayload,0x00,sizeof(char) * GSM_PAYLOAD_MAX_SIZE);
		/* Start of Frame */
		strcpy((char *)systemPayload,(char * )START_OF_FRAME);
		strcat((char *)systemPayload,(char * )",");

		/* Device UUID */
		strcat((char *)systemPayload,(char * )dinfo);
		strcat((char *)systemPayload,(char * )",");

		/* Model Number */
		strcat((char *)systemPayload,(char * )MODEL_NUMBER);
		strcat((char *)systemPayload,(char * )",");
		manufacturerDate(systemPayload);
		strcat((char *)systemPayload,"_Payload");
		strcat((char *)systemPayload,(char * )",");

		/*System Time Stamp */
		PaylodTime(systemPayload);


		/* Firmware Version*/
		strcat((char *)systemPayload,(char * )BOOT_REGION);
		strcat((char *)systemPayload,(char * )FIRMWARE_VER);
		strcat((char *)systemPayload,(char * )","); // as reply for GSP start with ','

		/* GPS Data */
		if(strlen(gsmInstance.agpsLocationData) > 0)
		{
			memset(gsmInstance.agpsTempData,0x00, strlen(gsmInstance.agpsTempData));
			GPS_SperateString();
//			strcat((char *)systemPayload,(char * )gsmInstance.agpsTempData);
			strcat((char *)systemPayload,(char * )GPSUTCTIME);
			strcat((char *)systemPayload,",");
			strcat((char *)systemPayload,(char * )GPS_LAT);
			strcat((char *)systemPayload,",");
			strcat((char *)systemPayload,(char * )GPS_LONG);
			strcat((char *)systemPayload,",");
			strcat((char *)systemPayload,(char * )GPSUTCDATE);
			strcat((char *)systemPayload,",");
			strcat((char *)systemPayload,(char * )GPS_SPEED_OVR_GND);
		}
		else
			strcat((char *)systemPayload,gpu8NoString);


		strcat((char *)systemPayload,",");
		itoa(u8daignostic,temp,PAYLOAD_DATA_STRING_RADIX);
		strcat((char *)systemPayload,temp);
		if(u8daignostic & (1<<7))
		{
			u8daignostic &= ~(1<<7);
		}
		if(u8daignostic & (1<<6))
		{
			u8daignostic &= ~(1<<6);
		}
		strcat((char *)systemPayload,",");

		updateInputVoltage(g_stAdcData.u32PwrSupplyVtg);
		strcat((char *)systemPayload,(char * )voltage);
		strcat((char *)systemPayload,",");

		updateInputVoltage(g_stAdcData.u32IntBatVtg);
		strcat((char *)systemPayload,(char * )voltage);
		strcat((char *)systemPayload,(char * )",");

		itoa(boolInginitionStatus1,temp,PAYLOAD_DATA_STRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");

		itoa(boolInginitionStatus2,temp,PAYLOAD_DATA_STRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");

		itoa(g_stAdcData.u32AI1RawData,temp,PAYLOAD_DATA_STRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");


		/* CAN Data */
		updateCanPayload(1, systemPayload );
		updateCanPayload(2, systemPayload );
		updateCanPayload(3, systemPayload );
		updateCanPayload(4, systemPayload );
		updateCanPayload(5, systemPayload );

		updateCanPayload(6, systemPayload );
		updateCanPayload(7, systemPayload );

        /*RS232*/
		strcat((char *)systemPayload,"RS232");
		strcat((char *)systemPayload,",");
		strcat((char *)systemPayload,(char * )buff);
		strcat((char *)systemPayload,",");

		/* RSSI */
		if(strlen(gsmInstance.agsmSignalStrength ) > 0)
		{
			strcat((char *)systemPayload,(char * )gsmInstance.agsmSignalStrength);
//			strcat((char *)systemPayload,(char * )",");
		}
		else
			strcat((char *)systemPayload,gpu8Norssi);

		strcat((char *)systemPayload,(char * )",");

		/* End of Frame */
		strcat((char *)systemPayload,(char * )END_OF_FRAME);
		gu32CanIdsLoopCounter = 1;

//		Payload_GEO_Toggle_Flag = GEO;
		return systemPayload;
	}
	else
		return NULL; // malloc Error . Memory Allocation Failure
}//end of char * getMachineDataString(void)

//char * getdynamicCanString(strctQUEUE * canqueue)
//{
//	//updateSystemParameters();
//	  char *canPayload = (char *)malloc(sizeof(char) * GSM_PAYLOAD_MAX_SIZE);// GSM_PAYLOAD_MAX_SIZE = 1500
////	  uint8_t loopcounter = 0;
////	  static uint8_t queueCounterLoop = 0;
//
//	if(canPayload != NULL)
//	{
//		/*
//			1.	Start of frame
//			2.	Hardware ID
//			3.	Model No.
//			4.	UTC date time
//			5.	Firmware Version
//			6. Dynamic can data
//		*/
//		memset(canPayload,0x00,sizeof(char) * 1200);
//		/* Start of Frame */
//		strcpy((char *)canPayload,(char * )START_OF_FRAME);
//		strcat((char *)canPayload,(char * )",");
//
//		/* Device UUID */
//		strcat((char *)canPayload,(char * )dinfo);
//		strcat((char *)canPayload,(char * )",");
//
//		/* Model Number */
//		strcat((char *)canPayload,(char * )MODEL_NUMBER);
//		strcat((char *)canPayload,"_CanPayload");
//		strcat((char *)canPayload,(char * )",");
//
//		/*System Time Stamp */
//		PaylodTime(canPayload);
//		/*Can payload */
//
//		for(canqueue->head = 0; canqueue->head <= 9 ;canqueue->head++)
//		{
//			if(canqueue->data[canqueue->head] != NULL)
//			{
//				strcat((char *)canPayload,(char * )canqueue->data[canqueue->head]);
//				strcat((char *)canPayload,(char * )",");
//				/* ToDo : Dequeue data*/
//			}
//
//		}
//		strcat((char *)canPayload,(char * )END_OF_FRAME);
//
//		return canPayload;
//	}
//	else
//		return NULL; // malloc Error . Memory Allocation Failure
//}//end of char * getMachineDataString(void)

/******************************************************************************
 Function Name: getKWPDataString
 Purpose: Generate KWP Payload
 Input:	None
 Return value: (char *)
	Note(s)(if any)
 	*,TorMini,Intg 1.0,20/5/28 12:23:69,05406193121196442369524317,
 	*: 1,0,19800106000027.000,,,,0.00,0.0,0,,,,,,0,0,,,,,,0,0,0,0,
 	*4030201,0,0,1040404,2020202,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
 	*0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,,21,0,#
 Change History:
 Author           	Date                Remarks
******************************************************************************/


void updateCanPayload(uint32_t data ,char * systemPayload )
{
//	char temp[20] = "";
//	uint8_t CANTempData = 0;
//
//	if(gu32CanConfigurationArrayPGN[data]!=0)
//	{
//		strcat((char *)systemPayload,"^,0x");
//		itoa(gu32CanConfigurationArrayPGN[data],temp,CAN_PAYLOADSTRING_RADIX);
//		strcat((char *)systemPayload,temp);
//		strcat((char *)systemPayload,",");
//	}
//	else
//	{
//		strcat((char *)systemPayload,"^,0x00000000,");
//	}
//
//
//	if(gu64CanMessageArray[data] != 0 )
//	{
////		sprintf(temp,"%ld",gu64CanMessageTimeStampArray[data]);
////		strcat((char *)systemPayload,temp);
////		strcat((char *)systemPayload,",");
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 56) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 48) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 40) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 32) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 24) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 16) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data] >> 8) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//
//		CANTempData = (((uint64_t)gu64CanMessageArray[data]) & 255);
//		sprintf(temp,"%02X",CANTempData);
//		CANTempData = 0;
//		strcat((char *)systemPayload,temp);
//		strcat((char *)systemPayload,",");
//
//		/*Clear Payload Array*/
//		/* Removed after discussion with team that we should keep previous data in case of CAN communication not available */
//		gu64CanMessageArray[data] = 0;
//	}
//	else if(gu64CanMessageArray[data] == 0 )
//	{
//		/* Added constant string in place of multiple strcat - 18/02/2021 Milind Vaze*/
////		strcat((char *)systemPayload,"00,00,00,00,00,00,00,00,");
//		strcat((char *)systemPayload,"0000000000000000,");
//
//	}
	char temp[10] = "";
	strcat((char *)systemPayload,"^,0x");
//	strcat((char *)systemPayload,"");
	itoa(gu64CanMessageIDPGN[data],temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa((((uint64_t)gu64CanMessageArray[data] >> 56) & 255),temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa((((uint64_t)gu64CanMessageArray[data] >> 48) & 255),temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa((((uint64_t)gu64CanMessageArray[data] >> 40) & 255),temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa((((uint64_t)gu64CanMessageArray[data] >> 32) & 255),temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa((((uint64_t)gu64CanMessageArray[data] >> 24) & 255),temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa((((uint64_t)gu64CanMessageArray[data] >> 16) & 255),temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa(((uint64_t)(gu64CanMessageArray[data] >> 8) & 255),temp,CAN_PAYLOADSTRING_RADIX);
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");
	itoa(((uint64_t)(gu64CanMessageArray[data]) & 255),temp,CAN_PAYLOADSTRING_RADIX); //255
	strcat((char *)systemPayload,temp);
	strcat((char *)systemPayload,",");

	/*Clear Payload Array*/
	/* Removed after discussion with team that we should keep previous data in case of CAN communication not available */
	gu64CanMessageArray[data] = 0;

}//end of void updateCanPayload(uint32_t data ,char * systemPayload )


//void update95_96CanPayload(uint8_t data[] ,char * systemPayload,uint8_t CANId)
//{
//	char temp[20] = "";
////	uint8_t CANTempData = 0;
//	uint8_t i;
//
//
//	for(i =0 ;i<64;i++)
//	{
//		if(i%8 == 0)
//		{
//			strcat((char *)systemPayload,"^,0x");
//			if(CANId == 95)
//			{
//				itoa(gu32CanConfigurationArray[6],temp,CAN_PAYLOADSTRING_RADIX);
//			}
//			else if(CANId == 96)
//			{
//				itoa(gu32CanConfigurationArray[7],temp,CAN_PAYLOADSTRING_RADIX);
//			}
//
//			strcat((char *)systemPayload,temp);
//			strcat((char *)systemPayload,",");
//		}
//			sprintf(temp,"%02X",data[i++]);
//			strcat((char *)systemPayload,temp);
//			sprintf(temp,"%02X",data[i++]);
//			strcat((char *)systemPayload,temp);
//			sprintf(temp,"%02X",data[i++]);
//			strcat((char *)systemPayload,temp);
//			sprintf(temp,"%02X",data[i++]);
//			strcat((char *)systemPayload,temp);
//			sprintf(temp,"%02X",data[i++]);
//			strcat((char *)systemPayload,temp);
//			sprintf(temp,"%02X",data[i++]);
//			strcat((char *)systemPayload,temp);
//			sprintf(temp,"%02X",data[i++]);
//			strcat((char *)systemPayload,temp);
//			sprintf(temp,"%02X",data[i]);
//			strcat((char *)systemPayload,temp);
//			strcat((char *)systemPayload,",");
//
//	}
//}

void PaylodTime(char * systemPayload)
{
	getrtcStamp();

	/*System Time Stamp */
	strcat((char *)systemPayload,(char * )gau8Date);
	strcat((char *)systemPayload,(char * )"/");
	strcat((char *)systemPayload,(char * )gau8Month);
	strcat((char *)systemPayload,(char * )"/");
	strcat((char *)systemPayload,(char * )gau8Year);
	strcat((char *)systemPayload,(char * )" ");
	strcat((char *)systemPayload,(char * )gau8Hour);
	strcat((char *)systemPayload,(char * )":");
	strcat((char *)systemPayload,(char * )gau8Minutes);
	strcat((char *)systemPayload,(char * )":");
	strcat((char *)systemPayload,(char * )gau8Seconds);
	strcat((char *)systemPayload,(char * )",");
}//end of void PaylodTime(char * systemPayload)

void GPS_SperateString()
{
	uint32_t LoopCounter = 0;
	uint32_t CopyCounter = 0;
	uint8_t genric_Cnt = 0;


	/* Skip ,*/
	for(LoopCounter = 0; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		// Nothing to do
	}
	gsmInstance.agpsTempData[CopyCounter++] = gsmInstance.agpsLocationData[LoopCounter];//,
	LoopCounter++;
	if(gsmInstance.agpsLocationData[LoopCounter] != ',')
	{

		for(; gsmInstance.agpsLocationData[LoopCounter] != '.'; LoopCounter++)
		{
		// Get date time
			gsmInstance.agpsTempData[CopyCounter] = gsmInstance.agpsLocationData[LoopCounter];
			GPSUTCTIME[genric_Cnt]=gsmInstance.agpsLocationData[LoopCounter];
			genric_Cnt++;
			CopyCounter++;
		}
//		u32GPSUTCTime = 0;
//		u32GPSUTCTime = atoi(GPSUTCTIME);
//		sprintf(u32GPSUTCTime[0],"%02X", GPSUTCTIME[0]);
//		sprintf(u32GPSUTCTime[1],"%02X", GPSUTCTIME[1]);
//		sprintf(u32GPSUTCTime[2],"%02X", GPSUTCTIME[2]);
//		sprintf(u32GPSUTCTime[3],"%02X", GPSUTCTIME[3]);
//		sprintf(u32GPSUTCTime[4],"%02X", GPSUTCTIME[4]);
//		sprintf(u32GPSUTCTime[5],"%02X", GPSUTCTIME[5]);
	/* Skip ms and 2nd ,*/
		for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
		{
			// Nothing to do
		}
	}
	gsmInstance.agpsTempData[CopyCounter++] = gsmInstance.agpsLocationData[LoopCounter];//','
	LoopCounter++;
	if(gsmInstance.agpsLocationData[LoopCounter] == 'A')
	{
		HAL_GPIO_WritePin( GPS_LED_GPIO_Port,  GPS_LED_Pin, GPIO_PIN_SET);
//		TelematicHeartBeat[enumTeleHearBeat_GPS_STATUS]=0x01;
	}
	else
	{
		HAL_GPIO_WritePin( GPS_LED_GPIO_Port,  GPS_LED_Pin, GPIO_PIN_RESET);
//		TelematicHeartBeat[enumTeleHearBeat_GPS_STATUS]=0x00;
	}

	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		// Nothing to do
	}
	LoopCounter++;
	genric_Cnt = 0;
	memset(GPS_LAT,0x00,sizeof(GPS_LAT));
	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		//LAT
		gsmInstance.agpsTempData[CopyCounter] = gsmInstance.agpsLocationData[LoopCounter];
		GPS_LAT[genric_Cnt++] = gsmInstance.agpsLocationData[LoopCounter];
		CopyCounter++;
	}
	if(strlen(GPS_LAT)!=0)
	{
		float templat = 0;
		templat = atof(&GPS_LAT[2]);
		templat /= 60;
		templat += (((GPS_LAT[0]-'0') * 10) + (GPS_LAT[1]-'0'));
		memset(GPS_LAT,0x00,sizeof(GPS_LAT));
		sprintf(GPS_LAT,"%.7f",templat);
	}
	gsmInstance.agpsTempData[CopyCounter++] = gsmInstance.agpsLocationData[LoopCounter];//','
	LoopCounter++;
	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		//N/S
		gsmInstance.agpsTempData[CopyCounter] = gsmInstance.agpsLocationData[LoopCounter];
		CopyCounter++;
	}
	gsmInstance.agpsTempData[CopyCounter++] = gsmInstance.agpsLocationData[LoopCounter];//','
	LoopCounter++;
	genric_Cnt = 0;
	memset(GPS_LONG,0x00,sizeof(GPS_LONG));
	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		//LONG
		gsmInstance.agpsTempData[CopyCounter] = gsmInstance.agpsLocationData[LoopCounter];
		GPS_LONG[genric_Cnt++] = gsmInstance.agpsLocationData[LoopCounter];
		CopyCounter++;
	}
	if(strlen(GPS_LONG)!=0)
	{
		float templon = 0;
		templon = atof(&GPS_LONG[3]);
		templon /= 60;
		templon += (((GPS_LONG[0]- '0') * 100 )+ ((GPS_LONG[1]-'0') * 10) +(GPS_LONG[2]-'0'));
		memset(GPS_LONG,0x00,sizeof(GPS_LONG));
		sprintf(GPS_LONG,"%.7f",templon);
	}

	gsmInstance.agpsTempData[CopyCounter++] = gsmInstance.agpsLocationData[LoopCounter];//','
	LoopCounter++;
	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		//E/W
		gsmInstance.agpsTempData[CopyCounter] = gsmInstance.agpsLocationData[LoopCounter];
		CopyCounter++;
	}
	gsmInstance.agpsTempData[CopyCounter++] = gsmInstance.agpsLocationData[LoopCounter];//','
	LoopCounter++;
	genric_Cnt = 0;
	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		GPS_SPEED_OVR_GND[genric_Cnt++]=gsmInstance.agpsLocationData[LoopCounter];
		//Speed Over Ground
		// Nothing to do
	}
	LoopCounter++;
	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		//Course Over Ground
		// Nothing to do
	}
	LoopCounter++;
	genric_Cnt = 0;
	for(; gsmInstance.agpsLocationData[LoopCounter] != ','; LoopCounter++)
	{
		//Date
		gsmInstance.agpsTempData[CopyCounter] = gsmInstance.agpsLocationData[LoopCounter];
		GPSUTCDATE[genric_Cnt++] = gsmInstance.agpsLocationData[LoopCounter];
		CopyCounter++;
	}
}



void Diagnostic(enmDiagnosticStatus DiagnosticStatus)
{
	Queue_LiveEnequeue(&gsmLivePayload,(char *)getDiagDataString(DiagnosticStatus));

}

char * getDiagDataString(enmDiagnosticStatus DiagnosticStatus)
{

	char * systemPayload = (char*)malloc(sizeof(char) * GSM_PAYLOAD_MAX_SIZE);
	if(systemPayload != NULL)
	{
		/*
			1.	Start of frame
			2.	Hardware ID
			3.	Model No.
			4.	UTC date time
			5.	Firmware Version
		*/
		memset(systemPayload,0x00,sizeof(char) * GSM_PAYLOAD_MAX_SIZE);
		/* Start of Frame */
		strcpy((char *)systemPayload,(char * )START_OF_FRAME);
		strcat((char *)systemPayload,(char * )",");

		/* Device UUID */
		strcat((char *)systemPayload,(char * )dinfo);
		strcat((char *)systemPayload,(char * )",");

		strcat((char *)systemPayload,(char * )MODEL_NUMBER);
		manufacturerDate(systemPayload);
		strcat((char *)systemPayload,"_Diagnostic");
		strcat((char *)systemPayload,(char * )",");

		PaylodTime(systemPayload);

		/* Firmware Version*/
		/* Firmware Version*/
		strcat((char *)systemPayload,(char * )BOOT_REGION);
		strcat((char *)systemPayload,(char * )FIRMWARE_VER);
		strcat((char *)systemPayload,(char * )",");

		DiagnosticString(systemPayload,DiagnosticStatus);

		/* End of Frame */
		strcat((char *)systemPayload,(char * )END_OF_FRAME);

		return systemPayload;
	}
	else
		return NULL; // malloc Error . Memory Allocation Failure
}

void DiagnosticString(char * systemPayload, enmDiagnosticStatus DiagnosticStatus)
{
	char temp[20] = "";
	switch(DiagnosticStatus)
	{
		case enmDiagnostic_SUCCESS_OK:
			strcat((char *)systemPayload,"FS");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			FOTACompleteFlag = TRUE;
			break;

		case enmDiagnostic_CAN_ID:
			strcat((char *)systemPayload,"CAN ID's : ");
			CANIDPrase(systemPayload);
			break;

		case enmDiagnostic_HW_ID_MISMATCH_ERROR: // Device HW ID Mismatch
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_FOTA_REQ_RX:
			strcat((char *)systemPayload,"FR");
			strcat((char *)systemPayload,(char * )",");
			strcat((char *)systemPayload,(char *)gau8FotaURL);
			strcat((char *)systemPayload,(char * )",");
			break;


		case enmDiagnostic_POWER_SUPPLY_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			strcat((char *)systemPayload,"Input Supply : ");
//			updateInputVoltage(ADCValues.ADC_Input_Voltage);
//			strcat((char *)systemPayload,(char * )voltage);
			strcat((char *)systemPayload,(char * )",");
			strcat((char *)systemPayload,"Internal Battery Supply : ");
//			updateInputVoltage(ADCValues.ADC_Batt_Voltage);
//			strcat((char *)systemPayload,(char * )voltage);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_RX_FILE_SIZE_ERROR: //Received File Size is Greater than (FLASH_SIZE-4k)/2
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",Received File Size : ");
			strcat((char *)systemPayload,gu32FotaFileSizeinBytes);
			strcat((char *)systemPayload,(char * )" Bytes,");
			break;

		case enmDiagnostic_FILE_DONWLOAD_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
//			strcat((char *)systemPayload,(char * )gsmInstance.u32GSMHttpResponseCode);
			strcat((char *)systemPayload,(char * )gu32FotaFileSizeinBytes);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_FLASH_ERRASE_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_FLASH_Write_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			itoa(u32FOTAFileBaseAddress,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )"//");
			itoa(u32FotaFileChunkCounter,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_BOOT_REGION_SELECTION_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_MEM_WR_CHK_SUM_ERROR: //MEM sector written, but downloaded DATA and Written data in MEM Mismatch
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_BOOT_REGION_JUMP_ERROR:
			strcat((char *)systemPayload,"FF");
			strcat((char *)systemPayload,(char * )",");
			itoa(DiagnosticStatus,temp,PAYLOAD_DATA_STRING_RADIX);
			strcat((char *)systemPayload,temp);
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_IMOBI_REQ_RX:

//			if(Current_Immobilization_State == 1)
//			{
//				strcat((char *)systemPayload,"IR,");
//				itoa(Current_Immobilization_State,temp,PAYLOAD_DATA_STRING_RADIX);
//				strcat((char *)systemPayload,temp);
//			}
//			else if(Current_Immobilization_State == 0)
//			{
//				strcat((char *)systemPayload,"IR,");
//				itoa(Current_Immobilization_State,temp,PAYLOAD_DATA_STRING_RADIX);
//				strcat((char *)systemPayload,temp);
//			}
//			else
//			{
//				strcat((char *)systemPayload,"IF,");
//				itoa(Current_Immobilization_State,temp,PAYLOAD_DATA_STRING_RADIX);
//				strcat((char *)systemPayload,temp);
//			}
//
//			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_IMOBI_SUCCESS_OK:
			strcat((char *)systemPayload,"IS");
			strcat((char *)systemPayload,(char * )",");
			break;

		case enmDiagnostic_IDLE:
			strcat((char *)systemPayload,"IDLE");
			strcat((char *)systemPayload,(char * )",");
			break;

		default:
			strcat((char *)systemPayload,"IDLE");
			strcat((char *)systemPayload,(char * )",");
			break;
	}//end of switch(DiagnosticStatus)

}//end of void DiagnosticString(char * systemPayload,enmDiagnosticStatus DiagnosticStatus)

void CANIDPrase(char * systemPayload)
{
	uint8_t CANIDloop = 1;

	char temp[10] = "";
	for(CANIDloop=1; CANIDloop <= (MAX_CAN_IDS_SUPPORTED + 1); CANIDloop++)
	{
		itoa(gu32CanConfigurationArray[CANIDloop],temp,CAN_PAYLOADSTRING_RADIX);
		strcat((char *)systemPayload,temp);
		strcat((char *)systemPayload,",");
	}
}

void manufacturerDate(char *systemPayload )
{
	char temp[10] = "";
//	itoa(DATE,temp,CAN_PAYLOADSTRING_RADIX);
	sprintf(temp,"%2d",DATE);
	strcat((char *)systemPayload,(char * )temp);

	memset(temp,0x00,sizeof(temp));
	sprintf(temp,"%2d",MONTH);
	strcat((char *)systemPayload,(char * )temp);

	memset(temp,0x00,sizeof(temp));
	sprintf(temp,"%d",YEAR);
	strcat((char *)systemPayload,(char * )temp);
}

/******************************************************************************
* Function : updateInputVoltage()
*//**
* \b Description:
*
* This function is used to Format Input voltage float to string .
*
* PRE-CONDITION: Add relevant linker flag -u float in settings
*
* POST-CONDITION:
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateDeviceSignature();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/12/2021 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateInputVoltage(float Volt)
{
//	sprintf(gcSystemSupplyVoltage,"%.2f",gfInputSupplyVoltage);
	uint8_t Integer = 0;
	float Frac =0;
	uint16_t ftoi = 0;
	if(Volt > 0)
	{
		Integer = Volt;
		Frac = (Volt - (int)Volt);
		ftoi = Frac*100;

		if(Integer >= 10)
		{
			voltage[0] = (Integer/10)+48;
			voltage[1] = (Integer%10)+48;
		}
		else
		{
			voltage[0] = '0';
			voltage[1] = Integer+48;
		}

		voltage[2] = '.' ;

		if(ftoi >= 10)
		{
			voltage[3] = (ftoi/10)+48;
			voltage[4] = (ftoi%10)+48;
		}
		else
		{
			voltage[3] = '0';
			voltage[4] = ftoi+48;
		}
	}
	else
	{
		strcpy(voltage,"0.00");
	}

	//ftoa[0] = "1";

}

//******************************* End of File *******************************************************************

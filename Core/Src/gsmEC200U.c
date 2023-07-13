/*
 * gsmEC200U.c
 *
 *  Created on: Mar 28, 2022
 *      Author: admin
 */


/*******************************************************************************
* Title                 :   2G Cellular Module Interfacing
* Filename              :   gsmEC200U.c
* Author                :
* Origin Date           :   28/2/20122
* Version               :   1.0.0
* Compiler              :
* Target                :   STM32L433 - TorMini-4G
* Notes                 :   None
*
* Copyright (c) by KloudQ Technologies Limited.

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
*
*
*******************************************************************************/
/*************** FILE REVISION LOG *****************************************
*
*    Date    Version   Author         	   Description
*  28/02/22   1.0.0    					   Initial Release.
*
*******************************************************************************/

/** @file  gsmEC200U.c
 *  @brief GSM HTTP / SMS / GPS and utility Functions
 */

/******************************************************************************
* Includes
*******************************************************************************/
#include "main.h"
#include "stm32l433xx.h"
#include "applicationdefines.h"
#include <string.h>
#include <stdlib.h>
#include "gsmEC200U.h"
#include "timer.h"
#include "externs.h"
#include "queue.h"
#include "user_flash.h"
#include "user_error.h"
#include "payload.h"

/******************************************************************************
* Module Preprocessor Constants
*******************************************************************************/
/**
 * Doxygen tag for documenting variables and constants
 */

/******************************************************************************
* Module Preprocessor Macros
*******************************************************************************/

/******************************************************************************
* Module Typedefs
*******************************************************************************/


#define SIM_DEACTIVATED 0x00
#define SIM_ACTIVATED	0x01

/******************************************************************************
* Module Variable Definitions
*******************************************************************************/

char g_u8PaylodLenMsgBuff[60] = {0};

#define PAYLOD_TYPE_LIVE	1
#define PAYLOD_TYPE_BUFF	2

uint8_t g_u8PayloadType = 0;

char * tempdata = "";
uint32_t payloadLength = 0;
char gau8FotaRequest[70]={'0'};
char gau8SUBRequest[150] = {'0'};
uint8_t SUBTriggerFlag = FALSE;
uint32_t gu32FotaRquestFlag = FALSE;
_Bool boolFOTACompleteFlag = FALSE;
char as8GSM_SUB_BUFF[200];
uint64_t cu32IMEI = 0;


/*GPS Command's*/
static char gau8GPS_POWERON[]="AT+QGPS=1\r\n";  							/* Turn on GNSS */
//static char gau8GPS_POWEROFF[]="AT+QGPSEND\r\n";							/* Turn OFF GNSS */

static char gau8GPS_GPSNMEASRC1[]="AT+QGPSCFG=\"nmeasrc\",1\r\n";
/* GPS Commands Supported : RMC / GGA - Dead reckoning*/
#if(GPSDATATYPE == GPSDATATYPERMC)
static char gau8GPS_GPSNMEA[]="AT+QGPSGNMEA=\"RMC\"\r\n";  					/* Time, date, position, course and speed data */
#elif(GPSDATATYPE == GPSDATATYPERECK)
static char gau8GPS_GPSNMEA[]="AT+QGPSGNMEA=\"GGA\"\r\n";  					/* Time, date, position, course and speed data */

#endif

static char gau8GPS_UARTEN[]="AT+QGPSCFG=\"outport\",\"none\"\r\n";      						/* Send out the GPS data */
static char gau8GPS_SETBR[]="AT+IPR=115200;&W\r\n";						    /* Set GPS BaudRate */
						/* Enable GPIO 01 for GPS */

/* GPS Type Supported : Triangulation or Onchip */
//#if(GPSTYPE == GPSTRANGULATION)
//static char gau8GPS_CGNSINF[]="AT+QLBS\r\n";//static char gau8GPS_CGNSINF[]="AT+QLBS=1\r\n";//"AT+CIPGSMLOC=1,1\r\n";						/* Trangulation :Get GPS Data */
//#elif (GPSTYPE == GPSGNSSCHIP)
//static char gau8GPS_CGNSINF[]="AT+QLBS\r\n";								/* GNSS Chip : Get GPS Data */
//#endif

uint8_t gu8HttpPayloadFlag = 0;												/* Update HTTP Payload Flag */
uint8_t gu8CheckSMS = FALSE;
/* Used for send data gsm command */
//static char * gu8GSMDataTimeout=",120000\r\n\0"; 							/* Data input : Max time in ms */

/* GPRS/GSM Commands */
static char gau8GSM_AT[4]="AT\r\n";												/* Module Attention */
static char gau8GSM_ATE0[6]="ATE0\r\n";    										/* Echo Off */
static char gau8GSM_ATCPIN[10]="AT+CPIN?\r\n";									/* Is SIM Ready */
static char gau8GSM_ATIMEI[]="AT+GSN=1\r\n";									/*IMEI No.*/
static char gau8GSM_CSQ[]="AT+CSQ\r\n";										/* Query Signal Strength */
static char gau8GSM_ATCREG[]="AT+CREG?\r\n"; 								/* Registration */
static char gau8GSM_ATQNWINFO[]="AT+QNWINFO\r\n";
//static char gau8GSM_ATCGREG[]="AT+CGREG?\r\n"; 								/* Registration Status*/
static char gau8GSM_ATCMEE[]="AT+CMEE=2\r\n"; 								/* Enable numeric error codes  */

static char gau8GSM_ATHTTPCONTXTID[]="AT+QHTTPCFG=\"contextid\",1\r\n"; 							/* Init HTTP */
static char gau8GSM_ATHTTPRESPONSEHDR[] ="AT+QHTTPCFG=\"responseheader\",0\r\n";
static char gau8GSM_ATHTTPREQHDR[] = "AT+QHTTPCFG=\"requestheader\",0\r\n";
static char gau8GSM_ATHTTPQIACT1[]="AT+QIACT?\r\n";				/* HTTP Parameter */
static char gau8GSM_ATSETAPN[]="AT+QICSGP=1,1,";						/* Set APN */
static char gau8GSM_ATHTTPQIACT[]="AT+QIACT=1\r\n";
static char gau8GSM_ATHTTPQIACT2[]="AT+QIACT?\r\n";

//static char gau8GSM_ATHTTPSSLCTXID[]="AT+QHTTPCFG=\"sslctxid\",1\r\n";
//static char gau8GSM_ATHTTPSSLCONFIG[] = "AT+QSSLCFG=\"sslversion\",0,1\r\n"; 	//Set SSL version as 1 which means TLSV1.0.
//static char gau8GSM_ATCHIPERSUITE[] = "AT+QSSLCFG=\"ciphersuite\",0,0xFFFF\r\n"; //Set SSL cipher suite as 0x0005 which means RC4-SHA.
//static char gau8GSM_ATSSLVERIFYLVL[]= "AT+QSSLCFG=\"seclevel\",0,0\r\n"; //Set SSL verify level as 0 which means CA certificate is not needed.

static char gau8GSM_ATQHTTPURLLEN[] = "AT+QHTTPURL=";
static char gau8GSM_ATHTTPACTIONFOTA[20]="AT+QHTTPGET=80\r\n";	 				/* Send Data over HTTP  80:response time*/
//static char gau8GSM_ATQHTTPGETEX[]="AT+QHTTPGETEX=80,"; // get data in chunks/* @Fota : Read File / Server URL Response . TODO: Array Length according to data */
static char gau8GSM_ATQHTTPGETEXREAD[30]={'0'};
static char gau8GSM_ATQHTTPREAD[]="AT+QHTTPREAD=80\r\n";


char gau8GSM_ATAPNL[100] = {'0'};
char gau8GSM_ATAPN[180] = {'0'};												/* Network APN */
char gau8GSM_ATURL[180] = {'0'};	/* Server URL */

static char gau8GSM_SMSRecepient[180] = {'0'};									/* User Phone Number */
//static char gau8GSM_ATHTTPACTION[17]="AT+HTTPACTION=1\r\n";	 					/* Send Data over HTTP  0:GET method 1:POST 2:HEAD method */static char gau8GSM_ATHTTPPARAUA[50]={0};/* User Agent Signature */
//static char gau8GSM_ATW[6]="AT&W\r\n";											/* Save the setting */
//static char gau8GSM_ATCCLK[10]="AT+CCLK?\r\n";									/* Query Time */
static char gau8GSM_ATCCLK[]="AT+QLTS\r\n";

/*MQTT Command*/
static char gau8GSM_ATQMQTTCFG[] = "AT+QMTCFG=\"recv/mode\",0,0,1\r\n";
static char gau8GSM_ATQMQTTOPEN[] = "AT+QMTOPEN=0,"; // KTL
static char gau8GSM_MQTTOPEN[100]={'0'};
//static char gau8GSM_ATQMQTTOPEN[] = "AT+QMTOPEN=0,\"13.127.47.100\",1883\r\n"; // MHNDR
static char gau8GSM_ATQMTCONN[] = "AT+QMTCONN=0,";
static char gau8GSM_MQTTCONN[150] = {'0'};
//static char gau8GSM_ATQMTCONN[] = "AT+QMTCONN=0,\"054055326513800775776160481\",\"kloudquser\",\"ELf9uWcsZr\"\r\n";
static char gau8GSM_ATQMTSUB[] = "AT+QMTSUB=0,1,";
static char gau8GSM_MQTTSUB[200] = {'0'};
//static char gau8GSM_ATQMTSUB[] = "AT+QMTSUB=0,1,\"KLOUDQ/device IMEI/PUB\",2\r\n";
static char gau8GSM_ATQMTPUBEX[250] = {0};  // Updated in func updateHttpDataLength
//static char gau8GSM_ATQMTUNS[] = "AT+QMTUNS=0,2,\"KLOUDQ/device IMEI/PUB\"" ; //Unsubscribe from topics.

//char gau8GSM_smsto[15]="+918669666703";
char gau8GSM_url[150]="http://kloudqpocapi.remotemonitor.in/api/Values/PostStringData";	/* Holds Server URL */
char gau8GSM_apn[100]="airteliot.com";										/* Holds Network APN */

char gau8GSM_TimeStamp[25]={'0'};												/* Stores Network Time Stamp */
char gu8NewURL[150] = "http://kloudqpocapi.remotemonitor.in/api/Values/PostStringData";//http://pocapitest.kloudqapps.net/api/values/PostStringData";
/* Testing URL @ Ajit Aher */
//char gau8FotaURL[100] = "http://59.163.219.178:81/fota/Terex/Piaggio_Y_LEDToggBin.bin";
char gau8FotaURL[100] = "";
//const char gau8FotaURLNEW[100] = "http://20.198.65.195/fota/Terex/";
const char gau8FotaURLNEW[150] = "";

const char MQTTUrl[]="Iemqtt.tor-iot.com";
const char MQTTPort[] = "1883";

char MQTT_ClientID[50] = "";
const char MQTT_UserName[] = "";
const char MQTT_Password[] = "";

const char MQTT_PUBPayload[] 	= "Terex/MQTT_v1/Payload";
const char MQTT_PUBDiagnostic[] = "Terex/MQTT_v1/Diagnostic";
const char MQTT_PUBGeo[] 		= "Terex/MQTT_v1/Geo";
const char MQTT_PUBBuffer[]		= "Terex/MQTT_v1/Buffer";

const char MQTT_SUBConfig[] 	= "Terex/MQTT_v1/Config";
/*
const char MQTTUrl[]="npdmqtt.remotemonitor.in";
const char MQTTPort[] = "1883";

char MQTT_ClientID[50] = "";
const char MQTT_UserName[] = "";
const char MQTT_Password[] = "";

const char MQTT_PUBPayload[] 	= "NPD/MQTT_v1/Payload";
const char MQTT_PUBDiagnostic[] = "NPD/MQTT_v1/Diagnostic";
const char MQTT_PUBGeo[] 		= "NPD/MQTT_v1/Geo";
const char MQTT_PUBBuffer[]		= "NPD/MQTT_v1/Buffer";

const char MQTT_SUBConfig[] 	= "NPD/MQTT_v1/Config";
*/

/*
const char MQTTUrl[]	="log9mqtt.remotemonitor.in";
const char MQTTPort[] 	= "1883";

char MQTT_ClientID[50] 		= "";
const char MQTT_UserName[]	= "";
const char MQTT_Password[] 	= "";

const char MQTT_PUBPayload[] 	= "LOG9/MQTT_v1/Payload";
const char MQTT_PUBDiagnostic[] = "LOG9/MQTT_v11/Diagnostic";
const char MQTT_PUBGeo[] 		= "LOG9/MQTT_v1/Geo";
const char MQTT_PUBBuffer[]		= "LOG9/MQTT_v1/Buffer";

const char MQTT_SUBConfig[] 	= "LOG9/MQTT_v1/Config";

*/

uint8_t g_u8HttpConEstablishedFlag = 0;

char gau8FotaData[2000]={'0'};
uint32_t testVar = 0;															/* Dummy : Test Variable */
uint8_t gu8SendSMS = FALSE;	 													/* Send SMS Flag */
uint8_t gu8ReadSMS = FALSE;														/* Read SMS Flag */
uint32_t u8LastHttpResponseCode = 0;											/* Last Http Response Code */
uint32_t u8LastMqttResponseCode = 0;											/* Last MQTT Response Code */

RTC_DateTypeDef lastKnownSDate1;


uint32_t gu32NWRTCYear = 0;
uint32_t gu32NWRTCMnt = 0;
uint32_t gu32NWRTCDay = 0;

uint32_t gu32NWRTCHH = 0;
uint32_t gu32NWRTCYMM = 0;
uint32_t gu32NWRTCSS = 0;

_Bool boolAttemptFota = FALSE;
char gu32FotaFileSizeinBytes[8] = {'0'};

uint32_t u32FOTAFileBaseAddress = 0;
uint32_t u32FotaFileChunkCounter = 0;
uint32_t u32FotaFileRemainingBytes = 0;
uint32_t u32FotaFileStatus = 0;
uint32_t u32FotaFileReadComplete = 0;
uint32_t u32MemoryWriteCycle = FALSE;
uint32_t u32FotaFileSizeInBytes = 0;
uint32_t u32FotaChunckLength = 0;
uint32_t u32MemoryEraseStatus = 0;
uint32_t u32FlashMemoryWriteStatus = 0;
char buffer[6] = {'0'};
char bufferRemBytes[6] = {'0'};

uint32_t gu32RTCTestFlag = 0;

/* Global GSM Instance */
strctGSM gsmInstance;
strctBuffQUEUE gsmBuffPayload;
strctQUEUE gsmLivePayload;

uint32_t gu32ModuleInitComplete = 0;

uint8_t u8MqqtResponseFlag = FALSE;
uint8_t GPSDataValid = FALSE;
uint8_t SystemOnStatus = TRUE;
uint8_t gu8ModuleInitFlag = FALSE;
char gau8GSM_NWINFO[15]={'0'};
char gau8GSM_SIM_REG[1]="";
char gau8GSM_Signal[2] = "";
uint8_t signal_qaulity = 0;
char gau8GSM_IMEI[18]={'0'};

/* Simcom Operation States */
const strctGSMStateTable gsmStateTableArray[45]=
{
	{gau8GSM_AT,"OK\r\n",ONE_SEC,&gsmInstance},//0
	{gau8GSM_ATE0,"OK\r\n",ONE_SEC,&gsmInstance},//1
	{gau8GSM_ATCPIN,"+CPIN:",ONE_SEC,&gsmInstance},//3
	{gau8GSM_ATIMEI,"+GSN:",ONE_SEC,&gsmInstance},
	{gau8GSM_ATCREG,"+CREG",TEN_SEC,&gsmInstance},//5
	{gau8GSM_CSQ,"+CSQ:",ONE_SEC,&gsmInstance},//4
	{gau8GSM_ATCCLK,"+QLTS:",ONE_SEC,&gsmInstance},//2
	{gau8GSM_ATQNWINFO,"+QNWINFO:",ONE_SEC,&gsmInstance},
	{gau8GSM_ATCMEE,"OK\r\n",ONE_SEC,&gsmInstance},//7
	{gau8GPS_SETBR,"OK\r\n",ONE_SEC,&gsmInstance},//8

	/*HTTP*/
	{gau8GSM_ATHTTPCONTXTID,"OK\r\n",ONE_SEC,&gsmInstance},//9
	{gau8GSM_ATHTTPRESPONSEHDR,"OK\r\n",ONE_SEC,&gsmInstance},//10
	{gau8GSM_ATHTTPREQHDR,"OK\r\n",ONE_SEC,&gsmInstance},//11
	{gau8GSM_ATHTTPQIACT1,"OK\r\n",ONE_SEC,&gsmInstance},//12
	{gau8GSM_ATAPN,"OK\r\n",ONE_SEC,&gsmInstance},//13
	{gau8GSM_ATHTTPQIACT,"OK\r\n",TEN_SEC,&gsmInstance},//14
	{gau8GSM_ATHTTPQIACT2,"OK\r\n",TEN_SEC,&gsmInstance},//15
//	/*SSL*/
//	{gau8GSM_ATHTTPSSLCTXID,"OK\r\n",TWO_SEC,&gsmInstance},//16
//	{gau8GSM_ATHTTPSSLCONFIG,"OK\r\n",TWO_SEC,&gsmInstance},//17
//	{gau8GSM_ATCHIPERSUITE,"OK\r\n",TWO_SEC,&gsmInstance},//18
//	{gau8GSM_ATSSLVERIFYLVL,"OK\r\n",TWO_SEC,&gsmInstance},//19

	/*GSP/GNSS Commamds*/
	{gau8GPS_UARTEN,"OK\r\n",ONE_SEC,&gsmInstance},//20
	{gau8GPS_POWERON,"OK\r\n",ONE_SEC,&gsmInstance}, //21
	{gau8GPS_GPSNMEASRC1,"OK\r\n",ONE_SEC,&gsmInstance}, //22
	{gau8GPS_GPSNMEA,"+QGPSGNMEA:",ONE_SEC,&gsmInstance}, //23

	/*MQTT Commands*/
	{gau8GSM_ATQMQTTCFG,"OK\r\n",ONE_SEC,&gsmInstance}, //24
	{gau8GSM_MQTTOPEN,"QMTOPEN: 0,2\r\n",TWO_SEC,&gsmInstance}, //25
	{gau8GSM_MQTTCONN,"QMTCONN: 0,0,0\r\n",FIVE_SEC,&gsmInstance}, //26
	{gau8GSM_MQTTSUB,"QMTSUB: 0,1,0",ONE_SEC,&gsmInstance}, //27
	{gau8GSM_ATQMTPUBEX,">",TEN_SEC,&gsmInstance}, //28
	{"","OK\r\n",FIFTEEN_SEC,&gsmInstance}, //29

	/*FOTA*/
	{gau8GSM_ATAPNL,"CONNECT",TWO_MIN,&gsmInstance},//30
	{gau8FotaURL,"OK\r\n",FIFTEEN_SEC,&gsmInstance},//31
	{gau8GSM_ATHTTPACTIONFOTA,"+QHTTPGET:",TWO_MIN,&gsmInstance},//32
	//{"","",FIFTEEN_SEC,&gsmInstance}, //28
	{gau8GSM_ATQHTTPGETEXREAD,"+QHTTPGET:",TWO_MIN,&gsmInstance},//33
	{gau8GSM_ATQHTTPREAD,"CONNECT",TWO_MIN,&gsmInstance},//34
};

/******************************************************************************
* Function Prototypes
*******************************************************************************/

/******************************************************************************
* Function Definitions
*******************************************************************************/
/******************************************************************************
* Function : initGSMSIM868()
*//**
* \b Description:
*
* This function is used to Initialise GSM Structure used for SIM868 Module
*
* PRE-CONDITION: Initialise UART with with the module is interfaced
*
* POST-CONDITION: Initialized GSM structure
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	initGSMSIM868();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void initGSMSIM868(void)
{
	/* Init Buffers */
	memset(gsmInstance.agpsLocationData, GSM_ARRAY_INIT_CHAR, (GPS_LOCATION_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmNetworkIP, GSM_ARRAY_INIT_CHAR, (GPRS_NETWORKIP_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmSignalStrength, GSM_ARRAY_INIT_CHAR, (GSM_SIGSTRGTH_ARRAY_SIZE * sizeof(char)));
	memset((char *)gsmInstance.as8GSM_Response_Buff,GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(char)));
	memset(gsmInstance.agsmCommandResponse, DATA_UNKNOWN, sizeof(gsmInstance.agsmCommandResponse[0][0])
		   * GSM_TOTAL_COMMANDS * GSM_TOTAL_COMMANDS);
	memset(gsmInstance.agsmHTTPRequestStatus,DATA_UNKNOWN,GSM_HTTP_RESPONSE_CODE_SIZE * sizeof(char));
	memset(gsmInstance.strSystemSMS.agsmSMSRecipient,DATA_UNKNOWN,SMS_MOB_NO_LENGTH * sizeof(char));
	memset(gsmInstance.strSystemSMS.agsmSMSMessageBody,0x00,SMS_MAX_MSG_LENGTH * sizeof(char));
	memset(gsmInstance.u32GSMHttpResponseCode,0x00,sizeof(char) * 3);
	memset(gau8GSM_ATAPN, 0x00, (180 * sizeof(char)));
	memset(gau8GSM_ATURL, 0x00, (180 * sizeof(char)));
	memset(gau8GSM_SMSRecepient, 0x00, ( 180 * sizeof(char)));

	strcat((char *)gau8GSM_ATAPN,(char *)gau8GSM_ATSETAPN);
	strcat((char *)gau8GSM_ATAPN,(char *)"\"");
	strcat((char *)gau8GSM_ATAPN,(char *)gau8GSM_apn);
	strcat((char *)gau8GSM_ATAPN,(char *)"\"");
	strcat((char *)gau8GSM_ATAPN,(char *)",\"\",\"\",1\r\n");

	strcpy((char *)gau8GSM_ATURL,"http://kloudqpocapi.remotemonitor.in/api/Values/PostStringData");
	strcat(gau8GSM_ATURL,"\r\n");

//	uint32_t temp_len = strlen(gau8FotaURL);
//	strcat((char*)gau8GSM_ATAPNL,(char *)gau8GSM_ATQHTTPURLLEN);
//	itoa(temp_len,buffer,PAYLOAD_DATA_STRING_RADIX);
//	strcat((char*)gau8GSM_ATAPNL,(char *)buffer);
//	strcat((char*)gau8GSM_ATAPNL,",80\r\n"); // time out


	memset(gau8GSM_MQTTOPEN, 0x00, (100 * sizeof(char)));
	strcat((char *)gau8GSM_MQTTOPEN,(char *)gau8GSM_ATQMQTTOPEN);
	strcat((char *)gau8GSM_MQTTOPEN,(char *)"\"");
	strcat((char *)gau8GSM_MQTTOPEN,(char *)MQTTUrl);
	strcat((char *)gau8GSM_MQTTOPEN,(char *)"\"");
	strcat((char *)gau8GSM_MQTTOPEN,(char *)",");
	strcat((char *)gau8GSM_MQTTOPEN,(char *)MQTTPort);
	strcat((char *)gau8GSM_MQTTOPEN,(char *)"\r\n");

	memset(gau8GSM_MQTTCONN, 0x00, (150 * sizeof(char)));
	strcat((char *)gau8GSM_MQTTCONN,(char *)gau8GSM_ATQMTCONN);
	strcat((char *)gau8GSM_MQTTCONN,(char *)"\"");
	strcpy(MQTT_ClientID,(char * )dinfo);
	strcat((char *)gau8GSM_MQTTCONN,(char *)MQTT_ClientID);
	strcat((char *)gau8GSM_MQTTCONN,(char *)"\",\"");
	strcat((char *)gau8GSM_MQTTCONN,(char *)MQTT_UserName);
	strcat((char *)gau8GSM_MQTTCONN,(char *)"\",\"");
	strcat((char *)gau8GSM_MQTTCONN,(char *)MQTT_Password);
	strcat((char *)gau8GSM_MQTTCONN,(char *)"\"\r\n");

	memset(gau8GSM_MQTTSUB, 0x00, (200 * sizeof(char)));
	strcat((char *)gau8GSM_MQTTSUB,(char *)gau8GSM_ATQMTSUB);
	strcat((char *)gau8GSM_MQTTSUB,(char *)"\"");
	strcat((char *)gau8GSM_MQTTSUB,(char *) MQTT_SUBConfig);
	strcat((char *)gau8GSM_MQTTSUB,(char *)"\",2\r\n");


	strcat(gau8GSM_SMSRecepient,"\r\n");

	gsmInstance.strSystemSMS.u8NewMessage = FALSE;
	gsmInstance.enmcurrentTask = enmGSMTASK_RESET;
	gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
	gsmInstance.u8isConnected = FALSE;
	gsmInstance.u8GSM_Response_Character_Counter = 0;
	gsmInstance.u8gsmRegistrationStatus = FALSE;
	gsmInstance.u8gsmSIMReadyStatus = FALSE;
	gsmInstance.u8gsmRetryCount = GSM_MAX_RETRY;
	gsmInstance.boolAttemptFota= FALSE;
	gsmInstance.u32GSMTimer = ONE_SEC;
	gu32GSMHangTimer = THREE_MIN;
	gsmInstance.u32GSMHeartbeatTimer = 0;
	gsmInstance.u8IllegalHttpResponseCounter = 0;
	gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
	gsmInstance.enmGSMCommand = enmGSMSTATE_ATE0;
	gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;

	gu32ModuleInitTimer = FIVEHUNDRED_MS;
	gu8ModuleInitFlag = TRUE;
	gu8ModuleInitNotifyCnt = 0;

	HAL_GPIO_WritePin(GPS_LED_GPIO_Port, GPS_LED_Pin,GPIO_PIN_RESET);

	g_u8HttpConEstablishedFlag = 0;
	/* Potential Hang Issue due to this variable */
	gu32ModuleInitComplete = 0;
	 u8daignostic|=(1<<6);
	GSMInitCompleteFlag = FALSE;

	SUBTriggerFlag = FALSE;
	gu32FotaRquestFlag =FALSE;
//	FOTACompleteFlag = FALSE;
#if(DEBUGENABLE == TRUE)
	strcat(DebugBuff,"\r\nGSM Module Init ......\r\n");
#endif					
}

/******************************************************************************
* Function : operateGSMSIM868()
*//**
* \b Description:
*
* This function is used to Operate GSM module interfaced with MCU over configured UART
*
* PRE-CONDITION: Initialise UART with with the module is interfaced .
*
* POST-CONDITION: Operates Module for HTTP data Upload and GPS co-ordinate fetching
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	initGSMSIM868();
* 	operateGSMSIM868();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 01/09/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> HTTP Timeout added (16 Mins) </td></tr>
* <tr><td> 01/02/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> default case handle </td></tr>
* <tr><td> 24/03/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> LIFO/FIFO Support Added </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
void operateGSMSIM868()
{
	if(gsmInstance.u32GSMTimer != 0)
		return;

	if(gsmInstance.enmcurrentTask > 9)
	{
		gsmInstance.enmcurrentTask = 0;
	}

	/* Operate Current Task */
	switch(gsmInstance.enmcurrentTask)
	{
		case enmGSMTASK_RESET:

			if(gsmInstance.enmGSMPwrState == enmGSM_PWRNOTSTARTED)
			{
				HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_SET);
				gsmInstance.u32GSMTimer =  SEVEN_SEC;
				gsmInstance.enmGSMPwrState = enmGSM_PWRSTARTED;
			}
			else if(gsmInstance.enmGSMPwrState == enmGSM_PWRSTARTED)
			{
				HAL_GPIO_WritePin(GSM_PWR_KEY_GPIO_Port,GSM_PWR_KEY_Pin,GPIO_PIN_RESET);
				gsmInstance.u32GSMTimer = FOUR_SEC;
				gsmInstance.enmcurrentTask = enmGSMTASK_INITMODULE;
				gsmInstance.enmGSMPwrState = enmGSM_PWRCOMPLETED;
			}
			else
			{
				if((gsmInstance.enmGSMPwrState == enmGSM_PWRCOMPLETED) && (gsmInstance.enmcurrentTask == enmGSMTASK_RESET))
				{
					gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
				}
			}

			if(gsmInstance.enmGSMPwrState > 2)
			{
				gsmInstance.enmGSMPwrState = enmGSM_PWRNOTSTARTED;
			}
			break;


		case enmGSMTASK_INITMODULE:
			/* Initialize Module : ATE0 to HTTP URL */
			if(gsmInstance.enmGSMCommandState == enmGSM_CMDTIMEOUT)
			{
				/* Reset Module */
				initGSMSIM868();
			}
			else
			{
				/* Send Command and Check Response */
				if(gsmInstance.enmGSMCommand == enmGSMSTATE_MQTTPUBEX)

				{
					if(gu8CheckSMS == TRUE)
					{
						gsmInstance.enmGSMCommand =  enmGSMSTATE_GPSNMEA;
						gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
						gu8ReadSMS = TRUE;
						gu8CheckSMS = FALSE;
					}
					else
					{
						/* Module is initialized ,Check Received SMS */
						/* 31-3-2020 : Altered for testing FOTA File Read */
						gsmInstance.enmGSMCommand =  enmGSMSTATE_GPSNMEA;
						gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
					}
				}
				else
				{
					sendGSMCommand(gsmInstance);
				}
			}
			break;

		case enmGSMTASK_ISALIVE:
				sendGSMCommand(gsmInstance);
			break;

		case enmGSMTASK_READSMS:
			if(gu8ReadSMS == TRUE)
				sendGSMCommand(gsmInstance);
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSNMEA;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
				gsmInstance.u32GSMTimer = TWO_SEC;
			}

			break;

		case enmGSMTASK_SENDSMS:
			if(gu8SendSMS == TRUE)
				sendGSMCommand(gsmInstance);
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSNMEA;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
				gsmInstance.u32GSMTimer = TWO_SEC;
			}
			break;

		case enmGSMTASK_UPDATELOCATION:
			/*Every 30 Sec */
			sendGSMCommand(gsmInstance);
			break;

		case enmGSMTASK_UPLOADDATA:
		{

			/* Send Data from the Queue with upload Time Interval */
			//if((Queue_IsLiveQueueEmpty(&gsmLivePayload) == FALSE )&& (gsmLivePayload.data[gsmLivePayload.tail] != NULL))
			if(((Queue_IsLiveQueueEmpty(&gsmLivePayload) == FALSE )&& (gsmLivePayload.data[gsmLivePayload.tail] != NULL))
							||((Queue_IsBuffQueueEmpty(&gsmBuffPayload) == FALSE )&& (gsmBuffPayload.data[gsmBuffPayload.tail] != NULL)))
			{
				if((gu8HttpPayloadFlag == 0) && (gsmInstance.enmGSMCommand == enmGSMSTATE_ATHTTPDATA))
				{
					/* Update Payload Length once Every Cycle */
					updateHttpDataLength();
					gu8HttpPayloadFlag = 1;
				}
				else if((gu8HttpPayloadFlag == 1) && (gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA))
				{
					/* Flag to update new data length */
					gu8HttpPayloadFlag = 0;
				}
				else if((gu8HttpPayloadFlag == 0) && (gsmInstance.enmGSMCommand ==enmGSMSTATE_MQTTPUBEX))
				{
					updateHttpDataLength();
					gu8HttpPayloadFlag = 1;
				}
				else if((gu8HttpPayloadFlag == 1) && (gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA))
				{
					/* Flag to update new data length */
					gu8HttpPayloadFlag = 0;
				}
				else
				{
					sendGSMCommand(gsmInstance);
				}
			}
			else
			{
				gsmInstance.enmGSMCommand = enmGSMSTATE_GPSNMEA;
				gsmInstance.enmcurrentTask = enmGSMTASK_UPDATELOCATION;
			}
		}
			break;

		case enmGSMTASK_GETDATA:
			/* Provision for Two Way communication with Server : ex Modbus . Not Implemented
			 * In this Firmware */
			initGSMSIM868();
			break;

		case enmGSMTASK_DOWNLOADFOTAFILE:
				sendGSMCommand(gsmInstance);
			break;

		case enmGSMTASK_IDLE:
			gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
			gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
			break;

		default:
			/* Log illegal State Error */
			initGSMSIM868();
			break;
	}
	gu32GSMHangTimer = THREE_MIN;
}

/******************************************************************************
* Function : sendGSMCommand()
*//**
* \b Description:
*
* This function is used to Send commands to  GSM module interfaced with MCU over
* configured UART . Called from operateGSMSIM868();
*
* PRE-CONDITION: Initialise UART with with the module is interfaced .
*
* POST-CONDITION: Send commands to operate module after valid response from module.
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	sendGSMCommand();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 01/09/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> HTTP Timeout added (16 Mins) </td></tr>
* <tr><td> 19/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Bug :IP Array is getting
* 																			  corrupt with OK line 460 : Solved </td></tr>
* <tr><td> 27/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Signal Strength Cut Copy bug -Solved </td></tr>
* <tr><td> 27/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Update : When Web Service is not
 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	 	   responding with 200
																					   maintain counter and reset .
																					    MAX_HTTP_ATTEMPTS = 10 </td></tr>
  <tr><td> 27/12/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Update : SMS Parsing Added </td></tr>
  <tr><td> 26/03/2020 </td><td> 0.0.1            </td><td> HL100133 </td><td> Restructured Case 200 for HTTP Action
																			  Memory Support Added for SMS .
																			  New Command for update mobile no added
																			  LIFO/FIFO Support Added </td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
char bufferChunkBytes[6] = {'0'};
void sendGSMCommand()
{
	static char * command;
	static uint32_t u8CharacterCounter;
	static uint32_t u8LoopCounter ;

	switch (gsmInstance.enmGSMCommandResponseState)
	{
		case enmGSM_SENDCMD:
			if(gsmInstance.enmGSMCommandState == enmGSM_CMDSEND)
			{
				u8LoopCounter = 0;
				if(gsmInstance.enmGSMCommand == enmGSMSTATE_SENDDATA)
				{
#if (DATA_PROCESSING_METHOD == FIFO )
					if(g_u8PayloadType == PAYLOD_TYPE_LIVE)
					{
						command = gsmLivePayload.data[gsmLivePayload.tail];
					}
					else if(g_u8PayloadType == PAYLOD_TYPE_BUFF)
					{
						command = gsmBuffPayload.data[gsmBuffPayload.tail];
					}

					if((command == NULL)||(strlen(command) < 20 ))
					{
						command = &g_u8PaylodLenMsgBuff[0];
					}
#elif  (DATA_PROCESSING_METHOD == LIFO)
					command = gstrGMSPayloadLIFO.au8PayloadStack[gstrGMSPayloadLIFO.u32Stacktop];
#endif

				}
				else if(gsmInstance.enmGSMCommand == enmGSMSTATE_MQTTPAYLOADDATA)
				{
					if(g_u8PayloadType == PAYLOD_TYPE_LIVE)
					{
						command = gsmLivePayload.data[gsmLivePayload.tail];
					}
					else if(g_u8PayloadType == PAYLOD_TYPE_BUFF)
					{
						command = gsmBuffPayload.data[gsmBuffPayload.tail];
					}

					if((command == NULL)||(strlen(command) < 20 ))
					{
						command = &g_u8PaylodLenMsgBuff[0];
					}
				}

				else
					//memset(command,0x00,sizeof(command));
					command = gsmStateTableArray[gsmInstance.enmGSMCommand].atCommand;
#if(DEBUGENABLE == TRUE)
					 getrtcStamp();
					 strcat(DebugBuff,gau8Date);
					 strcat(DebugBuff,"/");
					 strcat(DebugBuff,gau8Month);
					 strcat(DebugBuff,"/");
					 strcat(DebugBuff,gau8Year);
					 strcat(DebugBuff," ");
					 strcat(DebugBuff,gau8Hour);
					 strcat(DebugBuff,":");
					 strcat(DebugBuff,gau8Minutes);
					 strcat(DebugBuff,":");
					 strcat(DebugBuff,gau8Seconds);
			         strcat(DebugBuff,"\r\nGSM_CMD :");
			         strcat(DebugBuff,command);
#endif							  
				u8CharacterCounter = strlen((const char *)command);
				LL_USART_TransmitData8(USART1,command[u8LoopCounter++]);
				gsmInstance.enmGSMCommandState = enmGSM_CMDINPROCESS;

			}
			else if(gsmInstance.enmGSMCommandState == enmGSM_CMDINPROCESS)
			{
				if(!LL_USART_IsActiveFlag_TXE(USART1))
				{
					/*Do Nothing . Wait For Previous Character Transmission */
				}
				else
				{
					if(u8LoopCounter < (u8CharacterCounter))
					{
						LL_USART_TransmitData8(USART1,command[u8LoopCounter++]);
					}
					else
					{
						u8LoopCounter = 0;
						gu32GSMCharacterTimeout = FIVEHUNDRED_MS;
						u8CharacterCounter = 0;
						gsmInstance.enmGSMCommandResponseState = enmGSM_CHKRESPONSE;
						gsmInstance.u32GSMResponseTimer = gsmStateTableArray[gsmInstance.enmGSMCommand].msTimeOut;

						if((gsmInstance.enmGSMCommand == enmGSMSTATE_FOTAHTTPACTION) ||(gsmInstance.enmGSMCommand == enmGSMSTATE_ATQHTTPGETEX) || (gsmInstance.enmGSMCommand == enmGSMSTATE_READFILE))
							gu32FotaFileReadTimer = FIFTEEN_SEC;
						else
							gu32FotaFileReadTimer = 0;
					}
				}
			}
			else
			{
				initGSMSIM868();
			}

			break;

			/*if(gsmInstance.enmGSMCommand == enmGSMSTATE_FOTAHTTPACTION )*/
		case enmGSM_CHKRESPONSE:
			if((gu32GSMCharacterTimeout == 0) && (gsmInstance.u32GSMResponseTimer != 0) && (u8GSMCharRcv == 1) && (gu32FotaFileReadTimer == 0))
			{
#if(DEBUGENABLE == TRUE)
					strcat(DebugBuff,"\r\nGSM_Response : ");
					strcat(DebugBuff,(const char *)gsmInstance.as8GSM_Response_Buff);
#endif
				const char *SOF = strstr((const char *)gsmInstance.as8GSM_Response_Buff, "$,");
				if(SOF != NULL)
				{	/* SOF Found */
					const char *EOF = strstr((const char *)gsmInstance.as8GSM_Response_Buff, ",@");
					if(EOF != NULL)
					{
						if(strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)dinfo) != NULL)
						{
							const size_t mlen = EOF - SOF;
							memset(gau8SUBRequest,0x00,sizeof(gau8SUBRequest));
							memcpy(gau8SUBRequest,SOF-1, (mlen+3));
							SUBTriggerFlag = TRUE;
						}
						else
							SUBTriggerFlag = FALSE;
					}
					else
						SUBTriggerFlag = FALSE;
				}
				else
					SUBTriggerFlag = FALSE;

				/* Parse Response */
				if(strstr((const char *)gsmInstance.as8GSM_Response_Buff
						,(const char *)gsmStateTableArray[gsmInstance.enmGSMCommand].atCommandResponse) != NULL)
				{
#if(DEBUGENABLE == TRUE)
					strcat(DebugBuff,"\r\nGSM_Response : ");
					strcat(DebugBuff,(const char *)gsmInstance.as8GSM_Response_Buff);
#endif																	 
                    /*Required Response Received */
					switch(gsmInstance.enmGSMCommand)
					{

						case enmGSMSTATE_AT: //OK
							gsmInstance.u8IncrementGsmState = TRUE;
							break;


						case enmGSMSTATE_ATE0: //OK
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GETTIMESTAMP: //OK
						/* Parse and update local RTC . Add Flag check for update
						 * +CCLK: "18/06/21,12:00:21+22" */
						{
							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
											,(const char *)"+QLTS:");
							memset(gau8GSM_TimeStamp,0x00,(sizeof(char ) * strlen(gau8GSM_TimeStamp)));
							memcpy( gau8GSM_TimeStamp, &strLoc[10], strlen(strtok(&strLoc[10],"\0")));
							syncrtcwithNetworkTime();
							gsmInstance.u8IncrementGsmState = TRUE;
						}
						break;


						case enmGSMSTATE_ATCPIN://OK
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enumGSMSTATE_IMEI:
							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"+GSN:");
								memset(gau8GSM_IMEI,0x00,(sizeof(char ) * strlen(gau8GSM_IMEI)));
								memcpy( gau8GSM_IMEI, &strLoc[7], strlen(strtok(&strLoc[7],"\"\r\n")));
								cu32IMEI = (atoll(gau8GSM_IMEI));
								gsmInstance.u8IncrementGsmState = TRUE;
							}
								break;

						case enmGSMSTATE_ATCSQ: //OK
							{
							/* Store Signal Strength . Bug 19012019 Cut Copy-Solved	 */
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
									,(const char *)"+CSQ");
								memcpy(gsmInstance.agsmSignalStrength, &strLoc[6], strlen(strtok(strLoc,"\r")));
								//Log9
								memcpy(gau8GSM_Signal,gsmInstance.agsmSignalStrength,2);
								signal_qaulity = atoi(gau8GSM_Signal);

								if(gsmInstance.enmcurrentTask == enmGSMTASK_ISALIVE)
								{
									gsmInstance.u32GSMTimer = FIVE_SEC;
									gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
									gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
									gsmInstance.u8IncrementGsmState = FALSE;
								}
								else
									gsmInstance.u8IncrementGsmState = TRUE;
							}
							break;

						case enmGSMSTATE_CREG:
							if(strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"+CREG:") != NULL)
							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																	,(const char *)",");
								if(strLoc[1] == '0')
								{
									/* No network */
//									TelematicHeartBeat[enumTeleHearBeat_SIM_CARD_ACTIVE_STATUS] = 0x00;
									initGSMSIM868();
									gu8CheckSMS = TRUE;
								}
								else if((strLoc[1] == '1')||(strLoc[1] == '5'))
								{
//									TelematicHeartBeat[enumTeleHearBeat_SIM_CARD_ACTIVE_STATUS] = 0x01;
								}
								else
								{
//									TelematicHeartBeat[enumTeleHearBeat_SIM_CARD_ACTIVE_STATUS] = 0x00;
								}
							}

							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enumGSMSTATE_QNWINFO:
							{
								char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"+QNWINFO:");
								memset(gau8GSM_NWINFO,0x00,sizeof(gau8GSM_NWINFO));
								memcpy(gau8GSM_NWINFO, &strLoc[10], strlen(strtok(strLoc,",")));
								//Log9

								gsmInstance.u8IncrementGsmState = TRUE;
							}
								break;
						case enmGSMSTATE_CMEE:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSBR:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_QCONTEXTID:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_QRESPONSEHDR:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_QRESQUESTHDR:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_QICSGPAPN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_QIACT:
							gsmInstance.u8IncrementGsmState = TRUE;

						case enmGSMSTATE_QIACT2:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;


						case enmGSMSTATE_UARTGPS:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;
						case enmGSMSTATE_GPSON:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_GPSNMEA:
						{
							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"$GNRMC");
							memset(gsmInstance.agpsLocationData,0x00, sizeof(gsmInstance.agpsLocationData));
							if(strLoc != NULL)
							{
								memcpy(gsmInstance.agpsLocationData, strLoc, strlen(strtok(strLoc,"\r\n")));

							}//end of if(strLoc != NULL)
							if(gu32ModuleInitComplete == 1)
							{

								if((gu32FotaRquestFlag == TRUE) && (Queue_IsLiveQueueEmpty(&gsmLivePayload) == 1))
								{

									UrlLenforFOTA();
									gsmInstance.enmGSMCommand = enmGSMSTATE_QFOTAURLLEN;
									gsmInstance.enmcurrentTask = enmGSMTASK_DOWNLOADFOTAFILE;
									SUBTriggerFlag = FALSE;
								}
								else{
									gsmInstance.enmGSMCommand = enmGSMSTATE_ATCSQ;
									gsmInstance.enmcurrentTask = enmGSMTASK_ISALIVE;
								}

								gsmInstance.u8IncrementGsmState = FALSE;
							}
							else if(gu32ModuleInitComplete == 0)
							{
								gsmInstance.u8IncrementGsmState = TRUE;
								gu32ModuleInitComplete = 1;
							}

						}
						break;

						case enmGSMSTATE_MQTTPUBEX:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;
						case enmGSMSTATE_ATHTTPDATA:
						{
							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)": ");
							memcpy(gsmInstance.u32GSMHttpResponseCode, (char *)&strLoc[4], 3);
							gsmInstance.u8IncrementGsmState = TRUE;
						}
							break;

						case enmGSMSTATE_SENDDATA:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;
						case enmGSMSTATE_HTTPACTION:

							/* Data Uploaded Successfully . Upload Next data packet */
							/* Process HTTP Response code */
							{
							char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
																			,(const char *)": ");
							memcpy(gsmInstance.u32GSMHttpResponseCode, (char *)&strLoc[4], 3);
							/* Parse HTTP Response Code */
							switch(atoi(gsmInstance.u32GSMHttpResponseCode))
							{
								case 200:
								/* HTTP Request Successful . Send Next Packet */

#if (DATA_PROCESSING_METHOD == FIFO )
									if(g_u8PayloadType == PAYLOD_TYPE_LIVE)
									{
										Queue_LiveDequeue(&gsmLivePayload);
									}
									else if(g_u8PayloadType == PAYLOD_TYPE_BUFF)
									{
										Queue_BuffDequeue(&gsmBuffPayload);
									}
									else
									{
//										  SerialImpDebug_Print((char*)"Payload Dequeue Invalid State\r\n");
									}
#elif (DATA_PROCESSING_METHOD == LIFO)
									popDataFromStack(&gstrGMSPayloadLIFO);
#endif

								gsmInstance.u32GSMTimer = TWO_SEC; // Replace with Upload Frequency
								gu8HttpPayloadFlag = 0;
								gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
								gsmInstance.enmGSMCommand = enmGSMSTATE_ATHTTPDATA;
								gsmInstance.u8IncrementGsmState = FALSE;
								u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
								break;


								default:
									/* Log and Change State : Tested CSQ and DATA Upload in loop
									 * When Service is not responding with 200 maintain counter and reset*/
									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
									gsmInstance.u8IllegalHttpResponseCounter++;
									if(gsmInstance.u8IllegalHttpResponseCounter >= MAX_HTTP_ATTEMPTS)
									{
										/* Log and Reset the modem */
										initGSMSIM868();
										gu8CheckSMS = TRUE;
										break;
									}
									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
									gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
									gsmInstance.enmGSMCommand =enmGSMSTATE_ATHTTPDATA;
									gsmInstance.u32GSMResponseTimer = 0;
									gsmInstance.u8IncrementGsmState = FALSE;

								break;
								}
							}
							break;


						case enmGSMSTATE_MQTTCFG:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_MQTTEN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_MQTTCON:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_MQTTSUB:
							GSMInitCompleteFlag = TRUE;
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

						case enmGSMSTATE_MQTTPAYLOADDATA:
							/* Data Uploaded Successfully . Upload Next data packet */
							/* Process HTTP Response code */
							{
							uint32_t i =0;
							uint8_t cnt = 0;

							for(i=0;i<GSM_RESPONSE_ARRAY_SIZE && cnt<2;i++)
							{
								if(gsmInstance.as8GSM_Response_Buff[i] == '\0')
								{
									gsmInstance.as8GSM_Response_Buff[i]='!';
									cnt++;
								}
								else
								{
								   cnt=0;
								}
							}
//							char *token = strtok(gsmInstance.as8GSM_Response_Buff,"+");
							char *token = NULL;
							token = strtok(gsmInstance.as8GSM_Response_Buff,"+");
							while(token != NULL)
							{
////								HAL_IWDG_Refresh(&hiwdg);
//								HAL_GPIO_TogglePin(WD_FEED_uC_GPIO_Port, WD_FEED_uC_Pin);
								if(strncmp(token,"QMTPUBEX",strlen("QMTPUBEX"))==0)
								{
									memset(gsmInstance.u32GSMMQTTResponseCode,0x00,sizeof(gsmInstance.u32GSMMQTTResponseCode));
									memcpy(gsmInstance.u32GSMMQTTResponseCode, token, 13);
								}
								else if(strncmp(token,"CSQ",strlen("CSQ"))==0)
								{
									 memcpy(gsmInstance.agsmSignalStrength,&token[5],((strstr(token,"\r\n"))-(&token[5])));
//									memcpy(gsmInstance.agsmSignalStrength, &strLoc[6], strlen(strtok(strLoc,"\r")));
									memcpy(gau8GSM_Signal,gsmInstance.agsmSignalStrength,2);
									signal_qaulity = atoi(gau8GSM_Signal);
								}
								else if(strncmp(token,"QNWINFO",strlen("QNWINFO"))==0)
								{
									memset(gau8GSM_NWINFO,0x00,sizeof(gau8GSM_NWINFO));
//									memcpy(gau8GSM_NWINFO, &strLoc[11], strlen(strtok(strLoc,",")));								}
									memcpy(gau8GSM_NWINFO,&token[9],((strstr(token,","))-(&token[9])));
								  /* Identify network used(2G,LTE,5G) */
									if((strcmp(gau8GSM_NWINFO,"\"GSM\"") == 0) || (strcmp(gau8GSM_NWINFO,"\"GPRS\"") == 0) || (strcmp(gau8GSM_NWINFO,"\"EDGE\""))==0)
									{
										u8daignostic|=(1<<1); // 2G
									}
									else if((strcmp(gau8GSM_NWINFO,"\"TDD LTE\"")==0) || (strcmp(gau8GSM_NWINFO,"\"FDD LTE\""))==0)
									{
										u8daignostic|=(1<<2); // 4G
									}
									else
									{
										u8daignostic&=~((1<<0)|(1<<1)|(1<<2)); //
									}
								}
								else if(strncmp(token,"QGPSGNMEA",strlen("QGPSGNMEA"))==0)
								{
									strncpy(gsmInstance.agpsLocationData,&token[11],((strstr(token,"\r\n"))-(&token[11])));
//									memcpy(gsmInstance.agpsLocationData, strLoc, strlen(strtok(strLoc,"\r\n")));
								}
								token=strtok(NULL,"+");
							}//end of while(token !=NULL)
//							char * strLoc = NULL;
//							strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"+QMTPUBEX:");
//							memcpy(gsmInstance.u32GSMMQTTResponseCode, (char *)&strLoc[1], 13);
//							strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff ,(const char *)"+CSQ");
//							if(strLoc != NULL)
//							{
//								memcpy(gsmInstance.agsmSignalStrength, &strLoc[6], strlen(strtok(strLoc,"\r")));
//								memcpy(gau8GSM_Signal,gsmInstance.agsmSignalStrength,2);
//								signal_qaulity = atoi(gau8GSM_Signal);
//							}
//
//							strLoc = NULL;
//							strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"+");
//							if(strLoc != NULL)
//							{
//								/* Identify network used(2G,LTE,5G) */
//								memset(gau8GSM_NWINFO,0x00,sizeof(gau8GSM_NWINFO));
//								memcpy(gau8GSM_NWINFO, &strLoc[11], strlen(strtok(strLoc,",")));
//								if((strcmp(gau8GSM_NWINFO,"GSM")) || (strcmp(gau8GSM_NWINFO,"GPRS")) || (strcmp(gau8GSM_NWINFO,"EDGE")))
//								{
//									u8daignostic|=(1<<1); // 2G
//								}
//								else if((strcmp(gau8GSM_NWINFO,"TDD LTE")) || (strcmp(gau8GSM_NWINFO,"FDD LTE")))
//								{
//									u8daignostic|=(1<<2); // 4G
//								}
//								else
//								{
//									u8daignostic&=~((1<<0)|(1<<1)|(1<<2)); //
//								}
//							}
//
//							strLoc = NULL;
//							strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"$GNRMC");
//							memset(gsmInstance.agpsLocationData,0x00, sizeof(gsmInstance.agpsLocationData));
//							if(strLoc != NULL)
//							{
//								memcpy(gsmInstance.agpsLocationData, strLoc, strlen(strtok(strLoc,"\r\n")));
//
//							}//end of if(strLoc != NULL)

							/* Parse HTTP Response Code */
							if(!(strcmp(gsmInstance.u32GSMMQTTResponseCode,"QMTPUBEX: 0,0")))
								u8MqqtResponseFlag = TRUE;
//							switch(atoi(gsmInstance.u32GSMHttpResponseCode))
//							switch(atoi(gsmInstance.u32GSMMQTTResponseCode))
							switch(u8MqqtResponseFlag)
							{
								case 1:
								/* MQTT Request Successful . Send Next Packet */
									u8MqqtResponseFlag = FALSE;

									g_u8HttpConEstablishedFlag = 1;

									if(g_u8PayloadType == PAYLOD_TYPE_LIVE)
									{
										Queue_LiveDequeue(&gsmLivePayload);
									}
									else if(g_u8PayloadType == PAYLOD_TYPE_BUFF)
									{
										Queue_BuffDequeue(&gsmBuffPayload);
									}
									else
									{
//										  SerialImpDebug_Print((char*)"Payload Dequeue Invalid State\r\n");
									}


									if(g_u8PayloadType == PAYLOD_TYPE_BUFF)
									{
										/*	Writing SFlash Read address into EEPROM */
										if(FlashStr.bitFlagSFlashData == 1)
										{
											FlashStr.u8BufferQueueCounts--;
											if(FlashStr.u8BufferQueueCounts == 0)
											{
												FlashStr.bitFlagSFlashData = 0;
												g_stEepromBuffMemAdd.u32BuffFlashWriteAddress = FlashStr.u32FlashWriteAddress;
												g_stEepromBuffMemAdd.u32BuffFlashReadAddress =FlashStr.u32FlashReadAddress;
												EEPROM_WriteBuffMemData();
											}
										}
									}

									g_u8PayloadType = 0;

									//gsmInstance.u32GSMTimer = TWO_SEC; // Replace with Upload Frequency

								gu8HttpPayloadFlag = 0;

								gu8HttpPayloadFlag = 0;

								gsmInstance.u32GSMTimer = TWO_SEC; // Replace with Upload Frequency
								gu8HttpPayloadFlag = 0;
								gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
								gsmInstance.u8IncrementGsmState = FALSE;
//								u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
								u8LastMqttResponseCode = atoi(gsmInstance.u32GSMMQTTResponseCode);
								break;

								default:
								/* Log and Change State : Tested CSQ and DATA Upload in loop
								 * When Service is not responding with 200 maintain counter and reset*/
//								u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
									u8LastMqttResponseCode = atoi(gsmInstance.u32GSMMQTTResponseCode);
									gsmInstance.u8IllegalHttpResponseCounter++;
									if(gsmInstance.u8IllegalHttpResponseCounter >= MAX_HTTP_ATTEMPTS)
									{
									/* Log and Reset the modem */
										initGSMSIM868();
										gu8CheckSMS = TRUE;
										break;
									}
//									u8LastHttpResponseCode = atoi(gsmInstance.u32GSMHttpResponseCode);
									u8LastMqttResponseCode = atoi(gsmInstance.u32GSMMQTTResponseCode);
									gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
									gsmInstance.u32GSMResponseTimer = 0;
									gsmInstance.u8IncrementGsmState = FALSE;

									break;
							}
						}
						break;

						/* Fota */
						case enmGSMSTATE_QFOTAURLLEN:
							gsmInstance.u8IncrementGsmState = TRUE;
							break;

//
//						case enmGSMSTATE_SETFOTAURL:
//							gsmInstance.u8IncrementGsmState = TRUE;
//							break;
//
//						case enmGSMSTATE_FOTAHTTPACTION:
//							gsmInstance.u8IncrementGsmState = TRUE;
//							break;

						case enmGSMSTATE_FOTAHTTPACTION:
						{
						char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff
															,(const char *)"200,");
						if(strLoc != NULL)
						{

							memset(gu32FotaFileSizeinBytes,0x00, strlen(gu32FotaFileSizeinBytes));
							memcpy(gu32FotaFileSizeinBytes, &strLoc[4], strlen(strtok(&strLoc[4],"\r\n")));

							/*Extract Size bytes */
							u32FotaFileSizeInBytes = strlen(gu32FotaFileSizeinBytes);
							gsmInstance.u32FotaFileSizeBytes = atoi(gu32FotaFileSizeinBytes);
							/*
							 *
							 */
							if(gsmInstance.u32FotaFileSizeBytes <= (126000))// as Flash is of 128K
							{
								/* File Size is Valid */
								boolAttemptFota = TRUE;
								if(gsmInstance.u32FotaFileSizeBytes != 0)
									updateHTTPReadLength(gsmInstance.u32FotaFileSizeBytes);
								else
								{
									/*Error File Size is not valid */
									Diagnostic(enmDiagnostic_RX_FILE_SIZE_ERROR);
									Deinit_FOTA_Fail();
									break;
								}
							}
							else
							{
								/* Abort Fota / Raise Error / Continue with regular operation */
								gsmInstance.u8IncrementGsmState = FALSE;
								/* Raise size error and change state back to HTTP Upload */
								gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
								gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
								break;
							}
							gsmInstance.u8IncrementGsmState = TRUE;
						}
						else
						{
							/* Response Does not contain 200 . Read Failed . Issue Alart / Failure */
							gsmInstance.u8IncrementGsmState = FALSE;
							//gu32AttemptFota = FALSE;
							gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
							gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
						}
					}
						break;

					case enmGSMSTATE_ATQHTTPGETEX:
					{
						char * strLoc = strstr((const char *)gsmInstance.as8GSM_Response_Buff,(const char *)"206,");
						if(strLoc != NULL)
						{
							gsmInstance.u8IncrementGsmState = TRUE;
						}
						else{
							/* Response Does not contain 206 . Read Failed . Issue Alart / Failure */
							gsmInstance.u8IncrementGsmState = FALSE;
							//gu32AttemptFota = FALSE;
							gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
							gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;
						}
					}
							break;

					case enmGSMSTATE_READFILE:
							/* Read and Parse Received response and file
							 * +HTTPREAD: 800\r\n\r*/
						if(boolAttemptFota == TRUE)
						{
						/* Read File Chucks and write to Internal flash */
							HAL_GPIO_TogglePin(GPS_LED_GPIO_Port, GPS_LED_Pin);
							if(u32FotaFileReadComplete == 0)
							{
								memset(gau8FotaData,0x00,sizeof(gau8FotaData));
								memcpy(gau8FotaData,(const char *)&gsmInstance.as8GSM_Response_Buff[11],2000);
								/* More chuncks available */
								if(u32MemoryWriteCycle == FALSE)
								{
									/* Erase the Sector */
									FLASH_If_Init();

									if(FLASH_If_Erase(0,getNewFirmwareStorageLocation()) != FLASHIF_ERASE_OK)
									{
										/* Error Erasing Flash */
										Diagnostic(enmDiagnostic_FLASH_ERRASE_ERROR);
										Deinit_FOTA_Fail();
										break;
									}

									/* Write Received Chucnk to memory */
									u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,1);

									if(u32FlashMemoryWriteStatus == SUCCESS)
									{
										/* Memory Block Write Complete */
										u32MemoryWriteCycle = TRUE;
										updateHTTPReadLength(gsmInstance.u32FotaFileSizeBytes);
										memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t)));
										gsmInstance.enmGSMCommand = enmGSMSTATE_ATQHTTPGETEX;
									}
									else
									{
										/*Memory Write Failed . Raise Error and Back to HTTP Upload */
										Diagnostic(enmDiagnostic_FLASH_Write_ERROR);
										Deinit_FOTA_Fail();
										break;
									}
								}
								else
								{
									/* Write Next Chunck to Internal Flash */
									u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,0);

									if(u32FlashMemoryWriteStatus == SUCCESS)
									{
										/* Memory Block Write Complete */
										updateHTTPReadLength(gsmInstance.u32FotaFileSizeBytes);
										memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t)));
										gsmInstance.enmGSMCommand = enmGSMSTATE_ATQHTTPGETEX;
									}
									else
									{
										/*Memory Write Failed . Raise Error and Back to HTTP Upload */
										Diagnostic(enmDiagnostic_FLASH_Write_ERROR);
										Deinit_FOTA_Fail();
										break;
									}
								}
								/* Read the file */
								gsmInstance.u8IncrementGsmState = FALSE;
							}
							else if(u32FotaFileReadComplete == 1)
							{
								if(u32MemoryWriteCycle == FALSE)
								{
									/*
									 * USE CASE : File Size is Less than CHUNK SIZE = 2k
									 * */
									/* Initialise Flash Opr Flags . Erase required sector and write new FW */
									FLASH_If_Init();
									if(u32MemoryEraseStatus != FLASHIF_OK)
									{
										/* Error Erasing Flash */
									}
								}
								u32FlashMemoryWriteStatus = WriteDatatoFlash((uint8_t *)gau8FotaData,0);
								/* Memory Write Successful */
								if(u32FlashMemoryWriteStatus == SUCCESS)
								{
									/* Memory Block Write Complete */
									if(flashWriteBootSection(getNewFirmwareStorageLocation()) == 0)
									{
										/* Boot Info Updated Successfully*/
										boolAttemptFota =  FALSE;
										if(u32MemoryWriteCycle == TRUE)
											u32MemoryWriteCycle = FALSE;

										Diagnostic(enmDiagnostic_SUCCESS_OK);
										boolFOTACompleteFlag = TRUE;
										Deinit_FOTA_Fail();
									}
									else
									{
										/*Boot Info Updation Failed */
										Diagnostic(enmDiagnostic_FLASH_Write_ERROR);
										Deinit_FOTA_Fail();
										break;
									}
								}
								else
								{
									/*Memory Write Failed . Raise Error and Back to HTTP Upload */
									Diagnostic(enmDiagnostic_FLASH_Write_ERROR);
									Deinit_FOTA_Fail();
									break;
								}

								/* Write Chunk to memory . Checksum check and Application Jump.*/
								u32MemoryWriteCycle = TRUE;

							}
							else
							{
								/* Http Upload */
							}
						}//end of if(boolAttemptFota == TRUE)
						else
						{
							/*Continue HTTP/MQTT Upload */
							HAL_GPIO_WritePin(GPS_LED_GPIO_Port, GPS_LED_Pin, GPIO_PIN_RESET);
						}
					break;



						default:
							HAL_GPIO_WritePin(GPS_LED_GPIO_Port, GPS_LED_Pin, GPIO_PIN_RESET);
							//memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand], (char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
							break;
					}//end of switch(gsmInstance.enmGSMCommand)

					gsmInstance.u8GSM_Response_Character_Counter = 0;
					gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
					gsmInstance.u8gsmRetryCount = GSM_MAX_RETRY;
					gsmInstance.u32GSMResponseTimer = 0;
					u8GSMCharRcv = 0;
					gsmInstance.u8GSM_Response_Character_Counter = 0;
					if(gsmInstance.enmGSMCommand != enmGSMSTATE_READFILE)
						memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t))); /* Clear Response Buffer */

					if(gsmInstance.u8IncrementGsmState == TRUE)
					{
							gsmInstance.u32GSMTimer = ONE_SEC;
						gsmInstance.enmGSMCommand++;

					}
				}
				else
				{
					/* Response not found : Try Again */
					gsmInstance.u8GSM_Response_Character_Counter = 0;
					//gsmInstance.enmGSMCommandState = enmGSM_CMDRESPONSEERROR;
					/* 1-4-2020 */
					//gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_CMDSEND;//enmGSM_SENDCMD;
				//	memcpy(gsmInstance.agsmCommandResponse[gsmInstance.enmGSMCommand], (char *)&gsmInstance.as8GSM_Response_Buff, sizeof(gsmInstance.as8GSM_Response_Buff));
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR,(GSM_RESPONSE_ARRAY_SIZE * sizeof(uint8_t))); /* Clear Response Buffer */
					u8GSMCharRcv = 0;
				}
			}// end of if((gu32GSMCharacterTimeout == 0) && (gsmInstance.u32GSMResponseTimer != 0) && (u8GSMCharRcv == 1))
			else if(gsmInstance.u32GSMResponseTimer == 0)
			{
				/* Time Out */
				gsmInstance.u8gsmRetryCount--;
				if(gsmInstance.u8gsmRetryCount == 0)
				{
					/* Max Retry Attempt Reached Yet No Response . Reset the modem */
					/* Clear Response Buffer */
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
					if(gsmInstance.enmcurrentTask == enmGSMTASK_UPLOADDATA)
					{
						/* Check SMS even if upload data was not successful */
						gu8CheckSMS = TRUE;
					}
					initGSMSIM868();
				}
				else
				{
					/*Send Same Command Again */
					gsmInstance.enmGSMCommandState = enmGSM_CMDSEND;
					gsmInstance.enmGSMCommandResponseState = enmGSM_SENDCMD;
					/* Clear Response Buffer */
					memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
					gsmInstance.u32GSMTimer = ONE_SEC;
				}
				gsmInstance.u32GSMResponseTimer = 0;
				u8GSMCharRcv = 0;
			}

			break;
	}
}

/******************************************************************************
* Function : updateHttpDataLength()
*//**
* \b Description:
*
* This function is updates payload data length form HTTP upload.
* Configure data length and timeout for gsm data
  gau8GSM_ATHTTPDATA = datalength,timeout\r\n;
*
* PRE-CONDITION: None .
*
* POST-CONDITION: Payload length is updated in HTTP param
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updateHttpDataLength();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* *
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updateHttpDataLength()
{
	char* pDignosticPacket = NULL;
	if((Queue_IsLiveQueueEmpty(&gsmLivePayload) == FALSE )&& (gsmLivePayload.data[gsmLivePayload.tail] != NULL))
		{
			g_u8PayloadType = PAYLOD_TYPE_LIVE;
			tempdata = gsmLivePayload.data[gsmLivePayload.tail];
		}
		else if((Queue_IsBuffQueueEmpty(&gsmBuffPayload) == FALSE )&& (gsmBuffPayload.data[gsmBuffPayload.tail] != NULL))
		{
			g_u8PayloadType = PAYLOD_TYPE_BUFF;
			tempdata = gsmBuffPayload.data[gsmBuffPayload.tail];
		}

		uint32_t payloadLength;
		//= strlen(tempdata);
		payloadLength = strlen(tempdata);
		if(payloadLength == 0 )
		{
			payloadLength = strlen(g_u8PaylodLenMsgBuff);/*PAYLOAD LENGTH MISSMATCHED*/
		}


//		char buffer[payloadLength];
		char PUBTopic[15] = "";
		char *strLoc = NULL,*strLoc1 = NULL;
		uint8_t len = 0;
		memset(buffer, 0x00, (6 * sizeof(char))); /* Clear Response Buffer */
		/*MQTT*/

		memset(gau8GSM_ATQMTPUBEX, 0x00, ( 250 * sizeof(char)));
		itoa(payloadLength,buffer,PAYLOAD_DATA_STRING_RADIX);
		strcpy((char *)gau8GSM_ATQMTPUBEX,"AT+QMTPUBEX=0,0,0,0,");

		strcat((char *)gau8GSM_ATQMTPUBEX,(char *)"\"");
		strLoc = strstr(tempdata,"_");
		strLoc1 = strstr(strLoc+1,",");
		len = strLoc1-strLoc;
		memset(PUBTopic,0x00,sizeof(PUBTopic));
		memcpy(PUBTopic,(strLoc+1),(len-1));


		if(g_u8PayloadType == PAYLOD_TYPE_LIVE)
		{
//			strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBPayload);
			if(!strcmp(PUBTopic,"Diagnostic"))
			{
				strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBDiagnostic);
			}
			else if(!strcmp(PUBTopic,"Payload"))
			{
				strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBPayload);
			}
			else if(!strcmp(PUBTopic,"Geo"))
			{
				strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBGeo);
			}
			else
			{
				strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBPayload);
			}
		}
		else if(g_u8PayloadType == PAYLOD_TYPE_BUFF)
		{
			/**/
			pDignosticPacket = NULL;
			pDignosticPacket = strstr(tempdata,"Diagnostic");
			if(pDignosticPacket == NULL)
			{
				strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBBuffer);
			}
			else
			{
				strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBDiagnostic);
			}
			//strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBBuffer);
		}
		else
		{
			/*This will never happens but this is for saftey purpose.*/
			strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBPayload);
		}


//		if(!strcmp(PUBTopic,"Diagnostic"))
//		{
//			strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBDiagnostic);
//		}
//
//		else if(!strcmp(PUBTopic,"Geo"))
//		{
//			strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBGeo);
//		}
//		else if(!strcmp(PUBTopic,"Payload"))
//		{
//			strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBPayload);
//		}
//		else
//		{
//			strcat((char *)gau8GSM_ATQMTPUBEX,(char *)MQTT_PUBPayload);
//		}

		strcat((char *)gau8GSM_ATQMTPUBEX,(char *)"\",");
	//	strcpy((char *)gau8GSM_ATQMTPUBEX,"AT+QMTPUBEX=0,0,0,0,"<PUBTOPIC>",");

		strcat((char *)gau8GSM_ATQMTPUBEX,buffer); // lenght of payload
		strcat((char *)gau8GSM_ATQMTPUBEX,";+QGPSGNMEA=\"RMC\";+CSQ;+QNWINFO");//
		strcat((char *)gau8GSM_ATQMTPUBEX,"\r\n");

}

/******************************************************************************
* Function : sendSystemConfigurationSMS()
*//**
* \b Description:
*
* This function is updates SMS data in the SMS structure .
*
* PRE-CONDITION: None .
*
* POST-CONDITION: SMS is ready as per format for sending
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	sendSystemConfigurationSMS();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> HTTP Code of latest request added </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> Redundant code reduced (UUID)</td></tr>
*
* </table><br><br>
* <hr>
*
*******************************************************************************/
//void sendSystemConfigurationSMS(void)
//{
//	/* Tor Signature */
//	strcpy(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)KLOUDQ_SIGNATURE);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Config: \r\n");
//	/*Tor Version */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Version: \r\n");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)MODEL_NUMBER);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Device Id */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Id: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)dinfo);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Signal Strength in RSSI */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"RSSI: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agsmSignalStrength);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Network IP , if connected */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"IP: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agsmNetworkIP);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Network APN */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"APN: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gau8GSM_apn);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	/* Tor Server URL */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"URL: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gau8GSM_url);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//
//	/* Tor Upload Frequency */
//	char ontime[10];
//	char offtime[10];
//	itoa(gsmInstance.u32ONPayloadUploadFreq,ontime,10);
//	itoa(gsmInstance.u32OFFPayloadUploadFreq,offtime,10);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Up Freq ON: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,ontime);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Up Freq OFF: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,offtime);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//
//	/* Last Known Location and time */
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"Location: ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,gsmInstance.agpsLocationData);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\r\n");
//
//	/*Last HTTP Status Code */
//	char httpresp[5];
//	itoa(u8LastHttpResponseCode,httpresp,10);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,(char *)"HTTP Code : ");
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,httpresp);
//	strcat(gsmInstance.strSystemSMS.agsmSMSMessageBody,"\n");
//}

/******************************************************************************
* Function : updatePhoneNumber()
*//**
* \b Description:
*
* This function is used to updates phone number to send SMS .
*
* PRE-CONDITION: None .
*
* POST-CONDITION: Phone number is updated in SMS structure
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	updatePhoneNumber();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
**
* </table><br><br>
* <hr>
*
*******************************************************************************/
void updatePhoneNumber(void)
{
//	memset(gau8GSM_smsto,0x00,sizeof(char) * 15);
//	memcpy(gau8GSM_smsto, (char *)&gsmInstance.as8GSM_Response_Buff[25]
//		,(strlen(strtok((char *)&gsmInstance.as8GSM_Response_Buff[26],","))));
//	memset(gau8GSM_SMSRecepient, 0x00, ( 180 * sizeof(char)));
////	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_ATCMGS);
//	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
//	strcat((char *)gau8GSM_SMSRecepient,(char *)gau8GSM_smsto);
//	strcat((char *)gau8GSM_SMSRecepient,(char *)"\"");
//	strcat(gau8GSM_SMSRecepient,"\r\n");
//	sendSystemConfigurationSMS();
//	gu8SendSMS = TRUE;
}

/******************************************************************************
* Function : syncrtcwithNetworkTime()
*//**
* \b Description:
*
* This function is used to synchronize internal RTC with server / GPS time .
* Updates RTC Structure values with network time
  Network Time format : "yy/MM/dd,hh:mm:ss ± zz"
  zz - time zone
  (indicates the difference, expressed in quarters of an hour, between the
   local time and GMT; range -47...+48)

   E.g. 6th of May 2010, 00:01:52
   GMT+2 hours equals to "10/05/06,00:01:52+08".
*
* PRE-CONDITION: Initialize the internal RTC module .
*
* POST-CONDITION: Internal RTC is updated with server/gps time
*
*
* @return 		None.
*
* \b Example Example:
* @code
*
* 	syncrtcwithNetworkTime();
*
* @endcode
*
* @see
*
* <br><b> - HISTORY OF CHANGES - </b>
*
* <table align="left" style="width:800px">
* <tr><td> Date       </td><td> Software Version </td><td> Initials </td><td> Description </td></tr>
* <tr><td> 01/06/2019 </td><td> 0.0.1            </td><td> HL100133 </td><td> Interface Created </td></tr>
**
* </table><br><br>
* <hr>
*
*******************************************************************************/
uint32_t gu32Year = 0;
uint8_t gu8YY = 0;
uint32_t gu32Month = 0;
uint32_t gu32Date = 0;
uint32_t gu32Hours = 0;
uint32_t gu32Minutes = 0;
uint32_t gu32Seconds = 0;
void syncrtcwithNetworkTime(void)
{
	if(gu32TimeSyncFlag == 1)
		return;

	//	gu8YY = ((gau8GSM_TimeStamp[0]-'0')*10)+(gau8GSM_TimeStamp[1]-'0');
	////	gu32Year = (((gau8GSM_TimeStamp[0]-'0') * 1000) + ((gau8GSM_TimeStamp[1]-'0')*100)+((gau8GSM_TimeStamp[2]-'0')*10))+(gau8GSM_TimeStamp[3]-'0');
	//	gu32Year = ((gau8GSM_TimeStamp[2]-'0')*10)+(gau8GSM_TimeStamp[3]-'0');
	//	gu32Month = (((gau8GSM_TimeStamp[5]-'0') * 10) + (gau8GSM_TimeStamp[6]-'0'));
	//	gu32Date = (((gau8GSM_TimeStamp[8]-'0') * 10) + (gau8GSM_TimeStamp[9]-'0'));
	//
	//	gu32Hours = (((gau8GSM_TimeStamp[11]-'0') * 10) + (gau8GSM_TimeStamp[12]-'0'));
	//	gu32Minutes = (((gau8GSM_TimeStamp[14]-'0') * 10) + (gau8GSM_TimeStamp[15]-'0'));
	//	gu32Seconds = (((gau8GSM_TimeStamp[17]-'0') * 10) + (gau8GSM_TimeStamp[18]-'0'));

		gu8YY = ((gau8GSM_TimeStamp[0]-'0')*10)+(gau8GSM_TimeStamp[1]-'0');
	//	gu32Year = (((gau8GSM_TimeStamp[0]-'0') * 1000) + ((gau8GSM_TimeStamp[1]-'0')*100)+((gau8GSM_TimeStamp[2]-'0')*10))+(gau8GSM_TimeStamp[3]-'0');
		gu32Year = ((gau8GSM_TimeStamp[0]-'0')*10)+(gau8GSM_TimeStamp[1]-'0');
		gu32Month = (((gau8GSM_TimeStamp[3]-'0') * 10) + (gau8GSM_TimeStamp[4]-'0'));
		gu32Date = (((gau8GSM_TimeStamp[6]-'0') * 10) + (gau8GSM_TimeStamp[7]-'0'));

		gu32Hours = (((gau8GSM_TimeStamp[9]-'0') * 10) + (gau8GSM_TimeStamp[10]-'0'));
		gu32Minutes = (((gau8GSM_TimeStamp[12]-'0') * 10) + (gau8GSM_TimeStamp[13]-'0'));
		gu32Seconds = (((gau8GSM_TimeStamp[15]-'0') * 10) + (gau8GSM_TimeStamp[16]-'0'));

		/* Add 5 hr 30 Mins to get IST */

		SDate1.Year = DecimalToBCD(gu32Year);
		SDate1.Month = DecimalToBCD(gu32Month);
		SDate1.Date = DecimalToBCD(gu32Date);
		STime1.Hours = DecimalToBCD(gu32Hours);
		STime1.Minutes = DecimalToBCD(gu32Minutes);
		STime1.Seconds = DecimalToBCD(gu32Seconds);

		/*Store Last Syncd date */
		lastKnownSDate1 = SDate1 ;

		strTimeUpdate.u32RefTimeHH = gu32Hours;
		strTimeUpdate.u32RefTimeMin = gu32Minutes;
		strTimeUpdate.u32RefTimeSec = gu32Seconds;

		HAL_RTC_SetTime(&hrtc,&STime1,RTC_FORMAT_BCD);
		HAL_RTC_SetDate(&hrtc,&SDate1,RTC_FORMAT_BCD);
		/* Update/ Set RTC Structure */
		//backupCurrentRTC();
		gu32TimeSyncFlag = 1;
	//	gu32RTCTestFlag++;
}

/****************************************************************************
 Function updateHTTPReadLength
 Purpose: Read Next chunk of 2000 Bytes of FOTA File
 Input:	uint32_t fotaFileSizeBytes - File Size in Bytes .
 Return value: uint32_t status 0 - Null File
 	 	 	 	 	 	 	   1 - File Download Complete
 	 	 	 	 	 	 	   2 - File Download In Process

 Note(s)(if-any) :

	AT+HTTPREAD = <start_address> , <byte_size> \r\n
    Test Fota File size is 29630 Bytes .
	(fotaFileSizeBytes / 2000) + 1 = (29630/2000) + 1 = (14.8) + 1 = 15
	interations/read to get complete file



 Change History:
 Author           	Date                Remarks
 KloudQ Team      8-04-2020			Initial Definition
 Kloudq Team	  10-04-2020		Strcpy logic added
******************************************************************************/
#define FOTAFILECHUNKSIZEBYTES	(2000)
uint32_t updateHTTPReadLength(uint32_t fotaFileSizeBytes)
{

	if(fotaFileSizeBytes != 0)
	{

		/* Start of File */
		if(u32FOTAFileBaseAddress == 0)
		{
			u32FotaFileChunkCounter = (fotaFileSizeBytes / FOTAFILECHUNKSIZEBYTES);
			/* Calculates Remaining data bytes after multiples of 2000  */
			u32FotaFileRemainingBytes = (fotaFileSizeBytes % FOTAFILECHUNKSIZEBYTES);
			itoa(FOTAFILECHUNKSIZEBYTES,bufferChunkBytes,PAYLOAD_DATA_STRING_RADIX);
			itoa(u32FotaFileRemainingBytes,bufferRemBytes,PAYLOAD_DATA_STRING_RADIX);
		}

		strcpy(gau8GSM_ATQHTTPGETEXREAD,(char *)"AT+QHTTPGETEX=80,");
		memset(buffer,0x00,sizeof(char) * sizeof(buffer));

		if(u32FOTAFileBaseAddress < u32FotaFileChunkCounter)
		{
			itoa((u32FOTAFileBaseAddress * FOTAFILECHUNKSIZEBYTES),buffer,PAYLOAD_DATA_STRING_RADIX);
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)buffer);
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)",");
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)bufferChunkBytes); /* Byte(s) Chunk to read*/
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)"\r\n");
			u32FOTAFileBaseAddress++;
			gsmInstance.u32FotaFileSizeBytes = FOTAFILECHUNKSIZEBYTES;
			return 2;
		}
		else
		{
			itoa(u32FotaFileChunkCounter * FOTAFILECHUNKSIZEBYTES,buffer,PAYLOAD_DATA_STRING_RADIX);
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)buffer);
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)",");
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)bufferRemBytes); /* Byte(s) Chunk to read*/
			strcat(gau8GSM_ATQHTTPGETEXREAD,(char *)"\r\n");
			gsmInstance.u32FotaFileSizeBytes = u32FotaFileRemainingBytes;
			u32FOTAFileBaseAddress = 0;
			u32FotaFileChunkCounter = 0;
			u32FotaFileRemainingBytes = 0;
			u32FotaFileReadComplete = 1;
			return 1;
		}
	}
	else
		return 0;
}

/****************************************************************************
 Function initHTTPURLforFOTA
 Purpose: Update HTTP URL for Fota
 Input:	None.
 Return value: None.

 Note(s)(if-any) :
	ToDo : Add functioonality to update Fota URL through SMS

 Change History:
 Author           	Date                Remarks
 KloudQ Team      31-03-2020			Initial Definitions
******************************************************************************/
void initHTTPURLforFOTA(void )
{
//	strcpy((char *)gau8GSM_ATURL,(char *)gau8GSM_ATHTTPPARAURL);
//	strcat((char *)gau8GSM_ATURL,(char *)"\"");
//	strcat((char *)gau8GSM_ATURL,(char *)gau8FotaURL);
//	strcat((char *)gau8GSM_ATURL,(char *)"\"");
//	strcat((char *)gau8GSM_ATURL,"\r\n");
}

void UrlLenforFOTA(void)
{
	uint32_t temp_len = strlen(gau8FotaURL);
	strcat((char*)gau8GSM_ATAPNL,(char *)gau8GSM_ATQHTTPURLLEN);
	itoa(temp_len,buffer,PAYLOAD_DATA_STRING_RADIX);
	strcat((char*)gau8GSM_ATAPNL,(char *)buffer);
	strcat((char*)gau8GSM_ATAPNL,",80\r\n"); // time out

}


void Deinit_FOTA_Fail()
{
	memset((char *)gsmInstance.as8GSM_Response_Buff, GSM_ARRAY_INIT_CHAR, (GSM_RESPONSE_ARRAY_SIZE));
	HAL_GPIO_WritePin(GPS_LED_GPIO_Port, GPS_LED_Pin, GPIO_PIN_RESET);
	u32FOTAFileBaseAddress = 0;
	u32FotaFileChunkCounter = 0;
	u32FotaFileRemainingBytes = 0;
	SUBTriggerFlag = FALSE;
	boolAttemptFota = FALSE;
	u32MemoryWriteCycle = FALSE;
	gsmInstance.u8IncrementGsmState = FALSE;
	gsmInstance.enmcurrentTask = enmGSMTASK_UPLOADDATA;
	gsmInstance.enmGSMCommand = enmGSMSTATE_MQTTPUBEX;

}

/*************** END OF FUNCTIONS ***************************************************************************/


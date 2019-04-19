/*HEADER****************************************************************
 ************************************************************************
 ***
 ***	 Copyright (c) 2018 HARMAN CONNECTED SERVICES
 ***	 All rights reserved
 ***
 ***	This software embodies materials and concepts which are
 ***	confidential to HARMAN CONNECTED SERVICES and is made available solely
 ***	pursuant to the terms of a written license agreement with HARMAN CONNECTED SERVICES
 ***
 ***	Designed and Developed by Harman Connected Services for Hindustan Unilever
 ****
 *** File: <mqtt.c>
 ***
 *** Comments: Implementation of MQTT messages to an dfrom cloud
 *** Comments about the file.
 *** - Algorithm / logic explanation
 *** - formula explanation
 *** - inputs and outputs of this file
 *** - etc etc.
 ***
 ***
 ************************************************************************
 *END*******************************************************************/

#include "common_vars.h"
#include "Timer.h"
#include "ipc.h"

//MQTT Payload data
static const char       MQTT_PAYLOAD_PWR[]      =       "PWR";
static const char       MQTT_PAYLOAD_FW_VER[]   =       "FIRMWARE_VER";
static const char       MQTT_PAYLOAD_FW_TS[]    =       "FIRMWARE_TS";
static const char       MQTT_PAYLOAD_CONN[]     =       "CONN";
static const char       MQTT_PAYLOAD_CONL[]     =       "CONL";
static const char       MQTT_PAYLOAD_ZBAL[]     =       "BAL_Z";
static const char       MQTT_PAYLOAD_LBAL[]     =       "BAL_L";
static const char       MQTT_PAYLOAD_BAL[]      =       "BAL";
static const char       MQTT_PAYLOAD_FLOW_RATE[]=       "FLOW_RATE";
static const char       MQTT_PAYLOAD_TDS_IN[]   =       "TDS_IN";
static const char       MQTT_PAYLOAD_TDS_OUT[]  =       "TDS_OUT";
static const char       MQTT_PAYLOAD_PUMP_CURRENT[]=    "PUMP_CURRENT";
static const char       MQTT_PAYLOAD_LOW_FLOW[]  =      "LOW_FLOW";
static const char       MQTT_PAYLOAD_EMERROR[]  =       "EM_ERR";
static const char       MQTT_PAYLOAD_TAMPER[]   =       "TAMPER";
static const char       MQTT_PAYLOAD_RECHARGE[] =       "RC_BAL";
static const char       MQTT_PAYLOAD_RC_OK[]    =       "RC_OK";
static const char       MQTT_PAYLOAD_RC_FAIL[]  =       "RC_FAIL";
static const char       MQTT_PAYLOAD_PST_TS[]   =       "PST_TS";
static const char       MQTT_PAYLOAD_PSTP_TS[]  =       "PSTP_TS";
static const char       MQTT_PAYLOAD_VOLUME[]   =       "VOL";
static const char       MQTT_PAYLOAD_RES_CODE[] =       "RES_CODE";
static const char       MQTT_PAYLOAD_RC_ID[]    =       "RC_ID";
static const char       MQTT_PAYLOAD_RC_TS[]    =       "RC_TS";
static const char       MQTT_PAYLOAD_TS[]       =       "TS";
static const char       MQTT_PAYLOAD_RESET[]    =       "RST";
static const char       MQTT_PAYLOAD_LOCK[]     =       "LOCK";
static const char       MQTT_PAYLOAD_UNLOCK[]   =       "UNLOCK";
static const char       MQTT_PAYLOAD_LOCK_ACK[] =       "LOCK_ACK";
static const char       MQTT_PAYLOAD_UNLOCK_ACK[]=      "UNLOCK_ACK";
static const char       MQTT_PAYLOAD_LAT[]      =       "LAT";
static const char       MQTT_PAYLOAD_LON[]      =       "LON";

static const char       MQTT_PAYLOAD_LANGUAGE_ID[]=     "LANGUAGE";
static const char       MQTT_PAYLOAD_LANGUAGE_ACK[]=    "LANGUAGE_ACK";

static const char       MQTT_PAYLOAD_WATERUNIT_ID[]=    "WUNIT";

static const char       MQTT_PAYLOAD_RESTART[]     =    "RESTART";
static const char       MQTT_PAYLOAD_LICENSE[]     =    "LICENSE";
static const char       MQTT_PAYLOAD_HEATER[]      =    "HEATER";
/************IoTHUB related STRINGS*******************/

char DEVICE_CLIENT_ID[25] = {'\0'};
char DEVICE_USER_ID[100]   = {'\0'};
char DEVICE_SAS_TOKEN[] = "";

/************IoTHUB related TOPICS FOR PUBLISHING MESSAGES*******************/
char SECURE_GW_TOPIC_DATA[]  =  "GW_IoTHub-DATA";
char SECURE_GW_TOPIC_EVENT[] =  "GW_IoTHub-EVENT";
char SECURE_GW_TOPIC_ACK[]   =  "GW_IoTHub-ACK";

char payload_array[MAX_PAYLOADS][PAYLOAD_LENGTH];
char payload_count=0;

int MQTT_Send_Publish(u8 ssid, u16 MSG_type);

/*-----	MQTT -----*/
s32 MQTT_Send_CONN_DISCONN_Request(u8 ssid, u8 action);
s32 MQTT_Send_SUB_Request(u8 ssid,u8 param);
s32 TCP_Socket_Read_Data_SSL (u8* buff, s32 len);
void MQTT_Packet_Process(u8 ssid, char *buffer_mqtt_packet);

s32 buflen = sizeof(g_buf_recvd_data);
s32 g_len_send_data = 0;
s32 g_recvd_data_last_read_pos=0;

#define SSL_SOC_MAX_DATA_LENGTH         1460

u8 em_error[2] = {'\0'};
u8 tamper[2] = {'\0'};
u8 em_error_E1[2] = {'\0'};
u8 em_error_E5[2] = {'\0'};


char cmdrcbal[10] = {'\0'};
char Received_RCID[25] = {'\0'};
char Received_RCTS[25] = {'\0'};
u32 water_balance = 0;
u32 rechargebalance = 0;
char flow_rate[10] = {'\0'};
char TDS_Input_Data[10] = {'\0'};
char TDS_Ouput_Data[10] = {'\0'};
char Pump_Current[10] = {'\0'};

char local_time[25] = {0};

char last_connectedtime[25] = {'\0'};
extern u32 IPC_CMD_REQ_source;
extern u32 PUBLISH_RESPONSE_REQ_source;

char lock_unlock_ack[1] = {'\0'};;

char rcbal[10] = {'\0'};
char RCID[25] = {'\0'};
char RCTS[25] = {'\0'};

extern s32 RIL_SSL_QSSLSEND(u8 ssid,u8 *data,u32 length) ;

//******************** defines for Device TWIN desired properties ***************/
#define TWIN_DESIRED_PROPERTIES            "desired"
#define TWIN_REPORTED_PROPERTIES           "reported"

#define TWIN_GEOLOCATION                "GeoLocation"
#define TWIN_VENDOR_CODE                "VendorCode"
#define TWIN_DEVICE_TYPE                "DeviceType"
#define TWIN_DEVICE_STATUS              "DeviceStatus"
#define TWIN_IOT_HUB_NAME               "IotHubName"
#define TWIN_DPS_ID_SCOPE               "DpsIdScope"
#define TWIN_CNTRL_FW_FILE_NAME         "CntrlFirmwareFileName"
#define TWIN_CNTRL_FW_FILE_VER          "CntrlFirmwareVersion"
#define TWIN_CNTRL_FW_FILE_URL          "CntrlFirmwareUrl"
#define TWIN_VERSION                    "$version"
#define TWIN_IMEI                       "IMEI"
#define TWIN_CCID                       "CCID"
#define TWIN_CNTRL_FW_UPDATE_STATUS     "CntrlFirmwareUpdateStatus"

#define REPORT_TWIN_GEOLOCATION             (1<<0)
#define REPORT_TWIN_VENDOR_CODE             (1<<1)
#define REPORT_TWIN_DEVICE_TYPE             (1<<2)
#define REPORT_TWIN_DEVICE_STATUS           (1<<3)
#define REPORT_TWIN_IOT_HUB_NAME            (1<<4)
#define REPORT_TWIN_DPS_ID_SCOPE            (1<<5)
#define REPORT_TWIN_CNTRL_FW_INFO           (1<<6)
#define REPORT_TWIN_CNTRL_FW_VER            (1<<7)
#define REPORT_TWIN_CNTL_FW_UPDTAE_STATUS   (1<<8) //also defined in fota_app.c file
#define REPORT_TWIN_IMIE                    (1<<9)
#define REPORT_TWIN_CCID                    (1<<10)

u32 Process_TWIN_PROPERTY_CHANGE = 0;

#define REPORT_TWIN_CNTRL_FW_FILE_NAME                            (1<<0)
#define REPORT_TWIN_CNTRL_FW_FILE_VER                             (1<<1)
#define REPORT_TWIN_CNTRL_FW_FILE_URL                             (1<<2)

u8 FOTA_information = 0;

//Below are the arays where Device reported properties are stored..
char RP_Geolocation[5] = {"IND"}; //Set with default data to report back
char RP_vendorcode[10] = {"VD000"}; //Set with default data to report back
char RP_devicetype[10] = {"HCS2"}; //Set with default data to report back
char RP_devicestatus[10] = {"DEV"}; //Set with default data to report back
char RP_IotHubName[25] = {'\0'};
char RP_DpsIdScope[10] = {'\0'};
char RP_CntrlFirmwareFileName[50] = {'\0'};
char RP_CntrlFirmwareVersion[10] = {'\0'};
char RP_CntrlFirmwareFileURL[150] = {'\0'};
char RP_FW_Upgrade_status[5] = {'\0'};
char DP_Version_Info[5] = {'\0'};

char time_stamp[18] = {'\0'};
int Water_Unit_index_set = 0;// 0 == LITRE
// 1 == INDONESIAN GALLON ( 1 gallon = 19.6 litre)
// 2 == CAN ( 1 can  = 5 litre)

char Water_data_Publish_without_unit[5] = {'\0','\0','\0','\0','\0'};
extern u32 writeedlen, readedlen ;
extern s32 RIL_SSL_QSSLCLOSE(u8 ssid);
extern void Refresh_Communication_inprogress_Symbol(unsigned int colour);
s32 Process_Device_Twin_Properties(unsigned char *twin_msg);
/*****************************************************************
 * Function: s8 GetLocalTime_UTC(char* time_stamp_now)
 *
 * Description:
 *		returns the array with local time in UTC
 * Parameters:
 *		char* time_stamp_now : Array to store the time stamp in UTC
 *
 * Return:
 *		s8 : Check status Fail or PASS.
 *****************************************************************/
s8 GetLocalTime_UTC(char* time_stamp_now, char* time_stamp_string_copy)
{
	ST_Time* return_loc_time;
	ST_Time recv_loc_time;

	u64 UTC_time =0;

	return_loc_time = Ql_GetLocalTime(&recv_loc_time);
	if(return_loc_time == NULL)
	{
#ifdef DEBUG_MQTT
		APP_DEBUG("TIME READING FAIL\r\n");
#endif
		return -1;
	}
	UTC_time= Ql_Mktime(return_loc_time);

#ifdef DEBUG_MQTT
	APP_DEBUG("\r\n<-- SUCCESS: GetLocalTime(%d.%02d.%02d %02d:%02d:%02d timezone=%02d) -->\n\r\n", return_loc_time->year, return_loc_time->month, return_loc_time->day, return_loc_time->hour, return_loc_time->minute, return_loc_time->second, return_loc_time->timezone);
#endif
	Ql_sprintf(time_stamp_now,"%d",UTC_time);
	Ql_sprintf(time_stamp_string_copy,"%04d-%02d-%02d %02d:%02d:%02d",return_loc_time->year, return_loc_time->month, return_loc_time->day, return_loc_time->hour, return_loc_time->minute,return_loc_time->second);
	return 0;
}

/*****************************************************************
 * Function: MQTT_Send_CONN_DISCONN_Request
 *
 * Description:
 *		MQTT_Send_CONN_DISCONN_Request
 *		Send the Conn request on the Socket
 *		Serialise the Connect Req packet into the MQTT packet conn req format
 *
 * Parameters:
 *		u8 ssid : Socket id on which want to send the Conn req
 *		u8 action : 1-> Connect , 0-> Disconnect
 *
 * Return:
 *
 *****************************************************************/
s32 MQTT_Send_CONN_DISCONN_Request(u8 ssid, u8 action)
{
	//s32 ret;
	s32 len = 0;

	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

	char *buf_ssl_send_data = (char *)Ql_MEM_Alloc(SSL_SOC_MAX_DATA_LENGTH);

	Refresh_Communication_inprogress_Symbol(colAqua);

	g_len_send_data = 0;
	Ql_memset(buf_ssl_send_data, 0x0, sizeof(buf_ssl_send_data));

	data.keepAliveInterval = 0;
	data.cleansession = 1;

	switch(ssid)
	{
	case SSID_HOST_AZURE:
		data.will.qos = 1;
		data.clientID.cstring = DEVICE_CLIENT_ID;
		data.username.cstring = DEVICE_USER_ID;
		data.password.cstring = DEVICE_SAS_TOKEN;
		break;
	}
	if(action == MQTT_ON)
	{
		len = MQTTSerialize_connect((unsigned char *)buf_ssl_send_data, buflen, &data);
	}
	else
	{
		len = MQTTSerialize_disconnect((unsigned char*) buf_ssl_send_data, 0);
	}

	RIL_SSL_QSSLSEND(ssid,(u8 *)buf_ssl_send_data,len);

	Ql_MEM_Free(buf_ssl_send_data);
	buf_ssl_send_data = NULL;

	return 0;
}


/*****************************************************************
 * Function: MQTT_Send_SUB_Request
 *
 * Description:
 *		MQTT_Send_SUB_Request
 *		Send the Subscription request on the Socket
 *		Serialise the Subscription Req packet into the MQTT packet Subscription req format with
 *		Appropriate TopicString.
 *
 * Parameters:
 *		u8 ssid	: Socket id on which want to send the Subscription req.
 *		u8 param_subtopic : Number which topic is to be subscribe.
 * Return	 Status
 *
 *****************************************************************/
s32 MQTT_Send_SUB_Request(u8 ssid, u8 param_subtopic)
{
	s32 msgid = 1;
	s32 req_qos = 0;
	s32 len = 0;

	char topic_subscibe[100]= {0};
	Ql_memset(topic_subscibe, 0x0, sizeof(topic_subscibe));

	MQTTString topicString = MQTTString_initializer;
	char *buf_ssl_send_data = (char *)Ql_MEM_Alloc(SSL_SOC_MAX_DATA_LENGTH);
	Ql_memset(buf_ssl_send_data, 0x0, sizeof(buf_ssl_send_data));


	switch(param_subtopic)
	{
	case SUB_IOTHUB_TOPIC_ALL: // Subscribe for all messages from IoT Hub for devices
		Ql_sprintf(topic_subscibe,"devices/%s/messages/devicebound/#",DEVICE_CLIENT_ID);
		topicString.cstring = topic_subscibe;
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_CON_ON_SYMBOL);
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_ON_SYMBOL);
		msgid = 1;
		break;
	case SUB_DEVICE_TWIN_DESIRED_TOPIC: // Subscribe for Device twin desired parameter change notification
		Ql_sprintf(topic_subscibe,"$iothub/twin/PATCH/properties/desired/#");
		topicString.cstring = topic_subscibe;
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_ON_SYMBOL);
		msgid = 2;
		break;
	case SUB_DEVICETWIN_TOPIC: // Subscribe for device twin topic
		Ql_sprintf(topic_subscibe,"$iothub/twin/res/#");
		topicString.cstring = topic_subscibe;
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_ON_SYMBOL);
		msgid = 3;
		break;

	default :
		break;
	}
#ifdef DEBUG_MQTT
	APP_DEBUG("\r\n<-- SUBSCRIBED TOPIC : %s-->\r\n", topic_subscibe);
#endif
	len = MQTTSerialize_subscribe((unsigned char *)buf_ssl_send_data, buflen, 0, msgid, 1, &topicString, &req_qos);

	RIL_SSL_QSSLSEND(ssid,(u8 *)buf_ssl_send_data,len);
	Ql_MEM_Free(buf_ssl_send_data);
	buf_ssl_send_data = NULL;
	return 0;
}

/*****************************************************************
 * Function: MQTT_Packet_Process
 *
 * Description:
 *		MQTT_Packet_Process
 *		check the buffer or data recived on the socket and parse this according to
 *		SSID
 *		TOPIC
 *		and CON_ACK or SUB_ACK or PUBLISH
 *
 * Parameters:
 *		u8 ssid :socket id on which data recived
 *		char *buffer_mqtt_packet : actual packet data.
 * Return	 Status
 *
 *****************************************************************/
void MQTT_Packet_Process(u8 ssid, char *buffer_mqtt_packet)
{
	s32 ret = -1;

	char *buf_temp = (char *)Ql_MEM_Alloc(RECVD_DATA_LENGTH);

	g_recvd_data_last_read_pos = 0;

	unsigned char sessionPresent, connack_rc;

	unsigned short submsgid;
	int subcount;
	int granted_qos;

	unsigned char dup;
	int qos;
	unsigned char retained;
	unsigned short msgid;
	int payloadlen_in;
	unsigned char* payload_in;
	char* tempstring = NULL;
	cJSON *root = NULL;
	cJSON *tmp = NULL;
	cJSON *tmp_obj = NULL;
	float RCbalance_now =0;

	char cmd_Parameter[2] = {0};
	char Received_TS[50] = {0};

	MQTTString receivedTopic;

	/* MQTT Read Packet ----------------------------------------------------------------------*/

	if(ssid == SSID_HOST_AZURE)
	{
		ret = MQTTPacket_read((unsigned char *)buf_temp, buflen, TCP_Socket_Read_Data_SSL);
#ifdef DEBUG_MQTT

		APP_DEBUG("<-- AZURE IOT HUB : MQTT Data -");

		if(ret == -1)
		{
			APP_DEBUG("retuen error\r\n");
		}
		else
		{
			APP_DEBUG("%s , MQTT packet type : %d\r\n",buf_temp, ret);
		}
#endif
		switch(ret)
		{
		case CONNACK ://MQTT connection ACK
		{
			if(ssid == SSID_HOST_AZURE)
			{
				if (MQTTDeserialize_connack(&sessionPresent, &connack_rc, (unsigned char *)buffer_mqtt_packet, buflen) != 1 || connack_rc != 0)
				{
#ifdef DEBUG_MQTT

					APP_DEBUG("\r\n<-- AZURE IOT Broker Unable to connect, return code %d-->\n\r", connack_rc);
#endif
				}
				else
				{
					//MQTT connect ack received.. now Subcribe for topic IoT Hub device messages all
					MQTT_Send_SUB_Request(ssid, SUB_IOTHUB_TOPIC_ALL);
				}
			}
		}
		break;

		case SUBACK: //Subscribe ACK
		{
			if(ssid == SSID_HOST_AZURE)
			{
				MQTTDeserialize_suback(&submsgid,1,&subcount,&granted_qos,(unsigned char *)buffer_mqtt_packet,buflen);

				if(submsgid == 1)// IoT HuB Message Topic Subscribe ACK
				{
#ifdef DEBUG_MQTT
					APP_DEBUG("\n\r SUBACK: IoT Hub Message, Granted qos: %d \n\r", granted_qos);
#endif
					FLAG.MQTT_CONNECTION_STATUS = TRUE;
					PUBLISH_RESPONSE_REQ_source |=SCHEDULE_PUBLISH_CONNECTIVITY_ON;
					//Subscribe IoT Hub Message Topic ACK received.. now Subcribe device twin desired parameter update topic
					MQTT_Send_SUB_Request(ssid, SUB_DEVICE_TWIN_DESIRED_TOPIC);
				}
				else if(submsgid == 2)// IoT HuB Device twin desired topic Subscribe ACK
				{
#ifdef DEBUG_MQTT
					APP_DEBUG("\n\r SUBACK:  Device Twin Desired Topic, Granted qos: %d \n\r", granted_qos);
#endif
					//Subscribe IoT Hub Message Topic ACK received.. now Subcribe device twin desired parameter update topic
					MQTT_Send_SUB_Request(ssid, SUB_DEVICETWIN_TOPIC);
				}
				else if(submsgid == 3)// IoT HuB Device twin Topic Subscribe ACK
				{
#ifdef DEBUG_MQTT
					APP_DEBUG("\n\r SUBACK: Device Twin Topic, Granted qos: %d \n\r", granted_qos);
#endif
					//Publish an empty message to get the entire set of device twin desired and reported properties data
					PUBLISH_RESPONSE_REQ_source |=SCHEDULE_DEVICE_TWIN_GET_MESSAGE;
				}
			}
		}
		break;

		case PUBLISH : // Publish
		{
			if(ssid == SSID_HOST_AZURE )
			{
				MQTTDeserialize_publish(&dup, &qos, &retained, &msgid, &receivedTopic, &payload_in, &payloadlen_in, (unsigned char *)buffer_mqtt_packet, buflen);

				tempstring = Ql_strstr(receivedTopic.lenstring.data, "/messages/devicebound");// A received topic will contain this string non twin messages only
				if(tempstring !=NULL)
				{
#ifdef DEBUG_MSG
					APP_DEBUG("\r\n\n<--***AZURE_IOTHUB MESSAGE RECEIVED ***-->\r\n");
#endif

					cJSON_InitHooks(NULL); //init function pointers to Ql_MEM_Alloc calloc realloc
					root = cJSON_Parse((const char *)payload_in);
					tmp = cJSON_GetObjectItem(root, "d");

					//1. Check if RESET command is received in the MQTT payload.
					tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_RESET);
					if(NULL != tmp_obj)
					{
						Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
						APP_DEBUG("RST = %s\r\n", cmd_Parameter);
#endif
						tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_TS);
						Ql_strcpy(Received_TS,tmp_obj->valuestring);

						if(Ql_strncmp(cmd_Parameter,"1",Ql_strlen("1"))==0)
						{
							// ---------------Set up the language
							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_LANGUAGE_ID);
							Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
							APP_DEBUG("LANGUAGE = %s\r\n", cmd_Parameter);
#endif
							Last_language_index = language_index;
							language_index = Ql_atoi((const char *)cmd_Parameter);
							if(language_index < LANGUAGES_SUPPORTED)
							{
								repaint_water_balance_screen();
							}
							else
							{
								language_index = Last_language_index;
							}

							// ---------------Set up the water measurement unit
							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_WATERUNIT_ID);
							Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
							APP_DEBUG("WATER UNIT = %s\r\n", cmd_Parameter);
#endif

							Water_Unit_index_set = Ql_atoi((const char *)cmd_Parameter);
							if(Water_Unit_index_set < WATER_UNIT_SUPPORTED)
							{
								IPC_CMD_REQ_source |= SCHEDULE_IPC_SET_WATER_UNIT_CMD; // Schedule IPC command towards Control Module
							}

							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_RC_TS);
							Ql_strcpy(RCTS,tmp_obj->valuestring);

#ifdef DEBUG_MQTT
							APP_DEBUG("\r\n--> Resetting the water balance and configuring Language and water unit ..Please Wait...\r\n\n");
#endif
							//Reset all persistent parameters, reset water balance to zero, and configure language and water unit, then do a soft reset to restart

							Water_Unit_index_get = Water_Unit_index_set;
							rechargebalance = 0;
							IPC_CMD_REQ_source |= SCHEDULE_IPC_RESET_WATER_DATA_CMD; // Schedule IPC command towards Control Module
							FLAG.DEVICE_RESET_SCHEDULED = TRUE;
						}
					}

					//2. Check if RECHARGE command is received in the MQTT payload.
					if(NULL == tmp_obj)
					{
						tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_RECHARGE);
						if(NULL != tmp_obj)
						{
							Ql_strcpy(rcbal,tmp_obj->valuestring);

							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_RC_ID);
							Ql_strcpy(RCID,tmp_obj->valuestring);

							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_RC_TS);
							Ql_strcpy(RCTS,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
							APP_DEBUG("RC_BAL = %s \r\n", rcbal);
							APP_DEBUG("RC_ID = %s\r\n", RCID);
							APP_DEBUG("RC_TS = %s\r\n", RCTS);
#endif
							//recharge balance into integer
							if(rcbal[0] == '-') // Negative recharge
							{
								RCbalance_now = Ql_atof((const char *)&rcbal[1]); //Ignore negative sign
								rechargebalance = RCbalance_now * 10; // to make received recharge in unit of 100 ml that is required by control module
								if(rechargebalance == 0) // DO not attempt any actual recharge.. simply ack as success
								{
									Ql_strcpy(cmdrcbal,rcbal);
									Ql_strcpy(Received_RCID,RCID);
									Ql_strcpy(Received_RCTS,RCTS);
									Store_Persistent_Data();   //Write permanent data to flash file with recharge information
									if(FLAG.DIGNOSTIC_MODE == TRUE)
									{
										Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_DIAG_RECHARGE_INFO);//Update the recharge information on diagnostic screen
									}
									Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_RC_OK);
									Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_WATER_BALANCE); // Send updated water balance to cloud

								}
								else
								{
									if((water_balance) >= (rechargebalance)) // Recharge balance should never go negative
									{
										rechargebalance = (water_balance) - (rechargebalance);
										IPC_CMD_REQ_source |= SCHEDULE_IPC_RESET_WATER_DATA_CMD; // Schedule IPC command towards Control Module
									}
									else
									{
										rechargebalance =0;
										Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_RC_FAIL); // Send a NACK to cloud recharge requested indicating a fail
										Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_WATER_BALANCE); // Send updated water balance to cloud
									}
								}

							}
							else // Positive recharge
							{
								if(rcbal[0] == '+')
								{
									RCbalance_now = Ql_atof((const char *)rcbal[1]); // ignore positive sign if available
								}
								else
								{
									RCbalance_now = Ql_atof((const char *)rcbal);
								}
								rechargebalance = RCbalance_now * 10; // to make received recharge in unit of 100 ml that is required by control module
								if(rechargebalance == 0) // Do not attempt any actual recharge.. simply ack as success
								{
									Ql_strcpy(cmdrcbal,rcbal);
									Ql_strcpy(Received_RCID,RCID);
									Ql_strcpy(Received_RCTS,RCTS);
									Store_Persistent_Data();   //Write permanent data to flash file with recharge information
									if(FLAG.DIGNOSTIC_MODE == TRUE)
									{
										Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_DIAG_RECHARGE_INFO);//Update the recharge information on diagnostic screen
									}
									Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_RC_OK);
									Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_WATER_BALANCE); // Send updated water balance to cloud
								}
								else
								{
									if((water_balance) + (rechargebalance) <= MAX_ALLOWED_WATER_BALANCE)
									{
										IPC_CMD_REQ_source |= SCHEDULE_IPC_SET_WATER_DATA_CMD; // Schedule IPC command towards Control Module
									}
									else
									{
										rechargebalance =0;
										Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_RC_FAIL); // Send a NACK to cloud recharge requested indicating a fail
										Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PUBLISH_DATA, MQTT_WATER_BALANCE); // Send updated water balance to cloud
									}
								}
							}
						}
					}

					//3. Check if LOCK command is received in the MQTT payload.
					if(NULL == tmp_obj)
					{
						tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_LOCK);
						if(NULL != tmp_obj)
						{
							Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
							APP_DEBUG("LOCK = %s\r\n", cmd_Parameter);
#endif
							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_TS);
							Ql_strcpy(Received_TS,tmp_obj->valuestring);

							if(Ql_strncmp(cmd_Parameter,"1",Ql_strlen("1"))==0)
							{
								IPC_CMD_REQ_source |= SCHEDULE_IPC_LOCK_CMD;
							}
						}
					}

					//4. Check if UNLOCK command is received in the MQTT payload.
					if(NULL == tmp_obj)
					{
						tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_UNLOCK);
						if(NULL != tmp_obj)
						{
							Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
							APP_DEBUG("UNLOCK = %s\r\n", cmd_Parameter);
#endif
							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_TS);
							Ql_strcpy(Received_TS,tmp_obj->valuestring);

							if(Ql_strncmp(cmd_Parameter,"1",Ql_strlen("1"))==0)
							{
								IPC_CMD_REQ_source |= SCHEDULE_IPC_UNLOCK_CMD;
							}
						}
					}

					//5. Check if CONFIG command is received in the MQTT payload.
					if(NULL == tmp_obj)
					{
						tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_LANGUAGE_ID);
						if(NULL != tmp_obj)
						{
							Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
							APP_DEBUG("LANGUAGE = %s\r\n", cmd_Parameter);
#endif
							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_TS);
							Ql_strcpy(Received_TS,tmp_obj->valuestring);

							Last_language_index = language_index;
							language_index = Ql_atoi((const char *)cmd_Parameter);
							if(language_index < LANGUAGES_SUPPORTED)
							{
								repaint_water_balance_screen();
								PUBLISH_RESPONSE_REQ_source |= SCHEDULE_PUBLISH_RESPONSE_FOR_LANGUAGE_CMD;
							}
							else
								language_index = Last_language_index;
						}
					}

					//6. Check if RESTART command is received in the MQTT payload.
					if(NULL == tmp_obj)
					{
						tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_RESTART);
						if(NULL != tmp_obj)
						{
							Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
#ifdef DEBUG_MQTT
							APP_DEBUG("RESTART = %s\r\n", cmd_Parameter);
#endif
							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_TS);
							Ql_strcpy(Received_TS,tmp_obj->valuestring);

							if(Ql_strncmp(cmd_Parameter,"1",Ql_strlen("1"))==0)
							{
								IPC_CMD_REQ_source|=SCHEDULE_POWER_DOWN_WITH_BACKUP;;
							}
						}
					}
					//7. Check if Firmware upgrade command is received in the MQTT payload.
					if(NULL == tmp_obj)
					{
						tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_LICENSE);
						if(NULL != tmp_obj)
						{
							Ql_strcpy(cmd_Parameter,tmp_obj->valuestring);
//#ifdef DEBUG_MQTT
							APP_DEBUG("FW_UPGRADE = %s\r\n", cmd_Parameter);
//#endif
							tmp_obj = cJSON_GetObjectItem(tmp, MQTT_PAYLOAD_TS);
							Ql_strcpy(Received_TS,tmp_obj->valuestring);
//#ifdef DEBUG_MQTT
							APP_DEBUG("TS = %s\r\n", Received_TS);
//#endif
							tmp_obj = cJSON_GetObjectItem(tmp,"FWURL");
							Ql_strcpy(HTTP_LICENSE_URL_ADDR,tmp_obj->valuestring);
//#ifdef DEBUG_MQTT
							APP_DEBUG("FWURL = %s \r\n", HTTP_LICENSE_URL_ADDR);
//#endif
							ret = LICENSE_INIT();	//Call function for FOTA_INIT
//#ifdef DEBUG_MQTT
							Read_License();
//#endif

						}
					}

				}
				else // Check if messageis for device twin properties..
				{
					// A received topic will contain topic string "$iothub/twin/res/200/?$rid=0" for entire Device Twin properties response
					tempstring = Ql_strstr(receivedTopic.lenstring.data, "$iothub/twin/res/200/?$rid=0");
					if(tempstring !=NULL) // Device twin response received with success status /200/ and message ID 0
					{
#ifdef DEBUG_MSG
						APP_DEBUG("\r\n<--*** Device twin list received ***-->\r\n");
#endif
						if(Process_Device_Twin_Properties(payload_in) == RET_FAIL)
							PUBLISH_RESPONSE_REQ_source |= SCHEDULE_DEVICE_TWIN_GET_MESSAGE; //Retry to get twin properties
						else
							FLAG.TWIN_PROPERTIES_PROCESSED = TRUE; // All the properties in the twin list are now processed..
					}
					else
					{
						tempstring = Ql_strstr(receivedTopic.lenstring.data, "$iothub/twin/PATCH/properties/desired/");// A received topic wil contain the device ID for non twin messages only
						if(tempstring !=NULL) // Device twin desired properties change notificaiton received
						{
#ifdef DEBUG_MSG
							APP_DEBUG("\n\r<-- ***Device twin desired notification received ***-->\r\n");
#endif
							if(FLAG.TWIN_PROPERTIES_PROCESSED == TRUE) // Process the desired properties change notifications only after twin list is processed
							{
								Process_Device_Twin_Properties(payload_in);
							}
							else
							{
								PUBLISH_RESPONSE_REQ_source |= SCHEDULE_DEVICE_TWIN_GET_MESSAGE; //first do a Retry to get twin properties
							}
						}
					}
				}
			}
		}
		break;

		case PINGRESP: //Ping Response
#ifdef DEBUG_MQTT
			APP_DEBUG("<-- PING RESP Received-->\r\n");
#endif
			//make server connection connected flag set to 1
			//Read connection time

			break;

		default	:
			break;
		}
	}
	Ql_memset(buffer_mqtt_packet,0x0,RECVD_DATA_LENGTH);
	Ql_memset(buf_temp,0x0,RECVD_DATA_LENGTH);
	Ql_MEM_Free(buf_temp);

	buf_temp = NULL;
}

/*****************************************************************
 * Function: MQTT_Send_Publish
 *
 * Description:
 *		MQTT_Send_Publish Sending the Message MQTT IOTHUB
 * Parameters:
 *		u8 ssid	: Socket on Which  payload is Published.
 *		u16 msg_type : MSG_TYPE normal data or publish data
 *
 * Return:
 *	 int : status Fail or PASS.
 *****************************************************************/
int MQTT_Send_Publish(u8 ssid, u16 MSG_TYPE)
{
	bool LOG_MESSAGES_AVAILABLE = FALSE;
	s32  filehandleData = -1;
	unsigned char invalid_payload = 0,valid_payload=0;
	s32 ret=PASS,i=0;
	char msg_id[18];
	char string_temp[10] = {0};
	s32 rc = 0;
	char *Topic_String = NULL;
	cJSON *json_root = NULL;
	cJSON *value = NULL;

	char topic_subscibe[100]= {0};
	Ql_memset(topic_subscibe, 0x0, sizeof(topic_subscibe));

	//Initiate the topic for messages from Device to cloud
	Ql_sprintf(topic_subscibe,"devices/%s/messages/events/Topic=",DEVICE_CLIENT_ID);

	//get Time stamp in UTC

	GetLocalTime_UTC(time_stamp, local_time);

	char* payload=NULL;
	int payloadlen ;
	int len = 0;
	int buflen=0;
	char *buf_ssl = NULL;

	MQTTString topicString = MQTTString_initializer;

	for(i=0;i<10;i++)
	{
		msg_id[i] = time_stamp[i];
	}
	for(i=10;i<17;i++)
	{
		msg_id[i] = IMEI[i-1];
	}
	msg_id[17] = '\0';

#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<--****");
#endif
	switch(MSG_TYPE)
	{
	case MQTT_POWER_ON:
	case MQTT_POWER_OFF:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_POWER_");
		if( MSG_TYPE == MQTT_POWER_ON)
		{
			APP_DEBUG("ON ");
		}
		else if( MSG_TYPE == MQTT_POWER_OFF)
		{
			APP_DEBUG("OFF ");
		}
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		if( MSG_TYPE == MQTT_POWER_ON)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_PWR,"1");
		}
		else if( MSG_TYPE == MQTT_POWER_OFF)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_PWR,"0");
		}

		for(i=0;i<5;i++)
		{
			if(water_balance_buffer[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = water_balance_buffer[i];
		}

		cJSON_AddStringToObject(value, MQTT_PAYLOAD_BAL,Water_data_Publish_without_unit);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_FW_VER,FW_Version_STRING);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_FW_TS,__TIME__);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_ID,Received_RCID);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_TS,Received_RCTS);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_EVENT);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_CONNECTIVITY_ON:
	case MQTT_CONNECTIVITY_LOW:
	case MQTT_BAL_ZERO:
	case MQTT_BAL_LOW:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_");
		if( MSG_TYPE == MQTT_CONNECTIVITY_ON)
		{
			APP_DEBUG("CONNECTIVITY_ON ");
		}
		else if( MSG_TYPE == MQTT_CONNECTIVITY_LOW)
		{
			APP_DEBUG("CONNECTIVITY_LOW ");
		}
		else if( MSG_TYPE == MQTT_BAL_ZERO)
		{
			APP_DEBUG("BAL_ZERO ");
		}
		else if( MSG_TYPE == MQTT_BAL_LOW)
		{
			APP_DEBUG("BAL_LOW ");
		}
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		if( MSG_TYPE == MQTT_CONNECTIVITY_ON)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_CONN,"1");
		}
		else if( MSG_TYPE == MQTT_CONNECTIVITY_LOW)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_CONL,"1");
		}
		else if( MSG_TYPE == MQTT_BAL_ZERO)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_ZBAL,"1");
		}
		else if( MSG_TYPE == MQTT_BAL_LOW)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_LBAL,"1");
		}

		for(i=0;i<5;i++)
		{
			if(water_balance_buffer[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = water_balance_buffer[i];
		}
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_BAL,Water_data_Publish_without_unit);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_ID,Received_RCID);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_TS,Received_RCTS);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_EVENT);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_WATER_BALANCE:
	{
		Ql_sprintf((char *)flow_rate,"%d",water_flow_rate_ml_minute);
		Ql_sprintf((char *)TDS_Input_Data,"%d",TDS_Sensor_In_value);
		Ql_sprintf((char *)TDS_Ouput_Data,"%d",TDS_Sensor_OUT_value);
		Ql_sprintf((char *)Pump_Current,"%d",ro_pump_current_in_mA);
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_WATER_BAL ");
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());


		for(i=0;i<5;i++)
		{
			if(water_balance_buffer[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = water_balance_buffer[i];
		}

		cJSON_AddStringToObject(value, MQTT_PAYLOAD_BAL,Water_data_Publish_without_unit);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_FLOW_RATE,flow_rate);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TDS_IN,TDS_Input_Data);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TDS_OUT,TDS_Ouput_Data);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_PUMP_CURRENT,Pump_Current);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_ID,Received_RCID);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_TS,Received_RCTS);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_DATA);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_EM_ERROR:
	case MQTT_EM_ERROR_E1:
	case MQTT_EM_ERROR_E5:
	case MQTT_TAMPER:
	case MQTT_LANGUAGE_ACK:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_");
		if( MSG_TYPE == MQTT_EM_ERROR)
		{
			APP_DEBUG("EM_ERROR ");
		}
		else if( MSG_TYPE == MQTT_EM_ERROR_E1)
		{
			APP_DEBUG("EM_ERROR_E1 ");
		}
		else if( MSG_TYPE == MQTT_EM_ERROR_E5)
		{
			APP_DEBUG("EM_ERROR_E5 ");
		}
		else if( MSG_TYPE == MQTT_TAMPER)
		{
			APP_DEBUG("TAMPER ");
		}
		else if( MSG_TYPE == MQTT_LANGUAGE_ACK)
		{
			APP_DEBUG("LANGUAGE_ACK ");
		}
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		if( MSG_TYPE == MQTT_EM_ERROR)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_EMERROR,em_error);
		}
		else if( MSG_TYPE == MQTT_EM_ERROR_E1)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_EMERROR,em_error_E1);
		}
		else if( MSG_TYPE == MQTT_EM_ERROR_E5)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_LOW_FLOW,em_error_E5);
		}
		else if( MSG_TYPE == MQTT_TAMPER)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_TAMPER,tamper);
		}
		else if( MSG_TYPE == MQTT_LANGUAGE_ACK)
		{
			Ql_sprintf(string_temp, "%d",language_index);
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_LANGUAGE_ACK,string_temp);
		}
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_EVENT);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_RC_OK:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_RC_OK ");
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_OK,"1");
		for(i=0;i<5;i++)
		{
			if(water_balance_buffer[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = water_balance_buffer[i];
		}

		cJSON_AddStringToObject(value, MQTT_PAYLOAD_BAL,Water_data_Publish_without_unit);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_ID,Received_RCID);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_TS,Received_RCTS);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_ACK);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_RC_FAIL:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_RC_FAIL ");
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_FAIL,"1");
		for(i=0;i<5;i++)
		{
			if(water_balance_buffer[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = water_balance_buffer[i];
		}

		cJSON_AddStringToObject(value, MQTT_PAYLOAD_BAL,Water_data_Publish_without_unit);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RES_CODE,"4");
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_ID,RCID);   //RCID value is received recharge
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_TS,RCTS);   ////RCID value is received recharge TS
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_ACK);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_PURIFICATION_EVENT:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_PURIFICATION_EVENT ");
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_PST_TS,Purification_Start_local_time);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_PSTP_TS,Purification_Stop_local_time);
		for(i=0;i<5;i++)
		{
			if(Water_dispensed_volume[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = Water_dispensed_volume[i];
		}
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_VOLUME,Water_data_Publish_without_unit);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_EVENT);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_LOCATION:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_LOCATION ");
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_LAT,latitude_);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_LON,longitude_);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_ID,Received_RCID);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_RC_TS,Received_RCTS);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);


		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen = payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_DATA);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_LOCK_ACK:
	case MQTT_UNLOCK_ACK:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH MQTT_");
		if( MSG_TYPE == MQTT_LOCK_ACK)
		{
			APP_DEBUG("LOCK_ACK ");
		}
		else if( MSG_TYPE == MQTT_UNLOCK_ACK)
		{
			APP_DEBUG("UNLOCK_ACK ");
		}
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());

		cJSON_AddStringToObject(value,((MSG_TYPE == MQTT_LOCK_ACK) ? MQTT_PAYLOAD_LOCK_ACK : MQTT_PAYLOAD_UNLOCK_ACK), ((Ql_strcmp(lock_unlock_ack, "0") == 0) ? "1" : "0"));

		for(i=0;i<5;i++)
		{
			if(water_balance_buffer[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = water_balance_buffer[i];
		}

		cJSON_AddStringToObject(value, MQTT_PAYLOAD_BAL,Water_data_Publish_without_unit);

		cJSON_AddStringToObject(value,MQTT_PAYLOAD_RES_CODE,lock_unlock_ack);

		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen = payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_ACK);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_PING_REQUEST_AZURE:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PING - AZURE_IOTHUB ");
#endif
		buf_ssl = (char *)Ql_MEM_Alloc(100);
		buflen = 100;
		len = MQTTSerialize_pingreq((unsigned char *)buf_ssl,buflen);
	}
	break;

	case MQTT_HEART_BEAT_MESSAGE:
	{
		Ql_sprintf((char *)flow_rate,"%d",water_flow_rate_ml_minute);
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH HEART BEAT MESSAGE ");
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());

		for(i=0;i<5;i++)
		{
			if(water_balance_buffer[i] == ' ')// Check for a space to find end of Water data string
			{
				Water_data_Publish_without_unit[i] = '\0';
				i = 4;
			}
			else
				Water_data_Publish_without_unit[i] = water_balance_buffer[i];
		}

		cJSON_AddStringToObject(value, MQTT_PAYLOAD_BAL,Water_data_Publish_without_unit);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_FLOW_RATE,flow_rate);
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_DATA);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_HEATING_OFF:
	case MQTT_HEATING_ON:
	case MQTT_HEATING_COMPLETE:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH HEATING ");
		if( MSG_TYPE == MQTT_HEATING_OFF)
		{
			APP_DEBUG("OFF ");
		}
		else if( MSG_TYPE == MQTT_HEATING_ON)
		{
			APP_DEBUG("ON ");
		}
		else if( MSG_TYPE == MQTT_HEATING_COMPLETE)
		{
			APP_DEBUG("COMPLETE ");
		}
#endif
		json_root = cJSON_CreateObject();
		cJSON_AddItemToObject(json_root, "d", value = cJSON_CreateObject());
		if( MSG_TYPE == MQTT_HEATING_OFF)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_HEATER,"0");
		}
		if( MSG_TYPE == MQTT_HEATING_ON)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_HEATER,"1");
		}
		if( MSG_TYPE == MQTT_HEATING_COMPLETE)
		{
			cJSON_AddStringToObject(value, MQTT_PAYLOAD_HEATER,"2");
		}
		cJSON_AddStringToObject(value, MQTT_PAYLOAD_TS,local_time);

		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_EVENT);
		topicString.cstring = topic_subscibe;

		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	case MQTT_LOG:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH PENDIGN LOG ");
#endif
		// Open the log file that stored unpublished MQTT messages or pending messgaes
		filehandleData = fileSys_OpenOrCreateFile((u8*)file_data);

		if(filehandleData >= QL_RET_OK)
		{
			for(i = 0; (invalid_payload+valid_payload)<MAX_PAYLOADS+1 ; i++) // Scan for all payloads in the file one by one
			{
				if(i>MAX_PAYLOADS)
					i=0;
				ret = Ql_FS_Seek(filehandleData, i*PAYLOAD_LENGTH , QL_FS_FILE_BEGIN); // Point to the start of required message in the file
				if(QL_RET_OK != ret)
				{
#ifdef DEBUG_MQTT
					APP_DEBUG("\r\n<--Error : Ql_FS_Seek ret : %d-->\r\n",ret);
#endif
				}
				payload = (char*)Ql_MEM_Alloc(PAYLOAD_LENGTH);
				Ql_memset(payload, 0x0,PAYLOAD_LENGTH );
				ret = Ql_FS_Read(filehandleData,(u8*)payload,PAYLOAD_LENGTH, &readedlen); // Read the payload from the file
				if(QL_RET_OK != ret)
				{
#ifdef DEBUG_MQTT
					APP_DEBUG("\r\n<--nError : Ql_FS_Read ret : %d-->\r\n",ret);
#endif
				}

				if(payload[0] != '{') //A payload is invalid if it start with character other than '{'
				{
					invalid_payload++;

					Ql_MEM_Free(payload);
					payload = NULL;

					if(invalid_payload==MAX_PAYLOADS+1)
					{
						PUBLISH_RESPONSE_REQ_source &= (u32) (~SCHEDULE_PUBLISH_PENDING_MESSAGES); // Remove the pending log publish flag
						break;
					}
					continue; // scan the next payload..
				}
				else if(payload[0] == '{') // A valid palyload will always start with character '{'
				{
					valid_payload++;

					payloadlen = Ql_strlen(payload); // Find out the payload length

					buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
					buflen= payloadlen+100;

					// Check the topic of payload message ..it is one out of DATA or EVENT or ACK to topic_subscibe
					if(payload[2]=='D')
					{
						Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_DATA);
					}
					else if(payload[2]=='E')
					{
						Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_EVENT);
					}
					else if(payload[2] == 'A')
					{
						Ql_strcat(topic_subscibe,SECURE_GW_TOPIC_ACK);
					}
					topicString.cstring = topic_subscibe;
					payload[2]='d';   //replace payload[2] with "d" to published to cloud
					len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);

					LOG_MESSAGES_AVAILABLE = TRUE;
#ifdef DEBUG_MSG
					APP_DEBUG(" [%d] ", i);
#endif
					break;
				}

			}
		}
	}
	break;

	case MQTT_TWIN_PROPERTIES:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH TWIN PROPERTY GET ");
#endif
		buf_ssl = (char *)Ql_MEM_Alloc(100);
		buflen= 100;

		Ql_sprintf(topic_subscibe,"$iothub/twin/GET/?$rid=0");

		topicString.cstring = topic_subscibe;

		// Send an empty packet to receive the full list of desired / reported parameter Data set
		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, NULL, 0);
	}
	break;

	case MQTT_TWIN_REPORTED:
	{
#ifdef DEBUG_MSG
		APP_DEBUG(" PUBLISH TWIN REPORTED PROPERTY");
#endif
		json_root = cJSON_CreateObject();

		{
			if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_GEOLOCATION)
				cJSON_AddStringToObject(json_root, TWIN_GEOLOCATION,RP_Geolocation);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_GEOLOCATION);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_VENDOR_CODE)
		{
			cJSON_AddStringToObject(json_root, TWIN_VENDOR_CODE,RP_vendorcode);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_VENDOR_CODE);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_DEVICE_TYPE)
		{
			cJSON_AddStringToObject(json_root, TWIN_DEVICE_TYPE,RP_devicetype);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_DEVICE_TYPE);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_DEVICE_STATUS)
		{
			cJSON_AddStringToObject(json_root, TWIN_DEVICE_STATUS,RP_devicestatus);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_DEVICE_STATUS);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_IOT_HUB_NAME)
		{
			cJSON_AddStringToObject(json_root, TWIN_IOT_HUB_NAME,RP_IotHubName);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_IOT_HUB_NAME);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_DPS_ID_SCOPE)
		{
			cJSON_AddStringToObject(json_root, TWIN_DPS_ID_SCOPE,RP_DpsIdScope);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_DPS_ID_SCOPE);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_CNTRL_FW_INFO) // Report back FOTA related properties..
		{
			cJSON_AddStringToObject(json_root, TWIN_CNTRL_FW_FILE_NAME,RP_CntrlFirmwareFileName);
			cJSON_AddStringToObject(json_root, TWIN_CNTRL_FW_FILE_VER,RP_CntrlFirmwareVersion);
			cJSON_AddStringToObject(json_root, TWIN_CNTRL_FW_FILE_URL,RP_CntrlFirmwareFileURL);

			FOTA_information = 0; //Reset the flags
		}
		else if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_CNTRL_FW_VER) // Report back FW version if not reported
		{
			cJSON_AddStringToObject(json_root, TWIN_CNTRL_FW_FILE_VER,RP_CntrlFirmwareVersion);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_CNTRL_FW_VER);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_CNTL_FW_UPDTAE_STATUS)
		{
			cJSON_AddStringToObject(json_root, TWIN_CNTRL_FW_UPDATE_STATUS,RP_FW_Upgrade_status);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_CNTL_FW_UPDTAE_STATUS);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_IMIE)
		{
			cJSON_AddStringToObject(json_root, TWIN_IMEI,IMEI);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_IMIE);
		}
		if(Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_CCID)
		{
			cJSON_AddStringToObject(json_root, TWIN_CCID,CCID);
			Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_CCID);
		}
		payload = cJSON_Print(json_root);
		payloadlen = Ql_strlen(payload);
		cJSON_Minify(payload);
		payloadlen = Ql_strlen(payload);

		buf_ssl = (char *)Ql_MEM_Alloc(payloadlen+100);
		buflen= payloadlen+100;
		cJSON_Delete(json_root);

		Ql_sprintf(topic_subscibe,"$iothub/twin/PATCH/properties/reported/?$rid=1");
		topicString.cstring = topic_subscibe;

		// Send an empty packet to receive the full list of desired / reported parameter Data set
		len = MQTTSerialize_publish((unsigned char *)buf_ssl, buflen, 0, 0, 0, 0, topicString, (unsigned char*)payload, payloadlen);
	}
	break;

	default:
		break;
	}
#ifdef DEBUG_MSG
	APP_DEBUG("*****-->\r\n");
#endif
	// Now message is ready to publish .. process it

	//In case of Pending message type if no pending message is there.. need ot skip out from this point.
	if((MSG_TYPE == MQTT_LOG) && (LOG_MESSAGES_AVAILABLE == FALSE))// No message to send in pending list
	{
#ifdef DEBUG_MSG
		APP_DEBUG("\n\r<--No Pending Data to Publish -->\r\n");
#endif
		//-------------------------------------------------------
		fileSys_CloseFile(filehandleData); // Close the file
		Ql_MEM_Free(payload);
		payload = NULL;
		Ql_MEM_Free(buf_ssl);
		buf_ssl = NULL;
		//-------------------------------------------------------
		return rc;
	}

	//There is a definite message to transmit..
#ifdef DEBUG_MSG
	APP_DEBUG("<--Message Paylod : %s -->\r\n",payload);
#endif
	//Check the gprs and socket connection status and autoprovisioning status
	//if its ok then send data over tcp ip
	//also check return of data send

	if((FLAG.SSL_CONNECTION_STATE == TRUE) &&  (FLAG.GPRS_ACTIVE == TRUE) && (FLAG.AUTOPROVISIONING_COMPLETE == TRUE))
	{
		rc = SSL_Send_Data(ssid, buf_ssl , len); // There is a reconnect attempt at the start in this function if found not connected

		if(rc == RIL_ATRSP_SUCCESS)
		{
			Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_ON_SYMBOL);

			Ql_strcpy(last_connectedtime,local_time);  //store local time to connecttime variable

			if((FLAG.DIGNOSTIC_MODE == TRUE) && (!(FLAG.SUSPEND_IPC_COMMUNICATION)))//To avoid the LCD to display last connected time if device is in FW upgrading state with diagnostic mode ON.
			{
				Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_DIAG_LAST_CONNECT_TIME); // Update last connected time in diagnostic screen
			}
			FLAG.SCHEDULE_GPRS_RECONNECT_ATTEMPT = FALSE; // DO not attempt a re-connect to GPRS as there is already connection in place

			if(MSG_TYPE == MQTT_LOG)// Invalidate the recent log record published successfully
			{
				payload[0]=NULL; // Mark the payload as invalid
				ret = Ql_FS_Seek(filehandleData, i*PAYLOAD_LENGTH , QL_FS_FILE_BEGIN);
				ret = Ql_FS_Write(filehandleData, (u8 *)payload, 1 , (u32 *)&writeedlen);
				Ql_MEM_Free(payload);
				payload = NULL;
			}
#ifdef DEBUG_MSG
			if(ssid == SSID_HOST_AZURE)
				APP_DEBUG("<-- [SUCCESS] data Send-AZURE_IOTHUB -->\r\n");
#endif

			if(MSG_TYPE == MQTT_POWER_OFF) // MQTT disconnect before power OFF / recycle
			{
				MQTT_Send_CONN_DISCONN_Request(SSID_HOST_AZURE, MQTT_OFF);
				FLAG.MQTT_CONNECTION_STATUS = FALSE;
				GPRS_Deactivate();
				FLAG.GPRS_ACTIVE = FALSE;
				Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL);
			}

			if((MSG_TYPE == MQTT_TWIN_REPORTED) && (Process_TWIN_PROPERTY_CHANGE & REPORT_TWIN_CNTRL_FW_INFO))// Prepare for FOTA download once twin reported back
			{
				FLAG.READY_TO_INITIATE_FOTA_DOWNLOAD = TRUE;
				Process_TWIN_PROPERTY_CHANGE &= (u32) (~REPORT_TWIN_CNTRL_FW_INFO);
			}
		}
		else
		{
			Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL);
#ifdef DEBUG_MSG
			if(ssid == SSID_HOST_AZURE)
				APP_DEBUG("<-- [FAIL] data Send-AZURE_IOTHUB -->\r\n");
#endif
			//Message failed to publish will be storeed in the log
			if((MSG_TYPE != MQTT_PING_REQUEST_AZURE) // Do not store PING message
					&& (MSG_TYPE != MQTT_LOG) // Do not store log record
					&& (MSG_TYPE != MQTT_HEART_BEAT_MESSAGE) //Do not store regular heart beat events
					&& (MSG_TYPE != MQTT_TWIN_PROPERTIES)//Do not Store Device Twin related events
					&& (MSG_TYPE != MQTT_TWIN_REPORTED)//Do not Store Device Twin reported related events
					&& (FLAG.SSL_X509_CERTPRESENT == TRUE)) // Do not write log messages in case of device not configured with certificates
			{
				Topic_String = Ql_strstr(topic_subscibe,SECURE_GW_TOPIC_EVENT); //check GW_IoTHub-EVENT is present in topic_subscibe
				if(Topic_String != NULL)
				{
					payload[2]='E';  // replace "d" with "E" to identify log message is event
				}
				else
				{
					Topic_String = Ql_strstr(topic_subscibe,SECURE_GW_TOPIC_DATA); //check GW_IoTHub-DATA is present in topic_subscibe
					if(Topic_String != NULL)
					{
						payload[2]='D';  // replace "d" with "E" to identify log message is data
					}
					else
					{
						Topic_String = Ql_strstr(topic_subscibe,SECURE_GW_TOPIC_ACK); //check GW_IoTHub-ACK is present in topic_subscibe
						if(Topic_String != NULL)
						{
							payload[2]='A';  // replace "d" with "A" to identify log message is ack
						}
					}
				}
				Write_And_Store_Pending_MQTT_Log_Messages(payload);
			}

			Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_GET_GPRS_STRENGTH,0);
			Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_ANTENNA_SYMBOL);
			FLAG.MQTT_CONNECTION_STATUS = FALSE; // Mark the connection not available flag
			FLAG.CON_MSG_PUBLISH_PENDING = TRUE; // At next successful connection it is required to publish the connection ON record so enable this flag
		}
	}
	else
	{
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL);

		//Message failed to publish will be storeed in the log
		if((MSG_TYPE != MQTT_PING_REQUEST_AZURE) // Do not store PING message
				&& (MSG_TYPE != MQTT_LOG) // Do not store log record
				&& (MSG_TYPE != MQTT_HEART_BEAT_MESSAGE) //Do not store regular heart beat events
				&& (MSG_TYPE != MQTT_TWIN_PROPERTIES)//Do not Store Device Twin related events
				&& (MSG_TYPE != MQTT_TWIN_REPORTED)//Do not Store Device Twin reported related events
				&& (FLAG.SSL_X509_CERTPRESENT == TRUE)) // Do not write log messages in case of device not configured with certificates
		{
			Topic_String = Ql_strstr(topic_subscibe,SECURE_GW_TOPIC_EVENT); //check GW_IoTHub-EVENT is present in topic_subscibe
			if(Topic_String != NULL)
			{
				payload[2]='E';  // replace "d" with "E" to identify log message is event
			}
			else
			{
				Topic_String = Ql_strstr(topic_subscibe,SECURE_GW_TOPIC_DATA); //check GW_IoTHub-DATA is present in topic_subscibe
				if(Topic_String != NULL)
				{
					payload[2]='D';  // replace "d" with "D" to identify log message is data
				}
				else
				{
					Topic_String = Ql_strstr(topic_subscibe,SECURE_GW_TOPIC_ACK); //check GW_IoTHub-ACK is present in topic_subscibe
					if(Topic_String != NULL)
					{
						payload[2]='A';  // replace "d" with "A" to identify log message is ack
					}
				}
			}
			Write_And_Store_Pending_MQTT_Log_Messages(payload);
		}
		Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_GET_GPRS_STRENGTH,0);
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_ANTENNA_SYMBOL);
		FLAG.SCHEDULE_GPRS_RECONNECT_ATTEMPT = TRUE; // Enable this flag to re-attempt the GPRS reconnect
		FLAG.MQTT_CONNECTION_STATUS = FALSE; // Mark the connection not available flag
		FLAG.CON_MSG_PUBLISH_PENDING = TRUE; // At next successful connection it is required to publish the connection ON record so enable this flag
	}
	if(MSG_TYPE == MQTT_POWER_OFF)
		FLAG.POWER_OFF_MSG_PROCESSED = TRUE;
	//-------------------------------------------------------
	if(MSG_TYPE == MQTT_LOG)
		fileSys_CloseFile(filehandleData); // Close the file
	Ql_MEM_Free(payload);
	payload = NULL;
	Ql_MEM_Free(buf_ssl);
	buf_ssl = NULL;
	//-------------------------------------------------------
	return rc;
}

/*****************************************************************
 * Function: s8 GetLocalTime_UTC(char* time_stamp_now)
 *
 * Description:
 *		returns the array with local time in UTC
 * Parameters:
 *		char* time_stamp_now : Array to store the time stamp in UTC
 *
 * Return:
 *		s32 : Check status Fail or PASS.
 * Notes : 1. If desired Porperties are NULL and Reported properties are NULL - Report back current set of properties..
 *       : 2. If a desired Porperty is available and no corresponding reported property available - act (if required) and report back the property
 *       : 3. If a desired Porperty is available and corresponding reported property available - act and report back the property. only if there is change in property
 *       : 4. If a desired Porperty is not available and corresponding reported property available - no action required
 *****************************************************************/
s32 Process_Device_Twin_Properties(unsigned char *twin_msg)
{
	cJSON *root = NULL;
	cJSON *desired = NULL;
	cJSON *reported = NULL;
	cJSON *desired_obj = NULL;
	cJSON *reported_obj = NULL;

	u8 i=0;
	char Desired_Property_Data[150];
	char Reported_Property_Data[150];

	cJSON_InitHooks(NULL); //init function pointers to Ql_MEM_Alloc calloc realloc
	root = cJSON_Parse(twin_msg);
	//Check if the message is for entire property list via GET message or is a change notification
	// A change notification will have version information in root jason..
	desired_obj = cJSON_GetObjectItem(root, TWIN_VERSION);//Json object listing desired properties
	if(desired_obj == NULL) // This is a GET response with entire set of desired and reported properties
	{
		desired = cJSON_GetObjectItem(root, TWIN_DESIRED_PROPERTIES);//Json object listing desired properties
		reported = cJSON_GetObjectItem(root, TWIN_REPORTED_PROPERTIES);//Json object listing reported properties
		if((desired == NULL) && (reported == NULL)) // Jason is incorect
		{
			return RET_FAIL;
		}
		desired_obj = cJSON_GetObjectItem(desired, TWIN_VERSION);
	}
	else
	{
		FLAG.DP_CHANGE_NOTIFICATION_RECEIVED = TRUE;
	}
	//The Twin message contains desired and reported properties jason objects and extract that information now
	//1. Check the version of desired properties data set
	if(desired_obj == NULL) // Version information could not be retrived for desired properties.. do not bother about reported properties version
	{
		return RET_FAIL;
	}
	else
	{
#ifdef DEBUG_MSG
		APP_DEBUG("\n\r<-- Desired Prpoerties version [%d]-->\n\r", desired_obj->valueint);
#endif
		Ql_sprintf(DP_Version_Info,"%d",desired_obj->valueint);

		//Process all Desired and to be reported properties one by one
		// 1. "GeoLocation" [Desired + Reported]
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_GEOLOCATION);
		if (NULL != desired_obj) // Desired property is available
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_GEOLOCATION, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_GEOLOCATION);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_Geolocation));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_Geolocation, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_GEOLOCATION;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_Geolocation);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. first time connect to device twin
		{
			if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE)
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_GEOLOCATION;
		}

		// 2. "VendorCode"
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_VENDOR_CODE);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_VENDOR_CODE, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_VENDOR_CODE);

		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_vendorcode));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_vendorcode, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_VENDOR_CODE;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_vendorcode);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. first time connect to device twin
		{
			if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE)
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_VENDOR_CODE;
		}

		// 3. "DeviceType"
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_DEVICE_TYPE);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_DEVICE_TYPE, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_DEVICE_TYPE);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_devicetype));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_devicetype, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_DEVICE_TYPE;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_devicetype);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. first time connect to device twin
		{
			if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE)
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_DEVICE_TYPE;
		}

		// 4. "DeviceStatus"
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_DEVICE_STATUS);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_DEVICE_STATUS, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_DEVICE_STATUS);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_devicestatus));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_devicestatus, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_DEVICE_STATUS;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_devicestatus);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. first time connect to device twin
		{
			if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE)
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_DEVICE_STATUS;
		}

		//5. IotHubName
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_IOT_HUB_NAME);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_IOT_HUB_NAME, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_IOT_HUB_NAME);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_IotHubName));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_IotHubName, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				Ql_sprintf(AZURE_assigned_IoT_Hub,"%s.azure-devices.net",RP_IotHubName);
				Store_IoT_Persistent_Data(); // Store Updated assigned IoT Hub information in permanent storage
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_IOT_HUB_NAME;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_IotHubName);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. first time connect to device twin
		{
			if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE)
			{
				for(i=0;i<Ql_strlen(AZURE_assigned_IoT_Hub);i++)
				{
					if(AZURE_assigned_IoT_Hub[i] != '.')
					{
						RP_IotHubName[i] = AZURE_assigned_IoT_Hub[i];
					}
					else
					{
						RP_IotHubName[i] = '\0'; // terminate the string
						i = Ql_strlen(AZURE_assigned_IoT_Hub); // go out of for loop
					}
				}
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_IOT_HUB_NAME;
			}
		}

		//6. DpsIdScope
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_DPS_ID_SCOPE);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_DPS_ID_SCOPE, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_DPS_ID_SCOPE);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_DpsIdScope));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_DpsIdScope, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				Ql_strcpy(AZURE_IoT_DPS_ID,RP_DpsIdScope);
				Store_IoT_Persistent_Data(); // Store Updated DPS information
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_DPS_ID_SCOPE;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_DpsIdScope);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. first time connect to device twin
		{
			if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE)
			{
				Ql_strcpy(RP_DpsIdScope, AZURE_IoT_DPS_ID);
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_DPS_ID_SCOPE;
			}
		}

		//7,8,9. DispFirmwareFileName , DispFirmwareVersion, DispFirmwareUrl
		// No -Action - We do not process Display Firmware seperatley

		//10. CntrlFirmwareFileName
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_CNTRL_FW_FILE_NAME);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_CNTRL_FW_FILE_NAME, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_CNTRL_FW_FILE_NAME);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_CntrlFirmwareFileName));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_CntrlFirmwareFileName, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				FOTA_information |= REPORT_TWIN_CNTRL_FW_FILE_NAME;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_CntrlFirmwareFileName);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. no action
		{
		}
		//11. CntrlFirmwareVersion
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_CNTRL_FW_FILE_VER);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_CNTRL_FW_FILE_VER, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_CNTRL_FW_FILE_VER);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_CntrlFirmwareVersion));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_CntrlFirmwareVersion, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				FOTA_information |= REPORT_TWIN_CNTRL_FW_FILE_VER;
			}

#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_CntrlFirmwareVersion);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. first time connect to device twin
		{
			if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE)
			{
				Ql_strcpy(RP_CntrlFirmwareVersion, FW_Version_STRING);
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTRL_FW_VER;
			}
		}

		//12. CntrlFirmwareUrl
		Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
		Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
		desired_obj = NULL;
		reported_obj = NULL;

		desired_obj = cJSON_GetObjectItem((FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == TRUE) ? root : desired, TWIN_CNTRL_FW_FILE_URL);
		if (NULL != desired_obj)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r DP: %s --> %s ", TWIN_CNTRL_FW_FILE_URL, desired_obj->valuestring);
#endif
			Ql_strcpy(Desired_Property_Data, desired_obj->valuestring);
		}
		//Check Reported Properties .. if found different .then process the required action
		reported_obj = cJSON_GetObjectItem(reported, TWIN_CNTRL_FW_FILE_URL);
		Ql_strcpy(Reported_Property_Data, ((NULL != reported_obj) ? reported_obj->valuestring : RP_CntrlFirmwareFileURL));

		if (NULL != desired_obj) //If desired property available
		{
			Ql_strcpy(RP_CntrlFirmwareFileURL, Desired_Property_Data);
			if(Ql_strcmp(Desired_Property_Data,Reported_Property_Data)) //If there is a change in desired propery and reported property.. then take appropriate action
			{
				FOTA_information |= REPORT_TWIN_CNTRL_FW_FILE_URL;
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r RP: %s -->  ",RP_CntrlFirmwareFileURL);
#endif
		}
		else if (NULL == reported_obj) // Desired properties not available and reported properties are also not available.. no action
		{
		}

		if(FLAG.DP_CHANGE_NOTIFICATION_RECEIVED == FALSE) // These are not available in Desired properties
		{
			//13. IMEI
			Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
			Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
			desired_obj = NULL;
			reported_obj = NULL;

			//This property is not available in desired property set
			//Check Reported Properties .. if found different .then report back
			reported_obj = cJSON_GetObjectItem(reported, TWIN_IMEI);
			if (NULL != reported_obj) // Reported properties has property already reported in it
			{
				Ql_strcpy(Reported_Property_Data, reported_obj->valuestring);
			}
			if(Ql_strcmp(IMEI,Reported_Property_Data)) //If there is a change in IMEI number than the earlier reported then initiate a fresh reporting
			{
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_IMIE;
			}
			//14. CCD
			Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
			Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
			desired_obj = NULL;
			reported_obj = NULL;

			//This property is not available in desired property set
			//Check Reported Properties .. if found different .then report back
			reported_obj = cJSON_GetObjectItem(reported, TWIN_CCID);
			if (NULL != reported_obj) // Reported properties has property already reported in it
			{
				Ql_strcpy(Reported_Property_Data, reported_obj->valuestring);
			}
			if(Ql_strcmp(CCID,Reported_Property_Data)) //If there is a change in IMEI number than the earlier reported then initiate a fresh reporting
			{
				Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CCID;
			}

			//15."CntrlFirmwareUpdateStatus":
			Ql_memset(Desired_Property_Data, 0x0, sizeof(Desired_Property_Data));
			Ql_memset(Reported_Property_Data, 0x0, sizeof(Reported_Property_Data));
			desired_obj = NULL;
			reported_obj = NULL;

			//This property is not available in desired property set
			//Check Reported Properties .. if found different .then report back
			reported_obj = cJSON_GetObjectItem(reported, TWIN_CNTRL_FW_UPDATE_STATUS);
			if (NULL != reported_obj) // Reported properties has property already reported in it
			{
				Ql_strcpy(Reported_Property_Data, reported_obj->valuestring);
			}
			else
			{
				Ql_strcpy(Reported_Property_Data, "6");//mark valid data for first time used
			}
			if(Ql_strcmp(FW_Version_STRING,RP_CntrlFirmwareVersion)) //Compare communication module FW version with device twin reported FW version
			{
				if(atoi(Reported_Property_Data) < 4) //if last FOTA is fail due to power failure or another reason then FOTA is start automatically
				{
					FLAG.LAST_FOTA_ATTEMPT_FAILED = TRUE;// Update status less than 4 indiates, last FOTA was terminated incomplete i.e. without SUCCESS or FAIL
				}
			}
		}
	}
	FLAG.DP_CHANGE_NOTIFICATION_RECEIVED = FALSE; //Reset the flag

	if((FOTA_information & REPORT_TWIN_CNTRL_FW_FILE_NAME) && (FOTA_information & REPORT_TWIN_CNTRL_FW_FILE_VER) && (FOTA_information & REPORT_TWIN_CNTRL_FW_FILE_URL))
	{
		// To act on a Firmware upgrade.. all three respective desired properties need to have change in them.. i.e.. File Name, File Version and File URL should have change
		Ql_strcpy(FOTA_URL, RP_CntrlFirmwareFileURL);
		Store_IoT_Persistent_Data();
		Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTRL_FW_INFO;
	}
	else if(!((FOTA_information & REPORT_TWIN_CNTRL_FW_FILE_NAME) || (FOTA_information & REPORT_TWIN_CNTRL_FW_FILE_VER) || (FOTA_information & REPORT_TWIN_CNTRL_FW_FILE_URL)))
	{
		// To act on a Firmware upgrade terminated without success of fail case .. all three respective desired properties need to not have change in them. and CntrlFirmwareUpdateStatus should be less than value 4
		if(FLAG.LAST_FOTA_ATTEMPT_FAILED == TRUE)// Update status less than 4 indiates, last FOTA was terminated incomplete i.e. without SUCCESS or FAIL
		{
			FLAG.READY_TO_INITIATE_FOTA_DOWNLOAD = TRUE;
			FLAG.LAST_FOTA_ATTEMPT_FAILED = FALSE;
		}
	}

	if(Process_TWIN_PROPERTY_CHANGE)// Schedule a reported property response for device twin for non -action items
	{
		PUBLISH_RESPONSE_REQ_source |= SCHEDULE_DEVICE_TWIN_REPORT_MESSAGE;

	}
	return RET_SUCCESS;
}

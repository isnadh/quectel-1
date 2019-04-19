/**********common_vars.h************/

/*-----	TAGLIST	-----*/
/* structure which reads from	the	file */
#ifndef __COMMON_H__
#define __COMMON_H__

#include <stddef.h>
#include "ql_memory.h"
#include <string.h>
#include <stdlib.h>
#include "ql_type.h"
#include "ql_time.h"
#include "ril.h"
#include "ril_util.h"
#include "ril_telephony.h"
#include "ril_system.h"
#include "ril_http.h"
#include "ril_network.h"
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_uart.h"
#include "ql_system.h"
#include "ql_memory.h"
#include "ql_timer.h"
#include "ql_gprs.h"
#include "ql_socket.h"
#include "ql_fs.h"
//#include "fs.h"
#include "ql_wtd.h"
#include "ql_gpio.h"
#include "ql_spi.h"
#include "ql_memory.h"
#include "debug.h"
#include "SecureSocket.h"
#include "Version.h"
#include "math.h"
#include "file_Operations.h"
#include "MQTTPacket.h"
#include "colors.h"


#define                                DEBUG_MSG
//#define                              DEBUG_MQTT
#define                              INOUT
#define                              IPC_DEBUG_MSG
//#define                              DEBUG_BOOT_MSG
//#define                              AT_CMD_MSG
//#define                              FILE_STORAGE
//#define                              FOTA_MSG


//Undefine this to disable DPS connectivity after powwr ON.
#define                              POWER_ON_DPS_CONNECT

//********************#defines For SECURE       SOCKETS**************/
#define CONTEXT                         0

//********************#defines For MQTT in main**************/
#define SUB_IOTHUB_TOPIC_ALL            0
#define SUB_DEVICETWIN_TOPIC            1
#define SUB_DEVICE_TWIN_DESIRED_TOPIC    2

extern ST_UARTDCB       UART_Config;

//********************#defines For IPC functions**************/
#define MAX_DATA 255

#define CMD_GET_IDENTITY           0x49
#define CMD_STATUS_CEHCK           0x53
#define CMD_SET_CONFIG             0x43
#define CMD_GET_CONFIG             0x4E

#define RESP_GET_IDENTITY           0x69
#define RESP_GET_CONFIG             0x6E
#define RESP_GET_WATER_DATA         0x77
#define RESP_STATUS_CEHCK           0x73

#define CONFIG_PID                     0
#define CONFIG_DATA0                   1
#define CONFIG_DATA1                   2

#define SCHEDULE_IPC_GET_WATER_DATA_CMD            (1<<0)
#define SCHEDULE_IPC_SET_WATER_DATA_CMD            (1<<1)
#define SCHEDULE_IPC_UNLOCK_CMD                    (1<<2)
#define SCHEDULE_IPC_LOCK_CMD                      (1<<3)
#define SCHEDULE_IPC_RESET_CMD                     (1<<4)
#define SCHEDULE_IPC_RESET_WATER_DATA_CMD          (1<<5)
#define SCHEDULE_IPC_GET_IDENTITY_CMD              (1<<6)
#define SCHEDULE_IPC_GET_HEATER_STATUS_DATA_CMD    (1<<7)
#define SCHEDULE_IPC_GET_INTERNAL_ERROR_CODE_CMD   (1<<8)
#define SCHEDULE_IPC_GET_PURIFICATION_ON_COUNTER_CMD   (1<<9)
#define SCHEDULE_IPC_GET_FLOW_RATE_CMD             (1<<10)
#define SCHEDULE_IPC_SET_WATER_UNIT_CMD            (1<<11)
#define SCHEDULE_POWER_DOWN_WITH_BACKUP            (1<<12)
#define SCHEDULE_IPC_RO_PUMP_CURRENT_CMD           (1<<13)
#define SCHEDULE_IPC_GET_TDS_IN_TEMPRATURE         (1<<14)
#define SCHEDULE_IPC_GET_AMC_TIMER                 (1<<15)
#define SCHEDULE_IMMEDIATE_POWER_DOWN              (1<<16)

#define SCHEDULE_PUBLISH_RESPONSE_FOR_RECHARGE_CMD (1<<0)
#define SCHEDULE_PUBLISH_RESPONSE_FOR_UNLOCK_CMD   (1<<1)
#define SCHEDULE_PUBLISH_RESPONSE_FOR_LOCK_CMD     (1<<2)
#define SCHEDULE_PUBLISH_WATER_BALANCE             (1<<3)
#define SCHEDULE_PUBLISH_POWER_ON                  (1<<4)
#define SCHEDULE_PUBLISH_CONNECTIVITY_ON           (1<<5)
#define SCHEDULE_PUBLISH_PENDING_MESSAGES          (1<<6)
#define SCHEDULE_PUBLISH_BAL_LOW                   (1<<7)
#define SCHEDULE_PUBLISH_BAL_ZERO                  (1<<8)
#define SCHEDULE_PUBLISH_CONNECTIVITY_LOW          (1<<9)
#define SCHEDULE_PUBLISH_LAT_LONG                  (1<<10)
#define SCHEDULE_PUBLISH_PURIFICATION_EVENT        (1<<11)
#define SCHEDULE_PUBLISH_RESPONSE_FOR_LANGUAGE_CMD (1<<12)
#define SCHEDULE_PUBLISH_HEATING_OFF               (1<<13)
#define SCHEDULE_PUBLISH_HEATING_ON                (1<<14)
#define SCHEDULE_PUBLISH_HEATING_COMPLETE          (1<<15)
#define SCHEDULE_HEART_BEAT_MESSAGE                (1<<16)
#define SCHEDULE_DEVICE_TWIN_GET_MESSAGE           (1<<17)
#define SCHEDULE_DEVICE_TWIN_REPORT_MESSAGE        (1<<18)

//Status Byte information
#define FAULT_STATUS_SET                          (1<<0)
#define PURIFICATION_OFF_STATUS_SET               (1<<1)
#define WATER_DATA_AVAILABLE_STATUS_SET           (1<<2)
#define HOT_WATER_AVAILABLE_STATUS_SET            (1<<3)
#define DIGNOSTIC_MODE_STATUS_SET                 (1<<4)
#define AMC_MODE_ON                               (1<<5)
#define TDS_DATA_STATUS_SET                       (1<<6)

//Fault Status Byte information
#define LPS_FAULT                                 (1<<0)
#define SV_FAULT                                  (1<<1)
#define RO_FAULT                                  (1<<2)
#define BP_FAULT                                  (1<<3)
#define FLR_FAULT                                 (1<<4)
#define FLS_FAULT                                 (1<<5)
#define AMC_SV_FAULT                              (1<<6)
#define TDS_SENSOR_1_ERROR                        (1<<8)
#define TDS_SENSOR_2_ERROR                        (1<<9)
#define AMC_FAULT                                 (1<<10)
#define POWER_FAULT                               (1<<14)
#define Internal_Error_Fault                      (1<<15)

//Sensor Input Status information
#define HW_VER0                                   (1<<0)
#define HW_VER1                                   (1<<1)
#define COM_MODULE_DETECTED                       (1<<2)
#define HEATING_COMPLETED                         (1<<3)
#define HEATING_INPROGRESS                        (1<<4)
#define BP_ALERT                                  (1<<5)
#define RO_ALERT                                  (1<<6)
#define SV_ALERT                                  (1<<7)
#define DIAGNOSTIC_SW_OPEN                        (1<<8)
#define FLOAT_SW_OPEN                             (1<<9)
#define LPS_SW_OPEN                               (1<<10)
#define HPS_SW_OPEN                               (1<<11)
#define AMC_SV_ALERT                              (1<<12)

//AMC Cycle States
#define AMC_YET_TO_START                          (1<<0)
#define AMC_PHASE_1_START                         (1<<1)
#define AMC_PROCESS_BUILDING_PRESSURE             (1<<2)
#define AMC_MEMBRANE_CLEANING                     (1<<3)
#define HPS_SWITCH_IS_OFF                         (1<<4)

//Heater status byte information
#define HEATER_POWER_ON                           (1<<0)
#define HEATER_IN_PROGRESS                        (1<<1)
#define WATER_HEATING_COMPLETED                   (1<<2)

//Purification OFF Status Byte information
#define TANK_IS_FULL                              0x01
//Bit 1 is reserved
#define DEVICE_IS_LOCKED                          0x04


#define BIT0(B)      ((B)        & 0x0001)
#define BIT1(B)      ((B)        & 0x0002)
#define BITE(B)      ((B)        & 0x4000)
#define SetBIT1(B)   ((B) = (B)  | 0x0002)
#define SetBITE(B)   ((B) = (B)  | 0x4000)
#define SetBITF(B)   ((B) = (B)  | 0x8000)
#define ResetBIT1(B) ((B) = (B)  & 0xFFFD)
#define ResetBITE(B) ((B) = (B)  & 0xBFFF)
#define ResetBITF(B) ((B) = (B)  & 0x7FFF)
//functions

//********************#defines For MQTT in main**************/


#define MSG_ID_GW_STATE                  (MSG_ID_USER_START     +       200)
#define MSG_ID_SOCKET_CONNECTION         (MSG_ID_USER_START     +       201)

#define PASS 1
#define FAIL 0

#define MQTT_ON      1
#define MQTT_OFF     0

extern u16 Publish_ping_Counter;
extern u16 Heart_Beat_Message_Counter;

extern u32 dataLen_here;
extern u16 Purification_ON_Counter;
extern u16 Previous_Purification_ON_Counter;
extern u32 g_rcvdDataLen;
extern u8 Fota_Fail_Reason_Code;
extern s32 FOTA_INIT(void);
extern char local_time[];
extern u8 Previous_Purification_ON_OFF_State;
extern char latitude_[10];
extern char longitude_[10];
extern u8  CRC_Token;
extern s32 READ_WATER_BALANCE(void);
#define MAX_ALLOWED_WATER_BALANCE              9999// recharge balance and water balance are both in 0.1 Gallon units so max allowed water balance is 9999 gallons in unit of 0.1 Gallon
#define MESSAGES_IN_LANGUAGE                      8
#define LANGUAGES_SUPPORTED                       2
#define WATER_UNIT_SUPPORTED                      3
#define MAX_ERROR_COUNT                          11
extern int Last_language_index;
extern int language_index;
extern int Water_Unit_index_set;
extern int Water_Unit_index_get;
extern char Error_messages_array[MAX_ERROR_COUNT];
extern char  REST_MESSAGE_POST[ ];
extern char  REST_MESSAGE_PUT[ ];
extern u8 configuration_Data_Write[3];
extern void Send_IPC_Command(int CmdID);
typedef	enum
{
	MQTT_WATER_BALANCE=1,
	MQTT_POWER_ON,
	MQTT_POWER_OFF,
	MQTT_CONNECTIVITY_ON,
	MQTT_CONNECTIVITY_LOW,
	MQTT_BAL_ZERO,
	MQTT_BAL_LOW,
	MQTT_EM_ERROR,
	MQTT_EM_ERROR_E1,
	MQTT_EM_ERROR_E5,
	MQTT_TAMPER,
	MQTT_RC_OK,
	MQTT_RC_FAIL,
	MQTT_PURIFICATION_EVENT,
	MQTT_LOCATION,
	MQTT_LOCK_ACK,
	MQTT_UNLOCK_ACK,
	MQTT_LANGUAGE_ACK,
	MQTT_PING_REQUEST_AZURE,
	MQTT_HEART_BEAT_MESSAGE,
	MQTT_HEATING_OFF,
	MQTT_HEATING_ON,
	MQTT_HEATING_COMPLETE,
	MQTT_LOG,
	// Add Device twin related MQTT messages below this point
	MQTT_TWIN_PROPERTIES = 64,
	MQTT_TWIN_REPORTED,
}	Enum_MQTTmsg;

Enum_MQTTmsg MQTT_msg;

typedef	enum
{
	GW_STATE_PUBLISH_DATA,
	GW_STATE_PROCESS_SUBCRIBED_DATA,
	GW_STATE_LCD_DISPLAY,
	GW_STATE_SEND_IPC_CMD,
	GW_STATE_GET_GPRS_STRENGTH,
	GW_STATE_READ_LATITUDE_LONGITUDE,
	GW_STATE_GPRS_ACTIVATION,
	GW_STATE_PUBLISH_PENDING_DATA
}	Enum_GwState ;

Enum_GwState GW_sf_state;




typedef	enum
{
	TFT_STATE_DRAW_ANTENNA_SYMBOL,
	TFT_STATE_DRAW_CLOUD_CON_ON_SYMBOL,
	TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL,

	TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_ON_SYMBOL,
	TFT_STATE_DRAW_CLOUD_COM_INPROGRESS_OFF_SYMBOL,

	TFT_STATE_DRAW_INIT_SCREEN,
	TFT_STATE_DRAW_SIM_NOT_PRESENT,

	TFT_STATE_DRAW_STRING_E1,
	TFT_STATE_CLEAR_STRING_E1,
	TFT_STATE_DRAW_STRING_E2,
	TFT_STATE_DRAW_STRING_E3,
	TFT_STATE_DRAW_STRING_E4,
	TFT_STATE_DRAW_STRING_E5,
	TFT_STATE_CLEAR_STRING_E5,
	TFT_STATE_DRAW_STRING_E6,
	TFT_STATE_DRAW_STRING_E11,
	TFT_STATE_DRAW_STRING_E9,
	TFT_STATE_DRAW_STRING_E10,
	TFT_STATE_DRAW_STRING_E12,
	TFT_STATE_DRAW_STRING_E15,
	TFT_STATE_CLEAR_STRING_E15,
	TFT_STATE_DRAW_STRING_INTERNAL_ERROR_CODE,

	TFT_STATE_DRAW_STRING_VALUE,
	TFT_STATE_DRAW_STRING_TANK_FULL,

	TFT_STATE_CLEAR_STRING_TANK_FULL,

	TFT_STATE_DRAW_IMEI,
	TFT_STATE_DRAW_CCID,
	TFT_STATE_DRAW_DIAG_INIT,
	TFT_STATE_DRAW_DIAG_LAST_CONNECT_TIME,
	TFT_STATE_DRAW_DIAG_RECHARGE_INFO,

	TFT_STATE_DRAW_SENSOR_INPUT,
	TFT_STATE_DRAW_TOTAL_ON_TIME,
	TFT_STATE_DRAW_STRING_LOCKED,
	TFT_STATE_CLEAR_STRING_LOCKED,


	TFT_STATE_DRAW_STRING_HOTG,
	TFT_STATE_DRAW_STRING_HOTR,
	TFT_STATE_CLEAR_STRING_HOT,
	TFT_STATE_FOTA_UPDATE,
	TFT_STATE_FOTA_FAIL,
	TFT_STATE_FOTA_SUCCESS,
	TFT_FOTA_FILE_DOWNLOAD_SIZE,
	TFT_STATE_DRAW_FLOW_RATE,
	TFT_STATE_CLEAR_SCREEN,
	TFT_STATE_DRAW_RO_PUMP_CURRENT,
	TFT_STATE_DRAW_TDS_SENSOR_IN_TEMPRATURE,
	TFT_STATE_DRAW_TDS_SENSOR_OUT_TEMPRATURE,
	TFT_STATE_DRAW_TDS_IN_SENSOR_VALUE_IN_PPM,
	TFT_STATE_DRAW_TDS_OUT_SENSOR_VALUE_IN_PPM,
	TFT_STATE_DRAW_AMC_TIMER
}	Enum_TFTState;

Enum_TFTState TFT_state;
extern void display_on_tftlcd(u32 dparam);
extern void MQTT_scheduled_communication(void);
enum pid
{
	PID_INVALID_PARAM                  = 0x00,
	PID_Hot_Water_Heater_Status_Byte   = 0x01,
	PID_Internal_Error_Code            = 0x02,
	PID_LOCK_PURIFIER                  = 0x03,
	PID_PURIFICATION_ON_COUNTER        = 0x04,
	PID_UNLOCK_PURIFIER                = 0x05,
	PID_RESET_BOOTLOADER_BTYE          = 0x06,
	PID_FLOW_RATE_ML_MINUTE            = 0x07,
	PID_WATER_UNIT_CODE                = 0x08,
	PID_WATER_DISPENSED_DATA           = 0x09,
	PID_WATER_BALANCE_RESET            = 0x0A,
	PID_SET_BOOTLOADER_BTYE            = 0x0B,
	PID_NOT_USED                       = 0x0C,
	PID_INITIATE_POWER_CYCLE           = 0x0D,
	PID_RO_PUMP_CURRENT_MA             = 0x10,
	PID_TDS_SENSOR_IN_TEMPRATURE       = 0X11,
	PID_TDS_SENSOR_OUT_TEMPRATURE      = 0X12,
	PID_TDS_SENSOR_IN                  = 0X13,
	PID_TDS_SENSOR_OUT                 = 0X14,
	PID_AMC_TIMER                      = 0X15
};


/***********************************************************************
 * ENUM TYPE DEFINITIONS
************************************************************************/
typedef enum
{
    HDLR_TYPE_CPMS_READ_CMD = 0,
    HDLR_TYPE_CPMS_SET_CMD = 1,
    HDLR_TYPE_CMGR_PDU_CMD = 2,
    HDLR_TYPE_CMGS_PDU_CMD = 3,

    //==> Warning: Please add new Handler Type upper this line.
    HDLR_TYPE_INVALID = 0xFFFFFFFF
} Enum_HdlrType;


/*-----	UART -----*/
#define UART_PORT_MAIN           UART_PORT1
#define BAUDRATE_PORT_MAIN       115200 //9600
#define UART_PORT_IPC            UART_PORT3
#define IPC_BAUDRATE             4800

/*-----	SPI TFT LCD Interface -----*/
#define USR_SPI_CHANNAL          (1)

extern u8 ssid;

extern char MId[18];
extern char latitude_[10] ;
extern char longitude_[10];

extern char IMEI[20];
extern char CCID[25];

extern u8 FOTA_URL[150];
extern u8 HTTP_LICENSE_URL_ADDR[150];

extern char m_apn[50];
extern char *m_userid;
extern char *m_passwd;

extern u8 GPRS_Connect_Failed_Attempts_Count;
//------Disgnostic releated variable--------
extern char service_provider_name[30];
extern char Operator_name[20];
extern char latlong[30];
//extern u16 Timer_On;
extern char deviceidentitydata[30];
extern char IMEINumber[20];
extern char CCIDNumber[25];
extern char control_module_identity_number[20];
extern u8 Control_Module_Indentity_data[16];

/************* Persistent parameters to store ***********/
extern char cmdrcbal[10];
extern char rcbal[10];
extern char RCID[25];
extern char RCTS[25];
extern char Received_RCID[25];
extern char Received_RCTS[25];
extern u32 water_balance;

extern char water_balance_buffer[7];
extern char water_balance_buffer_last[7];
extern char Water_dispensed_volume[7];
extern u16 ro_pump_current_in_mA;
extern u16 previous_ro_pump_current;
extern u16 water_flow_rate_ml_minute;
extern u16 previous_flowrate;
extern s8 RssiLevel;
extern s8 previous_rssival;
extern u8 em_error[2];
extern u8 tamper[2];
extern u8 em_error_E1[2];
extern u8 em_error_E5[2];
extern char time_stamp[18];
extern char Purification_Start_local_time[25];
extern char Purification_Stop_local_time[25];

extern s32 Write_And_Store_Pending_MQTT_Log_Messages(char* payload);
/*********************mqtt functions*****************/
extern s32 MQTT_Send_CONN_DISCONN_Request(u8 ssid, u8 action);
extern int MQTT_Send_Publish(u8 ssid, u16 MSG_type);
extern s32 SSL_Send_Data(u8 ssid, char * payload, s32 payloa_len);
extern void MQTT_Packet_Process(u8 ssid, char *buffer_mqtt_packet);
/*********************Secure_socket_********************/
extern s32 SSL_Recv_Data(u16 cid_ssid);

/*********************TFT Display functions*****************/
extern void Init_display(void);
extern void display_on_tftlcd(u32);
extern s32 Write_Error_Messages(void);
extern s32 Read_Fault_Flag_File(void);

extern u16 TDS_Sensor_In_Temp_value;
extern u16 previous_TDS_Sensor_In_Temp_value;
extern u16 TDS_Sensor_Out_Temp_value;
extern u16 previous_TDS_Sensor_Out_Temp_value;
extern u16 TDS_Sensor_In_value;
extern u16 previous_TDS_Sensor_In_value;
extern u16 TDS_Sensor_OUT_value;
extern u16 previous_TDS_Sensor_OUT_value;
extern u16 ro_pump_current_in_mA;
extern u16 previous_ro_pump_current;
extern u16 previous_amc_timer;
extern u16 amc_timer;

typedef enum FunReturn
{
	RET_FAIL = -1,RET_SUCCESS = 0
}Enum_RET;
/*-----	FLAGS	-----*/
typedef struct flags
{
		bool GPRS_ACTIVE;
		bool SIMCARD_AVAILABLE;
		bool SIMCARD_REGISTERED;
		bool RSSI_LOW_DETECT;
		bool CON_MSG_PUBLISH_PENDING;
		bool SKIP_POWER_OFF_ALERT;
		bool LOCATION_DATA_GATHERED;
		bool TANK_STATUS_IS_FULL;
		bool IPC_RESPONSE_PROCESSED;
		bool NON_RECOVERABLE_FAULT;
		bool DIGNOSTIC_MODE;
		bool CLOUD_COMMUNICATION_IN_PROGRESS;
		bool SCHEDULE_GPRS_RECONNECT_ATTEMPT;
		bool MQTT_CONNECTION_STATUS;
		bool MQTT_POWER_ON_MSG_PUBLISHED;
		bool MQTT_POWER_OFF_MSG_PUBLISHED;
		bool SSL_CONNECTION_STATE;
		bool DEVICE_RESET_SCHEDULED;
		bool WATER_SCREEN_INITIALIZED;
		bool CON_IDENTITY_RECEIVED;
		bool HEATER_PRESENT;
		bool SSL_X509_CERTPRESENT;
		bool SSL_CONFIGURED;
		bool AUTOPROVISIONING_COMPLETE;
		bool FLOW_RATE_OR_RO_PUMP_CURRENT;
		bool SPECIAL_ACTION_STARTED;
		bool DEVICE_NOT_REGISTERED;
		bool DEVICE_UNAUTHORISED;
		bool INTERNAL_ERROR_SET;
		bool SUSPEND_IPC_COMMUNICATION;
		bool READY_FOR_WATER_DATA_PUBLISH;
		bool POWER_OFF_MSG_PROCESSED;
		bool CON_STARTED_SUCCESS;
		bool FOTA_FILE_COM_FW_INVALID;
		bool FOTA_FILE_CON_FW_INVALID;
		bool DISPLAY_IMEI_OR_CCID;
		bool READY_TO_INITIATE_FOTA_DOWNLOAD;
		bool DP_CHANGE_NOTIFICATION_RECEIVED;
		bool TWIN_PROPERTIES_PROCESSED;
		bool LAST_FOTA_ATTEMPT_FAILED;
}FLAGS;
extern FLAGS FLAG;

extern s8 GetLocalTime_UTC(char* time_stamp_now, char* time_stamp_string_copy);

extern s32 GetNW_Time(void);
extern void Send_IPC_Command(int CmdID);
extern s32 GPRS_Activate(void);
extern s32 GPRS_Deactivate(void);
extern void FirmwareUpgrade(void);
extern void SET_RESET_BOOTLOADER_BTYE(unsigned char set_reset);
extern u8 READ_IDENTITY(void);
extern s32 InitIPComm(void);
extern s32 SSL_Check_SOC_State(u8 ssid);
extern void publish_data(u32);
extern void Init_Water_Balance_Screen(void);
extern void repaint_water_balance_screen(void);
extern void display_please_wait(void);
extern void display_FW_Version_Info(void);
extern u32 wtdid;
extern u8 configuration_Data_Write[3];
extern u32 IPC_CMD_REQ_source;
extern u32 PUBLISH_RESPONSE_REQ_source;

extern u32 Timer_SendTimeOut;
extern u32 Timer_SendTimeOut_Interval; // mSecextern
extern char lock_unlock_ack[1];
extern char last_connectedtime[25];
extern char Internal_error_code[4];

extern char AZURE_assigned_IoT_Hub[50];
extern char AZURE_IoT_device_ID[25];
extern char DEVICE_CLIENT_ID[25];
extern char AZURE_IoT_DPS_ID[25];
extern char DEVICE_USER_ID[100];
#define MAX_PAYLOADS                  46
#define PAYLOAD_LENGTH 200
static const char data_00A0[128] = {0x5F,0x3F,0x90,0x3F,0x96,0x72,0x09,0x00,0x8E,0x16,0xCD,0x60,0x65,0xB6,0x90,0xE7,
					 0x00,0x5C,0x4C,0xB7,0x90,0xA1,0x21,0x26,0xF1,0xA6,0x20,0xB7,0x88,0x5F,0x3F,0x90,
					 0xE6,0x00,0xA1,0x20,0x26,0x07,0x3F,0x8A,0xAE,0x40,0x00,0x20,0x0C,0x3F,0x8A,0xAE,
					 0x00,0x80,0x42,0x58,0x58,0x58,0x1C,0x80,0x00,0x90,0x5F,0xCD,0x60,0x65,0x9E,0xB7,
					 0x8B,0x9F,0xB7,0x8C,0xA6,0x20,0xC7,0x50,0x5B,0x43,0xC7,0x50,0x5C,0x4F,0x92,0xBD,
					 0x00,0x8A,0x5C,0x9F,0xB7,0x8C,0x4F,0x92,0xBD,0x00,0x8A,0x5C,0x9F,0xB7,0x8C,0x4F,
					 0x92,0xBD,0x00,0x8A,0x5C,0x9F,0xB7,0x8C,0x4F,0x92,0xBD,0x00,0x8A,0x72,0x00,0x50,
					 0x5F,0x07,0x72,0x05,0x50,0x5F,0xFB,0x20,0x04,0x72,0x10,0x00,0x96,0x90,0xA3,0x00};

static const char data_01A0[48] = {0x00,0x50,0x5F,0x07,0x72,0x05,0x50,0x5F,0xFB,0x20,0x24,0x72,0x10,0x00,0x97,0x20,
					 0x1E,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,
					 0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x81};

static const char data_0120[128] = {0x07,0x27,0x0A,0x90,0x5C,0x1D,0x00,0x03,0x1C,0x00,0x80,0x20,0xAE,0xB6,0x90,0xB1,
					 0x88,0x27,0x1C,0x5F,0x3C,0x90,0xB6,0x90,0x97,0xCC,0x00,0xC0,0x9D,0x9D,0x9D,0x9D,
					 0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x9D,0x81,
					 0xCD,0x60,0x65,0x5F,0x3F,0x97,0x72,0x0D,0x00,0x8E,0x18,0x72,0x00,0x00,0x94,0x0B,
					 0xA6,0x01,0xC7,0x50,0x5B,0x43,0xC7,0x50,0x5C,0x20,0x08,0x35,0x81,0x50,0x5B,0x35,
					 0x7E,0x50,0x5C,0x3F,0x94,0xF6,0x92,0xA7,0x00,0x8A,0x72,0x0C,0x00,0x8E,0x13,0x72,
					 0x00,0x50,0x5F,0x07,0x72,0x05,0x50,0x5F,0xFB,0x20,0x04,0x72,0x10,0x00,0x97,0xCD,
					 0x60,0x65,0x9F,0xB1,0x88,0x27,0x03,0x5C,0x20,0xDB,0x72,0x0D,0x00,0x8E,0x10,0x72};

#endif

/*
 * SecureSocket.h
 *
 *  Created on: Apr 22, 2017
 *      Author: SAI
 */

#ifndef SECURESOCKET_H_
#define SECURESOCKET_H_

//system header files
#include <string.h>
#include <stdlib.h>
//quectel ril header files
#include "ril.h"
#include "ril_util.h"
#include "ril_location.h"
#include "ril_telephony.h"
#include "ril_system.h"
#include "ril_http.h"
#include "ril_network.h"
//quectel general header file
#include "ql_stdlib.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_uart.h"
#include "ql_time.h"
#include "ql_system.h"
#include "ql_memory.h"
#include "ql_timer.h"
#include "ql_gprs.h"
#include "ql_socket.h"
#include "ql_fs.h"
#include "ql_wtd.h"
#include "ql_gpio.h"
#include "ql_memory.h"
//user defined application header files

#include "debug.h"
#include "common_vars.h"

//macros
#define SSL_SOC_MAX_DATA_LENGTH         1460        //1024
#define RECVD_DATA_LENGTH               1460*10//1024   // Max data can be recieved 12.5k. Tested

#define GPRS_PDP_CONTEXT                0

#define SSID_HOST_AZURE                 0
#define CID_HOST_AZURE                  0
#define CID_HOST_FOTA                   1

#define CONNECT_MODE_TRANSPERENT        0
#define SECURITY_LEVEL_2                2
#define SSL_VERSION_4                   4

//------------ Azure DPS HOST-----------
#define AZURE_DPS_HOST                  "global.azure-devices-provisioning.net"
#define AZURE_DPS_HOST_PORT             443
#define AZURE_IOTHUB_HOST_PORT         8883

//extern variables

extern char g_buf_recvd_data [RECVD_DATA_LENGTH];
extern s32 g_recvd_data_last_read_pos;

extern s32 g_recvd_data_len;
typedef struct{
	char *prefix;
	s32 data;
}ST_AT_TCPparam;

typedef enum{
	SSL_SOC_INITIAL,
	SSL_SOC_CONNECTING,
	SSL_SOC_CONNECTED,
	SSL_SOC_REMOTECLOSING,
	SSL_SOC_CLOSING,
	SSL_SOC_CLOSED
}Enum_SSL_SOC_STATE;


typedef struct{
	char *prefix;
	s32 length;
	s32 return_val;
}SSL_SendParam;

//Below Structure is used in function store/read license file one by one
typedef struct{
	u8 *cert_type;
	u8 *cert_name;
	u32 cert_len;
	s32 errCode;
}SSL_WriteCert;


typedef struct{
	s32 ssid;
	Enum_SSL_SOC_STATE sslSocState;
}SSL_SocStateParam;


typedef struct{
	// char prefix[1];
	s32 length;
	bool err_state;
}SSL_RecvParam;

//extern Functions
extern s32 TCP_Socket_Read_Data_SSL (u8* buff, s32 len);

#endif

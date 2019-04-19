
//header files
#include "common_vars.h"
#include "SecureSocket.h"
#include "cJSON.h"
//global declaration
char g_buf_recvd_data [RECVD_DATA_LENGTH] = "\0";
s32 g_recvd_data_len = 0;
u8 write_flag = FALSE;
// SSL_RecvParam NULL_sslrecvParam = {"\0", 0};
SSL_RecvParam NULL_sslrecvParam = {0};

//function prototypes
s32 SSL_Check_SOC_State(u8 ssid);
s32 SSL_Send_Data(u8 ssid, char *payload, s32 payloa_len);
s32 RIL_SSL_QSSLSTATE(SSL_SocStateParam *ssl_state_data) ;
s32 RIL_SSL_QSSLCLOSE(u8 ssid);
s32 RIL_SSL_QSSLRECV(u16 cid_ssid, SSL_RecvParam *recvd_data);
s32 RIL_SSL_QSSLCFG(void);
s32 RIL_SSL_QSSLOPEN(u8* hostName, u32 port,u8 ssid,u8 ctxindex, u8 mode) ;
s32 RIL_SSL_QSSLCERTIFICATE_WRITE(u8 *cert_type, u8 *cert_name, u32 cert_len, u8 timeout);
s32 RIL_SSL_QSSLCERTIFICATE_DEL(u8 *cert_type);
s32 RIL_SSL_QSSLCERTIFICATE_READ(u8 *cert_type);
s32 RIL_SSL_QSSLSEND(u8 ssid,u8 *data,u32 length) ;
static s32 ATResponse_SSL_handler_open(char* line, u32 len, void* userdata);
static s32 ATResponse_SSL_handler_send(char* line, u32 len, void* userdata);
static s32 ATResponse_SSL_handler_recv(char* line, u32 len, void* userdata);
static s32 ATResponse_SSL_handler_state(char* line, u32 len, void* userdata);
static s32 ATResponse_SSL_handler_close(char* line, u32 len, void* userdata);
static s32 ATResponse_SSL_handler_CERTIFICATE_WRITE(char* line, u32 len, void* userdata);
static s32 ATResponse_SSL_handler_CERTIFICATE_DEL(char* line, u32 len, void* userdata);
static s32 ATResponse_SSL_handler_CERTIFICATE_READ(char* line, u32 len, void* userdata);
static s32 ATResponse_QSSLCFG_handler(char* line, u32 len, void* userdata);

s32 SSL_Recv_Data(u16 cid_ssid);

s32 GPRS_Activate(void);
s32 GPRS_Deactivate(void);
s32 Connect_Azure_IoT_host_socket(u8 host_id);
s32 Connect_Azure_IoT(void);



#ifdef SECURE_SOCKET
char AZURE_DPS_enrollment_group_ID[25] = {'\0'};
char AZURE_DPS_registration_ID[25]= {'\0'};
char AZURE_DPS_Status[25]= {'\0'};
char AZURE_DPS_Sub_Status[25]= {'\0'};
#endif

extern char AZURE_assigned_IoT_Hub[50];
extern char AZURE_IoT_device_ID[25];
extern char AZURE_IoT_DPS_ID[25];

extern char DEVICE_CLIENT_ID[25];
extern char DEVICE_USER_ID[100];

char REST_MESSAGE_POST[300]= {'\0'};
char REST_MESSAGE_PUT[300] = {'\0'};

/*
#define REST_MESSAGE_POST_STRING  "POST /0ne00031BAC/registrations/NOTYETREGISTERD?api-version=2018-09-01-preview HTTP/1.1\r\n\
Host: global.azure-devices-provisioning.net:443\r\n\
Content-Type: application/json\r\n\
Content-Length: 38\r\n\
\r\n\
{\"registrationId\":\"NOTYETREGISTERD\"}\r\n"

#define REST_MESSAGE_PUT_STRING  "PUT /0ne00031BAC/registrations/NOTYETREGISTERD/register?api-version=2018-09-01-preview HTTP/1.1\r\n\
Host: global.azure-devices-provisioning.net:443\r\n\
Content-Type: application/json\r\n\
Content-Length: 38\r\n\
\r\n\
{\"registrationId\":\"NOTYETREGISTERD\"}\r\n"

*/
static s32 Azure_DPS_Response_Process(void);
extern void Refresh_Communication_inprogress_Symbol(unsigned int colour);
/*****************************************************************
 * Function: s32 RIL_SSL_QSSLCERTIFICATE_DEL(u8 *cert_type)
 *
 * Description: This function delete all three certificates if available
 *		Certificates will be deleted from EEPROM area
 * Parameters: none
 *
 * Return: SUCCESS , FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLCERTIFICATE_DEL(u8 *cert_type)
{
	char strAT[300];
	s32  ret = RIL_AT_FAILED;
	s32  errCode = RIL_AT_FAILED;
	Ql_memset(strAT, 0, sizeof(strAT));

	//Delete certificate irrespective of whether available or not
	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QSECDEL=\"NVRAM:%s\"\n", cert_type);
	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_SSL_handler_CERTIFICATE_DEL,&errCode,0);

#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
	return ret;
}
/*****************************************************************
 * Function: s32 RIL_SSL_QSSLCERTIFICATE_WRITE(u8 *cert_type, u8 *cert_name, u32 cert_len, u8 timeout)
 *
 * Description: This function writes the certificate
 *		Certificates will be written in NVRAM area
 * Parameters: None
 *
 * Return: SUCCESS, FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLCERTIFICATE_WRITE(u8 *cert_type, u8 *cert_name, u32 cert_len, u8 timeout)
{
	char strAT[300];
	s32  ret = RIL_AT_SUCCESS;
	SSL_WriteCert sslwriteCert;

	sslwriteCert.cert_name = cert_name;
	sslwriteCert.cert_type = cert_type;
	sslwriteCert.cert_len = cert_len;
	sslwriteCert.errCode = RIL_AT_FAILED;

	//Upload certificate
	Ql_memset(strAT, 0, sizeof(strAT));
	write_flag= 1;
	Ql_sprintf(strAT, "AT+QSECWRITE=\"NVRAM:%s\",%d,%d\n",sslwriteCert.cert_type, sslwriteCert.cert_len, timeout);
	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT), ATResponse_SSL_handler_CERTIFICATE_WRITE, (void *)&sslwriteCert, 0);

#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif

	return ret;
}
/*****************************************************************
 * Function: s32 RIL_SSL_QSSLCERTIFICATE_READ(u8 *cert_type)
 *
 * Description: This function Read the certificate and checks their integrity
 *		Certificate will be read from NVRAM area
 * Parameters: None
 *
 * Return: SUCCESS, FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLCERTIFICATE_READ(u8 *cert_type)
{
	char strAT[300];
	s32  ret = RIL_AT_SUCCESS;
	ST_AT_TCPparam tcpParam;

	tcpParam.prefix="+QSECREAD:";
	tcpParam.data = 255;

	// Check Certificate
	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QSECREAD=\"NVRAM:%s\"\n", cert_type);
	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_SSL_handler_CERTIFICATE_READ,&tcpParam,0);

//#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
//#endif

	return ret;
}

/*****************************************************************
 * Function: s32 RIL_SSL_QSSLCFGL(void)
 *
 * Description: This function configures all the parameters required for SSL connection to establish
 *		Certificates will be used from EEPROM area
 * Parameters: none
 *
 * Return: SUCCESS , FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLCFG(void)
{
	s32 ret = RIL_AT_SUCCESS;
	char strAT[300];
	if(FLAG.SSL_CONFIGURED == FALSE) // Configure the SSL parameyters only if they are not configured earlier
	{
#ifdef SECURE_SOCKET
		APP_DEBUG("\n\r<-- Configure SSL parameters -->\r\n");
#endif
		//Configure root Certificate
		Ql_memset(strAT, 0, sizeof(strAT));
		Ql_sprintf(strAT, "AT+QSSLCFG=\"cacert\",%d,\"NVRAM:CA0\"\n",CID_HOST_AZURE);

		ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef SECURE_SOCKET
		APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif

		//Configure Client Certificate
		if(ret == RIL_AT_SUCCESS)
		{
			Ql_memset(strAT, 0, sizeof(strAT));
			Ql_sprintf(strAT, "AT+QSSLCFG=\"clientcert\",%d,\"NVRAM:CC0\"\n",CID_HOST_AZURE);

			ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef SECURE_SOCKET
			APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
		}

		//Configure Client key Certificate
		if(ret == RIL_AT_SUCCESS)
		{
			Ql_memset(strAT, 0, sizeof(strAT));
			Ql_sprintf(strAT, "AT+QSSLCFG=\"clientkey\",%d,\"NVRAM:CK0\"\n",CID_HOST_AZURE);

			ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef SECURE_SOCKET
			APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
		}

		//Enable Chain certificate
		if(ret == RIL_AT_SUCCESS)
		{
			Ql_memset(strAT, 0, sizeof(strAT));
			Ql_sprintf(strAT, "AT+QSSLCFG=\"certchain\",1\n");

			ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef SECURE_SOCKET
			APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
		}

		//Ignore certificate time
		if(ret == RIL_AT_SUCCESS)
		{
			Ql_memset(strAT, 0, sizeof(strAT));
			Ql_sprintf(strAT, "AT+QSSLCFG=\"ignorertctime\",1\n");

			ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef SECURE_SOCKET
			APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
		}

		//Set SSL version
		if(ret == RIL_AT_SUCCESS)
		{
			Ql_memset(strAT, 0, sizeof(strAT));
			Ql_sprintf(strAT, "AT+QSSLCFG=\"sslversion\",%d,%d\n",CID_HOST_AZURE,SSL_VERSION_4);

			ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef SECURE_SOCKET
			APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
		}

		//Set Sec_level
		if(ret == RIL_AT_SUCCESS)
		{
			Ql_memset(strAT, 0, sizeof(strAT));
			Ql_sprintf(strAT, "AT+QSSLCFG=\"seclevel\",%d,%d\n",CID_HOST_AZURE,SECURITY_LEVEL_2);

			ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef SECURE_SOCKET
			APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
		}
/*
		//Query the setting of context just to confirm everything is correct
		if(ret == RIL_AT_SUCCESS)
		{
			Ql_memset(strAT, 0, sizeof(strAT));
			Ql_sprintf(strAT, "AT+QSSLCFG=\"ctxindex\",%d\n",CID_HOST_AZURE);

			ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_QSSLCFG_handler,NULL,0);
#ifdef SECURE_SOCKET
			APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
		}
*/
		if(ret == RIL_AT_SUCCESS)
			FLAG.SSL_CONFIGURED = TRUE;// Mark the respective flag as true
	}
	return ret;
}
/*****************************************************************
 * Function: s32 RIL_SSL_QSSLOPEN(u8* hostName, u32 port,u8 ssid,u8 ctxindex, u8 mode)
 *
 * Description: This function opens a secured SSL session using the parameters set before
 *
 * Parameters:
 *
 * Return: SUCCESS , FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLOPEN(u8* hostName, u32 port,u8 ssid,u8 ctxindex, u8 mode)
{
	u32 timeout = 300;	// Sec. Default 90
	s32 ret = RIL_AT_SUCCESS;
	ST_AT_TCPparam tcpParam;
	char strAT[200];

	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QSSLOPEN=%d,%d,\"%s\",%d,%d\n", ssid,ctxindex,hostName, port,mode, timeout);
	tcpParam.prefix="+QSSLOPEN:";
	tcpParam.data = 255;

	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_SSL_handler_open,(void* )&tcpParam,0);
#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
	if(ret != RIL_AT_SUCCESS)
	{
		return ret;
	}
	else if(tcpParam.data != 0)
	{
		FLAG.SSL_CONNECTION_STATE = TRUE;
	}
	return tcpParam.data;
}

/*****************************************************************
 * Function: s32 RIL_SSL_QSSLSEND(u8 ssid,u8 *data,u32 length)
 *
 * Description: This function send a payload data of length on prescribed ssid
 *
 * Parameters: u8 ssid,u8 *data,u32 length
 *
 * Return: SUCCESS , FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLSEND(u8 ssid,u8 *data,u32 length)
{
	SSL_SendParam sslsendParam;
	s32 ret = RIL_AT_SUCCESS;
	char strAT[200];

	Ql_memset(strAT, 0, sizeof(strAT));
	sslsendParam.prefix=data;
	sslsendParam.length= length;
	Ql_sprintf(strAT, "AT+QSSLSEND=%d,%d\n", ssid,length);
	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_SSL_handler_send,(void* )&sslsendParam,0);
#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
	return ret;
}
/*****************************************************************
 * Function: s32 RIL_SSL_QSSLRECV(u16 cid_ssid, SSL_RecvParam *recvd_data)
 *
 * Description: This function receives a payload data of precribed cid and ssid
 *
 * Parameters: u8 ssid,u8 *data,u32 length
 *
 * Return: SUCCESS , FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLRECV(u16 cid_ssid, SSL_RecvParam *recvd_data)
{
	s32 ret = RIL_AT_SUCCESS;
	char strAT[200];

	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QSSLRECV=%d,%d,%d\n", (cid_ssid>>8) ,(cid_ssid & 0xFF),SSL_SOC_MAX_DATA_LENGTH); // Higher byte of cid_ssid is cid and lower byte is ssid
	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_SSL_handler_recv,recvd_data,0);
#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
	return ret;
}
/*****************************************************************
 * Function: RIL_SSL_QSSLSTATE(SSL_SocStateParam *ssl_state_data)
 *
 * Description: This function check the state of the SSL session
 *
 * Parameters:
 *
 * Return: SUCCESS , FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLSTATE(SSL_SocStateParam *ssl_state_data)
{
	s32 ret = RIL_AT_SUCCESS;
	char strAT[200];

	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QSSLSTATE\n");

	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_SSL_handler_state,ssl_state_data,0);
#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
	return ret;
}
/*****************************************************************
 * Function:RIL_SSL_QSSLCLOSE(u8 ssid)
 *
 * Description: This function close the SSL session
 *
 * Parameters:
 *
 * Return: SUCCESS , FAIL
 *
 *****************************************************************/
s32 RIL_SSL_QSSLCLOSE(u8 ssid)
{
	s32 ret = RIL_AT_SUCCESS;
	 char strAT[200];

	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QSSLCLOSE=%d\n", ssid);
	ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_SSL_handler_close,NULL,0);
#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
#endif
	return ret;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
//---------------Call Back Functions from this point forward-----------------------------
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/*****************************************************************
 * Function: ATResponse_SSL_handler_CERTIFICATE_DEL(char* line, u32 len, void* userdata)
 *
 * Description: This function is automatically called when AT command for certificate DELETE is executed
 *
 * Parameters: none
 *
 * Return: see function below
 *
 *****************************************************************/
static s32 ATResponse_SSL_handler_CERTIFICATE_DEL(char* line, u32 len, void* userdata)
{
	char* pHead = NULL;
#ifdef AT_CMD_MSG
	APP_DEBUG("%s\r\n",line);
#endif
	pHead = Ql_RIL_FindString(line, len, "OK");
	if (pHead)
	{
		return RIL_ATRSP_SUCCESS;
	}
	pHead = Ql_RIL_FindString(line, len, "ERROR");
	if (pHead)
	{
		if (userdata != NULL)
		{
			*((s32*)userdata) = RIL_AT_FAILED;
		}
		return RIL_ATRSP_FAILED;
	}
	pHead = Ql_RIL_FindString(line, len, "+CME ERROR:");
	if (pHead)
	{
		if (userdata != NULL)
		{
			Ql_sscanf(line, "%*[^: ]: %d[^\r\n]", (s32*)userdata);
		}
		return RIL_ATRSP_FAILED;
	}
	return RIL_ATRSP_CONTINUE;	// Just wait for the specified results above
}

/*****************************************************************
 * Function: ATResponse_SSL_handler_CERTIFICATE_WRITE(char* line, u32 len, void* userdata)
 *
 * Description: This function is automatically called when AT command for Certificate WRITE is executed
 *
 * Parameters: none
 *
 * Return: see function below
 *
 *****************************************************************/
static s32 ATResponse_SSL_handler_CERTIFICATE_WRITE(char* line, u32 len, void* userdata)
{
	SSL_WriteCert *sslwriteCert = (SSL_WriteCert *)userdata;
	char* pHead = NULL;
//#ifdef AT_CMD_MSG
	APP_DEBUG("%s\r\n",line);
//#endif
	pHead = Ql_RIL_FindString(line, len, "CONNECT");
	if (pHead)
	{
		Ql_Sleep(1000);//Do not move
		if (write_flag == 1)
		{
			Ql_RIL_WriteDataToCore((u8*)sslwriteCert->cert_name, sslwriteCert->cert_len);
		}
		return RIL_ATRSP_CONTINUE;	// wait for OK
	}
	pHead = Ql_RIL_FindString(line, len, "+QSECWRITE:");
	if (pHead)
	{
		return RIL_ATRSP_CONTINUE;
	}
	pHead = Ql_RIL_FindString(line, len, "OK");
	if (pHead)
	{
		return RIL_ATRSP_SUCCESS;
	}
	pHead = Ql_RIL_FindString(line, len, "ERROR");
	if (pHead)
	{
		if (sslwriteCert->errCode != NULL)
		{
			*((s32*)sslwriteCert->errCode) = RIL_AT_FAILED;
		}
		return RIL_ATRSP_FAILED;
	}
	pHead = Ql_RIL_FindString(line, len, "+CME ERROR:");
	if (pHead)
	{
		if (sslwriteCert->errCode != NULL)
		{
			Ql_sscanf(line, "%*[^: ]: %d[^\r\n]", (s32*)sslwriteCert->errCode);
		}
		return RIL_ATRSP_FAILED;
	}
	return RIL_ATRSP_CONTINUE;	// Just wait for the specified results above
}

/*****************************************************************
 * Function: ATResponse_SSL_handler_CERTIFICATE_READ(char* line, u32 len, void* userdata)
 *
 * Description: This function is automatically called when AT command for Data READ is executed
 *
 * Parameters: none
 *
 * Return: see function below
 *
 *****************************************************************/
static s32 ATResponse_SSL_handler_CERTIFICATE_READ(char* line, u32 len, void* userdata)
{
	ST_AT_TCPparam *tcpParam = (ST_AT_TCPparam *)userdata;
	char *head = Ql_RIL_FindString(line, len, tcpParam->prefix); //continue wait
//#ifdef AT_CMD_MSG
	APP_DEBUG("ATResponse_SSL_handler_CERTIFICATE_READ %s\r\n",line);
//#endif
	if(head)
	{
		char strTmp[10];
		char* p1 = NULL;
		char* p2 = NULL;
		Ql_memset(strTmp, 0x0, sizeof(strTmp));
		p1 = Ql_strstr(head, ":");
		p2 = Ql_strstr(p1 + 1, ",");
		if (p1 && p2)
		{
			Ql_memcpy(strTmp, p1 + 2, p2 - p1 - 2);
			tcpParam->data= Ql_atoi(strTmp);
		}
		return  RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "OK");
	if(head)
	{
		return  RIL_ATRSP_SUCCESS;
	}
	head = Ql_RIL_FindString(line, len, "ERROR");
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}
	head = Ql_RIL_FindString(line, len, "+CME ERROR:");//fail
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}
	head = Ql_RIL_FindString(line, len, "+CMS ERROR:");//fail
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}
	return RIL_ATRSP_CONTINUE; //continue wait
}

static s32 ATResponse_SSL_handler_open(char* line, u32 len, void* userdata)
{
	ST_AT_TCPparam *tcpParam = (ST_AT_TCPparam *)userdata;

	char *head = Ql_RIL_FindString(line, len, tcpParam->prefix); //continue wait
#ifdef AT_CMD_MSG
	APP_DEBUG("%s\r\n",line);
#endif
	if(head)
	{
		char strTmp[10];
		char* p1 = NULL;
		char* p2 = NULL;
		Ql_memset(strTmp, 0, sizeof(strTmp));
		p1 = Ql_strstr(head, ":");
		p2 = Ql_strstr(p1 + 1, "\r\n");
		if (p1 && p2)
		{
			Ql_memcpy(strTmp, p1 + 4, p2 - p1 - 4);
			tcpParam->data= Ql_atoi(strTmp);
		}
		if(tcpParam->data == 0)
			return  RIL_ATRSP_SUCCESS;
		else
			return  RIL_ATRSP_FAILED;
	}
	head = Ql_RIL_FindString(line, len, "OK");
	if(head)
	{
		return  RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "ERROR");
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}
	head = Ql_RIL_FindString(line, len, "+CME ERROR:");//fail
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}
	head = Ql_RIL_FindString(line, len, "+CMS ERROR:");//fail
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}
	return RIL_ATRSP_CONTINUE; //continue wait
}

static s32 ATResponse_SSL_handler_recv(char* line, u32 len, void* userdata)
{
	SSL_RecvParam *sslrecvParam = (SSL_RecvParam *)userdata;

	char *head ;

#ifdef AT_CMD_MSG
	APP_DEBUG("%s\r\n",line); // If enabled this debug message there will be error in buffer filling in steps below :-) so use this debug only to varify the received data..
#endif
	head = Ql_RIL_FindString(line, len, "\r\n+QSSLRECV:");
	if(head)
	{
		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "ERROR");
	if(head)
	{
		sslrecvParam->err_state = TRUE;
		return  RIL_ATRSP_FAILED;
	}

	head = Ql_RIL_FindString(line, len, "HTTP"); // For DPS resonse on HTTP we need to ignore few header.  Special exlcision to continue because "OK" string is included in this message :-)
	if(head)
	{
		sslrecvParam->length += len;

		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "Date:"); // Ignore Date header
	if(head)
	{
		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "Content-Type:"); // Ignore content type header
	if(head)
	{
		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "Transfer-Encoding:"); // Ignore Transfer-Encoding header
	if(head)
	{
		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "x-ms-request-id:"); // Ignore request-id header
	if(head)
	{
		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "Strict-Transport-Security:"); // Ignore security header
	if(head)
	{
		return RIL_ATRSP_CONTINUE;
	}
	//------- END of DPS header packets received on HTTP , identify and ignore-----------
	head = Ql_RIL_FindString(line, len, "OK");
	if(head)
	{	// count '6' means last 6 bytes "<CR><LF>OK<CR><LF>"
		if(len < 6 )
		{
			sslrecvParam->length += (6-len);
			for(int i=0; i<(6-len); i++)
			{
				g_recvd_data_len--;
				g_buf_recvd_data[g_recvd_data_len] = '\0';

			}
		}
		else
		{
			sslrecvParam->length += (len-6);

			for(int i=0; i<(len-6); i++, g_recvd_data_len++)
			{
				g_buf_recvd_data[g_recvd_data_len] = line[i];
			}

		}
		// Ql_memcpy(userdata,line,g_recvd_data_len);

		return  RIL_ATRSP_SUCCESS;
	}
	head = Ql_RIL_FindString(line, len, "+CMS ERROR:");//fail
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}

	// Non of above condition means Only DATA is in buffer. Read it.
	{
		// Ql_memcpy(userdata,line,len);
		sslrecvParam->length += len;

		for(int i=0; i<len; i++, g_recvd_data_len++)
		{
			g_buf_recvd_data[g_recvd_data_len] = line[i];

		}

	}
	return RIL_ATRSP_CONTINUE; //continue wait
}

static s32 ATResponse_SSL_handler_state(char* line, u32 len, void* userdata)
{
	SSL_SocStateParam *sslsocStateParam = (SSL_SocStateParam *)userdata;
	char tmp_buf[15] = "\0";

	char *head = Ql_RIL_FindString(line, len, "\r\n+QSSLSTATE:");
	if(head)
	{
		Ql_sprintf(tmp_buf, "\r\n+QSSLSTATE: %d", sslsocStateParam->ssid);
		head = Ql_RIL_FindString(line, len, tmp_buf);
		if(head)
		{
			head = Ql_RIL_FindString(line, len, "INITIAL");
			if(head)
			{
				sslsocStateParam->sslSocState = SSL_SOC_INITIAL;
				return RIL_ATRSP_CONTINUE;
			}
			head = Ql_RIL_FindString(line, len, "CONNECTING");
			if(head)
			{
				sslsocStateParam->sslSocState = SSL_SOC_CONNECTING;
				return RIL_ATRSP_CONTINUE;
			}
			head = Ql_RIL_FindString(line, len, "CONNECTED");
			if(head)
			{
				sslsocStateParam->sslSocState = SSL_SOC_CONNECTED;
				return RIL_ATRSP_CONTINUE;
			}
			head = Ql_RIL_FindString(line, len, "REMOTE CLOSING");
			if(head)
			{
				sslsocStateParam->sslSocState = SSL_SOC_REMOTECLOSING;
				return RIL_ATRSP_CONTINUE;
			}
			head = Ql_RIL_FindString(line, len, "CLOSING");
			if(head)
			{
				sslsocStateParam->sslSocState = SSL_SOC_CLOSING;
				return RIL_ATRSP_CONTINUE;
			}
			head = Ql_RIL_FindString(line, len, "CLOSED");
			if(head)
			{
				sslsocStateParam->sslSocState = SSL_SOC_CLOSED;
				return RIL_ATRSP_CONTINUE;
			}
		}

		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "ERROR");
	if(head)
	{
		return  RIL_ATRSP_FAILED;
	}
	head = Ql_RIL_FindString(line, len, "OK");
	if(head)
	{
		return  RIL_ATRSP_SUCCESS;
	}

	return RIL_ATRSP_CONTINUE; //continue wait
}

static s32 ATResponse_SSL_handler_send(char* line, u32 len, void* userdata)
{
	SSL_SendParam *sslsendParam = (SSL_SendParam *)userdata;
	char *head ;

	head = Ql_RIL_FindString(line, len, "\r\n>");
	if(head)
	{
		Ql_RIL_WriteDataToCore ((u8 *)sslsendParam->prefix,sslsendParam->length);
		sslsendParam->return_val = RIL_ATRSP_CONTINUE;
		return RIL_ATRSP_CONTINUE;
	}
	head = Ql_RIL_FindString(line, len, "ERROR");
	if(head)
	{
		sslsendParam->return_val = RIL_ATRSP_FAILED;
		return  RIL_ATRSP_FAILED;
	}
	head = Ql_RIL_FindString(line, len, "SEND OK");//fail
	if(head)
	{
		sslsendParam->return_val = RIL_ATRSP_SUCCESS;
		return  RIL_ATRSP_SUCCESS;
	}
	head = Ql_RIL_FindString(line, len, "+CMS ERROR:");//fail
	if(head)
	{
		sslsendParam->return_val = RIL_ATRSP_FAILED;
		return  RIL_ATRSP_FAILED;
	}

	sslsendParam->return_val = RIL_ATRSP_CONTINUE;
	return RIL_ATRSP_CONTINUE; //continue wait
}

static s32 ATResponse_SSL_handler_close(char* line, u32 len, void* userdata)
{
	char *head = Ql_RIL_FindString(line, len, "CLOSE OK");
	if(head)
	{
		return	RIL_ATRSP_SUCCESS;
	}
	head = Ql_RIL_FindString(line, len, "ERROR:");//fail
	if(head)
	{
		return	RIL_ATRSP_FAILED;
	}

	return RIL_ATRSP_FAILED; //continue wait
}

static s32 ATResponse_QSSLCFG_handler(char* line, u32 len, void* userdata)
{
	char *ret = NULL;
	ret = Ql_strstr(line,"+QSSLCFG: ");
#ifdef AT_CMD_MSG
		APP_DEBUG("%s\r\n",line);
#endif
	if (Ql_RIL_FindLine(line, len, "OK"))
	{
		return RIL_ATRSP_SUCCESS;
	}
	else if (Ql_RIL_FindLine(line, len, "ERROR") || Ql_RIL_FindString(line, len, "+CME ERROR:")|| Ql_RIL_FindString(line, len, "+CMS ERROR:"))
	{
		return RIL_ATRSP_FAILED;
	}

	return RIL_ATRSP_CONTINUE; //continue to wait

}

s32 SSL_Send_Data(u8 ssid, char * payload, s32 payloa_len)
{
	s32 ret = 0;
	s32 data_send_len = 0;
	s32 offset = 0;

	SEND_REMAIN:
	if(payloa_len > SSL_SOC_MAX_DATA_LENGTH)
	{
		data_send_len = SSL_SOC_MAX_DATA_LENGTH;
	}
	else{
		data_send_len = payloa_len;
	}

	//Check Server connected or not and do a reattempt to connect is found not connected
	if(SSL_SOC_CONNECTED != SSL_Check_SOC_State(ssid)) // State is not connected.. attempt to reconnect
	{
#ifdef SECURE_SOCKET
		APP_DEBUG("<-- Reconnecting Socket..- ");
		if(ssid == SSID_HOST_AZURE)
			APP_DEBUG("AZURE_IOT_HUB -->\r\n");

#endif
		FLAG.SSL_CONNECTION_STATE = FALSE; // Clear the flag before re-attempting the connection
		ret = Connect_Azure_IoT_host_socket(ssid);
	}

	if(ret == 0)
		ret = RIL_SSL_QSSLSEND(ssid, ((u8 *)payload + offset) ,(u32)data_send_len);

	if(ret != RIL_ATRSP_SUCCESS) //If send is failed return from here..
	{
		return ret;
	}
	else
	{
		offset += data_send_len;
		payloa_len = payloa_len-data_send_len;

		if(payloa_len != 0)
		{
			Ql_Sleep(1250);
			goto SEND_REMAIN;
		}
	}
	return ret;
}
/*****************************************************************
 * Function: s32 SSL_Recv_Data(u16 cid_ssid)
 *
 * Description: This function reads the response received from respective socket
 *
 * Parameters: Cid and ssid
 *
 * Return: SUCCESS, FAIL
 *
 *****************************************************************/
s32 SSL_Recv_Data(u16 cid_ssid)
{
	s32 ret = -1;
	SSL_RecvParam sslrecvParam = NULL_sslrecvParam;

	Ql_memset(g_buf_recvd_data, 0x0, sizeof(g_buf_recvd_data));

	ret = RIL_SSL_QSSLRECV(cid_ssid, &sslrecvParam);
	if(ret == RIL_ATRSP_SUCCESS) //If message received ..then proceess it
	{
		if(FLAG.AUTOPROVISIONING_COMPLETE == FALSE) // Response received from Azure DPS as a response to DPS autoprovisioning REST API message sent earlier
		{
#ifdef SECURE_SOCKET
			APP_DEBUG("\n\r<-- Read AZURE_DPS message -->\r\n");
#endif
			ret = Azure_DPS_Response_Process();

			Ql_memset(g_buf_recvd_data, 0x0, sizeof(g_buf_recvd_data));
		}
		else // Response is received from Azure IoT Hub
		{
#ifdef SECURE_SOCKET
			APP_DEBUG("\n\r<-- Read AZURE_IOTHUB message -->\r\n");
#endif
			Ql_OS_SendMessage(0,MSG_ID_GW_STATE,GW_STATE_PROCESS_SUBCRIBED_DATA, (cid_ssid & 0xFF));
		}
	}

	g_recvd_data_len = 0;

	return ret;
}

s32 SSL_Check_SOC_State(u8 ssid)
{
	s32 ret = -RIL_AT_FAILED;
	SSL_SocStateParam sslstateParam;

	sslstateParam.ssid = ssid;
	ret = RIL_SSL_QSSLSTATE(&sslstateParam); // The function call back will change the sslstateParam.sslSocState parameter state

	if(ret == 0)
		ret = sslstateParam.sslSocState; // Assign actual socket status when returned a AT command execution success..

#ifdef DEBUG_MSG
	if(ssid == SSID_HOST_AZURE)
	{
		APP_DEBUG("<-- SSL Socket status:");
	}

	if(sslstateParam.sslSocState == 0)
	{
		APP_DEBUG(" Initial -->\r\n");
	}
	else if(sslstateParam.sslSocState == 1)
	{
		APP_DEBUG(" Opening -->\r\n");
	}
	else if(sslstateParam.sslSocState == 2)
	{
		APP_DEBUG(" Connected -->\r\n");
	}
	else if(sslstateParam.sslSocState == 4)
	{
		APP_DEBUG(" Closing -->\r\n");
	}
#endif

	return ret;
}


/*****************************************************************
 * Function: s32 GPRS_Deactivate(void)
 *
 * Description: This function close a GPRS communication channel
 *
 * Parameters: None
 *
 * Return: SUCCESS, FAIL
 *
 *****************************************************************/
s32 GPRS_Deactivate(void)
{
	s32 ret = RIL_AT_FAILED;

	ret = RIL_NW_ClosePDPContext();
#ifdef SECURE_SOCKET
	APP_DEBUG("<-- [%s]GPRS Deactivate -->\r\n",(ret==0)?"SUCCESS":"FAIL");
#endif
	FLAG.GPRS_ACTIVE = FALSE;

	return ret;
}
/*****************************************************************
 * Function: s32 Connect_Azure_IoT(void)
 *
 * Description: This function First connects to Azure DPS service to collect assigned IOT Hub information ( only once after power up)
 *              Then connects to Azure assigned IoT hub
 * Parameters: None
 *
 * Return: SUCCESS, FAIL
 *
 *****************************************************************/
s32 Connect_Azure_IoT(void)
{
	s32 ret = RIL_AT_FAILED;

	if(FLAG.GPRS_ACTIVE == FALSE ) // Do not try to connect if GPRS state is not active
		return ret;

	//X509 certificates should be already present when code comes, Just to make it sure by re-confirm
	if(FLAG.SSL_X509_CERTPRESENT == FALSE)
		return ret;

	// SSL parameters need to be configured once before first connection attempt
	if(FLAG.SSL_CONFIGURED == FALSE)
		ret = RIL_SSL_QSSLCFG();

	//The flag will become true when successful, Check if SSL configutation is complete or not
	if(FLAG.SSL_CONFIGURED == FALSE)
		return ret;

	//Open SSL session
	//Check if autoprovisioning is complete or not..
	if(FLAG.AUTOPROVISIONING_COMPLETE == FALSE) // Auto provisioning is not done so do it in first place.
	{
		//Close any earlier open unresponded sessions if any
		RIL_SSL_QSSLCLOSE(SSID_HOST_AZURE);
		FLAG.SSL_CONNECTION_STATE = FALSE;

		// if Connection is closed then establish it a fresh
		if(FLAG.SSL_CONNECTION_STATE ==  FALSE)// if Connection is closed then establish it
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r<-- Open SSL connection - Azure DPS : ");
#endif
			Refresh_Communication_inprogress_Symbol(colGray);//colGray;colOrangeRed

			ret = RIL_SSL_QSSLOPEN(AZURE_DPS_HOST, AZURE_DPS_HOST_PORT, SSID_HOST_AZURE, CID_HOST_AZURE, CONNECT_MODE_TRANSPERENT);
			FLAG.SSL_CONNECTION_STATE = (ret == RIL_AT_SUCCESS) ? TRUE : FALSE ;
#ifdef DEBUG_MSG
			if(ret == RIL_AT_SUCCESS)
			{
				APP_DEBUG("Success-->\n\r");
			}	
			else
			{	
				APP_DEBUG("Fail-->\n\r");
			}	
#endif
		}
		//When Session is established.. send the REST message
		if(FLAG.SSL_CONNECTION_STATE == TRUE)
		{
#ifdef SECURE_SOCKET
			APP_DEBUG("\n\r<-- Send REST message to Azure DPS to register device first time -->\r\n");
#endif
			if((FLAG.DEVICE_NOT_REGISTERED == TRUE) // If device is not yet registered then it need to be registered first
				||(FLAG.DEVICE_UNAUTHORISED == TRUE))// Devices when deleted fron IoT Hub might show unregistered.. so let them also process through DPS connect
			{
				ret = RIL_SSL_QSSLSEND(SSID_HOST_AZURE,  REST_MESSAGE_PUT, Ql_strlen(REST_MESSAGE_PUT));
			}
			else //Device is already registered then just get the IoT Hub configuration information
			{
#ifdef SECURE_SOCKET
				APP_DEBUG("\n\r<-- Send REST message to Azure DPS to get IoT Hub configuration of a registered device -->\r\n");
#endif
				ret = RIL_SSL_QSSLSEND(SSID_HOST_AZURE,  REST_MESSAGE_POST, Ql_strlen(REST_MESSAGE_POST));
			}
		}
	}
	else
	{
		//Close any earlier open sessions if any
		RIL_SSL_QSSLCLOSE(SSID_HOST_AZURE);
		FLAG.SSL_CONNECTION_STATE = FALSE;

		// if Connection is closed then establish it a fresh
		if(FLAG.SSL_CONNECTION_STATE ==  FALSE)
		{
#ifdef DEBUG_MSG
			APP_DEBUG("\n\r<-- Open SSL connection - Azure IoT Hub : ");
#endif
			//Draw Network not connected symbol to start
			Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL);
			Refresh_Communication_inprogress_Symbol(colOrangeRed);
			ret = RIL_SSL_QSSLOPEN(AZURE_assigned_IoT_Hub, AZURE_IOTHUB_HOST_PORT, SSID_HOST_AZURE, CID_HOST_AZURE, CONNECT_MODE_TRANSPERENT);
#ifdef DEBUG_MSG
			if(ret == RIL_AT_SUCCESS)
			{
				APP_DEBUG("Success-->\n\r");
			}	
			else
			{	
				APP_DEBUG("Fail-->\n\r");
			}
#endif
			FLAG.SSL_CONNECTION_STATE = (ret == RIL_AT_SUCCESS) ? TRUE : FALSE ;
		}
	}
	return ret;
}

/*****************************************************************
 * Function: Connect_Azure_IoT_host_socket(u8 host_id)
 *
 * Description:
 *              Connect_Azure_IoT_host_socket
 *              Reconnecting the Socket if Disconnected
 *              establish a MQTT session at last
 *
 * Parameters:
 * 			u8 ssid : socket id .
 *
 * Return  Status
 *
 *****************************************************************/
s32 Connect_Azure_IoT_host_socket(u8 host_id)
{
	s32 ret = RIL_AT_FAILED;

	if(FLAG.AUTOPROVISIONING_COMPLETE == FALSE)
	{
		//Draw Network not connected symbol to start
		Refresh_Cloud_Connectivity_Symbol(0);
	}

	switch(host_id)
	{
		case SSID_HOST_AZURE:
			ret = Connect_Azure_IoT(); // Connect to Azure IoT ( DPS -> Hub)
			if(ret == 0 && (FLAG.AUTOPROVISIONING_COMPLETE == TRUE)) // The above functions returned true with provisioning complete
			{
#ifdef SECURE_SOCKET
				APP_DEBUG("<-- Sending MQTT Connection req : AZURE IOT HUB -->\r\n");
#endif
				MQTT_Send_CONN_DISCONN_Request(SSID_HOST_AZURE, MQTT_ON);
			}
		break;
	}

	return ret;
}


s32 TCP_Socket_Read_Data_SSL (u8 *buff, s32 len)
{
	Ql_memset((void*)buff, FALSE, sizeof(buff));

	g_recvd_data_last_read_pos = (g_recvd_data_last_read_pos+len)-1;

	for(int i=0; i<len; i++)
	{
		buff[i] = g_buf_recvd_data[g_recvd_data_last_read_pos+i];
		g_recvd_data_last_read_pos++;
	}


	return len;
}
/*****************************************************************
 * Function: Azure_DPS_Response_Process(void)
 *
 * Description:
 *		Thif function process the Azure DPS response in JSON format
 * Parameters:
 *		char *msg : Actual Payload received
 *
 * Return:
 *		s32 : Check status Fail or PASS.
 *****************************************************************/
s32 Azure_DPS_Response_Process(void)
{
	s8 ret = RIL_AT_SUCCESS;

	char* DPSstring = NULL;
	cJSON *root = NULL;
	cJSON *tmp = NULL;
	cJSON *tmpobj = NULL;

	// Strip out the headers and point to the start of actual jason packet of the Azure DPS response
	DPSstring = Ql_strstr(g_buf_recvd_data, "{\"");

	if(DPSstring != NULL)
	{
		cJSON_InitHooks(NULL); //init function pointers to Ql_MEM_Alloc calloc realloc
		root = cJSON_Parse(DPSstring);

		//1. Read enrollmentGroupId information
		tmp = cJSON_GetObjectItem(root, "x509");
		if(tmp != NULL)
		{
			tmpobj = cJSON_GetObjectItem(tmp, "enrollmentGroupId");
			if(tmpobj != NULL)
			{
#ifdef SECURE_SOCKET
				Ql_strcpy(AZURE_DPS_enrollment_group_ID,tmpobj->valuestring);
				APP_DEBUG("\n\r enrollmentGroupId-> %s",AZURE_DPS_enrollment_group_ID);
#endif
			}
			else
				ret = RIL_AT_FAILED;

			//2. Read RegisrtationID information
			if(ret == RIL_AT_SUCCESS)
			{
				tmp = cJSON_GetObjectItem(root, "registrationId");
				if(tmp != NULL)
				{
#ifdef SECURE_SOCKET
					Ql_strcpy(AZURE_DPS_registration_ID,tmp->valuestring);
					APP_DEBUG("\n\r registrationId-> %s",AZURE_DPS_registration_ID);
#endif
				}
				else
					ret = RIL_AT_FAILED;
			}
			//3. Read DeviceID information
			if(ret == RIL_AT_SUCCESS)
			{
				tmp = cJSON_GetObjectItem(root, "deviceId");
				if(tmp != NULL)
				{
					Ql_strcpy(AZURE_IoT_device_ID,tmp->valuestring);
#ifdef SECURE_SOCKET
					APP_DEBUG("\n\r deviceId-> %s",AZURE_IoT_device_ID);
#endif
				}
				else
					ret = RIL_AT_FAILED;
			}

			//4. Read assigned_Hub information
			if(ret == RIL_AT_SUCCESS)
			{
				tmp = cJSON_GetObjectItem(root, "assignedHub");
				if(tmp != NULL)
				{
					Ql_strcpy(AZURE_assigned_IoT_Hub,tmp->valuestring);
#ifdef SECURE_SOCKET
					APP_DEBUG("\n\r assignedHub-> %s",AZURE_assigned_IoT_Hub);
#endif
				}
				else
					ret = RIL_AT_FAILED;
			}

			//5. Read Status information
			if(ret == RIL_AT_SUCCESS)
			{
				tmp = cJSON_GetObjectItem(root, "status");
				if(tmp != NULL)
				{
#ifdef SECURE_SOCKET
					Ql_strcpy(AZURE_DPS_Status,tmp->valuestring);
					APP_DEBUG("\n\r status-> %s",AZURE_DPS_Status);
#endif
				}
				else
					ret = RIL_AT_FAILED;
			}

			//6. Read SubStatus information
			if(ret == RIL_AT_SUCCESS)
			{
				tmp = cJSON_GetObjectItem(root, "substatus");
				if(tmp != NULL)
				{
#ifdef SECURE_SOCKET
					Ql_strcpy(AZURE_DPS_Sub_Status,tmp->valuestring);
					APP_DEBUG("\n\r substatus-> %s\n\r",AZURE_DPS_Sub_Status);
#endif
				}
				else
					ret = RIL_AT_FAILED;
			}
		}
		else //Parse error message
		{
			tmp = cJSON_GetObjectItem(root, "message");
			if(tmp != NULL)
			{
				if(Ql_strcmp(tmp->valuestring, "Registration not found") == 0) // Device is not registered yet
				{
#ifdef SECURE_SOCKET
					APP_DEBUG("\n\r<-- Device Not Yet Registered -->\n\r");
#endif
					FLAG.DEVICE_NOT_REGISTERED = TRUE; //Next connect with Azure DPS is to register the device for first time
					//Schedule to connect with Azure DPS to register the device for first time
					Ql_OS_SendMessage(0, MSG_ID_SOCKET_CONNECTION,SSID_HOST_AZURE,0);
				}
				else if(Ql_strcmp(tmp->valuestring, "Unauthorized") == 0) // Device is Unauthorised..
				{
#ifdef SECURE_SOCKET
					APP_DEBUG("\n\r<-- Device Unauthorised -->\n\r");
#endif
					// Certificates are not present or not valid then Display Internal Fault F3 on LCD
			//		Ql_sprintf((char *)&Internal_error_code,"F4");
			//		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_STRING_INTERNAL_ERROR_CODE);
					FLAG.DEVICE_UNAUTHORISED = TRUE;
				}
			}

			tmp = cJSON_GetObjectItem(root, "status");
			if(tmp != NULL)
			{
				if(Ql_strcmp(tmp->valuestring, "assigning") == 0) // Device is registered and is in assigning status
				{
#ifdef SECURE_SOCKET
					APP_DEBUG("\n\r<-- Device Registered -->\n\r");
#endif
					FLAG.DEVICE_UNAUTHORISED = FALSE;
					FLAG.DEVICE_NOT_REGISTERED = FALSE; //Next connect with Azure DPS is to preview the device IoT Hub confguration
					//Schedule to connect with Azure DPS to preview the device IoT Hub confguration
					Ql_OS_SendMessage(0, MSG_ID_SOCKET_CONNECTION,SSID_HOST_AZURE,0);
				}
			}
			ret = RIL_AT_FAILED;
		}
		cJSON_Delete(root);
	}

	if(ret == RIL_AT_SUCCESS)
	{
		//Prepare MQTT connection strings.
		Ql_sprintf(DEVICE_USER_ID, "%s/%s/?api-version=2018-06-30",AZURE_assigned_IoT_Hub,AZURE_IoT_device_ID); // API verison is must to have device twin properties communication
		Ql_sprintf(DEVICE_CLIENT_ID, "%s",AZURE_IoT_device_ID);
		Store_IoT_Persistent_Data();   //Write permanent data to flash file for AZURE_assigned_IoT_Hub
		FLAG.AUTOPROVISIONING_COMPLETE = TRUE;
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL);
		Ql_OS_SendMessage(0, MSG_ID_SOCKET_CONNECTION,SSID_HOST_AZURE,0);//To connect Azure IoT Hub connection
	}

	return ret;
}

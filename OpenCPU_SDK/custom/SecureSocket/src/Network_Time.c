/*HEADER**************************************************************** 
************************************************************************ 
*** *** Copyright (c) 2009 << Client Company Name>> *** All rights reserved *** 
*** This software embodies materials and concepts which are 
*** confidential to << Client Company Name>> and is made available solely 
*** pursuant to the terms of a written license agreement with 
*** << Client Company Name>>. ***
*** Designed and Developed by CoreObjects Inc. (for << Client Company Name>>) 
*** *** File: <file name.h> ***
*** Comments: TIME  program to get the Real time from network
*** Comments about the file.
*** - Algorithm / logic explanation 
*** - formula explanation 
*** - inputs and outputs of this file 
*** - etc etc *** 
*** ************************************************************************ 
*END*******************************************************************/

#include <string.h>
#include <stdlib.h>
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
#include "ql_wtd.h"
#include "ql_gpio.h"
#include "ql_time.h"
#include "ql_memory.h"
#include "cJSON.h"
#include "debug.h"
#include "common_vars.h"
#include "ipc.h"

typedef enum NW_Time{
	CMD_ATI=1, CMD_QNITZ, CMD_QLTS, CMD_CTZU, CMD_CCLK
}NW_TIME;
NW_TIME NETWORK_FLAG = CMD_ATI;

char lineBuff[30]={0};

s32 Update(void);

u8 time_data[40]={0};

s32 GetNW_Time(void);


/*****************************************************************************
 * FUNCTION
 *  time_update
 *
 * DESCRIPTION
 *  This function is used to update the local machine time from netwrk response.
 *  
 * PARAMETERS
 *  line : response string for processing
 *
 * RETURNS
 *  
 *****************************************************************************/
/*
void time_update(char *line)
{
	ST_Time time;
	int i;
    char *ret;
    char *ret1;
   ret = Ql_strstr(line, "+CCLK: ");
   if(ret != NULL)
   {
	   int v;
	  u8 temp_buf[2];
	  u8 temp_buf1[2];
		ret1 = Ql_strcpy(time_data, ret);
		v = Ql_strlen(time_data);
		for(i=2;i<4;i++)
		{
			temp_buf1[i-2] = (time_data[i+6]);
		}
		time.year =(atoi(temp_buf1));//first 2 bytes
		time.year = time.year + 2000;
		
		for(i=0;i<2;i++)
		{
			temp_buf[i] = time_data[i+11];
		}
		time.month = atoi(temp_buf);

		
		for(i=0;i<2;i++)
		{
			temp_buf[i] = time_data[i+14];
		}
		time.day = atoi(temp_buf);
		
		for(i=0;i<2;i++)
		{
			temp_buf[i] = time_data[i+17];
		}
		time.hour = atoi(temp_buf);

		
		for(i=0;i<2;i++)
		{
			temp_buf[i] = time_data[i+20];
		}
		time.minute = atoi(temp_buf);

		
		for(i=0;i<2;i++)
		{
			temp_buf[i] = time_data[i+23];
		}
		time.second = atoi(temp_buf);

		for(i=0;i<2;i++)
		{
			temp_buf[i] = time_data[i+26];
		}
		time.timezone = atoi(temp_buf);

		ret = Ql_SetLocalTime(&time);
		
   }
   
}
*/
s32 ATResponse_SSL_handler_NetworkTime(char* line, u32 len, void* userdata)
{
	char *ret = NULL;
	ret = Ql_strstr(line,"+QLTS: ");
	if(ret!=NULL)
	{
		Ql_strcpy(lineBuff,ret);
	}

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
s32 Update(void)
{
	s32 i=0,j=0;
	char temp[10];
	ST_Time time;

	char *ret = Ql_strstr(lineBuff,"+QLTS: ");
	if(ret !=NULL)
	{
		for(i=0;i<2;i++,j++)
		{
			temp[j]=lineBuff[i+8];
		}
		time.year = Ql_atoi(temp);
		time.year = time.year+2000;

		j=0;

		for(i=0;i<2;i++,j++)
		{
			temp[j]=lineBuff[i+11];
		}
		time.month = Ql_atoi(temp);

		j=0;

		for(i=0;i<2;i++,j++)
		{
			temp[j]=lineBuff[i+14];
		}
		time.day = Ql_atoi(temp);

		j=0;

		for(i=0;i<2;i++,j++)
		{
			temp[j]=lineBuff[i+17];
		}
		time.hour = Ql_atoi(temp);

		j=0;

		for(i=0;i<2;i++,j++)
		{
			temp[j]=lineBuff[i+20];
		}
		time.minute = Ql_atoi(temp);

		j=0;

		for(i=0;i<2;i++,j++)
		{
			temp[j]=lineBuff[i+23];
			CRC_Token ^= temp[j];
			IPC_CMD_REQ_source |= SCHEDULE_IPC_GET_IDENTITY_CMD; // Schedule the Get identification commmand to pass this token to OCntrol Module
		}
		time.second = Ql_atoi(temp);

		j=0;

		for(i=0;i<2;i++,j++)
		{
			temp[j]=lineBuff[i+26];
		}
		time.timezone = Ql_atoi(temp);

		j=0;
	}
	Ql_SetLocalTime(&time);

	return 0;
}


/*****************************************************************************
 * FUNCTION
 *  GetNW_Time
 *
 * DESCRIPTION
 *  This function is used to send at commands to recieve the network time
 *  
 * PARAMETERS
 *
 * RETURNS
 *  
 *****************************************************************************/

s32 GetNW_Time(void)
{
	s32 ret;
	s32 len;
	char *atCmd = NULL;
	atCmd = "ATI\r\n";
	SEND_NXT:
	ret = Ql_RIL_SendATCmd(atCmd, sizeof(atCmd),  ATResponse_SSL_handler_NetworkTime, NULL, 2000);
	if(ret == RIL_ATRSP_SUCCESS)
	{
		if(NETWORK_FLAG == CMD_ATI)
		{
			atCmd = "AT+QNITZ=1\r\n";
			NETWORK_FLAG = CMD_QNITZ;
		}
		else if(NETWORK_FLAG == CMD_QNITZ )
		{
			atCmd = "AT+QLTS\r\n";
			NETWORK_FLAG = CMD_QLTS;
		}
		else if(NETWORK_FLAG == CMD_QLTS)
		{
			len = Ql_strlen(lineBuff);
			if(len>=33)
			{
				Update();
				atCmd = "AT+CTZU=1\r\n";
				NETWORK_FLAG = CMD_CTZU;
			}
//			else
//			{
//				Ql_Sleep(1000);
//				Ql_Sleep(1000);
//			}
		}
		else if(NETWORK_FLAG == CMD_CTZU)
		{
			atCmd = "AT+CCLK?\r\n";
			NETWORK_FLAG = CMD_CCLK;
		}
		else if(NETWORK_FLAG == CMD_CCLK)
		{
			NETWORK_FLAG = CMD_ATI;
			return RIL_ATRSP_SUCCESS;
		}
		goto SEND_NXT;
	}
	else
	{

		goto SEND_NXT;
	}

	return RIL_ATRSP_SUCCESS;
}

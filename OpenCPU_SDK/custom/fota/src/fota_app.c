//#ifdef __FOTA_HTTPS_SD__

#include "common_vars.h"

#include "ql_fota.h"
#include "common_vars.h"
#include "ril_http.h"

#define READ_SIZE 512

s32 RIL_HTTPS_QSSLCFG(void);
static void Callback_HTTPS_RcvData(u8* ptrData, u32 dataLen, void* reserved);
s32 FIRMWARE_Update(void);
s32 FIRMWARE_Download(void);
s32 FOTA_INIT(void);

static ST_FotaConfig FotaConfig;

u8 FOTA_URL[150] = {'\0'};
u8 HTTP_LICENSE_URL_ADDR[150] = {'\0'};
u8 g_HttpRcvBuf[250]    = {'\0'};
int g_Handle_File_FotaAppBin = -1;
int g_Handle_File_LICENSEPEM = -1;
u32 g_rcvdDataLen = 0;
u32 dataLen_here;
u32 total_size;

u8 Fota_Fail_Reason_Code;
extern s32 fileSys_getFileSize(u8 *file_name);
extern char RP_FW_Upgrade_status[5];
extern u32 Process_TWIN_PROPERTY_CHANGE;
#define REPORT_TWIN_CNTL_FW_UPDTAE_STATUS   (1<<8)//also defined in mqtt.c file

//1 : Downloading
//2 : Download Complete
//3 : Update In Progress
//4 : Update Success
//5 : Update Failed
//6 : Update Successful & Rebooting


s32 RIL_HTTPS_QSSLCFG()
{
	s32 ret = RIL_AT_SUCCESS;
	char strAT[50];

	//Enable Https
	if(ret == RIL_AT_SUCCESS)
	{
		Ql_memset(strAT, 0, sizeof(strAT));
		Ql_sprintf(strAT, "AT+QSSLCFG=\"https\",1\n");

		ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef FOTA_MSG
		APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
		if(ret != RIL_AT_SUCCESS)
		{
			APP_DEBUG("\r\n<-- ERROR : HTTPS CONFIG (ret = %d) -->\r\n", ret);
		}
#endif
	}

	//Set Cid for FOTA URL connection
	if(ret == RIL_AT_SUCCESS)
	{
		Ql_memset(strAT, 0, sizeof(strAT));
		Ql_sprintf(strAT, "AT+QSSLCFG=\"httpsctxi\",%d\n",CID_HOST_FOTA);

		ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),NULL,NULL,0);
#ifdef FOTA_MSG
		APP_DEBUG("<-- [%s]Send AT:%s -->\r\n",(ret==0)?"SUCCESS":"FAIL",strAT);
		if (RIL_AT_SUCCESS != ret)
		{
			APP_DEBUG("\r\n<-- ERROR : HTTPS_CTXI CONFIG (ret = %d) -->\r\n", ret);
		}
#endif
	}

	return ret;
}

s32 FIRMWARE_Download()
{
	s32 ret = -1;
	g_rcvdDataLen = 0;
	u8 Fota_Download_retry_count;
	bool FOTA_DOWNLOAD_SUCCESS = FALSE;
	FLAG.FOTA_FILE_COM_FW_INVALID = TRUE; //Set the flag as invalid state..
	FLAG.FOTA_FILE_CON_FW_INVALID = TRUE; //Set the flag as invalid state..
	for(Fota_Download_retry_count = 0 ; Fota_Download_retry_count < 10  && FOTA_DOWNLOAD_SUCCESS == FALSE; Fota_Download_retry_count++)
	{
		// Set HTTP server address (URL)
		ret = RIL_HTTP_SetServerURL((char *)FOTA_URL, Ql_strlen((char *)FOTA_URL));
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("<-- ERROR : HTTPS FOTA SET SERVER URL (ret = %d) -->\r\n", ret);
#endif
			Fota_Fail_Reason_Code = 4;
			return ret;
		}
#ifdef FOTA_MSG
		APP_DEBUG("<-- SUCCESS : HTTPS FOTA SET SERVER URL  %s-->\r\n",FOTA_URL);
#endif
		// Send HTTP GET Request
		ret = RIL_HTTP_RequestToGet(60);
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("<-- ERROR : HTTPS GET REQUEST Failed (ret = %d) -->\r\n", ret);
#endif
			Fota_Fail_Reason_Code = 5;
			return ret;
		}
#ifdef FOTA_MSG
		APP_DEBUG("<-- SUCCESS : HTTPS GET REQUEST -->\r\n");
#endif
		/*---- Read File From Server ----*/
		// Set Filesystem
		fileSys_SetFileSystem(FALSE);	// False means No filesystem Formate.

		// Create File to store
		ret = fileSys_OpenOrCreateFile((u8 *)File_FotaAppBin);
		if(ret < 0)
		{
			//Open file failed.
#ifdef FOTA_MSG
			APP_DEBUG("<-- Failed to Create File (%s) ret=%d -->\r\n", File_FotaAppBin, ret);
#endif
			Fota_Fail_Reason_Code = 6;
			return ret;
		}
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- File (%s) Created -->\r\n", File_FotaAppBin);
#endif
		g_Handle_File_FotaAppBin = ret;//Get file handle (Global Variable)

		// Read HTTP File Data and write to File
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- START RECEIVING FOTA FILE FROM SERVER -->\r\n");
#endif

		ret = RIL_HTTP_ReadResponse(30, Callback_HTTPS_RcvData);

		Ql_FS_Close(g_Handle_File_FotaAppBin);
		g_rcvdDataLen = 0;
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- ERROR : FOTA FILE CURRUPT WHILE DOWNLOAD!!! (ret = %d) -->\r\n", ret);
#endif
			// Delete Curreupt File
			ret = fileSys_deleteFile((u8 *)File_FotaAppBin);
			if(ret < 0)
			{
#ifdef FOTA_MSG
				APP_DEBUG("<-- ERROR : Deleting Currupt File (%s), (ret = %d) -->\r\n", File_FotaAppBin, ret);
#endif
				Fota_Fail_Reason_Code = 7;
				return ret;
			}

			// Re-Download File
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- RE-INITIATE FOTA FILE DOWNLOAD... -->\r\n");
#endif
		}
		else
		{
			FOTA_DOWNLOAD_SUCCESS = TRUE; //Get out of the the for loop
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- SUCCESS : FOTA FILE DOWNLOAD FROM HTTPS Server!!! -->\r\n");
#endif
		}
	}

	//Return either sucess of fail due to max attempts
	if(FOTA_DOWNLOAD_SUCCESS == TRUE)
		return 0;
	else
	{
		Fota_Fail_Reason_Code = 8;
		return -1;
	}
}

s32 FIRMWARE_Update()
{
	int ret=-1;
	int FileSize = 0;
	int ReadSize = 0;
	int ReadLen = 0;
	int Handle_File = -1;
	char buf[512];

	static int s_iSizeRem = 0;


#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- TWIN FOTA Status : Update in progress !!! -->\r\n");
#endif
	Ql_strcpy(RP_FW_Upgrade_status,"3");// Report "Update in progress"
	Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTL_FW_UPDTAE_STATUS;
	publish_data(MQTT_TWIN_REPORTED);

	//1. Init FOFA param.
	Ql_memset((void *)(&FotaConfig), 0, sizeof(ST_FotaConfig)); //Do not enable watch_dog
	FotaConfig.Q_gpio_pin1 = 0;
	FotaConfig.Q_feed_interval1 = 100;
	FotaConfig.Q_gpio_pin2 = 26;
	FotaConfig.Q_feed_interval2 = 500;

	//2. Begin, check the Bin file.
	ret = fileSys_getFileSize(File_FotaAppBin); //Get the size of upgrade file from file system
	if(ret < QL_RET_OK)
	{
		//The file does not exist
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- FOTA File (%s) Not Present (ret = %d) -->\r\n", File_FotaAppBin, ret);
#endif
		Fota_Fail_Reason_Code = 9;
		return -1;
	}
	FileSize = ret;
#ifdef FOTA_MSG
	APP_DEBUG("\r\n<-- FOTA File (%s) Present File Size:.%d Bytes -->\r\n", File_FotaAppBin, FileSize);
	APP_DEBUG("<-- Downloaded file size: %d -->\r\n", total_size );
#endif

	if(total_size != FileSize)	//Comapare Downloaded File size with new Final downloaded file size , if mismatch then there is Error.Fota Success but it restarts with Previous Firmware(FOTA failure)
	{
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- ERROR : Downloaded file size is mismatch  -->\r\n");
#endif
		Fota_Fail_Reason_Code = 10;
		return -1;
	}

	ret = fileSys_OpenOrCreateFile((u8 *)File_FotaAppBin);
	if(ret < 0)
	{
		//Open file failed.
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- ERROR : File Open/Create : %s -->\r\n", filePath);
#endif
		Fota_Fail_Reason_Code = 11;
		return -1;
	}
	Handle_File = ret; //Get file handle

	//Write App bin to flash
	ret = Ql_FOTA_Init(&FotaConfig); //Initialise the upgrade operation
	if(QL_RET_OK !=ret)
	{
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- ERROR : FOTA INIT -->\r\n");
#endif
		Fota_Fail_Reason_Code = 12;
		return -1;
	}

	while(FileSize > 0)
	{
		Ql_memset(buf, 0, sizeof(buf));
		if (FileSize <= READ_SIZE)
		{
			ReadSize = FileSize;
		}
		else
		{
			ReadSize = READ_SIZE;
		}

		ret = Ql_FS_Read(Handle_File, (u8 *)buf, ReadSize, (u32 *)&ReadLen); //read upgrade data from file system
		if(QL_RET_OK != ret)
		{
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- ERROR : FOTA File Read Failed (ret = %x) -->\r\n", ret);
#endif
			Fota_Fail_Reason_Code = 13;
			return -1;
		}

		//Write upgrade data to FOTA Cache region
		ret=Ql_FOTA_WriteData(ReadSize,(s8*)buf);

		if(QL_RET_OK != ret)
		{
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- ERROR : FOTA File Write Failed (ret = %d) -->\r\n", ret);
#endif
			Fota_Fail_Reason_Code = 14;
			return -1;
		}else
		{
			s_iSizeRem +=ReadSize;
		}
		FileSize -= ReadLen;

		Ql_Sleep(5); //Sleep 5 ms for outputing catcher log!!!
	}
	Ql_FS_Close(Handle_File);

	ret = Ql_FOTA_Finish(); //Finish the upgrade operation ending with calling this API

	if(QL_RET_OK != ret)//added for testing purpuse
	{
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- ERROR : FOTA File Finish -->\r\n");//added for testing purpuse
#endif
		Fota_Fail_Reason_Code = 15;
		return -1;
	}
	else
	{
#ifdef DEBUG_MSG
		APP_DEBUG("\r\n<-- SUCCESS : UPLOADING FOTA File to Core -->\r\n");
		APP_DEBUG("\r\n<-- TWIN FOTA Status : Update Success !!! -->\r\n");
#endif
		Ql_strcpy(RP_FW_Upgrade_status,"4");// Report "Update Success"
		Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTL_FW_UPDTAE_STATUS;
		publish_data(MQTT_TWIN_REPORTED);
		//Force Backup at Control module
		configuration_Data_Write[CONFIG_DATA0] = 0x76;
		configuration_Data_Write[CONFIG_DATA1] = 0xC0;
		Send_IPC_Command(PID_INITIATE_POWER_CYCLE);
		display_on_tftlcd(TFT_STATE_FOTA_SUCCESS);	//Display "SUCCESS" on LCD when Fota file downloading is Success.
		Ql_Sleep(2000); //Added delay to enable con module process the backup request.

#ifdef DEBUG_MSG
		APP_DEBUG("\r\n<-- TWIN FOTA Status : Update Success & rebooting!!! -->\r\n");
#endif
		Ql_strcpy(RP_FW_Upgrade_status,"6");// Report "update success and rebooting"
		Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTL_FW_UPDTAE_STATUS;
		publish_data(MQTT_TWIN_REPORTED);
#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- SUCCESS : FOTA File Finish SUCCESS  (ret = %x) -->\r\n", ret);
		APP_DEBUG("\r\n<-- MODULE WILL RESTART AUTOMATICALLY ON SUCCESS -->\r\n");
#endif
		ret = Ql_FOTA_Update(); //Update flag fields in the FOTA Cache.If this function successes then it RESTARTS device automatically.
		
		//Code below will not execute if FOTA is successfull!!!  Module will restart..
		//code will come back to this line only if FOTA update fails..
		if(QL_RET_OK != ret) //If this function succeeds, the module will automatically restart
		{
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- ERROR : FOTA UPDATE FAILED (ret = %d) -->\r\n", ret);
#endif
			Fota_Fail_Reason_Code = 16;
			return -1;
		}
		return 0;
	}

}

s32 FOTA_INIT()
{
	s32 ret = -1;
	Fota_Fail_Reason_Code =0;
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- FOTA INITIATED !!! -->\r\n");
#endif

	FLAG.SUSPEND_IPC_COMMUNICATION = TRUE;
	Store_Persistent_Data();
	Write_Error_Messages();
	//Check FOTA.bin File is present or not: If present then delete file and check free space
	ret = fileSys_checkFile((u8*)File_FotaAppBin);		//To Check file FOTA.bin is available or not
	if(ret == QL_RET_OK)		//If FOTA.bin file is available then delete file
	{
		ret = fileSys_deleteFile((u8*)File_FotaAppBin);		//Delete FOTA.bin file
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("<-- ERROR : Deleting Existing FOTA File (%s), (ret = %d) -->\r\n", File_FotaAppBin, ret);
#endif
			Fota_Fail_Reason_Code = 2;
			return ret;
		}
	}
#ifdef FOTA_MSG
	APP_DEBUG("<-- Memory Free Space  : %lld Byte's -->\r\n",Ql_FS_GetFreeSpace(storage));		//Print free space debug message on screen
#endif
	//Display "UPDATING..." on LCD when FOTA is Initiated.
	Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_FOTA_UPDATE);

	//Report to twin properties
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- TWIN FOTA Status : Updating !!! -->\r\n");
#endif
	Ql_strcpy(RP_FW_Upgrade_status,"1");// Report "Downloading"
	Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTL_FW_UPDTAE_STATUS;
	publish_data(MQTT_TWIN_REPORTED);

	// Confugure for HTTPS
	ret = RIL_HTTPS_QSSLCFG();
	if(ret < 0)
	{
#ifdef FOTA_MSG
		APP_DEBUG("<-- HTTPS Config Fail ret=%d -->\r\n", ret);
#endif
		Fota_Fail_Reason_Code = 1;
	}
	else
	{
		ret = FIRMWARE_Download();
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- FIRMWARE DOWNLOAD FAIL!!! -->\r\n");
#endif
		}
		else
		{
			if((FLAG.FOTA_FILE_COM_FW_INVALID == TRUE) || (FLAG.FOTA_FILE_CON_FW_INVALID == TRUE))// Check if the FOTA file is of correct parameters..
			{
				Fota_Fail_Reason_Code = 3;
				ret = -1;
			}
		}
	}

	// Start Firmware Update.. only if previous operation is SUCCESS
	if(ret >= 0)
	{
#ifdef DEBUG_MSG
		APP_DEBUG("\r\n<-- TWIN FOTA Status : Down load complete !!! -->\r\n");
#endif
		Ql_strcpy(RP_FW_Upgrade_status,"2");// Report "Download complete"
		Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTL_FW_UPDTAE_STATUS;
		publish_data(MQTT_TWIN_REPORTED);

		ret = FIRMWARE_Update();
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- FIRMWARE UPDATE FAIL!!! -->\r\n");
#endif

		}
	}
	//Send final response return from this function
	if(ret < 0)
	{
#ifdef DEBUG_MSG
		APP_DEBUG("\r\n<-- TWIN FOTA Status : Update Failed !!! -->\r\n");
#endif
		Ql_strcpy(RP_FW_Upgrade_status,"5");// Report "Download complete"
		Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTL_FW_UPDTAE_STATUS;
		publish_data(MQTT_TWIN_REPORTED);

		display_on_tftlcd(TFT_STATE_FOTA_FAIL);	//Display "FAIL" on LCD when Fota file downloading becomes Failed.
		Ql_Sleep(2000); //Added delay to enable user to read "FAIL" message before device re-start.
		IPC_CMD_REQ_source |= SCHEDULE_POWER_DOWN_WITH_BACKUP;
		return ret;
	}
	else
	{

		return 0;
	}

}

static void Callback_HTTPS_RcvData(u8* ptrData, u32 dataLen, void* reserved)
{
	s32 ret = -1;
	u16 i = 0;
	dataLen_here= dataLen;  //pass this value trough global variable to outside functions
#ifdef FOTA_MSG
	APP_DEBUG("<-- Data coming on https, total len: %d -->\r\n", (g_rcvdDataLen + dataLen) );
#endif
	display_on_tftlcd(TFT_FOTA_FILE_DOWNLOAD_SIZE);	//Display updated downloading FILE SIZE on LCD, till Downloading completed.
	total_size = g_rcvdDataLen + dataLen;

	if(FLAG.READY_TO_INITIATE_FOTA_DOWNLOAD == TRUE)	// If FOTA URL is got then save file
	{
		ret=Ql_FS_Write(g_Handle_File_FotaAppBin, ptrData, dataLen, &writeedlen);

		if(ret == QL_RET_OK)
		{
#ifdef FOTA_MSG
			APP_DEBUG("<-- File Writen Data : %d -->\r\n", writeedlen);
#endif
			// Extract the FW version of the new / updated code..from the FOTA Downloaded FW
			// Scan through the entire FW copy and find a token of '&', '$', '@', '#' .. version info is right after it.

			for(i=0;i<=writeedlen;i++)
			{
				if(ptrData[i] == '&')
				{
					if(ptrData[i+1] == '$')
					{
						if(ptrData[i+2] == '@')
						{
							if(ptrData[i+3] == '#')
							{
								if(ptrData[i+4] == HW_Version_COMM) // Check if received FOTA is for correct COM module HW version
								{
									if(ptrData[i+5] == HW_Version_CON) // Check if received FOTA is for correct CON module HW version
									{
#ifdef FOTA_MSG
										APP_DEBUG("<--HW_Version_COMM       = %x -->\r\n", ptrData[i+4]);
										APP_DEBUG("<--HW_Version_CON        = %x -->\r\n", ptrData[i+5]);
										APP_DEBUG("<--FW_Version_Major_COMM = %x -->\r\n", ptrData[i+6]);
										APP_DEBUG("<--FW_Version_Minor_COMM = %x -->\r\n", ptrData[i+7]);
#endif
										// There should be change in version information
										if(FW_Version_Major_COMM != ptrData[i+6] && FW_Version_Minor_COMM != ptrData[i+7]) 
										{
											FLAG.FOTA_FILE_COM_FW_INVALID = FALSE;
										}
										else
										{
#ifdef FOTA_MSG
											APP_DEBUG("<-- COM FW Version is Invalid-->\r\n");
#endif
											if(FLAG.DIGNOSTIC_MODE == TRUE) // Force FW upgrade in case in Diagnostic mode with same version number
												FLAG.FOTA_FILE_COM_FW_INVALID = FALSE;
										}
									}
								}
							}
						}
					}
				}
				// Extract the CON Module version information of the new / updated code..from the FOTA Downloaded FW
				// Scan through the entire FW copy and find a token of '<', '*', '!', '=' .. version info is right after it.
				if(ptrData[i] == '<')
				{
					if(ptrData[i+1] == '*')
					{
						if(ptrData[i+2] == '!')
						{
							if(ptrData[i+3] == '=')
							{
								if(ptrData[i+4] == HW_Version_CON) // Check if received FOTA is for correct CON module HW version
								{
#ifdef FOTA_MSG
									APP_DEBUG("<--HW_VERSION             = %x -->\r\n", ptrData[i+4]);
									APP_DEBUG("<--FW_VERSION_MAJOR       = %x -->\r\n", ptrData[i+5]);
									APP_DEBUG("<--FW_VERSION_MINOR       = %x -->\r\n", ptrData[i+6]);
									APP_DEBUG("<--CON_IPC_VERSION        = %x -->\r\n", ptrData[i+7]);
									APP_DEBUG("<--MODEL_PAYGO_RO_TDS_AMC = %x -->\r\n", ptrData[i+8]);
#endif
									if((Control_Module_Indentity_data[4] == ptrData[i+8])// Make sure the FW is for correct set of Control HW Model
										&& (Control_Module_Indentity_data[0] == ptrData[i+4]))// Make sure the FW is for correct set of Control HW module version
									{
										FLAG.FOTA_FILE_CON_FW_INVALID = FALSE;
									}
									else
									{
#ifdef FOTA_MSG
										APP_DEBUG("<-- CON FW Version is Invalid-->\r\n");
#endif
									}
								}
							}
						}
					}
				}
			}

		}
	}
	else	// start of else get FOTA URL
	{
		if ((g_rcvdDataLen + dataLen) <= sizeof(g_HttpRcvBuf))
		{
			Ql_memcpy((void*)(g_HttpRcvBuf + g_rcvdDataLen), (const void*)ptrData, dataLen);
		}
		else
		{
			if (g_rcvdDataLen < sizeof(g_HttpRcvBuf))
			{// buffer is not enough
				u32 realAcceptLen = sizeof(g_HttpRcvBuf) - g_rcvdDataLen;
				Ql_memcpy((void*)(g_HttpRcvBuf + g_rcvdDataLen), (const void*)ptrData, realAcceptLen);
#ifdef FOTA_MSG
				APP_DEBUG("<-- Rcv-buffer is not enough, discard part of data (len:%d/%d) -->\r\n", dataLen - realAcceptLen, dataLen);
#endif
			}
			else // No more buffer
			{
#ifdef FOTA_MSG
				APP_DEBUG("<-- No more buffer, discard data (len:%d) -->\r\n", dataLen);
#endif
			}
		}
	}// end of else get FOTA URL
	g_rcvdDataLen += dataLen;
}

//#endif // __FOTA_HTTPS_SD__


static void Callback_HTTPS_RecvData(u8* ptrData, u32 dataLen, void* reserved)
{
	s32 ret = -1;
	u16 i = 0;
	dataLen_here= dataLen;  //pass this value trough global variable to outside functions
//#ifdef FOTA_MSG
	APP_DEBUG("<-- Data coming on https, total len: %d -->\r\n", (g_rcvdDataLen + dataLen) );
//#endif
	display_on_tftlcd(TFT_FOTA_FILE_DOWNLOAD_SIZE);	//Display updated downloading FILE SIZE on LCD, till Downloading completed.
	total_size = g_rcvdDataLen + dataLen;

//	if(FLAG.READY_TO_INITIATE_FOTA_DOWNLOAD == TRUE)	// If FOTA URL is got then save file
	{
		ret=Ql_FS_Write(g_Handle_File_LICENSEPEM, ptrData, dataLen, &writeedlen);

		if(ret == QL_RET_OK)
		{
//#ifdef FOTA_MSG
			APP_DEBUG("<-- File Writen Data : %d -->\r\n", writeedlen);
//#endif
		}
	}
	/*
	else	// start of else get FOTA URL
	{
		if ((g_rcvdDataLen + dataLen) <= sizeof(g_HttpRcvBuf))
		{
			Ql_memcpy((void*)(g_HttpRcvBuf + g_rcvdDataLen), (const void*)ptrData, dataLen);
		}
		else
		{
			if (g_rcvdDataLen < sizeof(g_HttpRcvBuf))
			{// buffer is not enough
				u32 realAcceptLen = sizeof(g_HttpRcvBuf) - g_rcvdDataLen;
				Ql_memcpy((void*)(g_HttpRcvBuf + g_rcvdDataLen), (const void*)ptrData, realAcceptLen);
#ifdef FOTA_MSG
				APP_DEBUG("<-- Rcv-buffer is not enough, discard part of data (len:%d/%d) -->\r\n", dataLen - realAcceptLen, dataLen);
#endif
			}
			else // No more buffer
			{
#ifdef FOTA_MSG
				APP_DEBUG("<-- No more buffer, discard data (len:%d) -->\r\n", dataLen);
#endif
			}
		}
	}// end of else get FOTA URL
	*/
	g_rcvdDataLen += dataLen;
}





s32 LICENSE_Download()
{
	s32 ret = -1;
	g_rcvdDataLen = 0;
	u8 Fota_Download_retry_count;
	bool FOTA_DOWNLOAD_SUCCESS = FALSE;
	FLAG.FOTA_FILE_COM_FW_INVALID = TRUE; //Set the flag as invalid state..
	FLAG.FOTA_FILE_CON_FW_INVALID = TRUE; //Set the flag as invalid state..
	for(Fota_Download_retry_count = 0 ; Fota_Download_retry_count < 10  && FOTA_DOWNLOAD_SUCCESS == FALSE; Fota_Download_retry_count++)
	{
		// Set HTTP server address (URL)
		ret = RIL_HTTP_SetServerURL((char *)HTTP_LICENSE_URL_ADDR, Ql_strlen((char *)HTTP_LICENSE_URL_ADDR));
		if(ret < 0)
		{
//#ifdef FOTA_MSG
			APP_DEBUG("<-- ERROR : HTTPS FOTA SET SERVER URL (ret = %d) -->\r\n", ret);
//#endif
			Fota_Fail_Reason_Code = 4;
			return ret;
		}
//#ifdef FOTA_MSG
		APP_DEBUG("<-- SUCCESS : HTTPS FOTA SET SERVER URL  %s-->\r\n",FOTA_URL);
//#endif
		// Send HTTP GET Request
		ret = RIL_HTTP_RequestToGet(60);
		if(ret < 0)
		{
//#ifdef FOTA_MSG
			APP_DEBUG("<-- ERROR : HTTPS GET REQUEST Failed (ret = %d) -->\r\n", ret);
//#endif
			Fota_Fail_Reason_Code = 5;
			return ret;
		}
//#ifdef FOTA_MSG
		APP_DEBUG("<-- SUCCESS : HTTPS GET REQUEST -->\r\n");
//#endif
		/*---- Read File From Server ----*/
		// Set Filesystem
		fileSys_SetFileSystem(FALSE);	// False means No filesystem Formate.

		// Create File to store
		ret = fileSys_OpenOrCreateFile((u8 *)File_Licensepem);
		if(ret < 0)
		{
			//Open file failed.
#ifdef FOTA_MSG
			APP_DEBUG("<-- Failed to Create File (%s) ret=%d -->\r\n", File_Licensepem, ret);
#endif
			Fota_Fail_Reason_Code = 6;
			return ret;
		}
//#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- File (%s) Created -->\r\n", File_FotaAppBin);
//#endif
		g_Handle_File_LICENSEPEM = ret;//Get file handle (Global Variable)

		// Read HTTP File Data and write to File
//#ifdef FOTA_MSG
		APP_DEBUG("\r\n<-- START RECEIVING FOTA FILE FROM SERVER -->\r\n");
//#endif

		ret = RIL_HTTP_ReadResponse(30, Callback_HTTPS_RecvData);

		Ql_FS_Close(g_Handle_File_LICENSEPEM);
		g_rcvdDataLen = 0;
		if(ret < 0)
		{
//#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- ERROR : FOTA FILE CURRUPT WHILE DOWNLOAD!!! (ret = %d) -->\r\n", ret);
//#endif
			// Delete Curreupt File
			ret = fileSys_deleteFile((u8 *)File_Licensepem);
			if(ret < 0)
			{
#ifdef FOTA_MSG
				APP_DEBUG("<-- ERROR : Deleting Currupt File (%s), (ret = %d) -->\r\n", File_Licensepem, ret);
#endif
				Fota_Fail_Reason_Code = 7;
				return ret;
			}

			// Re-Download File
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- RE-INITIATE FOTA FILE DOWNLOAD... -->\r\n");
#endif
		}
		else
		{
			FOTA_DOWNLOAD_SUCCESS = TRUE; //Get out of the the for loop
//#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- SUCCESS : FOTA FILE DOWNLOAD FROM HTTPS Server!!! -->\r\n");
//#endif
		}
	}

	//Return either sucess of fail due to max attempts
	if(FOTA_DOWNLOAD_SUCCESS == TRUE)
		return 0;
	else
	{
		Fota_Fail_Reason_Code = 8;
		return -1;
	}
}



s32 LICENSE_INIT()
{
	s32 ret = -1;
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- LICENSE DOWNLOAD INITIATED !!! -->\r\n");
#endif

	Store_Persistent_Data();
	Write_Error_Messages();
	//Check LICENSE.pem File is present or not: If present then delete file and check free space
	ret = fileSys_checkFile((u8*)File_Licensepem);		//To Check file FOTA.bin is available or not
	if(ret == QL_RET_OK)		//If FOTA.bin file is available then delete file
	{
		ret = fileSys_deleteFile((u8*)File_Licensepem);		//Delete FOTA.bin file
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("<-- ERROR : Deleting Existing FOTA File (%s), (ret = %d) -->\r\n", File_FotaAppBin, ret);
#endif
			Fota_Fail_Reason_Code = 2;
			return ret;
		}
	}
#ifdef FOTA_MSG
	APP_DEBUG("<-- Memory Free Space  : %lld Byte's -->\r\n",Ql_FS_GetFreeSpace(storage));		//Print free space debug message on screen
#endif
	//Display "UPDATING..." on LCD when FOTA is Initiated.
	Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_FOTA_UPDATE);

	//Report to twin properties
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- TWIN FOTA Status : Updating !!! -->\r\n");
#endif
	//Ql_strcpy(RP_FW_Upgrade_status,"1");// Report "Downloading"
	//Process_TWIN_PROPERTY_CHANGE |= REPORT_TWIN_CNTL_FW_UPDTAE_STATUS;
	//publish_data(MQTT_TWIN_REPORTED);

	// Confugure for HTTPS
	ret = RIL_HTTPS_QSSLCFG();
	if(ret < 0)
	{
#ifdef FOTA_MSG
		APP_DEBUG("<-- HTTPS Config Fail ret=%d -->\r\n", ret);
#endif
		Fota_Fail_Reason_Code = 1;
	}
	else
	{
		ret = LICENSE_Download();
		if(ret < 0)
		{
#ifdef FOTA_MSG
			APP_DEBUG("\r\n<-- FIRMWARE DOWNLOAD FAIL!!! -->\r\n");
#endif
		}
		else
		{
			if((FLAG.FOTA_FILE_COM_FW_INVALID == TRUE) || (FLAG.FOTA_FILE_CON_FW_INVALID == TRUE))// Check if the FOTA file is of correct parameters..
			{
				Fota_Fail_Reason_Code = 3;
				ret = -1;
			}
		}
	}

}

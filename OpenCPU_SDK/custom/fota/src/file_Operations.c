
#include "common_vars.h"
#include "Version.h"

#define FILEDATA

#define PERSISTENT_PARAMETER_LENGTH   50
#define MAX_PERSISTENT_PARAMETERS     10	// 7 used out of 10 and 3 are for future use

#define PERSISTENT_IOT_PARAMETER_LENGTH   150
#define MAX_PERSISTENT_IOT_PARAMETERS 10	// 5 used out of 10 and 5 are for future use


//char FactCfg_LAT[10] 	= {'\0'};
//char FactCfg_LONG[10] 	= {'\0'};
//char *file_offset               = "Offset.txt";

char *file_data                 = "PendingData.txt";
char *file_devicedata           = "Permanent_DataStore";
char *file_IoTdata              = "Permanent_IoT_DataStore";
char *Error_Messagesdata        = "Error_Messages.txt";
char *File_FotaAppBin           = "Fota.bin";
char *File_Licensepem           = "LICENSE.pem";

extern u32 wtdid;
char payload_array[MAX_PAYLOADS][PAYLOAD_LENGTH];
extern char payload_count;
char persistent_parameter_array[MAX_PERSISTENT_PARAMETERS][PERSISTENT_PARAMETER_LENGTH];
char persistent_parameter_IoT_array[MAX_PERSISTENT_IOT_PARAMETERS][PERSISTENT_IOT_PARAMETER_LENGTH];

char AZURE_IoT_device_ID[25]    = {'\0'}; //Device serial ID
char AZURE_IoT_DPS_ID[25]       = {'\0'}; // Azure DPS Id Scope
char AZURE_assigned_IoT_Hub[50] = {'\0'}; // Azure Assigned IoT Hub 

/*****************************************************************************
 * FUNCTION
 *  fileSys_SetFileSystem
 *
 * DESCRIPTION
 *  This function is used to select storage type and set file system.
 *
 * PARAMETERS
 *
 * RETURNS
 *
 *****************************************************************************/
s32 fileSys_SetFileSystem(bool format)
{
	s32 ret = -1;
	s64 Free_space = 0;
	s64 Total_space = 0;

#if defined (__TEST_FOR_RAM_FILE__)
	Enum_FSStorage storage = Ql_FS_RAM;
#ifdef FILE_STORAGE
	APP_DEBUG("\r\n<-- File System(RAM) Selected! -->\r\n");
#endif
#elif defined (__TEST_FOR_UFS__)
	Enum_FSStorage storage = Ql_FS_UFS;
#ifdef FILE_STORAGE
	APP_DEBUG("\r\n<-- File System(UFS) Selected! -->\r\n");
#endif
#else   //!__TEST_FOR_MEMORY_CARD__
	Enum_FSStorage storage = Ql_FS_SD;
#ifdef FILE_STORAGE
	APP_DEBUG("\r<-- File System(SD) Selected! -->\r\n");
#endif
#endif

	// format Storage memory
	if(format == TRUE)
	{
		ret = Ql_FS_Format(storage);
		if(ret==QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("\r<-- Ql_FS_Format(storage=%d).Format OK! -->\r\n",storage);
#endif
		}
		else
		{
#ifdef FILE_STORAGE
			APP_DEBUG("\r<-- Format error. -->\r\n");
#endif
		}
	}
	//check freespace
	Free_space  = Ql_FS_GetFreeSpace(storage);
	if(Free_space < 0)
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- STORAGE NOT PRESENT -->\r\n");
#endif
		return -1;
	}

	//check total space
	Total_space = Ql_FS_GetTotalSpace(storage);
	if(Total_space < 0)
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- STORAGE NOT PRESENT -->\r\n");
#endif
		return -1;
	}

#ifdef DEBUG_MSG
	APP_DEBUG("<-- STORAGE Free Space : %lld Byte's out of total %lld Byte's-->\r\n",Free_space, Total_space);
#endif

	Ql_memset(filePath, 0x0, sizeof(filePath));
	Ql_sprintf(filePath,"%s\\\0",(char*)PATH_ROOT);

	ret = Ql_FS_CheckDir(filePath);
	if(ret != QL_RET_OK)
	{
#ifdef FILE_STORAGE
		APP_DEBUG("\r<-- Dir(%s) is not exist, creating.... -->\r\n", filePath);
#endif
		ret  = Ql_FS_CreateDir(filePath);
		if(ret != QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("\r<-- failed!! Create Dir(%s) fail-->\r\n", filePath);
#endif
			return -2;
		}
	}
	return ret;
}

/*****************************************************************************
 * FUNCTION
 *  fileSys_OpenOrCreateFile
 *
 * DESCRIPTION
 *  This function is used to open/create file in file system
 *
 * PARAMETERS
 *	<file_name>     Name of the file to be open/create
 *
 * RETURNS
 *  <ret>     SUCCESS or ERROR value
 *****************************************************************************/
s32 fileSys_OpenOrCreateFile(u8 *file_name)
{
	s32 ret = -1;
	s32 filehandle = -1;

	Ql_memset(filePath, 0x0, sizeof(filePath));
	Ql_sprintf(filePath,"%s\\%s\0", PATH_ROOT, file_name);

	//Creates a file
	ret = Ql_FS_Open(filePath, QL_FS_READ_WRITE|QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
	}
	else
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Error File Open/Create : %s -->\r\n", filePath);
#endif
	}

	Ql_FS_Flush(filehandle);

	return ret;
}


/*****************************************************************************
 * FUNCTION
 *  fileSys_ReadFile
 *
 * DESCRIPTION
 *  This function is used to read file stored in file system
 *
 * PARAMETERS
 *
 * RETURNS
 *  <strBuf>     pointer to the red data buffer
 *****************************************************************************/
/*
u8 *fileSys_ReadFile(s32 filehandle)
{
	s32 ret = -1;

	u8 *strBuf = Ql_MEM_Alloc(FILE_DATA_LENGTH);

	ret=Ql_FS_Seek(filehandle, OFFSET , QL_FS_FILE_BEGIN);

	//Read data from file
	Ql_memset(strBuf,0x0,FILE_DATA_LENGTH);
	ret = Ql_FS_Read(filehandle, strBuf, FILE_DATA_LENGTH, &readedlen);
	if(QL_RET_OK == ret)
	{
		//APP_DEBUG("<-- Read Success\r\n");
	}

	Ql_MEM_Free(parameter_);
	parameter_ = NULL;
	return strBuf;

	//!!!!! important need to deallocate before leaving this function
}
 */

/*****************************************************************************
 * FUNCTION
 * s32 fileSys_WriteFile(s32 filehandle, u8 *data_to_write, u8 write_type, u16 wtite_to_size)
 *
 * DESCRIPTION
 *  This function is used to write data to file in file system
 *
 * PARAMETERS
 *      <data_to_write>     Pointer to the Buffer to write
 *      <write_type>     write type - 0 : CASCADE_WRITE
                                      1 : OVER_WRITE
                                      1 : OVER_WRITE_TO_SIZE
 * RETURNS
 *  <ret>     SUCCESS or ERROR value
 *****************************************************************************/
s32 fileSys_WriteFile(s32 filehandle, u8 *data_to_write, u8 write_type, u16 wtite_to_size)
{
	s32 ret = -1;
	if(write_type == CASCADE_WRITE)
	{
		ret = Ql_FS_Seek(filehandle, OFFSET , QL_FS_FILE_END);
		if(ret != QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<-- Error File Ql_FS_Seek [CASCADE_WRITE] ret : %d -->\r\n", ret);
#endif
		}

		ret=Ql_FS_Write(filehandle, data_to_write, Ql_strlen((char*)data_to_write), &writeedlen);
		if(ret != QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<-- Error in File Write [CASCADE_WRITE] = %d-->\r\n", ret);
#endif
		}
	}
	else if(write_type == OVER_WRITE)
	{
		//Move the file pointer to the starting position.
		ret=Ql_FS_Seek(filehandle, OFFSET , QL_FS_FILE_BEGIN);
		if(ret != QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<-- Error File Ql_FS_Seek [OVER_WRITE] ret : %d -->\r\n", ret);
#endif
		}

		ret=Ql_FS_Write(filehandle, data_to_write, Ql_strlen((char*)data_to_write), &writeedlen);
		if(ret != QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<-- Error in File Write [OVER_WRITE] = %d-->\r\n", ret);
#endif
		}
	}
	else if(write_type == OVER_WRITE_TO_SIZE && wtite_to_size)
	{
		//Move the file pointer to the starting position.
		ret=Ql_FS_Seek(filehandle, OFFSET , QL_FS_FILE_BEGIN);
		if(ret != QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<-- Error File Ql_FS_Seek [OVER_WRITE_TO_SIZE] ret : %d -->\r\n", ret);
#endif
		}

		ret=Ql_FS_Write(filehandle, data_to_write, wtite_to_size, &writeedlen);
		if(ret != QL_RET_OK)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<-- Error in File Write [OVER_WRITE_TO_SIZE] = %d-->\r\n", ret);
#endif
		}
	}
	else
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Invalid Write Type = %d , use CASCADE_WRITE/OVER_WRITE-->\r\n", write_type);
#endif
	}
	data_to_write = NULL;

	return ret;
}

/*****************************************************************************
 * FUNCTION
 *  fileSys_checkFile
 *
 * DESCRIPTION
 *  This function is used to check file present or not in file system
 *
 * PARAMETERS
 *	<file_name>     Name of the file to be checked
 *
 * RETURNS
 *  <ret>     SUCCESS or ERROR value
 *****************************************************************************/
s32 fileSys_checkFile(u8 *file_name)
{
	s32 ret = -1;

	Ql_memset(filePath, 0x0, sizeof(filePath));
	Ql_sprintf(filePath,"%s\\%s\0", PATH_ROOT, file_name);

	ret = Ql_FS_Check(filePath);

	return ret;
}

/*****************************************************************************
 * FUNCTION
 *  fileSys_GetFilePos
 *
 * DESCRIPTION
 *  This function is used to pointer position of file present in file system
 *  It help us to find out how much data has been writen to file.
 *
 * PARAMETERS
 *
 * RETURNS
 *  <ret>     SUCCESS or ERROR value
 *****************************************************************************/
/*
s32 fileSys_GetFilePos(s32 filehandle)
{
	s32 file_position;
	//Get the position of the file pointer
	file_position = Ql_FS_GetFilePosition(filehandle);

	return file_position;
}
 */
/*****************************************************************************
 * FUNCTION
 *  fileSys_CloseFile
 *
 * DESCRIPTION
 *  This function is used to close already opened file
 *
 * PARAMETERS
 *
 * RETURNS
 *****************************************************************************/
void fileSys_CloseFile(s32 filehandle)
{
	//Close the file
	Ql_FS_Close(filehandle);
	filehandle=-1;
}

/*****************************************************************************
 * FUNCTION
 *  fileSys_deleteFile
 *
 * DESCRIPTION
 *  This function is used to delete file present in file system
 *
 * PARAMETERS
 *	<file_name>     Name of the file to delete
 *
 * RETURNS
 *  <ret>     SUCCESS or ERROR value
 *****************************************************************************/
s32 fileSys_deleteFile(u8 *file_name)
{
	s32 ret = -1;

	Ql_memset(filePath, 0x0, sizeof(filePath));
	Ql_sprintf(filePath,"%s\\%s\0", PATH_ROOT, file_name);

	ret = Ql_FS_Delete(filePath);
#ifdef FILE_STORAGE
	if(QL_RET_OK == ret)
	{
		APP_DEBUG("\r\n<-- File %s deleted.-->\r\n",file_name);
	}
	else
	{
		APP_DEBUG("\r\n<-- Error : Ql_FS_Delete %s ret : %d-->\r\n", __FUNCTION__, ret);
	}
#endif
	return ret;
}
/*****************************************************************************
 * FUNCTION
 *  fileSys_getFileSize
 *
 * DESCRIPTION
 *  This function is used to detect the file size in bytes
 *
 * PARAMETERS
 *	<file_name>     Name of the file to delete
 *
 * RETURNS
 *  <ret>     File size in bytes
 *****************************************************************************/
s32 fileSys_getFileSize(u8 *file_name)
{
	s32 ret = -1;

	Ql_memset(filePath, 0x0, sizeof(filePath));
	Ql_sprintf(filePath,"%s\\%s\0", PATH_ROOT, file_name);
	ret = Ql_FS_GetSize(filePath);
	if(ret < QL_RET_OK)
	{
		//The file does not exist
#ifdef FILE_STORAGE
		APP_DEBUG("\r\n<-- File (%s) Not Present (ret = %d) -->\r\n", File_FotaAppBin, ret);
#endif
		return -1;
	}

	return ret;
}

//***************************************************************************************//
//******************** Store Persistent Parameters in the UFS memory area ***************//
//***************************************************************************************//
//Following Persistent parameters are stored..
//1. Last sucessful cloud Connected time stamp in UTC
//2. GPS information in terms of Lattitude and Longitude read out of Cellular service provider communicatoin
//3. Last Water Balance Recharge amount
//4. Last Water Balance Recharge ID
//5. Last Water Balance Recharge Time Stamp
//6. Last Language Configured ( 0-> ENGLISH Default , 1-> Indonesian BAHASA)
//7. Empty and available for future use
//8. Empty and available for future use
//9. Empty and available for future use
//10. Empty and available for future use
//Each persistent parameter starts with '{'
s32 Store_Persistent_Data(void)
{
	char temp[256] = {0};
	s32 ret = -1;
	s32 file_handle = -1;
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<--Store to Persistent Parameters File -->\n\r");
#endif
	//create file.. if not available or open the file if avaialble
	file_handle = fileSys_OpenOrCreateFile((u8 *)file_devicedata);
	if(file_handle >= QL_RET_OK)
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Writing Persistent Parameters ..\n\r");
#endif
		//1. Last sucessful cloud Connected time stamp in UTC
		Ql_sprintf(temp,"{%s",last_connectedtime);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_array[0],temp);
		else
		{
			if(persistent_parameter_array[0][0] != '{') // if it is an invalid parameter mark it
			{
				persistent_parameter_array[0][0] = '{';
				persistent_parameter_array[0][1] = '\0';
			}
		}
#ifdef DEBUG_MSG
		APP_DEBUG("1. Last Connected Time      --> %s\n\r",persistent_parameter_array[0]);
#endif
		//2. GPS information in terms of Lattitude and Longitude read out of Cellular service provider communicatoin
		Ql_sprintf(temp,"{%s,%s",latitude_,longitude_);
		if(temp[1] != '\0' && temp[1] != ',') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_array[1],temp);
		else
		{
			if(persistent_parameter_array[1][0] != '{') // if it is an invalid parameter mark it
			{
				persistent_parameter_array[1][0] = '{';
				persistent_parameter_array[1][1] = '\0';
			}
		}
#ifdef DEBUG_MSG
		APP_DEBUG("2. GPS Co-ordinates         --> %s\n\r",persistent_parameter_array[1]);
#endif

		//3. Last Water Balance Recharge amount
		Ql_sprintf(temp,"{%s",cmdrcbal);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_array[2],temp);
		else
		{
			if(persistent_parameter_array[2][0] != '{') // if it is an invalid parameter mark it
			{
				persistent_parameter_array[2][0] = '{';
				persistent_parameter_array[2][1] = '\0';
			}
		}
#ifdef DEBUG_MSG
		APP_DEBUG("3. Last Recharge value      --> %s\n\r",persistent_parameter_array[2]);
#endif

		//4. Last Water Balance Recharge ID
		Ql_sprintf(temp,"{%s",Received_RCID);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_array[3],temp);
		else
		{
			if(persistent_parameter_array[3][0] != '{') // if it is an invalid parameter mark it
			{
				persistent_parameter_array[3][0] = '{';
				persistent_parameter_array[3][1] = '\0';
			}
		}
#ifdef DEBUG_MSG
		APP_DEBUG("4. Last Recharge ID         --> %s\n\r",persistent_parameter_array[3]);
#endif

		//5. Last Water Balance Recharge Time Stamp
		Ql_sprintf(temp,"{%s",Received_RCTS);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_array[4],temp);
		else
		{
			if(persistent_parameter_array[4][0] != '{') // if it is an invalid parameter mark it
			{
				persistent_parameter_array[4][0] = '{';
				persistent_parameter_array[4][1] = '\0';
			}
		}
#ifdef DEBUG_MSG
		APP_DEBUG("5. Last Recharge Time Stamp --> %s\n\r",persistent_parameter_array[4]);
#endif

		//6. Language COnfigured
		Ql_sprintf(temp,"{%d",language_index);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_array[5],temp);
		else
		{
			if(persistent_parameter_array[5][0] != '{') // if it is an invalid parameter mark it as default ENGLISH language
			{
				persistent_parameter_array[5][0] = '{';
				persistent_parameter_array[5][1] = '0';
				persistent_parameter_array[5][2] = '\0';
			}
		}
#ifdef DEBUG_MSG
		APP_DEBUG("6. Language Configured      --> ");
		if(persistent_parameter_array[5][1] == '0')
		{
			APP_DEBUG("ENGLISH \n\r");
		}
		else if(persistent_parameter_array[5][1] == '1')
		{
			APP_DEBUG("BAHASA \n\r");
		}
		else
		{
			APP_DEBUG("INVALID \n\r");
		}
#endif

		ret = fileSys_WriteFile(file_handle, (u8*)persistent_parameter_array, OVER_WRITE_TO_SIZE, MAX_PERSISTENT_PARAMETERS * PERSISTENT_PARAMETER_LENGTH);
		if(QL_RET_OK != ret)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<--Error : fileSys_WriteFile [%s] ret : %d-->\r\n",__FUNCTION__,ret);
#endif
			fileSys_CloseFile(file_handle);
			return -1;
		}
		fileSys_CloseFile(file_handle);
	}
	else
	{
		//Open file failed.
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Failed to Create File (%s) ret=%d -->\r\n", file_devicedata, ret);
#endif
		return -1;
	}

	return ret;
}


s32 Read_Persistent_Data(void)
{
	u32 i = 0;

	s32 ret = -1;
	s32 fileHandl = -1;
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- Read From Persistent Parameters File -->\r\n");
#endif

	char *parameter_ = NULL;

	fileHandl = fileSys_checkFile((u8*)file_devicedata);

	if(fileHandl == QL_RET_OK) // If file already present
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Persistent parameters: file present : ->\r\n");
#endif
		fileHandl = fileSys_OpenOrCreateFile((u8*)file_devicedata); //Open the file

		if(fileHandl >= QL_RET_OK)
		{
			for(i = 0; i<MAX_PERSISTENT_PARAMETERS ; i++) // Scan for all persistent parameters one by one in the file
			{
				Ql_WTD_Feed(wtdid);
				ret = Ql_FS_Seek(fileHandl, i*PERSISTENT_PARAMETER_LENGTH , QL_FS_FILE_BEGIN); // Point to the start of required message in the file

				if(QL_RET_OK != ret)
				{
#ifdef FILE_STORAGE
					APP_DEBUG("\r\nError : Ql_FS_Seek ret : %d\r\n",ret);
#endif
				}

				parameter_ = (char*)Ql_MEM_Alloc(PERSISTENT_PARAMETER_LENGTH);
				Ql_memset(parameter_, 0x0,PERSISTENT_PARAMETER_LENGTH );
				ret = Ql_FS_Read(fileHandl,(u8*)parameter_,PERSISTENT_PARAMETER_LENGTH, &readedlen); // Read the payload from the file

				if(QL_RET_OK != ret)
				{
#ifdef FILE_STORAGE
					APP_DEBUG("\r\nError : Ql_FS_Read ret : %d\r\n",ret);
#endif
				}

#ifdef DEBUG_MSG
				if(i==0)
				{
					APP_DEBUG("1. Last Connect time");
				}
				else if(i==1)
				{
					APP_DEBUG("2. GPS Co-ordinates");
				}
				else if(i==2)
				{
					APP_DEBUG("3. Last Recharge value");
				}
				else if(i==3)
				{
					APP_DEBUG("4. Last Recharge ID");
				}
				else if(i==4)
				{
					APP_DEBUG("5. Last Recharge Time Stamp");
				}
				else if(i==5)
				{
					APP_DEBUG("6. Language Configured");
				}
#endif
				if(parameter_[0] != '{') //A parameter is invalid if it start with character other than '{'
				{

					persistent_parameter_array[i][0]= '{';
					persistent_parameter_array[i][1]= NULL;
#ifdef DEBUG_MSG
					APP_DEBUG(": -->\n\r");
#endif
				}
				else // Valid parameter
				{
#ifdef DEBUG_MSG
					APP_DEBUG(": %s -->\n\r",&parameter_[1]);
#endif
					//Read the parameter and assign them to global variables..
					if(i == 0)//Last sucessful cloud Connected time stamp in UTC
					{
						Ql_strcpy(last_connectedtime,&parameter_[1]);
					}
					else if(i == 1)//GPS information in terms of Lattitude and Longitude read out of Cellular service provider communicatoin
					{
						Ql_strcpy(latlong,&parameter_[1]);
					}
					else if(i == 2)//Last Water Balance Recharge amount
					{
						Ql_strcpy(cmdrcbal,&parameter_[1]);
					}
					else if(i == 3)//Last Water Balance Recharge ID
					{
						Ql_strcpy(Received_RCID,&parameter_[1]);
					}
					else if(i == 4)//Last Water Balance Recharge Time Stamp
					{
						Ql_strcpy(Received_RCTS,&parameter_[1]);
					}
					else if(i == 5)//Language Configured
					{
						language_index = Ql_atoi(&parameter_[1]);
					}

					//Copy in the array that will be used to stoer back while power down
					strcpy(persistent_parameter_array[i],parameter_);
				}
				Ql_MEM_Free(parameter_);
				parameter_ = NULL;
			}
			fileSys_CloseFile((s32)fileHandl); //Close the file
		}
	}
	else
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Persistent parameters: file not present->\r\n");
#endif
		//return RET_FAIL;
	}

	return 1;
}

//***************************************************************************************//
//******************** Store IoT Platorm Persistent Parameters in the UFS memory area ***************//
//***************************************************************************************//
//Following Persistent parameters are stored..
//1. Device Serial ID - 15 characters
//2. Assigned DPS ID 
//3. Assigned IoT Hub
//4. Empty and available for future use
//5. Empty and available for future use
//6. Empty and available for future use
//7. Empty and available for future use
//8. Empty and available for future use
//9. Empty and available for future use
//10. Empty and available for future use
//Each persistent parameter starts with '{'
s32 Store_IoT_Persistent_Data(void)
{
	char temp[256] = {0};
	s32 ret = -1;
	s32 file_handle = -1;
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<--Store to IoT Persistent Parameters File -->\n\r");
#endif
	//create file.. if not available or open the file if avaialble
	file_handle = fileSys_OpenOrCreateFile((u8 *)file_IoTdata);
	if(file_handle >= QL_RET_OK)
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Writing IoT Persistent Parameters ..\n\r");
#endif
		//1. Device Serial ID string
		Ql_sprintf(temp,"{%s",AZURE_IoT_device_ID);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_IoT_array[0],temp);
		else
		{
			persistent_parameter_IoT_array[0][0] = '{';
			persistent_parameter_IoT_array[0][1] = '\0';
		}
#ifdef DEBUG_MSG
		APP_DEBUG("1. Device Serial ID --> %s\n\r",persistent_parameter_IoT_array[0]);
#endif

		//2. IoT DPS  details
		Ql_sprintf(temp,"{%s",AZURE_IoT_DPS_ID);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_IoT_array[1],temp);
		else
		{
			persistent_parameter_IoT_array[1][0] = '{';
			persistent_parameter_IoT_array[1][1] = '\0';
		}	
#ifdef DEBUG_MSG
		APP_DEBUG("2. Assigned DPS ID  --> %s\n\r",persistent_parameter_IoT_array[1]);
#endif

		//3. IoT Hub details
		Ql_sprintf(temp,"{%s",AZURE_assigned_IoT_Hub);
		if(temp[1] != '\0') //Update the last stored parameter only if the parameter is not NULL
			strcpy(persistent_parameter_IoT_array[2],temp);
		else
		{
			persistent_parameter_IoT_array[2][0] = '{';
			persistent_parameter_IoT_array[2][1] = '\0';
		}	
#ifdef DEBUG_MSG
		APP_DEBUG("3. Assigned IoT Hub --> %s\n\r",persistent_parameter_IoT_array[2]);
#endif

		ret = fileSys_WriteFile(file_handle, (u8*)persistent_parameter_IoT_array, OVER_WRITE_TO_SIZE, MAX_PERSISTENT_IOT_PARAMETERS * PERSISTENT_IOT_PARAMETER_LENGTH);
		if(QL_RET_OK != ret)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<--Error : fileSys_WriteFile [%s] ret : %d-->\r\n",__FUNCTION__,ret);
#endif
			fileSys_CloseFile(file_handle);
			return -1;
		}
		fileSys_CloseFile(file_handle);
	}
	else
	{
		//Open file failed.
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Failed to Create File (%s) ret=%d -->\r\n", file_IoTdata, ret);
#endif
		return -1;
	}

	return ret;
}

s32 Read_IoT_Persistent_Data(void)
{
	u32 i = 0;

	s32 ret = -1;
	s32 fileHandl = -1;
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- Read From IoT Persistent Parameters File -->\r\n");
#endif

	char *parameter_ = NULL;

	fileHandl = fileSys_checkFile((u8*)file_IoTdata);

	if(fileHandl == QL_RET_OK) // If file already present
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Persistent IoT parameters: file present : ->\r\n");
#endif
		fileHandl = fileSys_OpenOrCreateFile((u8*)file_IoTdata); //Open the file

		if(fileHandl >= QL_RET_OK)
		{
			for(i = 0; i<MAX_PERSISTENT_IOT_PARAMETERS ; i++) // Scan for all persistent parameters one by one in the file
			{
				Ql_WTD_Feed(wtdid);
				ret = Ql_FS_Seek(fileHandl, i*PERSISTENT_IOT_PARAMETER_LENGTH , QL_FS_FILE_BEGIN); // Point to the start of required message in the file

				if(QL_RET_OK != ret)
				{
#ifdef FILE_STORAGE
					APP_DEBUG("\r\nError : Ql_FS_Seek ret : %d\r\n",ret);
#endif
				}

				parameter_ = (char*)Ql_MEM_Alloc(PERSISTENT_IOT_PARAMETER_LENGTH);
				Ql_memset(parameter_, 0x0,PERSISTENT_IOT_PARAMETER_LENGTH );
				ret = Ql_FS_Read(fileHandl,(u8*)parameter_,PERSISTENT_IOT_PARAMETER_LENGTH, &readedlen); // Read the payload from the file

				if(QL_RET_OK != ret)
				{
#ifdef FILE_STORAGE
					APP_DEBUG("\r\nError : Ql_FS_Read ret : %d\r\n",ret);
#endif
				}

#ifdef DEBUG_MSG
				if(i==0)
				{
					APP_DEBUG("1. Device Serial ID");
				}
				else if(i==1)
				{
					APP_DEBUG("2. Assigned DPS ID");
				}
				else if(i==2)
				{
					APP_DEBUG("3. Assigned IoT Hub");
				}

#endif
				if(parameter_[0] != '{') //A parameter is invalid if it start with character other than '{'
				{

					persistent_parameter_IoT_array[i][0]= '{';
					persistent_parameter_IoT_array[i][1]= NULL;
#ifdef DEBUG_MSG
					APP_DEBUG(": -->\n\r");
#endif
				}
				else // Valid parameter
				{
					APP_DEBUG(": %s -->\n\r",&parameter_[1]);
					//Read the parameter and assign them to global variables..
					if(i == 0)//Device serial ID
					{
						if(Ql_strlen(&parameter_[1]))// There is a valid string i.e. non zero string length
						{
							if(FLAG.AUTOPROVISIONING_COMPLETE == FALSE)//Do not recover if recovered earlier
							{
								Ql_strcpy(AZURE_IoT_device_ID,&parameter_[1]);
							}
						}
					}
					else if(i == 1)//Azure DPS ID
					{
						if(Ql_strlen(&parameter_[1])) // There is a valid string i.e. non zero string length
						{
							if(FLAG.AUTOPROVISIONING_COMPLETE == FALSE)//Do not recover if recovered earlier
							{
								Ql_strcpy(AZURE_IoT_DPS_ID,&parameter_[1]);
							}
						}
					}
					else if(i == 2)//Azure Assigned Hub
					{
						if(Ql_strlen(&parameter_[1])) // There is a valid string i.e. non zero string length
						{
							Ql_strcpy(AZURE_assigned_IoT_Hub,&parameter_[1]);
							if(FLAG.AUTOPROVISIONING_COMPLETE == FALSE)//Do not recover if recovered earlier
							{
								Ql_sprintf(DEVICE_USER_ID, "%s/%s",AZURE_assigned_IoT_Hub,AZURE_IoT_device_ID);
								Ql_sprintf(DEVICE_CLIENT_ID, "%s",AZURE_IoT_device_ID);
#ifndef POWER_ON_DPS_CONNECT
								FLAG.AUTOPROVISIONING_COMPLETE = TRUE;
#endif
								Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_CLOUD_CON_OFF_SYMBOL);
							}
						}
					}

					//Copy in the array that will be used to stoer back while power down
					strcpy(persistent_parameter_IoT_array[i],parameter_);
				}
				Ql_MEM_Free(parameter_);
				parameter_ = NULL;
			}
			fileSys_CloseFile((s32)fileHandl); //Close the file
		}
	}
	else
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Persistent IoT parameters: file not present->\r\n");
#endif
		//return RET_FAIL;
	}

	return 1;
}
/*****************************************************************
 * Function: Find_Newest_Payload_Number(void)
 *
 * Description:
 *  This function scans the entire MQTT payload messages in the log file and return the number for new MQTT message payload to store
 *
 * Parameters:None
 *
 * Return:
 *
 *****************************************************************/
void Find_Newest_Payload_Number(void)
{
	s32  i = 0;
	s32  ret = -1;
	s32  filehandleData = -1;
	u8 valid_payload = 0;
#ifdef FILE_STORAGE
	APP_DEBUG("\r\n<-- Find Next MQTT Pending Message number to Wtite the payload -->\n\r");
#endif
	// Open the log file that stored unpublished MQTT messages or pending messgaes
	filehandleData = fileSys_OpenOrCreateFile((u8*)file_data);
	if(filehandleData >= QL_RET_OK)
	{
		for(i = 0; i<MAX_PAYLOADS ; i++) // Read through all payloads / records one by one and copy in the corresponding array records
		{
			Ql_WTD_Feed(wtdid);
			ret = Ql_FS_Seek(filehandleData, i*PAYLOAD_LENGTH , QL_FS_FILE_BEGIN); // Reach to the start of the current payload number

			if(QL_RET_OK != ret)
			{
#ifdef FILE_STORAGE
				APP_DEBUG("\r\nError : Ql_FS_Seek ret : %d\r\n",ret);
#endif
			}

			ret = Ql_FS_Read(filehandleData,(u8*)payload_array[i],PAYLOAD_LENGTH, &readedlen); // Read the entire payload and copy in the array

			if(QL_RET_OK != ret)
			{
#ifdef FILE_STORAGE
				APP_DEBUG("\r\nError : Ql_FS_Read ret : %d\r\n",ret);
#endif
			}
#ifdef FILE_STORAGE
			APP_DEBUG("\r\n Pending MQTT Messgae[%d][0]: %c\r\n",i,payload_array[i][0]);
#endif
		}
		// Close the log file that stored unpublished MQTT messages or pending messgaes.. as all the payloads / messages are now copied in array temporarly
		// Array pointer handling is easy than file pointer handling :-)
		fileSys_CloseFile(filehandleData);

		// Scan the array for all records and find out where is the new MQTT message payload to be stored i.e. an empty payload storage just after latest Message
		for(i = 0; i<MAX_PAYLOADS ; i++)
		{
			if(valid_payload && payload_array[i][0]!='{')// An empty / invalid payload found after a valid messgae / payload storage
			{
				payload_count = i;
				break;
			}
			else if((!valid_payload) && payload_array[i][0]!='{')
			{
				payload_count = 0; // No valid payload found to re-wind to start of the array.
			}
			if(payload_array[i][0] == '{')
				valid_payload++; // This is a valid payload so scan the next payload.
		}
	}
}

/*****************************************************************
 * Function: Write_And_Store_Pending_MQTT_Log_Messages(void)
 *
 * Description:
 *  This function Stores the entire MQTT payload messages in the log file
 *
 * Parameters:None
 *
 * Return:
 *
 *****************************************************************/
s32 Write_And_Store_Pending_MQTT_Log_Messages(char* payload)
{
	s32  filehandle = -1;
	s32 ret = -1;
	Find_Newest_Payload_Number(); // Find the next empty payload number in the storage
	strcpy(payload_array[payload_count],payload);
#ifdef DEBUG_MSG
	APP_DEBUG("\r<-- Storing in New Pending MQTT Message [%d] -->\r\n",payload_count);
#endif
	//Make space for next payload. Mark the next payload as invalid or empty payload to store next / upcomming payload
	if(payload_count == (MAX_PAYLOADS -1)) // This is the last payload so it is time to rewind.
	{
		payload_array[0][0] = NULL;		//Set first payload NULL
		payload_count = 0;
	}
	else
	{
		payload_count++;
		payload_array[payload_count][0] = NULL;		//Set next payload NULL
	}


	//create file.. if not available or open the file if avaialble
	filehandle = fileSys_OpenOrCreateFile((u8*)file_data);
	if(filehandle >= QL_RET_OK)
	{
		ret = fileSys_WriteFile(filehandle,(u8*)payload_array,OVER_WRITE_TO_SIZE, MAX_PAYLOADS*PAYLOAD_LENGTH);
		if(QL_RET_OK != ret)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("\r\nError : fileSys_WriteFile [%s] ret : %d\r\n",__FUNCTION__,ret);
#endif
			fileSys_CloseFile(filehandle);
			return -1;
		}
		else
			PUBLISH_RESPONSE_REQ_source |= SCHEDULE_PUBLISH_PENDING_MESSAGES; //schedule event to send pending messgaes in next connection success
		fileSys_CloseFile(filehandle);
	}
	else
	{
#ifdef FILE_STORAGE
		APP_DEBUG("\r\nError : File Open/Create [%s] ret : %d\r\n",file_data,ret);
#endif
		return -1;
	}
	return 1;
}



/*****************************************************************
 * Function: Write_Error_Messages(void)
 *
 * Description:
 *  This function Stores each error flag into file
 *
 * Parameters:None
 *
 * Return:
 *
 *****************************************************************/
s32 Write_Error_Messages(void)
{

	int i;
	s32 ret = -1;
	s32 file_handle = -1;
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<--Store Fault Flags:");
	for(i=0;i<11;i++)
	{
		if(Error_messages_array[i])
			APP_DEBUG("E[%d], ",i+1);
	}
	APP_DEBUG(" -->\n\r");
#endif

	//create file.. if not available or open the file if available
	file_handle = fileSys_OpenOrCreateFile((u8 *)Error_Messagesdata);
	if(file_handle >= QL_RET_OK)
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Writing Fault Messages ..\n\r");
#endif

		ret = fileSys_WriteFile(file_handle, (u8*)Error_messages_array, OVER_WRITE_TO_SIZE, MAX_ERROR_COUNT);
		if(QL_RET_OK != ret)
		{
#ifdef FILE_STORAGE
			APP_DEBUG("<--Error : fileSys_WriteFile [%s] ret : %d-->\r\n",__FUNCTION__,ret);
#endif
			fileSys_CloseFile(file_handle);
			return -1;
		}
		fileSys_CloseFile(file_handle);
	}
	else
	{
		//Open file failed.
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Failed to Create File (%s) ret=%d -->\r\n", Error_Messagesdata, ret);
#endif
		return -1;
	}

	return 1;

}

//***************************************************************************************//
//******************** *********Read Fault Flags******************************************//
/****************************************************************************************
 * Function: s32 Read_Fault_Flag(void)
 *
 * Description:
 *  This function reads a fault messages.
 *
 * Parameters:None
 *
 * Return:
 *
 *****************************************************************/
s32 Read_Fault_Flag_File(void)
{
	u32 i = 0;

	s32 ret = -1;
	s32 fileHandl = -1;

	fileHandl = fileSys_checkFile((u8*)Error_Messagesdata);
	if(fileHandl == QL_RET_OK) // If file already present
	{
#ifdef FILE_STORAGE
		APP_DEBUG("<-- Fault flag  file present : ->\r\n");
#endif
		fileHandl = fileSys_OpenOrCreateFile((u8*)Error_Messagesdata); //Open the file
		if(fileHandl >= QL_RET_OK)
		{
			ret = Ql_FS_Seek(fileHandl, OFFSET , QL_FS_FILE_BEGIN); // Reach to the start of the current payload number
			if(QL_RET_OK != ret)
			{
#ifdef FILE_STORAGE
				APP_DEBUG("\r\nError : Ql_FS_Seek ret : %d\r\n",ret);
#endif
			}

			ret = Ql_FS_Read(fileHandl,(u8*)Error_messages_array,MAX_ERROR_COUNT, &readedlen); // Read the payload from the file

			if(QL_RET_OK != ret)
			{
#ifdef FILE_STORAGE
				APP_DEBUG("\r\nError : Ql_FS_Read ret : %d\r\n",ret);
#endif
			}
#ifdef DEBUG_MSG
			APP_DEBUG("\r\n<--Read Fault Flags:");
			for(i=0;i<11;i++)
			{
				if(Error_messages_array[i])
					APP_DEBUG("E[%d], ",i+1);
			}
			APP_DEBUG(" -->\n\r");
#endif
		}
		fileSys_CloseFile((s32)fileHandl); //Close the file
	}
	return 1;
}


s32 Read_License(void)
{

	char new_CK0[2000];
	char new_CC0[2000];
	char new_CA0[2000];
	int j,ctr;
	j=0; ctr=0;
	u32 i = 0;

	s32 ret = -1;
	s32 fileHandl = -1;
	s32  parameter_length_ID  = 0;
	char parameter_ID_temp[6000];
	char *parameter_ID = NULL;
	//#ifdef FILE_STORAGE
	APP_DEBUG("\r\n<-- Read  Device ID -->\r\n");
	//#endif
	fileHandl = fileSys_checkFile((u8*)File_Licensepem);
	if(fileHandl == QL_RET_OK) // If file already present
	{
		ret = fileSys_getFileSize(File_Licensepem);
		APP_DEBUG("<-- File size: %d ->\r\n",ret);
		//#ifdef FILE_STORAGE
		APP_DEBUG("<-- Device ID: file present : ->\r\n");
		//#endif
		fileHandl = fileSys_OpenOrCreateFile((u8*)File_Licensepem); //Open the file
		if(fileHandl >= QL_RET_OK)
		{
			ret = Ql_FS_Seek(fileHandl, OFFSET , QL_FS_FILE_BEGIN); // Reach to the start of the current payload number
			if(QL_RET_OK != ret)
			{
				//#ifdef FILE_STORAGE
				APP_DEBUG("\r\nError : Ql_FS_Seek ret : %d\r\n",ret);
				//#endif
			}
			parameter_ID = (char*)Ql_MEM_Alloc(6000);
			Ql_memset(parameter_ID, 0x0,6000);
			ret = Ql_FS_Read(fileHandl,(u8*)parameter_ID,6000, &readedlen); // Read the payload from the file
			if(QL_RET_OK != ret)
			{
				//#ifdef FILE_STORAGE
				APP_DEBUG("\r\nError : Ql_FS_Read ret : %d\r\n",ret);
				//#endif
			}
			APP_DEBUG("\r\nreadedlen: %d\r\n",readedlen);
			parameter_length_ID = Ql_strlen(parameter_ID); // Find out the payload length
			//#ifdef DEBUG_MSG
			//APP_DEBUG("\r\n--> Length is %d Device ID is %s\r\n\n",parameter_length_ID,parameter_ID);
			//#endif

			for(i=0;i<=(parameter_length_ID);i++)
			{
				// if space or NULL found, assign NULL into newString[ctr]
				if(parameter_ID[i]==' ' && parameter_ID[i+1]=='-')
				{
					ctr++;  //for next word
					j=0;    //for next word, init index to 0
				}
				else if (ctr == 1)
				{

					new_CK0[j]=parameter_ID[i];
					j++;
				}
				else if (ctr == 2)
				{
					new_CC0[j]=parameter_ID[i];
					j++;
				}
				else if (ctr == 3)
				{
					new_CA0[j]=parameter_ID[i];
					j++;
				}
			}


			APP_DEBUG("\r\n--> Length is %d Device ID is %s\r\n\n",Ql_strlen(new_CK0),new_CK0);
			Ql_Sleep(100);
			ret = RIL_SSL_QSSLCERTIFICATE_DEL("CK0");	//First Delete file if available.
			ret= RIL_SSL_QSSLCERTIFICATE_WRITE("CK0",new_CK0,Ql_strlen(new_CK0),100); //Write Licence file in NVRAM
			RIL_SSL_QSSLCERTIFICATE_READ("CK0");


			APP_DEBUG("\r\n--> Length is %d Device ID is %s\r\n\n",Ql_strlen(new_CC0),new_CC0);
			Ql_Sleep(100);
			ret = RIL_SSL_QSSLCERTIFICATE_DEL("CC0");	//First Delete file if available.
			ret= RIL_SSL_QSSLCERTIFICATE_WRITE("CC0",new_CC0,Ql_strlen(new_CC0),100); //Write Licence file in NVRAM
			RIL_SSL_QSSLCERTIFICATE_READ("CC0");


			APP_DEBUG("\r\n--> Length is %d Device ID is %s\r\n\n",Ql_strlen(new_CA0),new_CA0);
			Ql_Sleep(100);
			ret = RIL_SSL_QSSLCERTIFICATE_DEL("CA0");	//First Delete file if available.
			ret= RIL_SSL_QSSLCERTIFICATE_WRITE("CA0",new_CA0,Ql_strlen(new_CA0),100); //Write Licence file in NVRAM
			RIL_SSL_QSSLCERTIFICATE_READ("CA0");

		}
		Ql_MEM_Free(parameter_ID);
		parameter_ID = NULL;
		fileSys_CloseFile((s32)fileHandl); //Close the file
	}
	return 1;
}

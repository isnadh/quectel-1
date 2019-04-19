/*HEADER****************************************************************
 ************************************************************************
 ***
 ***     Copyright (c) 2018 HARMAN CONNECTED SERVICES
 ***     All rights reserved
 ***
 ***    This software embodies materials and concepts which are
 ***    confidential to HARMAN CONNECTED SERVICES and is made available solely
 ***    pursuant to the terms of a written license agreement with HARMAN CONNECTED SERVICES
 ***
 ***    Designed and Developed by Harman Connected Services for Hindustan Unilever
 ***
 ***    File:    <file name.h>
 *** File:  <ipc.c>
 ***
 *** Comments: Implementation of Interprocessor communication
 *** Comments about the file.
 *** - Algorithm / logic explanation
 *** - formula explanation
 *** - inputs and outputs of this file
 *** - etc etc.
 ***
 ***
 ************************************************************************
 *END*******************************************************************/


#include "ipc.h"
#include "common_vars.h"
#include "ql_stdlib.h"
#include "fota_main.h"
#include "file_Operations.h"
#include "sms.h"

u8 configuration_Data_Write[3]; // Config Parameter ID , Data 0 of Parameter, Data 1 of Parameter
u8 configuration_Data_Read; // Config Parameter ID
u8 Special_Action_Timer =0;
static u16  CRC2 =0;
u8  CRC_Token =0;

#define PURIFICATION_START_EVENT          ((!(Status_Byte & PURIFICATION_OFF_STATUS_SET)) && (Previous_Status_Byte & PURIFICATION_OFF_STATUS_SET))
#define PURIFICATION_STOP_EVENT           ((Status_Byte & PURIFICATION_OFF_STATUS_SET) && (!(Previous_Status_Byte & PURIFICATION_OFF_STATUS_SET)))

#define MAX_IPC_RESPONSE_MISSING_COUNT  10
#define MAX_PACKET_SIZE                 16


extern u8 Control_Module_Indentity_data[MAX_PACKET_SIZE];
u8 Control_Module_Indentity_data[MAX_PACKET_SIZE] = {"\0"};
char deviceidentitydata[30] = {"\0"};
char control_module_identity_number[20] = {"\0"};
char Error_messages_array[MAX_ERROR_COUNT] = {0};



u16 Purification_ON_Counter = 0;
u16 Previous_Purification_ON_Counter = 0;
u16 Fault_Status_Byte = 0;
u16 PreviousFaultStatus_Data = 0;
u8 PreviousPurificationOffStatus_Data = 0;
u16 Sensor_Status_Byte = 0;
u16 PreviousSensor_Status_Byte = 0;
u8 AMC_Status_Byte = 0;
u8 Previous_AMC_Status_Byte = 0;
s32 InitIPComm(void);
static void IPC_TIMER_INIT(void);


void Callback_OnIPCTimer(u32 timerId, void* param);

// Different flags declared here.. most of them are to check the operation happen one time only

u32 IPC_CMD_REQ_source =0;
u32 PUBLISH_RESPONSE_REQ_source =0;
u16 Timer_On;
void StartIPCTimer(void);
void format_water_balance_buffer(unsigned int water_bal, char *c);
extern void Refresh_Cloud_Connectivity_Symbol(char cloud_connetcivity_status);

extern u32 rechargebalance;
u8 IPC_Sequence_Counter_GET_Special_parameters =0;
u16 IPC_Sequence_Counter =0;
u16 Publish_ping_Counter = 0;
u16 Heart_Beat_Message_Counter = 0;
u16 Power_Up_Sequencing_Counter = 4;
char Internal_error_code[4] = {0x43, 0x30, 0x30, '\0'}; // Default set to F0 which is invalid internal error code
u16 Con_Module_Reset_Counter =0;

u8 IPC_Response_missing_count = MAX_IPC_RESPONSE_MISSING_COUNT; // A zero in this count indicates that there are consecutive 10 IPC commands sent without getting any response.

char water_balance_buffer[7]={'\0','\0','\0','\0','\0','\0','\0'};
char water_balance_buffer_last[7]={'\0','\0','\0','\0','\0','\0','\0'};
char Water_dispensed_volume[7] = {'\0','\0','\0','\0','\0','\0','\0'};

char Purification_Start_local_time[25] = {'\0'};
char Purification_Stop_local_time[25] = {'\0'};
u32 Purification_ON_Event_Water_Balance = 0;
u32 Purification_OFF_Event_Water_Balance = 0;
u8 Status_Byte = 0;
u8 Previous_Status_Byte = 0;
int Water_Unit_index_get = 0;
u16 ro_pump_current_in_mA = 0;
u16 previous_ro_pump_current = 0;

u16 previous_TDS_Sensor_In_Temp_value = 0;
u16 TDS_Sensor_In_Temp_value = 0;

u16 previous_TDS_Sensor_Out_Temp_value = 0;
u16 TDS_Sensor_Out_Temp_value = 0;

u16 previous_TDS_Sensor_In_value = 0;
u16 TDS_Sensor_In_value = 0;

u16 previous_TDS_Sensor_OUT_value =0;
u16 TDS_Sensor_OUT_value = 0;

u16 amc_timer=0;
u16 previous_amc_timer=0;
u16 water_flow_rate_ml_minute = 0;
u16 previous_flowrate = 0;

//This magic byte transfrom must be identical to the one defined in Control Module FW
const u8 magic_byte[256] = {
		0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
		0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
		0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
		0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
		0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
		0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
		0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
		0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
		0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
		0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
		0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
		0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
		0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
		0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
		0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
		0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/*****************************************************************
 * Function: format_water_balance_buffer(unsigned int water_bal, char *c)
 *
 * Description: Converts the digital water balance data as printable format for LCD display
 *
 * Parameters: unsigned int water_bal, char *c
 *
 * Return: void
 *
 *****************************************************************/
void format_water_balance_buffer(unsigned int water_bal, char *c)
{
	char local_buff[7] = "       "; // Blank string
	unsigned char string_length, temp;
	//      Ql_itoa(water_bal, local_buff, 10);
	Ql_sprintf((char *)&local_buff,"%d",water_bal);

	string_length = strlen(local_buff);
	for(temp = 0; temp<string_length;temp++)
	{
		if(temp == (string_length-1))//last char in the string then insert a decimal point
		{
			if(string_length == 1)// for values less than 1 add a leading zero before decimal point
			{
				c[temp] = '0';
				c[temp+1] = '.';
				c[temp+2] = local_buff[temp];
				c[temp+3] = ' ';
				if(Water_Unit_index_get == 0)
					c[temp+4] = 'L';
				else if(Water_Unit_index_get == 1)
					c[temp+4] = 'G';
				else if(Water_Unit_index_get == 2)
					c[temp+4] = 'C';
				c[temp+5] = '\0';
			}
			else
			{	c[temp] = '.';
			c[temp+1] = local_buff[temp];
			c[temp+2] = ' ';
			if(Water_Unit_index_get == 0)
				c[temp+3] = 'L';
			else if(Water_Unit_index_get == 1)
				c[temp+3] = 'G';
			else if(Water_Unit_index_get == 2)
				c[temp+3] = 'C';
			c[temp+4] = '\0';
			}
			temp = string_length; //Just to go out of for loop
		}
		else
			c[temp] = local_buff[temp];
	}
}

/*****************************************************************
 * Function: Transform(u8 InByte)
 *
 * Description: Computes the CRC2 of the transmission packet
 *
 * Parameters: u8 InByte
 *
 * Return: void
 *
 *****************************************************************/
void Transform(u8 InByte)
{
	u8 Bit;
	InByte = magic_byte[InByte];
	for(Bit=0;Bit<8;Bit++) // Process for all bits in the InByte
	{
		if( BIT0(InByte) != BIT0(CRC2) ) // If new bit xor bit 0 of CRC does not match
		{
			if( !BIT1(CRC2) ) // If bit 1 of CRC is zero
				SetBIT1(CRC2);
			else
				ResetBIT1(CRC2);
			if( !BITE(CRC2) ) // If bit 14 of CRC is zero
				SetBITE(CRC2);
			else
				ResetBITE(CRC2);
			CRC2 = ( CRC2 / 2 );

			SetBITF(CRC2);//After the shift direction Bit0 the new bit 15 becomes 1
		}
		else
		{
			CRC2 = ( CRC2 / 2 );
			ResetBITF(CRC2);
		}
		InByte = InByte / 2;
	}
}




/*****************************************************************
 * Function: IPC_TIMER_INIT
 *
 * Description: Initialize Timer
 *
 * Parameters: void
 *
 * Return: void
 *****************************************************************/
static void IPC_TIMER_INIT(void)
{
	s32 ret;

	ret = Ql_Timer_Register(Timer_SendTimeOut, Callback_OnIPCTimer, NULL);
	APP_DEBUG("\r<-- success, Ql_Timer_Register : Timer_SendTimeOut, ret : %d -->\r\n",ret);
#ifdef INOUT
	if(ret <0)
	{
		APP_DEBUG("\r<-- Error, Ql_Timer_Register : Timer_SendTimeOut, ret : %d -->\r\n",ret);
	}
#endif
}


/*****************************************************************
 * Function: Callback_OnIPCTimer
 *
 * Description:
 *              This is a periodic timer fo rIPC communication.. All the actions are now scheduled with this interval
 * Parameters:
 *           void
 * Return:
 *           void
 *****************************************************************/
void Callback_OnIPCTimer(u32 timerId, void* param)
{
	// Increment differnet periodic counters
	IPC_Sequence_Counter++;

	//#ifdef IPC_DEBUG_MSG
	APP_DEBUG("\r\n<-- IPC Transmit Event : %d ",IPC_Sequence_Counter);
	//#endif
	// Below are periodic IPC communication schedules..
	if(timerId == Timer_SendTimeOut)
	{

		// Below are periodic events are scheduled..
		if(IPC_Sequence_Counter && ((IPC_Sequence_Counter % 60)== 0)) // Execute one minute sequences
		{
			//SMS_SendSMS_Text("+918796608727","13",0,"This is Quectel M66",20,NULL);
		}
		if(IPC_Response_missing_count)
			IPC_Response_missing_count--; // Decrement the IPC response counter
	}
	// End of else if(IPC_Response_missing_count)
	if(IPC_Sequence_Counter == 3600) // Reset the counter every 1 hour .. note that this IPC periodic loop is 1 Sec execute
		IPC_Sequence_Counter = 0;
}


/*****************************************************************
 * Function: InitIPComm
 *
 * Description: Inter process communication init
 *
 * Parameters: void
 *
 * Return:
 *           void
 *****************************************************************/
s32 InitIPComm(void)
{
	s32 ret = 0;

	//IPC_UART_INIT(IPC_BAUDRATE);
	IPC_TIMER_INIT();

	return ret;
}
/*****************************************************************
 * Function: Send_IPC_Command (int CmdID)
 *
 * Description: Send command to Control module
 *
 * Parameters: int
 *
 * Return: void
 *
 *****************************************************************/
void Send_IPC_Command(int CmdID)
{
	u16 buflen = 0;
	u8 loop_ = 0;
	u8 packet_checksum =0;
	u8 UART_IPC_Write_Buffer[MAX_PACKET_SIZE] = {0x00};

	UART_IPC_Write_Buffer[buflen++] = 0xFF; // Start byte is always 0xFF
	UART_IPC_Write_Buffer[buflen++] = 0x00; // Start with 0x00 in this.. will be update later once command bytes are fill in
	UART_IPC_Write_Buffer[buflen++] = CmdID;

	switch(CmdID)
	{
	case CMD_GET_IDENTITY:
		UART_IPC_Write_Buffer[buflen++] = CRC_Token;
		CRC_Token = magic_byte[CRC_Token];
		break;
	case CMD_SET_CONFIG:
		UART_IPC_Write_Buffer[buflen++] = configuration_Data_Write[CONFIG_PID];
		UART_IPC_Write_Buffer[buflen++] = configuration_Data_Write[CONFIG_DATA0];
		UART_IPC_Write_Buffer[buflen++] = configuration_Data_Write[CONFIG_DATA1];
		break;
	case CMD_GET_CONFIG:
		UART_IPC_Write_Buffer[buflen++] = configuration_Data_Read;
		break;
	}
	//Update the number of bytes data value
	UART_IPC_Write_Buffer[1] = buflen - 3;

	if(CmdID == CMD_SET_CONFIG) // For all config set commands the CRC2 checksum calculation is used.
	{
		CRC2 =water_balance;// Reset the CRC at the start of the frame with current water balance data
		Transform(CRC_Token);
		Transform(Control_Module_Indentity_data[0]);
		Transform(CmdID);
		Transform(Control_Module_Indentity_data[10]);
		Transform(configuration_Data_Write[CONFIG_PID]);
		Transform(Control_Module_Indentity_data[1]);
		Transform(UART_IPC_Write_Buffer[1]);
		Transform(Control_Module_Indentity_data[9]);
		Transform(configuration_Data_Write[CONFIG_DATA0]);
		Transform(Control_Module_Indentity_data[2]);
		Transform(configuration_Data_Write[CONFIG_DATA1]);
		Transform(Control_Module_Indentity_data[8]);
		Transform(Control_Module_Indentity_data[3]);
		Transform(Control_Module_Indentity_data[7]);
		Transform(Control_Module_Indentity_data[4]);
		Transform(Purification_ON_Counter & 0xFF);
		Transform(Control_Module_Indentity_data[6]);
		Transform((Purification_ON_Counter>>8) & 0xFF);
		Transform(Control_Module_Indentity_data[5]);

		packet_checksum = (CRC2 & 0xFF);
		packet_checksum ^=((CRC2>>8) & 0xFF);
	}
	else // Simple checksum
	{
		//compute and Update the checksum byte
		for(loop_ = 1 ; loop_ <= (2 + UART_IPC_Write_Buffer[1]);loop_++)
		{
			packet_checksum += UART_IPC_Write_Buffer[loop_];
		}
		packet_checksum = (u8)((0x100-packet_checksum)&0xFF);
	}
	UART_IPC_Write_Buffer[buflen++] = packet_checksum;

	Ql_UART_Write(UART_PORT_IPC, UART_IPC_Write_Buffer, buflen);
#ifdef IPC_DEBUG_MSG
	APP_DEBUG("\r<--UART Send Data:");
	for(loop_=0; loop_<buflen; loop_++)
	{
		APP_DEBUG("0x%2.2x ", UART_IPC_Write_Buffer[loop_]);
	}
	APP_DEBUG("-->\r\n");
#endif
}

/*****************************************************************
 * Function: READ_ACK
 *
 * Description: SEND_ECHO
 *
 * Parameters: void
 *
 * Return: UART READ BYTE
 *
 *****************************************************************/
u16 READ_ACK(void)
{
	s32 rdLen = -1;
	unsigned int count = 0;
	u8 UART_IPC_Read_Buffer = 0x00;

	while(rdLen <= 0 && count < 60000)
	{
		count++;
		rdLen = Ql_UART_Read(UART_PORT_IPC, &UART_IPC_Read_Buffer, 1);
	}
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("count:%d\r\n",count);
#endif
	if(rdLen <= 0)
		return count;
	else
	{
#ifdef DEBUG_BOOT_MSG
		APP_DEBUG("get Read Echo:%d:%x:\r\n",rdLen,UART_IPC_Read_Buffer);
#endif
		return (u16) UART_IPC_Read_Buffer;
	}
}

/*****************************************************************
 * Function: READ_BOOTLOADER_BTYE
 *
 * Description: READ BOOTLOADER BTYE
 *
 * Parameters: void
 *
 * Return: POINTER OF PARAMETER DATA
 *
 *****************************************************************/
/*
void READ_BOOTLOADER_BTYE(unsigned char BL_byte1,unsigned char BL_byte2)
{
	u8 command[5] = {0xFF, 0x01, 0x4E, 0x0B, 0x00};
	u8 parameter_data[4] = {0xFF, 0x03, 0x6E, 0x0B};
	u8 UART_IPC_Read_Buffer[7] = {0x00},i;
	u8 *p = &command[0];

	//command[4] = checksum_calculation(&command,4);

	u8 index = 0;
	u32 sum = 0;

	for(index = 1;index < 4; index++)
	{
		sum += command[index];
#ifdef DEBUG_BOOT_MSG
		APP_DEBUG("\r\n<-- SUM: and data %d 0x%2.2x-->\r\n",sum,command[index]);
#endif
	}

	//checksum is 2s complement of given sum
	command[4] = 256 - sum;
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("\r\n<-- TOTAL SUM: %d %d-->\r\n",sum,command[4]);
#endif

	//APP_DEBUG("READ_BOOTLOADER_BTYE:%x:\r\n",command[4]);

	do
	{
		Ql_UART_Write(UART_PORT_IPC, p, 5);
		for(i=0;i<7;)
		{
			UART_IPC_Read_Buffer[i] = READ_ACK();

			if(i>3)
			{
#ifdef DEBUG_BOOT_MSG
				APP_DEBUG("1UART_IPC_Read_Buffer[%x] = %x\r\n",i,UART_IPC_Read_Buffer[i]);
#endif
				i++;
			}
			else if(UART_IPC_Read_Buffer[i] == parameter_data[i])
			{
#ifdef DEBUG_BOOT_MSG
				APP_DEBUG("2UART_IPC_Read_Buffer[%x] = %x: %x\r\n",i,UART_IPC_Read_Buffer[i],parameter_data[i]);
#endif
				i++;
			}
		}
	}
	while(BL_byte1 != UART_IPC_Read_Buffer[4] && BL_byte2 != UART_IPC_Read_Buffer[5]);
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("true\r\n");
#endif
	//return UART_IPC_Read_Buffer;
}
 */

/*****************************************************************
 * Function: SET_RESET_BOOTLOADER_BTYE
 *
 * Description: SET RESET BOOTLOADER BTYE
 *
 * Parameters: Bootloader byte1, Bootloader byte2
 *
 * Return: void
 *
 *****************************************************************/
void SET_RESET_BOOTLOADER_BTYE(unsigned char set_reset)
{
	u8 command[5] = {0xFF, 0x01, 0x4E, 0x00, 0x00};
	u8 *p = &command[0];
	u8 index = 0;
	u32 sum = 0;

	command[3] = set_reset;

	for(index = 1;index < 4; index++)
	{
		sum += command[index];
#ifdef DEBUG_BOOT_MSG
		APP_DEBUG("\r\n<-- SUM: and data %d 0x%2.2x-->\r\n",sum,command[index]);
#endif
	}

	//checksum is 2s complement of given sum
	command[4] = 256 - sum;
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("\r\n<-- TOTAL SUM: %d %d-->\r\n",sum,command[4]);
#endif


#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("SET_RESET_BOOTLOADER_BTYE:%x:\r\n",command[4]);
#endif
	Ql_UART_Write(UART_PORT_IPC, p, 5);
	Ql_Sleep(200);          //wait to receive command on STM8

}

/*****************************************************************
 * Function: SYNCHR_COMMAND
 *
 * Description: SYNCHR_COMMAND
 *
 * Parameters: void
 *
 * Return: return Echo (ACK or NACK)
 *
 *****************************************************************/
u8 SYNCHR_COMMAND(void)
{
	u8 data = 0x7F;
	u8 *p = &data;
	u8 UART_IPC_Read_Buffer[2] = {0x00};
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("SYNCHR_COMMAND \r\n");
#endif
	Ql_UART_Write(UART_PORT_IPC, p, 1);
	Ql_Sleep(200);
	Ql_UART_Read(UART_PORT_IPC, UART_IPC_Read_Buffer, sizeof(UART_IPC_Read_Buffer));
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("Read Echo6:%x:%x\r\n",UART_IPC_Read_Buffer[0],UART_IPC_Read_Buffer[1]);
#endif
	if(UART_IPC_Read_Buffer[0] == 0x79 || UART_IPC_Read_Buffer[1] == 0x79)
		return 0x79;
	else
		return 0x7F;
}

/*****************************************************************
 * Function: SEND_ECHO
 *
 * Description: SEND_ECHO
 *
 * Parameters: Last received byte
 *
 * Return: void
 *
 *****************************************************************/
void SEND_ECHO(u8 command)
{
	u8 *p = &command;

	Ql_UART_Write(UART_PORT_IPC, p, 1);
}


/*****************************************************************
 * Function: GET_COMMAND
 *
 * Description: GET COMMAND
 *
 * Parameters: void
 *
 * Return: IPC ACK or NACK
 *
 *****************************************************************/
u8 GET_COMMAND(void)
{
	u8 command[2] = {0x00, 0xFF};
	u8 *p = &command[0];
	u8 UART_IPC_Read_Buffer = 0x00;
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("GET_COMMAND\r\n");
#endif
	Ql_UART_Write(UART_PORT_IPC, p, 2);
	UART_IPC_Read_Buffer = READ_ACK();

	if(UART_IPC_Read_Buffer == 0x79)
	{
		SEND_ECHO(UART_IPC_Read_Buffer);
		UART_IPC_Read_Buffer = READ_ACK();
		if(UART_IPC_Read_Buffer == 0x05)
		{
			SEND_ECHO(UART_IPC_Read_Buffer);
			UART_IPC_Read_Buffer = READ_ACK();
			if(UART_IPC_Read_Buffer == 0x13)
			{
				SEND_ECHO(UART_IPC_Read_Buffer);
				UART_IPC_Read_Buffer = READ_ACK();
				if(UART_IPC_Read_Buffer == 0x00)
				{
					SEND_ECHO(UART_IPC_Read_Buffer);
					UART_IPC_Read_Buffer = READ_ACK();
					if(UART_IPC_Read_Buffer == 0x11)
					{
						SEND_ECHO(UART_IPC_Read_Buffer);
						UART_IPC_Read_Buffer = READ_ACK();
						if(UART_IPC_Read_Buffer == 0x21)
						{
							SEND_ECHO(UART_IPC_Read_Buffer);
							UART_IPC_Read_Buffer = READ_ACK();
							if(UART_IPC_Read_Buffer == 0x31)
							{
								SEND_ECHO(UART_IPC_Read_Buffer);
								UART_IPC_Read_Buffer = READ_ACK();
								if(UART_IPC_Read_Buffer == 0x43)
								{
									SEND_ECHO(UART_IPC_Read_Buffer);
									UART_IPC_Read_Buffer = READ_ACK();
									if(UART_IPC_Read_Buffer == 0x79)
									{
										SEND_ECHO(UART_IPC_Read_Buffer);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	return UART_IPC_Read_Buffer;
}

/*****************************************************************
 * Function: WRITE_COMMAND
 *
 * Description: WRITE COMMAND
 *
 * Parameters: address, length of data and data array
 *
 * Return: IPC ACK or NACK
 *
 *****************************************************************/
u8 WRITE_COMMAND(unsigned int address,unsigned char length, char data[])
{
	u8 command[2] = {0x31, 0xCE};
	u8 a[5]={0x00},i,checksum=0;
	u8 *p = &command[0];
	u8 *q = &a[0];
	u8 *r = &length;
	u8 *s = (u8 *)&data[0];
	u8 *t = &checksum;

	u8 UART_IPC_Read_Buffer = 0x00;

	for(i=0;i<4;i++)
	{
		a[i] = (unsigned char) (address>>(24-(8*i)));
		a[4] = a[4] ^ a[i];             //XOR operation for checksum
	}
	for(i=0;i<length+1;i++)
		checksum = checksum ^ data[i];          //XOR operation for checksum

	checksum = checksum ^ length;                   //XOR operation for checksum
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("WRITE_COMMAND:%x:%x:%x:\r\n", address, checksum, data[0]);
#endif
	Ql_UART_Write(UART_PORT_IPC, p, 2);
	UART_IPC_Read_Buffer = READ_ACK();
	if(UART_IPC_Read_Buffer == 0x79)
	{
		SEND_ECHO(UART_IPC_Read_Buffer);
		Ql_UART_Write(UART_PORT_IPC, q, 5);
		UART_IPC_Read_Buffer = READ_ACK();
		if(UART_IPC_Read_Buffer == 0x79)
		{
			SEND_ECHO(UART_IPC_Read_Buffer);
			Ql_UART_Write(UART_PORT_IPC, r, 1);
			Ql_UART_Write(UART_PORT_IPC, s, length+1);
			Ql_UART_Write(UART_PORT_IPC, t, 1);
			UART_IPC_Read_Buffer = READ_ACK();
			if(UART_IPC_Read_Buffer == 0x79)
			{
				SEND_ECHO(UART_IPC_Read_Buffer);
			}
		}
	}

	return UART_IPC_Read_Buffer;
}

/*****************************************************************
 * Function: READ_COMMAND
 *
 * Description: READ COMMAND
 *
 * Parameters: address, length of data
 *
 * Return: void
 *
 *****************************************************************/
void READ_COMMAND(unsigned int address,unsigned char length)
{
	u8 command[2] = {0x11, 0xEE};
	unsigned char lenght_compliment;
	u8 a[5]={0x00},i;
	u8 *p = &command[0];
	u8 *q = &a[0];
	u8 *r = &length;
	u8 UART_IPC_Read_Buffer[0x2F] = {0x00};

	for(i=0;i<4;i++)
	{
		a[i] = (unsigned char) (address>>(24-(8*i)));
		a[4] = a[4] ^ a[i];             //XOR operation for checksum
	}

	lenght_compliment = 0xFF - length;
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("READ_COMMAND2:%x:%x:%x:%x:%x\r\n",a[4],a[3],a[2],a[1],a[0]);
#endif
	Ql_UART_Write(UART_PORT_IPC, p, 2);
	UART_IPC_Read_Buffer[0] = READ_ACK();
	if(UART_IPC_Read_Buffer[0] == 0x79)
	{
		SEND_ECHO(UART_IPC_Read_Buffer[0]);
		Ql_UART_Write(UART_PORT_IPC, q, 5);
		UART_IPC_Read_Buffer[0] = READ_ACK();
		if(UART_IPC_Read_Buffer[0] == 0x79)
		{
			SEND_ECHO(UART_IPC_Read_Buffer[0]);
			Ql_UART_Write(UART_PORT_IPC, r, 1);
			r = &lenght_compliment;
			Ql_UART_Write(UART_PORT_IPC, r, 1);
			UART_IPC_Read_Buffer[0] = READ_ACK();
			if(UART_IPC_Read_Buffer[0] == 0x79)
			{
				SEND_ECHO(UART_IPC_Read_Buffer[0]);
				for(i=0;i<length+1;i++)
				{
					UART_IPC_Read_Buffer[i] = READ_ACK();
					SEND_ECHO(UART_IPC_Read_Buffer[i]);
				}
			}
		}
	}
}
#if 0
/*****************************************************************
 * Function: FULL_ERASE_COMMAND
 *
 * Description: FULL ERASE COMMAND
 *
 * Parameters: void
 *
 * Return: void
 *
 *****************************************************************/
void FULL_ERASE_COMMAND(void)
{
	u8 command[2] = {0x43, 0xBC};
	u8 full_erase[2] = {0xFF, 0x00};
	u8 *p = &command[0];
	u8 *q = &full_erase[0];
	u8 UART_IPC_Read_Buffer = 0x00;

	Ql_UART_Write(UART_PORT_IPC, p, 2);
	UART_IPC_Read_Buffer = READ_ACK();
	if(UART_IPC_Read_Buffer == 0x79)
	{
		SEND_ECHO(UART_IPC_Read_Buffer);
		Ql_UART_Write(UART_PORT_IPC, q, 2);
		UART_IPC_Read_Buffer = READ_ACK();
		if(UART_IPC_Read_Buffer == 0x79)
		{
			SEND_ECHO(UART_IPC_Read_Buffer);
		}
	}
}
#endif

/*****************************************************************
 * Function: ERASE_SECTOR_COMMAND
 *
 * Description: ERASE SECTOR COMMAND
 *
 * Parameters: sec_num (total numbers of sector to be erase), start_num: start number of sector
 *
 * Return: IPC ACK or NACK
 *
 *****************************************************************/
u8 ERASE_SECTOR_COMMAND(unsigned char total_sec_to_erase, unsigned char start_num)
{
	u8 sec_num = total_sec_to_erase - 1;
	u8 command[2] = {0x43, 0xBC};
	u8 *p = &command[0];
	u8 sector_code[12] = {0x00} ,sector_checksum = 0;
	u8 i,UART_IPC_Read_Buffer = 0x00;
	u8 *q = &sector_code[0];

	sector_code[0] = sec_num;
	for(i=1;i<sec_num+2;i++)
	{
		sector_code[i] = i+start_num-1;
		sector_checksum = sector_checksum ^ sector_code[i];
	}
	sector_checksum = sector_checksum ^ sec_num;
	sector_code[sec_num+2] = sector_checksum;
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("ERASE_SECTOR_COMMAND:%x\r\n",sector_checksum);
#endif
	Ql_UART_Write(UART_PORT_IPC, p, 2);
	UART_IPC_Read_Buffer = READ_ACK();
	if(UART_IPC_Read_Buffer == 0x79)
	{
		SEND_ECHO(UART_IPC_Read_Buffer);
		Ql_UART_Write(UART_PORT_IPC, q, sec_num+3);
		UART_IPC_Read_Buffer = READ_ACK();
		if(UART_IPC_Read_Buffer == 0x79)
		{
			SEND_ECHO(UART_IPC_Read_Buffer);
		}
	}
	return UART_IPC_Read_Buffer;
}

/*****************************************************************
 * Function: Go_COMMAND
 *
 * Description: Go COMMAND
 *
 * Parameters: void
 *
 * Return: void
 *
 *****************************************************************/
void GO_COMMAND(unsigned int jump_address)
{
	u8 command[2] = {0x21, 0xDE};
	u8 *p = &command[0];
	u8 UART_IPC_Read_Buffer = 0x00;
	u8 a[5] = {0x00}, i;
	u8 *q = &a[0];

	for(i=0;i<4;i++)
	{
		a[i] = (unsigned char) (jump_address>>(24-(8*i)));
		a[4] = a[4] ^ a[i];             //XOR operation for checksum
	}
#ifdef DEBUG_BOOT_MSG
	APP_DEBUG("GO COMMAND:%x\r\n",a[4]);
#endif
	Ql_UART_Write(UART_PORT_IPC, p, 2);
	UART_IPC_Read_Buffer = READ_ACK();
	if(UART_IPC_Read_Buffer == 0x79)
	{
		SEND_ECHO(UART_IPC_Read_Buffer);
		Ql_UART_Write(UART_PORT_IPC, q, 5);
		UART_IPC_Read_Buffer = READ_ACK();
		if(UART_IPC_Read_Buffer == 0x79)
		{
			SEND_ECHO(UART_IPC_Read_Buffer);
		}
	}
}

/*****************************************************************
 * Function: READ_IDENTITY
 *
 * Description: READ IDENTITY
 *
 * Parameters: void
 *
 * Return: firmware upgrade required or not (true - yes, false - no)
 *
 *****************************************************************/
u8 READ_IDENTITY(void)
{
	u8 command[5] = {0xFF, 0x01, 0x49, 0x00, 0xb6}; //Hard coded Read identity command sequnce.
	u16 i;
	u8 *p = &command[0];
	u16 UART_IPC_Read_Buffer[17] = {0x00};
	u8 HW_version = 0xff;
	u8 FW_version0 = 0xff;
	u8 FW_version1 = 0xff;
	u8 IPC_version = 0xff;
	READ_ACK();
	Ql_UART_Write(UART_PORT_IPC, p, 5); // Send the Read Identity command over IPC UART
	Ql_Sleep(500);		//wait for response to arrive
	for(i=0;i<15;i++)
	{
		UART_IPC_Read_Buffer[i] = READ_ACK();
		if(UART_IPC_Read_Buffer[i] == 60000)
		{
			display_please_wait();
			return 3;
		}

	}
	Water_Unit_index_get = UART_IPC_Read_Buffer[12]; // This is the water measurement unit configuration code
	Water_Unit_index_set = Water_Unit_index_get;
	FLAG.CON_IDENTITY_RECEIVED = TRUE;
	// Extract the FW version of the new / updated code..from the control moudle FW copy available with Communication module
	// Scan through the entire FW copy and find a token of '<', '*', '!', '=' .. version info is right after it.
	for(i=0;i<MCU_FW_SIZE;i++)
	{
		if(updated_code[i] == '<')
		{
			if(updated_code[i+1] == '*')
			{
				if(updated_code[i+2] == '!')
				{
					if(updated_code[i+3] == '=')
					{
						HW_version =  updated_code[i+4];
						FW_version0 = updated_code[i+5];
						FW_version1 = updated_code[i+6];
						IPC_version = updated_code[i+7];
						break;
					}
				}
			}
		}
	}
#ifdef DEBUG_MSG
	APP_DEBUG("\r\n<-- Old FW version:%x:%x:%x:%x | New FW version:%x:%x:%x:%x:\r\n",UART_IPC_Read_Buffer[3],UART_IPC_Read_Buffer[4],UART_IPC_Read_Buffer[5],UART_IPC_Read_Buffer[6],HW_version,FW_version0,FW_version1,IPC_version);
#endif
	if((HW_version == UART_IPC_Read_Buffer[3]) && (FW_version0*10+FW_version1) != (UART_IPC_Read_Buffer[4]*10+UART_IPC_Read_Buffer[5]))
	{
		display_please_wait();
#ifdef DEBUG_MSG
		APP_DEBUG("<-- Upgrading the Firmware-->\r\n");
#endif

		return 1;
	}
	else
	{
		display_FW_Version_Info();
		FLAG.DIGNOSTIC_MODE = FALSE;
#ifdef DEBUG_MSG
		APP_DEBUG("<-- firmware upgrade is not required-->\r\n");
#endif
		return 2;
	}
}


/*****************************************************************
 * Function: FirmwareUpgrade
 *
 * Description: Function for Firmware Upgrade
 *
 * Parameters: void
 *
 * Return: void
 *
 *****************************************************************/
void FirmwareUpgrade(void)
{
	u32 i;
	u8 FW_Upgrade_Attempt;
	FLAG.SKIP_POWER_OFF_ALERT = TRUE;// To avoid false power off alert capture

	for(FW_Upgrade_Attempt =0; (FW_Upgrade_Attempt < 2) && (FLAG.CON_STARTED_SUCCESS == FALSE);FW_Upgrade_Attempt++)
	{
		Ql_GPIO_SetLevel(PINNAME_RI, PINLEVEL_LOW); // Set RESET pin LOW
		Ql_Sleep(100);
		Ql_GPIO_SetLevel(PINNAME_RI, PINLEVEL_HIGH); // Set RESET pin HIGH
		Ql_Sleep(500);

		//UART is already initialised..so start communicating ..

		if(SYNCHR_COMMAND()==0x79)
		{
			//send command to get device supported command list
			SEND_ECHO(0x79);
			if(GET_COMMAND() == 0x79)
			{
				if(GET_COMMAND() == 0x79)
				{
					//send command to erase code memory of controller.
					if(WRITE_COMMAND(0x00A0,0x7f,(char*)data_00A0) == 0x79)
					{
						if(WRITE_COMMAND(0x0120,0x7f,(char*)data_0120) == 0x79)
						{
							READ_COMMAND(0x01A0,0x2f);
							if(WRITE_COMMAND(0x01A0,0x2f,(char*)data_01A0) == 0x79)
							{
								READ_COMMAND(0x00A0,0x7f);
								READ_COMMAND(0x0120,0x7f);
								READ_COMMAND(0x01A0,0x2f);
								READ_COMMAND(0x0040,0x01);
								if(ERASE_SECTOR_COMMAND(10,0x00) == 0x79)
								{
									if(ERASE_SECTOR_COMMAND(10,0x0A)==0x79)
									{
										if(ERASE_SECTOR_COMMAND(10,0x14)==0x79)
										{
											if(ERASE_SECTOR_COMMAND(2,0x1E)==0x79)
											{
												//send command for code download
												if(WRITE_COMMAND(0x00A0,0x7f,(char*)data_00A0)==0x79)
												{
													if(WRITE_COMMAND(0x0120,0x7f,(char*)data_0120) == 0x79)
													{
														READ_COMMAND(0x01A0,0x2f);
														if(WRITE_COMMAND(0x01A0,0x2f,(char*)data_01A0) == 0x79)
														{
															READ_COMMAND(0x00A0,0x7f);
															READ_COMMAND(0x0120,0x7f);
															READ_COMMAND(0x01A0,0x2f);
															READ_COMMAND(0x8000,0x01);

															for(i=0;i<(MCU_FW_SIZE);i=i+128)
															{
																if(WRITE_COMMAND(0x8000+i,0x7f,(char*)&updated_code[i]) != 0x79)
																{
#ifdef IPC_DEBUG_MSG
																	APP_DEBUG("<-- Firmware upgrade Attempt[%d] Failed, -->\r\n", FW_Upgrade_Attempt);
#endif
																	break;
																}
																if(i==MCU_FW_SIZE-128) //This is the last page to be written
																{
#ifdef DEBUG_MSG
																	APP_DEBUG("<-- Firmware upgraded, Device restarting  -->\r\n");
#endif
																	//Go command to execute STM8S code
																	GO_COMMAND(0x8000);
																	Ql_Sleep(1000);
																	SET_RESET_BOOTLOADER_BTYE(PID_RESET_BOOTLOADER_BTYE);
																	//READ_BOOTLOADER_BTYE(0x00,0xFF);
																	FLAG.CON_STARTED_SUCCESS = TRUE;
																}
															}
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
#ifdef DEBUG_MSG
			APP_DEBUG("<-- Communication with CON Module Not established -->\r\n");
#endif
			// Display Internal Fault F5 on LCD
			Ql_sprintf((char *)&Internal_error_code,"F5");
			Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_STRING_INTERNAL_ERROR_CODE);
		}
		if(FLAG.CON_STARTED_SUCCESS == FALSE)
			SET_RESET_BOOTLOADER_BTYE(PID_SET_BOOTLOADER_BTYE);// Lets do it for next attempt of FW upgrade
	}
	if(FLAG.CON_STARTED_SUCCESS == FALSE)// Control module failed to upgrade through 5 attepts..
	{
		// Display Internal Fault F7 on LCD
		Ql_sprintf((char *)&Internal_error_code,"F7");
		Ql_OS_SendMessage(3, MSG_ID_GW_STATE,GW_STATE_LCD_DISPLAY,TFT_STATE_DRAW_STRING_INTERNAL_ERROR_CODE);
		IPC_CMD_REQ_source |= SCHEDULE_IPC_GET_IDENTITY_CMD; // Get identification commmand forever..
	}
	display_FW_Version_Info();
}

/*****************************************************************
 * Function: READ_WATER_BALANCE
 *
 * Description: READ WATER BALANCE
 *
 * Parameters: void
 *
 * Return: None
 *
 *****************************************************************/
s32 READ_WATER_BALANCE(void)
{
	u8 command[5] = {0xFF, 0x01, 0x4E, 0x09, 0xA8};//Hard coded Read identity command sequnce.
	u16 i=0;
	u8 *p = &command[0];
	u16 UART_IPC_Read_Buffer[7] = {0x00};

	do
	{
		UART_IPC_Read_Buffer[i] = READ_ACK();
	}while(UART_IPC_Read_Buffer[i] != 60000); //Flush the input buffer

	Ql_UART_Write(UART_PORT_IPC, p, 5); // Send the Read Identity command over IPC UART
	for(i=0;i<7;i++)
	{
		UART_IPC_Read_Buffer[i] = READ_ACK();
		if(UART_IPC_Read_Buffer[i] == 60000)
		{
			return -1;
		}
		if(i>2)
		{
			if((UART_IPC_Read_Buffer[2] == 'n') &&( UART_IPC_Read_Buffer[3] == 0x09))
			{
				if(i == 4)
					water_balance = (UART_IPC_Read_Buffer[4]);
				if(i == 5)
				{
					water_balance |= (UART_IPC_Read_Buffer[5] << 8);
					format_water_balance_buffer(water_balance, water_balance_buffer); // format the latest water balance to display
#ifdef IPC_DEBUG_MSG
					APP_DEBUG("\n\r<--  Water balance is %s -->\r\n",water_balance_buffer);
#endif
					if(water_balance >= 0 && water_balance <= MAX_ALLOWED_WATER_BALANCE)
					{
						Init_Water_Balance_Screen();
						FLAG.CON_STARTED_SUCCESS = TRUE; // Set the flag at this place
					}
				}
			}
		}
	}
	return 0;
}

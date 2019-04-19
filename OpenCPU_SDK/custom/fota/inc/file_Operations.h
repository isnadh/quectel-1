#ifndef __SDOPS_H__
#define __SDOPS_H__

//#include "common_vars.h"

#define OFFSET                 0
#define FILE_DATA_LENGTH       1024*7	//(3Kbyte)
#define CASCADE_WRITE          0x00
#define OVER_WRITE             0x01
#define OVER_WRITE_TO_SIZE     0x02

extern char *file_data;
extern char *File_FotaAppBin;
extern char *File_Licensepem;

s32  fileSys_SetFileSystem(bool format);
s32  fileSys_OpenOrCreateFile(u8 *file_name);
u8   *fileSys_ReadFile(s32 filehandle);
s32  fileSys_WriteFile(s32 filehandle, u8 *data_to_write, u8 write_type, u16 wtite_to_size);
s32  fileSys_checkFile(u8 *file_name);
s32  fileSys_GetFilePos(s32 filehandle);
void fileSys_CloseFile(s32 filehandle);
s32  fileSys_deleteFile(u8 *file_name);
s32 Store_Persistent_Data(void);
s32 Read_Persistent_Data(void);
s32 Store_IoT_Persistent_Data(void);
s32 Read_IoT_Persistent_Data(void);
s32 Write_Error_Messages(void);
s32 Read_Fault_Flag_File(void);
void Find_Newest_Payload_Number(void);

char filePath[100];
u32 writeedlen, readedlen ;

#endif

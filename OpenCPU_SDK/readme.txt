Firmware_test_REV1.0

bacnet protocol with 20 tags reading and publish.

Firmware_test_REV1.01

BACnet protocol with 20 tags reading and publish.
MODBUS protocol with 20 tags reading and publish.

1.01a : BACnet alert checking for low limit and for high limit
1.01b : optimised the actual size of packet using 
		CJson_minify(); function.
		changing the data type from float to double.
		compressing the actual size of variable by %0.2f for min max avg and current value.
	
	Effect of this the packet size reduced to half of previous packet size.(35k to 18700Bytes).
1.01c : Publish data ALARM NORMAL and ON_DEMAND aligned.
1.01d : Added the ACK's for DD and IOTHUB but not tested.
1.01e : writen a code to write the data on SDcard before going to Publish.
1.01f : checked the code for modbus scan and publish. 

NOTE  : required files on Sdcard are placed in same folder. 
KNOWN ISSUES : sometimes DHCP Offer is not Recieved(possible reason not enough space.
	 

Firmware_test_REV1.02      

Added original Device DignoStic Utility connetion code with IP : 52.166.205.181

1.02a : changed the eclipse IP to DD ip.
1.02b : changed the Certificate(DD_cert) at both Sides (Gateway and spy).

tested the connection with sending the messages and checking the end to end flow.

DATE_13May2017
Firmware_test_REV1.03

Added the code to check the Actual local error indication methods 

Firmware_test_REV1.03a :local error indication methods for Sim card,Sd card ,GPRS availibity.

Firmware_test_REV1.03b : SDcard Read ,Sdcard Write DignoStic codes.
 
 
DATE_14May2017
Firmware_test_REV1.04


Firmware_test_REV1.04a : Checking for LAT and LONG also for Fota .


DATE_15May2017
Firmware_test_REV1.05

Firmware_test_REV1.05a : Added code for LAT and LONG(current location) .
						 Issue facing while reading the cert so changed the 
						 //u8 strBuf[FILE_DATA_LENGTH] = {0};
						 u8 *strBuf = Ql_MEM_Alloc(FILE_DATA_LENGTH);
						 in Sd_read_write file.
						 but we are not freeing that memory so need to be freed.
						 
Firmware_test_REV1.05b : checked the FactCfg file location parameters 
						 verify both Current location and the previous location stored in the file
						 if(match)
							 nothing ...to do;
						 else{
							 check for ffile taglist.txt
							 if(Present)
								 Delete this file.
								so we can start the whole process of verification again.
								and download the taglist.txt again.
						 }
						 
Firmware_test_REV1.05c : changed the code for #define BMS_PRESENT to check from file UI_cfg
						 changed the code where we used the #define BMS_PRESENT  to 
						 if(BMS_PRESENT)
							 else
							 {
								 //BMS_NOTPRESENT;
							 };
							 
							tested only by checking the Firing query or not .
Firmware_test_REV1.05d : Added New Error Code BMS_PRESENT but not Accessing.
						 One Timer for checking 
						 if(BMS_PRESENT)
						 {
							 //started one timer.
							 before timer expires any data on uart is  not recievd)
								 then BMS_not Accessing;
								 Stopped the Other timers.
									publish and query sending.
							 
						 }else
						 {
							 no need to start the timer.
						 }
						 
						 
DATE_16May2017
Firmware_test_REV1.06

Firmware_test_REV1.06a : Added code for FOTA over Normal FTP (Quectel FTP server).
						Added the actual Message download that message from MQTT 
						
						DD_GW-FirmwareUpgrade/868325025842529

						{
						"MId":"12345678909012345",
						"EPN":"1234567890",
						"IMEI":"868325025842529",
						"TD":"1234567890",
						"FADDR":"124.74.41.170",
						"FPORT":"21",
						"FPATH":"/stanley/",
						"FUNAME":"max.tang",
						"FPASS":"quectel!~@",
						"FILENAME":"M66_Fotahttp_app.bin"
						}
						
						ftp://124.74.41.170/stanley/M66_Fotahttp_app.bin:21@max.tang:quectel!~@
						
						Issue : While starting the Connection
								pdpContext ID is not getting so connection Fails.
								so we used Ql_GPRS_GetPDPContextId(); to get pdpContext then set that and remove that 
								and retry the connection now the connection is establishing and .bin file is downloading
								auto restart done.
								
Firmware_test_REV1.06_IOTHUB_added:
					Added the IOTHUB Acknowledge types 3-9 not tested.
					
					
					
					
					
					
Firmware_test_REV1.16_IOTHUB_added:

			Separated the Binary for Only Modbus
            ADDED the UTC GMT timestamp 
			Added the Digital tags handled seperated.
			Added the changes related to the APN Fact_cfg.txt
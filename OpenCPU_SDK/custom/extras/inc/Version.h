/*
 * Version.h
 *
 *  Created on: Jun 26, 2017
 *      Author: SAI
 */

#ifndef VERSION_H_
#define VERSION_H_

#define FW_Version_Major_COMM     0x0
#define FW_Version_Minor_COMM     0x4
#define FW_Version_STRING     "0.4" //Modify this as per above FW versions.

//Communication HE model identity code
#define COM_2G_GPRS        0
#define COM_WiFi           1
#define COM_4G_LTE         2

#define HW_Version_COMM            COM_2G_GPRS

//Below constants should match with contorl module attached..
#define HW_Version_CON            0

#define Protocol_Version_COMM 4

#endif /* VERSION_H_ */

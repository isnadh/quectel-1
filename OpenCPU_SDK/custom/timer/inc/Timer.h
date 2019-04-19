/*
 * Timer.h
 *
 *  Created on: Apr 22, 2017
 *      Author: SAI
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <string.h>
#include <stdlib.h>
#include "ril.h"
#include "ril_util.h"
#include "ril_location.h"
#include "ril_telephony.h"
#include "ril_system.h"
#include "ril_http.h"
#include "ril_network.h"
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
#include "cJSON.h"
//#include "fs.h"
#include "debug.h"
#include "common_vars.h"

extern void Timer_register(void);

extern void timer_handler_publish(u32 timerId, void* param);
extern void timer_handler_send_req(u32 timerId, void* param);
extern void LEDIndicationTimerHandler(u32 timerId, void* param);
extern void Callback_OnTimer(u32 timerId, void* param); //modbus
extern void timer_handler_verification(u32 timerId, void* param);
extern void Callback_OnIPCTimer(u32 timerId, void* param);

//---------------------------------------------------------------------------------
extern u32 ID_WATCHDOG_FEED;
extern u32 INTERVAL_WATCHDOG_FEED;			// 10 min
//---------------------------------------------------------------------------------




/********************ethernet TIMER RElated Declarations****************/



#endif /* TIMER_H_ */

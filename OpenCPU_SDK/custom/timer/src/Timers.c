/*
 * Timers.c
 *
 */

#include "Timer.h"
#include "common_vars.h"
#include "SecureSocket.h"
#include "ipc.h"
u32 ID_WATCHDOG_FEED = (TIMER_ID_USER_START+90);
u32 INTERVAL_WATCHDOG_FEED = 1000 * 60 * 10;			// 10 min

u32 Timer_TimeOut  = (TIMER_ID_USER_START+111);

u32 Timer_Delay  = (TIMER_ID_USER_START+100);

u32 Timer_SendTimeOut_Interval = 1000; // mSec .. This is IPC periodic communication timer.
u32 Timer_SendTimeOut  = (TIMER_ID_USER_START+114);


void Timer_register(void);
void timer_handler_watchdogFeed(u32 timerId, void* param);
void Callback_OnTimer(u32 timerId, void* param);




/*****************************************************************
 * Function: Timer_register
 *
 * Description:
 *              Timer_register Register the Timers used
 * Parameters:
 *             s32 result,
 *             ST_LocInfo* loc_info structure containing Lat and Long
 * Return:
 *           void
 *****************************************************************/

void Timer_register(void)
{

	s32 ret;


	ret = Ql_Timer_Register(Timer_Delay, Callback_OnTimer, NULL);
	if(ret <0)
	{
#ifdef INOUT
			APP_DEBUG("\r<-- Error, Ql_Timer_Register : Timer_TimeOut, ret : %d -->\r\n",ret);
#endif
	}

//	ret = Ql_Timer_Register(ID_WATCHDOG_FEED, timer_handler_watchdogFeed, NULL);
//		if(ret <0)
//		{
//	#ifdef DEBUG_MSG
//			APP_DEBUG("\r<-- Error, Ql_Timer_Register : Watchdog, ret : %d -->\r\n",ret);
//	#endif
//
}


void timer_handler_watchdogFeed(u32 timerId, void* param)
{
	Ql_WTD_Feed(wtdid);
	APP_DEBUG("<-- Time to Feed Logic Watchdog -->\r\n");
}



/*****************************************************************
 * Function: Callback_OnTimer
 *
 * Description:
 *              Callback_OnTimer
 * Parameters:
 *             u32 Timer_id,
 *             void* param,
 * Return:
 *           void
 *****************************************************************/

void Callback_OnTimer(u32 timerId, void* param)
{
	if(timerId ==  Timer_Delay)
	{
		Ql_Timer_Stop(Timer_Delay);
#ifdef DEBUG_MSG
		APP_DEBUG("Timer Event : Timer_Delay\r\n");
#endif
	}


}

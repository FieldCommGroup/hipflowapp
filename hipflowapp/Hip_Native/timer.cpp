/*************************************************************************************************
 * Copyright 2019 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**********************************************************
 *
 * File Name:
 *   timer.c
 * File Description:
 *   Functions to set timers for various events used
 *   primarily by the state machine.
 *
 **********************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "delay.h"
//#include "hthreads.h"
#include "timer.h"

#define  TIMING_LOG      0

/************
 *  Globals
 ************/
/* Flag to keep track of the event that set the timer */
uint16_t eventFlag = 0;
uint8_t wdTimerFlag = TRUE;

/************************************
 *  Private variables for this file
 ************************************/
static itimerval_t inITimer, saveITimer;
static uint16_t lastEvent = 0;
static uint16_t saveEvent = 0;
static bool_t haltTimer = FALSE;

#if TIMING_LOG 
static double tlast = 0;
static FILE *fp = NULL;
static struct timeval tv;
static struct timezone tz;
#endif  /* TIMING_LOG = 1 */ 

/**********************************************
 *  Private function prototypes for this file
 **********************************************/
static void display_timer(itimerval_t *p_thisTimer);
static void flag_event(uint16_t event);
static void set_timeval(timeval_t *p_thisTimer, int32_t secTime,
		int32_t usecTime);

errVal_t halt_itimer(void)
{
	errVal_t errVal = NO_ERROR;
	uint32_t secTime, usecTime;
	char buf[80];

	/* Save old values and reset value of interval timer */
	wdTimerFlag = FALSE;
	haltTimer = TRUE;
	saveEvent = eventFlag;
	eventFlag = 0;
	secTime = 0;
	usecTime = 0;

	/* Set interval (repetition) and target values of ITimer */
	set_timeval(&(inITimer.it_interval), secTime, usecTime);
	set_timeval(&(inITimer.it_value), secTime, usecTime);

	sprintf(buf, "halt_itimer    new timer: secTime=%d  usecTime=%d", secTime,
			usecTime);
	timinglog(buf);

	if (setitimer(ITIMER_REAL, &inITimer, &saveITimer) == LINUX_ERROR)
	{
		fprintf(p_toolLogPtr, "Error %d in halt_itimer()\n", errno);
		errVal = ITIMER_ERROR;
	}
	else
	{
		dbgp_tmr("Timer Halted (event = 0x%.4X, Time left:\n", saveEvent);
		display_timer(&saveITimer);
	}

	sprintf(buf, "halt_itimer    saveITimer: secTime=%d  usecTime=%d",
			(int32_t) saveITimer.it_value.tv_sec,
			(int32_t) saveITimer.it_value.tv_usec);
	timinglog(buf);

	return (errVal);
}

errVal_t resume_itimer(void)
{
	errVal_t errVal = NO_ERROR;
	itimerval_t outITimer;
	char buf[80];

	/* Restore event flag */
	flag_event(saveEvent);
	haltTimer = FALSE;

	/* Set value of interval timer from saved old value */
	inITimer = saveITimer;

	dbgp_tmr("Resuming Timer (event = 0x%.4X, Time left:\n", saveEvent);
	display_timer(&inITimer);

	sprintf(buf, "resume_itimer    new timer: secTime=%d  usecTime=%d",
			(int32_t) inITimer.it_value.tv_sec,
			(int32_t) inITimer.it_value.tv_usec);
	timinglog(buf);

	if (setitimer(ITIMER_REAL, &inITimer, &outITimer) == LINUX_ERROR)
	{
		fprintf(p_toolLogPtr, "Error %d in store_itimer()\n", errno);
		errVal = ITIMER_ERROR;
	}

	sprintf(buf, "resume_itimer    saveITimer: secTime=%d  usecTime=%d",
			(int32_t) saveITimer.it_value.tv_sec,
			(int32_t) saveITimer.it_value.tv_usec);
	timinglog(buf);

	return (errVal);
}

errVal_t adjust_itimer(uint32_t uSecElapsed)
{
	uint32_t secTime, usecTime, savedUSec;

	/* If saved time is less than adjustment, then timer should
	 * trigger right away, otherwise adjust trigger for the
	 * time already lapsed
	 */
	savedUSec = saveITimer.it_value.tv_usec
			+ (saveITimer.it_value.tv_sec) * USEC_PER_SEC;

	dbgp_tmr("Old saved timer value = %u us\n", savedUSec);

	if (savedUSec > uSecElapsed)
	{
		/* Adjust Saved timer by the time already elapsed */
		savedUSec -= uSecElapsed;
		secTime = savedUSec / USEC_PER_SEC;
		usecTime = savedUSec % USEC_PER_SEC;
	}
	else
	{
		/* Set Saved timer to a small value for near-immediate trigger */
		secTime = 0;
		usecTime = 1000; /* trigger after 1 ms */
	}

	set_timeval(&(saveITimer.it_value), secTime, usecTime);
	dbgp_tmr("New saved timer: \n");
	display_timer(&saveITimer);
	dbgp_tmr("Now calling resume timer with new saved time..\n");

	return (resume_itimer());
}

errVal_t set_itimer(uint32_t uSec, uint16_t event)
{
	itimerval_t outITimer;
	errVal_t errVal = NO_ERROR;
	uint32_t secTime, usecTime;
	char buf[100];

	/* Set interval (repetition) value of ITimer to 0 */
	secTime = 0;
	usecTime = 0;
	set_timeval(&(inITimer.it_interval), secTime, usecTime);

	/* Set target value of ITimer as requested */
	secTime = uSec / USEC_PER_SEC;
	usecTime = uSec % USEC_PER_SEC;
	set_timeval(&(inITimer.it_value), secTime, usecTime);

	sprintf(buf, "set_itimer %d   secTime=%d  usecTime=%d", uSec, secTime,
			usecTime);
	timinglog(buf);

	flag_event(event);
	/* Set value of interval timer in real time */
	if (setitimer(ITIMER_REAL, &inITimer, &outITimer) == 0)
	{
		dbgp_tmr("Replaced old time: \n");
		display_timer(&outITimer);
		dbgp_tmr("...with new time: \n");
		display_timer(&inITimer);
	}
	else
	{
		fprintf(p_toolLogPtr, "Error %d in setitimer()\n", errno);
		errVal = ITIMER_ERROR;
	}

	return (errVal);
}

errVal_t set_wdTimer(void)
{
	dbgp_tmr("Watchdog Timer Started\n");
	wdTimerFlag = TRUE;

	return (set_itimer(WD_TIME, eventWDTimer));
}

void timinglog(const char *text)
{
	return;
#if (TIMING_LOG)

	if (fp == NULL)
	{
		fp = fopen("timing.log", "w");
	}

	if (fp)
	{
		gettimeofday(&tv, &tz);
		double t = tv.tv_sec%10000 + ((double)tv.tv_usec)/1000000.0;
		double deltat = t - tlast;
		fprintf(fp, "%14.6f\t%10.6f\t%-10s\n", t, deltat, text);
		tlast = t;
		fflush(fp);
	}

#endif /* (TIMING_LOG) */
}

void timinglogmsg(const char *text, uint8_t rspCount, BYTE *rspMsg)
{
#if (TIMING_LOG)

	uint8_t i;
	char buf[500], buf2[500];

	for (i = 0; i < rspCount; i++)
	{
		sprintf(buf+3*i, "%02x ", rspMsg[i]);
	}

	sprintf(buf2, "%s: %s", text, buf);
	timinglog(buf2);

#endif /* (TIMING_LOG) */
}

/****************************************************
 *          Private functions for this file
 ****************************************************/
static void display_timer(itimerval_t *p_thisTimer)
{
	dbgp_tmr(" Sec = %d, uSec = %d\n",
			(int32_t) p_thisTimer->it_value.tv_sec,
			(int32_t) p_thisTimer->it_value.tv_usec);
	return;
}

static void flag_event(uint16_t event)
{
	lastEvent = eventFlag;
	eventFlag = event;
	if (event)
	{
		/* Not a watchdog timer event */
		wdTimerFlag = FALSE;
	}
}

static void set_timeval(timeval_t *p_thisTimer, int32_t secTime,
		int32_t usecTime)
{
	p_thisTimer->tv_sec = secTime;
	p_thisTimer->tv_usec = usecTime;
}


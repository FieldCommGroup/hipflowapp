/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
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
 **********************************************************/

/**********************************************************
 *
 * File Name:
 *   timer.h
 * File Description:
 *   Header file for timer.c
 *
 **********************************************************/
#ifndef _TIMER_H
#define _TIMER_H

#include  <sys/time.h>
#include  "datatypes.h"
#include  "errval.h"


/*************
 *  Typedefs
 *************/
typedef struct timeval    timeval_t;
typedef struct itimerval  itimerval_t;
 

/****************
 *  Definitions 
 ****************/
/* Definitions of events used to trigger the timer */
#define eventACK      (uint16_t)(1 << 0)     /* 0x0001 */
#define eventCD       (uint16_t)(1 << 1)     /* 0x0002 */
#define eventERR      (uint16_t)(1 << 2)     /* 0x0004 */
#define eventETO      (uint16_t)(1 << 3)     /* 0x0008 */
#define eventGap      (uint16_t)(1 << 4)     /* 0x0010 */
#define eventGTO      (uint16_t)(1 << 5)     /* 0x0020 */
#define eventHold     (uint16_t)(1 << 6)     /* 0x0040 */
#define eventInitSM   (uint16_t)(1 << 7)     /* 0x0080 */
#define eventMsgErr   (uint16_t)(1 << 8)     /* 0x0100 */
#define eventOACK     (uint16_t)(1 << 9)     /* 0x0200 */
#define eventOBACK    (uint16_t)(1 << 10)    /* 0x0400 */
#define eventRT1      (uint16_t)(1 << 11)    /* 0x0800 */
#define eventRxDone   (uint16_t)(1 << 12)    /* 0x1000 */
#define eventTxDone   (uint16_t)(1 << 13)    /* 0x2000 */
#define eventUnused1  (uint16_t)(1 << 14)    /* 0x4000 */
#define eventUnused2  (uint16_t)(1 << 15)    /* 0x8000 */
#define eventWDTimer  (uint16_t)(0)          /* 0x0000 */

#define WD_TIME        (2 * USEC_PER_SEC)  /* watchdog timer */


/************
 *  Globals 
 ************/
extern uint16_t  eventFlag;
extern uint8_t   wdTimerFlag;


/************************
 *  Function Prototypes
 ************************/
errVal_t  adjust_itimer(uint32_t uSecElapsed);
errVal_t  halt_itimer(void);
errVal_t  resume_itimer(void);
errVal_t  set_itimer(uint32_t uSec, uint16_t event);
errVal_t  set_wdTimer(void);
void      timinglog(const char *text);
void      timinglogmsg(const char *text, uint8_t rspCount, BYTE *rspMsg);


#endif /* _TIMER_H */


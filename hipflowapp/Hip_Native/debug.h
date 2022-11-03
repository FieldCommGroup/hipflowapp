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
 *****************************************************************/

/**********************************************************
 *
 * File Name:
 *   debug.h
 * File Description:
 *   Header file to define the values of various constants
 *   used to enable/disable print statements in specific
 *   code areas.
 *
 **********************************************************/
#ifndef _DEBUG_H
#define _DEBUG_H


#include <stdio.h>
#include "common.h"     /* for p_toolLogPtr */

#include "toolutils.h"     /* for p_toolLogPtr */

/* Direct all stdout to stderr for unbuffered printing */
#define printf(format, a...)     fprintf(stderr, format, ## a)

//#define DEBUG_MODE

/****************
 *  Definitions
 ****************/
/* Set values of the following to 1 to turn on debug printing
 * for the desired code areas.
 */
#define DEBUG_ACCPT   0     /* Access Pt port debug */
#define DEBUG_APP     0     /* application debug */
#define DEBUG_AUTH    0     /* authentication debug */
#define DEBUG_BLTNS   0     /* built-ins debug */
#define DEBUG_BURST   0     /* burst functionality debug */
#define DEBUG_DVLP    0     /* debug during development */
#define DEBUG_GEN     0     /* general debug */
#define DEBUG_GWDUT   0     /* GW-DUT communication debug */
#define DEBUG_HLL     0     /* HART link layer debug */
#define DEBUG_HLS     0     /* HART state machine debug */
#define DEBUG_INIT    0     /* Initial debug (logged on screen) */
#define DEBUG_INTFC   0     /* Interface debug */
#define DEBUG_NMGW    0     /* NM/GW debug (logged on screen) */
#define DEBUG_NOOP    0     /* Replace tested dbgp_* w/ dbgp_noop */
#define DEBUG_RCV     0     /* RCV state machine debug */
#define DEBUG_SEM     0     /* semaphore debug */
#define DEBUG_SIG     0     /* signal debug */
#define DEBUG_THR     0     /* thread debug */
#define DEBUG_TMR     0     /* timer debug */

#define DEBUG_TRC     0     /* trace calls */
#define DEBUG_REQRSP    0     /* transmission debug */
#define DEBUG_TXRX    0     /* transmission debug */
#define DEBUG_UI      0     /* running user-interface/command-line mode */
#define DEBUG_WD      0     /* watchdog timer debug */
#define DEBUG_WIAPPL  0     /* wireless application debug */
#define DEBUG_WICB    0     /* circular buffer debug */
#define DEBUG_WIGEN   0     /* general debug */
#define DEBUG_WILIB   0     /* lib message funcs debug */
#define DEBUG_WINWK   0     /* Nwk functions debug */
#define DEBUG_WISEM   0     /* semaphore debug */
#define DEBUG_WITHR   0     /* Wi-thread debug */
#define DEBUG_WIREQRSP  0     /* transmission debug */

#if (DEBUG_APP)
 #if 1  /* To print to screen */
  #define dbgp_app(format, a...)     print_to_both(p_toolLogPtr, format, ## a)
 #else  /* To print to log */
  #define dbgp_app(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
 #endif
#else
#define dbgp_app(format, a...)  
#endif

#if (DEBUG_AUTH)
#define dbgp_auth(format, a...)    print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_auth(format, a...)  
#endif

#if (DEBUG_BLTNS)
#define dbgp_bltns(format, a...)   print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_bltns(format, a...)  
#endif

#if (DEBUG_DVLP)
#define dbgp_dvlp(format, a...)    print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_dvlp(format, a...)  
#endif

#if (DEBUG_BURST)
#define dbgp_burst(format, a...)   print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_burst(format, a...)
#endif


#if (DEBUG_GEN)
#define dbgprint(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgprint(format, a...)  
#endif

#if (DEBUG_HLL)
#define dbgp_hll(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_hll(format, a...)  
#endif

#if (DEBUG_HLS)
#define dbgp_hls(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_hls(format, a...)  
#endif

#if (DEBUG_INIT)
#define dbgp_init(format, a...)    print_to_both(p_toolLogPtr, format, ## a)
#else
#define dbgp_init(format, a...)
#endif

#if (DEBUG_INTFC)
#define dbgp_intfc(format, a...)    print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_intfc(format, a...)
#endif

#if (DEBUG_RCV)
#define dbgp_rcv(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_rcv(format, a...)  
#endif

#if (DEBUG_SEM)
#define dbgp_sem(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_sem(format, a...)  
#endif

#if (DEBUG_SIG)
#define dbgp_sig(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_sig(format, a...)  
#endif

#if (DEBUG_THR)
#define dbgp_thr(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_thr(format, a...)  
#endif

#if (DEBUG_TMR)
#define dbgp_tmr(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_tmr(format, a...)  
#endif

#if (DEBUG_TRC)
#define dbgp_trc(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#define dbgp_trace(format, a...)     print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_trc(format, a...)  
#define dbgp_trace(format, a...)
#endif

#if (DEBUG_REQRSP)
#define dbgp_txrx(format, a...)    print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_txrx(format, a...)
#endif

#if (DEBUG_TXRX)
#define dbgp_txrx(format, a...)    print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_txrx(format, a...)  
#endif

#if (DEBUG_UI)
#define dbgp_ui(format, a...)      print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_ui(format, a...)  
#endif

#if (DEBUG_WD)
#define dbgp_wd(format, a...)      print_to_log(p_toolLogPtr, format, ## a)
#else
#define dbgp_wd(format, a...)  
#endif

/* Always print this information to screen and log file. */
#define dbgp_log(format, a...)     print_to_both(p_toolLogPtr, format, ## a)

#ifdef DEBUG_MODE
#define dbgp_logdbg(format, a...)    print_to_both(p_toolLogPtr, format, ## a)
#else
#define dbgp_logdbg(format, a...)    (p_toolLogPtr && fprintf(p_toolLogPtr, format, ## a))
#endif


#endif /* _DEBUG_H */


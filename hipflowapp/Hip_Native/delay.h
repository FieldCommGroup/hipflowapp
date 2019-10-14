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
 *
 *****************************************************************/

/**********************************************************
 * Copyright (C) 2011, FieldComm Group, Inc.
 **********************************************************/

/**********************************************************
 *
 * File Name:
 *   delay.h
 * File Description:
 *   Header file to define the duration of various time 
 *   delays used in the state machine and other operations.
 *
 **********************************************************/
#ifndef _DELAY_H
#define _DELAY_H


#include "common.h"


/****************
 *  Definitions
 ****************/
#define FSK            12       /* arbit value (from bitrate) */
#define PSK            96       /* arbit value (from bitrate) */

#define FSK_BPCHAR     11       /* FSK: 11 bits per character */
#define FSK_BPS        1200u    /* FSK: bit rate */
#define PSK_BPCHAR     9        /* PSK: 9 bits per character */
#define PSK_BPS        9600u    /* PSK: bit rate */

#define PHYS_LAYER     FSK      /* choose either FSK or PSK */

#if (PHYS_LAYER == FSK)

/* Time definitions for FSK (in microseconds) */
 #define CHAR_TIME     ((FSK_BPCHAR * USEC_PER_SEC) / FSK_BPS) /* 9167us */
 #define HOLD_TIME     20000u    
 #define STO_TIME      256000u  

 #define BRST_TIME     (CHAR_TIME * 8)
 #define RT2_TIME      ((CHAR_TIME * 6) + HOLD_TIME)
 #define RT1P_TIME     ((CHAR_TIME * 5) + STO_TIME)
 #define RT1S_TIME     (RT1P_TIME + RT2_TIME)

#elif (PHYS_LAYER == PSK)

 /* Time definitions for C8PSK (in microseconds) */
 #define CHAR_TIME     ((PSK_BPCHAR * USEC_PER_SEC) / PSK_BPS) /* 937us */
 #define HOLD_TIME     3000u    
 #define STO_TIME      32000u  

 #define MISC_TIME     11000u /* Teflg+Tcon+Tcdon+Tsaf (Spec-81) */ 
 #define RT2_TIME      (MISC_TIME + HOLD_TIME)
 #define RT1P_TIME     (MISC_TIME+  STO_TIME)
 #define RT1S_TIME     (RT1P_TIME + RT2_TIME)

#else
 #error time definitions for physical layer needed!!!!
#endif /* PHYS_LAYER type */


#endif /* _DELAY_H */


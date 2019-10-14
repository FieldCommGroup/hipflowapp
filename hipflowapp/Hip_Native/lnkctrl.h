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
 *   lnkctrl.h
 * File Description:
 *   Common definitions and prototypes required by the 
 *   Master Data Link Layer. Most of the definitions and 
 *   prototypes in this file are for internal use by the 
 *   hart link layer processes.
 *
 **********************************************************/
#ifndef _LNKCTRL_H
#define _LNKCTRL_H

#include "common.h"
#include "errval.h"
#include "tpdll.h"

/**********************************************************
 * Definitions
 **********************************************************/
/*
 * States for queue entries.
 */
#define QSTATE_EMPTY          0
#define QSTATE_WAITING        1
#define QSTATE_WORKING        2
#define QSTATE_DONE           3
#define QSTATE_DEAD           4

/*
 * Events that cause a trip through the HART link layer state machine.
 * The values are chosen to accomodate a bitmask for queued events.
 * The lower numbers are higher priority.
 */
#define EVENT_RCVD_MSG        1    /* received a message to handle */
#define EVENT_XMIT_DONE       2    /* finished sending a message */
#define EVENT_TIMEOUT         4    /* a timer expired */
#define EVENT_XMIT_PENDING    8    /* message to send has been placed in queue */

/*
 * Communication errors.
 */
#define PARITY_ERROR       3
#define FRAMING_ERROR      4
#define OVERRUN_ERROR      5
#define CHECKSUM_ERROR     6
#define NORESPONSE_ERROR   7
#define SOM_ERROR          8

#if 0
/*
 * Default and limits for preambles.
 */
#define HART_DEFAULT_PREAMBLES     15
#define HART_MIN_PREAMBLES          5
#define HART_MAX_PREAMBLES         40
#endif

/*
 * Max. msg len 327: 40 preamble bytes + 267 frame bytes.
 * Max. frame len 267: 5 addr. + 3 exp. + 255 data bytes +
 * one each of delimiter + command + data byte count +
 * check byte)
 */
#define HART_MAX_ADDRBYTES    5
#define HART_MAX_DATABYTES    255
#define HART_MAX_EXPBYTES     0   /* currently not used */
#define HART_MAX_FRAME_LEN    (HART_MAX_ADDRBYTES  +  \
                               HART_MAX_DATABYTES  +  \
                               HART_MAX_EXPBYTES   +  4)

#define HART_MAX_MSG_LEN      (HART_MAX_PREAMBLES  +  \
                               HART_MAX_FRAME_LEN)



/*
 * Millisecond values for timers.
 */
#if 0  /* orig SWRI */

#define RT1PRI      305
#define RT1SEC      380
#define RT2          75
#define RT3         (RT1SEC - RT1PRI)
#define THOLD        20
#define TGAP         20

#else   /* Microsecond values for timers */

#define RT1PRI      RT1P_TIME
#define RT1SEC      RT1S_TIME
#define RT2         RT2_TIME 
#define RT3         (RT1SEC - RT1PRI)
#define THOLD       HOLD_TIME
#define TGAP        HOLD_TIME

#endif

/*
 * Bit patterns for delimiters.
 */
#define FRAME_BURST      0x01
#define FRAME_STX        0x02
#define FRAME_ACK        0x06
#define FRAME_EXPANSION  0x60

/***************************************************************************
 * Data Structures
 ***************************************************************************/

/*
 * This data structure is used to collect statistics on the link layer
 * activity.  It can be accessed by hll_ calls defined in hartmdll.h.
 * It is defined here since it must be included in the hart state machine
 * control structure.
 */
typedef struct link_statistics
{
     WORD  wSent;    /* Messages sent by this master  */
     WORD  wPSTX;    /* Messages rcvd from Primary master  */
     WORD  wPACK;    /* Slave acks to Primary master  */
     WORD  wPBACK;   /* Burst acks to Primary master  */
     WORD  wSSTX;    /* Messages rcvd from Secondary master */
     WORD  wSACK;    /* Slave acks to Secondary master */
     WORD  wSBACK;   /* Burst acks to Secondary master */
     WORD  wCommErr; /* framing, parity, or overrun*/
     WORD  wChkErr;  /* msgs with checksum errors  */
     WORD  wSomErr;  /* Invalid Start of Message */
     WORD  wNoResp;  /* Messages from this master with no response */
     WORD  wGapErr;  /* Number of gap timeouts */
     DWORD dwTotal;  /* total messages tallied */

} LINKSTATS, *PLINKSTATS;

/*
 * Queue entry used to track multiple message requests.
 */
typedef struct commq
{
     BYTE      cQState;
     BYTE      cMsgId;
     BYTE      acMessage[ HART_MAX_MSG_LEN ];
     WORD      wMsgLength;
     BYTE      acResponse[ HART_MAX_MSG_LEN ];
     WORD      wRspCount;
     BYTE      cRspPreams;
     BYTE      cRspQuality;   /* STATUS_OK, or one of the comm errors */
} COMMQ, *PCOMMQ;


/*
 * This structure is used to track the operation of the receive state machine.
 */
typedef struct rcv_ctrl
{
     BYTE      cState;        /* current state                         */
     BOOL      bGapActive;    /* True if Gap timer is running          */
     BYTE      cTemp;         /* counter for header and data locations */
     BYTE      cCheckSum;     /* checksum accumulator                  */
     WORD      wRspCount;     /* count of received characters          */
     BYTE      cRspPreams;    /* count of received preambles           */
     BYTE      cRspQuality;   /* STATUS_OK, or one of the comm errors  */
     BYTE      acResponse[ HART_MAX_MSG_LEN ];  /* recieve buffer      */
} RCV_CTRL, *PRCV_CTRL;

/*
 * The Generic HART link layer processor call functions to perform tasks
 * that are specific to the link that it is servicing.  These include:
 * 1.  deterimining if a message is ready to send
 * 2.  sending a message
 * 3.  setting timer values
 * 4.  checking carrier detect status
 */
typedef FSTAT (*PGETQFUNC)( PCOMMQ * );
typedef void (*PSENDFUNC)( BYTE, PBYTE, WORD );
typedef errVal_t (*PTMRFUNC)( uint32_t, uint16_t );
typedef BYTE (*PCDFUNC)( void );

/*
 * This structure is used to track the operation of the HART state machine.
 */
typedef struct hart_ctrl
{
     /*
      * These values control the operation of the state machine and must
      * be supplied by the caller.
      */
     BYTE      cPreambles;    /* # preambles to use                   */
     BOOL      bPrimary;      /* TRUE if a PRIMARY master setting     */
     BYTE      cRetries;      /* # times message can be retried       */
     uint32_t  wRT1;          /* current value of RT1 based on master */
     uint32_t  wExtend;       /* Timeout extension for slow response slaves */
     PGETQFUNC pfGetQ;        /* ptr to function to get next queue entry */
     PSENDFUNC pfSendMsg;
     PTMRFUNC  pfSetTimer;
     PCDFUNC   pfReadCD;

     /*
      * These are the working variables for the state machine.
      */
     BYTE      cState;        /* current state                        */
     BOOL      bBurst;        /* TRUE if burst msgs are being rcvd.   */
     PCOMMQ    pCommq;        /* ptr to comm q being processed.       */
     BYTE      cRetriesLeft;
     LINKSTATS LinkStats;
     BYTE      acBurstBuf[ HART_MAX_MSG_LEN ];
     BYTE      cBurstLen;
     BYTE      cMsgOngoing;
     RCV_CTRL  rc;            /* area controlling char rcv.           */

} HART_CTRL, *PHART_CTRL;


/***************************************************************************
 * Global extern variables.
 ***************************************************************************/
/*
 * global HART control
 */
extern HART_CTRL    ghc;


/***************************************************************************
 * Function Prototypes
 ***************************************************************************/
/*
 * These are interface to the receive character state machine.
 * They are not for general use.
 */
FSTAT rcv_state_machine( PRCV_CTRL psc, BYTE cData, BYTE cStatus );
void  rcv_init_state_machine( PRCV_CTRL psc );

/*
 * These are interface functions to the HART link layer state machine.
 * They are not for general use.
 */
void hls_process_received_char( PHART_CTRL phc, BYTE cData, uint8_t * p_msgDone );
void hls_state_machine( PHART_CTRL phc, BYTE cEvent );
void hls_init_state_machine( PHART_CTRL phc );

#endif /* _LNKCTRL_H */


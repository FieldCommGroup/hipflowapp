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

#ifndef _MSG_H
#define _MSG_H

// #include "comm.h"

/* defining the status word to be carried along with a message
 */
typedef enum { SUCCESS=0, 
    NO_MSG,            /* no message i.e. never saw the SOM */
    ADDR_ERR,        /* error receiving the address */
    EXP_ERR,        /* error while getting the expansion bytes */
    BC_ERR,            /* error receiving the byte count */
    PE, FE, OE,        /* UART error (command or data fields only */
    BCCE,            /* bad BCC */
    GAP_ERR            /* this one is left as an exercise for the student */ 
} MSG_STAT;

/* used to index the virtual function pointers for accessing header data */
enum {GETCMD, GETBC, PUTCMD, PUTBC, EXPAND};


/******************************************************************************
 * there can be several different headers that come with a HART message
 * we will define 8 different images using structures to map these combinations
 *
 * start with long frame messages with differing expansion bytes
 */

/* the definition of the delimiter byte
 */
#define LONG_FRAME  0x80
#define EXPANDED    0x60
#define FRAME_TYPE  0x1F

#define PSK_STX     0x0A
#define PSK_ACK     0x0E
#define PSK_BACK    0x09

#define STX         0x02
#define ACK         0x06
#define BACK        0x01

/* definition of first address byte
 */

#define MSTR        0x80
#define BURSTING    0x40
#define MFR_ADDR    0x3F
#define POLLING_ADDR 0x3F

typedef struct pMsg{
    MSG_STAT status;    /* keep track of the message we are receiving */

	unsigned commandNum;
	// mfg is (addr1 & 0x3f)
    char preambles,     /* counts the number of preambles */

        del,            /* the delimiter byte */

        addr1,          /* mstr, bursting, (mfr_addr || poll_addr) */
        devType,        /* manufacturer assigned devicec type */
        devID[3],        /* device ID, unique for every unit shipped of this device type */

        expand[3],      /* potential expansion bytes */

        command,        /* command number */
        byteCount,      /* byte count */

        respCode,
        devStat,

        data[240],      /* the data portion of the message */
        bcc;            /* the block check character, i.e. the check byte */

}MSG;

/* message buffer related functions
 */
 typedef enum { FSK = 0x01, PSK } PHL; // originally in comm.h

extern void initBufs(void);
extern MSG *newBuf (void);
extern char deleteBuf (MSG *pMsg);
extern MSG* recv_msg(void);
extern char xmit_msg(MSG *pMsg, PHL signal);


#endif






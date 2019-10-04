/*****************************************************************
 * Copyright (C) 2009-2015 FieldComm Group.
 *
 * All Rights Reserved.
 * This software is CONFIDENTIAL and PROPRIETARY INFORMATION of
 * FieldComm Group, Austin, Texas USA, and may not
 * be used either directly or by reference without permission of
 * FieldComm Group.
 *
 * THIS SOFTWARE FILE AND ITS CONTENTS ARE PROVIDED AS IS WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
 * WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 * A PARTICULAR PURPOSE AND BEING FREE OF DEFECT.
 *
 *****************************************************************/

/**********************************************************
 * Copyright (C) 2011, FieldComm Group, Inc.
 **********************************************************/

/**********************************************************
 *
 * File Name:
 *   htesttypes.h
 * File Description:
 *   Header file to define the various constants and
 *   typedefs used in HTest.
 *
 **********************************************************/
#ifndef _HTESTTYPES_H
#define _HTESTTYPES_H

#include  "datatypes.h"
#include  "hartmdll.h"


/*************
 *  Typedefs
 *************/
typedef union htest_frame_union 
{
  struct
  {
    uint8_t  delimByte;
    uint8_t  addrByte;
//  uint8_t  expBytes[HART_MAX_EXPBYTES]; /* Currently not used */
    uint8_t  cmdByte;
    uint8_t  byteCount;
    uint8_t  dataBytes[HART_MAX_DATABYTES];
    uint8_t  checkByte;   /* Offset/addr of this field varies with
                             value of byteCount! Access with care!! */
  } frameFieldShort;
  struct
  {
    uint8_t  delimByte;
    uint8_t  addrBytes[HART_MAX_ADDRBYTES];
//  uint8_t  expBytes[HART_MAX_EXPBYTES]; /* Currently not used */
    uint8_t  cmdByte;
    uint8_t  byteCount;
    uint8_t  dataBytes[HART_MAX_DATABYTES];
    uint8_t  checkByte;   /* Offset/addr of this field varies with
                             value of byteCount! Access with care!! */
  } frameFieldExt;
  uint8_t  frameBuff[HART_MAX_FRAME_LEN];
} htest_frame_t;


typedef struct htest_msg_struct 
{
  uint8_t        numPreams;
//  uint8_t        numExpBytes;    /* Currently not used */
  uint16_t       frameLen;
  htest_frame_t  msgFrame;
} htest_msg_t;
 

typedef struct htest_queue_struct 
{
  uint8_t      qFlags;  /* misc. status bits (see below) */
  DEVDATA      info;
  htest_msg_t  rcvMsg;
  htest_msg_t  sndMsg;
} htest_queue_t;
 


/****************
 *  Definitions
 ****************/
/* Quees for HART field devices */
#define MAX_HFDEV_QUEUES    10

/* Plus a dedicated queue for Bus Analyzer */
#define MAX_HTEST_QUEUES    (MAX_HFDEV_QUEUES + 1) 


/* Definitions of qFlags bits to track/evaluate actions
 *
 *   .---.---.---.---.---.---.---.---.
 *   | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *    ---'---'---'---'---'---'---'---'
 * 
 *    0 - QDEV_IDENTIFIED   1 - QERR_BUILT     
 *    2 - QMSG_BUILT        3 - QMSG_RECEIVED    
 *    4 - QMSG_SENT         5 - QMSG_FOR_BA 
 *    6 - USE_POLL_ADDR     7 - USE_SHORT_FRAME       
 */
#define QDEV_IDENTIFIED    (uint8_t)(1 << 0)
#define QERR_BUILT         (uint8_t)(1 << 1)
#define QMSG_BUILT         (uint8_t)(1 << 2)
#define QMSG_RECEIVED      (uint8_t)(1 << 3)
#define QMSG_SENT          (uint8_t)(1 << 4)
#define QMSG_FOR_BA        (uint8_t)(1 << 5)
#define USE_POLL_ADDR      (uint8_t)(1 << 6)
#define USE_SHORT_FRAME    (uint8_t)(1 << 7)


#endif /* _HTESTTYPES_H */


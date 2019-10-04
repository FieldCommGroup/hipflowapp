/*****************************************************************
 * Copyright (C) 2009-2018 FieldComm Group.
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
 *   sems.h
 * File Description:
 *   Header file for sems.c
 *
 **********************************************************/
#ifndef _SEMS_H
#define _SEMS_H

#include <semaphore.h>
#include "datatypes.h"
#include "htesttypes.h"
#include "toolsems.h"


/*************
 *  Typedefs
 *************/
typedef struct shrd_vars_struct
{
  htest_queue_t    queueSh;       /* queue to share info across threads */
  errVal_t         cmdStatusSh;
  uint8_t          primMasterSh;
  uint8_t          nicknameSh;
} shrd_vars_t;



/****************
 *  Definitions
 ****************/


/************
 *  Globals
 ************/
extern sem_t  *p_semQInfoSh;
extern sem_t  *p_semQFlagSh;
extern sem_t  *p_semQRcvSh;
extern sem_t  *p_semQSndSh;
extern sem_t  *p_semVarsSh;

extern sem_t  *p_semInit;
extern sem_t  *p_semRunCmd;
extern sem_t  *p_semSndFSKThr;
extern sem_t  *p_semStopMain;


/************************
 *  Function Prototypes
 ************************/
//int       getNumSems();	// should be get_sem_count()
void      clear_shQueue(void);
void      copy_flags_from_sh(uint8_t *p_flags);
void      copy_flags_to_sh(uint8_t *p_flags);
void      copy_info_from_sh(DEVDATA *p_devInfo);
void      copy_info_to_sh(DEVDATA *p_devInfo);
void      copy_rcvMsg_from_sh(htest_msg_t *p_msg);
void      copy_rcvMsg_to_sh(htest_msg_t *p_msg);
void      copy_sndMsg_from_sh(htest_msg_t *p_msg);
void      copy_sndMsg_to_sh(htest_msg_t *p_msg);
errVal_t  tp_create_semaphores(uint8_t createFlag);	// 
void      tp_delete_semaphores(void);
void      end_all(void);
uint8_t   get_shCmdStatus(void);
uint8_t   get_shNickname(void);
uint8_t   get_shPrimMaster(void);
void      save_shCmdStatus(errVal_t status);
void      save_shPollAddr(uint8_t pollAddr);
void      set_shMsgRcd_flag(void);
void      set_shNickname(uint8_t nickname);
void      set_shPrimMaster(uint8_t master);


#endif /* _SEMS_H */


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

/**********************************************************
 *
 * File Name:
 *   sems.c
 * File Description:
 *   Functions to create, delete and use semaphores, and
 *   access data protected by semaphores in HTest.
 *
 **********************************************************/
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>        
#include <unistd.h>

#include "debug.h"
//#include "hthreads.h"
#include "sems.h"
#include "safe_lib.h"

/************
 *  Globals
 ************/
/* Semaphores for process synchronization */
sem_t *p_semInit; /* sync UI & main thread */
sem_t *p_semRunCmd; /* sync lib func & main thread */
sem_t *p_semSndFSKThr; /* sync lib func & send thread */
sem_t *p_semStopMain; /* sync UI & main thread */

/* Semaphores for shared resource access */
sem_t *p_semQInfoSh; /* control access to queueSh.info */
sem_t *p_semQFlagSh; /* control access to queueSh.qFlags */
sem_t *p_semQRcvSh; /* control access to queueSh.rcvMsg */
sem_t *p_semQSndSh; /* control access to queueSh.sndMsg */
sem_t *p_semVarsSh; /* control access to misc. shared vrbls */

/************************************
 *  Private variables for this file  
 ************************************/
//static sem_info_t htestSems[MAX_SEMS]; /* array of all semaphores */
//static uint8_t numSems = 0; /* total number of sems used */

static shrd_vars_t shrdVar;

/**********************************************
 *  Private function prototypes for this file
 **********************************************/

static void clear_shQFlags(void);
static void clear_shQInfo(void);
static void clear_shQRcvMsg(void);
static void clear_shQSndMsg(void);

static sem_t *open_sem(const char *semName, uint8_t createFlag,
		uint8_t initVal);
static errVal_t remove_sem(sem_t *p_sem, const char *semName);

#if 0
int getNumSems()
{
	return numSems;
}
#endif

void clear_shQueue(void)
{
	clear_shQFlags();
	clear_shQInfo();
	clear_shQRcvMsg();
	clear_shQSndMsg();
}

void copy_flags_from_sh(uint8_t *p_flags)
{
	sem_wait(p_semQFlagSh);
	*p_flags |= shrdVar.queueSh.qFlags;
	sem_post(p_semQFlagSh);
}

void copy_flags_to_sh(uint8_t *p_flags)
{
	sem_wait(p_semQFlagSh);
	shrdVar.queueSh.qFlags = *p_flags;
	sem_post(p_semQFlagSh);
}

void copy_info_from_sh(DEVDATA *p_devInfo)
{
	sem_wait(p_semQInfoSh);
	memcpy_s(p_devInfo, sizeof(DEVDATA), &shrdVar.queueSh.info, sizeof(DEVDATA));
	sem_post(p_semQInfoSh);
}

void copy_info_to_sh(DEVDATA *p_devInfo)
{
	sem_wait(p_semQInfoSh);
	memcpy_s(&shrdVar.queueSh.info, sizeof(DEVDATA), p_devInfo, sizeof(DEVDATA));
	sem_post(p_semQInfoSh);
}

void copy_rcvMsg_from_sh(htest_msg_t *p_msg)
{
	/* The value of queue is assumed to be valid. */
	sem_wait(p_semQRcvSh);
	memcpy_s(p_msg, sizeof(htest_msg_t), &shrdVar.queueSh.rcvMsg, sizeof(htest_msg_t));
	sem_post(p_semQRcvSh);
}

void copy_rcvMsg_to_sh(htest_msg_t *p_msg)
{
	sem_wait(p_semQRcvSh);
	memcpy_s(&shrdVar.queueSh.rcvMsg, sizeof(htest_msg_t), p_msg, sizeof(htest_msg_t));
	sem_post(p_semQRcvSh);
}

void copy_sndMsg_from_sh(htest_msg_t *p_msg)
{
	/* The value of queue is assumed to be valid. */
	sem_wait(p_semQSndSh);
	memcpy_s(p_msg, sizeof(htest_msg_t), &shrdVar.queueSh.sndMsg, sizeof(htest_msg_t));
	sem_post(p_semQSndSh);
}

void copy_sndMsg_to_sh(htest_msg_t *p_msg)
{
	sem_wait(p_semQSndSh);
	memcpy_s(&shrdVar.queueSh.sndMsg, sizeof(htest_msg_t), p_msg, sizeof(htest_msg_t));
	sem_post(p_semQSndSh);
}

errVal_t tp_create_semaphores(uint8_t createFlag)
{
	errVal_t errVal = SEM_ERROR;
	uint8_t initVal;

	do
	{
		/* Proceed only if no errors */
		if (create_semaphores(createFlag))
		{
			break;
		}

		/* Create/open all synchronization semaphores */
		initVal = (createFlag ? SEMTAKEN : SEMIGN);

		p_semInit = open_a_semaphore("semInit", createFlag, initVal);
		if (p_semInit == NULL)
		{
			break;
		}

		p_semRunCmd = open_a_semaphore("semRunCmd", createFlag, initVal);
		if (p_semRunCmd == NULL)
		{
			break;
		}

		p_semSndFSKThr = open_a_semaphore("semSndFSKThr", createFlag, initVal);
		if (p_semSndFSKThr == NULL)
		{
			break;
		}

		p_semStopMain = open_a_semaphore("semStopMain", createFlag, initVal);
		if (p_semStopMain == NULL)
		{
			break;
		}

		p_semQInfoSh = open_a_semaphore("semQInfoSh", createFlag, initVal);
		if (p_semQInfoSh == NULL)
		{
			break;
		}

		p_semQFlagSh = open_a_semaphore("semQFlagSh", createFlag, initVal);
		if (p_semQFlagSh == NULL)
		{
			break;
		}

		p_semQRcvSh = open_a_semaphore("semQRcvSh", createFlag, initVal);
		if (p_semQRcvSh == NULL)
		{
			break;
		}

		p_semQSndSh = open_a_semaphore("semQSndSh", createFlag, initVal);
		if (p_semQSndSh == NULL)
		{
			break;
		}

		p_semVarsSh = open_a_semaphore("semVarsSh", createFlag, initVal);
		if (p_semVarsSh == NULL)
		{
			break;
		}
		errVal = NO_ERROR;
	} while (FALSE); /* Run the loop at most once */

	if (createFlag)
	{
		dbgp_sem("Total %d semaphores created\n", get_sem_count());
	}

	return (errVal);
}

void tp_delete_semaphores(void)
{
	delete_semaphores();

#if 0
	numSems = 0;
	/* Avoid repeated deletion via different means */
	if (count)
	{
		dbgp_sem("Deleting %d semaphores..", count);
		for (n = 0; n < count; n++)
		{
			p_sem = htestSems[n].p_sem;
			semName = htestSems[n].semName;
			if (remove_sem(p_sem, semName) == LINUX_ERROR)
			{
				fprintf(p_toolLogPtr, "Error %d deleting semaphore %s!!\n",
				errno, semName);
			}
			else
			{
				/* Reset pointer to prevent accidental misuse */
				dbgp_sem("%s..", semName);
				htestSems[n].p_sem = NULL;
			}
		} /* for */
	
		dbgp_sem("\nTotal %d semaphores deleted\n", n);
	} /* if (count) */
#endif
}


uint8_t get_shCmdStatus(void)
{
	sem_wait(p_semVarsSh);
	uint8_t status = shrdVar.cmdStatusSh;
	sem_post(p_semVarsSh);
	return (status);
}

uint8_t get_shNickname(void)
{
	sem_wait(p_semVarsSh);
	uint8_t nickname = shrdVar.nicknameSh;
	sem_post(p_semVarsSh);
	return (nickname);
}

uint8_t get_shPrimMaster(void)
{
	sem_wait(p_semVarsSh);
	uint8_t master = shrdVar.primMasterSh;
	sem_post(p_semVarsSh);
	return (master);
}

void save_shCmdStatus(errVal_t status)
{
	sem_wait(p_semVarsSh);
	shrdVar.cmdStatusSh = status;
	sem_post(p_semVarsSh);
}

void set_shMsgRcd_flag(void)
{
	sem_wait(p_semQFlagSh);
	shrdVar.queueSh.qFlags |= QMSG_RECEIVED;
	sem_post(p_semQFlagSh);
}

void set_shNickname(uint8_t nickname)
{
	sem_wait(p_semVarsSh);
	shrdVar.nicknameSh = nickname;
	sem_post(p_semVarsSh);
}

void set_shPrimMaster(uint8_t master)
{
	sem_wait(p_semVarsSh);
	shrdVar.primMasterSh = master;
	sem_post(p_semVarsSh);
}

/****************************************************
 *          Private functions for this file
 ****************************************************/

static void clear_shQFlags(void)
{
	sem_wait(p_semQFlagSh);
	memset(&shrdVar.queueSh.qFlags, 0, sizeof(uint8_t));
	sem_post(p_semQFlagSh);
}

static void clear_shQInfo(void)
{
	sem_wait(p_semQInfoSh);
	memset(&shrdVar.queueSh.info, 0, sizeof(DEVDATA));
	sem_post(p_semQInfoSh);
}

static void clear_shQRcvMsg(void)
{
	sem_wait(p_semQRcvSh);
	memset(&shrdVar.queueSh.rcvMsg, 0, sizeof(htest_msg_t));
	sem_post(p_semQRcvSh);
}

static void clear_shQSndMsg(void)
{
	sem_wait(p_semQSndSh);
	memset(&shrdVar.queueSh.sndMsg, 0, sizeof(htest_msg_t));
	sem_post(p_semQSndSh);
}

#if 0
static sem_t *open_sem(const char *semName, uint8_t createFlag, uint8_t initVal)
{
	sem_t *p_sem;

	if (createFlag)
	{
		p_sem = sem_open(semName, O_CREAT, SEMPERMS, initVal);
	}
	else
	{
		p_sem = sem_open(semName, 0);
	}

	if (p_sem == SEM_FAILED)
	{
		fprintf(p_toolLogPtr, "Error %d in sem_open(%s)\n", errno, semName);
		p_sem = NULL;
	}
	else if (createFlag)
	{
		/* Save semaphore info in the array */
		htestSems[numSems].p_sem = p_sem;
		htestSems[numSems].semName = semName;
		numSems++;
		dbgp_sem("Created semaphore #%d (%s)\n", numSems, semName);
	}

	return (p_sem);
}


static errVal_t remove_sem(sem_t *p_sem, const char *semName)
{
	errVal_t errVal = LINUX_ERROR;

	if (p_sem != NULL)
	{
		if (sem_close(p_sem) != LINUX_ERROR)
		{
			if (sem_unlink(semName) != LINUX_ERROR)
			{
				/* Reset pointer to prevent accidental misuse */
				p_sem = NULL;
				errVal = NO_ERROR;
			}
		}
	}
	return (errVal);
}
#endif


/*****************************************************************
 * Copyright (C) 2018 FieldComm Group.
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

#include "native.h"

//#include <errno.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
//
//#include "datatypes.h"
#include "debug.h" // holds dbg logs
#include "errval.h"
//#include "hthreads.h"
#include "sems.h"
//#include "tooldef.h"
//#include "timer.h"
////#include "serport.h"
#include "cDataRaw.h"
#include "hartPhy.h" 
#include <string.h>
#include "configuration_Default.h"
//#include "PV.h"

#include "hartdefs.h"
#include "appconnector.h"
#include "apppdu.h"
#include "PhysicalAlarms.h"

//extern AppConnector<AppPdu> *pAppConnector;
extern int init_ProcessVariables(void);/*  instead of all PV.h  */
//extern  uint8_t *pack(uint8_t *dest, const uint8_t *source, int nChar);// in database.cpp
extern int initBurstStack(); // in burst.cpp
extern errVal_t FillDefaultValues(); // in FillDefaultValues.cpp
/*
 * There should only be one TP Master Stack object per executable
 */
NativeData::NativeData():myAddr( {0,0,0,0,0} )
{
	//strcpy(port, "/dev/ttyS0");
	//pollAddress = -1;	// error condition
	
}

NativeData::~NativeData()
{
	// empty
}

errVal_t NativeData::commandline(int argc, char *argv[])  // -p <port>
{
	errVal_t retval = NO_ERROR;
	// native has no command line variables at this time
	return retval;
}
extern void testTime();
// read a file and/or set static data items
/* stevev --- set variables from file...or default values..burst coomand included */
errVal_t NativeData::configure()
{
	//testTime();
	struct stat sb;
	errVal_t retval = NO_ERROR;

	int ret = Open_File(CONFIG_FILENAME);

	if ( CheckFile(CONFIG_FILENAME) && ! ret)// it exists, has data, and is closed
	{
		retval = getData();
		handleDataDependencies();
	}
	else
	if( CheckFile(CONFIG_FILENAME) && ret > 0 )// it exists, is empty and is closed
	{
		retval = fileFillDefault();// generate and save default values to new file
		handleDataDependencies();
	}
	else
	{
		retval = FATAL_ERROR;// utter and complete failure
	}

	return retval;
}

void NativeData::handleDataDependencies()
{
	GenerateMyAddress();
	initBurstStack();
	//NONvolatileRaw,burstMsg_Raw_t brstMsgs[MAX_BURSTMSGS];
}

void NativeData::GenerateMyAddress()
{
	myAddr[0] = ItemValue(mfrID, uint8_t) & 0x3F; // top two bits set elsewhere
	myAddr[1] = ItemValue(devType, uint8_t);
	myAddr[2] = ((uint8_t *)devID.pRaw)[0];
	myAddr[3] = ((uint8_t *)devID.pRaw)[1];
	myAddr[4] = ((uint8_t *)devID.pRaw)[2];
}

// programatically set data, semaphores, spin threads, etc
errVal_t NativeData::initialize()
{
	errVal_t ret = NO_ERROR;
	int r = 0;
	// data has been configured, generate data acquisition 

	//if ( (r=init_ProcessVariables()) ) // had an error
	//{
	//	fprintf(stderr,"Process Variables failed to initialize!\n");
	//	ret = (errVal_t)r;
	//}

	return ret;


#if 0 // these don't work anymore...
	errVal_t retval;
	uint8_t status;
	const char *mainThrName = "Main Thread";
	do
	{
		retval = tp_create_semaphores(TRUE);
		if (retval == SEM_ERROR)
		{
			fprintf(p_toolLogPtr, "\nSemaphore Error!!!\n");
			break;
		}

		/* Create main thread to do misc. setups */
		status = pthread_create(&mainThrID, NULL, mainThrFunc,
				(void *) mainThrName);
		if (retval != NO_ERROR)
		{
			fprintf(p_toolLogPtr, "\nError Creating %s!!\n", mainThrName);
			break;
		}

		/* Wait till all necessary setup has been done by the
		 * main thread before transferring control to the master state machine
		 */
		dbgp_sem("Waiting for semInit\n");
		status = sem_wait(p_semInit);
		if (status == LINUX_ERROR)
		{
			fprintf(p_toolLogPtr, "Error getting semaphore p_semInit\n");
			break;
		}dbgp_sem("Got semInit\n");

		//initialize_htest_queues();	// not sure that this is required for TP Master

	} while (FALSE); /* Run the loop at most once */

	if ((status != NO_ERROR) || (sysError))
	{
		if (status != LICENSE_ERROR)
		{
			printf("\n\n");
			printf("System Error!!\n");
			printf("   Terminating......\n\n\n");
		}
		status = LINUX_ERROR;
//		sleep(2);
//		cleanup();
//		exit(1);
	} /* if (retVal = ERROR) */

//	dbgp_dvlp("Going to %s prompt...\n\n", TOOL_NAME);
	return (errVal_t) status;
#endif //0
}


// stop threads, delete semaphores and allocated memory
//static 
errVal_t NativeData::cleanup()
{
	errVal_t retval = NO_ERROR;
	int32_t errVal;
	
	dbgp_sem("Total %d semaphores exist\n", get_sem_count());
	if (get_sem_count())
	{
		sem_post(p_semStopMain);
	}
#if 0
	/* Wait for main thread to terminate */
	if (mainThrID != (pthread_t) NULL)
	{
		errVal = pthread_join(mainThrID, NULL);

		if (errVal == NO_ERROR)
		{
			/* Reset ID to prevent accidental misuse */
			mainThrID = (pthread_t) NULL;
		}
		else
		{
			fprintf(stderr, "Error %d in pthread_join() for Main Thread: %s\n",
					errVal, "Unknownerror"/*strerror(errVal)*/);
		}

	}


	delete_threads();
#endif
	delete_semaphores();
//serial	close_fsk_port();
// let the enclosing app do this	close_toolLog();

	//printf("\n%s Done!\n", TOOL_NAME);

	return retval;
}

void NativeData::setUniqueID(uint8_t *pLowMACaddr) //we will use first 3 bytes here
{
	NONvolatileData.devID[0] = pLowMACaddr[0];
	NONvolatileData.devID[1] = pLowMACaddr[1];
	NONvolatileData.devID[2] = pLowMACaddr[2];
}

// fill all non volatile with default values and save 'em to the open file
// close the file on exit
errVal_t NativeData::fileFillDefault()
{
	errVal_t retval = FillDefaultValues();

	putData();// write to disk

	return retval;
}
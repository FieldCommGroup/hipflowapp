/*************************************************************************************************
 *
 * Workfile: shared.h
 * 20Mar18 - paul
 *
 *************************************************************************************************
 * The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description:
 *		3/20/18	pdu	created to handle shared information from the io thread and the main thread
 *	
 * #include "shared.h"
 */
#pragma once

#ifndef _SHARED_H
#define _SHARED_H
#ifdef INC_DEBUG
#pragma message("In shared.h") 
#endif

#include <semaphore.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>

#ifdef INC_DEBUG
#pragma message("    Finished Includes::shared.h") 
#endif

// NOW DEINED ON COMMAND LINE...// #define _FLOWDEVICE
// NOW DEINED ON COMMAND LINE...// #define _PRESSUREDEVICE

#define ever   (;;)

extern const void * mapFailed;
extern void* run_io(void * pArgs);

struct sharedArgs_s
{
	volatile int32_t * padc; // points to a adc[0] that has eight entries
	volatile int32_t * upCnt;// points to the update counter, incremented on update
	volatile bool    * pDieCmd; // true to kill us

	sem_t * pSemaPhore;
#ifdef _DEBUG
	volatile int32_t * pScanCount;
#endif

};


#endif // _SHARED_H
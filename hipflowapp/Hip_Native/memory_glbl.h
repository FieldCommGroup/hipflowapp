/*************************************************************************************************
 *
 * Workfile: memory_glbl.h 
 * 28Mar18 - paul
 *
 *************************************************************************************************
* The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved 
 *************************************************************************************************
 *
 * Description: This holds the functions to cleanup before exit.
 *	
 *		
 *	
 * #include "memory_glbl.h"
 */
#pragma once

#ifndef _GLOBALMEMORY_H
#define _GLOBALMEMORY_H
#ifdef INC_DEBUG
#pragma message("In memory_glbl.h") 
#endif


#include <semaphore.h>
#include <string>
#include <vector>

using namespace std;

#include "shared.h"
#include "datatypes.h"

#ifdef INC_DEBUG
#pragma message("    Finished Includes::memory_glbl.h") 
#endif

extern sem_t            dataSemaphore;
extern volatile bool    dieThreadDie; // cmd true to kill IOthread, thread sets it false at exit
extern volatile bool    dieDataThDie; // cmd true to kill Datathread, thread sets it false at exit
extern volatile bool    dieBursthDie; // cmd true to kill the burst thread, sets it false at exit
extern sharedArgs_s     myArgs;
//extern bool             queuesRopen;  // true if we are required to close the queues
extern bool             isBIGendian;
extern volatile uint32_t  time1_32mS;

extern pthread_t IOthread;
extern pthread_t Datathread;
extern pthread_t Burstthread;

#endif //_GLOBALMEMORY_H



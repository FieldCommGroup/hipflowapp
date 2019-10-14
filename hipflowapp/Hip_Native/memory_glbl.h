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



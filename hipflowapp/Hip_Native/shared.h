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
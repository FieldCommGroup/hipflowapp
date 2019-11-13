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
 * Description: This is the burst handler for the native device.
 *	
 *		
 *	
 * #include "burst.h"
 */
#pragma once

#ifndef _BURST_H
#define _BURST_H
#ifdef INC_DEBUG
#pragma message("In burst.h") 
#endif


#include "datatypes.h"
#include "msg.h"
#include "cDataRaw.h"
#include "appconnector.h"

//#include <vector>
//using namespace std;

#ifdef INC_DEBUG
#pragma message("    Finished Includes::burst.h") 
#endif

#define HART_DELIM_LONGFRAME_BURST 0x81

extern AppConnector<AppPdu> *pAppConnector;

typedef struct stackStruct_s
{
	bool      is_enabled;
	uint32_t  msg_Number;//extern burstMessage burstMsgArray[MAX_BURSTMSGS];
	 int32_t  remaining50mSticks;// per burst...ie burst every timeCycles runs of the sleep command(50 mS)
	 int32_t  remainingMaxSticks;
	uint32_t  sentCnt;
	// don't know what this was for::> char (*serviceFunc)(MSG * pMsg);
	stackStruct_s():is_enabled(false), msg_Number(-1), remaining50mSticks(0), 
		remainingMaxSticks(0), sentCnt(0){};
}stackStruct_t;

extern stackStruct_t burstStack[MAX_BURSTMSGS];



#endif // _BURST_H
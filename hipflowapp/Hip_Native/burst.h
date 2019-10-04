/*************************************************************************************************
 *
 * Workfile: burst.h 
 * 06apr18 - paul
 *
 *************************************************************************************************
* The content of this file is the 
 *     Proprietary and Confidential property of the HART Communication Foundation
 * Copyright (c) 2018, FieldComm Group, Inc., All Rights Reserved 
 *************************************************************************************************
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
					sentCnt(0){};
}stackStruct_t;

extern stackStruct_t burstStack[MAX_BURSTMSGS];



#endif // _BURST_H
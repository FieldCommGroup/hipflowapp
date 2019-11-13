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
 * Description: This is the base class for commands
 *
 * #include "command.h"
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdio.h>
#include "cDatabase.h" // all our children need it
#include "apppdu.h"
#include "hartdefs.h"
#include "InsertExtract.h"
#include "debug.h"		/* convert printf to fprintf */

using namespace std;

// Return-codes for command functions
// in hartdefs   #define RC_SUCCESS   0		/* volatileRaw.responseCode must be zero'd                      */
#define RC_WARNING   1		/* volatileRaw.responseCode must have the proper warning RC set */
#define RC_ERROR    -1		/* volatileRaw.responseCode must have the proper  error  RC set */
#define RC_DONOTSEND 0x80     /* do not reply to this message */
#define RC_CFGMISMATCH 0x09		/* Configuration Change Counter Mismatch */

/*  response code type identification */
#define isWarning( c ) (  (c!=0)&&( (c > 95) || (c == 8) || (c == 14) || (c==30) || (c==31)|| (c>23 && c < 28) )  )
#define isError( e )   (  (e!=0)&&( ! isWarning( e ) )  )

#define aReturnCode( a ) (  (a==0)?0:  (isWarning( a ))?1:-1   )

class cmd_base
{
	int  cmdNumber;
protected:
	bool fromPrimary;// true if command originated from primary master..used by handle cmd & some children

	// generic test for a devVar index - used by totalizer commands
	// returns:RC-17/* not in this device*/||RC-19/* bad index(not toalizer) */||RC-0x - used by 
	uint8_t isDevVarIndex2Totalizer(uint8_t qDVarIndex);

public: // c.dtor
	cmd_base( int num = -1);
	virtual ~cmd_base();

public: // work
	int number() { return cmdNumber; };
	
	// read, handle, generate reply
	// return a return-code , >1  if no response required
	int  handleCmd(AppPdu *pPDU);
	int setRespCode(uint8_t RespCode);// returns a return-code (see above)
	// use macro isWarning( ResponseCode )

	virtual void setIndexes(dataItem indexList[]) {}; // only implemented in burst commands with indexes
	// extract ByteCnt bytes from data at pData
	// return responseCode on error or warning, -1 if non-recoverable error
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData) = 0;// pure virtual method

	// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
	// only called if extractData returned success or a warning response code
	// return success or responseCode on error or warning (with byte count 0 on error, bytes-inserted on warning)
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData)  = 0;// pure virtual method

	virtual uint8_t burstThisCmd(dataItem indexList[], AppPdu *pPDU);
	virtual void triggerOnChanged( dataItem indexList[MAX_CMD_9_RESP] );
	virtual bool isTriggered(burstMessage & bMsg );

	void    bumpConfigCnt()
	{		
		{
			uint16_t localCnt = ItemValue(configChangeCnt, uint16_t);
			localCnt++;
			configChangeCnt.setValue(localCnt);
			configChangedBit_Pri = true;
			configChangedBit_Sec = true;
#ifdef _DEBUG
fprintf(stderr,"        SecCCB set.\n");
#endif
		}
	}

	// use semaphore protection to get a volatile value, convert it
	// from standard to desired units and insert the result into a command response
	int convertAndInsert(deviceVar &dvar, uint8_t **ppData, uint8_t *dataCnt);
	int convertAndInsert(dataItem &val, deviceVar &unitsVar, uint8_t **ppData, uint8_t *dataCnt);
};


#endif // COMMAND_H_
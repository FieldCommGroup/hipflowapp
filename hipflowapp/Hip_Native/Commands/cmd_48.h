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
 * Description: This holds one command
 *
 * #include "cmd_48.h"
 */

#ifndef CMD_48_H_
#define CMD_48_H_

#include "command.h"
#include "hartmdll.h"

extern bool getCmd48Bytes(uint8_t &Byte0, uint8_t &ExDevSt);//  always false


class cmd_48 : public cmd_base
{
protected:
	bool mismatch;

public: // c.dtor
	cmd_48():cmd_base(48) {	};
	~cmd_48(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);
	virtual bool isTriggered(burstMessage & bMsg);// we're a burstable command

	//void update_MoreStatus(uint8_t * pData);
};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_48::extractData(uint8_t &ByteCnt, uint8_t *pData)
{// wally says to assume a HART 7 host in this project
 // real world ain't like that
	uint8_t  ret = RC_SUCCESS;
	if (ByteCnt == 0)// legacy request (transaction 1)
		return ret; // success, nothing to do here
	uint8_t  requestSize = ByteCnt;
	if (requestSize > CMD48_SIZE)
		requestSize = CMD48_SIZE;
	if (requestSize < CMD48_SIZE)
		ret = RC_TOO_FEW;// by spec

uint8_t * pcmd48data; // actually an array

	if (fromPrimary)
	{
		pcmd48data = tempData.Pricmd48TmpData;
	}
	else
	{
		pcmd48data = tempData.Seccmd48TmpData;
	}

	int y = 0;
	while ( (requestSize > 0) && (ret == RC_SUCCESS) )
	{// pData is incremented and requestSze is decremented in extract
		ret = extract(pcmd48data[y++], &pData, requestSize);
		y++;
	}

	return ret;// the response is the same for both transactions
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_48::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case

	uint8_t byte0, exDevStatus;

	getCmd48Bytes(byte0, exDevStatus);// has its own mutex lock

	*pData++ = byte0; ByteCnt++;// byte 0
	
	for (int y = 1; y < 6; y++)// byte 1 thru 5
	{
		*pData++ = (uint8_t)0; ByteCnt++;
	}
	*pData++ = exDevStatus; ByteCnt++;// byte 6 - Extentended Device Status

	*pData++ = (uint8_t)0; ByteCnt++;// byte 7 operating mode - requires 0
	*pData++ = (uint8_t)0; ByteCnt++;// byte 8 standardized Status 0 - we do not monitor so 0x00

    return ret;
};


bool cmd_48::isTriggered(burstMessage & bMsg)// this is a burstable command
{
	bool R = false;
	// is changed is the only one that makes sense and it's not supported right now... 
	// see More Status Available bit!
	return R;
}




#endif // CMD_48_H_
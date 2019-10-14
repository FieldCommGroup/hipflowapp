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
 * Description: This holds one command
 *
 * #include "cmd_103.h"
 */

#ifndef CMD_103_H_
#define CMD_103_H_

#include "command.h"

class cmd_103 : public cmd_base
{
	uint8_t burstMsgNumber;
public: // c.dtor
	cmd_103():cmd_base(103) {	};
	~cmd_103(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_103::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;

	if (ByteCnt < 9)
		return RC_TOO_FEW; // Too few data bytes

	burstMsgNumber = *(pData++); ByteCnt--;

	if (burstMsgNumber >= MAX_BURSTMSGS)// it's unsigned so zero thru max
	{
		printf( "Cmd 103 found %d to an invalid burst message number.\n", burstMsgNumber);
		return RC_MULTIPLE_9;//Invalid Burst Message
	}
	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);


	if ((ByteCnt >= 4) && (ret == RC_SUCCESS))
	{
		ret = pBM->burstCommPeriod.extractSelf( &pData, &ByteCnt, chngCnt);
	}
	if ((ByteCnt >= 4) && (ret == RC_SUCCESS))
	{
		ret = pBM->maxBurstCommPeriod.extractSelf(&pData, &ByteCnt, chngCnt);
	}

	if (ret == RC_SUCCESS) 
	{// returns 8, is changed warning or 0 success
		ret = pBM->stepTimes();// adusts times to spec's steps
	}

	if (!isError(ret) && chngCnt != 0)// something changed in this extraction
	{
		bumpConfigCnt();
	}
	return ret;
}

// 5-too few; 9 invalid burst sselector;    8 warn-times adjusted; 6-devicespec; 

// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_103::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		*(pData++) = burstMsgNumber; ByteCnt++;
		burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);

		if (  (ret = pBM->burstCommPeriod.insertSelf(&pData, &ByteCnt)) ) break;
		ret = pBM->maxBurstCommPeriod.insertSelf(&pData, &ByteCnt);
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_103_H_
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
 * #include "cmd_105.h"
 */

#ifndef CMD_105_H_
#define CMD_105_H_

#include "command.h"



class cmd_105 : public cmd_base
{
	uint8_t burstMsgNumber;
	bool    isLegacy;
public: // c.dtor
	cmd_105():cmd_base(105) {	};
	~cmd_105(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_105::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;
	isLegacy = (ByteCnt == 0);

	if (!isLegacy)// unsigned, byte count is at least one
	{
		burstMsgNumber = *pData++; ByteCnt--;
		if (burstMsgNumber >= MAX_BURSTMSGS)// it's unsigned
		{
			return RC_MULTIPLE_9;//Invalid Burst Message
		}
	}
	else
	{
		burstMsgNumber = 0;// legacy location
	}
	
	return ret;
};

// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_105::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case

	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);
	if (pBM == NULL)
		return RC_DEV_SPEC;
	pBM->burstModeCtrl.insertSelf(&pData, &ByteCnt);

	if (isLegacy)// this is the test's interpretation of the spec
	{
		insert((uint8_t)(ItemValue(pBM->command_number, uint16_t) & 0x00ff), &pData, ByteCnt);
	}
	else
	{
		*pData++ = 31; ByteCnt++;// constant on non-legacy
	}

	for (int i = 0; i < MAX_CMD_9_RESP && ret == RC_SUCCESS; i++)
	{
		ret = pBM->indexList[i].insertSelf(&pData, &ByteCnt);
	}

	if (ret == RC_SUCCESS)
	{
		*pData++ = burstMsgNumber; ByteCnt++;
		*pData++ = MAX_BURSTMSGS;  ByteCnt++;// max supported
			        ret = pBM->command_number.insertSelf(&pData, &ByteCnt);
		if (! ret) ret = pBM->burstCommPeriod.insertSelf(&pData, &ByteCnt);
		if ( !ret) ret = pBM->maxBurstCommPeriod.insertSelf(&pData, &ByteCnt);
	}
	if (ret == RC_SUCCESS)
	{
		pBM->trigLvlUnits = getBurstUnit(burstMsgNumber);

uint8_t localUnits = ItemValue(pBM->trigLvlUnits,uint8_t);
if (localUnits == HART_NOT_USED)
{
	printf("Units in 105 show not-used for command %d\n", ItemValue(burstMsgArray[burstMsgNumber].command_number, uint16_t));
}
		do // once
		{
			if (ret = pBM->TrigLvlMode. insertSelf(&pData, &ByteCnt)) break;
			if (ret = pBM->trigLvlClass.insertSelf(&pData, &ByteCnt)) break;
			if (ret = pBM->trigLvlUnits.insertSelf(&pData, &ByteCnt)) break;
			if (ret = pBM->trigLvlValue.insertSelf(&pData, &ByteCnt)) break;
		} while (false);// execute once
	}
	
	if (ret)
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}
    return ret;
};

#endif // CMD_105_H_
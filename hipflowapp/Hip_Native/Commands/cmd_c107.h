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
 *************************************************************************************************
 *
 * Description: This holds one command
 *
 * #include "cmd_107.h"
 */

#ifndef CMD_107_H_
#define CMD_107_H_

#include "command.h"


class cmd_107 : public cmd_base
{
	uint8_t burstMsgNumber;
	bool    isLegacy;
public: // c.dtor
	cmd_107():cmd_base(107) {	};
	~cmd_107(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_107::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = RC_SUCCESS, tmp = 0;
	isLegacy    = false;
	if (ByteCnt < 1)
	{
		return RC_TOO_FEW; // Too few data bytes
	}
	else
	if ( ByteCnt <= 4 )
	{
		isLegacy = true;
	}
	else
	if (ByteCnt > 4 && ByteCnt < 9)
	{
		return RC_TOO_FEW; // Too few data bytes..non legacy host has to send em all		
	}

	if (isLegacy)
	{
		burstMsgNumber = 0;
	}
	else
	{ 
		burstMsgNumber = pData[8];
	}
	if (burstMsgNumber >= MAX_BURSTMSGS)// it's unsigned
	{
		return RC_MULTIPLE_9;//Invalid Burst Message
	}

	uint8_t localArray[9];
	memset(localArray, HART_NOT_USED, sizeof(localArray));

	// slot zero CANNOT be 250 not-used
	if (*pData >= HART_NOT_USED)
	{
		return RC_INDEX_DISALLOWED;//19
	}
	int idxNumber = 0;
	while ( idxNumber < MAX_CMD_9_RESP && ByteCnt > 0 && ret == RC_SUCCESS )// byte count has been verified
	{
		tmp = *pData++; ByteCnt--;

		if (tmp != HART_NOT_USED)
		{
			if (NULL == deviceVar::devVarPtr(tmp) && tmp != 3)// QV is required to be supported here
			{
				printf("DevVar %d wasn't found.\n", tmp);
				return RC_INVALID;
			}// else insert it
			localArray[idxNumber++] = tmp;
		}
		else  // first not used ends the list
			break; // outta while
	}//wend
	// if we get here we have a good array - install it
	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);
	for (idxNumber = 0; idxNumber < MAX_CMD_9_RESP; idxNumber++)
	{
		pBM->indexList[idxNumber].setValue(localArray[idxNumber]);
	}
	
	if (RC_SUCCESS == ret)
	{
		bumpConfigCnt();
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_107::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case

	burstMessage * pBM = &(burstMsgArray[burstMsgNumber]);
	
	for (int i = 0; i < MAX_CMD_9_RESP && ret == RC_SUCCESS; i++)
	{
		ret = pBM->indexList[i].insertSelf(&pData, &ByteCnt);
	}
	*pData++ = burstMsgNumber; ByteCnt++;

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_107_H_
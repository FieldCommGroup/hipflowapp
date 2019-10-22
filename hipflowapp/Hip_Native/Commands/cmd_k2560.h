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
 * #include "cmd_2560.h"
 */

#ifndef CMD_2560_H_
#define CMD_2560_H_

#include "command.h"


class cmd_2560 : public cmd_base
{
	uint8_t activeDevVar;
public: // c.dtor
	cmd_2560():cmd_base(2560) {	};
	~cmd_2560(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_2560::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0;
	ByteCnt = 0; // just in case
	if (ByteCnt < 1)
	{
		activeDevVar = 0xff;
		ret = RC_TOO_FEW;// too few data bytes
	}
	ret = extract(activeDevVar, &pData, ByteCnt);
	if (ret)// not success
	{
		printf( "Data extraction error in cmd %d. ret = %d.\n", number(), ret);
	}
	else
	if (NULL == deviceVar::devVarPtr(activeDevVar))
	{
		ret = RC_INVALID;
	}
	else
	{
		ret = isDevVarIndex2Totalizer(activeDevVar);
	}
	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error
uint8_t cmd_2560::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	do // once
	{
		if ( ret = insert(activeDevVar, &pData, ByteCnt)  )  break;
		if ( ret = totStatus.insertSelf( &pData, &ByteCnt) ) break;
		if ( ret = totAddStatus.insertSelf(&pData, &ByteCnt))break;
	}
	while(false);// execute once

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_2560_H_
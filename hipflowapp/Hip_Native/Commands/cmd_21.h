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
 * #include "cmd_21.h"
 */

#ifndef CMD_21_H_
#define CMD_21_H_

#include "command.h"


class cmd_21 : public cmd_base
{

public: // c.dtor
	cmd_21():cmd_base(21) {	};
	~cmd_21(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_21::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	uint8_t ret = 0, chngCnt = 0;

	if ( templongTag.extractSelf( &pData, &ByteCnt, chngCnt) )
	{
		ret = RC_DONOTSEND;// no valid response code
	}

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, DO-NOT-SEND on error (there are no valid response codes except success)
uint8_t cmd_21::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	int ret = 0;
	ByteCnt = 0; // just in case
	bool isEqual = true;
	for ( int j = 0; j < 32; j++)
	{
		if ( ((uint8_t*)(templongTag.pRaw))[j] != ((uint8_t*)(longTag.pRaw))[j]  )
		{
			isEqual = false;
			break;// out of for loop 
		}// else it'll 
	}
	if (isEqual )// our address 
	{
		if (insert_Ident(ByteCnt, pData))// a zero packet
		{
			ret = RC_DONOTSEND;
		}
	}
	else// mismatch 
	{
		ret = RC_DONOTSEND;
	}

    return ret;
};

#endif // CMD_21_H_
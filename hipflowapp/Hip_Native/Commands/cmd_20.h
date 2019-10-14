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
 * Description: This holds one command, readLongTag
 *
 * #include "cmd_20.h"
 */

#ifndef CMD_20_H_
#define CMD_20_H_

#include "command.h"


class cmd_20 : public cmd_base
{
public: // c.dtor
	cmd_20():cmd_base(20) {	
	};
	~cmd_20(){};

public: // work
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_20::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	int ret = 0;

	// there are no reads in this command

	return ret;
}

// read, handle, generate reply
// return response code 
//     return RC with High Bit SET if no response is required
uint8_t cmd_20::insert_Data(uint8_t &ByteCnt, uint8_t *pData)
{
	int ret = 0;
	ByteCnt = 0; // just in case

	ret = longTag.      insertSelf( &pData, &ByteCnt );

	if ( ret )
	{
		printf( "Data insertion error in cmd %d. ret = %d.\n", number(), ret);
	}

    return ret;
};

#endif // CMD_20_H_
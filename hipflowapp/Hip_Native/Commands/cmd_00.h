/*************************************************************************************************
 * Copyright 2019-2021 FieldComm Group, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
	 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Description: This holds one command
 *
 * #include "cmd_00.h"
 */

#ifndef CMD_00_H_
#define CMD_00_H_

#include "command.h"


class cmd_00 : public cmd_base
{

public: // c.dtor
	cmd_00():cmd_base(00) {
	};
	~cmd_00(){};

public: // work	
	virtual uint8_t extractData(uint8_t &ByteCnt, uint8_t *pData);
	virtual uint8_t insert_Data(uint8_t &ByteCnt, uint8_t *pData);

};

// extract ByteCnt bytes from data at pData
// return zero on success, response code on error
uint8_t cmd_00::extractData(uint8_t &ByteCnt, uint8_t *pData)
{
	int ret = 0;

	// there are no reads in this command

	return ret;
}


// generate reply
// add bytes from data to pData, filling ByteCnt with the number added (caller adds RC/DS)
// return zero on success, response code (with byte count 0) on error

uint8_t cmd_00::insert_Data(uint8_t &ByteCnt, uint8_t *pData) 
{
	return insert_Ident(ByteCnt, pData);
}

#endif // CMD_00_H_